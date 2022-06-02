;Copyright 3/28/05 Washington University.  All Rights Reserved.
;get_create_contrast.pro  $Revision: 1.6 $

;************
pro workerbee
;************
common get_create_contrast_comm,wd_text,wd_reset,parameter_default,wd_out1,wd_out2,gtot_eff,geffect_labelus,gwstr,buttons2, $
    wd_insert,wd_create,wd_delete,wd_exit,gindex,gcount,gcc,gc,gccnorm,gcnorm,gnc,gcontrast_labels,geffect_label,topbase,checked, $
    geffect_column,geffect_length,wd_labels,gcnorm_dontchange,wd_con,colbase,gc_scrap

gindex = where(stregex(gwstr,'[1-9]',/BOOLEAN) eq 1,gcount)
if gcount ne 0 then begin
    for i=0,gcount-1 do begin
        dummy = wd_labels[gindex[i]]
        widget_control,dummy[0],GET_VALUE=scrap
        geffect_labelus[gindex[i]] = scrap
    endfor
    i1 = where(stregex(strmid(geffect_labelus[gindex],0,1),'[0-9]',/BOOLEAN) eq 1,c1)
    if c1 ne 0 then geffect_labelus[gindex[i1]] = '_' + geffect_labelus[gindex[i1]]
    gwstr = strtrim(gwstr,2)
    indexpm = where(stregex(strmid(gwstr[gindex],0,1),'[+-]',/BOOLEAN) eq 0,countpm)
    if countpm ne 0 then gwstr[gindex[indexpm]] = '+' + gwstr[gindex[indexpm]]
    cname = strjoin(gwstr[gindex]+geffect_labelus[gindex],/SINGLE)
    if strmid(cname,0,1) eq '+' then cname = strmid(cname,1)
endif else $
    cname = ''
widget_control,wd_out1,SET_VALUE=cname
end

;*******************************
pro get_create_contrast_event,ev
;*******************************
common get_create_contrast_comm

index = where(wd_labels eq ev.id,count)
if count ne 0 then workerbee 

index = where(wd_text eq ev.id,count)
if count ne 0 then begin
    dummy = wd_text[index]
    widget_control,dummy[0],GET_VALUE=scrap
    gwstr[index] = scrap
    workerbee
endif

if ev.id eq wd_reset then begin
    for i=0,gtot_eff-1 do widget_control,wd_text[i],SET_VALUE=parameter_default[i]
    widget_control,wd_out1,SET_VALUE=''
endif
if ev.id eq wd_insert then begin 
    widget_control,wd_out1,GET_VALUE=scrap
    widget_control,wd_out2,SET_VALUE=scrap
endif
if ev.id eq wd_create then begin
    if gcount ne 0 then begin
        dummy = gwstr[gindex]
        wts = fltarr(gcount)
        for i=0,gcount-1 do begin 
            scrap = float(strsplit(dummy[i],'/',/EXTRACT))
            if n_elements(scrap) eq 2 then scrap = scrap[0]/scrap[1]
            wts[i] = scrap
        endfor
   
        for i=0,gcount-1 do begin
            dummy = wd_con[gindex[i]]
            widget_control,dummy[0],GET_VALUE=scrap
            ;print,'scrap=',scrap
            scrap2 = float(strsplit(scrap[0],/EXTRACT))
            i1 = where(gcnorm_dontchange[*,gindex[i]] ne 0.,c1)
            eci = geffect_column[i1]
            eli = geffect_length[i1]

            nscrap2 = n_elements(scrap2)
            if nscrap2 ne eli[0] then begin
                stat=dialog_message('You have entered '+strtrim(nscrap2,2)+' values for event type '+geffect_label[i1]+string(10B) $
                    +'Need to enter '+strtrim(eli,2),/ERROR)
                goto,bottom
            endif

            ;gc[eci[0]:eci[0]+eli[0]-1,i1] = scrap2
            ;START72
            gc_scrap[eci[0]:eci[0]+eli[0]-1,i1] = scrap2
        endfor

        c = gc[*,gindex] 
        gcc[*,0] = 0. 
        for i=0,gcount-1 do begin
            ;print,'i=',i,' wts=',wts[i]
            ;print,'c=',c[*,i]
            gcc[*,0] = gcc[*,0] + wts[i]*c[*,i] 
        endfor
        ;START72
        gcc[*,0] = 0.
        for i=0,gcount-1 do begin
            gcc[*,0] = gcc[*,0] + wts[i]*gc_scrap[*,gindex[i]]
        endfor


        sum = total(gcc[*,0])
        if abs(sum) gt .01 then $ 
            stat=dialog_message('Contrast sums to '+strtrim(sum,2)+string(10B)+'Must sum to zero to be a valid contrast.',/ERROR) $
        else begin
            gc = [[gc],[gcc]]
            gccnorm[*,0] = 0.
            gccnorm[gindex,0] = float(wts) 
            gcnorm = [[gcnorm],[gccnorm]]

            widget_control,wd_out2,GET_VALUE=scrap
            if scrap[0] eq '' then widget_control,wd_out1,GET_VALUE=scrap            

            ;gcontrast_labels = [gcontrast_labels,scrap]
            ;START72
            ;print,'size(scrap)=',size(scrap)
            gcontrast_labels = [gcontrast_labels,scrap[0]]

            gnc = gnc + 1
            buttons2 = [buttons2,widget_button(colbase,VALUE=scrap[0])]
            checked = [checked,0] 
        endelse
    endif
    bottom:
endif

