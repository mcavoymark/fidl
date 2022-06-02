;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_list_of_files.pro  $Revision: 1.18 $

;********************
pro get_path_event,ev
;********************

common path_comm,wd_path_list,wd_path,wd_ok,file_path,path_labels,nlab,wd_select_files, $
                 glm_files,nglm,filter,file_ids,nids,wd_filter,wd_file_list,wd_delete_files, $
                 selected_files_idx,otitle,g_below

case ev.id of
    wd_path_list: begin
        if(ev.index eq 1) then begin
            len = strlen(file_path)
            repeat begin
                lenm1 = len
                pos = rstrpos(file_path,'/')
                if(pos gt 0) then $
                    file_path = strmid(file_path,0,pos)
                len = strlen(file_path)
            endrep until ((lenm1-len) gt 1)
        endif else begin
            file_path = path_labels[ev.index]
        endelse
        get_path_list
        widget_control,wd_path_list,SET_VALUE=path_labels
        widget_control,wd_path,SET_VALUE=file_path
    end
    wd_path: begin
        file_path = ev.value
        get_path_list
        widget_control,wd_path_list,SET_VALUE=path_labels
        widget_control,wd_path,SET_VALUE=file_path
    end
    wd_filter: begin
        filter = ev.value
    end
    wd_select_files: begin
        ;cmd = 'find ' + file_path + ' -follow -name "*' + filter + '*" -print'
        ;cmd = 'find ' + file_path + ' -follow -name "*' + filter + '*" -print | sort'
        ;cmd = 'find ' + file_path + ' -name "' + filter + '" | sort'
        cmd = 'find ' + file_path + ' -name "' + filter + '" -follow | sort'
        widget_control,/HOURGLASS
        spawn,cmd,candidate_list
        widget_control,HOURGLASS=0
        jdstr = file_ids[nglm:nids-1]

;        Now select the files from the list.
        if(n_elements(candidate_list) gt 0) then begin


            ;list = get_ordered_list(candidate_list,jdstr,TITLE=otitle,BELOW=g_below) 
            ;if((n_elements(list) eq 1) and (list[0] eq '')) then $
            ;    nlist = 0 $
            ;else $
            ;    nlist = n_elements(list)
            ;START0
            rtn = get_ordered_list(candidate_list,jdstr,TITLE=otitle,BELOW=g_below) 
            list = rtn.labels_sorted
            nlist = rtn.count




            if(nlist gt 0) then begin
                for i=0,nlist-1 do begin
                    glm_files[nglm] =  list[i]
                    nglm = nglm + 1
                endfor
                glm_files_disp = strarr(nglm)
                for i=0,nglm-1 do glm_files_disp[i] = strcompress(string(file_ids[i],glm_files[i],FORMAT='(a,": ",a)'))
                widget_control,wd_file_list,SET_VALUE=glm_files_disp
            endif
        endif
    end
    wd_delete_files: begin
        ndel = n_elements(selected_files_idx)
        print,ndel,selected_files_idx
        mask = bytarr(nglm)
        mask[*] = 1
        mask[selected_files_idx] = 0
        nglm = nglm - ndel
        if(nglm gt 0) then begin
            tmplist = strarr(nglm)
            tmplist = glm_files[where(mask)]
            glm_files = tmplist
            glm_files_disp = strarr(nglm)
        endif else begin
            glm_files = ''
            glm_files_disp = ''
            nglm = 0
        endelse
        for i=0,nglm-1 do glm_files_disp[i] = strcompress(string(file_ids[i],glm_files[i],FORMAT='(a,": ",a)'))
        widget_control,wd_file_list,SET_VALUE=glm_files_disp
    end
    wd_file_list: begin
        selected_files_idx = widget_info(wd_file_list,/LIST_SELECT)
    end
    wd_ok: begin
        g_ok: widget_control,ev.top,/DESTROY
    end
    else: print,'Invalid event in get_path_event'
endcase

return
end

;******************
pro get_path_list
;******************
common path_comm

;path_labels = strarr(100)
;cmd = "ls -pL " + file_path + " | gawk '{if($1 ~ /\//) print substr($1,0,length($1)-1)}'"
;spawn,cmd,file_list
;path_labels[0] = file_path + '/.'
;path_labels[1] = file_path + '/..'
;nlab = 2
;for i=0,n_elements(file_list)-1 do begin
;     path_labels[nlab] = file_path + '/' + file_list[i]
;     nlab = nlab + 1
;endfor
;path_labels = path_labels[0:nlab-1]

cmd = "ls -pL " + file_path + " | gawk '{if($1 ~ /\//) print substr($1,0,length($1)-1)}'"
spawn,cmd,file_list
nfiles = n_elements(file_list)
path_labels = strarr(nfiles+2)
path_labels[0] = file_path + '/.'
path_labels[1] = file_path + '/..'
for i=0,nfiles-1 do path_labels[i+2] = file_path + '/' + file_list[i]

return
end

;*************************************************
function get_list_of_files,filter_in,path_in,title,file_ids_in,ORDERED_TITLE=ordered_title,BELOW=below
;*************************************************
common path_comm

if not keyword_set(ORDERED_TITLE) then otitle = '' else otitle = ordered_title
if not keyword_set(BELOW) then g_below=0 else g_below=1

filter = filter_in
file_path = path_in
file_ids = file_ids_in
nids = n_elements(file_ids)
glm_files = strarr(1000)
;glm_files_disp = strarr(100)
nglm = 0

get_path_list

path_base = widget_base(/COLUMN,TITLE=title,GROUP_LEADER=!FMRI_LEADER,/MODAL)
disp_base = widget_base(path_base,/COLUMN)
list_base = widget_base(disp_base,/ROW)
path_list_base = widget_base(list_base,/COLUMN)
file_list_base = widget_base(list_base,/COLUMN)
text_base = widget_base(disp_base,/COLUMN)
button_base = widget_base(disp_base,/ROW)

wd_label_path = widget_label(path_list_base,VALUE='Select Path')
wd_path_list = widget_list(path_list_base,VALUE=path_labels,SCR_YSIZE=400,SCR_XSIZE=450)

wd_label_file = widget_label(file_list_base,VALUE='Specified files:')
wd_file_list = widget_list(file_list_base,VALUE='',SCR_YSIZE=400,SCR_XSIZE=450,/MULTIPLE)

wd_path = cw_field(text_base,VALUE=file_path,/STRING,XSIZE=100,TITLE='Path:',/RETURN_EVENTS)
wd_filter = cw_field(text_base,VALUE=filter,/STRING,XSIZE=30,TITLE='Filter:',/ALL_EVENTS)

wd_select_files = widget_button(button_base,value='Get files',XSIZE=200)
wd_delete_files = widget_button(button_base,value='Remove selected files',XSIZE=200)
wd_ok = widget_button(button_base,value='Done',XSIZE=200)

widget_control,path_base,/REALIZE
;;;widget_control,wd_ok,/INPUT_FOCUS
widget_control,path_base,/SHOW
xmanager,'get_list_of_files',path_base,EVENT_HANDLER='get_path_event'

;out_files = glm_files[0:nglm-1]
if nglm eq 0 then out_files = '' else out_files = glm_files[0:nglm-1]

filter_in = filter
return,out_files
end
