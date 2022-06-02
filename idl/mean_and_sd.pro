;Copyright 6/26/01 Washington University.  All Rights Reserved.
;mean_and_sd.pro  $Revision: 1.2 $

;This program differs from idl's stddev in that it excludes !UNSAMPLED_VOXEL's.

;*********************
function mean_and_sd,x
;*********************

index = where(x ne !UNSAMPLED_VOXEL,count)
if count eq 0 then begin
    mean = !UNSAMPLED_VOXEL 
    sd = !UNSAMPLED_VOXEL 
endif else begin
    x = x[index]
    mean = total(x)/float(count)
    sd = stddev(x,/DOUBLE)
endelse

return,rtn = {count:count,mean:mean,sd:sd}
end
