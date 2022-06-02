; $Id: rmd_pickfile.pro,v 1.1 2005/02/15 21:11:53 rdimeo Exp $
; ************************************************ ;
;
; NAME:
;  RMD_PICKFILE
;
; PURPOSE:
;  File selection dialog that provides additional
;  functionality beyond that provided by DIALOG_PICKFILE
;  and CW_FILESEL.  Note that this cannot be used for writing
;  files.  This is a modal widget.  The return value is a
;  scalar array or string array of fully path-qualified filenames.
;
;  User single-clicks on a file to select it.  User double-clicks
;  on a directory to go into it.
;
; CATEGORY:
;  DAVE, objects, widgets, file utilities
;
; CALLING SEQUENCE:
;  files = RMD_PICKFILE( GROUP_LEADER = group_leader,  $
;                        FILTER_IN = filter_in,        $
;                        FILTER_OUT = filter_out,      $
;                        MULTIPLE = multiple,          $
;                        PATH = path,                  $
;                        GET_PATH = get_path,          $
;                        TITLE = title,                $
;                        CANCELLED = cancelled         )
;
; KEYWORDS:
;  GROUP_LEADER:  group leader for the widget (optional input)
;  FILTER_IN:Set this keyword to a string value or an array of
;           strings specifying the file types to be displayed
;           in the file list. This keyword is used to reduce
;           the number of files displayed in the file list.
;           (optional input)
;  FILTER_OUT:
;           On output, if the OPEN button is pressed, the final
;           filter (possibly determined by the user) is returned.
;           (optional output)
;  MULTIPLE: Set this keyword so that the user can accept multiple
;            files. (optional input)
;  PATH:     Set this keyword to a scalar string specifying the
;            directory in which to perform the file selection.
;            (optional input)
;  GET_PATH: Set this keyword to a named variable in which the
;            path of the selection is returned. (optional output)
;  TITLE:    Name to be displayed at the top of the dialog
;            (optional input)
;  CANCELLED:Output keyword that indicates if the user pressed the
;            "CANCEL" button.
;
; AUTHOR:
;   Robert M. Dimeo, Ph.D.
;   NIST Center for Neutron Research
;   100 Bureau Drive
;   Gaithersburg, MD 20899
;   Phone: (301) 975-8135
;   E-mail: robert.dimeo@nist.gov
;   http://www.ncnr.nist.gov/staff/dimeo
;
; COMMON BLOCKS:
;  None
;
; REQUIRED PROGRAMS:
;  None
;
; REQUIREMENTS:
;  IDL 6.1 and higher
;
; EXAMPLE USAGE:
;  See the code appended to the end of this listing named TEST_PF.
;
; MODIFICATION HISTORY:
;  -Written 02/10/05 (RMD)
;  -Replaced list widget with a tree widget (02/12/05)
;  -Made some cosmetic improvements in widget layout (02/15/05)
;  -Replaced the FILTER keyword with FILTER_IN and FILTER_OUT
;   to make it clear that one is input and one is output (02/15/05)
;  -Added a checkbox to list the files in reverse alphabetical
;   order (02/15/05)
;  -Added drive selection ability using a droplist widget (02/15/05)
;
; LICENSE:
;  The software in this file is written by an employee of
;  National Institute of Standards and Technology
;  as part of the DAVE software project.
;
;  The DAVE software package is not subject to copyright
;  protection and is in the public domain. It should be
;  considered as an experimental neutron scattering data
;  reduction, visualization, and analysis system. As such,
;  the authors assume no responsibility whatsoever for its
;  use, and make no guarantees, expressed or implied,
;  about its quality, reliability, or any other
;  characteristic. The use of certain trade names or commercial
;  products does not imply any endorsement of a particular
;  product, nor does it imply that the named product is
;  necessarily the best product for the stated purpose.
;  We would appreciate acknowledgment if the DAVE software
;  is used or if the code in this file is included in another
;  product.
;
; ************************************************ ;
pro rmd_filesel_cleanup,tlb
compile_opt hidden,idl2
end
; ************************************************ ;
pro rmd_filesel::cleanup
compile_opt hidden,idl2
ptr_free,self.file_ptr
ptr_free,self.dir_contents_ptr,self.dir_ptr
end
; ************************************************ ;
pro rmd_filesel::get_property,   filter_in = filter_in,     $
                                 filter_out = filter_out,   $
                                 path = path,               $
                                 files = files,             $
                                 cancel = cancel
