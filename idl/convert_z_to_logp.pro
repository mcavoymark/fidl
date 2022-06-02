;Copyright 12/31/99 Washington University.  All Rights Reserved.
;convert_z_to_logp.pro  $Revision: 12.83 $

;***************************************
pro convert_z_to_logp,fi,wd,dsp,help,stc
;***************************************

widget_control,/HOURGLASS

hdr = *fi.hdr_ptr(fi.n)
if(ptr_valid(fi.paradigm(fi.n))) then begin
    paradigm = *fi.paradigm(fi.n)
endif else begin
    paradigm = fltarr(hdr.tdim)
    paradigm[*] = 1
    paradigm[0:4] = 0
endelse

img = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
for z=0,hdr.zdim-1 do begin
    img(*,*,z) = get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + z,fi,stc)
endfor
mask = img lt 0
logp = reform(.5*float(errorfc(abs(img)/1.4142135)),dsp[fi.cw].xdim,dsp[fi.cw].ydim,hdr.zdim)
zmask = logp eq 0
logp = -alog10(logp+zmask)
logp = (1 - 2*mask)*logp

name = strcompress(string(fi.name,FORMAT='(a,"_logp")'),/REMOVE_ALL)
put_image,logp,fi,wd,dsp,name,dsp[fi.cw].xdim,dsp[fi.cw].ydim,hdr.zdim,1, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,fi.n,paradigm,IFH=hdr.ifh

widget_control,HOURGLASS=0
end
