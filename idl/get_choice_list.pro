;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_choice_list.pro  $Revision: 1.5 $

;*************************
pro get_choice_list_event,ev
;*************************

common get_choice_list_comm,num,strs,intbase,buttons,list,g_ok,len,ncol,wd_text,g_go_back

;if(ev.id eq g_ok) then begin
;    widget_control,ev.top,/DESTROY

if ev.id eq g_ok then begin
    widget_control,ev.top,/DESTROY
endif else if ev.id eq g_go_back then begin
    list[0] = -1 
    widget_control,ev.top,/DESTROY




endif else begin
    for id=0,len-1 do begin
        if(ev.id eq buttons[id]) then begin
            if(ev.value eq 0) then $
                list[id] = !TRUE $
            else $
                list[id] = !FALSE
            widget_control,buttons[id],SET_VALUE=(list[id] xor 1)
        endif
    endfor
endelse

return
end

;****************************************************************************
;function get_choice_list,labels,TITLE=title,DEFAULT=default,GROUP_LEADER=group_leader,TEXT=text
function get_choice_list,labels,TITLE=title,DEFAULT=default,GROUP_LEADER=group_leader,TEXT=text,GO_BACK=go_back
;****************************************************************************
; Labels: 2xN array of labels. (2 choices per label, N = number of button rows.
; Default: 2xN array of default values.

common get_choice_list_comm

if keyword_set(GROUP_LEADER) then leader = group_leader else leader = !FMRI_LEADER
intbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=leader)
if keyword_set(TITLE) then stat = widget_label(intbase,VALUE=title)
if not keyword_set(GO_BACK) then $
    g_go_back = -1 $
else begin
    g_go_back = 1
    ;okbase  = widget_base(intbase,/ROW)
endelse

len = n_elements(labels[0,*])
butbase  = lonarr(len)
buttons = lonarr(len)


if(keyword_set(DEFAULT)) then begin
    list = default
endif else begin
    list = intarr(len)
    list[*] = 1
endelse

for id=0,len-1 do begin
    butbase[id]  = widget_base(intbase,/ROW)
    buttons[id] = cw_bgroup(butbase[id],labels[*,id],EVENT_FUNCT=get_choice_event,/RETURN_INDEX,SET_VALUE=(list[id] xor 1),FRAME=2,/EXCLUSIVE,ROW=1)
endfor

if(keyword_set(TEXT)) then begin
    wd_text = widget_text(intbase,/WRAP,/SCROLL,YSIZE=10,VALUE=text)
endif else begin
    wd_text = 0L
endelse

;g_ok = widget_button(intbase,VALUE='OK',XSIZE=60)
if g_go_back eq -1 then begin
    g_ok = widget_button(intbase,value='OK')
endif else begin
    okbase  = widget_base(intbase,/ROW)
    g_ok = widget_button(okbase,value='OK',XSIZE=100)
    if g_go_back gt -1 then g_go_back = widget_button(okbase,value='GO BACK')
endelse


widget_control,intbase,/REALIZE
widget_control,g_ok,/INPUT_FOCUS
xmanager,'get_choice_list',intbase

return,list
end

