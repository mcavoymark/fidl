;Copyright 9/11/14 Washington University.  All Rights Reserved.
;fidl_motion.pro  $Revision: 1.1 $
pro fidl_motion,fi,dsp,wd,glm,help,stc,pref
directory = getenv('PWD')
scrap = get_button(['Continue','Exit'],BASE_TITLE='Motion scrubbing',TITLE='This program takes a conc and vals file for each ' $
    +'subject and creates a scrub file (single column of 0s and 1s) that can be used to create a glm.'+string(10B)+'You provide ' $
    +'the threshold and number of contiguous frames.')
if scrap eq 1 then return
goback0:
scrap=get_glms(pref,FILTER='*.vals')
if scrap.msg eq 'GO_BACK' then return else if scrap.msg eq 'EXIT' then return
total_nsubjects = scrap.total_nsubjects
valsselect = scrap.valsselect
goback1:
scrap = get_str(2,['dvars motion censoring threshold','number of contiguous frames'],['15','5'],/GO_BACK)
if scrap[0] eq 'GO_BACK' then goto,goback0
scrap = trim(scrap)
valstr = ' -dvarsthresh '+scrap[0]+' -contiguousframes '+scrap[1]
goback2:
scrap = 'script'
scraplabels = 'fidl_motion.csh'
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter filenames.',/ONE_COLUMN,/GO_BACK, $
    FRONT=directory+'/',/REPLACE)
if names[0] eq 'GO_BACK' then goto,goback1 
script = fix_script_name(names[0])
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=script)
if action eq 2 then goto,goback2
openw,lu,script,/GET_LUN
top_of_script,lu
for i=0,total_nsubjects-1 do printf,lu,'nice +19 $BIN/fidl_motion -vals '+valsselect[i]+valstr


ALSO NEED CONCS

close,lu
free_lun,lu
spawn,'chmod +x '+script
if action eq 0 then begin
    if action eq 0 then scrap='>'+script+'.log' else scrap=''
    spawn,script+scrap+' &'
    stats = dialog_message(script+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
