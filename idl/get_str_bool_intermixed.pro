;Copyright 7/26/02 Washington University.  All Rights Reserved.
;get_str_bool_intermixed.pro  $Revision: 1.12 $
pro get_str_bool_intermixed_event,ev

    ;common get_strbool_intermixed_comm,strs,g_ok,wdstr,gbelow,widget_type,g_go_back
    ;START150114
    common get_strbool_intermixed_comm,strs,g_ok,wdstr,gbelow,widget_type,g_go_back,bool_labels,nstr

    ;index = where(ev.id eq wdstr,count)
    ;if count ne 0 then begin
    ;    scrap = widget_type[index]
    ;    scrap = scrap[0]
    ;    if scrap eq 'field' then begin
    ;        if gbelow eq 0 then $
    ;           strs[index] = ev.value $
    ;        else begin
    ;            widget_control,ev.id,GET_VALUE=scrap
    ;            strs[index] = scrap
    ;        endelse
    ;    endif else begin
    ;        strs[index] = strtrim(ev.value,2)
    ;    endelse
    ;endif else if ev.id eq g_ok then begin 
    ;    widget_control,/HOURGLASS
    ;    widget_control,ev.top,/DESTROY 
    ;endif else if ev.id eq g_go_back then begin
    ;    strs[0] = 'GO_BACK'
    ;    widget_control,ev.top,/DESTROY
    ;endif
    ;START150114
    index = where(ev.id eq wdstr,count)
    if count ne 0 then begin
        if bool_labels[0] eq '' then begin
            scrap = widget_type[index]
            scrap = scrap[0]
            if scrap eq 'field' then begin
                if gbelow eq 0 then $
                   strs[index] = ev.value $
                else begin
                    widget_control,ev.id,GET_VALUE=scrap
                    strs[index] = scrap
                endelse
            endif else begin
                strs[index] = strtrim(ev.value,2)
            endelse
        endif
    endif else if ev.id eq g_ok then begin
        if bool_labels[0] ne '' then begin
            for i=0,nstr*2-1 do begin
                widget_control,wdstr[i],GET_VALUE=scrap 
                strs[i]=scrap
            endfor
            strs = trim(strs)
        endif

        widget_control,/HOURGLASS
        widget_control,ev.top,/DESTROY
    endif else if ev.id eq g_go_back then begin
        strs[0] = 'GO_BACK'
        widget_control,ev.top,/DESTROY
    endif


end

