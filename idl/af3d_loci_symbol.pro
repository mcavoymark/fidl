;Copyright 11/30/01 Washington University.  All Rights Reserved.
;af3d_loci_symbol.pro  $Revision: 1.3 $

;****************************
pro af3d_loci_symbol_event,ev
;****************************

common af3d_loci_symbol_comm,wd_shape,wd_color,wd_fill,wd_size,wd_thick,wd_ok,af3d,af3dh,af3dptr

;print,'here top af3d_loci_symbol_event'

index = where(wd_shape eq ev.id,count)
if count gt 0 then begin
    af3d.shape = af3dh.shape[index]
    af3d.shape_index = index 
    af3d.psym = af3dh.psym[index] 
endif

index = where(wd_color eq ev.id,count)
if count gt 0 then begin
    af3d.color = af3dh.color[index]
    af3d.color_index = index
endif

if ev.id eq wd_fill then begin
    af3d.fill = af3dh.fill[ev.select]
endif

if ev.id eq wd_size then af3d.size = ev.value
if ev.id eq wd_thick then af3d.thick = ev.value

if ev.id eq wd_ok then begin
    *af3dptr = af3d
    widget_control,ev.top,/DESTROY
endif

;print,'here bottom af3d_loci_symbol_event'
end


;*****************************************************************************
pro af3d_loci_symbol,af3d_in,group_leader,af3dh_in,nshapes,ncolors,points_size
;*****************************************************************************
common af3d_loci_symbol_comm

af3dptr = af3d_in
af3d = *af3d_in
af3dh = af3dh_in

shapefiles = strarr(nshapes)
for i=0,nshapes-1 do shapefiles[i] = !PIXMAP_PATH+'/'+af3dh.shape[i]+'_black_unfilled.m.pm' 
colorfiles = strarr(ncolors)
for i=0,ncolors-1 do colorfiles[i] = !PIXMAP_PATH+'/square_'+af3dh.color[i]+'_filled.m.pm' 

if af3d.size eq 0 then af3d.size = 1.
if af3d.thick eq 0 then af3d.thick = 1.

topbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=group_leader,TITLE='Define Symbols')
shapebase = widget_base(topbase,/ROW,/EXCLUSIVE)
wd_shape = lonarr(nshapes)
for i=0,nshapes-1 do wd_shape[i] = widget_button(shapebase,VALUE=shapefiles[i],/BITMAP)
colorbase = widget_base(topbase,/ROW,/EXCLUSIVE)
wd_color = lonarr(ncolors)
for i=0,ncolors-1 do wd_color[i] = widget_button(colorbase,VALUE=colorfiles[i],/BITMAP)
rowbase = widget_base(topbase,/ROW)
fillbase = widget_base(rowbase,/ROW,/NONEXCLUSIVE)
wd_fill = widget_button(fillbase,VALUE='Filled')
wd_size = cw_field(rowbase,/ROW,TITLE="Size:",/FLOATING,VALUE=string(af3d.size,FORMAT='(f3.1)'),XSIZE=5,/ALL_EVENTS)
wd_thick = cw_field(rowbase,/ROW,TITLE="Thickness:",/FLOATING,VALUE=string(af3d.thick,FORMAT='(f3.1)'),XSIZE=5, $
    /ALL_EVENTS)
wd_ok = widget_button(topbase,value='OK')

widget_control,topbase,/REALIZE
widget_control,wd_shape[af3d.shape_index],/SET_BUTTON
widget_control,wd_color[af3d.color_index],/SET_BUTTON
if af3d.fill eq 'filled.m.pm' then widget_control,wd_fill,/SET_BUTTON
xmanager,'af3d_loci_symbol',topbase
end
