;Copyright 1/12/11 Washington University.  All Rights Reserved.
;fidl_glm_metrics.pro  $Revision: 1.3 $
pro fidl_glm_metrics,glm,fi,dsp,wd,help,stc,pref
directory = getenv('PWD')
goback0:
rtn_get_glms=get_glms()
if rtn_get_glms.msg eq 'GO_BACK' or rtn_get_glms.msg eq 'EXIT' then return
nglmfiles = rtn_get_glms.total_nsubjects
glmfiles = rtn_get_glms.glmfiles
txtselect = rtn_get_glms.txtselect
glmlist = rtn_get_glms.glm_list
if txtselect[0] eq '' then begin
    scrap = get_button(['goback','exit'],TITLE='Need txt files that designate valid (1) and invalid (0) frames.')
    if scrap eq 0 then goto,goback0 else return
endif

;scrap = ['script']
;if glmlist ne '' then begin
;    rtn_get_root = get_root(glmlist,'.',/KEEPPATH)
;    root = rtn_get_root.file
;    scraplabels = [root+'_glm_metrics.csh']
;endif else begin 
;    scraplabels = ['fidl_glm_metrics.csh']
;endelse
;START110118
scrap = ['script']
if glmlist ne '' then begin
    rtn_get_root = get_root(glmlist,'.',/KEEPPATH)
    root = rtn_get_root.file
    scraplabels = [root+'_metrics.csh']
endif else begin
    scraplabels = ['fidl_metrics.csh']
endelse
if nglmfiles eq 1 then scrap = [scrap,'output'] else scrap = [scrap,'output '+strtrim(indgen(nglmfiles)+1,2)]
rtn_get_root = get_root(glmfiles,'.',/KEEPPATH)
scraplabels = [scraplabels,rtn_get_root.file+'_metrics.txt']

goback1:
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter filenames.',/ONE_COLUMN,/GO_BACK, $
    FRONT=directory+'/',/REPLACE)
if names[0] eq 'GO_BACK' then goto,goback0 
names = fix_script_name(names)
script = names[0]
output = names[1:nglmfiles]
openw,lu,script,/GET_LUN
top_of_script,lu
for i=0,nglmfiles-1 do printf,lu,'nice +19 $BIN/fidl_glm_metrics -glm '+glmfiles[i]+' -txt '+txtselect[i]+' -out '+output[i]
close,lu
free_lun,lu
action = get_button(['execute','return','go back'],TITLE='Please select.',BASE_TITLE=script)
if action eq 2 then goto,goback1
spawn,'chmod +x '+script
if action eq 0 then begin
    spawn,script+'>'+script+'.log &'
    stat = dialog_message(script+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
