;Copyright 12/31/99 Washington University.  All Rights Reserved.
;fit_trend.pro  $Revision: 1.5 $

;*******************************************************************************
pro fit_trend,filnum,tfirst,tlast,wd,fi,stc,hdr,paradigm_in,slope,intcpt,t_rest1
;*******************************************************************************

common fidl_batch,macro_commands,num_macro_cmd,lcbatch,batch_base, $
        batch_id,batch_name,macro_descriptor,num_macro_dsc,batch_pro

s  = fltarr(hdr.xdim,hdr.ydim)
sx = fltarr(hdr.xdim,hdr.ydim)
sy = fltarr(hdr.xdim,hdr.ydim)
sxx = fltarr(hdr.xdim,hdr.ydim)
sxy = fltarr(hdr.xdim,hdr.ydim)
den = fltarr(hdr.xdim,hdr.ydim)
slope  = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
intcpt = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)

paradigm = paradigm_in
t_rest1 = tfirst
N_rest1 = 0
tdim = n_elements(paradigm)
while(paradigm(t_rest1) lt 1) do begin
    if(paradigm(t_rest1) lt 0) then $
        N_rest1 = N_rest1 + 1
    t_rest1 = t_rest1 + 1
    if(t_rest1 ge tdim) then goto,END1
endwhile
END1:
if(t_rest1 eq tdim) then  begin
    t_rest1 = 0
    N_rest1 = 0
endif
if(N_rest1 lt 24) then begin
    for i=0,t_rest1-1 do $
        paradigm(i) = 0
endif

N = 0
for t=tfirst,tlast do begin
    if(paradigm(t) lt 0) then $
        N = N + 1
endfor

s(*,*) = N
npts = 0
for z=0,hdr.zdim-1 do begin
    if(lcbatch eq !FALSE) then $
        widget_control,wd.error,SET_VALUE=string('Plane: ',z+1)
    sy(*,*) = 0.
    sxy(*,*) = 0.
    sx1 = 0.
    sxx1 = 0.
    npts = 0
    for t=tfirst,tlast do begin
        if(paradigm(t) lt 0) then begin
            image = get_image(z+t*hdr.zdim,fi,stc,FILNUM=filnum+1)
            sy = sy + image
            sxy = sxy + t*image
            sx1 = sx1 + t
            sxx1 = sxx1 + t^2
            npts = npts + 1
        endif
    end
    sx(*,*) = sx1
    sxx(*,*) = sxx1
    den           = s*sxx - sx^2
    if(npts gt 5) then begin
        slope(*,*,z)  = (s*sxy -sx*sy)/den
        intcpt(*,*,z) = (sxx*sy - sx*sxy)/den
    endif else begin
        slope = 0
        intcpt = 0
        stat = widget_message("Too few points in control periods to estimate slope and intercept. Aborting trend-fitting.",/ERROR)
        return
    endelse
endfor

return
end
