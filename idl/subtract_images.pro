;Copyright 12/31/99 Washington University.  All Rights Reserved.
;subtract_images.pro  $Revision: 12.89 $
pro subtract_images,fi,st,dsp,wd,stc,help,glm,pref
goback0:
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr1,idx1,'first image',TITLE='Subtract images') ne !OK then return
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr2,idx2,'image to be subtracted',TITLE='Subtract images') ne !OK then return
lc_tdim=0
if hdr1.tdim eq hdr2.tdim and hdr1.tdim gt 1 then begin
    lc_tdim = get_button(['Yes','No','Go back','Exit'],TITLE='Process first frame only?')
    if lc_tdim eq 3 then return else if lc_tdim eq 2 then goto,goback0
endif
if lc_tdim eq 0 then begin
    tdim1 = 1
    tdim2 = 1
endif else begin
    tdim1 = hdr1.tdim
    tdim2 = hdr2.tdim
endelse
x1 = long(hdr1.xdim)*long(hdr1.ydim)*long(hdr1.zdim)*long(hdr1.tdim)
x2 = long(hdr2.xdim)*long(hdr2.ydim)*long(hdr2.zdim)*long(hdr2.tdim)
if x1 ne x2 then begin
    stat = dialog_message('Dimensions of images must be the same.',/ERROR)
    return
endif
sum = fltarr(hdr1.xdim,hdr1.ydim,hdr1.zdim,hdr1.tdim)
widget_control,/HOURGLASS
for t=0,hdr1.tdim-1 do begin
    for z=0,hdr1.zdim-1 do begin
        tz = t*hdr1.zdim + z
        sum(*,*,z,t) = get_image(tz,fi,stc,FILNUM=idx1+1)
        img = get_image(tz,fi,stc,FILNUM=idx2+1)
        sum(*,*,z,t) = sum(*,*,z,t) - img
    endfor
endfor
widget_control,SENSITIVE=0
name = strcompress(string(fi.tails[idx1],fi.tails[idx2],FORMAT='(a,"-",a)'),/REMOVE_ALL)
mom = -1

;paradigm = -1
;put_image,reform(sum,hdr1.xdim,hdr1.ydim,hdr1.zdim*tdim1),fi,wd,dsp,name,hdr1.xdim,hdr1.ydim,hdr1.zdim,tdim1,!FLOAT, $
;    hdr1.dxdy,hdr1.dz,hdr1.scl,!FLOAT_ARRAY,mom,paradigm,IFH=hdr1.ifh
;START150731
put_image,reform(sum,hdr1.xdim,hdr1.ydim,hdr1.zdim*tdim1),fi,wd,dsp,name,hdr1.xdim,hdr1.ydim,hdr1.zdim,tdim1,!FLOAT, $
    hdr1.dxdy,hdr1.dz,hdr1.scl,!FLOAT_ARRAY,mom,IFH=hdr1.ifh

fi.color_scale1[fi.n] = !GRAY_SCALE
fi.color_scale2[fi.n] = !NO_SCALE
load_colortable,fi,dsp,TABLE1=fi.color_scale1[fi.n],TABLE2=fi.color_scale2[fi.n]
end