index = where(buttons2 eq ev.id,count)
if count gt 0 then checked[index] = abs(checked[index]-1)
if ev.id eq wd_delete then begin
    index = where(checked eq 0,count)
    i1 = where(checked eq 1,c1)
    if count ne 0 then begin
        if count lt gnc then begin
            gc = gc[*,index]
            gcnorm = gcnorm[*,index]
            gcontrast_labels = gcontrast_labels[index]
            gnc = count
            for i=0,c1-1 do widget_control,buttons2[i1[i]],/DESTROY
            buttons2 = buttons2[index]
            checked = intarr(gnc)
        endif
    endif
endif
if ev.id eq wd_exit then widget_control,ev.top,/DESTROY
end

;****************************************
pro get_create_contrast_widgets,dc,dcnorm
;****************************************
common get_create_contrast_comm
lettersize=19
scr_ysize = 40
y_row1 = gtot_eff*scr_ysize < 500
y_row2d = !DISPYMAX - 200 - y_row1
gwstr[*] = ''
x = max(strlen(geffect_label))*lettersize
topbase = widget_base(/COLUMN,TITLE='Contrast Creator and Deleter',/MODAL,GROUP_LEADER=!FMRI_LEADER)
row1 = widget_base(topbase,/ROW,Y_SCROLL_SIZE=y_row1)
wl_base = widget_base(row1,/COLUMN)
label_base = widget_base(row1,/COLUMN)
text_base = widget_base(row1,/COLUMN)
con_base = widget_base(row1,/COLUMN)
row2a1 = widget_base(topbase,/ROW)
row2b = widget_base(topbase,/ROW)
row2b1 = widget_base(topbase,/ROW)
wd_wl = lonarr(gtot_eff)
for i=0,gtot_eff-1 do wd_wl[i] = widget_label(wl_base,VALUE=geffect_label[i],SCR_YSIZE=40,/ALIGN_LEFT)
wd_labels = lonarr(gtot_eff)
for i=0,gtot_eff-1 do wd_labels[i] = widget_text(label_base,VALUE=geffect_label[i],SCR_XSIZE=x,SCR_YSIZE=40,/EDITABLE,/ALL_EVENTS)
wd_text = lonarr(gtot_eff)
for i=0,gtot_eff-1 do wd_text[i] = widget_text(text_base,VALUE=parameter_default[i],/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=100, $
    /ALL_EVENTS)


;constr = strarr(gtot_eff)
;wd_con = lonarr(gtot_eff)
;for i=0,gtot_eff-1 do begin
;    index = where(gcnorm[*,i] ne 0.,count)
;    eci = geffect_column[index]
;    eli = geffect_length[index]
;    constr[i] = strjoin(strtrim(gc[eci[0]:eci[0]+eli[0]-1,i],2),' ',/SINGLE)
;    wd_con[i] = widget_text(con_base,VALUE=constr[i],/EDITABLE,SCR_YSIZE=40,/ALL_EVENTS)
;endfor
;START72
wd_con = lonarr(gtot_eff)
for i=0,gtot_eff-1 do begin
    i1 = where(gcontrast_labels eq geffect_label[i],c1)     
    if c1 ne 1 then begin
        scrap_cnorm = dcnorm[*,i]
        scrap_c = dc[*,i]
    endif else begin
        scrap_cnorm = gcnorm[*,i1[0]] 
        scrap_c = gc[*,i1[0]]
    endelse
    index = where(scrap_cnorm ne 0.,count)
    eci = geffect_column[index]
    eli = geffect_length[index]
    constr = strjoin(strtrim(scrap_c[eci[0]:eci[0]+eli[0]-1],2),' ',/SINGLE)
    wd_con[i] = widget_text(con_base,VALUE=constr,/EDITABLE,SCR_YSIZE=40,/ALL_EVENTS)
endfor



wd_out1 = widget_label(row2a1,VALUE='',/ALIGN_LEFT,/DYNAMIC_RESIZE)
wd_reset = widget_button(row2b,VALUE='Reset',FRAME=2)
wd_insert = widget_button(row2b,VALUE='Insert default name',FRAME=2)
wd_create = widget_button(row2b,VALUE='Create',FRAME=2)
wd_exit = widget_button(row2b,VALUE='Exit',FRAME=2)
wd_out2 = cw_field(row2b1,TITLE='Contrast name',VALUE='',XSIZE=100,/ALL_EVENTS)
colbase = widget_base(topbase,/COLUMN,/NONEXCLUSIVE,Y_SCROLL_SIZE=y_row2d)
row3 = widget_base(topbase,/ROW)
checked = intarr(gnc)
buttons2 = lonarr(gnc)
for i=0,gnc-1 do buttons2[i] = widget_button(colbase,VALUE=gcontrast_labels[i])
wd_delete = widget_button(row3,VALUE='Delete')
widget_control,topbase,/REALIZE
xmanager,'get_create_contrast',topbase
end

;**************************************************************************************************************
pro get_create_contrast,tot_eff,effect_label,nc,contrast_labels,c,cnorm,N,effect_column,effect_length,dc,dcnorm
;**************************************************************************************************************
common get_create_contrast_comm
gtot_eff = tot_eff
geffect_label = strtrim(effect_label,2)
gnc = nc
gcontrast_labels = strtrim(contrast_labels,2)
gc = c
gc_scrap = c
gcnorm = cnorm
gcnorm_dontchange = cnorm

gcc = fltarr(N,1)
gccnorm = fltarr(tot_eff,1)
geffect_labelus = geffect_label
geffect_column = effect_column
geffect_length = effect_length

gwstr = strarr(tot_eff)
parameter_default = make_array(tot_eff,/STRING,VALUE='0')

get_create_contrast_widgets,dc,dcnorm

nc = gnc
contrast_labels = gcontrast_labels
c = gc
cnorm = gcnorm

end
