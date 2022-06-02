;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_bool_list.pro  $Revision: 12.108 $
pro get_bool_list_event,ev

    ;common get_bool_list_comm,num,strs,buttons,list,g_ok,len,ncol,g_go_back,g_exit,number,gspecial,g_special,special_out, $
    ;    g_set,unset_set
    ;START150422
    common get_bool_list_comm,num,strs,buttons,list,g_ok,len,ncol,g_go_back,g_exit,number,gspecial,g_special,special_out, $
        g_set,unset_set,gs2,g_s2,s2_out

    if ev.id eq g_special then begin
        special_out = 1
        widget_control,ev.top,/DESTROY

    ;START150422
    endif else if ev.id eq g_s2 then begin
        s2_out = 1
        widget_control,ev.top,/DESTROY

    endif else if ev.id eq g_ok then begin
        widget_control,ev.top,/DESTROY
    endif else if ev.id eq g_go_back then begin
        list[0] = -1
        widget_control,ev.top,/DESTROY
    endif else if ev.id eq g_exit then begin
        list[0] = -2
        widget_control,ev.top,/DESTROY
    endif else if ev.id eq g_set then begin
        unset_set = abs(unset_set-1)
        list[*] = unset_set
        ilab=0
        for col=0,ncol-1 do begin
            dlab = len - ilab
            if dlab gt number then dlab = number
            widget_control,buttons[col],SET_VALUE=list[ilab:ilab+dlab-1]
            ilab = ilab + number
        endfor
    
    endif else begin
        for col=0,ncol-1 do begin
            if ev.id eq buttons[col] then begin
                idx = ev.value + col*number
                if list[idx] eq 1 then list[idx] = 0 else list[idx] = 1
                icol = col*number
                ecol = len - icol - 1
                if ecol gt number-1 then ecol = number - 1
                widget_control,buttons[col],SET_VALUE=list[icol:icol+ecol]
            endif
        endfor
    endelse
end
function create_widgets_gbl,labels,leader,title,one_column,base_title,align_left,X_SCROLL_SIZE=x_scroll_size, $
    Y_SCROLL_SIZE=y_scroll_size
    common get_bool_list_comm
    if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then $ 
        rtn = get_base(labels,leader,align_left,TITLE=title,ONE_COLUMN=one_column,BASE_TITLE=base_title,/WIDTH, $
            X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size) $
    else $ 
        rtn = get_base(labels,leader,align_left,TITLE=title,ONE_COLUMN=one_column,BASE_TITLE=base_title,/WIDTH)
    len = rtn.len
    ncol = rtn.ncol
    buttons = rtn.buttons
    intbase = rtn.intbase
    butbase = rtn.butbase
    okbase = rtn.okbase
    number = rtn.number
    list = intarr(len)
    ilab = 0
    for col=0,ncol-1 do begin
        dlab = len - ilab
        if dlab gt number then dlab = number
        buttons[col] = cw_bgroup(butbase,labels[ilab:ilab+dlab-1],EVENT_FUNCT=get_but_event,/RETURN_INDEX,/NONEXCLUSIVE, $
            SET_VALUE=list[ilab:ilab+dlab-1])
        ilab = ilab + number
    endfor
    if g_go_back+g_exit eq -2 then begin
        g_ok = widget_button(okbase,value='Ok',XSIZE=60)
    endif else begin
        g_ok = widget_button(okbase,value='Ok',XSIZE=100)
        if g_go_back gt -1 then g_go_back = widget_button(okbase,value='Go back')
        if g_exit gt -1 then g_exit = widget_button(okbase,value='Exit',XSIZE=100)
    endelse
    g_set = widget_button(okbase,value='Set/unset')
    if gspecial ne '' then g_special = widget_button(okbase,value=gspecial)
    if gs2 ne '' then g_s2 = widget_button(okbase,value=gs2)
    return,intbase
