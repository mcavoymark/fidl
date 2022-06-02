;Copyright 2/21/01 Washington University.  All Rights Reserved.
;get_view_slice_pref.pro  $Revision: 1.16 $
pro get_view_slice_pref_event,ev
common view_slice_pref_comm, wd_reslice_redraw,wd_g_cancel,wd_slice_not_z,wd_view,wd_sagittal_face_left,wd_first_slice, $
    wd_last_slice,wd_slice_incr,slice_not_z,view,sagittal_face_left,first_slice,last_slice,slice_incr,prfptr,wd_reslice, $
    slice_type,wd_slice_type,wd_sup_to_inf,sup_to_inf,wd_inf_to_sup,wd_mask,mask,cancel
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw
cancel = !FALSE

;print,'ev.id=',ev.id

case ev.id of
    wd_first_slice: begin
        first_slice = ev.value
    end
    wd_last_slice: begin
        last_slice = ev.value
    end
    wd_slice_incr: begin
        slice_incr = abs(ev.value)
        if(ev.value lt 0) then $
            widget_control,wd_slice_incr,SET_VALUE=slice_incr
    end
    wd_slice_not_z: begin
        case ev.value of
	    0: slice_not_z = !FALSE
	    1: slice_not_z = !TRUE
 	    else: print,'Invalid button for orientation.'
	endcase
    end
    wd_sagittal_face_left : begin
        case ev.value of
	    0: sagittal_face_left = !TRUE
	    1: sagittal_face_left = !FALSE
 	    else: print,'Invalid button for orientation.'
	endcase
    end
    wd_slice_type : begin
        case ev.value of
	    0: slice_type = !ANATOMIC
	    1: slice_type = !STATISTICAL
 	    else: print,'Invalid button for orientation.'
	endcase
    end

    wd_sup_to_inf: begin

        ;sup_to_inf = ev.select
        ;if ev.select ne 0 then sup_to_inf = !TRUE
        ;START130329
        if ev.select ne 0 then sup_to_inf = !FALSE

        ;print,'wd_sup_to_inf sup_to_inf=',sup_to_inf,' ev.select=',ev.select
    end
    wd_inf_to_sup: begin

        ;if ev.select ne 0 then sup_to_inf = !FALSE
        ;START130329
        sup_to_inf = ev.select
        if ev.select ne 0 then sup_to_inf = !TRUE


        ;print,'wd_inf_to_sup sup_to_inf=',sup_to_inf,' ev.select=',ev.select
    end

    wd_view: begin
        case ev.value of
	    0: begin
                view = !TRANSVERSE

                ;val1 = 'Superior to inferior:'
                ;val2 = 'Inferior to superior:'

                val1 = 'Inferior to superior:'
                val2 = 'Superior to inferior:'

                widget_control,wd_sagittal_face_left,SENSITIVE=0
            end
	    1: begin
                view = !SAGITAL
                val1 = 'Right to left'
                val2 = 'Left to right:'
                widget_control,wd_sagittal_face_left,/SENSITIVE
            end
	    2: begin
                view = !CORONAL
                val1 = 'Anterior to posterior'
                val2 = 'Posterior to anterior'
                widget_control,wd_sagittal_face_left,SENSITIVE=0
            end
 	    else: print,'Invalid button for orientation.'
	endcase
        widget_control,wd_sup_to_inf,SET_VALUE=val1
        widget_control,wd_inf_to_sup,SET_VALUE=val2
    end

    wd_mask : begin
        case ev.value of
            0: mask = !TRUE
            1: mask = !FALSE
            else: print,'Invalid button for mask.'
        endcase
    end

    wd_reslice: begin

        ;print,'sup_to_inf=',sup_to_inf

        prf = *prfptr
        prf.slice_not_z = slice_not_z
        prf.view = view
        prf.sagittal_face_left = sagittal_face_left
        prf.slice_type = slice_type
        prf.sup_to_inf = sup_to_inf
        if(sup_to_inf eq !TRUE) then begin 
            prf.first_slice = first_slice > last_slice
            prf.last_slice =  first_slice < last_slice
            prf.slice_incr =  -abs(slice_incr)
        endif else begin
            prf.first_slice = first_slice < last_slice
            prf.last_slice =  first_slice > last_slice
            prf.slice_incr =  abs(slice_incr)
        endelse
        prf.mask = mask
        ;prf.cancel = !FALSE
        *prfptr = prf
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
        print,'Invalid event in get_view_slice_pref'
    end

endcase

return
end

;*********************************************
;pro get_view_slice_pref,prfptr_in,group_leader
function get_view_slice_pref,prfptr_in,group_leader
;*********************************************
common view_slice_pref_comm
common stats_comm

prfptr = prfptr_in[0]
prf = *prfptr_in[0]
slice_not_z = prf.slice_not_z
view = prf.view
sagittal_face_left = prf.sagittal_face_left
slice_type = prf.slice_type
sup_to_inf = prf.sup_to_inf
first_slice = prf.first_slice < prf.last_slice
last_slice =  prf.first_slice > prf.last_slice
slice_incr =  -abs(prf.slice_incr)
mask = prf.mask

prefbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=group_leader)
colbase = widget_base(prefbase,/COLUMN)
slice_base = widget_base(prefbase,/ROW)
supbase = widget_base(prefbase,/ROW)
ok_base = widget_base(prefbase,/ROW)

labels = ['Talairach XYZ','Atlas plane']
case prf.slice_not_z of
     !TRUE: val = 1
     !FALSE: val = 0
     else: begin
         prf.slice_not_z = !FALSE
         val = 0
     end
endcase
wd_slice_not_z = cw_bgroup(colbase,labels,EVENT_FUNCT=get_view_slice_pref_event,LABEL_LEFT='Slice spec:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

labels = ['Anatomic','Statistical/Functional']
case prf.slice_type of
    !ANATOMIC: val = 0
    !STATISTICAL: val = 1
    else: begin
        slice_type = !ANATOMIC
        prf.slice_type = !ANATOMIC
        val = 0
    end
endcase
wd_slice_type = cw_bgroup(colbase,labels,EVENT_FUNCT=get_view_slice_pref_event,LABEL_LEFT='Base image type:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

labels = ['Transverse','Sagittal','Coronal']
case prf.view of
    !TRANSVERSE: val = 0
    !SAGITAL: val = 1
    !CORONAL:val = 2
    else: begin
        prf.view = !TRANSVERSE
        view = !TRANSVERSE
        val = 0 ; Invalid value, choose transverse as default.
    end
endcase
wd_view = cw_bgroup(colbase,labels,EVENT_FUNCT=get_view_slice_pref_event,LABEL_LEFT='View:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

labels = ['Facing-Left','Facing-Right']
case prf.sagittal_face_left of
     !TRUE: val = 0
     !FALSE: val = 1
     else: begin
         prf.sagittal_face_left = !TRUE
         sagittal_face_left = !TRUE
         val = 0
     end
endcase
wd_sagittal_face_left = cw_bgroup(colbase,labels,EVENT_FUNCT=get_view_slice_pref_event,LABEL_LEFT='Sagittal orientation:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)
if(prf.view ne !SAGITAL) then $
    widget_control,wd_sagittal_face_left,SENSITIVE=0 $
else $
    widget_control,wd_sagittal_face_left,/SENSITIVE

case prf.mask of
    !TRUE: val = 0
    !FALSE: val = 1
    else: begin
        val = 0
        mask = !TRUE
    end
endcase
wd_mask = cw_bgroup(colbase,['On','Off'],EVENT_FUNCT=get_view_slice_pref_event,LABEL_LEFT='Mask:',/EXCLUSIVE,ROW=1,SET_VALUE=val, $
    /NO_RELEASE)
   

wd_first_slice = cw_field(slice_base,/FLOATING,/ROW,TITLE="Slice boundary:",VALUE=first_slice,XSIZE=5,/ALL_EVENTS,/INTEGER)
wd_last_slice = cw_field(slice_base,/FLOATING,/ROW,TITLE="Slice boundary:",VALUE=last_slice,XSIZE=5,/ALL_EVENTS,/INTEGER)
wd_slice_incr = cw_field(slice_base,/FLOATING,/ROW,TITLE="Slice increment:",VALUE=abs(slice_incr),XSIZE=5,/ALL_EVENTS,/INTEGER)

;labels = ['Superior to inferior','Inferior to superior']
labels = ['Inferior to superior','Superior to inferior']
case sup_to_inf of
     !TRUE: val = 0
     !FALSE: val = 1
     else: begin
         sup_to_inf = !TRUE
         val = 0
     end
endcase
wd_sup_to_inf_text = widget_label(supbase,VALUE='Slice Ordering:')
supbut_base = widget_base(supbase,/ROW,/EXCLUSIVE)
case prf.view of
    !TRANSVERSE: begin
        ;val1 = 'Superior to inferior:'
        ;val2 = 'Inferior to superior:'

        val1 = 'Inferior to superior:'
        val2 = 'Superior to inferior:'
    end
    !SAGITAL: begin
        val1 = 'Left to right:'
        val2 = 'Right to left'
    end
    !CORONAL: begin
        val1 = 'Anterior to posterior'
        val2 = 'Posterior to anterior'
    end
    else: 
endcase
wd_sup_to_inf = widget_button(supbut_base,VALUE=val1)
wd_inf_to_sup = widget_button(supbut_base,VALUE=val2)

wd_reslice = widget_button(ok_base,value='Reslice')
wd_g_cancel = widget_button(ok_base,value='Cancel')
index = -1

widget_control,prefbase,/REALIZE
widget_control,wd_sup_to_inf,SET_VALUE=val1
widget_control,wd_inf_to_sup,SET_VALUE=val2
widget_control,wd_sup_to_inf,SET_BUTTON=sup_to_inf
widget_control,wd_inf_to_sup,SET_BUTTON=(not sup_to_inf)
xmanager,'get_view_slice_pref',prefbase

return,cancel
end

