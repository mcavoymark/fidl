;Copyright 12/31/99 Washington University.  All Rights Reserved.
;refresh_display.pro  $Revision: 12.103 $
pro refresh_display,fi,dsp,stc,wd,pref,IMAGE=image

if dsp[fi.cw].low_bandwidth eq !TRUE then return
hdr = *fi.hdr_ptr[dsp[fi.cw].file_num]
if not keyword_set(IMAGE) then begin
    pln = dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + dsp[fi.cw].plane - 1
    image_in = get_image(pln,fi,stc,FILNUM=dsp[fi.cw].file_num+1)
    if(n_elements(image_in) eq 1) then image_in = fltarr(hdr.xdim,hdr.ydim)
endif else begin
    image_in = image
    ;print,'refresh_display n_elements(image)=',n_elements(image)
endelse

sz = size(image_in)
if(sz[n_elements(sz)-2] eq 6) then begin
;   Complex data.
    image_in = abs(image_in)
endif

dsp[fi.cw].sizex = fi.zoom(dsp[fi.cw].file_num)*dsp[fi.cw].xdim
dsp[fi.cw].sizey = fi.zoom(dsp[fi.cw].file_num)*dsp[fi.cw].ydim
dsp[fi.cw].zoom = fi.zoom[dsp[fi.cw].file_num]
if(dsp[fi.cw].sizex ne dsp[fi.cw].dsizex or dsp[fi.cw].sizey ne dsp[fi.cw].dsizey or wd.draw[fi.cw] lt 0) then begin
    if(wd.draw[fi.cw] ge 0) then begin
        widget_control,wd.wkg_base[fi.cw],/DESTROY
        for i=0,!MAX_WORKING_WINDOWS-1 do begin
            if(wd.draw[i] le 0) then begin
                fi.cw = i
                goto,END1
            endif
        endfor
        END1:
    endif
    dsp[fi.cw].dsizex = dsp[fi.cw].sizex
    dsp[fi.cw].dsizey = dsp[fi.cw].sizey
    defsysv,'!DRAW_SIZEX',dsp[fi.cw].sizex
    defsysv,'!DRAW_SIZEY',dsp[fi.cw].sizey
    title = strcompress(string(fi.cw,FORMAT='("Working window ",i2)'))
    wd.wkg_base[fi.cw] = widget_base(TITLE=title,/COLUMN,GROUP_LEADER=wd.leader)
    yscrollsize = 700
    if(dsp[fi.cw].dsizey gt yscrollsize) then begin
        if(dsp[fi.cw].dsizex gt 800) then $
            wd.draw[fi.cw] = widget_draw(wd.wkg_base[fi.cw],XSIZE=dsp[fi.cw].dsizex, $
                  YSIZE=dsp[fi.cw].dsizey,RETAIN=2,X_SCROLL_SIZE=800, $
                  Y_SCROLL_SIZE=yscrollsize,/SCROLL, $
                  /BUTTON_EVENTS,/MOTION_EVENTS,KILL_NOTIFY='working_dead',COLORS=!NUM_COLORS) $
        else $
            wd.draw[fi.cw] = widget_draw(wd.wkg_base[fi.cw],XSIZE=dsp[fi.cw].dsizex, $
                  YSIZE=dsp[fi.cw].dsizey,RETAIN=2,X_SCROLL_SIZE=dsp[fi.cw].dsizex, $
                  Y_SCROLL_SIZE=yscrollsize,/SCROLL,KILL_NOTIFY='working_dead', $
                  /BUTTON_EVENTS,/MOTION_EVENTS)
    endif else begin
        wd.draw[fi.cw] = widget_draw(wd.wkg_base[fi.cw],XSIZE=dsp[fi.cw].dsizex, $
                  YSIZE=dsp[fi.cw].dsizey,RETAIN=2,KILL_NOTIFY='working_dead', $
                  /BUTTON_EVENTS,/MOTION_EVENTS,COLORS=!NUM_COLORS)
    endelse
    widget_control,wd.wkg_base[fi.cw],/REALIZE
    xmanager,'stats_exec',wd.wkg_base[fi.cw]
    i = 0L
    widget_control,GET_VALUE=i,wd.draw[fi.cw]
    dsp[fi.cw].image_index[fi.cw] = i
