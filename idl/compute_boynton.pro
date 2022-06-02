;Copyright 5/24/02 Washington University.  All Rights Reserved.
;compute_boynton.pro  $Revision: 1.9 $
pro compute_boynton_event,ev

;common compute_boynton_comm,wd_text,buttons,wd_ok,wd_reset,wd_exit,parameter_default,parameter,butlabels,wd_out, $
;    wd_echo,wd_scale,wd_format
;START120313
common compute_boynton_comm,wd_text,buttons,wd_ok,wd_reset,wd_exit,parameter_default,parameter,butlabels,wd_out, $
    wd_echo,wd_scale,wd_format,wd_decimal

if ev.id eq wd_ok then begin
    for i=0,5 do begin
        widget_control,wd_text[i],GET_VALUE=scrap
        parameter[i] = scrap
    endfor
    parameter = strtrim(parameter,2)

    ;str = !BIN+'/compute_boynton'
    ;START120313
    str = !BINEXECUTE+'/compute_boynton'

    str = str + ' -hrf_delta ' + parameter[0]
    str = str + ' -hrf_tau ' + parameter[1]
    str = str + ' -TR ' + parameter[2]
    str = str + ' -num_timepoints ' + parameter[3]

    widget_control,buttons[4],GET_VALUE=scrap
    scrap = butlabels[scrap]
    str = str + ' -initial_delay_' + scrap + ' ' + parameter[4]

    widget_control,buttons[5],GET_VALUE=scrap
    scrap = butlabels[scrap]
    str = str + ' -stimlen_' + scrap + ' ' + parameter[5]

    widget_control,wd_out,GET_VALUE=scrap
    str = str + ' -output ' + strtrim(scrap,2) 

    widget_control,wd_scale,GET_VALUE=scrap
    case scrap of
        0:str = str + ' -unscaled'
        1:str = str + ' -max1'
        2:str = str + ' -magnorm'
    endcase

    widget_control,wd_format,GET_VALUE=scrap
    case scrap of
        0:;do nothing
        1:str = str + ' -column'
    endcase

    ;START120313
    widget_control,wd_decimal,GET_VALUE=scrap
    str = str + ' -decimal ' + strtrim(scrap,2) 

    print,'******************'
    print,'HERE IS THE SCRIPT'
    print,'******************'
    print,str
    print,''
    spawn,str,result
    widget_control,wd_echo,SET_VALUE=result 
endif
if ev.id eq wd_reset then begin
    for i=0,5 do widget_control,wd_text[i],SET_VALUE=parameter_default[i]
    for i=0,5 do widget_control,buttons[i],SET_VALUE=0
endif
if ev.id eq wd_exit then widget_control,ev.top,/DESTROY

end


;*********************************************
pro compute_boynton,fi,wd,dsp,help,pr,stc,pref
;*********************************************
common compute_boynton_comm

;topbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=!FMRI_LEADER,TITLE='Boynton HRF')
topbase = widget_base(/COLUMN,TITLE='Boynton HRF')
row1 = widget_base(topbase,/ROW)
label_base = widget_base(row1,/COLUMN)
text_base = widget_base(row1,/COLUMN)
butbase = widget_base(row1,/COLUMN)
row1a = widget_base(topbase,/ROW)
row1a1 = widget_base(topbase,/ROW)
row1b = widget_base(topbase,/ROW)
row2 = widget_base(topbase,/ROW)
row3 = widget_base(topbase,/ROW)

wd_labels = lonarr(6)

;wd_labels[0] = widget_text(label_base,VALUE='HRF delay',SCR_XSIZE=200,SCR_YSIZE=40,/ALL_EVENTS)
;wd_labels[1] = widget_text(label_base,VALUE='HRF time constant',SCR_XSIZE=200,SCR_YSIZE=40,/ALL_EVENTS)
;wd_labels[2] = widget_text(label_base,VALUE='TR',SCR_XSIZE=200,SCR_YSIZE=40,/ALL_EVENTS)
;wd_labels[3] = widget_text(label_base,VALUE='Number of MR frames',SCR_XSIZE=200,SCR_YSIZE=40,/ALL_EVENTS)
;wd_labels[4] = widget_text(label_base,VALUE='Initial delay',SCR_XSIZE=200,SCR_YSIZE=40,/ALL_EVENTS)
;wd_labels[5] = widget_text(label_base,VALUE='Duration',SCR_XSIZE=200,SCR_YSIZE=40,/ALL_EVENTS)

