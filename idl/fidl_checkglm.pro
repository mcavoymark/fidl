;Copyright 2/24/12 Washington University.  All Rights Reserved.
;fidl_checkglm.pro  $Revision: 1.2 $
pro fidl_checkglm,fi,dsp,wd,glm,help,stc,pref
directory=getenv('PWD')
goback0:
widget_control,/HOURGLASS
get_glms_struct=get_glms(pref,GET_THIS='glms',FILTER='*.glm')
if get_glms_struct.msg eq 'GO_BACK' or get_glms_struct.msg eq 'EXIT' then return
total_nsubjects = get_glms_struct.total_nsubjects
glmfiles = get_glms_struct.glmfiles
if glmfiles[0] eq '' then begin
    stat=dialog_message("No glm files found.",/ERROR)
    return
endif
scrap = ['script','out']
scraplabels = ['fidl_checkglm.csh','fidl_checkglm.txt']
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter.',/ONE_COLUMN,/GO_BACK, $
    FRONT=directory+'/',/REPLACE)
if names[0] eq 'GO_BACK' then goto,goback0
names = fix_script_name(names)
script = names[0]
out = names[1]
openw,lu,script,/GET_LUN
top_of_script,lu
for i=0,total_nsubjects-1 do printf,lu,'$BIN/fidl_checkglm -glm_file '+glmfiles[i]
close,lu
free_lun,lu
spawn,'chmod +x '+script
widget_control,/HOURGLASS
spawn,script+'>'+out
stat = dialog_message('Output has been written to '+out,/INFORMATION)
print,'DONE'
end
