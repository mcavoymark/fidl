;Copyright 10/01/01 Washington University.  All Rights Reserved.
;get_hrf.pro  $Revision: 1.5 $

;********************************************************************
function get_hrf,hrf_model,period,TR,shift_TR,delay,stimlen,hrf_param
;********************************************************************

hrf = fltarr(period)
if hrf_model eq !BOYNTON then begin
    for i=0,period-1 do begin

        ;t = TR*i + TR*shift_TR - delay
        ;if t lt 0 then  t = 0.

        t = TR*i + TR*shift_TR - delay > 0.

        hrf[i] = hemo_resp(t,stimlen,PARAMETERS=hrf_param)
    endfor
endif else begin
    ;Must be SPM "cannonical" model.
    for i=0,period-1 do begin
        adjusted_frame = float(i) - delay/TR + shift_TR
        if adjusted_frame lt 0 then adjusted_frame = 0
        hrf[i] = hemo_resp_spm(adjusted_frame,TR,STIMLEN=stimlen,DELAY=delay)
    endfor
endelse

return,hrf
end
