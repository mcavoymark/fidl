;Copyright 12/1/00 Washington University.  All Rights Reserved.
;add_custom_img.pro  $Revision: 12.89 $

;*****************
pro add_custom_img
;*****************

common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

if(cstm.init ne !TRUE) then begin
    stat=widget_message('*** Custom display not initialized.  ***',/ERROR)
    return
endif
if(cstm.crop(1) gt 0) then begin
    xdim = cstm.crop(1) - cstm.crop(0) + 1
    ydim = cstm.crop(3) - cstm.crop(2) + 1
    nxdim = cstm.crop(1) - cstm.crop(0) + 1
    nydim = cstm.crop(3) - cstm.crop(2) + 1
endif else begin
    xdim = n_elements(dsp_image(*,0,0))
    ydim = n_elements(dsp_image(0,*,0))
    nxdim = dsp[fi.cw].xdim
    nydim = dsp[fi.cw].ydim
endelse

if  (xdim gt n_elements(bltimg(*,0,0))) or $
    (ydim gt n_elements(bltimg(0,*,0))) then begin
    stat=widget_message(string('Invalid image dimensions.'),/ERROR)
    error = !TRUE
    return
endif

bxdim = n_elements(bltimg(*,0,cstm.nblt))
bydim = n_elements(bltimg(0,*,cstm.nblt))
if(bxdim lt nxdim) then begin
    stat=widget_message(string('*** Custom images must have same dimensions. ***'),/ERROR)
    error = !TRUE
    return
endif
if(bydim lt nydim) then begin
    stat=widget_message(string('*** Custom images must have same dimensions. ***'),/ERROR)
    error = !TRUE
    return
endif
xpad = (bxdim - nxdim)/2
ypad = (bydim - nydim)/2
if(dsp[fi.cw].orientation eq !NEURO) then begin
    img = reverse(dsp_image,1)
endif else begin
    img = dsp_image
endelse
if(cstm.crop(1) gt 0) then begin
    bltimg(xpad:bxdim-xpad-1,ypad:bydim-ypad-1,cstm.nblt) = $
	img(cstm.crop(0):cstm.crop(1),cstm.crop(2):cstm.crop(3))
endif else begin
    bltimg(xpad:bxdim-xpad-1,ypad:bydim-ypad-1,cstm.nblt) = img(*,*)
endelse
cstm.nblt = cstm.nblt + 1
wset,cstm.id(cstm.cur_window)
case dsp[fi.cw].scale of
    !GLOBAL: display_glob,bltimg,min,max,pref,ZOOM=dsp[fi.cw].zoom,NPANES=cstm.nblt, $
		NCOLORS=!LEN_COLORTAB1,ORIENT=!RADIOL
    !LOCAL: display,bltimg,min,max,pref,ZOOM=dsp[fi.cw].zoom,NPANES=cstm.nblt, $
		NCOLORS=!LEN_COLORTAB1,ORIENT= !RADIOL
    !FIXED: begin
        scale = !LEN_COLORTAB1/float(dsp[fi.cw].fixed_max - dsp[fi.cw].fixed_min)
	    display,bltimg,min,max,pref,ZOOM=dsp[fi.cw].zoom,SCL=scale,FIXED_MIN= $
		dsp[fi.cw].fixed_min,NPANES=cstm.nblt,NCOLORS=!LEN_COLORTAB1, $
		ORIENT= !RADIOL
        min = dsp[fi.cw].fixed_min
        max = dsp[fi.cw].fixed_max
    end
    else: stat=widget_message('Illegal display mode.',/ERROR)
endcase
if(dsp[fi.cw].show_color_scale eq !TRUE) then $
    showcolors,'Custom',wd,dsp,fi
cstm.minmax(0) = min
cstm.minmax(1) = max
widget_control,wd.build_addimg,/SENSITIVE
widget_control,wd.build_annotate,/SENSITIVE
wset,dsp[fi.cw].image_index[fi.cw]

return
end
