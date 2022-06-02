;Copyright 4/23/08 Washington University.  All Rights Reserved.
;fidl_extract_fzstat.pro  $Revision: 1.4 $
pro fidl_extract_fzstat,fi,dsp,wd,stc,glm,help
glmi = get_bolds(fi,dsp,wd,glm,help,stc,FILTER='*.glm',TITLE='Please select glm.')
if glm[glmi].nF eq 1 then $ 
    cstr = ' -F 1' $
else begin

    ;rtn = select_files(*glm[glmi].F_names,TITLE='F statistics to be extracted.',/EXIT,MIN_NUM_TO_SELECT=1)
    ;START140826
    rtn = select_files(*glm[glmi].F_names2,TITLE='F statistics to be extracted.',/EXIT,MIN_NUM_TO_SELECT=1)

    if rtn.files[0] eq 'EXIT' then return
    cstr = ' -F '+strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)
endelse
csh = 'fidl_extract_fzstat.csh'
openw,lu,csh,/GET_LUN
top_of_script,lu
printf,lu,'nice +19 $BIN/fidl_extract_fzstat -glm_file '+fi.names[glmi]+cstr
close,lu
free_lun,lu
spawn,'chmod +x '+csh
spawn_cover,csh,fi,wd,dsp,help,stc
print,'DONE'
end
