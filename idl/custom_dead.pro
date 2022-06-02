;Copyright 12/31/99 Washington University.  All Rights Reserved.
;custom_dead.pro  $Revision: 12.84 $

;********************
pro custom_dead,draw
;********************
 
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw
 
cstm.init = !FALSE
widget_control,wd.build_addimg,SENSITIVE=0
widget_control,wd.build_annotate,SENSITIVE=0
for i=0,!PROF_NWIN-1 do begin
    if cstm.draw[i] eq draw then begin
        cstm.id[i] = -1
        cstm.draw[i] = -1
        cstm.base[i] = -1
    end
end
 
end
