;Copyright 12/31/99 Washington University.  All Rights Reserved.
;add_scalar.pro  $Revision: 12.87 $

;********************************
pro add_scalar,fi,wd,dsp,help,stc
;********************************

; Add two images.

labels = strarr(fi.nfiles)
labels(0:fi.nfiles-1) = fi.tails(0:fi.nfiles-1)
idx = get_button(labels,TITLE='Image')

str = get_str(1,"Constant: ",0)
constant = float(str[0])

hdr = *fi.hdr_ptr(idx)

sum = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim)
widget_control,/SENSITIVE
for t=0,hdr.tdim-1 do begin
    for z=0,hdr.zdim-1 do begin
        tz = t*hdr.zdim + z
        sum(*,*,z,t) = constant + get_image(tz,fi,stc,FILNUM=idx+1)
    endfor
endfor
widget_control,SENSITIVE=0

name = strcompress(string(labels(idx),str, $
			FORMAT='(a,"_+_",a)'),/REMOVE_ALL)

;START150731
;if(ptr_valid(fi.paradigm(idx))) then begin
;    paradigm = *fi.paradigm(idx)
;endif else begin
;    paradigm = fltarr(hdr.tdim)
;    paradigm[*] = 1
;    paradigm[0:hdr.tdim-1] = 0
;endelse

mom = -1

;put_image,reform(sum,hdr.xdim,hdr.ydim,hdr.zdim*hdr.tdim),fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,mom,paradigm,IFH=hdr.ifh
;START150731
put_image,reform(sum,hdr.xdim,hdr.ydim,hdr.zdim*hdr.tdim),fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,mom,IFH=hdr.ifh

fi.color_scale1(fi.n) = !GRAY_SCALE
fi.color_scale2(fi.n) = !NO_SCALE

return
end
