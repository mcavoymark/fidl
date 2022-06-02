;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_text.pro  $Revision: 12.84 $

;********************
pro get_text_event,ev
;********************

common get_text_comm,intbase,g_ok,wdtext,text

case ev.id of
    g_ok: widget_control,ev.top,/DESTROY
    wdtext: begin
        widget_control,wdtext,GET_VALUE=txt
        text = txt
    end
    else: stat = widget_message('Invalid event in get_text_event')
endcase

return
end

;**********************
function get_text,title
;**********************

common get_text_comm

if(keyword_set(TITLE)) then $
    title = title $
else $
    title = ' '

intbase = widget_base(/COLUMN,TITLE=title,GROUP_LEADER=!FMRI_LEADER,/MODAL)

wdtext = widget_text(intbase,/ALL_EVENTS,/EDITABLE,XSIZE=80,YSIZE=12,/SCROLL)
g_ok = widget_button(intbase,value='OK')

widget_control,intbase,/REALIZE
widget_control,g_ok,/INPUT_FOCUS
xmanager,'get_text',intbase

return,text
end

