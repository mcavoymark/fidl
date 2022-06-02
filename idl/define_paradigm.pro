;Copyright 12/31/99 Washington University.  All Rights Reserved.
;define_paradigm.pro  $Revision: 12.83 $

;*********************************
function define_paradigm,tdim,pref
;*********************************

; Define stimulus time course.

status = !OK
paradigm = fltarr(tdim+1)
value = strarr(5)
label = strarr(5)
value(0) = string(pref.paraskip)
value(1) = string(pref.parainit)
value(2) = string(pref.paraact)
value(3) = string(pref.paractl)
value(4) = string('')
label(0) = string("Number of frames to skip: ")
label(1) = string("Initial control period: ")
label(2) = string("Activation period: ")
label(3) = string("Control period: ")
label(4) = string("Encoded paradigm")
vals = get_str(5,label,value)

skip = fix(vals(0))
rest1 = fix(vals(1))
act = fix(vals(2))
rest = fix(vals(3))
mri_paradigm_format = vals(4)

if(strlen(mri_paradigm_format) eq 0) then begin
    if(skip gt 0) then $
        paradigm(0:skip-1) = 0
    if(rest1 gt 0) then $
        paradigm(skip:skip+rest1-1) = -1
    ncycles = tdim/(act+rest) + 1
    j = skip+rest1
    for i=0,ncycles-1 do begin
        if(j+act lt tdim) then begin
            paradigm(j:j+act-1) = 1
            if(j+act+rest le tdim) then $
                paradigm(j+act:j+act+rest-1) = -1 $
            else $
                paradigm(j+act:tdim-1) = -1
        endif else begin
    	    if(j lt tdim) then $
                paradigm(j:tdim) = 1
        endelse
        j = j + act + rest
    endfor
    status = !OK
endif else begin
    paradigm = convert_paradigm(mri_paradigm_format,tdim)
    if(n_elements(paradigm) eq 0) then begin
        msg = string('Invalid paradigm code.')
        status = !ERROR
	rtn = {name:'',status:status,msg:'',paradigm:paradigm}
	return,rtn
    endif
endelse

code = encode_paradigm(paradigm,tdim)

rtn = {name:'',status:status,msg:'',paradigm:paradigm}
return,rtn
end
