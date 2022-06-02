;Copyright 10/26/00 Washington University.  All Rights Reserved.
;get_view_scale_pref.pro  $Revision: 1.21 $
pro get_view_scale_pref_event,ev

common view_scale_pref_comm, wd_g_ok,wd_g_cancel,xdim_draw,ydim_draw,zoom,overlay_zoom_type,scale_pos_max,scale_pos_min, $
    lc_auto_scale,lc_act_type,prfptr,wd_zoom,wd_overlay_zoom_type,wd_act_type,wd_lc_auto_scale,wd_scale_pos_min, $
    wd_scale_pos_max,wd_scale_neg_min,wd_scale_neg_max,scale_neg_min,scale_neg_max,wd_orient,orientation,wd_scale_anat_min, $
    wd_scale_anat_max,scale_anat_min,scale_anat_max,wd_display_units,display_units,wd_coding,coding,cancel, $
    wd_anat_local_global,anat_local_global,wd_crop_anat,crop_anat

common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

cancel = !FALSE
case ev.id of
    wd_scale_pos_min: scale_pos_min = ev.value
    wd_scale_pos_max: scale_pos_max = ev.value
    wd_scale_neg_min: scale_neg_min = ev.value
    wd_scale_neg_max: scale_neg_max = ev.value
    wd_scale_anat_min: scale_anat_min = ev.value
    wd_scale_anat_max: scale_anat_max = ev.value
    wd_anat_local_global: anat_local_global = ev.value 
    wd_crop_anat: crop_anat = ev.value 
    wd_zoom: zoom = 2^ev.value
    wd_overlay_zoom_type: begin
        case ev.value of
            0: overlay_zoom_type = !BILINEAR
            1: overlay_zoom_type = !NEAREST_NEIGHBOR
            else: print,'Invalid button for overlay_zoom_type 
        endcase
    end
    wd_orient: begin
        case ev.value of
	    0: orientation = !NEURO
	    1: orientation = !RADIOL
 	    else: print,'Invalid button for activation types.'
        endcase
    end
    wd_act_type: begin
        if(lc_auto_scale eq !TRUE) then begin
            case ev.value of
                0: begin
                    lc_act_type = !POSITIVE_ACTIVATIONS
                    widget_control,wd_scale_neg_min,SENSITIVE=0
                    widget_control,wd_scale_pos_min,/SENSITIVE
                    widget_control,wd_scale_pos_max,SENSITIVE=0
                    widget_control,wd_scale_neg_max,SENSITIVE=0
                end
                1: begin
                    lc_act_type = !NEGATIVE_ACTIVATIONS
                    widget_control,wd_scale_neg_min,/SENSITIVE
                    widget_control,wd_scale_pos_min,SENSITIVE=0
                    widget_control,wd_scale_pos_max,SENSITIVE=0
                    widget_control,wd_scale_neg_max,SENSITIVE=0
                end
                2: begin
                    lc_act_type = !ALL_ACTIVATIONS
                    widget_control,wd_scale_neg_min,/SENSITIVE
                    widget_control,wd_scale_neg_max,SENSITIVE=0
                    widget_control,wd_scale_pos_min,/SENSITIVE
                    widget_control,wd_scale_pos_max,SENSITIVE=0
                end
                else: print,'Invalid activation type.'
            endcase
        endif else begin
            case ev.value of
                0: begin
                    lc_act_type = !POSITIVE_ACTIVATIONS
                    widget_control,wd_scale_neg_min,SENSITIVE=0
                    widget_control,wd_scale_neg_max,SENSITIVE=0
                    widget_control,wd_scale_pos_min,/SENSITIVE
                    widget_control,wd_scale_pos_max,/SENSITIVE
                end
                1: begin
                    lc_act_type = !NEGATIVE_ACTIVATIONS
                    widget_control,wd_scale_neg_min,/SENSITIVE
                    widget_control,wd_scale_neg_max,/SENSITIVE
                    widget_control,wd_scale_pos_min,SENSITIVE=0
                    widget_control,wd_scale_pos_max,SENSITIVE=0
                end
                2: begin
                    lc_act_type = !ALL_ACTIVATIONS
                    widget_control,wd_scale_neg_min,/SENSITIVE
                    widget_control,wd_scale_neg_max,/SENSITIVE
                    widget_control,wd_scale_pos_min,/SENSITIVE
                    widget_control,wd_scale_pos_max,/SENSITIVE
                end
                else: print,'Invalid activation type.'
            endcase
        endelse
    end

    wd_lc_auto_scale: begin
        if(ev.value eq 0) then begin
            lc_auto_scale = !TRUE
            case lc_act_type of
                !POSITIVE_ACTIVATIONS: begin
                    widget_control,wd_scale_neg_min,SENSITIVE=0
                    widget_control,wd_scale_pos_min,/SENSITIVE
                    widget_control,wd_scale_pos_max,SENSITIVE=0
                    widget_control,wd_scale_neg_max,SENSITIVE=0
                end
                !NEGATIVE_ACTIVATIONS: begin
                    widget_control,wd_scale_neg_min,/SENSITIVE
                    widget_control,wd_scale_pos_min,SENSITIVE=0
                    widget_control,wd_scale_pos_max,SENSITIVE=0
                    widget_control,wd_scale_neg_max,SENSITIVE=0
                end
                !ALL_ACTIVATIONS: begin
                    widget_control,wd_scale_neg_min,/SENSITIVE
                    widget_control,wd_scale_neg_max,SENSITIVE=0
                    widget_control,wd_scale_pos_min,/SENSITIVE
                    widget_control,wd_scale_pos_max,SENSITIVE=0
                end
                else: print,'Invalid activation type.'
            endcase
        endif else begin
            lc_auto_scale = !FALSE
            case lc_act_type of
                !POSITIVE_ACTIVATIONS: begin
                    widget_control,wd_scale_neg_min,SENSITIVE=0
                    widget_control,wd_scale_neg_max,SENSITIVE=0
                    widget_control,wd_scale_pos_min,/SENSITIVE
                    widget_control,wd_scale_pos_max,/SENSITIVE
                end
                !NEGATIVE_ACTIVATIONS: begin
                    widget_control,wd_scale_neg_min,/SENSITIVE
                    widget_control,wd_scale_neg_max,/SENSITIVE
                    widget_control,wd_scale_pos_min,SENSITIVE=0
                    widget_control,wd_scale_pos_max,SENSITIVE=0
                end
                !ALL_ACTIVATIONS: begin
                    widget_control,wd_scale_neg_min,/SENSITIVE
                    widget_control,wd_scale_neg_max,/SENSITIVE
                    widget_control,wd_scale_pos_min,/SENSITIVE
                    widget_control,wd_scale_pos_max,/SENSITIVE
                end
                else: print,'Invalid activation type.'
            endcase
        endelse
    end
    wd_display_units: begin
        case ev.value of
	    0: display_units = !DISPLAY_Z
	    1: display_units = !DISPLAY_P
	    2: display_units = !DISPLAY_P_ROUNDED
 	    else: print,'Invalid button for display_units
        endcase
    end

    wd_coding: coding = ev.value 

    wd_g_ok: begin
        prf = *prfptr
        prf.scale_pos_min = scale_pos_min
        prf.scale_pos_max = scale_pos_max
        prf.scale_neg_min = scale_neg_min
        prf.scale_neg_max = scale_neg_max
        prf.zoom = zoom
        prf.overlay_zoom_type = overlay_zoom_type
        prf.lc_act_type = lc_act_type
        prf.lc_auto_scale = lc_auto_scale
        prf.orientation = orientation
        prf.scale_anat_min = scale_anat_min
        prf.scale_anat_max = scale_anat_max
        prf.anat_local_global = anat_local_global
        prf.crop_anat = crop_anat 
        prf.display_units = display_units
        prf.coding = coding
        ;prf.cancel = !FALSE
        *prfptr = prf

        ;print,'prf.scale_pos_min=',prf.scale_pos_min
        ;print,'prf.scale_pos_max=',prf.scale_pos_max
        ;print,'prf.scale_neg_min=',prf.scale_neg_min
        ;print,'prf.scale_neg_max=',prf.scale_neg_max
        ;print,''

        widget_control,ev.top,/DESTROY
    end


    ;wd_g_cancel: begin
    ;    prf = *prfptr
    ;    prf.cancel = !TRUE
    ;    *prfptr = prf
    ;    widget_control,ev.top,/DESTROY
    ;end

    wd_g_cancel: begin
        cancel = !TRUE
        widget_control,ev.top,/DESTROY
    end


    else: begin
        print,'Invalid event'
    end

