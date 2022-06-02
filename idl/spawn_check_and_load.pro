;Copyright 12/31/99 Washington University.  All Rights Reserved.
;spawn_check_and_load.pro  $Revision: 1.25 $
function spawn_check_and_load,result,fi,wd,dsp,help,nfiles_loaded,files_loaded,stc
;print,'spawn_check_and_load top'
nfiles_loaded=0
if result[0] ne '' then begin

    path = getenv('PWD')
    nfiles = n_elements(result)
    check = intarr(nfiles)
    strpos_segfault = strpos(result,'Segmentation')
    strpos_error = strpos(result,'Error:')
    strpos_message = strpos(result,'Message')
    strpos_information = strpos(result,'Information')
    strpos_version = strpos(result,'Version')

    ;print,'here191 result=',result

    for i=0,nfiles-1 do begin
        print,result[i]
        if strpos_segfault[i] ge 0 then begin
            stat=dialog_message(result[i],/ERROR)
            return,rtn={msg:'ERROR'}
        endif else if strpos_error[i] ge 0 then begin
            scrap = strmid(result[i],7)+string(10B)+string(10B)
            if strpos(result[i],'Error: Could not open') ge 0 then begin
                if strmid(result[i],strlen(result[i])-9) eq '.4dfp.img' then $
                    stat=dialog_message(scrap+'4dfp stack needs to be written out to disk before calling binary.',/ERROR) $
                else $
                    stat=dialog_message(scrap+'Please check your disk. The file must reside in another directory.'+string(10B) $
                        +'Perhaps you have deleted it.',/ERROR)
                return,rtn={msg:'ERROR'} 
            endif else begin 
                return,rtn={msg:'ERROR',str:scrap}
            endelse
        endif else if strpos_message[i] ge 0 or strpos_information[i] ge 0 or strpos_version[i] ge 0 then $
            check[i] = 1
    endfor

    ;print,'here197'

    index = where(check eq 0,nfiles_loaded)
    if nfiles_loaded gt !NUM_FILES then begin
        stat = dialog_message("Too many files to load.")
        return,rtn={msg:'ERROR'}
    endif
    if nfiles_loaded ne 0 then files_loaded = strarr(nfiles_loaded)
    msg = ''
    j = 0


    ;print,'here200'

    for i=0,nfiles-1 do begin
        if check[i] eq 0 then begin
            strings = strsplit(result[i],/EXTRACT)
            files_loaded[j] = strings[n_elements(strings)-1]
            if strpos(files_loaded[j],!SLASH) lt 0 then files_loaded[j] = path + !SLASH + files_loaded[j] 

            ;print,'here201 j=',j,' files_loaded[j]=',files_loaded[j]

            tmp = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,FILENAME=files_loaded[j],MODEL=model)
            j = j + 1
        endif else if check[i] eq 1 and strpos_version[i] lt 0 then begin 
            msg = msg + result[i] + string(10B)
        endif
    endfor
    if msg ne '' then stat = dialog_message(msg,/INFORMATION)
endif
;print,'here spawn_check_and_load bottom'
return,rtn={msg:'OK'}
end
