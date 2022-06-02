;Copyright 12/31/99 Washington University.  All Rights Reserved.
;working_dead.pro  $Revision: 12.86 $

;********************
pro working_dead,draw
;********************
 
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg, $
       glm,dsp_image,help,pref,first_pass,xm1,ym1,vw
 
delete_wdw = -1
for i=0,!MAX_WORKING_WINDOWS-1 do begin
    if(wd.draw[i] eq draw) then $
        delete_wdw = i
endfor
if(delete_wdw lt 0) then $
    return
wd.draw[delete_wdw] = -1
wd.wkg_base[delete_wdw] = -1
if(delete_wdw eq fi.cw) then begin
    for i=0,!MAX_WORKING_WINDOWS-1 do begin
        if(wd.draw[i] gt 0) then begin
            fi.cw = i
            goto,END1
        endif
    endfor
    END1:
endif
 
return
end

