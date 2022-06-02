;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_choice_list_many.pro  $Revision: 1.69 $
pro get_choice_list_many_event,ev

;common get_choice_list_comm_many,num,strs,buttons,list,g_ok,len,ncol,wd_text,nonexclude,g_go_back,glengths,g_set_buttons,length, $
;    nbuttons,gstaggered,g_set_unset,g_set_diagonal,flip,g_sameforall,special1,g_exit1,set_unset1,g_set_same,g_special, $
;    g_set_order
;START161221
common get_choice_list_comm_many,num,buttons,list,g_ok,len,ncol,wd_text,nonexclude,g_go_back,glengths,g_set_buttons,length, $
    nbuttons,gstaggered,g_set_unset,g_set_diagonal,flip,g_sameforall,special1,g_exit1,set_unset1,g_set_same,g_special, $
    g_set_order,check0,identifiers0

if ev.id eq g_special then begin
    special1 = 'SPECIAL'
    widget_control,ev.top,/DESTROY
endif else if ev.id eq g_sameforall then begin
    special1 = 'okforall'
    widget_control,ev.top,/DESTROY
endif else if ev.id eq g_ok then begin

    ;widget_control,/HOURGLASS
    ;widget_control,ev.top,/DESTROY
    ;START161221
    if check0 eq 1 then begin
        for id=0,len-1 do begin
            if total(list[*,id]) eq 0 then begin
                scrap=get_button('Ok',BASE_TITLE='Error',TITLE='No selections made for '+identifiers0[id])
                goto,darby
            endif
        endfor
    endif 
    widget_control,/HOURGLASS
    widget_control,ev.top,/DESTROY
    darby:

endif else if ev.id eq g_go_back then begin
    special1 = 'GO_BACK'
    widget_control,ev.top,/DESTROY
endif else if ev.id eq g_exit1 then begin
    special1 = 'EXIT'
    widget_control,ev.top,/DESTROY
endif else if ev.id eq g_set_buttons then begin
    valid_frms = make_array(length,/INT,VALUE=1)
    tdim = max(glengths[0:nbuttons-1])
    rtn = get_frames(tdim,nrun,tdim_file,len_hrf,valid_frms,/KEEP,/LIMITS_ONLY)
    idx = where(valid_frms eq 0,count)
    if count ne 0 then begin
        if gstaggered eq 0 then $
            list[idx,0:nbuttons-1] = 1 $
        else begin 
            list[*,*] = 0
            for i=0,nbuttons-1 do list[idx[i],i] = 1
        endelse
        for id=0,len-1 do widget_control,buttons[id],SET_VALUE=list[0:glengths[id]-1,id]
    endif
endif else if ev.id eq g_set_unset then begin
    if set_unset1 eq 1 then begin
        flip=abs(flip - 1)
        sizelist=size(list)
        if sizelist[0] gt 1 then begin
            list[*,*]=flip
            for id=0,len-1 do widget_control,buttons[id],SET_VALUE=list[0:glengths[id]-1,id]
        endif else begin
            list[*]=flip
            for id=0,len-1 do widget_control,buttons[id],SET_VALUE=list
        endelse
    endif else if set_unset1 eq 2 then begin
        flip = abs(flip - 1)
        list[*] = flip
        for id=0,len-1 do widget_control,buttons[id],SET_VALUE=list[id]
    endif
endif else if ev.id eq g_set_diagonal then begin
    offset = fix(get_str(1,'offset','1'))
    sizelist = size(list)
    if sizelist[0] gt 1 then begin
        offset = offset-1
        list[*,*] = 0
        i = 0
        repeat begin
            list[offset,i] = 1
            i = i + 1
            offset = offset + 1
        endrep until i eq len or offset eq length
        for id=0,len-1 do widget_control,buttons[id],SET_VALUE=list[0:glengths[id]-1,id]
    endif else begin
        list[*] = 0
        i = 0
        repeat begin
            list[i] = offset 
            i = i + 1
            offset = offset + 1
        endrep until i eq len or offset eq length
        for id=0,len-1 do widget_control,buttons[id],SET_VALUE=list[id]
    endelse
