;Copyright 12/14/06 Washington University.  All Rights Reserved.
;plot_labels.pro  $Revision: 1.2 $

;************************
pro plot_labels_event,ev
;************************
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

common view_images_comm_shared,lc_refreshed,prf,win,lc_color_bar_once,wd_hidecolors,wd_showcolors_vert, $
    wd_showcolors_horiz,current_label,current_cells,display_mode,af3dh

common plot_labels_comm,wd_filename,wd_plot,wd_exit,topbase,ncolors,wd_msymbol,checked,color,color_index,wd_labels,pixmap_file, $
    nlab,labels,xstr,ystr,wd_x,wd_y,overfile,msg

if win.filenamelabels[win.idxidx] ne overfile then begin
    overfile = fi.list[win.over_idx[win.idxidx]]
    widget_control,msg,SET_VALUE=overfile

    labels[0] = win.filenamelabels[win.idxidx]
    widget_control,wd_labels[0],SET_VALUE=labels[0]

    checked = *win.label[win.idxidx].checked
    for i=0,nlab-1 do widget_control,wd_filename[i],SET_BUTTON=checked[i]
endif

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
    widget_control,ev.id,SET_VALUE=scrap,/BITMAP,SET_BUTTON=0
endif

index = where(wd_x eq ev.id,count)
if count ne 0 then begin
    dummy = wd_x[index]
    widget_control,dummy[0],GET_VALUE=scrap
    xstr[index] = scrap
endif
index = where(wd_y eq ev.id,count)
if count ne 0 then begin
    dummy = wd_y[index]
    widget_control,dummy[0],GET_VALUE=scrap
    ystr[index] = scrap
endif

index = where(wd_labels eq ev.id,count)
if count ne 0 then begin
    dummy = wd_labels[index]
    widget_control,dummy[0],GET_VALUE=scrap
    labels[index] = scrap
endif

if ev.id eq wd_plot then begin
    if total(checked) eq 0 then $
        stat=dialog_message('Please select at least one label.',/ERROR) $
    else begin    
        set_winlab
        view_images_refresh,fi,dsp,vw.fnt
    endelse 
endif
if ev.id eq wd_exit then begin
    set_winlab
    widget_control,ev.top,/DESTROY
endif
end

;*************
pro set_winlab
;*************
common view_images_comm_shared
common plot_labels_comm
win.filenamelabels[win.idxidx] = labels[0]
goose = *win.labels 
goose[*].s = labels 
goose[*].color = color 
goose[*].color_index = color_index
goose[*].pixmap_file = pixmap_file
goose[*].x = float(xstr)
goose[*].y = float(ystr)
*win.labels = goose 
if ptr_valid(win.label[win.idxidx].checked) then ptr_free,win.label[win.idxidx].checked
win.label[win.idxidx].checked = ptr_new(checked)
end

;****************************************************************************
pro create_widgets_pl,X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size
;****************************************************************************
common stats_comm
common view_images_comm_shared
common plot_labels_comm
scr_ysize = 40
title = 'Plot Labels'
if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then begin
    topbase = widget_base(/COLUMN,TITLE=title,X_SCROLL_SIZE=(1024<x_scroll_size),XSIZE=x_scroll_size, $
        Y_SCROLL_SIZE=(800<y_scroll_size),YSIZE=y_scroll_size)
endif else $
    topbase = widget_base(/COLUMN,TITLE=title)

;msg = widget_label(topbase,/ALIGN_LEFT,VALUE=fi.list[win.over_idx[win.idxidx]]) 
msg = widget_label(topbase,/ALIGN_LEFT,VALUE=overfile) 

rowbase = widget_base(topbase,/ROW)
prefbase = widget_base(topbase,/ROW)
colbase_symbol = widget_base(prefbase,/COLUMN,/EXCLUSIVE)
colbase = widget_base(prefbase,/COLUMN,/NONEXCLUSIVE)
xbase = widget_base(prefbase,/COLUMN)
ybase = widget_base(prefbase,/COLUMN)
label_base = widget_base(prefbase,/COLUMN)

wd_msymbol = lonarr(nlab)
wd_filename = lonarr(nlab)
wd_x = lonarr(nlab)
wd_y = lonarr(nlab)
wd_labels = lonarr(nlab)

xs = max(strlen(labels))*15

for i=0,nlab-1 do begin
    wd_msymbol[i] = widget_button(colbase_symbol,VALUE=pixmap_file[i],/BITMAP,SCR_YSIZE=scr_ysize,/NO_RELEASE)
    wd_filename[i] = widget_button(colbase,VALUE=' ',SCR_YSIZE=scr_ysize)
    wd_x[i] = widget_text(xbase,VALUE=xstr[i],SCR_XSIZE=50,SCR_YSIZE=scr_ysize,/EDITABLE,/ALL_EVENTS)
    wd_y[i] = widget_text(ybase,VALUE=ystr[i],SCR_XSIZE=50,SCR_YSIZE=scr_ysize,/EDITABLE,/ALL_EVENTS)
    wd_labels[i] = widget_text(label_base,VALUE=labels[i],SCR_XSIZE=xs,SCR_YSIZE=scr_ysize,/EDITABLE,/ALL_EVENTS)
endfor
wd_plot = widget_button(rowbase,value='Plot')
wd_exit = widget_button(rowbase,value='Exit')
end

;**************
pro plot_labels
;**************
common stats_comm
common view_images_comm_shared
common plot_labels_comm

ncolors = n_elements(af3dh.color)
nlab = !VIEW_MAX_LABELS

goose = *win.labels
labels = goose[*].s
labels[0] = win.filenamelabels[win.idxidx]
color = goose[*].color
color_index = goose[*].color_index
pixmap_file = goose[*].pixmap_file
xstr = strtrim(goose[*].x,2)
ystr = strtrim(goose[*].y,2)
checked = *win.label[win.idxidx].checked
overfile = fi.list[win.over_idx[win.idxidx]]

create_widgets_pl
geo = widget_info(topbase,/GEOMETRY)
xdim = geo.scr_xsize + 2*geo.margin
ydim = geo.scr_ysize + 2*geo.margin
if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
    widget_control,topbase,/DESTROY
    create_widgets_pl,X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim
endif

widget_control,topbase,/REALIZE
for i=0,nlab-1 do widget_control,wd_filename[i],SET_BUTTON=checked[i]
xmanager,'plot_labels',topbase
end
