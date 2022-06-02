;Copyright 11/25/03 Washington University.  All Rights Reserved.
;dialog_message_long.pro  $Revision: 1.5 $

;*******************************
pro dialog_message_long_event,ev
;*******************************
common dialog_message_long_comm,str,wd_ok,wd_exit,wd_goback,wd_special,gspecial

case ev.id of
    wd_ok: begin
        str = 'OK' 
        widget_control,ev.top,/DESTROY
    end
    wd_exit: begin
        str = 'EXIT' 
        widget_control,ev.top,/DESTROY
    end
    wd_goback: begin
        str = 'GOBACK' 
        widget_control,ev.top,/DESTROY
    end
    wd_special: begin
        str = gspecial 
        widget_control,ev.top,/DESTROY
    end
    else: ;do nothing
endcase

end


;function dialog_message_long,title,text,NO_MODAL=no_modal,SPECIAL=special
;START17
function dialog_message_long,title,text,NO_MODAL=no_modal,SPECIAL=special,NLINES=nlines

common dialog_message_long_comm
if not keyword_set(NO_MODAL) then no_modal = 0

;START17
if not keyword_set(NLINES) then nlines = 20
 
if no_modal eq 0 then $
    topbase = widget_base(/COLUMN,/MODAL,TITLE=title,GROUP_LEADER=!FMRI_LEADER) $
else $
    topbase = widget_base(/COLUMN,TITLE=title,GROUP_LEADER=!FMRI_LEADER)

row1 = widget_base(topbase,/ROW)
row2 = widget_base(topbase,/ROW)

;wd_text = widget_text(row1,YSIZE=20,XSIZE=100,/SCROLL,VALUE=text)
;START17
wd_text = widget_text(row1,YSIZE=nlines,XSIZE=100,/SCROLL,VALUE=text)

wd_ok = widget_button(row2,VALUE='Ok')
wd_exit = widget_button(row2,VALUE='Exit')
wd_goback = widget_button(row2,VALUE='Go back')



if not keyword_set(SPECIAL) then begin 
    wd_special = -1
    gspecial = ''
endif else begin 
    wd_special = widget_button(row2,VALUE=special)
    gspecial = special
endelse
 


str = ''
widget_control,topbase,/REALIZE
xmanager,'dialog_message_long',topbase
return,str
end
