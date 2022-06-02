;Copyright 3/13/12 Washington University.  All Rights Reserved.
;fidl_eog.pro  $Revision: 1.3 $
pro fidl_eog,fi,dsp,wd,glm,help,stc,pref
directory = getenv('PWD')
goback0:
scrap=get_glms(pref,/GET_CONC)
if scrap.msg eq 'GO_BACK' then return else if scrap.msg eq 'EXIT' then return
total_nsubjects = scrap.total_nsubjects
concselect = scrap.concselect
goback1:
get_dialog_pickfile,'*.txt',fi.path,'Please load HRF file.',filehrf,nfilehrf,filehrf_path
if filehrf[0] eq 'GOBACK' then goto,goback0 else if filehrf[0] eq 'EXIT' then return
filehrfstr = ' -convolve '+filehrf

;goback2:
;scrap = get_str(2,['EOG sampling rate in Hz','TR in sec'],['500','3.013'],/GO_BACK)
;if scrap[0] eq 'GO_BACK' then goto,goback1
;scrap = strtrim(scrap,2)
;valstr = ' -samplingrateHz '+scrap[0]+' -boldTR '+scrap[1]
;START120327
goback2:
scrap = get_str(4,['EOG sampling rate in Hz','BOLD TR in sec','threshold for large eye movement uV', $
    'tolerance for return to baseline uV'],['500','3.013','175','20'],/GO_BACK)
if scrap[0] eq 'GO_BACK' then goto,goback1
scrap = strtrim(scrap,2)
valstr = ' -samplingrateHz '+scrap[0]+' -boldTR '+scrap[1]+' -threshuV '+scrap[2]+' -tol '+scrap[3]

goback3:
scrap = 'script'
scraplabels = 'fidl_eog.csh'
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter filenames.',/ONE_COLUMN,/GO_BACK, $
    FRONT=directory+'/',/REPLACE)
if names[0] eq 'GO_BACK' then goto,goback2 
script = fix_script_name(names[0])
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=script)
if action eq 2 then goto,goback3
openw,lu,script,/GET_LUN
top_of_script,lu
for i=0,total_nsubjects-1 do printf,lu,'nice +19 $BIN/fidl_eog -files '+concselect[i]+filehrfstr+valstr
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