wd_labels[0] = widget_text(label_base,VALUE='HRF delay',SCR_XSIZE=200,SCR_YSIZE=40)
wd_labels[1] = widget_text(label_base,VALUE='HRF time constant',SCR_XSIZE=200,SCR_YSIZE=40)
wd_labels[2] = widget_text(label_base,VALUE='TR',SCR_XSIZE=200,SCR_YSIZE=40)
wd_labels[3] = widget_text(label_base,VALUE='Number of MR frames',SCR_XSIZE=200,SCR_YSIZE=40)
wd_labels[4] = widget_text(label_base,VALUE='Initial delay',SCR_XSIZE=200,SCR_YSIZE=40)
wd_labels[5] = widget_text(label_base,VALUE='Duration',SCR_XSIZE=200,SCR_YSIZE=40)


butlabels = ['frames','seconds']
buttons = lonarr(6)
buttons[0] = cw_bgroup(butbase,butlabels[1],EVENT_FUNCT=compute_boynton_event,SET_VALUE=0,FRAME=2,/EXCLUSIVE,ROW=1,YSIZE=30)
buttons[1] = cw_bgroup(butbase,butlabels[1],EVENT_FUNCT=compute_boynton_event,SET_VALUE=0,FRAME=2,/EXCLUSIVE,ROW=1,YSIZE=30)
buttons[2] = cw_bgroup(butbase,butlabels[1],EVENT_FUNCT=compute_boynton_event,SET_VALUE=0,FRAME=2,/EXCLUSIVE,ROW=1,YSIZE=30)
buttons[3] = cw_bgroup(butbase,butlabels[0],EVENT_FUNCT=compute_boynton_event,SET_VALUE=0,FRAME=2,/EXCLUSIVE,ROW=1,YSIZE=30)
buttons[4] = cw_bgroup(butbase,butlabels,EVENT_FUNCT=compute_boynton_event,SET_VALUE=0,FRAME=2,/EXCLUSIVE,ROW=1,YSIZE=30)
buttons[5] = cw_bgroup(butbase,butlabels,EVENT_FUNCT=compute_boynton_event,SET_VALUE=0,FRAME=2,/EXCLUSIVE,ROW=1,YSIZE=30)

parameter_default = strarr(6)
parameter_default[0] = '2'
parameter_default[1] = '1.25'
parameter_default[2] = '2.36'
parameter_default[3] = '8'
parameter_default[4] = '0'
parameter_default[5] = '0'
wd_text = lonarr(6)
for i=0,5 do wd_text[i] = widget_text(text_base,VALUE=parameter_default[i],/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=100)
parameter = strarr(6)
parameter[0] = parameter_default[0]
parameter[1] = parameter_default[1]
parameter[2] = parameter_default[2]
parameter[3] = parameter_default[3]
parameter[4] = parameter_default[4]
parameter[5] = parameter_default[5]
 

;scaling = ' -unscaled'
wd_scale = cw_bgroup(row1a,['unscaled','maximum 1','zero mean, magnitude 1'],EVENT_FUNCT=compute_boynton_event,SET_VALUE=0, $
    FRAME=2,/EXCLUSIVE,ROW=1,YSIZE=30,LABEL_LEFT='Scaling ')
wd_format = cw_bgroup(row1a1,['row','column'],EVENT_FUNCT=compute_boynton_event,SET_VALUE=0,FRAME=2,/EXCLUSIVE,ROW=1,YSIZE=30, $
    LABEL_LEFT='Output format ')

;START120313
wd_decimal = cw_field(row1a1,TITLE='Decimal places',VALUE='2',/ALL_EVENTS,XSIZE=10)

wd_out = cw_field(row1b,TITLE='Output filename',VALUE='boynton_hrf.txt',/ALL_EVENTS,XSIZE=50)
wd_ok = widget_button(row2,VALUE='OK')
wd_reset = widget_button(row2,VALUE='Reset')
wd_exit = widget_button(row2,VALUE='Exit')
wd_echo = widget_text(row3,YSIZE=20,XSIZE=68,/SCROLL)
widget_control,topbase,/REALIZE
xmanager,'compute_boynton',topbase
end
