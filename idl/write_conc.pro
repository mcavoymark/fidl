;Copyright 12/31/99 Washington University.  All Rights Reserved.
;write_conc.pro  $Revision: 1.8 $

;pro write_conc,stc,fi,filename,NUM_FILES=num_files,BOLDNAME=boldname
;if not keyword_set(NUM_FILES) then nfiles = stc[fi.n].n else nfiles = num_files
;openw,lu,filename,ERROR=err,/GET_LUN
;if err ne 0 then begin
;     stat = widget_message(string('Could not open file for writing: ',filename, $
;         +' saving data to tmp.conc in your home directory (it is a small file).'))
;     openw,lu,getenv('HOME')+'/'+'tmp.conc'
;endif
;filnam = *stc[fi.n].filnam
;for i=0,nfiles-1 do begin
;    if not keyword_set(NUM_FILES) then begin
;        xxx = strpos(filnam[i],'/',/REVERSE_SEARCH)
;        if xxx lt 0 then begin
;            path = fi.paths[fi.n]
;            if path eq '.' then path = directory
;            file_name = path + '/' + filnam[i]
;        endif else begin
;            file_name = filnam[i]
;        endelse
;    endif else if keyword_set(BOLDNAME) then begin
;        file_name = boldname
;    endif else begin
;        stat=dialog_message('Need to use the keyword BOLDNAME',/ERROR)
;        return
;    endelse
;    printf,lu,file_name
;endfor
;START150127
pro write_conc,stc,fi,filename
filnam = *stc[fi.n].filnam
xxx = strpos(filnam,'/',/REVERSE_SEARCH)
idx = where(xxx lt 0,cnt)
if cnt ne 0 then begin
    path = fi.paths[fi.n]
    if path eq '.' then path = getenv('PWD') 
    filnam[idx] = path + '/' + filnam[idx]
endif
openw,lu,filename,ERROR=err,/GET_LUN
if err ne 0 then begin
     stat = widget_message(string('Could not open file for writing: ',filename, $
         +' saving data to tmp.conc in your home directory (it is a small file).'))
     openw,lu,getenv('HOME')+'/'+'tmp.conc'
endif
for i=0,stc[fi.n].n-1 do printf,lu,filnam[i]
close,lu
free_lun,lu
print,'Concatenated set written to ',filename
end
