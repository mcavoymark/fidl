;Copyright 1/18/07 Washington University.  All Rights Reserved.
;spatial_extent.pro  $Revision: 1.2 $

;*********************************************************************
function spatial_extent,zimage,actmask,xdim,ydim,zdim,threshold,extent
;*********************************************************************
vol = long(xdim)*long(ydim)*long(zdim)
passtype = bytarr(10)
passtype[2] = 1
passtype[3] = 1
passtype[4] = 1
passtype[6] = 1
passtype[7] = 1
passtype[8] = 1
passtype[9] = 1
num_sig_reg = 0L
num_sig_reg = call_external(!SHARE_LIB,'_spatial_extent',zimage,actmask,xdim,ydim,zdim,threshold,1,extent,vol,1,VALUE=passtype)
return,num_sig_reg
end
