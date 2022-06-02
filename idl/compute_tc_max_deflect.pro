;Copyright 6/25/02 Washington University.  All Rights Reserved.
;compute_tc_max_deflect.pro  $Revision: 1.11 $

;*****************************************************
pro compute_tc_max_deflect,fi,dsp,wd,glm,help,stc,pref
;*****************************************************

if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx,'timecourse') eq !ERROR then return
filnam_4dfp = get_filename_from_index(fi,idx)

if(idx lt fi.nfiles) and (fi.whereisit[idx] eq !INTERNAL) then begin 
    stat = write_4dfp(filnam_4dfp,fi,wd,dsp,help,stc,hdr.tdim,INDEX=idx)
endif

rtn = select_files(strtrim(indgen(hdr.tdim)+1,2),TITLE='Please select frames.',/EXIT)
if rtn.files[0] eq 'EXIT' then return
frames_str = ' -frames'
for i=0,rtn.count-1 do frames_str = frames_str + ' ' + rtn.files[i]

;cmd = 'compute_tc_max_deflect -file ' + filnam_4dfp + frames_str
cmd = !BIN+'/compute_tc_max_deflect -file ' + filnam_4dfp + frames_str
spawn_cover,cmd,fi,wd,dsp,help,stc

if idx lt fi.nfiles and fi.whereisit[idx] eq !INTERNAL then begin
    scrap = filnam_4dfp
    scrap = strmid(scrap,0,strpos(scrap,'.',/REVERSE_SEARCH)) + '.ifh'
    spawn,'rm '+filnam_4dfp+' '+scrap,result
endif

print,'DONE'
end
