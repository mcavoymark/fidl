;Copyright 1/3/17 Washington University.  All Rights Reserved.
;brain_mask2.pro  $Revision: 1.1 $

pro brain_mask2_event,ev
    common brain_mask2_comm,wd_text,buttons,wd_ok,wd_reset,wd_exit,parameter_default,parameter,butlabels,wd_out, $
        wd_echo,wd_scale,wd_format,wd_decimal
    common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw


common brain_mask2_comm,wd_text,buttons,wd_ok,wd_reset,wd_exit,parameter_default,parameter,butlabels,wd_out, $
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

pro brain_mask2,fi,wd,dsp,help,stc
    common brain_mask2_comm
    common stats_comm
    image_index = !D.WINDOW
    if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx_tgt,'image to threshold') eq !ERROR then return
    dsp[fi.cw].file_num = idx_tgt
    tgt_image = fltarr(dsp[fi.cw].xdim,dsp[fi.cw].ydim,dsp[fi.cw].zdim)
    for z=0,dsp[fi.cw].zdim-1 do tgt_image[*,*,z] = get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + z,fi,stc,FILNUM=idx_tgt+1)
    parameter_default=strarr(6)
    parameter_default[0]='200'
    parameter_default[1]=''
    parameter_default[2]='0.5'
    parameter_default[3]='1.8'
    parameter_default[4]=''
    parameter_default[5]=''
    topbase=widget_base(/COLUMN,TITLE='Create mask')
    row0=widget_base(topbase,/ROW)
        label_base0=widget_base(row0,/COLUMN)
            wd_lab0=widget_text(label_base0,VALUE='brain thresh >',SCR_XSIZE=200,SCR_YSIZE=40)
        text_base0=widget_base(row0,/COLUMN)
            wd_txt0=widget_text(text_base0,VALUE=parameter_default[0],/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=100)
        label_base1=widget_base(row0,/COLUMN)
            wd_lab1=widget_text(label_base1,VALUE='mean =',SCR_XSIZE=100,SCR_YSIZE=40)
        text_base1=widget_base(row0,/COLUMN)
            wd_txt1=widget_text(text_base1,VALUE=parameter_default[1],SCR_YSIZE=40,SCR_XSIZE=100)
    row1=widget_base(topbase,/ROW)
        wd_lab2=widget_text(row1,VALUE=Rule to create mask = > thresh 1 AND < thresh 2',SCR_XSIZE=200,SCR_YSIZE=40)
    row1a=widget_base(topbase,/ROW)
        wd_scale=cw_bgroup(row1a,['fractions of mean','image values'],EVENT_FUNCT=brain_mask2_event,SET_VALUE=0, $
            FRAME=2,/EXCLUSIVE,ROW=1,YSIZE=30,LABEL_LEFT='thresh multiplier ')
    row2=widget_base(topbase,/ROW)
        label_base=widget_base(row2,/COLUMN)
            wd_labels=lonarr(2)
            wd_labels[0]=widget_text(label_base,VALUE='> thresh 1 =',SCR_XSIZE=200,SCR_YSIZE=40)
            wd_labels[1]=widget_text(label_base,VALUE='< thresh 2 =',SCR_XSIZE=200,SCR_YSIZE=40)
        text_base=widget_base(row2,/COLUMN)
            wd_text=lonarr(2)
            for i=0,1 do wd_text[i]=widget_text(text_base,VALUE=parameter_default[i+2],/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=100)
        label_base2=widget_base(row2,/COLUMN)
            wd_labels2=lonarr(2)
            for i=0,1 do wd_labels[i]=widget_text(label_base2,VALUE='x',SCR_XSIZE=50,SCR_YSIZE=40)
        text_base2=widget_base(row2,/COLUMN)
            wd_text2=lonarr(2)
            for i=0,1 do wd_text2[i]=widget_text(text_base2,VALUE=parameter_default[i+4],SCR_YSIZE=40,SCR_XSIZE=100)
    parameter=strarr(6)
    parameter[0]=parameter_default[0]
    parameter[1]=parameter_default[1]
    parameter[2]=parameter_default[2]
    parameter[3]=parameter_default[3]
    parameter[4]=parameter_default[4]
    parameter[5]=parameter_default[5]
    wd_ok=widget_button(row2,VALUE='OK')
    wd_reset=widget_button(row2,VALUE='Reset')
    wd_exit=widget_button(row2,VALUE='Exit')
    widget_control,topbase,/REALIZE
    xmanager,'brain_mask2',topbase
end
