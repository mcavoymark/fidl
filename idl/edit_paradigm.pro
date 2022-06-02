;Copyright 12/31/99 Washington University.  All Rights Reserved.
;edit_paradigm.pro  $Revision: 12.83 $

;*************************
pro edit_paradigm_event,ev
;*************************

common get_paradigm_comm,paradigm,tdim,wplot,wslider,wrest,wact,wbad, $
	wdone,plot_window,image_index,xm1,frame,x_axis,paradigm_old, $
	wcancel

strs = !TRUE
case ev.id of

wslider: begin
    wset,plot_window
    xxx = intarr(16)
    device,SET_GRAPHICS=6,CURSOR_IMAGE=xxx
    widget_control,wslider,GET_VALUE=frame
    x = fix((!PARADIGM_WIDTH-2*!PARADIGM_OFFSET-1)*float(frame-1)/tdim) $
	+ !PARADIGM_OFFSET
    if(xm1 ge 0) then $
        plots,[xm1,xm1],[0,!PARADIGM_HGHT],COLOR=!D.TABLE_SIZE-1,/DEV,/NOCLIP
    plots,[x,x],[0,!PARADIGM_HGHT],COLOR= !D.TABLE_SIZE-1,/DEV,/NOCLIP
    wset,image_index
    device,SET_GRAPHICS=3,/CURSOR_CROSSHAIR     ;Resume normal graphics.
    xm1 = x
end

wrest: begin
    paradigm(frame-1) = -1
    wset,plot_window
    plot,x_axis,paradigm,XMARGIN=5,XRANGE=[1,tdim+1], $
	YRANGE=[-2,1],XSTYLE=9,YSTYLE=20,PSYM=3,SYMSIZE=2,XTITLE='Frame', $
	POSITION=[!PARADIGM_OFFSET,!PARADIGM_Y0, $
	!PARADIGM_WIDTH-!PARADIGM_OFFSET,!PARADIGM_Y1],/DEVICE,COLOR=!WHITE
    xm1 = -1	; Don't try to cover up last line in wslider:
    wset,image_index
end

wact: begin
    paradigm(frame-1) = 1
    wset,plot_window
    plot,x_axis,paradigm,XMARGIN=5,XRANGE=[1,tdim+1], $
	YRANGE=[-2,1],XSTYLE=9,YSTYLE=20,PSYM=3,SYMSIZE=2,XTITLE='Frame', $
	POSITION=[!PARADIGM_OFFSET,!PARADIGM_Y0, $
	!PARADIGM_WIDTH- !PARADIGM_OFFSET, !PARADIGM_Y1],/DEVICE,COLOR= !WHITE
    xm1 = -1	; Don't try to cover up last line in wslider:
    wset,image_index
end

wbad: begin
    paradigm(frame-1) = 0
    wset,plot_window
    plot,x_axis,paradigm,XMARGIN=5,XRANGE=[1,tdim+1], $
	YRANGE=[-2,1],XSTYLE=9,YSTYLE=20,PSYM=3,SYMSIZE=2,XTITLE='Frame', $
	POSITION=[!PARADIGM_OFFSET,!PARADIGM_Y0, $
                  !PARADIGM_WIDTH-!PARADIGM_OFFSET, !PARADIGM_Y1],/DEVICE,COLOR=!WHITE
    xm1 = -1	; Don't try to cover up last line in wslider:
    wset,image_index
end

wdone: widget_control,ev.top,/DESTROY

wcancel: begin
    paradigm = paradigm_old
    widget_control,ev.top,/DESTROY
end

else: stat=widget_message('*** Invalid event in edit_paradigm.  ***',/ERROR)

endcase

return
end

;*********************************
function edit_paradigm,paradigm_in
;*********************************

common get_paradigm_comm

tdim = n_elements(paradigm_in)
paradigm_old = paradigm_in
paradigm = paradigm_in
xm1 = -1
image_index = !D.WINDOW

intbase = widget_base(/COLUMN,TITLE='Stimulus',/MODAL,GROUP_LEADER= !FMRI_LEADER)
plot = widget_base(intbase,/COLUMN)
slider = widget_base(intbase,/ROW)
buttons = widget_base(intbase,/ROW)

wplot = widget_draw(plot,XSIZE= !PARADIGM_WIDTH,YSIZE= !PARADIGM_HGHT)
wslider = widget_slider(slider,Title='Frame Number',SCROLL=1, $
	/DRAG,MINIMUM=1,MAXIMUM=tdim,XSIZE= !PARADIGM_WIDTH)
frame=1
wrest = widget_button(buttons,VALUE='Rest')
wact = widget_button(buttons,VALUE='Activation')
wbad = widget_button(buttons,VALUE='Ignore')
wdone = widget_button(buttons,VALUE='Done')
wcancel = widget_button(buttons,VALUE='Cancel')

widget_control,intbase,/REALIZE

x_axis = indgen(tdim)
x_axis(*) = x_axis(*) + 1
widget_control,GET_VALUE=plot_window,wplot
wset,plot_window
plot,x_axis,paradigm,XMARGIN=5,XRANGE=[1,tdim+1], $
	YRANGE=[-2,1],XSTYLE=9,YSTYLE=20,PSYM=3,SYMSIZE=2,XTITLE='Frame', $
	POSITION=[ !PARADIGM_OFFSET, !PARADIGM_Y0, $
        !PARADIGM_WIDTH- !PARADIGM_OFFSET, !PARADIGM_Y1],/DEVICE,COLOR= !WHITE

xmanager,'edit_paradigm',intbase

wset,image_index
rtn = {name:'',status:!OK,msg:'',paradigm:reform(paradigm,tdim)}
return,rtn
end
