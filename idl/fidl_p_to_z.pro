;Copyright 11/4/10 Washington University.  All Rights Reserved.
;fidl_p_to_z.pro  $Revision: 1.4 $
pro fidl_p_to_z_event,ev
common fidl_p_to_z_comm,wd_text,wd_ok,wd_reset,wd_exit,parameter_default,parameter,wd_echo,wd_clear,append
if ev.id eq wd_ok then begin
    for i=0,2 do begin
        widget_control,wd_text[i],GET_VALUE=scrap
        parameter[i] = scrap
    endfor
    parameter = strtrim(parameter,2)
    str = !BINEXECUTE+'/fidl_p_to_z'

    ;str = str + ' -pval ' + parameter[0]
    ;str = str + ' -zstat ' + parameter[1]
    ;str = str + ' -nvox ' + parameter[2]
    ;START140723
    str = str + ' -p ' + parameter[0]
    str = str + ' -z ' + parameter[1]
    str = str + ' -ntests ' + parameter[2]

    ;print,'******************'
    ;print,'HERE IS THE SCRIPT'
    ;print,'******************'
    print,str
    ;print,''
    spawn,str,result
    ;print,'result=',result
    ;widget_control,wd_echo,SET_VALUE=result,/APPEND 
    widget_control,wd_echo,SET_VALUE=result,APPEND=append
    append=1
endif
if ev.id eq wd_reset then begin
    for i=0,2 do widget_control,wd_text[i],SET_VALUE=parameter_default[i]
endif
if ev.id eq wd_exit then widget_control,ev.top,/DESTROY
if ev.id eq wd_clear then begin
    widget_control,wd_echo,SET_VALUE=''
    append=0
endif
end

pro fidl_p_to_z,fi,wd,dsp,help,pr,stc,pref
common fidl_p_to_z_comm
append=1
topbase = widget_base(/COLUMN,TITLE='P to Z and Z to P')
row1 = widget_base(topbase,/ROW)
label_base = widget_base(row1,/COLUMN)
text_base = widget_base(row1,/COLUMN)
row2 = widget_base(topbase,/ROW)
row3 = widget_base(topbase,/ROW)
wd_labels = lonarr(3)

;wd_labels[0] = widget_text(label_base,VALUE='p value',SCR_XSIZE=200,SCR_YSIZE=40)
;wd_labels[1] = widget_text(label_base,VALUE='z statistic',SCR_XSIZE=200,SCR_YSIZE=40)
;wd_labels[2] = widget_text(label_base,VALUE='number of voxels',SCR_XSIZE=200,SCR_YSIZE=40)
;START140723
wd_labels[0] = widget_text(label_base,VALUE='p',SCR_XSIZE=200,SCR_YSIZE=40)
wd_labels[1] = widget_text(label_base,VALUE='z',SCR_XSIZE=200,SCR_YSIZE=40)
wd_labels[2] = widget_text(label_base,VALUE='number of tests',SCR_XSIZE=200,SCR_YSIZE=40)

parameter_default = ['0.001','3.00','210237']
wd_text = lonarr(3)
for i=0,2 do wd_text[i] = widget_text(text_base,VALUE=parameter_default[i],/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=100)
parameter = parameter_default
wd_ok = widget_button(row2,VALUE='OK')
wd_reset = widget_button(row2,VALUE='Reset')
wd_exit = widget_button(row2,VALUE='Exit')
wd_clear = widget_button(row2,VALUE='Clear display')

;wd_echo = widget_text(row3,YSIZE=10,XSIZE=65,/SCROLL)
;START140723
wd_echo = widget_text(row3,YSIZE=10,XSIZE=75,/SCROLL)

widget_control,topbase,/REALIZE
xmanager,'fidl_p_to_z',topbase
end