endif
wset,dsp[fi.cw].image_index[fi.cw]
widget_control,wd.draw[fi.cw],/SHOW
if dsp[fi.cw].type eq !OVERLAY_ARRAY then begin
    scale = 1
    display,image_in,min,max,pref,SCL=scale,FIXED_MIN=0,ZOOM=dsp[fi.cw].zoom,/ANNOTATE,ORIENT=dsp[fi.cw].orientation, $
        NCOLORS=!NUM_COLORS,INTERP=dsp[fi.cw].zoomtype
    dsp[fi.cw].min_primary = min(image_in,MAX=max)
    dsp[fi.cw].max_primary = max
endif else begin

    ;if(dsp[fi.cw].scale eq !FIXED) then begin
    ;    scale = !LEN_COLORTAB1/float(dsp[fi.cw].fixed_max - dsp[fi.cw].fixed_min)
    ;    display,image_in,min,max,pref,ZOOM=dsp[fi.cw].zoom,SCL=scale,FIXED_MIN=dsp[fi.cw].fixed_min,/ANNOTATE, $
    ;        ORIENT=dsp[fi.cw].orientation,NCOLORS=!LEN_COLORTAB1,INTERP=dsp[fi.cw].zoomtype
    ;endif else begin
    ;    display,image_in,min,max,pref,ZOOM=dsp[fi.cw].zoom,/ANNOTATE,ORIENT=dsp[fi.cw].orientation,NCOLORS=!LEN_COLORTAB1, $
    ;        INTERP=dsp[fi.cw].zoomtype
    ;endelse

    if dsp[fi.cw].scale eq !VOLUME then begin
        display,image_in,min,max,pref,ZOOM=dsp[fi.cw].zoom,/ANNOTATE,ORIENT=dsp[fi.cw].orientation,NCOLORS=!LEN_COLORTAB1, $
            INTERP=dsp[fi.cw].zoomtype,/VOLSCL,CONADJ=dsp[fi.cw].conadj
    endif else if dsp[fi.cw].scale eq !SLICE then begin
        goto,slice
    endif else if dsp[fi.cw].scale eq !FIXED then begin
        scale = !LEN_COLORTAB1/float(dsp[fi.cw].fixed_max - dsp[fi.cw].fixed_min)
        display,image_in,min,max,pref,ZOOM=dsp[fi.cw].zoom,SCL=scale,FIXED_MIN=dsp[fi.cw].fixed_min,/ANNOTATE, $
            ORIENT=dsp[fi.cw].orientation,NCOLORS=!LEN_COLORTAB1,INTERP=dsp[fi.cw].zoomtype
    endif else begin
        slice:
        display,image_in,min,max,pref,ZOOM=dsp[fi.cw].zoom,/ANNOTATE,ORIENT=dsp[fi.cw].orientation,NCOLORS=!LEN_COLORTAB1, $
            INTERP=dsp[fi.cw].zoomtype,CONADJ=dsp[fi.cw].conadj
    endelse





    dsp[fi.cw].min_primary = min(image_in,MAX=max)
    dsp[fi.cw].max_primary = max
endelse
load_colortable,fi,dsp

if(fi.secondary[dsp[fi.cw].file_num] ge 0) then begin
    img = get_image(hdr.zdim*(dsp[fi.cw].frame-1)+dsp[fi.cw].plane-1,fi,stc,FILNUM=fi.secondary[dsp[fi.cw].file_num]+1)
    dsp[fi.cw].min_secondary = min(img,MAX=max)
    dsp[fi.cw].max_secondary = max
endif else begin
    dsp[fi.cw].min_secondary = 0.
    dsp[fi.cw].max_secondary = 0.
endelse

if dsp[fi.cw].show_color_scale eq !TRUE then showcolors,'Main',wd,dsp,fi

;print,'refresh_display bottom'
end
