;Copyright 12/31/99 Washington University.  All Rights Reserved.
;hemo_friston.pro  $Revision: 12.83 $

;**********************
function hemo_friston,t
;**********************

; Compute hemodynamic response.

hrf = fltarr(6)
if(t lt 0) then begin
    return,hrf
endif

hrf[0] = (t^(!HRF_FRISTON_ALPHA1-1))*exp(-t)
hrf[1] = (t^(!HRF_FRISTON_ALPHA2-1))*exp(-t)
hrf[2] = (t^(!HRF_FRISTON_ALPHA3-1))*exp(-t)
hrf[3] = (t^(!HRF_FRISTON_ALPHA1-2))*(!HRF_FRISTON_ALPHA1 - 1 - t)*exp(-t)
hrf[4] = (t^(!HRF_FRISTON_ALPHA2-2))*(!HRF_FRISTON_ALPHA2 - 1 - t)*exp(-t)
hrf[5] = (t^(!HRF_FRISTON_ALPHA3-2))*(!HRF_FRISTON_ALPHA3 - 1 - t)*exp(-t)

return,hrf
end
