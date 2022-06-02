;Copyright 8/22/01 Washington University.  All Rights Reserved.
;get_base.pro  $Revision: 1.16 $
function get_base,labels,leader,align_left,TITLE=title,BASE_TITLE=base_title,WIDTH=width,ONE_COLUMN=one_column, $
    X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size
if not keyword_set(BASE_TITLE) then base_title = ''
intbase = widget_base(/COLUMN,/MODAL,GROUP_LEADER=leader,TITLE=base_title)
if keyword_set(TITLE) then begin
    stat = widget_label(intbase,VALUE=title,ALIGN_LEFT=align_left)
    geo = widget_info(stat,/GEOMETRY)
    xdim = geo.scr_xsize + 2*geo.margin
    ydim = geo.scr_ysize + 2*geo.margin
    if xdim gt !DISPXMAX then begin
         widget_control,stat,/DESTROY
         intbase1 = widget_base(intbase,/ROW,TITLE=title,X_SCROLL_SIZE=!DISPXMAX,XSIZE=xdim,Y_SCROLL_SIZE=ydim)
         stat = widget_label(intbase1,VALUE=title,ALIGN_LEFT=align_left)
    endif
endif
len = n_elements(labels)
if not keyword_set(ONE_COLUMN) then ncol = fix((len-1)/25) + 1 else ncol = 1
buttons = lonarr(ncol)
screenx = !DISPXMAX
screeny = !DISPYMAX
if not keyword_set(WIDTH) then begin
    if ncol gt 5 then begin
        butbase = widget_base(intbase,/ROW,X_SCROLL_SIZE=1034,XSIZE=x_scroll_size,Y_SCROLL_SIZE=screeny)
    endif else begin
        length = max(strlen(labels))*ncol
        y = 25
        if not keyword_set(ONE_COLUMN) then begin
            if length lt 100 then begin 
                butbase = widget_base(intbase,/ROW)
            endif else begin 
                butbase = widget_base(intbase,/ROW,SCR_XSIZE=!DISPXMAX,SCR_YSIZE=len*50<!DISPYMAX,/SCROLL)
            endelse
        endif else begin
            if len gt y then begin
                butbase = widget_base(intbase,/ROW,SCR_XSIZE=length*11<screenx,Y_SCROLL_SIZE=len*35<screeny)
            endif else begin
                if length lt 100 then $
                    butbase = widget_base(intbase,/ROW) $
                else $
                    butbase = widget_base(intbase,/ROW,SCR_XSIZE=1200,SCR_YSIZE=len*35<screeny,/SCROLL)
            endelse
        endelse
    endelse
endif else begin
    if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then $ 
        butbase = widget_base(intbase,/ROW,X_SCROLL_SIZE=(!DISPXMAX<x_scroll_size),XSIZE=x_scroll_size, $
            Y_SCROLL_SIZE=(!DISPYMAX<y_scroll_size),YSIZE=y_scroll_size) $
    else $
        butbase = widget_base(intbase,/ROW)
endelse
okbase  = widget_base(intbase,/ROW)
if not keyword_set(ONE_COLUMN) then number = 25 else number = len
return,rtn={len:len,ncol:ncol,buttons:buttons,intbase:intbase,butbase:butbase,okbase:okbase,number:number}
end
