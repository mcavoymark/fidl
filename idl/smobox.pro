;Copyright 12/31/99 Washington University.  All Rights Reserved.
;smobox.pro  $Revision: 12.84 $
pro smobox,fi,dsp,wd
label = strarr(2)
label(0) = string('3D')
label(1) = string('2D')
title = string('Filter across 2D planes or 3D volume?')
dim3d = get_boolean(title,label)

value = strarr(1)
label = strarr(1)
value(0) = string('0')
label(0) = string("Width of Boxcar filter in pixels: ")
dims = get_str(1,label,value)
width = float(dims(0))

widget_control,/HOURGLASS

hdr = *fi.hdr_ptr(fi.n)

if(dim3d eq !TRUE) then begin
    ndim = '3D'
    img = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
    for z=0,hdr.zdim-1 do begin
	img(*,*,z) = get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + z,fi,stc)
    end
    simg = box_smoth(img,width)
    name = strcompress(string(fi.path,fi.tail,dsp[fi.cw].frame,width,FORMAT='(a,a,"_f",i2,"_3Dboxcar_",f5.2,".4dfp.img")'),/REMOVE_ALL)
endif else begin
    ndim = '2D'
    simg = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
    for z=0,hdr.zdim-1 do begin
	img = get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + z,fi,stc)
        simg(*,*,z) = box_smoth(img,width)
    end
    name = strcompress(string(fi.path,fi.tail,dsp[fi.cw].frame,width,FORMAT='(a,a,"_f",i2,"_2Dboxcar_",f5.2,".4dfp.img")'),/REMOVE_ALL)
endelse

fi.rec[fi.nfiles] = fi.rec[fi.n] + ' + ' + string(width,ndim, $
        FORMAT='("Boxcar smooth(Width:",f7.2," Dimensions: ",a,")")')

;paradigm = -1
;put_image,simg,fi,wd,dsp,name,dsp[fi.cw].xdim,dsp[fi.cw].ydim,hdr.zdim,1, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,fi.n,paradigm,IFH=hdr.ifh
;START150731
put_image,simg,fi,wd,dsp,name,dsp[fi.cw].xdim,dsp[fi.cw].ydim,hdr.zdim,1, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,fi.n,IFH=hdr.ifh

widget_control,HOURGLASS=0

return
end
