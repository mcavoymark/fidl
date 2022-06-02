;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_table.pro  $Revision: 12.83 $

;********************
pro get_table_event,ev
;********************

common gettab_comm,table,wtab,g_ok,tabmask

if(ev.id eq g_ok) then begin
    g_ok: widget_control,ev.top,/DESTROY
endif else begin
   if(ev.id eq wtab) then begin
        widget_control,wtab,GET_VALUE=table
;;;	table = table*tabmask
;;;        widget_control,wtab,SET_VALUE=table
   endif
endelse

return
end

;*****************************************************************************
function get_table,init_table,col_labels,row_labels,WIDTH=width,TITLE=title,MASK=mask
;*****************************************************************************

; mask: 1 = editable, 0 = non-editable

common gettab_comm

table = init_table
if(keyword_set(MASK)) then begin
    tabmask = mask
endif else begin
    tabmask = table
    tabmask(*,*) = 1
endelse
if(keyword_set(WIDTH)) then $
    cwidth = width $
else $
    cwidth = 2
intbase = widget_base(/COLUMN,TITLE=title,GROUP_LEADER= !FMRI_LEADER,/MODAL)
wtab = widget_table(intbase,/EDITABLE,/RESIZEABLE_COLUMNS,VALUE=init_table, $
        FORMAT='(f6.3)',COLUMN_LABELS=col_labels,ROW_LABELS=row_labels, $
        COLUMN_WIDTHS=cwidth,UNITS=2)
g_ok = widget_button(intbase,value='OK')

widget_control,intbase,/REALIZE
widget_control,g_ok,/INPUT_FOCUS
xmanager,'get_table',intbase

return,table
end

