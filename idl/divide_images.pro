;Copyright 12/31/99 Washington University.  All Rights Reserved.
;divide_images.pro  $Revision: 1.7 $

;***********************************
pro divide_images,fi,wd,dsp,help,stc
;***********************************

; Add two images.

labels = strarr(fi.nfiles)
labels(0:fi.nfiles-1) = fi.tails(0:fi.nfiles-1)
idx1 = get_button(labels,TITLE='Numerator')

labels = strarr(fi.nfiles)
labels(0:fi.nfiles-1) = fi.tails(0:fi.nfiles-1)
idx2 = get_button(labels,TITLE='Denominator')

hdr1 = *fi.hdr_ptr(idx1)
hdr2 = *fi.hdr_ptr(idx2)

x1 = hdr1.xdim*hdr1.ydim*hdr1.zdim*hdr1.tdim
x2 = hdr2.xdim*hdr2.ydim*hdr2.zdim*hdr2.tdim
if(x1 ne x2) then begin
    stat = widget_message('Dimensions of images must be the same.')
    return
endif

quotient = fltarr(hdr1.xdim,hdr1.ydim,hdr1.zdim,hdr1.tdim)
widget_control,/SENSITIVE
for t=0,hdr1.tdim-1 do begin
    for z=0,hdr1.zdim-1 do begin
        tz = t*hdr1.zdim + z
        quotient(*,*,z,t) = get_image(tz,fi,stc,FILNUM=idx1+1)
        img = get_image(tz,fi,stc,FILNUM=idx2+1)
        mask = img eq 0.
        quotient(*,*,z,t) = (1-mask)*quotient(*,*,z,t)/(img + mask)
    endfor
endfor
widget_control,SENSITIVE=0
name = strcompress(string(labels(idx1),labels(idx2),FORMAT='(a,"_div_",a)'),/REMOVE_ALL)
mom = -1

;if(ptr_valid(fi.paradigm(idx1))) then begin
;    paradigm = *fi.paradigm(idx1)
;endif else begin
;    paradigm = fltarr(hdr1.tdim)
;    paradigm[*] = 1
;    paradigm[0:hdr1.tdim-1] = 0
;endelse
;put_image,reform(quotient,hdr1.xdim,hdr1.ydim,hdr1.zdim*hdr1.tdim),fi,wd,dsp,name,hdr1.xdim,hdr1.ydim,hdr1.zdim,hdr1.tdim, !FLOAT,hdr1.dxdy,hdr1.dz,hdr1.scl, !FLOAT_ARRAY,mom,paradigm,IFH=hdr1.ifh
;START150731
put_image,reform(quotient,hdr1.xdim,hdr1.ydim,hdr1.zdim*hdr1.tdim),fi,wd,dsp,name,hdr1.xdim,hdr1.ydim,hdr1.zdim,hdr1.tdim, !FLOAT,hdr1.dxdy,hdr1.dz,hdr1.scl, !FLOAT_ARRAY,mom,IFH=hdr1.ifh

fi.color_scale1(fi.n) = !GRAY_SCALE
fi.color_scale2(fi.n) = !NO_SCALE
end
