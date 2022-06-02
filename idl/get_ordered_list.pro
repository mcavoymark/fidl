;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_ordered_list.pro  $Revision: 1.19 $
pro get_ordered_list_event,ev
common get_ordered_comm,num,strs,intbase,g_ok,wdstr,nbutton,wdbut,idx_list,label_list,iselect,order_labels,button_labels, $
    norder,wdprompt,max_id_size,g_consec,g_rmpath,button_labels2
if ev.id eq g_rmpath then begin
    widget_control,wdbut[0],GET_VALUE=str
    if str[0] eq button_labels[0] then goose = button_labels2 else goose = button_labels 
    for i=0,nbutton-1 do widget_control,wdbut[i],SET_VALUE=goose[i]
endif else if ev.id eq g_consec then begin
    for ibase=0,nbutton-1 do begin
        widget_control,wdstr[ibase],GET_VALUE=str
        ;print,'ibase=',ibase,' str[0]=',str[0],'END'
        if str[0] ne '' then begin
            for j=0,nbutton-1 do begin
                widget_control,wdbut[j],SET_BUTTON=0
                widget_control,wdstr[j],SET_VALUE=''
                idx_list[j]=0
            endfor
            goto,outofhere
        endif
    endfor
    if str[0] eq '' then begin
        for ibase=0,nbutton-1 do begin
            widget_control,wdbut[ibase],/SET_BUTTON
            widget_control,wdstr[ibase],SET_VALUE=order_labels[ibase]
            idx_list[ibase] = ibase+1
        endfor
    endif
    outofhere:
endif else if ev.id eq g_ok then begin
    for ibase=0,nbutton-1 do begin
        widget_control,wdstr[ibase],GET_VALUE=str
        order_labels[ibase]=str
    endfor
    order_labels = strtrim(order_labels[0:nbutton-1],2)
    index = where(order_labels eq '',count)
    if count ne 0 then order_labels[index]=0
    if total(idx_list) eq 0 then begin
        if dialog_message('You forgot to check the boxes.'+string(10B)+'Hit OK to check the boxes.',/ERROR,/CANCEL) eq 'OK' then $
            goto,THE_END
    endif
    order_labels = fix(order_labels)
    widget_control,ev.top,/DESTROY
endif else if ev.id eq wdprompt  then begin
    widget_control,ev.id,GET_VALUE=str
    str = strcompress(str[0],/REMOVE_ALL)
    idx = strmatch(order_labels,str[0],/FOLD_CASE)
    iselect = long(str[0])
    j = where(idx)
    iselect = j[0]
    widget_control,wdprompt,SET_VALUE=order_labels[iselect]
endif else begin
    for ibase=0,nbutton-1 do begin
        if ev.id eq wdstr[ibase] then begin
            tst = 1
            widget_control,ev.id,GET_VALUE=str
            str = strcompress(str[0],/REMOVE_ALL)
            idx = strmatch(order_labels,str[0],/FOLD_CASE)
            if total(idx) eq 1 then begin
                j = where(idx)
                jselect = j[0]
	        label_list[ibase] = order_labels[j]
                idx_list[ibase] = jselect + 1
                widget_control,wdstr[ibase],SET_VALUE=label_list[ibase]
                widget_control,wdbut[ibase],SET_BUTTON=string(idx_list[ibase])
                if iselect lt jselect+1 then  iselect = jselect + 1
                widget_control,wdprompt,SET_VALUE=order_labels[iselect]
            endif else begin
                widget_control,wdstr[ibase],SET_VALUE='X'
            endelse
            goto, JUMP
        endif
        if ev.id eq wdbut[ibase] then begin

            ;START130710
            iselect = ibase

            if idx_list[ibase] ne 0 then begin
                idx_list[ibase] = 0

                ;START130710
                ;iselect = iselect - 1

                label_list[ibase] = ''
            endif else begin
                if iselect ge norder then begin
                    stat = dialog_message('All labels have been assigned.')
                    goto,THE_END
                endif
                label_list[ibase] = order_labels[iselect]
                idx_list[ibase] = iselect+1
                iselect = iselect + 1
            endelse
            widget_control,wdstr[ibase],SET_VALUE=label_list[ibase]
            widget_control,wdbut[ibase],SET_BUTTON=string(idx_list[ibase])
            if iselect lt n_elements(order_labels) then $
                widget_control,wdprompt,SET_VALUE=order_labels[iselect] $
            else $
                widget_control,wdprompt,SET_VALUE='All used'
            tst = 1
            goto, JUMP
        endif
    endfor
JUMP:if tst eq 0 then begin
        stat=widget_message('Invalid event in get_ordered_list_event',/ERROR)
     endif
endelse
THE_END:
end

