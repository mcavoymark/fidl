;Copyright 12/31/99 Washington University.  All Rights Reserved.
;edit_mask_dead.pro  $Revision: 12.86 $

;**********************
pro edit_mask_dead,draw
;**********************
 
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw
 
if widget_info(wd.draw[fi.cw],/VALID_ID) then begin
    if dsp[fi.cw].image_index[fi.cw] gt 0 then wset,dsp[fi.cw].image_index[fi.cw]
endif
 
end