endif else if ev.id eq g_set_order then begin
    scrap = get_str(1,'order','1',LABEL='Starts at 1.',/GO_BACK,WIDTH=50)
    if scrap[0] ne 'GO_BACK' then begin
        offset = fix(strsplit(scrap[0],'[ '+string(9B)+',]',/REGEX,/EXTRACT))
        sizelist = size(list)
        if sizelist[0] gt 1 then begin
            offset = offset-1
            list[*,*] = 0
            i = 0
            repeat begin
                if offset[i] lt sizelist[1] then list[offset[i],i] = 1
                i = i + 1
            endrep until i eq len or i eq n_elements(offset)
            for id=0,len-1 do widget_control,buttons[id],SET_VALUE=list[0:glengths[id]-1,id]
        endif else begin
            list[*] = 0
            i = 0
            repeat begin
                if offset[i] le sizelist[1] then list[i] = offset[i]
                i = i + 1
            endrep until i eq len or i eq n_elements(offset)
            for id=0,len-1 do widget_control,buttons[id],SET_VALUE=list[id]
        endelse
    endif
endif else if ev.id eq g_set_same then begin
    offset = fix(get_str(1,'set','1'))
    sizelist = size(list)
    if sizelist[0] gt 1 then begin
        list[*,*] = 0
        list[offset-1,*]=1
        for id=0,len-1 do widget_control,buttons[id],SET_VALUE=list[0:glengths[id]-1,id]
    endif else begin
        list[*] = 0
        list[*] = offset-1 
        for id=0,len-1 do widget_control,buttons[id],SET_VALUE=list[id]
    endelse
endif else if nonexclude eq 0 then begin
    for id=0,len-1 do begin
        if(ev.id eq buttons[id]) then begin
            list[id] = ev.value
            widget_control,buttons[id],SET_VALUE=list[id]
        endif
    endfor
endif else begin
    for id=0,len-1 do begin
        if ev.id eq buttons[id] then begin
            idx = ev.value
            if list[idx,id] eq 1 then $
                list[idx,id] = 0 $
            else $
                list[idx,id] = 1
            widget_control,buttons[id],SET_VALUE=list[0:glengths[id]-1,id]
        endif
    endfor
endelse
end

pro create_widgets_gclm,leader,top_title,title,labels,column_layout,labeltop,exclude,si,sl,sl_max,max_width,no_buttons_scroll, $
    text,identifiers,go_back,set_unset,set_diagonal,intbase,butbase,ydim,sameforall,exit1,set_same,special,set_order, $
    X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size 
    common get_choice_list_comm_many
    intbase = widget_base(/COLUMN,TITLE=top_title,GROUP_LEADER=leader,/MODAL)
    if title ne '' then stat = widget_label(intbase,VALUE=title)
    butbase = lonarr(len)
    buttons = lonarr(len)
    if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then begin
        if labeltop eq 1 then $
            butcol = widget_base(intbase,/ROW,SCR_XSIZE=(!DISPXMAX<x_scroll_size),SCR_YSIZE=(!DISPYMAX<y_scroll_size),/SCROLL) $
        else $
            butcol = widget_base(intbase,X_SCROLL_SIZE=(!DISPXMAX<x_scroll_size),XSIZE=x_scroll_size, $
                Y_SCROLL_SIZE=(!DISPYMAX<y_scroll_size),YSIZE=y_scroll_size,COLUMN=column_layout,ROW=labeltop)
    endif else begin
        butcol = widget_base(intbase,COLUMN=column_layout,ROW=labeltop) ;only one can be set
    endelse
    if labeltop ne 1 then butbase  = widget_base(butcol,/COLUMN,ALIGN_LEFT=labeltop,ALIGN_RIGHT=align_right)
    widget_control,/HOURGLASS
    for id=0,len-1 do begin
        if nonexclude eq 1 then set_value = list[0:glengths[id]-1,id] else set_value = list[id]
        if labeltop eq 1 then begin
            butbase[id]  = widget_base(butcol,/ROW,ALIGN_LEFT=labeltop,ALIGN_RIGHT=align_right)
            if(sl_max > si[id]) le max_width or no_buttons_scroll eq 1 then begin
                buttons[id] = cw_bgroup(butbase[id],labels[0:glengths[id]-1,id],/RETURN_INDEX,SET_VALUE=set_value,FRAME=2, $
                    NONEXCLUSIVE=nonexclude,EXCLUSIVE=exclude,COLUMN=1,LABEL_TOP=identifiers[id])
            endif else begin
                buttons[id] = cw_bgroup(butbase[id],labels[0:glengths[id]-1,id],/RETURN_INDEX,SET_VALUE=set_value,FRAME=2, $
                    NONEXCLUSIVE=nonexclude,EXCLUSIVE=exclude,COLUMN=1,LABEL_TOP=identifiers[id], $
                    X_SCROLL_SIZE=si[id]>sl_max,Y_SCROLL_SIZE=!DISPYMAX<ydim)
            endelse
        endif else begin
            buttons[id] = cw_bgroup(butbase,labels[0:glengths[id]-1,id],/RETURN_INDEX,SET_VALUE=set_value,FRAME=2, $
                NONEXCLUSIVE=nonexclude,EXCLUSIVE=exclude,ROW=1,LABEL_LEFT=identifiers[id])
        endelse
    endfor
    if text ne '' then wd_text = widget_text(intbase,/WRAP,/SCROLL,YSIZE=10,VALUE=text) else wd_text = 0L
    okbase  = widget_base(intbase,/ROW)
    g_ok = widget_button(okbase,VALUE='OK')
    if go_back eq 1 then g_go_back = widget_button(okbase,VALUE='Go back')
    if exit1 eq 1 then g_exit1 = widget_button(okbase,VALUE='Exit')
    if nbuttons ne 0 then g_set_buttons = widget_button(okbase,VALUE='SET BUTTONS')
    if set_unset ne 0 then g_set_unset = widget_button(okbase,VALUE='SET/UNSET')
    if set_diagonal ne 0 then g_set_diagonal = widget_button(okbase,VALUE='SET DIAGONAL')
    if set_order ne 0 then g_set_order = widget_button(okbase,VALUE='SET ORDER')
    if set_same ne 0 then g_set_same = widget_button(okbase,VALUE='SET')
    if sameforall ne 0 then g_sameforall = widget_button(okbase,VALUE='OK - REPEAT FOR ALL')
    if special ne '' then g_special = widget_button(okbase,VALUE=special)
    ;geo = widget_info(butbase,/GEOMETRY)
    ;print,'butbase START'
    ;print,'geo.scr_xsize=',geo.scr_xsize,' geo.scr_ysize=',geo.scr_ysize,' geo.margin=',geo.margin
    ;print,'geo.xsize=',geo.xsize,' geo.ysize=',geo.ysize,' geo.draw_xsize=',geo.draw_xsize,' geo.draw_ysize=',geo.draw_ysize
    ;print,'butbase END'
