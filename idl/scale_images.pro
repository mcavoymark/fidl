;Copyright 12/31/99 Washington University.  All Rights Reserved.
;scale_images.pro  $Revision: 12.89 $
pro scale_images,fi,wd,dsp,help,stc
labels = strarr(fi.nfiles)
labels(0:fi.nfiles-1) = fi.tails(0:fi.nfiles-1)
idx_image = get_button(labels,TITLE='Image to be scaled')
hdr = *fi.hdr_ptr(idx_image)

if(hdr.tdim eq 1) then begin
    str = get_str(1,'Scale factor',1.)
    scale_factor = float(str[0])
    frame = 1
endif else begin
    str = get_str(2,['Scale factor','Frames (all/frm #)'],['1.','All'])
    scale_factor = float(str[0])
    if(str[1] eq 'All') then $
        frame = -1 $
    else $
        frame = fix(str[1])
endelse

widget_control,/HOURGLASS
if(frame lt 0) then begin
    scaled_image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim)
    for t=0,hdr.tdim-1 do begin
        for z=0,hdr.zdim-1 do begin
            tz = t*hdr.zdim + z
            scaled_image(*,*,z,t) = scale_factor*get_image(tz,fi,stc,FILNUM=idx_image+1)
        endfor
    endfor
    scaled_image = reform(scaled_image,hdr.xdim,hdr.ydim,hdr.zdim*hdr.tdim)
endif else begin
    scaled_image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
    tz = (frame-1)*hdr.zdim 
    for z=0,hdr.zdim-1 do begin
        scaled_image(*,*,z) = scale_factor*get_image(tz+z,fi,stc,FILNUM=idx_image+1)
    endfor
    hdr.tdim = 1
endelse
widget_control,HOURGLASS=0
name = strcompress(string(labels(idx_image),scale_factor, $
            FORMAT='(a,"-",f)'),/REMOVE_ALL)
mom = -1

;if(ptr_valid(fi.paradigm(idx_image))) then begin
;    paradigm = *fi.paradigm(idx_image)
;endif else begin
;    paradigm = fltarr(hdr.tdim)
;    paradigm[*] = 1
;    paradigm[0:hdr.tdim-1] = 0
;endelse
;put_image,scaled_image,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,mom,paradigm,IFH=hdr.ifh
;START150731
put_image,scaled_image,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,mom,IFH=hdr.ifh


fi.color_scale1(fi.n) = !GRAY_SCALE
fi.color_scale2(fi.n) = !NO_SCALE
load_colortable,fi,dsp,TABLE1=!GRAY_SCALE,TABLE2=!NO_SCALE

return
end
