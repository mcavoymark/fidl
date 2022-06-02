;Copyright 6/26/01 Washington University.  All Rights Reserved.
;control_pln_and_frm_sliders.pro  $Revision: 1.4 $

;********************************************
pro control_pln_and_frm_sliders,fi,dsp,wd,stc
;********************************************
;print,'fi.cw=',fi.cw
;print,'control_pln_and_frm_sliders top plane=',dsp[fi.cw].plane,'  zdim=',dsp[fi.cw].zdim,'  frame=',dsp[fi.cw].frame, $
;    '  tdim=',dsp[fi.cw].tdim

if dsp[fi.cw].plane ge dsp[fi.cw].zdim then dsp[fi.cw].plane = dsp[fi.cw].zdim
if dsp[fi.cw].plane le 0 then dsp[fi.cw].plane = 1
widget_control,wd.sld_pln,SET_SLIDER_MIN=1,SET_SLIDER_MAX=dsp[fi.cw].zdim
if dsp[fi.cw].zdim eq 1 then $
    widget_control,wd.sld_pln,SENSITIVE=0 $
else $
    widget_control,wd.sld_pln,/SENSITIVE
widget_control,wd.sld_pln,SET_VALUE=dsp[fi.cw].plane

if dsp[fi.cw].frame ge dsp[fi.cw].tdim then dsp[fi.cw].frame = dsp[fi.cw].tdim
if dsp[fi.cw].frame le 0 then dsp[fi.cw].frame = 1
widget_control,wd.sld_frm,SET_SLIDER_MIN=1,SET_SLIDER_MAX=dsp[fi.cw].tdim
if dsp[fi.cw].tdim eq 1 then $
    widget_control,wd.sld_frm,SENSITIVE=0 $
else $
    widget_control,wd.sld_frm,/SENSITIVE
widget_control,wd.sld_frm,SET_VALUE=dsp[fi.cw].frame

;print,'control_pln_and_frm_sliders bottom plane=',dsp[fi.cw].plane,'  zdim=',dsp[fi.cw].zdim,'  frame=',dsp[fi.cw].frame, $
;    '  tdim=',dsp[fi.cw].tdim
end