compile_opt hidden,idl2
if n_elements(*self.file_ptr) ne 0 then files = *self.file_ptr $
   else files = ''
cancel = self.cancel
filter_in = self.filter_in
filter_out = self.filter_out
path = self.path
end
; ************************************************ ;
function rmd_filesel::return_sel_file_names,files
compile_opt hidden,idl2
; First determine the selection(s) in the list widget.
; The names of valid files are returned in the output
; parameter FILES.

file_id = widget_info(self.tlb,find_by_uname = 'FILENAME')
widget_control,file_id,get_value = text_contents
files = strsplit(text_contents,',',/extract)
if n_elements(files) eq 0 then return,0B
return,1B
end
; ************************************************ ;
pro rmd_filesel::tree_event,event
compile_opt hidden,idl2
if event.type eq 0 then begin
   case event.clicks of
   1: begin ; one button click
         tree_id = widget_info(event.top,find_by_uname = 'FILE_TREE')
         sel = widget_info(tree_id,/tree_select)
         nsel = n_elements(sel)
         counter = 0L
         if n_elements(*self.file_ptr) eq 0 then return
         for i = 0,nsel-1 do begin
            widget_control,sel[i],get_value = value
            ok = where(*self.file_ptr eq value,count)
            if count gt 0 then begin
               if counter eq 0L then begin
                  files = value
               endif else begin
                  files = [files,value]
               endelse
               counter++
            endif
         endfor
         if n_elements(files) gt 0 then begin
            file_id = widget_info(event.top,find_by_uname = 'FILENAME')
            widget_control,file_id,set_value = strjoin(files,',')
         endif
      end
   2: begin ; two button clicks
         ; If user has clicked on a single directory then go down into it
         tree_id = widget_info(event.top,find_by_uname = 'FILE_TREE')
         sel = widget_info(tree_id,/tree_select)
         child_id = widget_info(tree_id,/child)
         if event.id eq child_id then begin
            self->up_dir,event
            return
         endif
         nsel = n_elements(sel)
         if nsel ne 1 then return
         widget_control,event.id,get_value = value
         ; Is this a directory?
         cd,current = this_dir
         if self->is_top() then begin
         ; Remove the path separator on Windows
            if strupcase(!version.os_family) eq 'WINDOWS' then begin
               dir_length = strlen(this_dir)
               this_dir = strmid(this_dir,0,strlen(this_dir)-1)
            endif
         endif
         new_path = this_dir+path_sep()+value
         if ~file_test(new_path,/directory) then return
         ; Remove the final path separator
         path_len = strlen(new_path)
         new_path = strmid(new_path,0,path_len-1)
         self.path = new_path
         cd,self.path
         path_id = widget_info(event.top,find_by_uname = 'PATH_LIST')
         widget_control,path_id,set_value = self.path
         ret = self->update_dir_contents()
         id = widget_info(self.tlb,find_by_uname = 'FILENAME')
         widget_control,id,set_value = ''
      end
   else:
   endcase
endif
end
; ************************************************ ;
pro rmd_filesel::quit,event
compile_opt hidden,idl2
widget_control,event.id,get_value = value
action = strupcase(value)
case action of
'OPEN':  $
   begin
      self.cancel = 0B
      ok = self->return_sel_file_names(files)
      if (ok and (n_elements(files) ne 0)) then begin
         n = n_elements(files)
         files_out = strarr(n)
         for i = 0,n-1 do begin
            files_out[i] = self.path+path_sep()+files[i]
         endfor
         ; Are these valid files?
         counter = 0L
         for i = 0,n-1 do begin
            if file_test(files_out[i]) then begin
               if counter eq 0L then files = files_out[i] else $
                  files = [files,files_out[i]]
               counter++
            endif
         endfor
         *self.file_ptr = files
      endif else begin
         *self.file_ptr = ''
      endelse
   end
'CANCEL':   $
   begin
      self.cancel = 1B
      *self.file_ptr = ''
      self.filter_out = self.orig_filter
      cd,self.orig_directory
   end
