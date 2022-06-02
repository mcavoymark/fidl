;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_frm.pro  $Revision: 12.83 $

;*******************
pro get_frm_event,ev
;*******************

common getfrm_comm,wok,frame,wfrm

if(ev.id eq wok) then  begin
    widget_control,wfrm,GET_VALUE=frame
    widget_control,ev.top,/DESTROY
endif

end

;***********************
function get_frm,nframes
;***********************

common getfrm_comm

if(nframes eq 1) then $
    return,1

getfrm = widget_base(title='Planes',/COLUMN,/MODAL,GROUP_LEADER=!FMRI_LEADER)
wfrm = widget_slider(getfrm,Title='Frame',MINIMUM=1,MAXIMUM=nframes,VALUE=1)
wok = widget_button(getfrm,value='OK',uvalue='done')

widget_control,getfrm,/REALIZE
xmanager,'get_frm',getfrm

return,frame
end
