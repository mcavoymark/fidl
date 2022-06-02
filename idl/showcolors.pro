;Copyright 9/27/00 Washington University.  All Rights Reserved.
;showcolors.pro  $Revision: 12.96 $

;************************************************************************************
pro make_scale,wd,dsp,fi,win_xsz,win_ysz,xoff,yoff,ysz,horizontal,min1,max1,min2,max2
;************************************************************************************

if((dsp[fi.cw].show_color_scale eq !FALSE) or (dsp[fi.cw].width_color_scale ne win_xsz)) then begin
    if(dsp[fi.cw].show_color_scale eq !TRUE) then begin
        widget_control,wd.color_scale_base,/DESTROY
    endif
    dsp[fi.cw].show_color_scale = !TRUE
    wd.color_scale_base = widget_base(title='Colors',/COLUMN)
    wd.color_scale_widget = widget_draw(wd.color_scale_base,xsize=win_xsz, $
         ysize=win_ysz,RETAIN=2,KILL_NOTIFY=string('colscl_dead'))
    widget_control,wd.color_scale_widget,/REALIZE
endif
scl_index = 0
widget_control,GET_VALUE=scl_index,wd.color_scale_widget
wset,scl_index
dsp[fi.cw].width_color_scale = win_xsz

x1 = xoff
y1 = yoff
xmax = fix(ysz*(float(!LEN_COLORTAB1)/float(!LEN_COLORTAB)))
xmax = ysz
x2 = x1 + !COLSCL_WIDTH
y2 = y1 + xmax

; Black out entire window.
tv,replicate(0,win_xsz,win_ysz),0,0,/DEVICE

; Load primary scale.
if(horizontal eq !TRUE) then begin
    scale = bytscl(indgen(xmax) # replicate(1,!COLSCL_WIDTH),TOP=!LEN_COLORTAB1-1)
    tv,scale,x1,y1,/DEVICE

    ;plot_colorscale_labels,dsp[fi.cw].min_primary,dsp[fi.cw].max_primary,x1,y1,x2,y2,TITLE='Primary',/HORIZONTAL
    ;START51
    plot_colorscale_labels,dsp[fi.cw].min_primary,dsp[fi.cw].max_primary,x1,y1,x2,y2,'','Primary',1

endif else begin
    scale = bytscl(replicate(1,!COLSCL_WIDTH) # indgen(xmax),TOP=!LEN_COLORTAB1-1)
    tv,scale,x1,y1,/DEVICE

    ;plot_colorscale_labels,min1,max1,x1,y1,x2,y2,TITLE='Primary'
    ;START51
    plot_colorscale_labels,min1,max1,x1,y1,x2,y2,'','Primary',0

endelse

y1 = yoff
y2 = y1 + xmax
if(fi.color_scale2[fi.current_colors] ne !NO_SCALE) then begin
;   Load secondary scale.
    if(horizontal eq !TRUE) then begin
        scale = bytscl(replicate(1,!COLSCL_WIDTH) # indgen(xmax),TOP=!LEN_COLORTAB2-1) + !LEN_COLORTAB1
        tv,scale,x1+win_xsz,y1,/DEVICE

        ;if(min2 lt max2) then plot_colorscale_labels,min2,max2,x1+win_xsz,y1,x2+win_xsz,y2,TITLE='Secondary',/HORIZONTAL
        ;START51
        if min2 lt max2 then plot_colorscale_labels,min2,max2,x1+win_xsz,y1,x2+win_xsz,y2,'','Secondary',1

    endif else begin
        x1 = xoff + !COLSCL_WIN_XSZ
        x2 = x1 + !COLSCL_WIDTH
       scale = bytscl(replicate(1,!COLSCL_WIDTH) # indgen(xmax),TOP=!LEN_COLORTAB2-1) + !LEN_COLORTAB1
        tv,scale,x1,y1,/DEVICE

        ;if min2 lt max2 then plot_colorscale_labels,min2,max2,x1,y1,x2,y2,TITLE='Secondary'
        ;START51
        if min2 lt max2 then plot_colorscale_labels,min2,max2,x1,y1,x2,y2,'','Secondary',0

    endelse
endif

; Draw a box around the scale.
;;;boxx = [x1,x2,x2,x1,x1]
;;;boxy = [y1,y1,y2,y2,y1]
;;;plots,boxx,boxy,/DEVICE,COLOR=!WHITE

return
end


;*****************************************************************
pro showcolors,title,wd,dsp,fi,MINMAX=minmax,HORIZONTAL=horizontal
;*****************************************************************

if(keyword_set(MINMAX)) then begin
    min1 = minmax[0]
    max1 = minmax[1]
    min2 = minmax[2]
    max2 = minmax[3]
endif else begin
    if dsp[fi.cw].scale eq !FIXED then begin
        min1 = dsp[fi.cw].fixed_min
        max1 = dsp[fi.cw].fixed_max
    endif else begin
        min1 = dsp[fi.cw].min_primary
        max1 = dsp[fi.cw].max_primary
    endelse
    min2 = dsp[fi.cw].min_secondary
    max2 = dsp[fi.cw].max_secondary
endelse

if(fi.color_scale1[fi.current_colors] eq !OVERLAY_SCALE) then $
    win_xsz = !COLSCL_WIN_XSZ_DUAL $
else $
    win_xsz = !COLSCL_WIN_XSZ

if(keyword_set(HORIZONTAL)) then begin
    make_scale,wd,dsp,fi,!COLSCL_WIN_YSZ,win_xsz,!COLSCL_YOFF,!COLSCL_YSZ,!COLSCL_XSZ,!TRUE,min1,max1,min2,max2
endif else begin
    make_scale,wd,dsp,fi,win_xsz,!COLSCL_WIN_YSZ,!COLSCL_XOFF,!COLSCL_YOFF,!COLSCL_YSZ,!FALSE,min1,max1,min2,max2
endelse

;wset,dsp[fi.cw].image_index[fi.cw]
;Without the if, if dsp[fi.cw].image_index[fi.cw] = 0, then the window of death will fly up.
if dsp[fi.cw].image_index[fi.cw] gt 0 then wset,dsp[fi.cw].image_index[fi.cw]

return
end
