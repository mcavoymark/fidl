;Copyright 12/31/99 Washington University.  All Rights Reserved.
;load_colortable_dead.pro  $Revision: 1.9 $

;****************************
pro load_colortable_dead,draw
;****************************
 
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

common adjust_colortable_comm,red1,green1,blue1,wd_min1,wd_max1,wd_gamma1, $
    wd_tables1,wd_exit,num_colors1,lct1,lc_use_2nd_table, $
    image_index,wd_tables2,lct2,wd_min2,wd_max2,wd_gamma2,table_num

dsp[fi.cw].adjust_color_top = 0
wd_tables1 = 0
wd_min1 = 0
wd_max1 = 0
wd_gamma1 = 0
wd_tables2 = 0
wd_min2 = 0
wd_max2 = 0
wd_gamma2 = 0
 
return
end
