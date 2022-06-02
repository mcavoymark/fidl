;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_array.pro  $Revision: 12.83 $

;********************
pro get_array_event,ev
;********************

common getarray_comm,strs,intbase,g_ok,wdstr,nstr,nc,nr,vals

if(ev.id eq g_ok) then begin
    g_ok: widget_control,ev.top,/DESTROY
endif else begin
    tst = 0
    for icol=0,nc-1 do begin
        for irow=0,nr-1 do begin
            if(ev.id eq wdstr(irow+1,icol+1)) then begin
                tst = 1
                strs[irow,icol] = ev.value
            endif
        endfor
    endfor
    if(tst eq 0) then begin
        stat=widget_message('Invalid event in get_array_event',/ERROR)
    endif
endelse

return
end

;************************************************************
function get_array,nrow,ncol,labrow,labcol,values,WIDTH=width,TITLE=title, $
	GROUP_LEADER=group_leader
;************************************************************

common getarray_comm

if(keyword_set(GROUP_LEADER)) then $
    leader = group_leader $
else $
    leader = !FMRI_LEADER

if(keyword_set(WIDTH)) then $
    xsize = width $
else $
    xsize = 15

if(keyword_set(TITLE)) then $
    title = title $
else $
    title = ' '

maxlen = 0
vals = values
nstr = ncol*nrow
nc = ncol
nr = nrow
strs = reform(string(values),nrow*ncol)
for i=0,nrow*ncol-1 do begin
    len = strlen(strs(i))
    if(len gt maxlen) then $
        maxlen = len
endfor
xsize = maxlen
strs = string(values)

intbase = widget_base(/COLUMN,TITLE=title,GROUP_LEADER=leader,/MODAL)
butcol = widget_base(intbase,/ROW)
okbase  = widget_base(intbase,/ROW)

butbase = lonarr(ncol+1)
for icol=0,ncol do $
    butbase[icol] = widget_base(butcol,/COLUMN)

wdstr = intarr(nrow+1,ncol+1)
wdstr(0,0) = cw_field(butbase[0],VALUE=labcol(0), $
				XSIZE=xsize,/NOEDIT,TITLE=' ')
for irow=0,nrow-1 do $
    wdstr(irow+1,0) = cw_field(butbase[0],VALUE=labrow(irow), $
		XSIZE=xsize,/NOEDIT,TITLE=' ')
for icol=1,ncol do $
    wdstr(0,icol) = cw_field(butbase[icol],VALUE=labcol(icol), $
		XSIZE=xsize,/NOEDIT,TITLE=' ')
for icol=0,ncol-1 do begin
    for irow=0,nrow-1 do begin
        if(values[irow,icol] lt -1000) then $
            str = 'N/S' $
        else $
            str = string(values[irow,icol]) 
        wdstr(irow+1,icol+1) = cw_field(butbase[icol+1],VALUE=str, $
		XSIZE=xsize,/ALL_EVENTS,TITLE=' ')
    endfor
endfor

g_ok = widget_button(butbase[ncol/2],value='OK',XSIZE=20)

widget_control,intbase,/REALIZE
widget_control,g_ok,/INPUT_FOCUS
xmanager,'get_array',intbase

return,strs
end

