;Copyright 12/31/99 Washington University.  All Rights Reserved.
;zoom.pro  $Revision: 12.84 $
pro zoom,fi,wd,dsp,help,stc
strs = get_str(2,['X or both zoom factor','Y zoom factor'],['2.','-1'])
zoomx = float(strs[0])
zoomy = float(strs[1])
if(zoomy lt 0) then $
    zoomy = zoomx
method = get_button(['Nearest neighbor','Bilinear','Cubic spline'],TITLE='Select interpolation method.')
widget_control,/HOURGLASS
hdr = *fi.hdr_ptr(fi.n)
imgin = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
image = fltarr(zoomx*hdr.xdim,zoomy*hdr.ydim,hdr.zdim)
for z=0,hdr.zdim-1 do begin
    imgin[*,*,z] = get_image(z,fi,stc,FILNUM=fi.n+1)
    case method+1 of
        !NEAREST_NEIGHBOR: image[*,*,z] = $
            rebin(imgin[*,*,z],hdr.xdim*zoomx,hdr.ydim*zoomy,/SAMPLE)
        !BILINEAR: image[*,*,z] = $
            congrid(imgin[*,*,z],hdr.xdim*zoomx,hdr.ydim*zoomy,/INTERP)
        !CUBIC_SPLINE: image[*,*,z] = $
	    congrid(imgin[*,*,z],hdr.xdim*zoomx,hdr.ydim*zoomy,CUBIC=-.5)
        else: print,'Invalid method.'
    endcase
endfor
widget_control,HOURGLASS=0
dsp[fi.cw].zoom = dsp[fi.cw].zoom/fix(zoomx)
name = strcompress(string(fi.tails(fi.n),zoomx,zoomy,FORMAT='(a,"_zoom",i1,"-",i2)'),/REMOVE_ALL)

;paradigm = fltarr(3)
;paradigm(*) = -1
;put_image,image,fi,wd,dsp,name,zoomx*hdr.xdim,zoomy*hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy/zoomx,hdr.dz,hdr.scl,!FLOAT_ARRAY,fi.n,paradigm,MODEL=fi.n,IFH=hdr.ifh
;START150731
put_image,image,fi,wd,dsp,name,zoomx*hdr.xdim,zoomy*hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy/zoomx,hdr.dz,hdr.scl,!FLOAT_ARRAY,fi.n,MODEL=fi.n,IFH=hdr.ifh

return
end