pro create_order_widgets,butcol,leader,title,below,X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size
    common get_ordered_comm
    intbase = widget_base(/COLUMN,TITLE=title,GROUP_LEADER=leader,/MODAL)
    if keyword_set(X_SCROLL_SIZE) then begin
        if nbutton lt 15 then $
            butcol = widget_base(intbase,/ROW,X_SCROLL_SIZE=!DISPXMAX,XSIZE=x_scroll_size,Y_SCROLL_SIZE=nbutton*45) $
        else $
            butcol = widget_base(intbase,/ROW,X_SCROLL_SIZE=!DISPXMAX,XSIZE=x_scroll_size,Y_SCROLL_SIZE=!DISPYMAX)
    endif else $
        butcol = widget_base(intbase,/ROW)
    if below gt 0 then begin
        ncols = 1
        icol = intarr(nbutton)
    endif else begin
        ncols = fix((nbutton+1)/15) + 1 
        icol = indgen(nbutton)/15
    endelse
    wdbut = lonarr(nbutton)
    butbase = lonarr(ncols)
    for i=0,ncols-1 do butbase[i] = widget_base(butcol,/COLUMN,SPACE=0)
    wdstr = intarr(nbutton)
    for i=0,nbutton-1 do begin
        line_base = widget_base(butbase[icol[i]],/ROW,SPACE=0)
        strbase = widget_base(line_base,/ROW,SPACE=0)
        bbase = widget_base(line_base,/ROW,/NONEXCLUSIVE)
        wdstr[i] = widget_text(strbase,VALUE='',XSIZE=max_id_size,/EDITABLE)
        wdbut[i] = widget_button(bbase,VALUE=button_labels[i])
    endfor
    okbase  = widget_base(intbase,/ROW)
    promptbase  = widget_base(intbase,/ROW)
    wdprompt_label = widget_label(promptbase,VALUE='Next label:')
    wdprompt = widget_text(promptbase,VALUE=order_labels[0],XSIZE=10,/EDITABLE)
    g_ok = widget_button(okbase,value='Ok')
    g_consec = widget_button(okbase,value='Set/unset')

    ;g_rmpath = widget_button(okbase,value='Remove/add paths')
    ;START130917
    if g_rmpath ne -1 then g_rmpath = widget_button(okbase,value='Remove/add paths')

end

;function get_ordered_list,but_labels,ord_labels,TITLE=title,GROUP_LEADER=group_leader,ALIGN_RIGHT=align_right,BELOW=below
;START130917
function get_ordered_list,but_labels,ord_labels,TITLE=title,GROUP_LEADER=group_leader,ALIGN_RIGHT=align_right,BELOW=below, $
    NO_RMPATH=no_rmpath

    common get_ordered_comm
    if not keyword_set(BELOW) then below=0
    if keyword_set(GROUP_LEADER) then leader = group_leader else leader = !FMRI_LEADER

    if keyword_set(NO_RMPATH) then g_rmpath=-1 else g_rmpath=0

    order_labels = strtrim(ord_labels,2)

    ;max_id_size = ceil(n_elements(order_labels)/10.)
    ;START150127
    max_id_size = max(strlen(order_labels))
    
    button_labels = but_labels
    rtn = get_root(button_labels)
    button_labels2 = rtn.file 
    nbutton = n_elements(button_labels)
    norder = n_elements(order_labels)
    label_list = strarr(nbutton)
    idx_list = lonarr(nbutton)
    iselect = 0
    if keyword_set(TITLE) then title = title else title = ''
    create_order_widgets,butcol,leader,title,below
    geo = widget_info(intbase,/GEOMETRY)
    xdim = geo.scr_xsize + 2*geo.margin
    ydim = geo.scr_ysize + 2*geo.margin
    if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
        widget_control,intbase,/DESTROY
        create_order_widgets,butcol,leader,title,below,X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim
    endif
    widget_control,intbase,/REALIZE
    widget_control,g_ok,/INPUT_FOCUS
    widget_control,intbase,/SHOW
    xmanager,'get_ordered_list',intbase
    idx1 = where(idx_list,count)
    if count ne 0 then begin

        ;print,'idx_list=',idx_list
        ;print,'idx1=',idx1
        ;print,'idx_list[idx1]=',idx_list[idx1]
        ;print,'sort(idx_list[idx1]=',sort(idx_list[idx1])
        ;print,'idx1[sort(idx_list[idx1]-1)]=',idx1[sort(idx_list[idx1]-1)]

        ;isorted = idx1[sort(idx_list[idx1]-1)]
        ;button_labels = button_labels[isorted]
        ;order_labels = order_labels[isorted]
        ;START130802
        isorted=idx1[sort(order_labels[idx1])]
        button_labels = button_labels[isorted]
        order_labels = order_labels[isorted]



    endif else begin
        isorted = -1
        button_labels = ''
        order_labels = ''
    endelse
    return,rtn={labels_sorted:button_labels,isorted:isorted,count:count,order_sorted:order_labels}
end