endcase

;help,ev,/STRUCTURE
;print,'scale_pos_min=',scale_pos_min
;print,'scale_pos_max=',scale_pos_max
;print,'scale_neg_min=',scale_neg_min
;print,'scale_neg_max=',scale_neg_max
;print,''

;return
end

;**************************************************
function get_view_scale_pref,prfptr_in,group_leader
;**************************************************
common view_scale_pref_comm
common stats_comm

;;;italics = '-b&h-lucidabright-demibold-i-normal--12-120-75-75-p-72-iso8859-1'

prfptr = prfptr_in
prf = *prfptr
zoom = prf.zoom
overlay_zoom_type = prf.overlay_zoom_type
lc_act_type = prf.lc_act_type
lc_auto_scale = prf.lc_auto_scale
scale_pos_max = prf.scale_pos_max
scale_pos_min = prf.scale_pos_min
scale_neg_max = prf.scale_neg_max
scale_neg_min = prf.scale_neg_min
scale_anat_min = prf.scale_anat_min
scale_anat_max = prf.scale_anat_max
anat_local_global = prf.anat_local_global
crop_anat = prf.crop_anat
orientation = prf.orientation
display_units = prf.display_units
coding = prf.coding

prefbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=group_leader)
zoom_base = widget_base(prefbase,/ROW)
orient_base = widget_base(prefbase,/ROW)
type_base = widget_base(prefbase,/ROW)
scale_lc_base = widget_base(prefbase,/ROW)
scale_pos_base = widget_base(prefbase,/ROW)
scale_neg_base = widget_base(prefbase,/ROW)
scale_anat_base = widget_base(prefbase,/ROW)
anat_local_global_base = widget_base(prefbase,/ROW)
display_units_base = widget_base(prefbase,/ROW)
coding_base = widget_base(prefbase,/ROW)
ok_base = widget_base(prefbase,/ROW)


