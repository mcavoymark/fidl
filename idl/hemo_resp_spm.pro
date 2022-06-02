;Copyright 12/31/99 Washington University.  All Rights Reserved.
;hemo_resp_spm.pro  $Revision: 1.5 $

;************************************
function compute_spmhrf_kernel,h,x,TR
;************************************

if(x eq 0.) then $
    return,1.D
krnl = 0.D
n = double(h - 1)
for i=0.D,n do begin
    factorial_term = 0.D
    for j=0.D,i-1 do begin
        factorial_term = factorial_term + double(alog(n-j))
    end
    if(x gt 0.) then $
        krnl = krnl + $
           exp(factorial_term + (n-i)*alog(x*TR) - x*TR - lngamma(h)) $
    else $
        krnl = krnl + exp(-lngamma(h))
endfor

return,krnl
end

;**********************************************************
function hemo_resp_spm,frame,TR,STIMLEN=stimlen,DELAY=delay
;**********************************************************

; frame, stimlen, and delay are all in units of "frames" expressed in 
; fractional form.

if(not keyword_set(stimlen)) then $
    stimlen = -1.
if(not keyword_set(delay)) then $
    delay = 0.
if(stimlen le 0.) then begin
    x = frame - delay
    if(x gt 0.) then begin
        hrf = exp((!HRF_SPM_RESPONSE_DELAY-1)*alog(x) + $
                       !HRF_SPM_RESPONSE_DELAY*alog(TR)    - $
                       TR*x - lngamma(!HRF_SPM_RESPONSE_DELAY)) - $
              exp((!HRF_SPM_UNDERSHOOT_DELAY-1)*alog(x) + $
                      !HRF_SPM_UNDERSHOOT_DELAY*alog(TR)    - $
                      TR*x -lngamma(!HRF_SPM_UNDERSHOOT_DELAY)) / $
                      !HRF_SPM_MAGNITUDE_RATIO
    endif else begin
        hrf = 0.
    endelse
endif else begin
    T1 = delay/TR
    T2 = (stimlen + delay)/TR
    if(frame lt T1) then begin
        return,0.
    endif 
    if(frame le T2) then begin
        hrf = (compute_spmhrf_kernel(!HRF_SPM_RESPONSE_DELAY,0.,TR)  - $
               compute_spmhrf_kernel(!HRF_SPM_RESPONSE_DELAY,frame-T1,TR))  - $
              (compute_spmhrf_kernel(!HRF_SPM_UNDERSHOOT_DELAY,0.,TR) - $
               compute_spmhrf_kernel(!HRF_SPM_UNDERSHOOT_DELAY,frame-T1,TR))/ $
               !HRF_SPM_MAGNITUDE_RATIO
    endif else begin
        hrf = (compute_spmhrf_kernel(!HRF_SPM_RESPONSE_DELAY,frame-T2,TR)   - $
               compute_spmhrf_kernel(!HRF_SPM_RESPONSE_DELAY,frame-T1,TR))  - $
              (compute_spmhrf_kernel(!HRF_SPM_UNDERSHOOT_DELAY,frame-T2,TR) - $
               compute_spmhrf_kernel(!HRF_SPM_UNDERSHOOT_DELAY,frame-T1,TR))/ $
               !HRF_SPM_MAGNITUDE_RATIO
    endelse
endelse

return,hrf
end
