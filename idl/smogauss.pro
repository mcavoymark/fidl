;Copyright 12/31/99 Washington University.  All Rights Reserved.
;smogauss.pro  $Revision: 12.86 $
pro smogauss,fi,st,dsp,wd,stc,help,glm,pref
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx,'image') eq !ERROR then return

dims = get_str(1,'FWHM in voxels: ','2',/EXIT)
if dims[0] eq 'EXIT' then return
width = float(dims[0])

widget_control,/HOURGLASS
if hdr.tdim gt 1 then begin
    tdim = hdr.tdim
    simage = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,tdim)
endif else begin
    tdim = 1
    simage = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
endelse

widget_control,/HOURGLASS
image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
widget_control,/HOURGLASS
for t=0,tdim-1 do begin
    if tdim gt 0 then widget_control,wd.error,SET_VALUE=strcompress(string(t+1,FORMAT='("Processing frame #",i4)'))
    for z=0,hdr.zdim-1 do image[*,*,z] = get_image(z+t*hdr.zdim,fi,stc,FILNUM=idx+1)
    simg = gauss_smoth(image,width)
    simage[*,*,*,t] = simg
endfor

name = fi.tails[idx]+'_fwhm'+strtrim(string(width,FORMAT='(f4.1)'),2)
fi.rec[fi.nfiles] = fi.rec[fi.n] + ' + ' + string(width,'3D',FORMAT='("Gaussian smooth(FWHM:",f7.2," Dimensions: ",a,")")')

;paradigm = -1
;put_image,reform(simage,hdr.xdim,hdr.ydim,hdr.zdim*tdim),fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,tdim, $
;    !FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother,paradigm,IFH=hdr.ifh
;START150731
put_image,reform(simage,hdr.xdim,hdr.ydim,hdr.zdim*tdim),fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,tdim, $
    !FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother,IFH=hdr.ifh


end
