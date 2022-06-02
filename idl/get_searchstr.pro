;Copyright 5/4/05 Washington University.  All Rights Reserved.
;get_searchstr.pro  $Revision: 1.5 $
;**************************
pro get_searchstr_event,ev
;**************************

;common get_searchstr_comm,wd_select,wd_label,wd_ok,wd_goback,wd_help,wd_search,checked,glabel,gsearch,gindex,gcount,nlabel
;START120710
common get_searchstr_comm,wd_select,wd_label,wd_ok,wd_goback,wd_help,wd_search,checked,glabel,gsearch,gindex,gcount,nlabel, $
    wd_setunset,unset_set
 
index = where(wd_select eq ev.id,count)
if count gt 0 then checked[index] = abs(checked[index]-1)
if ev.id eq wd_ok then begin
    widget_control,/HOURGLASS
    for i=0,nlabel-1 do begin
        widget_control,wd_label[i],GET_VALUE=scrap
        glabel[i] = scrap
        widget_control,wd_search[i],GET_VALUE=scrap
        gsearch[i] = scrap
    endfor
    gindex = where(checked,gcount)
    if gcount eq 0 then begin
        if get_button(['Try again.','Go back'],TITLE='You failed to select a contrast.') eq 1 then begin 
            glabel[0] = 'GO_BACK'
            widget_control,ev.top,/DESTROY
        endif
    endif else $
        widget_control,ev.top,/DESTROY
endif
if ev.id eq wd_goback then begin 
    glabel[0] = 'GO_BACK'
    widget_control,ev.top,/DESTROY
endif
if ev.id eq wd_help then begin
    text = '*'+string(9B)+'Matches any string including the null string.'+string(10B)+'?'+string(9B)+'Matches any single ' $
        +'character.'+string(10B)+'[...]'+string(9B)+'Matches any of the enclosed characters.'+string(10B)+string(9B) $
        +'A pair of characters separated by "-" matches any character lexically between the pair, inclusive.'+string(10B) $
        +string(9B)+'If the first character following the opening [ is a !, any character not enclosed is matched.' $
        +string(10B)+string(9B)+'To prevent one of these characters from acting as a wildcard, it can be quoted by ' $
        +'preceding it with a backslash character.'
    stat=dialog_message_long('Search string help',text,/NO_MODAL)
endif

;START120710
if ev.id eq wd_setunset then begin
    unset_set = abs(unset_set-1)
    checked[*] = unset_set
    for i=0,nlabel-1 do widget_control,wd_select[i],SET_BUTTON=checked[i]
endif

end

;*************************************************************************************************
function get_searchstr_widgets,label,title,X_SCROLL_SIZE=x_scroll_size,Y_SCROLL_SIZE=y_scroll_size
;*************************************************************************************************
common get_searchstr_comm
lettersize=11
x = max(strlen(label))*lettersize > 110
y=40
intbase = widget_base(TITLE=title,/MODAL,GROUP_LEADER=!FMRI_LEADER,/COLUMN)
if keyword_set(X_SCROLL_SIZE) or keyword_set(Y_SCROLL_SIZE) then begin
    topbase = widget_base(intbase,/ROW,X_SCROLL_SIZE=(!DISPXMAX<x_scroll_size),XSIZE=x_scroll_size, $
        Y_SCROLL_SIZE=(!DISPYMAX<y_scroll_size),YSIZE=y_scroll_size)
endif else $
    topbase = widget_base(intbase,/ROW)
col1 = widget_base(topbase,/COLUMN)
col2 = widget_base(topbase,/COLUMN)
col3 = widget_base(topbase,/COLUMN)
header0 = widget_label(col1,VALUE='',SCR_XSIZE=x,SCR_YSIZE=y+10)
hlabel = widget_label(col2,VALUE='output label',SCR_XSIZE=x,SCR_YSIZE=y)
hsearch = widget_label(col3,VALUE='search string',SCR_XSIZE=x,SCR_YSIZE=y)
selectbase = widget_base(col1,/COLUMN,/NONEXCLUSIVE)
labelbase = widget_base(col2,/COLUMN)
searchbase = widget_base(col3,/COLUMN)
row2 = widget_base(intbase,/ROW)
wd_select = lonarr(nlabel)
wd_label = lonarr(nlabel)
wd_search = lonarr(nlabel)
for i=0,nlabel-1 do begin
    wd_select[i] = widget_button(selectbase,VALUE=label[i],SCR_YSIZE=y,SCR_XSIZE=x)
    wd_label[i] = widget_text(labelbase,VALUE=label[i],/EDITABLE,SCR_YSIZE=y,SCR_XSIZE=x)
    wd_search[i] = widget_text(searchbase,VALUE=label[i],/EDITABLE,SCR_YSIZE=y,SCR_XSIZE=x)
endfor
wd_ok = widget_button(row2,value='OK')
wd_goback = widget_button(row2,value='GO BACK')
wd_help = widget_button(row2,value='Search string help')

;START120710
wd_setunset = widget_button(row2,value='Set/unset')

return,intbase
end


;*********************************
function get_searchstr,label,title
;*********************************
common get_searchstr_comm
nlabel = n_elements(label)
checked = intarr(nlabel)
glabel = strarr(nlabel)
gsearch = strarr(nlabel)
gindex = 0
gcount = 0

;START120710
unset_set = 0

widget_control,/HOURGLASS
intbase = get_searchstr_widgets(label,title)
geo = widget_info(intbase,/GEOMETRY)
xdim = geo.scr_xsize + 2*geo.margin
ydim = geo.scr_ysize + 2*geo.margin
if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
    widget_control,intbase,/DESTROY
    widget_control,/HOURGLASS
    intbase = get_searchstr_widgets(label,title,X_SCROLL_SIZE=xdim,Y_SCROLL_SIZE=ydim)
endif
widget_control,intbase,/REALIZE
xmanager,'get_searchstr',intbase
return,rtn={index:gindex,count:gcount,label:glabel[gindex],searchstr:gsearch}
end
