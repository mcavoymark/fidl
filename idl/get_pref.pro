;Copyright 9/27/00 Washington University.  All Rights Reserved.
;get_pref.pro  $Revision: 12.95 $
pro get_pref_event,ev
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

;common pref_comm,g_ok,glmbut,orient_but,orient_hl_but,wd_printer_name,printer_list,np,wd_current_printer,concpath,fidlpath,paraskip, $
;    g_rpath,chunk_but
;START170224
common pref_comm,g_ok,glmbut,orient_but,orient_hl_but,wd_printer_name,printer_list,np,wd_current_printer,concpath,fidlpath,paraskip, $
    g_rpath,chunk_but,cpu,period

case ev.id of
    wd_printer_name: begin
        if(ev.value lt np+1) then begin
            pref.printer_name = printer_list[ev.value-1]
        endif else begin
            str = get_str(1,'Printer name',!PRINTER) 
            pref.printer_name = str[0]
        endelse
        str = string(pref.printer_name,FORMAT='("Current printer: ",a)')
        widget_control,wd_current_printer,SET_VALUE=str
    end

    ;START4
    ;wilcbut: begin
    ;    case ev.value of
    ;       0: pref.wilc_sig = ev.select
    ;       1: pref.wilc_prob = ev.select
    ;       2: pref.wilc_z = ev.select
    ;       else: print,'Invalid button from cw_bgroup.'
    ;    endcase
    ;end

    glmbut: begin
        case ev.value of
	    0: pref.glm_z = ev.select
	    1: pref.glm_mag = ev.select
	    2: pref.glm_var = ev.select
            3: pref.glm_t = ev.select
 	    else: print,'Invalid button from cw_bgroup.'
	endcase
    end

    ;START4
    ;diffbut: begin
    ;    case ev.value of
    ;       0: pref.diff_diff = ev.select
    ;       1: pref.diff_pct = ev.select
    ;       2: pref.diff_z = ev.select
    ;       3: pref.diff_var = ev.select
    ; 	    else: print,'Invalid button from cw_bgroup.'
    ;    endcase
    ;end
    ;slopbut: begin
    ;    case ev.value of
    ;       0: pref.slope = ev.select
    ;       1: pref.intercept = ev.select
    ; 	    else: print,'Invalid button from cw_bgroup.'
    ;   endcase
    ;end



    orient_but: begin
        case ev.value of
	    0: pref.orientation = !RADIOL
	    1: pref.orientation = !NEURO
 	    else: print,'Invalid button for orientation.'
	endcase
    end
    orient_hl_but: pref.orientation_hide_label = ev.select 
    paraskip: pref.paraskip = ev.value

    ;parainit: pref.parainit = ev.value
    ;paraact: pref.paraact = ev.value
    ;paractl: pref.paractl = ev.value
    period: pref.period = ev.value

    concpath: pref.concpath = ev.value
    fidlpath: pref.fidlpath = ev.value
    chunk_but: pref.chunk=1-ev.value

    ;START170224
    cpu:pref.cpu=ev.value


    g_ok: widget_control,ev.top,/DESTROY
    g_rpath:get_rpath
    else: begin
        eff = -1
    end
endcase
end

pro get_pref
    common pref_comm
    common stats_comm
    prefbase = widget_base(/COLUMN,GROUP_LEADER=!FMRI_LEADER)
    printer_base = widget_base(prefbase,/ROW)
    glmbase = widget_base(prefbase,/ROW)
    orientbase = widget_base(prefbase,/ROW)
    concbase = widget_base(prefbase,/ROW)
    fidlbase = widget_base(prefbase,/ROW)
    chunkbase = widget_base(prefbase,/ROW)
    title = widget_label(printer_base,VALUE='Printer name:',/ALIGN_LEFT)
    printer_list = get_printer_list() 
    np = n_elements(printer_list)
    if(np gt 0) then begin
        labels = strarr(np+2)
        labels[0] = '1\Printers'
        for i=1,np do begin
            labels[i] = string(printer_list[i-1],FORMAT='("0\",a)')
        endfor
        labels[np+1] = '0\Other'
        wd_printer_name = cw_pdmenu(printer_base,labels,/RETURN_INDEX)
    endif
    str = string(pref.printer_name,FORMAT='("Current printer:  ",a)')
    wd_current_printer = widget_label(printer_base,VALUE=str,/ALIGN_LEFT,XSIZE=450)
    labels = ['Z-statistic','Magnitude','Variance','T-statistic']
    val = [pref.glm_z,pref.glm_mag,pref.glm_var,pref.glm_t]
    glmbut = cw_bgroup(glmbase,labels,EVENT_FUNCT=get_pref_event,LABEL_LEFT='Linear Model',/NONEXCLUSIVE,ROW=1,SET_VALUE=val)
    labels = ['Radiological','Neuropsych']
    if pref.orientation eq !NEURO then val = 1 else val = 0
    orient_but = cw_bgroup(orientbase,labels,EVENT_FUNCT=get_pref_event,LABEL_LEFT='Orientation:',/EXCLUSIVE,ROW=1, $
        SET_VALUE=val,/NO_RELEASE)
    orient_hl_butbase = widget_base(orientbase,/ROW,/NONEXCLUSIVE)
    orient_hl_but = widget_button(orient_hl_butbase,VALUE='Hide label')
    paraskip = cw_field(orientbase,/INTEGER,/ROW,TITLE="Skip:",VALUE=pref.paraskip,XSIZE=3,/ALL_EVENTS)
 
    ;START170224
    period=cw_field(orientbase,/INTEGER,/ROW,TITLE="HRF period (TRs):",VALUE=pref.period,XSIZE=3,/ALL_EVENTS)


    concpath = cw_field(concbase,/ROW,TITLE="conc path:",VALUE=pref.concpath,XSIZE=100,/ALL_EVENTS)
    fidlpath = cw_field(fidlbase,/ROW,TITLE="fidl path:",VALUE=pref.fidlpath,XSIZE=100,/ALL_EVENTS)
    chunk_but = cw_bgroup(chunkbase,['on','off'],EVENT_FUNCT=get_pref_event,LABEL_LEFT='chunk:',/EXCLUSIVE,ROW=1, $
        SET_VALUE=1-pref.chunk,/NO_RELEASE)

    ;START170224
    cpu=cw_field(chunkbase,/ROW,TITLE="percent of cpu:",VALUE=pref.cpu,XSIZE=3,/ALL_EVENTS)

    okbase = widget_base(prefbase,/ROW)
    g_ok = widget_button(okbase,value='Ok')
    g_rpath=widget_button(okbase,value='R path')
    index = -1
    widget_control,prefbase,/REALIZE
    if pref.orientation_hide_label eq 1 then widget_control,orient_hl_but,/SET_BUTTON
    xmanager,'get_pref',prefbase
    ;help,pref,/STRUCTURE
end
