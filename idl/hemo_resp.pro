;Copyright 12/31/99 Washington University.  All Rights Reserved.
;hemo_resp.pro  $Revision: 12.88 $

;*************************************************
function hemo_resp,t,stimlen,PARAMETERS=parameters
;************************************************

;if(t lt 0) then begin
;    hrf = 0.
;    return,hrf
;endif

if t lt 0. then return,0.

if(keyword_set(PARAMETERS)) then begin
    hrf_delta = parameters[0]
    hrf_tau = parameters[1]
endif else begin
    hrf_delta = !HRF_DELTA
    hrf_tau = !HRF_TAU
endelse

passtype = bytarr(4)

;hrf = call_external(!SHARE_LIB,'_boynton_model',t,stimlen,hrf_delta,hrf_tau,VALUE=passtype,/F_VALUE)
tfloat = float(t)
hrf = call_external(!SHARE_LIB,'_boynton_model',tfloat,stimlen,hrf_delta,hrf_tau,VALUE=passtype,/F_VALUE)

;print,'here hemo_resp t=',t,' stimlen=',stimlen,' hrf_delta=',hrf_delta,' hrf_tau=',hrf_tau,' hrf=',hrf
;print,'here hemo_resp t=',t,' stimlen=',stimlen,' hrf=',hrf
;print,'here hemo_resp size(stimlen)=',size(stimlen)

return,hrf
end

;;*****************************************************************
;function hemo_resp,t,stimlen,IMPULSE=impulse,PARAMETERS=parameters
;;*****************************************************************
;if(t lt 0) then begin
;    hrf = 0.
;    return,hrf
;endif
;if(keyword_set(PARAMETERS)) then begin
;    hrf_delta = parameters[0]
;    hrf_tau = parameters[1]
;endif else begin
;    hrf_delta = !HRF_DELTA
;    hrf_tau = !HRF_TAU
;endelse
;tp = t-hrf_delta
;if(keyword_set(IMPULSE) or (stimlen lt .5)) then begin
;    if(tp gt 0) then $
;        hrf = ((tp/hrf_tau)^2)*exp(-tp/hrf_tau) $
;    else $
;        hrf = 0.
;endif else begin
;    if(tp gt stimlen) then begin
;        b = stimlen
;    endif else begin
;        if(tp gt 0) then $
;            b = tp $
;        else $
;            b = 0
;    endelse
;    p1 = ((tp - b)/hrf_tau)^2 + 2.*(tp-b)/hrf_tau + 2.
;    p2 = (tp/hrf_tau)^2 + 2.*tp/hrf_tau + 2.
;    ;print,'t: ',t,' tp : ',tp,' b: ',b,' p1: ',p1,' p2: ',p2
;
;    hrf = 1.93*hrf_tau*(exp(-(tp-b)/hrf_tau)*p1 - exp(-tp/hrf_tau)*p2)
;endelse
;    ;print,'Time: ',t,' HRF(t): ',hrf
;return,hrf
;end