;pro create_widgets_gsbi,butcol,labels,values,xsize,leader,title,label,one_column,nstr,intbase,X_SCROLL_SIZE=x_scroll_size, $
;    Y_SCROLL_SIZE=y_scroll_size
;START150113
;pro create_widgets_gsbi,butcol,labels,values,xsize,leader,title,label,one_column,nstr,intbase,bool_labels,field_label, $
;    X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size
;START150114
pro create_widgets_gsbi,butcol,labels,values,xsize,leader,title,label,one_column,intbase,field_label, $
    X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size

    common get_strbool_intermixed_comm

    ;intbase = widget_base(/COLUMN,TITLE=title,GROUP_LEADER=leader,/MODAL)
    ;if label ne '' then labelbase = widget_label(intbase,VALUE=label)
    ;if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then begin
    ;    butcol = widget_base(intbase,/ROW,X_SCROLL_SIZE=(!DISPXMAX<x_scroll_size),XSIZE=x_scroll_size, $
    ;        Y_SCROLL_SIZE=(!DISPYMAX<y_scroll_size),YSIZE=y_scroll_size)
    ;endif else $
    ;    butcol = widget_base(intbase,/ROW)
    ;if one_column eq 1 then begin
    ;    next = nstr
    ;    ncols = 1
    ;endif else begin
    ;    next = 15
    ;    ncols = fix((nstr+1)/next) + 1
    ;endelse
    ;boolbase = widget_base(intbase,/ROW)
    ;okbase  = widget_base(intbase,/ROW)
    ;butbase = lonarr(ncols)
    ;if gbelow eq 0 then begin 
    ;    for i=0,nstr-1 do begin
    ;        icol = i/next
    ;        if widget_type[i] eq 'field' then begin 
    ;            wdstr[i] = cw_field(butbase[icol],TITLE=labels[i],VALUE=values[i],XSIZE=xsize,/ALL_EVENTS)
    ;        endif else begin
    ;            wdstr[i] = cw_bgroup(butbase[icol],labels[i],/RETURN_INDEX,/EXCLUSIVE,SET_VALUE=0,COLUMN=1)
    ;        endelse
    ;    endfor
    ;endif else begin
    ;    for i=0,nstr-1 do begin
    ;        icol = i/next
    ;        if widget_type[i] eq 'field' then begin
    ;            wdstr_label[i] = widget_label(butbase[icol],VALUE=labels[i])
    ;            wdstr[i] = widget_text(butbase[icol],XSIZE=100,YSIZE=10,/ALL_EVENTS,/EDITABLE,/WRAP,/SCROLL)
    ;        endif else begin
    ;            wdstr[i] = cw_bgroup(butbase[icol],labels[i],/RETURN_INDEX,/EXCLUSIVE,SET_VALUE=0,COLUMN=1)
    ;        endelse
    ;    endfor
    ;endelse
    ;START150113
    if one_column eq 1 then begin
        next = nstr
        ncols = 1
    endif else begin
        next = 15
        ncols = fix((nstr+1)/next) + 1
    endelse
    if bool_labels[0] eq '' then begin
        intbase = widget_base(/COLUMN,TITLE=title,GROUP_LEADER=leader,/MODAL)
        if label ne '' then labelbase = widget_label(intbase,VALUE=label)
        if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then begin
            butcol = widget_base(intbase,/ROW,X_SCROLL_SIZE=(!DISPXMAX<x_scroll_size),XSIZE=x_scroll_size, $
                Y_SCROLL_SIZE=(!DISPYMAX<y_scroll_size),YSIZE=y_scroll_size)
        endif else $
            butcol = widget_base(intbase,/ROW)
        boolbase = widget_base(intbase,/ROW)
        okbase  = widget_base(intbase,/ROW)
        butbase = lonarr(ncols)
        for icol=0,ncols-1 do butbase[icol] = widget_base(butcol,/COLUMN,FRAME=2)

        ;wdstr = intarr(nstr)
        ;START150114
        wdstr = lonarr(nstr)

        if gbelow eq 0 then begin
            for i=0,nstr-1 do begin
                icol = i/next
                if widget_type[i] eq 'field' then begin
                    wdstr[i] = cw_field(butbase[icol],TITLE=labels[i],VALUE=values[i],XSIZE=xsize,/ALL_EVENTS)
                endif else begin
                    wdstr[i] = cw_bgroup(butbase[icol],labels[i],/RETURN_INDEX,/EXCLUSIVE,SET_VALUE=0,COLUMN=1)
                endelse
            endfor
        endif else begin
            for i=0,nstr-1 do begin
                icol = i/next
                if widget_type[i] eq 'field' then begin
                    wdstr_label[i] = widget_label(butbase[icol],VALUE=labels[i])
                    wdstr[i] = widget_text(butbase[icol],XSIZE=100,YSIZE=10,/ALL_EVENTS,/EDITABLE,/WRAP,/SCROLL)
                endif else begin
                    wdstr[i] = cw_bgroup(butbase[icol],labels[i],/RETURN_INDEX,/EXCLUSIVE,SET_VALUE=0,COLUMN=1)
                endelse
            endfor
        endelse
    endif else begin
        intbase = widget_base(/COLUMN,TITLE=title,GROUP_LEADER=leader,/MODAL)
        if label ne '' then labelbase = widget_label(intbase,VALUE=label)
        if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then begin
            butcol = widget_base(intbase,/COLUMN,X_SCROLL_SIZE=(!DISPXMAX<x_scroll_size),XSIZE=x_scroll_size, $
                Y_SCROLL_SIZE=(!DISPYMAX<y_scroll_size),YSIZE=y_scroll_size)
        endif else $
            butcol = widget_base(intbase,/COLUMN)
        boolbase = widget_base(intbase,/ROW)
        okbase  = widget_base(intbase,/ROW)
        butbase = lonarr(nstr)

        ;wdstr = intarr(nstr)
        ;wdstr2 = intarr(nstr)
        ;for i=0,nstr-1 do begin
        ;    butbase[i] = widget_base(butcol,/ROW)
        ;    wdstr[i] = cw_bgroup(butbase[i],bool_labels,/RETURN_INDEX,/EXCLUSIVE,SET_VALUE=1,COLUMN=1,LABEL_LEFT=labels[i])
        ;    wdstr2[i] = cw_field(butbase[i],TITLE=field_label,VALUE=values[i],XSIZE=xsize,/ALL_EVENTS)
        ;endfor
        ;START150114
        wdstr = lonarr(nstr*2)
        ;widget_type = strarr(nstr*2) 
        for i=0,nstr-1 do begin
            butbase[i] = widget_base(butcol,/ROW)
            wdstr[i] = cw_bgroup(butbase[i],bool_labels,/RETURN_INDEX,/EXCLUSIVE,SET_VALUE=1,COLUMN=1,LABEL_LEFT=labels[i])
            ;widget_type[i] = 'button'
            wdstr[nstr+i] = cw_field(butbase[i],TITLE=field_label,VALUE=values[i],XSIZE=xsize,/ALL_EVENTS)
            ;widget_type[nstr+i] = 'field'
        endfor
        strs=strarr(nstr*2)

    endelse




    g_ok = widget_button(okbase,value='OK')
    if g_go_back gt 0 then g_go_back = widget_button(okbase,value='GO BACK')
