;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_view_print_pref.pro  $Revision: 1.8 $

;*******************************
pro get_view_print_pref_event,ev
;*******************************

common view_print_pref_comm, wd_g_ok,wd_g_cancel, wd_orient, $
       wd_color,wd_x0,wd_y0,wd_xdim,wd_ydim,print_xdim,print_ydim, $
       print_x0,print_y0,print_orientation,print_color,prfptr,printer_list, $
       printer_name,wd_printer_name,wd_current_printer,wd_load_slice_pref

common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg, $
       glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

case ev.id of

;;;    wd_x0: print_x0 = ev.value
;;;    wd_y0: print_y0 = ev.value
    wd_xdim: print_xdim = ev.value
    wd_ydim: print_ydim = ev.value

    wd_printer_name: begin
        printer_name = printer_list[ev.value-1]
        str = string(printer_name,FORMAT='("Current printer: ",a)')
        widget_control,wd_current_printer,SET_VALUE=str
    end

    wd_orient: begin
        case ev.value of
	    0: print_orientation = !PRINT_LANDSCAPE
	    1: print_orientation = !PRINT_PORTRAIT
 	    else: print,'Invalid button for print orientation.'
        endcase
      end

    wd_color: begin
        case ev.value of
	    0: print_color = !TRUE
	    1: print_color = !FALSE
 	    else: print,'Invalid button for print color.'
        endcase
      end

    wd_g_ok: begin
        prf = *prfptr
        prf.print_xdim = print_xdim
        prf.print_ydim = print_ydim
        prf.print_x0 = print_x0
        prf.print_y0 = print_y0
        prf.print_orientation = print_orientation
        prf.print_color = print_color
        prf.printer_name = printer_name
        *prfptr = prf
        widget_control,ev.top,/DESTROY
    end

    wd_g_cancel: begin
        widget_control,ev.top,/DESTROY
    end

    else: begin
        print,'Invalid event'
    end

endcase

return
end

;*********************************************
pro get_view_print_pref,prfptr_in,group_leader
;*********************************************


common view_print_pref_comm

common stats_comm

prfptr = prfptr_in
prf = *prfptr
print_xdim = prf.print_xdim
print_ydim = prf.print_ydim
print_x0 = prf.print_x0
print_y0 = prf.print_y0
print_orientation = prf.print_orientation
print_color = prf.print_color
printer_name = prf.printer_name

prefbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=group_leader)
printer_base = widget_base(prefbase,/ROW)
orient_base = widget_base(prefbase,/ROW)
color_base = widget_base(prefbase,/ROW)
origin_base = widget_base(prefbase,/ROW)
size_base = widget_base(prefbase,/ROW)
ok_base = widget_base(prefbase,/ROW)


;printer_list = str_sep(!PRINTERS,' ',/TRIM)
;printer_list = str_sep(!PRINTERS,',',/TRIM)
printer_list = get_printer_list()


np = n_elements(printer_list)
if(np gt 0) then begin
    labels = strarr(np+1)
    labels[0] = '1\Printers'
    for i=1,np do begin
        labels[i] = string(printer_list[i-1],FORMAT='("0\",a)')
    endfor
    wd_printer_name = cw_pdmenu(printer_base,labels,/RETURN_INDEX)
endif
str = string(printer_name,FORMAT='("Current printer: ",a)')
wd_current_printer = widget_label(printer_base,VALUE=str,/ALIGN_LEFT,XSIZE=450)

case print_orientation of
    !PRINT_LANDSCAPE: val = 0
    !PRINT_PORTRAIT: val = 1
    else: begin
        val = 0
        print_orientation = !PRINT_LANDSCAPE
    end
endcase
wd_orient = cw_bgroup(orient_base,['Landscape','Portrait'],EVENT_FUNCT=get_view_print_pref_event,LABEL_LEFT='Orientation:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

if(print_color eq !TRUE) then $
    val = 0 $
else $
    val = 1
wd_color = cw_bgroup(color_base,['Color','Black & white'],EVENT_FUNCT=get_view_print_pref_event,LABEL_LEFT='Printer type:',/EXCLUSIVE,ROW=1,SET_VALUE=val,/NO_RELEASE)

;;;str = strcompress(string(prf.print_x0,FORMAT='(f5.2)'),/REMOVE_ALL)
;;;wd_x0 = cw_field(origin_base,/FLOATING,/ROW,TITLE="X margin (inches):",VALUE=str,XSIZE=5,/ALL_EVENTS)
;;;str = strcompress(string(prf.print_y0,FORMAT='(f5.2)'),/REMOVE_ALL)
;;;wd_y0 = cw_field(origin_base,/FLOATING,/ROW,TITLE="Y margin (inches):",VALUE=str,XSIZE=5,/ALL_EVENTS)
str = strcompress(string(prf.print_xdim,FORMAT='(f5.2)'),/REMOVE_ALL)
wd_xdim = cw_field(size_base,/FLOATING,/ROW,TITLE="X size in inches",VALUE=str,XSIZE=5,/ALL_EVENTS)
str = strcompress(string(prf.print_ydim,FORMAT='(f5.2)'),/REMOVE_ALL)
wd_ydim = cw_field(size_base,/FLOATING,/ROW,TITLE="Y size in inches",VALUE=str,XSIZE=5,/ALL_EVENTS)

wd_g_ok = widget_button(ok_base,value='OK')
wd_g_cancel = widget_button(ok_base,value='Cancel')
index = -1

widget_control,prefbase,/REALIZE
xmanager,'get_view_print_pref',prefbase

return
end

