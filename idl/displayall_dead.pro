;Copyright 12/31/99 Washington University.  All Rights Reserved.
;displayall_dead.pro  $Revision: 12.84 $

;***********************
pro displayall_dead,draw
;***********************
 
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

dsp[fi.cw].cur_window = -1
for i=0,!PROF_NWIN-1 do begin
    if dsp[fi.cw].draw[i] eq draw then begin
        dsp[fi.cw].id[i] = -1
        dsp[fi.cw].draw[i] = -1
        dsp[fi.cw].base[i] = -1
    end
end

end