end

;function get_str_bool_intermixed,nstr,labels,bool_labels,values,widget_type_in,WIDTH=width,TITLE=title, $
;    GROUP_LEADER=group_leader,BELOW=below,LABEL=label,ONE_COLUMN=one_column,GO_BACK=go_back
;START150113
;function get_str_bool_intermixed,nstr,labels,bool_labels,values,widget_type_in,field_label,WIDTH=width,TITLE=title, $
;    GROUP_LEADER=group_leader,BELOW=below,LABEL=label,ONE_COLUMN=one_column,GO_BACK=go_back
;START150114
function get_str_bool_intermixed,nstr_in,labels,bool_labels_in,values,widget_type_in,field_label,WIDTH=width,TITLE=title, $
    GROUP_LEADER=group_leader,BELOW=below,LABEL=label,ONE_COLUMN=one_column,GO_BACK=go_back



common get_strbool_intermixed_comm
if keyword_set(WIDTH) then xsize = width else xsize = 15
if keyword_set(TITLE) then title = title else title = ''
if keyword_set(GROUP_LEADER) then leader = group_leader else leader = !FMRI_LEADER
if keyword_set(BELOW) then gbelow = 1 else gbelow = 0
if keyword_set(LABEL) then label = label else label = ''
if not keyword_set(ONE_COLUMN) then one_column = 0
if not keyword_set(GO_BACK) then g_go_back=0 else g_go_back=1

;strs = string(values)
;START150113
strs = values

maxlen = max(strlen(strs))
if maxlen gt xsize then xsize = maxlen

;START150114
nstr = nstr_in
bool_labels = bool_labels_in

widget_type = widget_type_in

;create_widgets_gsbi,butcol,labels,values,xsize,leader,title,label,one_column,nstr,intbase
;START150113
;create_widgets_gsbi,butcol,labels,values,xsize,leader,title,label,one_column,nstr,intbase,bool_labels,field_label
;START150114
create_widgets_gsbi,butcol,labels,values,xsize,leader,title,label,one_column,intbase,field_label

geo = widget_info(intbase,/GEOMETRY)
xdim = geo.scr_xsize + 2*geo.margin
ydim = geo.scr_ysize + 2*geo.margin
;print,'xdim=',xdim,' !DISPXMAX=',!DISPXMAX,' ydim=',ydim,' !DISPYMAX=',!DISPYMAX
if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
    widget_control,intbase,/DESTROY

    ;create_widgets_gsbi,butcol,labels,values,xsize,leader,title,label,one_column,nstr,intbase,X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim
    ;START150113
    create_widgets_gsbi,butcol,labels,values,xsize,leader,title,label,one_column,intbase,field_label, $
        X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim

endif
widget_control,intbase,/REALIZE
widget_control,g_ok,/INPUT_FOCUS
widget_control,intbase,/SHOW

xmanager,'get_str_bool_intermixed',intbase

return,strs
end