case zoom of
    1: val = 0
    2: val = 1
    4: val = 2
    8: val = 3
    else: begin
        val = 1
        zoom = 2
    end
endcase
wd_zoom = cw_bgroup(zoom_base,['1','2','4','8'],EVENT_FUNCT=get_view_scale_pref_event,LABEL_LEFT='Zoom factor:', $
    /EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

case overlay_zoom_type of
    !BILINEAR: val = 0
    !NEAREST_NEIGHBOR: val = 1
    else: begin
        val = 0
        overlay_zoom_type = !BILINEAR
    end
endcase
wd_overlay_zoom_type = cw_bgroup(zoom_base,['Bilinear','Nearest neighbor'],EVENT_FUNCT=get_view_scale_pref_event, $
    LABEL_LEFT='Overlay zoom type:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

case orientation of
    !NEURO: val = 0
    !RADIOL: val = 1
    else: begin
        val = 0
        orientation = !NEURO
    end
endcase
wd_orient = cw_bgroup(orient_base,['Neuropsych','Radiological'],EVENT_FUNCT=get_view_scale_pref_event,LABEL_LEFT='Rigt/left orientation:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

case lc_act_type of
    !POSITIVE_ACTIVATIONS: val = 0
    !NEGATIVE_ACTIVATIONS: val = 1
    !ALL_ACTIVATIONS: val = 2
    else: begin
        val = 2
        lc_act_type = !ALL_ACTIVATIONS
        lc_auto_scale = !TRUE
    end
endcase
wd_act_type = cw_bgroup(type_base,['Positive','Negative','All'],EVENT_FUNCT=get_view_scale_pref_event,LABEL_LEFT='Activations to display:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

if(prf.lc_auto_scale eq !TRUE) then $
    val = 0 $
else $
    val = 1
wd_lc_auto_scale = cw_bgroup(scale_lc_base,['Auto','Manual'],EVENT_FUNCT=get_view_scale_pref_event,LABEL_LEFT='Scaling method:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

str = strcompress(string(prf.scale_pos_min,FORMAT='(g0)'))
wd_scale_pos_min = cw_field(scale_pos_base,/ROW,TITLE="Weakest positive value:",VALUE=str,XSIZE=12,/ALL_EVENTS)
str = strcompress(string(prf.scale_pos_max,FORMAT='(g0)'))
wd_scale_pos_max = cw_field(scale_pos_base,/ROW,TITLE="Strongest positive value:",VALUE=str,XSIZE=12,/ALL_EVENTS)
str = strcompress(string(prf.scale_neg_min,FORMAT='(g0)'))
wd_scale_neg_min = cw_field(scale_neg_base,/ROW,TITLE="Weakest negative value:",VALUE=str,XSIZE=12,/ALL_EVENTS)
str = strcompress(string(prf.scale_neg_max,FORMAT='(g0)'))
wd_scale_neg_max = cw_field(scale_neg_base,/ROW,TITLE="Strongest negative value:",VALUE=str,XSIZE=12,/ALL_EVENTS)

if(prf.lc_auto_scale eq !TRUE) then begin
    case lc_act_type of
        !POSITIVE_ACTIVATIONS: begin
            widget_control,wd_scale_neg_min,SENSITIVE=0
            widget_control,wd_scale_pos_min,/SENSITIVE
            widget_control,wd_scale_pos_max,SENSITIVE=0
            widget_control,wd_scale_neg_max,SENSITIVE=0
        end
        !NEGATIVE_ACTIVATIONS: begin
            widget_control,wd_scale_neg_min,/SENSITIVE
            widget_control,wd_scale_pos_min,SENSITIVE=0
            widget_control,wd_scale_pos_max,SENSITIVE=0
            widget_control,wd_scale_neg_max,SENSITIVE=0
        end
        !ALL_ACTIVATIONS: begin
            widget_control,wd_scale_neg_min,/SENSITIVE
            widget_control,wd_scale_neg_max,SENSITIVE=0
            widget_control,wd_scale_pos_min,/SENSITIVE
            widget_control,wd_scale_pos_max,SENSITIVE=0
        end
        else: print,'Invalid activation type.'
    endcase
endif else begin
    case lc_act_type of
        !POSITIVE_ACTIVATIONS: begin
            widget_control,wd_scale_neg_min,SENSITIVE=0
            widget_control,wd_scale_neg_max,SENSITIVE=0
            widget_control,wd_scale_pos_min,/SENSITIVE
            widget_control,wd_scale_pos_max,/SENSITIVE
        end
        !NEGATIVE_ACTIVATIONS: begin
            widget_control,wd_scale_neg_min,/SENSITIVE
            widget_control,wd_scale_neg_max,/SENSITIVE
            widget_control,wd_scale_pos_min,SENSITIVE=0
            widget_control,wd_scale_pos_max,SENSITIVE=0
        end
        !ALL_ACTIVATIONS: begin
            widget_control,wd_scale_neg_min,/SENSITIVE
            widget_control,wd_scale_neg_max,/SENSITIVE
            widget_control,wd_scale_pos_min,/SENSITIVE
            widget_control,wd_scale_pos_max,/SENSITIVE
        end
        else: print,'Invalid activation type.'
    endcase
endelse




;wd_scale_anat_text = widget_label(scale_anat_base,VALUE='Anatomy adjust contrast:')
;wd_scale_anat_min = cw_field(scale_anat_base,/ROW,TITLE='min',VALUE=strtrim(prf.scale_anat_min,2),XSIZE=8,/ALL_EVENTS)
;wd_scale_anat_max = cw_field(scale_anat_base,/ROW,TITLE='max',VALUE=strtrim(prf.scale_anat_max,2),XSIZE=8,/ALL_EVENTS)
;wd_anat_local_global = cw_bgroup(scale_anat_base,['Local','Global'],EVENT_FUNCT=get_view_scale_pref_event, $
;    LABEL_LEFT='scaling',/EXCLUSIVE,ROW=1,SET_VALUE=anat_local_global,/NO_RELEASE)
;START131016
wd_anat_local_global = cw_bgroup(scale_anat_base,['Off','On'],EVENT_FUNCT=get_view_scale_pref_event, $
    LABEL_LEFT='Anatomy adjust contrast:',/EXCLUSIVE,ROW=1,SET_VALUE=anat_local_global,/NO_RELEASE)
wd_scale_anat_min = cw_field(scale_anat_base,/ROW,TITLE='min',VALUE=strtrim(prf.scale_anat_min,2),XSIZE=8,/ALL_EVENTS)
wd_scale_anat_max = cw_field(scale_anat_base,/ROW,TITLE='max',VALUE=strtrim(prf.scale_anat_max,2),XSIZE=8,/ALL_EVENTS)




wd_crop_anat = cw_field(anat_local_global_base,/ROW,TITLE="Crop anatomy:",VALUE=prf.crop_anat,XSIZE=8,/ALL_EVENTS)
case display_units of
    !DISPLAY_Z: val = 0
    !DISPLAY_P: val = 1
    !DISPLAY_P_ROUNDED: val = 2
    else: begin
        val = 0
        display_units = !DISPLAY_Z
    end
endcase
wd_display_units = cw_bgroup(display_units_base,['Z statistic','Significance probability', $
    'Significance probability (rounded)'],EVENT_FUNCT=get_view_scale_pref_event,LABEL_LEFT='Units:',/EXCLUSIVE, $
    ROW=1,SET_VALUE=val,/NO_RELEASE)

wd_coding = cw_bgroup(coding_base,['None','One-zero','P value'],EVENT_FUNCT=get_view_scale_pref_event,LABEL_LEFT='Coding:', $
    /EXCLUSIVE,ROW=1,SET_VALUE=coding,/NO_RELEASE)

wd_g_ok = widget_button(ok_base,value='OK')
wd_g_cancel = widget_button(ok_base,value='Cancel')
index = -1

widget_control,prefbase,/REALIZE
xmanager,'get_view_scale_pref',prefbase

return,cancel
end
