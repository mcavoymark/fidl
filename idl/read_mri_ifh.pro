;Copyright 12/31/99 Washington University.  All Rights Reserved.
;read_mri_ifh.pro  $Revision: 12.91 $
function read_mri_ifh,file_name
dummy = strmid(file_name,strlen(file_name[0])-3)
if dummy eq 'glm' then $ 
    hdrfil = file_name $
else begin
    dot = strpos(file_name,'.',/REVERSE_SEARCH)
    if dot gt 0 then $
        stem = strmid(file_name,0,dot) $
    else $
        stem = file_name
    hfil = strcompress(string(stem,".ifh"),/REMOVE_ALL)
    hdrfil = ''
    for i=0,n_elements(hfil)-1 do hdrfil = hdrfil+hfil[i]
endelse
openr,lu,hdrfil,/GET_LUN,ERROR=error
if error ne 0 then begin
    print,!ERROR_STATE.MSG


    ;stat = dialog_message(!ERROR_STATE.MSG+string(10B)+string(10B) $
    ;    +'My best guess is that you have moved the file to another location.',/ERROR)
    ;return,!ERROR
    ;START131113
    ifh = {InterFile_Header}
    scrap = get_button(['Skip this one','Go back','Exit'],BASE_TITLE='File not found',TITLE=!ERROR_STATE.MSG+string(10B) $
        +string(10B)+'My best guess is that you have moved the file to another location.')
    if scrap eq 2 then ifh.msg='EXIT' else if scrap eq 1 then ifh.msg='GOBACK' else ifh.msg='SKIP' 
    return,ifh


endif 
flag = 0
if dummy eq 'glm' then begin
    s = '         ' ;s must be nine spaces long to read INTERFILE
    readu,lu,s
    flag = strpos(s,'INTERFILE')
    if flag eq -1 then begin
        point_lun,lu,0
        readu,lu,rev
        ifh = {InterFile_Header}
        ifh.glm_rev = rev
    endif
endif
if flag eq 0 then get_mri_ifh,lu,ifh
close,lu
free_lun,lu
return,ifh
end
