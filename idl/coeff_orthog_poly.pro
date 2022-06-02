;Copyright 11/28/06 Washington University.  All Rights Reserved.
;coeff_orthog_poly.pro  $Revision: 1.2 $

;********************************
function coeff_orthog_poly,poly,k
;********************************
typestr = ''
coeff = intarr(k)
coeffstr = make_array(k,/STRING,VALUE='')
ok_poly_k = 0
if k eq 3 then begin
    if poly eq 1 then $
        coeff = [-1,0,1] $
    else if poly eq 2 then $
        coeff = [1,-2,1] $
    else $ 
        ok_poly_k = 1
endif else if k eq 4 then begin 
    if poly eq 1 then $
        coeff = [-3,-1,1,3] $
    else if poly eq 2 then $
        coeff = [1,-1,-1,1] $
    else if ploy eq 3 then $
        coeff = [-1,3,-3,1] $
    else $
        ok_poly_k = 1 
endif else if k eq 5 then begin
    if poly eq 1 then $
        coeff = [-2,-1,0,1,2] $
    else if poly eq 2 then $
        coeff = [2,-1,-2,-1,2] $
    else if poly eq 3 then $
        coeff = [-1,2,0,-2,1] $
    else if poly eq 4 then $
        coeff = [1,-4,6,-4,1] $
    else $ 
        ok_poly_k = 1
endif else if k eq 6 then begin
    if poly eq 1 then $
        coeff = [-5,-3,-1,1,3,5] $
    else if poly eq 2 then $
        coeff = [5,-1,-4,-4,-1,5] $
    else if poly eq 3 then $
        coeff = [-5,7,4,-4,-7,5] $
    else if poly eq 4 then $
        coeff = [1,-3,2,2,-3,1] $
    else $ 
        ok_poly_k = 1
endif else if k eq 7 then begin
    if poly eq 1 then $
        coeff = [-3,-2,-1,0,1,2,3] $
    else if poly eq 2 then $
        coeff = [5,0,-3,-4,-3,0,5] $
    else if poly eq 3 then $
        coeff = [-1,1,1,0,-1,-1,1] $
    else if poly eq 4 then $
        coeff = [3,-7,1,6,1,-7,3] $
    else $ 
        ok_poly_k = 1
endif else if k eq 8 then begin
    if poly eq 1 then $
        coeff = [-7,-5,-3,-1,1,3,5,7] $
    else if poly eq 2 then $
        coeff = [7,1,-3,-5,-5,-3,1,7] $
    else if poly eq 3 then $
        coeff = [-7,5,7,3,-3,-7,-5,7] $
    else if poly eq 4 then $
        coeff = [7,-13,-3,9,9,-3,-13,7] $
    else if poly eq 5 then $
        coeff = [-7,23,-17,-15,15,17,-23,7] $
    else $ 
        ok_poly_k = 1
endif else if k eq 9 then begin
    if poly eq 1 then $
        coeff = [-4,-3,-2,-1,0,1,2,3,4] $
    else if poly eq 2 then $
        coeff = [28,7,-8,-17,-20,-17,-8,7,28] $
    else if poly eq 3 then $
        coeff = [-14,7,13,9,0,-9,-13,-7,14] $
    else if poly eq 4 then $
        coeff = [14,-21,-11,9,18,9,-11,-21,14] $
    else if poly eq 5 then $
        coeff = [-4,11,-4,-9,0,9,4,-11,4] $
    else $
        ok_poly_k = 1 
endif else if k eq 10 then begin
    if poly eq 1 then $
        coeff = [-9,-7,-5,-3,-1,1,3,5,7,9] $
    else if poly eq 2 then $
        coeff = [6,2,-1,-3,-4,-4,-3,-1,2,6] $
    else if poly eq 3 then $
        coeff = [-42,14,35,31,12,-12,-31,-35,-14,42] $
    else if poly eq 4 then $
        coeff = [18,-22,-17,3,18,18,3,-17,-22,18] $
    else if poly eq 5 then $
        coeff = [-6,14,-1,-11,-6,6,11,1,-14,6] $
    else $
        ok_poly_k = 1 
endif else begin
    ok_poly_k = 2
endelse

if ok_poly_k eq 1 then $ 
    stat=dialog_message('k='+strtrim(k,2)+': Incorrect poly='+strtrim(poly,2),/ERROR) $
else if ok_poly_k eq 2 then $
    stat=dialog_message('Incorrect k='+strtrim(k,2)+' greater than 10',/ERROR) $
else begin
    if poly eq 1 then $
        typestr = 'lin' $
    else if poly eq 2 then $
        typestr = 'quad' $
    else if poly eq 3 then $
        typestr = 'cub' $
    else if poly eq 4 then $
        typestr = 'quar' $
    else if poly eq 5 then $
        typestr = 'quin'


    ;scrap = strtrim(coeff,2)
    ;for i=0,k-1 do begin
    ;    if coeff[i] gt 0 then $
    ;        coeffstr[i]='+' $
    ;    else if coeff[i] lt 0 then $
    ;        coeffstr[i]='-' $
    ;    else if i gt 0 then $
    ;        coeffstr[i]='+' $
    ;    else $
    ;        coeffstr[i]=''
    ;    coeffstr[i] = coeffstr[i] + scrap[i]
    ;endfor

    for i=1,k-1 do if coeff[i] ge 0 then coeffstr[i]='+'
    coeffstr = coeffstr+strtrim(coeff,2)



endelse
rtn = {Cop_struct}
rtn.typestr = typestr
rtn.coeff = ptr_new(coeff)
rtn.coeffstr = ptr_new(coeffstr)
return,rtn
end
