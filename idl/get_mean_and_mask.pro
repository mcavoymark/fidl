;Copyright 6/1/01 Washington University.  All Rights Reserved.
;get_mean_and_mask.pro  $Revision: 1.2 $

;***************************************************
function get_mean_and_mask,grand_mean,xdim,ydim,zdim
;***************************************************

mean_mask = grand_mean le !GRAND_MEAN_THRESH
if(total(mean_mask) gt .9*n_elements(mean_mask)) then begin
    ;More than 90% of voxels in grand mean less than 100.  Must not be unprocessed image data.
    mean = fltarr(xdim,ydim,zdim,/NOZERO)
    mean[*,*,*] = 1.
    mean_mask = fltarr(xdim,ydim,zdim)
    pct_chng_scl = 1.
endif else begin
    mean = grand_mean
    pct_chng_scl = 100.
endelse

return,rtn = {mean:mean,mean_mask:mean_mask,pct_chng_scl:pct_chng_scl}
end
