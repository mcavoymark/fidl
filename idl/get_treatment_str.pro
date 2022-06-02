;Copyright 10/30/02 Washington University.  All Rights Reserved.
;get_treatment_str.pro  $Revision: 1.3 $
function get_treatment_str,itn
nfactors = itn.nfactors
factor_names = itn.factor_names
nlevels = itn.nlevels
treatments = itn.treatments
level_names = itn.level_names
driver_levels = strarr(treatments[0])
if nfactors eq 0 then begin
    driver_levels[0] = ''
    treatment_str = ''
endif else begin
    driver_table = intarr(treatments[0]*nfactors,/NOZERO)
    for i=nfactors-1,0,-1 do begin
        index = i
        value = 0
        for j=0,treatments[0]/treatments[i+1]-1 do begin
            for k=0,treatments[i+1]-1 do begin
                driver_table[index] = value
                index = index + nfactors
            endfor
            value = value + 1
            if value gt nlevels[i]-1 then value = 0
        endfor
    endfor
    treatment_str = strarr(treatments[0])
    k = 0
    for i=0,treatments[0]-1 do begin
        index = 0
        treatment_str[i] = ''
        driver_levels[i] = ''
        for j=0,nfactors-1 do begin
            if j gt 0 then index = index + nlevels[j-1]
            treatment_str[i] = treatment_str[i] + factor_names[j] + '.' + level_names[index+driver_table[k]] + ' '
            driver_levels[i] = driver_levels[i] + level_names[index+driver_table[k]] + '     '
            k = k + 1
        endfor
    endfor
endelse

;return,rtn={nfactors:nfactors,factor_names:factor_names,nlevels:nlevels,treatments:treatments,level_names:level_names, $
;    treatment_str:treatment_str,driver_levels:driver_levels}
;START120820
return,rtn={nfactors:nfactors,factor_names:factor_names,nlevels:nlevels,treatments:treatments,level_names:level_names, $
    treatment_str:strtrim(treatment_str,2),driver_levels:driver_levels}

end
