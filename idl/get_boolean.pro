;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_boolean.pro  $Revision: 12.83 $

;***********************
pro get_boolean_event,ev
;***********************

common get_bool_comm,num,strs,intbase,get_true,get_false

strs = !TRUE
case ev.id of

    get_true: strs = !TRUE

    get_false: strs = !FALSE

    else: stat = widget_message(string("Invalid event in get_str_event"),/ERROR)

endcase

widget_control,ev.top,/DESTROY

check_math_errors,!FALSE
return
end

;********************************
function get_boolean,title,labels
;********************************

common get_bool_comm

if(!FMRI_LEADER gt 0) then $
    intbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=!FMRI_LEADER) $
else $
    intbase = widget_base(/COLUMN)

wlabel = widget_base(intbase,/COLUMN)
buttons = widget_base(intbase,/ROW)

wroinum   = widget_label(wlabel,value=title)
get_true = widget_button(buttons,VALUE=labels(0))
get_false = widget_button(buttons,VALUE=labels(1))

widget_control,intbase,/REALIZE
widget_control,get_true,/INPUT_FOCUS
xmanager,'get_boolean',intbase

return,strs
end

