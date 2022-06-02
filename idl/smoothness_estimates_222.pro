;Copyright 12/31/99 Washington University.  All Rights Reserved.
;smoothness_estimates_222.pro  $Revision: 1.4 $

;*************************************
function smoothness_estimates_222,fwhm
;*************************************

;These smoothness estimates were computed through compute_glm for various smoothnesses.
;"fwhm" is in voxels.

;if fix(fwhm) eq 0 then begin
if fwhm le 1.5 then begin
    fwhmx = 2.12
    fwhmy = 1.97
    fwhmz = 3.08
endif else if fix(fwhm) eq 2 then begin
    fwhmx = 3.22
    fwhmy = 2.98
    fwhmz = 3.68
endif else if fix(fwhm) eq 3 then begin
    fwhmx = 4.17
    fwhmy = 3.92
    fwhmz = 4.26
endif else if fix(fwhm) eq 4 then begin
    fwhmx = 5.23
    fwhmy = 5.02
    fwhmz = 5.01
endif else if fix(fwhm) eq 5 then begin
    fwhmx = 6.46
    fwhmy = 6.27
    fwhmz = 5.83
endif else if fix(fwhm) eq 6 then begin
    fwhmx = 5.43
    fwhmy = 5.74
    fwhmz = 5.53
endif else if fix(fwhm) eq 7 then begin
    fwhmx = 6.18
    fwhmy = 6.63
    fwhmz = 6.15
endif else if fix(fwhm) eq 8 then begin
    fwhmx = 6.95
    fwhmy = 7.53
    fwhmz = 6.79
endif else begin
    stat=widget_message('No smoothness estimates available for the FWHM you have chosen.')
    fwhmx = 0. 
    fwhmy = 0. 
    fwhmz = 0. 
endelse
W = (fwhmx*fwhmy*fwhmz)^(1./3.)/double(!VSCL)

rtn = {fwhmx:fwhmx,fwhmy:fwhmy,fwhmz:fwhmz,W:W}
return,rtn
end