else:
endcase
widget_control,self.tlb,/destroy
end
; ************************************************ ;
function rmd_filesel::is_top
compile_opt hidden,idl2
case strupcase(!version.os_family) of
'WINDOWS':  result = (strmid(self.path,strlen(self.path)-2) eq ':\')
else: result = (self.path eq '/')
endcase
return,result
end
; ************************************************ ;
function rmd_filesel::update_dir_contents
compile_opt hidden,idl2
id = widget_info(self.tlb,find_by_uname = 'DIR_LIST')
dirs = file_search('*',/test_directory)
ndirs = n_elements(dirs)
if (ndirs eq 1) and (dirs[0] eq '') then ndirs = 0
if ndirs gt 0 then begin
   output = dirs + path_sep()
   dir_sort = sort(strupcase(output))
   output = output[dir_sort]
   dir_out = output
   ptr_free,self.dir_ptr
   self.dir_ptr = ptr_new(/allocate_heap)
   *self.dir_ptr = output
endif
filter_id = widget_info(self.tlb,find_by_uname = 'FILTER')
widget_control,filter_id,get_value = val
all_results = file_search(val)
; Which ones in result are directories?
n = n_elements(all_results)
count = 0L
for i = 0,n-1 do begin
   dir_math = where(dirs eq all_results[i],true)
   if ~true then begin
      if count eq 0L then $
         files = all_results[i] else $
         files = [files,all_results[i]]
      count++
   endif
endfor
ptr_free,self.file_ptr
self.file_ptr = ptr_new(/allocate_heap)
if n_elements(files) gt 0 then begin
   file_sort = sort(strupcase(files))
   if self.order eq (-1) then file_sort = reverse(file_sort)
   if ndirs gt 0 then begin
      output = [output,files[file_sort]]
   endif else begin
      output = files[file_sort]
   endelse
   *self.file_ptr = files[file_sort]
endif
if n_elements(output) gt 0 then $
   *self.dir_contents_ptr = output

; Destroy the first child of the tree widget and remake it to
; eliminate the leaves easily.
tree_id = widget_info(self.tlb,find_by_uname = 'FILE_TREE')
child = widget_info(tree_id,/child)
if widget_info(child,/valid_id) then $
   widget_control,child,/destroy
cd,current = this_dir
current_dir = widget_tree(tree_id,value = this_dir,/folder,      $
              /expanded,uvalue = {object:self,method:'tree_event'}  )

; Now build up the tree widget
ndirs = n_elements(dir_out)
for i = 0,ndirs-1 do begin
   group = widget_tree(current_dir,value = dir_out[i],/folder,       $
            /expanded,uvalue = {object:self,method:'tree_event'})
endfor
if (n_elements(*self.file_ptr) eq 1) then begin
  if (*self.file_ptr)[0] eq '' then $
  nfiles = 0 else nfiles = 1
endif else begin
   nfiles = n_elements(*self.file_ptr)
endelse
for i = 0,nfiles-1 do begin
   leaf1 = widget_tree(current_dir, value = (*self.file_ptr)[i],  $
   uvalue = {object:self,method:'tree_event'})
endfor

return,1B
end
; ************************************************ ;
pro rmd_filesel::filter_event,event
compile_opt hidden,idl2
widget_control,event.id,get_value = filter
self.filter_out = filter
ret = self->update_dir_contents()
end
; ************************************************ ;
pro rmd_filesel::up_dir,event
compile_opt hidden,idl2
if self->is_top() then begin
   ; can't go up any further!
endif else begin
   ; we can go up another level here
   cd,'..'
   cd,current = this_dir
   self.path = this_dir
   id = widget_info(event.top,find_by_uname = 'PATH_LIST')
   widget_control,id,set_value = self.path
   cd,self.path
   ret = self->update_dir_contents()
   id = widget_info(self.tlb,find_by_uname = 'FILENAME')
   widget_control,id,set_value = ''
endelse

end
; ************************************************ ;
pro rmd_filesel::do_nothing,event
compile_opt hidden,idl2

end
; ************************************************ ;
pro rmd_filesel::change_drive,event
compile_opt hidden,idl2
catch,the_error
if the_error ne 0 then begin
   catch,/cancel
   void = dialog_message(dialog_parent = event.top,!error_state.msg)
   return
endif
index = widget_info(event.id,/droplist_select)
drives = get_drive_list()
new_drive = drives[index]
cd,new_drive
self.path = new_drive
ret = self->update_dir_contents()
; Update the directory path text field
path_id = widget_info(event.top,find_by_uname = 'PATH_LIST')
widget_control,path_id,set_value = self.path
catch,/cancel
end
; ************************************************ ;
pro rmd_filesel::toggle_order,event
compile_opt hidden,idl2
setting = widget_info(event.id,/button_set)
if setting then self.order = (-1) else self.order = 1
ret = self->update_dir_contents()
end
; ************************************************ ;
pro rmd_filesel_events,event
compile_opt hidden,idl2
widget_control,event.id,get_uvalue = cmd
call_method,cmd.method,cmd.object,event
end
; ************************************************ ;
function rmd_filesel::build_widget
compile_opt hidden,idl2
device,get_screen_size = ss
if widget_info(self.group_leader,/valid_id) then modal = 1B else modal = 0B
self.tlb = widget_base(group_leader = self.group_leader, $
   /col,title = self.title,modal = modal,/tlb_frame_attr)

void = widget_label(self.tlb,value = 'Directory')
row1 = widget_base(self.tlb,/row)

case strupcase(!version.os_family) of
'WINDOWS':  $
   begin
      drive_vol = 'Drive: '
      drives = get_drive_list()
      drive_loc = (where(strupcase(strmid(self.path,0,strpos(self.path,'\')+1)) $
         eq strupcase(drives)) > 0)[0]
   end
else: drive_vol = ''
endcase
if drive_vol ne '' then begin
   drive_id = widget_droplist(row1,value = drives, $
      uvalue = {object:self,method:'change_drive'})
   widget_control,drive_id,set_droplist_select = drive_loc
endif

bitmap_filename = filepath('up1lvl.bmp', $
   subdirectory = ['resource','bitmaps'])
up_dir = widget_button(row1,value = bitmap_filename, $
   uvalue = {object:self,method:'up_dir'},/bitmap, $
   tooltip = 'Go up a directory')
xsize = 50 & ysize = 10
path_id = widget_text(row1,xsize = xsize,value = self.path, $
   uname = 'PATH_LIST',/editable,                        $
   uvalue = {object:self,method:'do_nothing'})

tree_base = widget_base(self.tlb,/col)
tree_xsize = fix(ss[0]/3.)
file_tree = widget_tree(tree_base,multiple = self.multiple, $
   uvalue = {object:self,method:'do_nothing'},              $
   xsize = tree_xsize,uname = 'FILE_TREE')
row2 = widget_base(self.tlb,/row,/align_center)
nonex_base = widget_base(row2,/nonexclusive)
void = widget_button(nonex_base,value = 'Reverse order', $
   uvalue = {object:self,method:'toggle_order'})
label1 = widget_label(row2,value = 'Filter')
filter_id = widget_text(row2,value = self.filter_in,xsize = 30, $
   uvalue = {object:self,method:'filter_event'},/editable,$
   uname = 'FILTER')
row2a = widget_base(self.tlb,/row,/align_center)
label2 = widget_label(row2a,value = 'Filename')
filename_id = widget_text(row2a,value = '',uname = 'FILENAME',     $
   uvalue = {object:self,method:'do_nothing'},xsize = 30,   $
   /editable)
row3 = widget_base(self.tlb,/row,/align_center)
void = widget_button(row3,value = 'Open',     $
   uvalue = {object:self,method:'quit'}       )
void = widget_button(row3,value = 'Cancel',   $
   uvalue = {object:self,method:'quit'}       )

widget_control,self.tlb,/realize
dir_geom = widget_info(up_dir,/geom)
tlb_geom = widget_info(self.tlb,/geom)
path_geom = widget_info(path_id,/geom)
label1_geom = widget_info(label1,/geom,units = 0)
label2_geom = widget_info(label2,/geom,units = 0)
nonex_geom = widget_info(nonex_base,/geom,units = 0)
if n_elements(drive_id) ne 0 then $
   drive_geom = widget_info(drive_id,/geom,units = 0)
widget_control,filter_id,scr_xsize = tree_xsize-label1_geom.xsize-nonex_geom.xsize
widget_control,filename_id,scr_xsize = tree_xsize-label2_geom.xsize
if n_elements(drive_id) ne 0 then $
   widget_control,path_id,scr_xsize = tree_xsize-dir_geom.xsize-drive_geom.xsize $
   else $
   widget_control,path_id,scr_xsize = tree_xsize-dir_geom.xsize
widget_control,self.tlb,set_uvalue = self
ret = self->update_dir_contents()
reg_name = 'rmd_filesel'
xmanager,reg_name,self.tlb,event_handler = 'rmd_filesel_events', $
   no_block = modal,cleanup = 'rmd_filesel_cleanup'
return,1B
end
; ************************************************ ;
function rmd_filesel::init,   group_leader = group_leader,  $
                              filter_in = filter_in,        $
                              multiple = multiple,          $
                              path = path,                  $
                              title = title
compile_opt hidden,idl2
cd,current = current_dir
self.orig_directory = current_dir
self.cancel = 0B
self.title = (n_elements(title) eq 0) ? 'Select file(s)':title
if n_elements(group_leader) eq 0 then group_leader = 0L
self.group_leader = group_leader
if n_elements(filter_in) ne 0 then self.filter_in = filter_in else $
   self.filter_in = '*'
self.filter_out = self.filter_in
self.orig_filter = self.filter_in
self.file_ptr = ptr_new(/allocate_heap)
self.dir_ptr = ptr_new(/allocate_heap)
self.tlb = 0L

if n_elements(multiple) eq 0 then self.multiple = 0B else $
   self.multiple = multiple
if n_elements(path) eq 0 then begin
   cd,current = path
endif
self.dir_contents_ptr = ptr_new(/allocate_heap)
self.path = path
cd,self.path
self.order = 1 ; alphabetical order
return,1B
end
; ************************************************ ;
pro rmd_filesel__define
compile_opt hidden,idl2
void =   {  rmd_filesel,                  $
            filename:'',                  $
            multiple:0B,                  $
            path:'',                      $
            filter_in:'',                 $
            filter_out:'',                $
            file_ptr:ptr_new(),           $
            dir_contents_ptr:ptr_new(),   $
            dir_ptr:ptr_new(),            $
            tlb:0L,                       $
            cancel:0B,                    $
            orig_directory:'',            $
            orig_filter:'',               $
            order:0,                      $
            title:'',                     $
            group_leader:0L               }

end
; ************************************************ ;
function rmd_pickfile,  group_leader = group_leader,  $
                        filter_in = filter_in,        $
                        filter_out = filter_out,      $
                        multiple = multiple,          $
                        path = path,                  $
                        get_path = get_path,          $
                        cancelled = cancelled,        $
                        title = title
compile_opt hidden,idl2
if n_elements(group_leader) eq 0 then group_leader = 0L
if n_elements(filter_in) eq 0 then filter_in = '*'
if n_elements(multiple) eq 0 then multiple = 0B
if n_elements(path) eq 0 then begin
   cd,current = path
endif
this_title = (n_elements(title) eq 0) ? 'Select file(s)' : title
obj_ref = obj_new('rmd_filesel',                $
               group_leader = group_leader,     $
               filter_in = filter_in,           $
               multiple = multiple,             $
               title = this_title,              $
               path = path                      )

ret = obj_ref->build_widget()
obj_ref->get_property,  files = files,          $
                        path = get_path,        $
                        cancel = cancelled,     $
                        filter_out = filter_out
obj_destroy,obj_ref

return,files
end
; ************************************************ ;
; ************************************************ ;
; BEGIN TEST PROGRAM NAMED TEST_PF
; ************************************************ ;
; ************************************************ ;
pro test_pf_event,event
case widget_info(event.id,/uname) of
'QUIT':  widget_control,event.top,/destroy
'FILE':  $
   begin
      files = rmd_pickfile(   group_leader = event.top,     $
                              filter_in = '*',              $
                              get_path = out_path,          $
                              cancelled = cancelled,        $
                              /multiple,                    $
                              title = 'Select some file(s)' )
      if ~cancelled then begin
         case n_elements(files) of
         0: void = dialog_message('No files selected',/info)
         1: void = dialog_message(files,/info)
         else: void = dialog_message(transpose(files),/info)
         endcase
      endif
   end
else:
endcase
end
; ************************************************ ;
pro test_pf
; This is just a simple test program to see that the
; dialog behaves as expected.
tlb = widget_base(/col,title = 'Test program',/tlb_frame_attr)
void = widget_button(tlb,value = 'Select File',uname = 'FILE')
void = widget_button(tlb,value = 'Quit',uname = 'QUIT')
widget_control,tlb,/realize
xmanager,'test_pf',tlb,/no_block
end
; ************************************************ ;