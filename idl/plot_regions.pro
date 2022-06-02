;Copyright 4/22/03 Washington University.  All Rights Reserved.
;plot_regions.pro  $Revision: 1.9 $
pro plot_regions_event,ev
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw
common view_images_comm_shared,lc_refreshed,prf,win,lc_color_bar_once,wd_hidecolors,wd_showcolors_vert, $
    wd_showcolors_horiz,current_label,current_cells,display_mode,af3dh

;common plot_regions_comm,wd_filename,wd_plot,wd_exit,topbase,ncolors,wd_msymbol,checked,color,color_index,wd_labels,colorbarlabels, $
;    pixmap_file
;START130219
common plot_regions_comm,wd_filename,wd_plot,wd_exit,topbase,ncolors,wd_msymbol,checked,color,color_index,wd_labels,colorbarlabels, $
    pixmap_file,val

index = where(wd_filename eq ev.id,count)
if count gt 0 then begin
    checked[index] = abs(checked[index]-1)
    lc_refreshed = !FALSE
endif
index = where(wd_msymbol eq ev.id,count)
if count gt 0 then begin
    color0 = color[index]
    color_index0 = color_index[index]
    color0 = color0[0]
    color_index0 = color_index0[0]
    plot_regions_color,topbase,ncolors,af3dh,color0,color_index0
    color[index] = color0
    color_index[index] = color_index0
    scrap = !PIXMAP_PATH+'/square_'+color0+'_filled.m.pm'
    pixmap_file[index] = scrap 
    lc_refreshed = !FALSE

    ;widget_control,ev.id,SET_VALUE=scrap,/BITMAP,SET_BUTTON=0
    ;START131202
    widget_control,ev.id,SET_VALUE=scrap[0],/BITMAP,SET_BUTTON=0

endif
index = where(wd_labels eq ev.id,count)
if count ne 0 then begin
    dummy = wd_labels[index]
    widget_control,dummy[0],GET_VALUE=scrap
    colorbarlabels[index] = scrap
endif
if ev.id eq wd_plot then begin
    if total(checked) eq 0 then $
        stat=dialog_message('Please select at least one region.',/ERROR) $
    else begin    
        set_winreg

        ;START130214
        fi.color_scale1[!NUM_FILES] = !OVERLAY_SCALE
        fi.color_scale2[!NUM_FILES] = !LINECOLORS

        view_images_refresh,fi,dsp,vw.fnt
    endelse 
endif
if ev.id eq wd_exit then begin
    set_winreg
    widget_control,ev.top,/DESTROY
endif
end
;*************
pro set_winreg
;*************
common view_images_comm_shared
common plot_regions_comm

;print,'checked=',checked
;print,'color=',color
;print,'color_index=',color_index
;print,'colorbarlabels=',colorbarlabels
;print,'pixmap_file=',pixmap_file

if ptr_valid(win.reg[win.idxidx].checked) then ptr_free,win.reg[win.idxidx].checked
win.reg[win.idxidx].checked = ptr_new(checked)
if ptr_valid(win.reg[win.idxidx].color) then ptr_free,win.reg[win.idxidx].color
win.reg[win.idxidx].color = ptr_new(color)
if ptr_valid(win.reg[win.idxidx].color_index) then ptr_free,win.reg[win.idxidx].color_index
win.reg[win.idxidx].color_index = ptr_new(color_index)
if ptr_valid(win.reg[win.idxidx].colorbarlabels) then ptr_free,win.reg[win.idxidx].colorbarlabels
win.reg[win.idxidx].colorbarlabels = ptr_new(colorbarlabels)
if ptr_valid(win.reg[win.idxidx].pixmap_file) then ptr_free,win.reg[win.idxidx].pixmap_file
win.reg[win.idxidx].pixmap_file = ptr_new(pixmap_file)

;START130219
if ptr_valid(win.reg[win.idxidx].val) then ptr_free,win.reg[win.idxidx].val
win.reg[win.idxidx].val = ptr_new(val)

end
;**********************************************************************************************
pro create_widgets_pr,nreg,region_names,X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size
;**********************************************************************************************
common stats_comm
common view_images_comm_shared
common plot_regions_comm
scr_ysize = 40
if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then begin
    topbase = widget_base(/COLUMN,TITLE='Plot Regions',X_SCROLL_SIZE=(1024<x_scroll_size),XSIZE=x_scroll_size, $
        Y_SCROLL_SIZE=(800<y_scroll_size),YSIZE=y_scroll_size)
endif else $
    topbase = widget_base(/COLUMN,TITLE='Plot Regions')