end

;function get_choice_list_many,identifiers,labels,TITLE=title,DEFAULT=default,GROUP_LEADER=group_leader,TEXT=text, $
;    LABEL_TOP=label_top,NONEXCLUSIVE=nonexclusive,GO_BACK=go_back,TOP_TITLE=top_title,COLUMN_LAYOUT=column_layout, $
;    LENGTHS=lengths,NO_BUTTONS_SCROLL=no_buttons_scroll,SET_BUTTONS=set_buttons,STAGGERED=staggered,SET_UNSET=set_unset, $
;    SET_DIAGONAL=set_diagonal,SAMEFORALL=sameforall,EXIT=exit,SET_SAME=set_same,SPECIAL=special,SET_ORDER=set_order
;START161221
function get_choice_list_many,identifiers,labels,TITLE=title,DEFAULT=default,GROUP_LEADER=group_leader,TEXT=text, $
    LABEL_TOP=label_top,NONEXCLUSIVE=nonexclusive,GO_BACK=go_back,TOP_TITLE=top_title,COLUMN_LAYOUT=column_layout, $
    LENGTHS=lengths,NO_BUTTONS_SCROLL=no_buttons_scroll,SET_BUTTONS=set_buttons,STAGGERED=staggered,SET_UNSET=set_unset, $
    SET_DIAGONAL=set_diagonal,SAMEFORALL=sameforall,EXIT=exit,SET_SAME=set_same,SPECIAL=special,SET_ORDER=set_order,CHECK=check

    common get_choice_list_comm_many
    if keyword_set(GROUP_LEADER) then leader = group_leader else leader = !FMRI_LEADER
    if keyword_set(TOP_TITLE) then top_title = top_title else top_title = '' 
    if keyword_set(TITLE) then title = title else title = ''
    if keyword_set(NONEXCLUSIVE) then nonexclude = 1 else nonexclude = 0
    if not keyword_set(NO_BUTTONS_SCROLL) then no_buttons_scroll = 0 
    if keyword_set(STAGGERED) then gstaggered = 1 else gstaggered = 0 
    if not keyword_set(NONEXCLUSIVE) then nonexclude = 0
    if not keyword_set(SAMEFORALL) then begin
        sameforall = 0
        g_sameforall = -1
    endif
    special1 = ''
    if not keyword_set(SPECIAL) then begin
        special=''
        g_special = -1
    endif
    exclude = 1 - nonexclude
    if not keyword_set(GO_BACK) then begin
        go_back = 0
        g_go_back = -1
    endif
    if not keyword_set(EXIT) then begin
        exit1 = 0
        g_exit1 = -1
    endif else $ 
        exit1 = 1
    if not keyword_set(COLUMN_LAYOUT) then column_layout = 0
    if not keyword_set(LABEL_TOP) then labeltop = 0 else labeltop = 1
    align_right = 1 - labeltop 
    if not keyword_set(TEXT) then text = ''
    if not keyword_set(SET_UNSET) then begin
       set_unset = 0
       g_set_unset = -1
    endif
    set_unset1 = set_unset
    flip = 0
    if not keyword_set(SET_DIAGONAL) then begin
       set_diagonal = 0
       g_set_diagonal = -1
    endif
    if not keyword_set(SET_ORDER) then begin
       set_order = 0
       g_set_order = -1
    endif
    if not keyword_set(SET_SAME) then begin
       set_same = 0
       g_set_same = -1
    endif

    ;START161221
    if keyword_set(CHECK) then check0=1 else check0=0
    

    len = n_elements(labels[0,*])
    length = n_elements(labels[*,0])
    if not keyword_set(LENGTHS) then begin
        lengths = intarr(len)
        lengths[*] = length
    endif
    glengths = lengths
    if nonexclude eq 1 then begin
        if keyword_set(DEFAULT) then list = default else list = intarr(length,len)
    endif else begin
        if Keyword_set(DEFAULT) then list = default else list = intarr(len)
    endelse
    
    ;print,'get_choice_list_many glengths=',glengths
    ;print,'size(glengths)=',size(glengths)
    ;print,'get_choice_list_many len=',len,' length=',length
    ;print,'get_choice_list_many list=',list
    ;print,'get_choice_list_many size(list)=',size(list)

    ;START161221
    identifiers0=identifiers
    
    si = strlen(identifiers)*12
    sl = strlen(labels[*,0])*12
    sl_max = max(sl)
    max_width = 375 
    if keyword_set(SET_BUTTONS) then begin
        nbuttons = set_buttons
    endif else begin
        g_set_buttons = -1
        nbuttons = 0
    endelse 
    ydim = length*30 
    create_widgets_gclm,leader,top_title,title,labels,column_layout,labeltop,exclude,si,sl,sl_max,max_width,no_buttons_scroll,text, $
        identifiers,go_back,set_unset,set_diagonal,intbase,butbase,ydim,sameforall,exit1,set_same,special,set_order
        geo = widget_info(intbase,/GEOMETRY)
    xdim = geo.scr_xsize + 2*geo.margin
    ydim = geo.scr_ysize + 2*geo.margin
    ;print,'geo.scr_xsize=',geo.scr_xsize,' geo.scr_ysize=',geo.scr_ysize,' geo.margin=',geo.margin
    ;print,'geo.xsize=',geo.xsize,' geo.ysize=',geo.ysize,' geo.draw_xsize=',geo.draw_xsize,' geo.draw_ysize=',geo.draw_ysize
    ;print,'!DISPXMAX=',!DISPXMAX,' !DISPYMAX=',!DISPYMAX
    ;print,'xdim=',xdim,' ydim=',ydim
    if ydim eq 0 then begin
        geo = widget_info(butbase,/GEOMETRY)
        ydim = total(geo.scr_ysize + 2*geo.margin)
    endif
    if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
        widget_control,intbase,/DESTROY
        create_widgets_gclm,leader,top_title,title,labels,column_layout,labeltop,exclude,si,sl,sl_max,max_width,no_buttons_scroll, $
            text,identifiers,go_back,set_unset,set_diagonal,intbase,butbase,ydim,sameforall,exit1,set_same,special,set_order, $
            X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim
    
    endif
    widget_control,intbase,/REALIZE
    widget_control,g_ok,/INPUT_FOCUS
    widget_control,intbase,/SHOW
    xmanager,'get_choice_list_many',intbase
    ;print,'get_choice_list_many bottom'
    return,rtn={list:list,special:special1}
end
