;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_str_bool.pro  $Revision: 12.91 $
pro get_str_bool_event,ev

    ;common get_strbool_comm,num,strs,intbase,g_ok,wdstr,nstr,wbool_lab,g_text_box_below,g_special
    ;START160715
    common get_strbool_comm,num,strs,intbase,g_ok,wdstr,nstr,wbool_lab,g_text_box_below,g_special,g_goback,g_exit

    case ev.id of
        g_ok: widget_control,ev.top,/DESTROY
        wbool_lab: begin
            strs[nstr] = string(ev.value)
            vals = intarr(3)
            vals[ev.value] = 1
            widget_control,wbool_lab,SET_VALUE=ev.value
        end
        g_special: begin
            strs[0]='SPECIAL'
            widget_control,ev.top,/DESTROY
        end

        ;START160715
        g_goback:begin
            strs[0]='GOBACK'
            widget_control,ev.top,/DESTROY
        end
        g_exit:begin
            strs[0]='EXIT'
            widget_control,ev.top,/DESTROY
        end

        else: begin
            tst = 0
            for istr=0,nstr-1 do begin
                if ev.id eq wdstr[istr] then begin
                    tst = 1
                    if g_text_box_below eq 0 then $
    	                strs[istr] = ev.value $
                    else begin
                        widget_control,ev.id,GET_VALUE=scrap ;GET_VALUE=strs[istr] bombs
                        strs[istr] = scrap
                    endelse
                endif
            endfor
            if(tst eq 0) then begin
                stat=widget_message('Invalid event in get_str_bool_event',/ERROR)
            endif
        endelse
    endcase
end

;function get_str_bool,number,labels,bool_labels,values,bool_dflt,WIDTH=width,TITLE=title,GROUP_LEADER=group_leader, $
;    BOOL_TITLE=bool_title,TEXT_BOX_BELOW=text_box_below,LABEL=label
;START160128
function get_str_bool,number,labels,bool_labels,values,bool_dflt,WIDTH=width,TITLE=title,GROUP_LEADER=group_leader, $
    BOOL_TITLE=bool_title,TEXT_BOX_BELOW=text_box_below,LABEL=label,SPECIAL=special

    common get_strbool_comm
    if(not keyword_set(BOOL_TITLE)) then $
        bool_title = ''
    if(keyword_set(GROUP_LEADER)) then $
        leader = group_leader $
    else $
        leader = !FMRI_LEADER
    nstr = number
    strs = strarr(nstr+1)
    if(keyword_set(WIDTH)) then $
        xsize = width $
    else $
        xsize = 15
    if(keyword_set(TITLE)) then $
        title = title $
    else $
        title = ' '
    maxlen = 0
    for i=0,nstr-1 do begin
        strs(i) = values(i)
        len = strlen(strs(i))
        if(len gt maxlen) then $
            maxlen = len
    endfor
    strs[nstr] = string(bool_dflt)
    if(maxlen gt 15) then $
        xsize = maxlen
    
    intbase = widget_base(/COLUMN,TITLE=title,GROUP_LEADER=leader,/MODAL)
    
    if keyword_set(LABEL) then labelbase = widget_label(intbase,VALUE=label)
        
    ;if nstr*xsize gt 800 then $
    ;    butcol = widget_base(intbase,/ROW,SCR_XSIZE=800,SCR_YSIZE=600,/SCROLL) $
    ;else $
    ;    butcol = widget_base(intbase,/ROW)
    ;START160129
    butcol = widget_base(intbase,/ROW)


    boolbase = widget_base(intbase,/ROW)
    okbase  = widget_base(intbase,/ROW)
    ncols = fix((nstr+1)/15) + 1
    butbase = lonarr(ncols)
    for icol=0,ncols-1 do butbase[icol] = widget_base(butcol,/COLUMN,FRAME=2)
    wdstr = intarr(nstr)
    if keyword_set(TEXT_BOX_BELOW) then begin 
        wdstr_label = intarr(nstr)
        g_text_box_below = 1
    endif else $
        g_text_box_below = 0
    
    for i=0,nstr-1 do begin
        icol = i/15
        if not keyword_set(TEXT_BOX_BELOW) then $
            wdstr[i] = cw_field(butbase[icol],TITLE=labels[i],VALUE=values[i],XSIZE=xsize,/ALL_EVENTS) $
        else begin
            wdstr_label[i] = widget_label(butbase[icol],VALUE=labels[i])

            ;wdstr[i] = widget_text(butbase[icol],XSIZE=100,YSIZE=10,/ALL_EVENTS,/EDITABLE,/WRAP,/SCROLL)
            ;START160129
            ;if n_elements(values) eq 0 then value=0 else begin
            ;    if values[i] eq '' then value=0 else value=values[i]
            ;endelse
            ;wdstr[i] = widget_text(butbase[icol],XSIZE=100,YSIZE=10,/ALL_EVENTS,/EDITABLE,/WRAP,/SCROLL,VALUE=value)
            ;START160129
            wdstr[i] = widget_text(butbase[icol],XSIZE=100,YSIZE=10,/ALL_EVENTS,/EDITABLE,/WRAP,/SCROLL,VALUE=values[i])

        endelse
    endfor
    
    boolbase = widget_base(intbase,/ROW,FRAME=2)
    if(keyword_set(TITLE)) then stat = widget_label(boolbase,VALUE=bool_title)
    nbool = n_elements(bool_labels)
    xbool_dflt = intarr(nbool)
    xbool_dflt[bool_dflt] = 1
    wbool_lab = cw_bgroup(boolbase,bool_labels,/RETURN_INDEX,/EXCLUSIVE,SET_VALUE=bool_dflt,COLUMN=1)
    okbase = widget_base(intbase,/ROW)

    ;g_ok = widget_button(okbase,value='OK')
    ;START160715
    g_ok=widget_button(okbase,value='Ok')
    g_goback=widget_button(okbase,value='Go back')
    g_exit=widget_button(okbase,value='Exit')

    if keyword_set(SPECIAL) then g_special=widget_button(okbase,value=special) else g_special=-1
    widget_control,intbase,/REALIZE
    widget_control,g_ok,/INPUT_FOCUS
    xmanager,'get_str_bool',intbase
    return,strs
end
