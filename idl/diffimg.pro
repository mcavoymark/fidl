;Copyright 12/31/99 Washington University.  All Rights Reserved.
;diffimg.pro  $Revision: 12.85 $

;*******************************************************************************************
pro diffimg,fi,pr,dsp,wd,stc,help,pref,lcfit,fwhm,hd,hdr,plus,var2,var1,minus,df,BATCH=batch
;*******************************************************************************************

; Compute difference images by correlating with square waves of different
;  phases.

common linear_fit,fit_name,slope,intcpt,trend

if(keyword_set(BATCH)) then $
    lcbatch = !TRUE $
else $
    lcbatch = !FALSE

hd = *fi.hdr_ptr(fi.n)
hdr = *fi.hdr_ptr(hd.mother)

if(not ptr_valid(trend.slope_ptr(hd.mother)) and lcfit eq !TRUE) then $
    get_trend,hd.mother,fi,pr,dsp,wd,stc,help,pref

if(ptr_valid(fi.paradigm(hd.mother))) then begin
    paradigm = *fi.paradigm(hd.mother)
endif else begin
    paradigm = fltarr(hdr.tdim)
    paradigm[*] = 1
    paradigm[0:4] = 0
endelse

len_plus = 0
len_minus = 0
for t=0,hdr.tdim-1 do begin
    if(paradigm(t) gt 0) then $
	len_plus = len_plus + 1 
    if(paradigm(t) lt 0) then $
	len_minus = len_minus + 1
endfor
plus = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
minus = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
var1 = fltarr(dsp[fi.cw].xdim,dsp[fi.cw].ydim,dsp[fi.cw].zdim)
mean1 = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
var2 = fltarr(dsp[fi.cw].xdim,dsp[fi.cw].ydim,dsp[fi.cw].zdim)
mean2 = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
mask = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)

if(lcbatch eq !FALSE) then spw_marker,"Plane",0,(hdr.tdim-1)*(hdr.zdim-1)

for z=0,hdr.zdim-1 do begin
    nplus = 0
    nminus = 0
    n1 = 0.
    n2 = 0.
    if(lcbatch eq !FALSE) then widget_control,wd.error,SET_VALUE=string('Plane: ',z+1)
    for t=0,hdr.tdim-1 do begin
        if(lcbatch eq !FALSE) then spam_marker,t*z,dsp[fi.cw].white,/FILL
	if(paradigm(t) ne 0) then begin
	    if(lcfit eq !TRUE) then begin
                img = get_image(t*hdr.zdim+z,fi,stc,FILNUM=hd.mother+1,/SUBTRACT_TREND)
 	    endif else begin
                img = get_image(t*hdr.zdim+z,fi,stc,FILNUM=hd.mother+1)
	    endelse
	    if(fwhm gt 0.) then $
	        image = convol(img,krnl,/CENTER,/EDGE_TRUNCATE) $
	    else $
	        image = img
	    if(paradigm(t) lt 0) then begin
	        minus(*,*,z) = minus(*,*,z) + image
	        nminus = nminus + 1
	        mean1(*,*,z)  =  mean1(*,*,z) + image(*,*)
        	var1(*,*,z) = var1(*,*,z) + image(*,*)^2
		n1 = n1 + 1.
            endif
	    if(paradigm(t) gt 0) then begin
	        plus(*,*,z) = plus(*,*,z) + image
	        nplus = nplus + 1
		mean2(*,*,z)  =  mean2(*,*,z) + image(*,*)
        	var2(*,*,z) = var2(*,*,z) + image(*,*)^2
		 n2 = n2 + 1.
	    endif
	endif
    endfor
    var1(*,*,z)  = (n1*var1(*,*,z) - mean1(*,*,z)^2)/(n1*(n1-1.))
    mean1(*,*,z) = mean1(*,*,z)/n1
    var2(*,*,z)  = (n2*var2(*,*,z) - mean2(*,*,z)^2)/(n2*(n2-1.))
    mean2(*,*,z) = mean2(*,*,z)/n2
endfor
spam_marker,0,!WHITE,/DESTROY
var1 = sqrt((var1 + var2)/(n1+n2))

plus = plus/float(nplus) - minus/float(nminus)

mask = mean1 gt 0
minus = plus/(mean1 + (not mask)) ; Avoid division by zero.
; Mask out noise-only regions.
for z=0,hdr.zdim-1 do $
    mask(*,*,z) = mean1(*,*,z) gt (.1*max(mean1(*,*,z)))
minus = 100.*mask*minus

mask = var1 le 0
var2 = plus/(var1 + mask)

df = n1+n2-3

return
end
