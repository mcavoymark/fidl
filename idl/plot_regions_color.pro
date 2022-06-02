;Copyright 4/22/03 Washington University.  All Rights Reserved.
;plot_regions_color.pro  $Revision: 1.5 $
pro plot_regions_color_event,ev
    common plot_regions_color_comm,wd_color,af3dh,color,color_index,old_evid,wd_ok
    index = where(wd_color eq ev.id,count)
    if count gt 0 then begin
        color = af3dh.color[index]
        color_index = index
        if old_evid ne -1 and old_evid ne ev.id then widget_control,old_evid,SET_BUTTON=0
        old_evid = ev.id 
    endif
    if ev.id eq wd_ok then widget_control,ev.top,/DESTROY
end
pro plot_regions_color,group_leader,ncolors,af3dh_in,reg_colors,reg_colors_index
    common plot_regions_color_comm
    af3dh = af3dh_in
    color = reg_colors
    color_index = reg_colors_index
    colorfiles = strarr(ncolors)
    for i=0,ncolors-1 do colorfiles[i] = !PIXMAP_PATH+'/square_'+af3dh.color[i]+'_filled.m.pm' 
    topbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=group_leader,TITLE='Please select color')
    stat = widget_label(topbase,VALUE='You may not have enough colors to see all colors.')
    wd_color = lonarr(ncolors)
    ncol = 7
    npercol = 15
    k = 0
    colorbase = lonarr(ncol)
    for i=0,ncol-1 do colorbase[i] = widget_base(topbase,/ROW,/EXCLUSIVE)
    for i=0,ncol-1 do begin
        for j=0,npercol-1 do begin
            wd_color[k] = widget_button(colorbase[i],VALUE=colorfiles[k],/BITMAP)
            k = k + 1
            if k eq ncolors then goto,exitloop
        endfor
    endfor
    exitloop:
    wd_ok = widget_button(topbase,value='OK')
    widget_control,topbase,/REALIZE
    old_evid = wd_color[color_index]
    widget_control,old_evid,/SET_BUTTON
    xmanager,'plot_regions_color',topbase
    reg_colors = color
    reg_colors_index = color_index
end
