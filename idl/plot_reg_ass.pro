;Copyright 10/29/03 Washington University.  All Rights Reserved.
;plot_reg_ass.pro  $Revision: 1.9 $

;************************
pro plot_reg_ass_event,ev
;************************
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

common view_images_comm_shared,lc_refreshed,prf,win,lc_color_bar_once,wd_hidecolors,wd_showcolors_vert, $
    wd_showcolors_horiz,current_label,current_cells,display_mode,af3dh

common plot_reg_ass_comm,topbase,wd_filename,wd_val,wd_plot,wd_exit,nreg,checked,gregion_str,nplot

index = where(wd_filename eq ev.id,count)
if count gt 0 then begin
    checked[index] = abs(checked[index]-1)
    lc_refreshed = !FALSE
endif

if ev.id eq wd_plot then begin
    val = strarr(nreg)
    for i=0,nreg-1 do begin
        widget_control,wd_val[i],GET_VALUE=scrap
        val[i] = scrap
    endfor

    if gregion_str eq '' then begin
        win.reg[win.idxidx].checked = ptr_new(checked)
        win.reg[win.idxidx].val = ptr_new(float(val))
        view_images_refresh,fi,dsp,vw.fnt
    endif else begin
        index = where(checked eq 1,count)
        if count eq 0 then $
            stat=dialog_message('You have not checked any regions. Please try again.',/INFORMATION) $
        else begin
            scrap = get_str(1,'output','regval'+strtrim(nplot,2)+'.4dfp.img',WIDTH=50,TITLE='Please enter desired filename.', $
                /GO_BACK)
            if scrap[0] ne 'GO_BACK' then begin
                if strpos(scrap[0],'.4dfp.img',/REVERSE_SEARCH) eq -1 then scrap[0] = scrap[0] + '.4dfp.img'
                out_str = ' -output ' + scrap[0]

                scrap = strtrim(index+1,2)
                region_numbers_str = ' -regions_of_interest'
                for i=0,count-1 do region_numbers_str = region_numbers_str + ' ' + scrap[i]
                scrap = strtrim(val[index],2)
                val_str = ' -val'
                for i=0,count-1 do val_str = val_str + ' ' + scrap[i]

                ;cmd = !BIN+'/fidl_reg_ass'+gregion_str+region_numbers_str+val_str+out_str
                ;START130516
                cmd = !BINEXECUTE+'/fidl_reg_ass'+gregion_str+region_numbers_str+val_str+out_str

                spawn_cover,cmd,fi,wd,dsp,help,stc
                nplot = nplot + 1
            endif
        endelse
    endelse


endif
if ev.id eq wd_exit then begin
    if gregion_str eq '' then begin
        ptr_free,win.reg[win.idxidx].checked
        ptr_free,win.reg[win.idxidx].val
    endif
    widget_control,ev.top,/DESTROY
endif

end

;******************************************************************************************
pro create_widgets_pra,region_names,X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size
;******************************************************************************************
common stats_comm
common view_images_comm_shared
common plot_reg_ass_comm

title = 'Please assign values.'
if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then begin
    topbase = widget_base(/COLUMN,TITLE=title,X_SCROLL_SIZE=(1024<x_scroll_size),XSIZE=x_scroll_size, $
        Y_SCROLL_SIZE=(800<y_scroll_size),YSIZE=y_scroll_size)
endif else $
    topbase = widget_base(/COLUMN,TITLE=title)

prefbase = widget_base(topbase,/ROW)
colbase = widget_base(prefbase,/COLUMN,/NONEXCLUSIVE)
colbase_val = widget_base(prefbase,/COLUMN)
rowbase = widget_base(topbase,/ROW)

reg_val = make_array(nreg,/STRING,VALUE='0.0')
wd_filename = lonarr(nreg)
wd_val = lonarr(nreg)
for i=0,nreg-1 do begin
    wd_filename[i] = widget_button(colbase,VALUE=region_names[i],SCR_YSIZE=32)
    wd_val[i] = widget_text(colbase_val,VALUE=reg_val[i],/EDITABLE,SCR_YSIZE=32)
endfor

;wd_plot = widget_button(rowbase,value='Plot')
;if make_4dfp eq 0 then value = 'Plot' else value = 'Make 4dfp'
;if gmake_4dfp eq 0 then value = 'Plot' else value = 'Make 4dfp'
if gregion_str eq '' then value = 'Plot' else value = 'Make 4dfp'
wd_plot = widget_button(rowbase,VALUE=value)

wd_exit = widget_button(rowbase,VALUE='Exit')
end



;**************************************************
pro plot_reg_ass,region_names,REGION_STR=region_str
;**************************************************
common stats_comm
common view_images_comm_shared
common plot_reg_ass_comm

;nreg = win.nreg[win.idxidx]
nreg = n_elements(region_names) 

checked = intarr(nreg)

;if not keyword_set(MAKE_4DFP) then gmake_4dfp = 0 else gmake_4dfp = 1
if not keyword_set(REGION_STR) then gregion_str='' else gregion_str = region_str 
;create_widgets_pra,region_names,make_4dfp
create_widgets_pra,region_names
geo = widget_info(topbase,/GEOMETRY)
xdim = geo.scr_xsize + 2*geo.margin
ydim = geo.scr_ysize + 2*geo.margin
if xdim gt 1024 or ydim gt 800 then begin
    widget_control,topbase,/DESTROY
    create_widgets_pra,region_names,X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim
    ;create_widgets_pra,region_names,make_4dfp,X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim
endif

nplot = 1
widget_control,topbase,/REALIZE
xmanager,'plot_reg_ass',topbase
end
