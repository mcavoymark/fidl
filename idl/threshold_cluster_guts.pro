;Copyright 1/19/07 Washington University.  All Rights Reserved.
;threshold_cluster_guts.pro  $Revision: 1.2 $
function threshold_cluster_guts,fi,wd,dsp,hdr,zimage,threshold,extent,name
mask = zimage eq !UNSAMPLED_VOXEL
actmask = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
num_sig_reg = spatial_extent(zimage,actmask,hdr.xdim,hdr.ydim,hdr.zdim,threshold,extent)
spider = 'threshold = '+strtrim(threshold,2)+' extent = '+strtrim(extent,2) + ' num_sig_reg = ' + strtrim(num_sig_reg,2)
print,spider
if num_sig_reg eq 0 then $
    stat=dialog_message(spider+string(10B)+string(10B)+'No regions survived!',/INFORMATION) $
else begin
    actmask_bin = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
    index = where(actmask gt 0,count)
    if count ne 0 then actmask_bin[index] = 1
    statmap = (1-mask)*zimage*actmask_bin + mask*!UNSAMPLED_VOXEL
    widget_control,HOURGLASS=0

    ;paradigm= -1.
    ;put_image,statmap,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, $
    ;    !FLOAT_ARRAY,hdr.mother,paradigm,IFH=hdr.ifh
    ;START150731
    put_image,statmap,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother,IFH=hdr.ifh

    fi.color_scale1[fi.n] = !GRAY_SCALE
    fi.color_scale2[fi.n] = !NO_SCALE
    load_colortable,fi,dsp,TABLE1=fi.color_scale1[fi.n],TABLE2=fi.color_scale2[fi.n]
endelse
return,rtn={num_sig_reg:num_sig_reg,actmask:actmask}
end