end
function get_bool_list,labels,TITLE=title,DEFAULT=default,GROUP_LEADER=group_leader,GO_BACK=go_back,EXIT=exit, $
    ONE_COLUMN=one_column,BASE_TITLE=base_title,SPECIAL=special,S2=s2,ALIGN_LEFT=align_left
    common get_bool_list_comm
    if keyword_set(GROUP_LEADER) then leader = group_leader else leader = !FMRI_LEADER
    if not keyword_set(ONE_COLUMN) then one_column = 0 
    if not keyword_set(BASE_TITLE) then base_title = '' 
    if not keyword_set(TITLE) then title = 0 
    if not keyword_set(GO_BACK) then g_go_back = -1 else g_go_back = 1
    if not keyword_set(EXIT) then g_exit = -1 else g_exit = 1
    if not keyword_set(SPECIAL) then begin
        gspecial = ''
        g_special = -1
    endif else begin
        gspecial = special
        g_special = 1
    endelse
    special_out = 0
    if not keyword_set(S2) then begin
        gs2 = ''
        g_s2 = -1
    endif else begin
        gs2 = s2 
        g_s2 = 1
    endelse
    s2_out = 0
    if not keyword_set(ALIGN_LEFT) then align_left=0
    unset_set = 0


    ;widget_control,/HOURGLASS
    ;intbase = create_widgets_gbl(labels,leader,title,one_column,base_title,align_left)
    ;geo = widget_info(intbase,/GEOMETRY)
    ;xdim = geo.scr_xsize + 2*geo.margin
    ;ydim = geo.scr_ysize + 2*geo.margin
    ;if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
    ;    widget_control,intbase,/DESTROY
    ;    widget_control,/HOURGLASS
    ;    intbase = create_widgets_gbl(labels,leader,title,one_column,base_title,align_left,X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim)
    ;endif
    ;geo = widget_info(intbase,/GEOMETRY)
    ;xdim = geo.scr_xsize + 2*geo.margin
    ;ydim = geo.scr_ysize + 2*geo.margin
    ;if ydim lt n_elements(labels) then begin
    ;    widget_control,intbase,/DESTROY
    ;    lizard=get_button(['go back','exit'],TITLE='Too many files to display',BASE_TITLE='Big problem')
    ;    if lizard eq 0 then list[0]=-1 else list[0]=-2
    ;endif else begin
    ;    widget_control,intbase,/REALIZE
    ;    widget_control,g_ok,/INPUT_FOCUS
    ;    xmanager,'get_bool_list',intbase
    ;endelse
    ;START211009
    superbird=1000
    if n_elements(labels) le superbird then begin
        widget_control,/HOURGLASS
        intbase = create_widgets_gbl(labels,leader,title,one_column,base_title,align_left)
        geo = widget_info(intbase,/GEOMETRY)
        xdim = geo.scr_xsize + 2*geo.margin
        ydim = geo.scr_ysize + 2*geo.margin
        if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
            widget_control,intbase,/DESTROY
            widget_control,/HOURGLASS
            intbase = create_widgets_gbl(labels,leader,title,one_column,base_title,align_left,X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim)
        endif
        geo = widget_info(intbase,/GEOMETRY)
        xdim = geo.scr_xsize + 2*geo.margin
        ydim = geo.scr_ysize + 2*geo.margin
        if ydim lt n_elements(labels) then begin
            widget_control,intbase,/DESTROY
            lizard=get_button(['go back','exit'],TITLE='Too many files to display',BASE_TITLE='Big problem')
            if lizard eq 0 then list[0]=-1 else list[0]=-2
        endif else begin
            widget_control,intbase,/REALIZE
            widget_control,g_ok,/INPUT_FOCUS
            xmanager,'get_bool_list',intbase
        endelse
    endif else begin
        lizard=get_button(['ok','go back','exit'],TITLE='More than 1000 files. Too many too display.'+string(10B)+'We will use all '+trim(n_elements(labels)-1)+'.', $
            BASE_TITLE='Big problem')
        if lizard eq 1 then list[0]=-1 else if lizard eq 2 then list[0]=-2 else begin
            print,'get_bool list here0'
            list=intarr(n_elements(labels))
            list[*]=1
        endelse
    endelse



    return,rtn={list:list,special:special_out,s2:s2_out}
end
