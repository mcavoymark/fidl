;Copyright 12/31/99 Washington University.  All Rights Reserved.
;prof_dead.pro  $Revision: 12.85 $

;*****************
pro prof_dead,draw
;*****************
 
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw
 
pr.prof_on = !PROF_NO

for i=0,!PROF_NWIN-1 do begin
    if pr.draw[i] eq draw then begin
        pr.id[i] = -1
        pr.draw[i] = -1
        pr.base[i] = -1
    end
end
 
end
