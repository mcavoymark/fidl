;Copyright 3/26/01 Washington University.  All Rights Reserved.
;get_points_pref.pro  $Revision: 1.3 $

;***************************
pro get_points_pref_event,ev
;***************************

common get_points_pref_comm,wd_shape,wd_color,wd_size,wd_ok,wd_cancel,prf,prfptr

index = where(wd_shape eq ev.id,count)
if count gt 0 then begin
    case index[0] of
        0: prf.points_shape = 1
        1: prf.points_shape = 7
        else: begin
            print,'index is unknown shape in get_points_event, assigning "+"'
            prf.points_shape = 1
        endelse
    endcase
endif

index = where(wd_color eq ev.id,count)
;if count gt 0 then prf.points_color = !WHITE + index
if count gt 0 then prf.points_color = index

if ev.id eq wd_size then prf.points_size = ev.value

if ev.id eq wd_ok then begin
    *prfptr = prf 
    widget_control,ev.top,/DESTROY
endif

if ev.id eq wd_cancel then widget_control,ev.top,/DESTROY 

end


;*****************************************
pro get_points_pref,prfptr_in,group_leader
;*****************************************
common get_points_pref_comm

prfptr = prfptr_in
prf = *prfptr


npts = 2

shapefiles = strarr(npts)
shapefiles[0] = !PIXMAP_PATH+'/plus_black.m.pm'
shapefiles[1] = !PIXMAP_PATH+'/x_black.m.pm'
colorfiles = strarr(!AF3D_NCOLORS)
colorfiles[0] = !PIXMAP_PATH+'/square_white_filled.m.pm'
colorfiles[1] = !PIXMAP_PATH+'/square_yellow_filled.m.pm'
colorfiles[2] = !PIXMAP_PATH+'/square_red_filled.m.pm'
colorfiles[3] = !PIXMAP_PATH+'/square_green_filled.m.pm'
colorfiles[4] = !PIXMAP_PATH+'/square_blue_filled.m.pm'
colorfiles[5] = !PIXMAP_PATH+'/square_orange_filled.m.pm'
colorfiles[6] = !PIXMAP_PATH+'/square_magenta_filled.m.pm'
colorfiles[7] = !PIXMAP_PATH+'/square_cyan_filled.m.pm'
colorfiles[8] = !PIXMAP_PATH+'/square_brown_filled.m.pm'
colorfiles[9] = !PIXMAP_PATH+'/square_black_filled.m.pm'


topbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=group_leader,TITLE='Define Points')
shapebase = widget_base(topbase,/ROW,/EXCLUSIVE)
wd_shape = lonarr(npts)
for i=0,npts-1 do wd_shape[i] = widget_button(shapebase,VALUE=shapefiles[i],/BITMAP)
colorbase = widget_base(topbase,/ROW,/EXCLUSIVE)
wd_color = lonarr(!AF3D_NCOLORS)
for i=0,!AF3D_NCOLORS-1 do wd_color[i] = widget_button(colorbase,VALUE=colorfiles[i],/BITMAP)
wd_size = cw_field(topbase,/ROW,TITLE="Size:",/FLOATING,VALUE=string(prf.points_size,FORMAT='(f3.1)'),XSIZE=5,/ALL_EVENTS)
okbase = widget_base(topbase,/ROW)
wd_ok = widget_button(okbase,value='OK')
wd_cancel = widget_button(okbase,value='Cancel')

widget_control,topbase,/REALIZE

case prf.points_shape of
    1: scrap = 0
    7: scrap = 1
    else : begin
        print,'Error: prf.points_shape='+strtrim(prf.points_shape)+' assigned a plus sign'
        scrap = 0
    endelse
endcase
widget_control,wd_shape[scrap],/SET_BUTTON


;widget_control,wd_color[prf.points_color-!WHITE],/SET_BUTTON

if prf.points_color gt !AF3D_NCOLORS-1 then prf.points_color = 0
widget_control,wd_color[prf.points_color],/SET_BUTTON


xmanager,'get_points_pref',topbase
end
