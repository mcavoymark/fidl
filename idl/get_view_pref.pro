;Copyright 03/13/01 Washington University.  All Rights Reserved.
;get_view_pref.pro  $Revision: 1.17 $

;*************************
pro get_view_pref_event,ev
;*************************

common view_pref_comm,wd_width,wd_height,wd_g_ok,wd_g_cancel,wd_fx,wd_fy,xdim_draw,ydim_draw,wd_xgap,wd_ygap,wd_sx,wd_sy, $
    xgap,ygap,sx,sy,prfptr,color_x,color_y,wd_color_x,wd_color_y,wd_background,black_background,fx,fy, $
    wd_transverse_width,wd_transverse_height,wd_sagittal_width,wd_sagittal_height,wd_coronal_width,wd_coronal_height, $
    transverse_width,transverse_height,sagittal_width,sagittal_height,coronal_width,coronal_height, $
    wd_background_mask,black_background_mask,cancel

common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

cancel = !FALSE
case ev.id of
    wd_width: xdim_draw = ev.value
    wd_height: ydim_draw = ev.value
    wd_xgap: xgap = ev.value
    wd_ygap: ygap = ev.value
    wd_sx: sx = ev.value
    wd_sy: sy = ev.value
    wd_fx: fx = ev.value
    wd_fy: fy = ev.value
    wd_color_x: color_x = ev.value
    wd_color_y: color_y = ev.value
    wd_transverse_width: transverse_width = ev.value
    wd_transverse_height: transverse_height = ev.value
    wd_sagittal_width: sagittal_width = ev.value
    wd_sagittal_height: sagittal_height = ev.value
    wd_coronal_width: coronal_width = ev.value
    wd_coronal_height: coronal_height = ev.value

    wd_background: begin
        case ev.value of
            0: black_background = !TRUE
            1: black_background = !FALSE
            else: print,'Invalid vaule of ev.value for background.'
        endcase
    end

    wd_background_mask: begin
        case ev.value of
            0: black_background_mask = !TRUE
            1: black_background_mask = !FALSE
            else: print,'Invalid vaule of ev.value for background_mask.'
        endcase
    end

    wd_g_ok: begin
        ;print,'transverse_height=',transverse_height
        prf = *prfptr
        prf.xdim_draw = xdim_draw
        prf.ydim_draw = ydim_draw
        prf.xgap = xgap
        prf.ygap = ygap
        prf.color_x = color_x
        prf.color_y = color_y
        prf.transverse_width = transverse_width
        prf.transverse_height = transverse_height
        prf.sagittal_width = sagittal_width
        prf.sagittal_height = sagittal_height
        prf.coronal_width = coronal_width
        prf.coronal_height = coronal_height
        prf.sx = sx
        prf.sy = sy
        prf.fx = fx
        prf.fy = fy
        prf.black_background = black_background
        prf.black_background_mask = black_background_mask
        *prfptr = prf
        widget_control,ev.top,/DESTROY
    end

    wd_g_cancel: begin
        cancel = !TRUE
        widget_control,ev.top,/DESTROY
    end

    else: begin
        print,'Invalid event'
    end
endcase

end

;********************************************
function get_view_pref,prfptr_in,group_leader
;********************************************
common view_pref_comm
common stats_comm

;;;italics = '-b&h-lucidabright-demibold-i-normal--12-120-75-75-p-72-iso8859-1'

prfptr = prfptr_in
win = *vw.win[vw.cw]
sprf = win.sprf[win.idxidx]
prf = *prfptr

xdim_draw = prf.xdim_draw
ydim_draw = prf.ydim_draw
xgap = prf.xgap
ygap = prf.ygap
sx = prf.sx
sy = prf.sy
fx = prf.fx
fy = prf.fy
color_x = prf.color_x
color_y = prf.color_y
transverse_width = prf.transverse_width
transverse_height = prf.transverse_height
;print,'transverse_height=',transverse_height
sagittal_width = prf.sagittal_width
sagittal_height = prf.sagittal_height
coronal_width = prf.coronal_width
coronal_height = prf.coronal_height
black_background = prf.black_background
black_background_mask = prf.black_background_mask

prefbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=group_leader,TITLE='Screen layout')
offset_base = widget_base(prefbase,/ROW)
bkgrnd_base = widget_base(prefbase,/ROW)
bkgrnd_base_mask = widget_base(prefbase,/ROW)
ok_base = widget_base(prefbase,/ROW)
offset_base1 = widget_base(offset_base,/COLUMN)
offset_base2 = widget_base(offset_base,/COLUMN)

wd_width = cw_field(offset_base1,/INTEGER,/ROW,TITLE="Window width:",VALUE=prf.xdim_draw,XSIZE=5,/ALL_EVENTS)
wd_height = cw_field(offset_base2,/INTEGER,/ROW,TITLE="Window height:",VALUE=prf.ydim_draw,XSIZE=5,/ALL_EVENTS)
wd_xgap = cw_field(offset_base1,/INTEGER,/ROW,TITLE="Column border:",VALUE=prf.xgap,XSIZE=5,/ALL_EVENTS)
wd_ygap = cw_field(offset_base2,/INTEGER,/ROW,TITLE="Row border:",VALUE=prf.ygap,XSIZE=5,/ALL_EVENTS)
wd_sx = cw_field(offset_base1,/INTEGER,/ROW,TITLE="Slice label x offset:",VALUE=prf.sx,XSIZE=5,/ALL_EVENTS)
wd_sy = cw_field(offset_base2,/INTEGER,/ROW,TITLE="Slice label y offset:",VALUE=prf.sy,XSIZE=5,/ALL_EVENTS)
wd_fx = cw_field(offset_base1,/INTEGER,/ROW,TITLE="File label x offset:",VALUE=prf.fx,XSIZE=5,/ALL_EVENTS)
wd_fy = cw_field(offset_base2,/INTEGER,/ROW,TITLE="File label y offset:",VALUE=prf.fy,XSIZE=5,/ALL_EVENTS)
wd_color_x = cw_field(offset_base1,/INTEGER,/ROW,TITLE="Color scale x offset:",VALUE=prf.color_x,XSIZE=5,/ALL_EVENTS)
wd_color_y = cw_field(offset_base2,/INTEGER,/ROW,TITLE="Color scale y offset:",VALUE=prf.color_y,XSIZE=5,/ALL_EVENTS)

wd_transverse_width = cw_field(offset_base1,/INTEGER,/ROW,TITLE='Transverse width:',VALUE=prf.transverse_width,XSIZE=3, $
    /ALL_EVENTS)
wd_transverse_height = cw_field(offset_base2,/INTEGER,/ROW,TITLE='Transverse height:',VALUE=prf.transverse_height,XSIZE=3, $
    /ALL_EVENTS)
wd_sagittal_width = cw_field(offset_base1,/INTEGER,/ROW,TITLE='Sagittal width:',VALUE=prf.sagittal_width,XSIZE=3,/ALL_EVENTS)
wd_sagittal_height = cw_field(offset_base2,/INTEGER,/ROW,TITLE='Sagittal height:',VALUE=prf.sagittal_height,XSIZE=3,/ALL_EVENTS)
wd_coronal_width = cw_field(offset_base1,/INTEGER,/ROW,TITLE='Coronal width:',VALUE=prf.coronal_width,XSIZE=3,/ALL_EVENTS)
wd_coronal_height = cw_field(offset_base2,/INTEGER,/ROW,TITLE='Coronal height:',VALUE=prf.coronal_height,XSIZE=3,/ALL_EVENTS)

if prf.black_background eq !TRUE then val = 0 else val = 1
wd_background = cw_bgroup(bkgrnd_base,['Black','White'],EVENT_FUNCT=get_view_scale_pref_event,LABEL_LEFT='Background color:', $
    /EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

if prf.black_background_mask eq !TRUE then val = 0 else val = 1
wd_background_mask = cw_bgroup(bkgrnd_base_mask,['Black','White'],EVENT_FUNCT=get_view_scale_pref_event, $
    LABEL_LEFT='Background color outside of mask:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

wd_g_ok = widget_button(ok_base,value='OK')
wd_g_cancel = widget_button(ok_base,value='Cancel')
index = -1

widget_control,prefbase,/REALIZE
xmanager,'get_view_pref',prefbase

return,cancel
end