msg = widget_label(topbase,/ALIGN_RIGHT,VALUE='Color scale label      ') 
prefbase = widget_base(topbase,/ROW)
colbase_symbol = widget_base(prefbase,/COLUMN,/EXCLUSIVE)
colbase = widget_base(prefbase,/COLUMN,/NONEXCLUSIVE)
label_base = widget_base(prefbase,/COLUMN)
rowbase = widget_base(topbase,/ROW)
wd_filename = lonarr(nreg)
wd_msymbol = lonarr(nreg)
wd_labels = lonarr(nreg)
for i=0,nreg-1 do begin
    wd_filename[i] = widget_button(colbase,VALUE=region_names[i],SCR_YSIZE=scr_ysize)
    wd_msymbol[i] = widget_button(colbase_symbol,VALUE=pixmap_file[i],/BITMAP,SCR_YSIZE=scr_ysize,/NO_RELEASE)
    wd_labels[i] = widget_text(label_base,VALUE=region_names[i],SCR_XSIZE=200,SCR_YSIZE=scr_ysize,/EDITABLE,/ALL_EVENTS)
endfor
wd_plot = widget_button(rowbase,value='Plot')
wd_exit = widget_button(rowbase,value='Exit')
end
;****************************

;pro plot_regions,region_names
;START130219
pro plot_regions,region_names,voxval

;****************************
common stats_comm
common view_images_comm_shared
common plot_regions_comm
nreg = win.nreg[win.idxidx]
ncolors = n_elements(af3dh.color)




;if ptr_valid(win.reg[win.idxidx].pixmap_file) then $
;    pixmap_file = *win.reg[win.idxidx].pixmap_file $
;else begin
;    pixmap_file = strarr(nreg)
;    j = 0
;    for i=0,nreg-1 do begin
;        pixmap_file[i] = !PIXMAP_PATH+'/'+'square_'+af3dh.color[j]+'_filled.m.pm'
;        j = j + 1
;        if j eq ncolors then j = 0
;    endfor
;endelse
;if ptr_valid(win.reg[win.idxidx].checked) then checked = *win.reg[win.idxidx].checked else checked = intarr(nreg) 
;if ptr_valid(win.reg[win.idxidx].color) then $
;    color = *win.reg[win.idxidx].color $
;else begin
;    color = strarr(nreg)
;    for i=0,nreg-1 do begin
;        scrap = strsplit(pixmap_file[i],'_',/EXTRACT)
;        color[i] = scrap[1]
;    endfor
;endelse
;if ptr_valid(win.reg[win.idxidx].color_index) then $
;    color_index = *win.reg[win.idxidx].color_index $
;else begin
;    color_index = intarr(nreg)
;    for i=0,nreg-1 do begin
;        index = where(color[i] eq af3dh.color)
;        color_index[i] = index[0]
;    endfor
;endelse
;if ptr_valid(win.reg[win.idxidx].colorbarlabels) then $ 
;    colorbarlabels = *win.reg[win.idxidx].colorbarlabels $
;else $
;    colorbarlabels = region_names 
;START130219
if ptr_valid(win.reg[win.idxidx].val) then val=*win.reg[win.idxidx].val else val=voxval 
if ptr_valid(win.reg[win.idxidx].checked) then checked = *win.reg[win.idxidx].checked else checked = intarr(nreg)
if ptr_valid(win.reg[win.idxidx].colorbarlabels) then $
    colorbarlabels = *win.reg[win.idxidx].colorbarlabels $
else $
    colorbarlabels = region_names
if ptr_valid(win.reg[win.idxidx].color_index) then color_index = *win.reg[win.idxidx].color_index
if ptr_valid(win.reg[win.idxidx].color) then $
    color = *win.reg[win.idxidx].color $
else if n_elements(color_index) ne 0 then $
    color = af3dh.color[color_index]
if ptr_valid(win.reg[win.idxidx].pixmap_file) then $
    pixmap_file = *win.reg[win.idxidx].pixmap_file $
else if n_elements(color) ne 0 then $ 
    pixmap_file = !PIXMAP_PATH+'/'+'square_'+color+'_filled.m.pm' $
else begin
    pixmap_file = strarr(nreg)
    j = 0
    for i=0,nreg-1 do begin
        pixmap_file[i] = !PIXMAP_PATH+'/'+'square_'+af3dh.color[j]+'_filled.m.pm'
        j = j + 1
        if j eq ncolors then j = 0
    endfor
endelse
if n_elements(color) eq 0 then begin
    color = strarr(nreg)
    for i=0,nreg-1 do begin
        scrap = strsplit(pixmap_file[i],'_',/EXTRACT)
        color[i] = scrap[1]
    endfor
endif
if n_elements(color_index) eq 0 then begin
    color_index = intarr(nreg)
    for i=0,nreg-1 do begin
        index = where(color[i] eq af3dh.color)
        color_index[i] = index[0]
    endfor
endif




create_widgets_pr,nreg,region_names
geo = widget_info(topbase,/GEOMETRY)
xdim = geo.scr_xsize + 2*geo.margin
ydim = geo.scr_ysize + 2*geo.margin
if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
    widget_control,topbase,/DESTROY
    create_widgets_pr,nreg,region_names,X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim
endif
widget_control,topbase,/REALIZE
for i=0,nreg-1 do widget_control,wd_filename[i],SET_BUTTON=checked[i]
xmanager,'plot_regions',topbase
end
