;Copyright 12/31/99 Washington University.  All Rights Reserved.
;brain_mask.pro  $Revision: 12.87 $

;********************************
pro brain_mask,fi,wd,dsp,help,stc
;********************************

; Compute mask that is one for valid activations.

common profiles,x1,x2,y1,y2,x1m1,x2m1,y1m1,y2m1,row,col,profdat,profmask

if(help.enable eq !TRUE) then begin
    x = 'Empirical thresholds are applied to roughly determine where gray and white matter lie.'
    widget_control,help.id,SET_VALUE=x
endif

labels = strarr(fi.nfiles)
labels(0:fi.nfiles-1) = fi.tails(0:fi.nfiles-1)
idx = get_button(labels,TITLE='Image to generate mask')

hdr = *fi.hdr_ptr(idx)
length = hdr.xdim*hdr.ydim*hdr.zdim

image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
for z=0,hdr.zdim-1 do $
    image(*,*,z) = get_image(z,fi,stc,FILNUM=idx+1)

mask_tmp = image gt 200
mask_image = image*mask_tmp
mean1 = total(mask_image)/total(mask_tmp)

thresh1 = .5*mean1
thresh2 = 1.8*mean1
brain_mask = (image gt thresh1)*(image lt thresh2)
name = strcompress(string(labels(idx),FORMAT='(a,"_brain_mask")'),/REMOVE_ALL)

;paradigm = -1
;put_image,brain_mask,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!LOGICAL_ARRAY,hdr.mother,paradigm,IFH=hdr.ifh
;START150731
put_image,brain_mask,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!LOGICAL_ARRAY,hdr.mother,IFH=hdr.ifh

fi.color_scale1(fi.n) = !GRAY_SCALE
fi.color_scale2(fi.n) = !NO_SCALE
end
