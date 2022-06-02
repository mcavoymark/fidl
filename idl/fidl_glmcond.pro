;Copyright 3/25/15 Washington University.  All Rights Reserved.
;fidl_glmcond.pro  $Revision: 1.2 $
pro fidl_glmcond,fi,dsp,wd,glm,help,stc,pref
widget_control,/HOURGLASS
pwd=getenv('PWD')
widget_control,/HOURGLASS
spawn,!BINEXECUTE+'/fidl_timestr2',timestr
goback0:
widget_control,/HOURGLASS
gg=get_glms(pref,GET_THIS='glms',FILTER='*.glm',/NOTMORETHANONE)
if gg.msg eq 'GO_BACK' or gg.msg eq 'EXIT' then return
total_nsubjects = gg.total_nsubjects
if gg.msg ne 'OK' then return 
if gg.glmfiles[0] eq '' then begin
    stat=dialog_message("No glm files found.",/ERROR)
    return
endif
scrap = ['script','out']
scraplabels = ['fidl_glmcond_'+timestr[0]+'.csh','fidl_glmcond_'+timestr[0]+'.txt']
goback1:
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter.',/ONE_COLUMN,/GO_BACK,FRONT=pwd+'/',/REPLACE)
if names[0] eq 'GO_BACK' then goto,goback0
names = fix_script_name(names)
csh = names[0]
out = names[1]
openw,lu,csh,/GET_LUN
top_of_script,lu
for i=0,total_nsubjects-1 do printf,lu,'$BIN/fidl_glmcond -glm '+gg.glmfiles[i]
close,lu
free_lun,lu
spawn,'chmod +x '+csh
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 2 then goto,goback1
if action eq 0 then begin
    spawn,csh+'>'+out+' &'
    stat = dialog_message(csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
