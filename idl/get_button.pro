;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_button.pro  $Revision: 12.97 $
pro get_button_event,ev
    common getbut_comm,ncol,strs,intbase,buttons,index,len,number,wd_ok,wd_cancel,g_default
    if wd_ok eq -1 then begin
        for col=0,ncol-1 do begin
            if ev.id eq buttons[col] then begin
                index = ev.value + col*number
                widget_control,ev.top,/DESTROY
            endif
        endfor
    endif else begin
        for col=0,ncol-1 do begin
            if ev.id eq buttons[col] then begin
                index = ev.value + col*number
                ;widget_control,ev.top,SET_VALUE=index
            endif
        endfor
        if ev.id eq wd_ok then widget_control,ev.top,/DESTROY
        if ev.id eq wd_cancel then begin
            index = g_default
            widget_control,ev.top,/DESTROY
        endif
    endelse
end

;function get_button,labels,TITLE=title,GROUP_LEADER=group_leader,DEFAULT=default,OK=ok,BASE_TITLE=base_title, $
;    ONE_COLUMN=one_column,WIDTH=width 
;START150429
function get_button,labels,TITLE=title,GROUP_LEADER=group_leader,DEFAULT=default,OK=ok,BASE_TITLE=base_title, $
    ONE_COLUMN=one_column,WIDTH=width,ALIGN_LEFT=align_left 

    common getbut_comm
    if not keyword_set(BASE_TITLE) then base_title = ''
    if not keyword_set(GROUP_LEADER) then group_leader = !FMRI_LEADER

    ;START150429
    if not keyword_set(ALIGN_LEFT) then align_left=0

    intbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=group_leader,TITLE=base_title)
    if keyword_set(TITLE) then begin

        ;stat = widget_label(intbase,VALUE=title)
        ;START150429
        stat = widget_label(intbase,VALUE=title,ALIGN_LEFT=align_left)
        geo = widget_info(stat,/GEOMETRY)
        xdim = geo.scr_xsize + 2*geo.margin
        ydim = geo.scr_ysize + 2*geo.margin
        if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
            widget_control,stat,/DESTROY
        
            ;intbase1 = widget_base(intbase,/ROW,TITLE=title,X_SCROLL_SIZE=!DISPXMAX<xdim,XSIZE=xdim, $
            ;    Y_SCROLL_SIZE=!DISPYMAX<ydim,YSIZE=ydim)
            ;stat = widget_label(intbase1,VALUE=title)
            ;START150429
            intbase1 = widget_base(intbase,/ROW,X_SCROLL_SIZE=!DISPXMAX<xdim,XSIZE=xdim,Y_SCROLL_SIZE=!DISPYMAX<ydim,YSIZE=ydim)
            stat = widget_label(intbase1,VALUE=title,ALIGN_LEFT=align_left)
        
        endif
    endif
    len = n_elements(labels)
    if not keyword_set(ONE_COLUMN) then begin
        ncol = fix((len-1)/25) + 1
        number = 25
    endif else begin
        ncol = 1
        number = len
    endelse
    buttons = lonarr(ncol)
    butbase = widget_base(intbase,/ROW)
    ilab = 0
    for col=0,ncol-1 do begin
        dlab = len - ilab
        if dlab gt number then dlab = number
        if not keyword_set(DEFAULT) then $
            buttons[col] = cw_bgroup(butbase,labels[ilab:ilab+dlab-1],/RETURN_INDEX,/EXCLUSIVE) $
        else $
            buttons[col] = cw_bgroup(butbase,labels[ilab:ilab+dlab-1],/RETURN_INDEX,/EXCLUSIVE,SET_VALUE=default)
        ilab = ilab + dlab
    endfor
    geo = widget_info(butbase,/GEOMETRY)
    xdim = geo.scr_xsize + 2*geo.margin
    ydim = geo.scr_ysize + 2*geo.margin
    if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
        widget_control,butbase,/DESTROY
        butbase = widget_base(intbase,/ROW,X_SCROLL_SIZE=!DISPXMAX<xdim,XSIZE=xdim,Y_SCROLL_SIZE=!DISPYMAX<ydim,YSIZE=ydim)
        ilab = 0
        for col=0,ncol-1 do begin
            dlab = len - ilab
            if dlab gt number then dlab = number
            if not keyword_set(DEFAULT) then $
                buttons[col] = cw_bgroup(butbase,labels[ilab:ilab+dlab-1],/RETURN_INDEX,/EXCLUSIVE) $
            else $
                buttons[col] = cw_bgroup(butbase,labels[ilab:ilab+dlab-1],/RETURN_INDEX,/EXCLUSIVE,SET_VALUE=default)
            ilab = ilab + dlab
        endfor
    endif
    if not keyword_set(OK) then begin
        wd_ok = -1
        wd_cancel = -1
        g_default = -1
    endif else begin
        okbase  = widget_base(intbase,/ROW)
        wd_ok = widget_button(okbase,value='OK')
        wd_cancel = widget_button(okbase,value='Cancel')
        g_default = default
    endelse
    widget_control,intbase,/REALIZE
    check_math_errors,!FALSE
    xmanager,'get_button',intbase
    return,index
end
