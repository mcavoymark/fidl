;Copyright 12/31/99 Washington University.  All Rights Reserved.
;reslice.pro  $Revision: 12.88 $
pro reslice,fi,wd,dsp,help,stc
if fi.view[fi.n] ne !TRANSVERSE then begin
    stat = dialog_message('Only transverse views can be resliced.')
    return
endif
view = get_button(['Sagittal','Coronal'],TITLE='Please select view')
if view eq 0 then $
    view = !SAGITTAL $
else $
    view = !CORONAL 

hdr = *fi.hdr_ptr[fi.n]

if hdr.array_type eq !OVERLAY_ARRAY then begin
    fi.color_scale1[fi.n] = !GRAY_SCALE
    fi.color_scale2[fi.n] = !NO_SCALE
    load_colortable,fi,dsp,TABLE1=!GRAY_SCALE,TABLE2=!NO_SCALE
    array_type = !OVERLAY_ARRAY
endif else begin
    array_type = !FLOAT_ARRAY
endelse

widget_control,/HOURGLASS

;START150731
;paradigm = fltarr(3)
;paradigm[*] = -1

imgin = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
case view of
    !SAGITTAL: begin
        xd = hdr.ydim
        yd = hdr.zdim
        zd = hdr.xdim
        ;ydout = float(yd)*hdr.dz/hdr.dxdy
        jimg = fltarr(xd,yd,zd)
        ;resliced_image = fltarr(xd,ydout,zd,hdr.tdim)
        resliced_image = fltarr(xd,yd,zd,hdr.tdim)
        widget_control,/HOURGLASS
        for t=0,hdr.tdim-1 do begin
            for z=0,hdr.zdim-1 do imgin[*,*,z] = get_image(z+t*hdr.zdim,fi,stc,FILNUM=fi.n+1)
            for i=0,xd-1 do begin
                for j=0,yd-1 do begin
                    jimg[i,j,*] = imgin[*,i,j]
                endfor
            endfor
            xmg = reverse(jimg,2)
            resliced_image[*,*,*,t] = xmg
        endfor
        name = strcompress(string(fi.tails(fi.n),FORMAT='(a,"_sagittal")'),/REMOVE_ALL)
        widget_control,/HOURGLASS

        ;put_image,reform(resliced_image,xd,yd,zd*hdr.tdim),fi,wd,dsp,name,xd,yd,zd,hdr.tdim,!FLOAT,hdr.dxdy,hdr.dxdy, $
        ;    hdr.scl,array_type,fi.n,paradigm,MODEL=fi.n,IFH=hdr.ifh,VIEW=view
        ;START150731
        put_image,reform(resliced_image,xd,yd,zd*hdr.tdim),fi,wd,dsp,name,xd,yd,zd,hdr.tdim,!FLOAT,hdr.dxdy,hdr.dxdy, $
            hdr.scl,array_type,fi.n,MODEL=fi.n,IFH=hdr.ifh,VIEW=view

    end
 
    !CORONAL: begin
        xd = hdr.xdim
        yd = hdr.zdim
        zd = hdr.ydim
        ;ydout = float(yd)*hdr.dz/hdr.dxdy
        jimg = fltarr(xd,yd,zd)
        ;resliced_image = fltarr(xd,ydout,zd,hdr.tdim)
        resliced_image = fltarr(xd,yd,zd,hdr.tdim)
        widget_control,/HOURGLASS
        for t=0,hdr.tdim-1 do begin
            for z=0,hdr.zdim-1 do imgin[*,*,z] = get_image(z+t*hdr.zdim,fi,stc,FILNUM=fi.n+1)
            for i=0,xd-1 do begin
                for j=0,yd-1 do begin
                    jimg[i,j,*] = imgin[i,*,j]
                endfor
            endfor
            ;xmg = reverse(congrid(jimg,xd,ydout,zd),2)
            xmg = reverse(jimg,2)
            resliced_image[*,*,*,t] = xmg
        endfor
        name = strcompress(string(fi.tails(fi.n),FORMAT='(a,"_coronal")'),/REMOVE_ALL)
        widget_control,/HOURGLASS

        ;put_image,reform(resliced_image,xd,yd,zd*hdr.tdim),fi,wd,dsp,name,xd,yd,zd,hdr.tdim,!FLOAT,hdr.dxdy,hdr.dxdy, $
        ;    hdr.scl,array_type,fi.n,paradigm,MODEL=fi.n,IFH=hdr.ifh,VIEW=view
        ;START150731
        put_image,reform(resliced_image,xd,yd,zd*hdr.tdim),fi,wd,dsp,name,xd,yd,zd,hdr.tdim,!FLOAT,hdr.dxdy,hdr.dxdy, $
            hdr.scl,array_type,fi.n,MODEL=fi.n,IFH=hdr.ifh,VIEW=view

    end
endcase
widget_control,HOURGLASS=0

return
end
