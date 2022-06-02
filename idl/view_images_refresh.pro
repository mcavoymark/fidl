;Copyright 10/4/02 Washington University.  All Rights Reserved.
;view_images_refresh.pro  $Revision: 1.25 $
pro view_images_refresh,fi,dsp,fnt,NO_OUTLINES=no_outlines
common view_images_comm_shared,lc_refreshed,prf,win,lc_color_bar_once,wd_hidecolors,wd_showcolors_vert, $
    wd_showcolors_horiz,current_label,current_cells,display_mode,af3dh,lcview,lc_add_label_file
if lc_refreshed eq !TRUE then return
if !D.WINDOW gt -1 then begin
    if prf.black_background eq !FALSE then begin
        img = bytarr(win.xdim_draw,win.ydim_draw,/NOZERO)
        img[*,*] = !WHITE
        tv,img
    endif else begin
        erase
    endelse
endif
if win.color_bar eq !TRUE and lc_color_bar_once eq !FALSE then begin ;without this you get multiple color bars
    case prf.color_bar_type of
        !HORIZ_COLOR_BAR: begin
            create_view_color_scale,win,prf,fnt,win.ncell,/HORIZONTAL
            widget_control,wd_hidecolors,/SENSITIVE
            widget_control,wd_showcolors_vert,SENSITIVE=0
            widget_control,wd_showcolors_horiz,SENSITIVE=0
        end
        !VERT_COLOR_BAR: begin
            create_view_color_scale,win,prf,fnt,win.ncell
            widget_control,wd_hidecolors,/SENSITIVE
            widget_control,wd_showcolors_vert,SENSITIVE=0
            widget_control,wd_showcolors_horiz,SENSITIVE=0
        end
        !NO_COLOR_BAR: win.color_bar = !FALSE
        else: print,'Invalid value of color_bar_type.'
    endcase
    lc_color_bar_once = !TRUE
endif
view_images_lct,fi.color_scale1[!NUM_FILES],fi.color_scale2[!NUM_FILES]
for itr=0,win.ntrans-1 do begin
    icell = win.translation_table[itr]
    if(win.cell[icell].valid eq !TRUE) then begin
        if(win.cell[icell].view eq !VIEW_COLOR_SCALE) then begin
            display_view,win.cell[icell],win.prf,win.ydim_draw
        endif else begin
            image_set = win.cell[icell].image_set
            display_view,win.cell[icell],win.prf,win.ydim_draw
        endelse
    endif
endfor
if ptr_valid(win.labels) then begin
    checked = *win.label[win.idxidx].checked
    index = where(checked,count)
    if count ne 0 then begin
        device,cursor_standard=2  ;added 050208
        labs = *win.labels
        labs[0].s = win.filenamelabels[win.idxidx]
        for ilab=0,count-1 do begin
            i = index[ilab]
            x = labs[i].x
            y = win.ydim_draw - labs[i].y - 1
            device,SET_FONT=labs[i].font
            name = labs[i].s
            xyouts,x,y,name,/DEVICE,COLOR=labs[i].color_index+!LEN_COLORTAB,CHARSIZE=2
        endfor
        if checked[0] eq 1 then begin
            win.prf.lcf=1
            win.prf.fx = labs[0].x
            win.prf.fy = labs[0].y 
        endif
    endif
endif
device,SET_FONT=fnt.current_font
if current_label ge 0 then outline_label,win,current_label,win.ydim_draw,!GREEN
if not keyword_set(NO_OUTLINES) then begin
    if current_cells[0] lt 0 then $
        nc = 0 $
    else $
        nc = n_elements(current_cells)
    for ic=0,nc-1 do outline_cell,win,current_cells[ic],prf.zoom,win.ydim_draw,display_mode,!GREEN
endif
if n_elements(af3dh) gt 0 then af3d_loci_plot_symbol,af3dh,win
fi.current_colors = !NUM_FILES
lc_refreshed = !TRUE
end
