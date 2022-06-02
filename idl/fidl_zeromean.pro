;Copyright 7/20/12 Washington University.  All Rights Reserved.
;fidl_zeromean.pro  $Revision: 1.2 $
pro fidl_zeromean,fi,wd,dsp,help,stc
rtn_get_glms=get_glms(GET_THIS='imgs/concs')
if rtn_get_glms.msg ne 'OK' then return
spawn,!BINEXECUTE+'/fidl_timestr',timestr
script = 'fidl_zeromean'+timestr[0]+'.csh'
openw,lu,script,/GET_LUN
top_of_script,lu
for i=0,rtn_get_glms.total_nsubjects-1 do printf,lu,'$BIN/fidl_zeromean -file '+rtn_get_glms.concselect[i]
close,lu
free_lun,lu
spawn,'chmod +x '+script,result
spawn_cover,script,fi,wd,dsp,help,stc
print,'DONE'
end
