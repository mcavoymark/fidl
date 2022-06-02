;Copyright 12/31/99 Washington University.  All Rights Reserved.
;logical_xor.pro  $Revision: 1.8 $

;*********************************
pro logical_xor,fi,wd,dsp,help,stc
;*********************************

; Compute logical xor of two images

labels = strarr(fi.nfiles)
labels(0:fi.nfiles-1) = fi.tails(0:fi.nfiles-1)
lclist = get_bool_list(labels,TITLE='Select z-images to combine.')
lclist = lclist.list
nfiles = total(lclist)
if(nfiles lt 2) then begin
   stat = widget_message('Less than two files were specified.  Try again.')
   return
endif
name = 'Logical_xor_'
for ifile=0,n_elements(lclist)-1 do begin
    if(lclist[ifile] ne 0) then $
        name = name + string(ifile,FORMAT='(i2,"_")')
endfor
str = get_str(1,'Name',name)
name = str[0]

first = !TRUE
widget_control,/HOURGLASS
for ifile=0,n_elements(lclist)-1 do begin
    if(lclist[ifile] ne 0) then begin
        if(first eq !TRUE) then begin
            first = !FALSE
            hdr = *fi.hdr_ptr(ifile)
            if(hdr.array_type ne !LOGICAL_ARRAY) then begin
                stat = widget_message('Images must be binary values.')
                return
            endif
            image = lonarr(hdr.xdim,hdr.ydim,hdr.zdim)
            for z=0,hdr.zdim-1 do $
                image[*,*,z] = long(get_image(z,fi,stc,FILNUM=ifile+1))
         endif else begin
            hdr1 = *fi.hdr_ptr(ifile)
            if(hdr1.xdim ne hdr.xdim or hdr1.ydim ne hdr.ydim or hdr1.zdim ne hdr.zdim) then begin
                stat = widget_message('All images must have the same dimensions.')
                return
            endif
            if(hdr1.array_type ne !LOGICAL_ARRAY) then begin
                stat = widget_message('Images must be binary values.')
                return
            endif
            for z=0,hdr.zdim-1 do $
                image[*,*,z] = image[*,*,z] xor long(get_image(z,fi,stc,FILNUM=ifile+1))
         endelse
    endif
endfor
image = float(image)
name = strcompress(name,/REMOVE_ALL)

;paradigm = -2
;put_image,image,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!LOGICAL_ARRAY,hdr.mother,paradigm,IFH=hdr.ifh
;START150731
put_image,image,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!LOGICAL_ARRAY,hdr.mother,IFH=hdr.ifh

fi.color_scale1(fi.n) = !GRAY_SCALE
fi.color_scale2(fi.n) = !NO_SCALE
load_colortable,fi,dsp,TABLE1=!GRAY_SCALE,TABLE2=!NO_SCALE

return
end
