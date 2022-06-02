;Copyright 2/25/16 Washington University.  All Rights Reserved.
;get_rpath.pro  $Revision: 1.2 $
pro get_rpath_event,ev
    common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw
    common rpath_comm,prefrhost,prefrpath,wd_host,wd_path,wd_reset,wd_ok,rhost,rpath,nlab

    ;index = where(wd_host eq ev.id,count)
    ;if count ne 0 then begin
    ;    dummy = wd_host[index]
    ;    widget_control,dummy[0],GET_VALUE=scrap
    ;    rhost[index] = scrap
    ;endif
    ;index = where(wd_path eq ev.id,count)
    ;if count ne 0 then begin
    ;    dummy = wd_path[index]
    ;    widget_control,dummy[0],GET_VALUE=scrap
    ;    rpath[index] = scrap
    ;endif

    if ev.id eq wd_reset then begin
        rhost[*]=''
        rpath[*]=''
        rhost[0:n_elements(prefrhost)-1]=prefrhost
        rpath[0:n_elements(prefrpath)-1]=prefrpath
        for i=0,nlab-1 do begin
            widget_control,wd_host[i],SET_VALUE=rhost[i]
            widget_control,wd_path[i],SET_VALUE=rpath[i]
        endfor
    endif
    if ev.id eq wd_ok then begin
        for i=0,nlab-1 do begin
            widget_control,wd_host[i],GET_VALUE=host
            widget_control,wd_path[i],GET_VALUE=path
            rhost[i]=host
            rpath[i]=path
        endfor
        idx=where(rhost ne '' and rpath ne '',cnt)
        if cnt ne 0 then begin
            ptr_free,pref.rhost,pref.rpath
            pref.rhost=ptr_new(rhost[idx])
            pref.rpath=ptr_new(rpath[idx])
        endif
        widget_control,ev.top,/DESTROY
    endif

end
function create_widgets_rpath,X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size
    common rpath_comm
    title = 'R path'
    if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then begin
        topbase = widget_base(/COLUMN,TITLE=title,X_SCROLL_SIZE=(1024<x_scroll_size),XSIZE=x_scroll_size, $
            Y_SCROLL_SIZE=(800<y_scroll_size),YSIZE=y_scroll_size)
    endif else $
        topbase = widget_base(/COLUMN,TITLE=title)
    msg = widget_label(topbase,/ALIGN_LEFT,VALUE=getenv('HOME')+'/.fidl')
    rowbase = widget_base(topbase,/ROW)
    prefbase = widget_base(topbase,/ROW)
    ybase = widget_base(prefbase,/COLUMN)
    label_base = widget_base(prefbase,/COLUMN)
    wd_host = lonarr(nlab)
    wd_path= lonarr(nlab)
    for i=0,nlab-1 do begin
        wd_host[i] = widget_text(ybase,VALUE=rhost[i],/EDITABLE,/ALL_EVENTS)
        wd_path[i] = widget_text(label_base,VALUE=rpath[i],/EDITABLE,/ALL_EVENTS)
    endfor
    wd_reset=widget_button(rowbase,value='Reset')
    wd_ok=widget_button(rowbase,value='Ok')
    return,topbase
end
pro get_rpath
    common stats_comm
    common rpath_comm
    nlab=20
    rhost=strarr(nlab)
    rpath=strarr(nlab)
    prefrhost=*pref.rhost
    prefrpath=*pref.rpath 
    rhost[0:n_elements(prefrhost)-1]=prefrhost
    rpath[0:n_elements(prefrpath)-1]=prefrpath
    topbase=create_widgets_rpath()
    geo = widget_info(topbase,/GEOMETRY)
    xdim = geo.scr_xsize + 2*geo.margin
    ydim = geo.scr_ysize + 2*geo.margin
    if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
        widget_control,topbase,/DESTROY
        topbase=create_widgets_rpath(X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim)
    endif
    widget_control,topbase,/REALIZE
    xmanager,'get_rpath',topbase
end
