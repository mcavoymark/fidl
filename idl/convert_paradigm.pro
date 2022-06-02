;Copyright 12/31/99 Washington University.  All Rights Reserved.
;convert_paradigm.pro  $Revision: 12.84 $

;*************************************************
function convert_paradigm,mri_paradigm_format,tdim
;*************************************************

if(!VERSION.OS_FAMILY eq 'Windows') then begin
    paradigm = fltarr(tdim)
    paradigm[!GLM_SKIP:tdim-1] = -1
    return,paradigm
endif

str = string(!BIN,'/expandf "',mri_paradigm_format,'"')
case n_elements(str) of
    1: cmd = str
    2: cmd = str(0) + str(1)
    3: cmd = str(0) + str(1) + str(2)
    4: cmd = str + str(1) + str(2) + str(3)
    else: cmd = str + str(1) + str(2) + str(3) + str(4)
endcase
spawn,cmd,str
case n_elements(str) of
    1: paradigm_string = str
    2: paradigm_string = str(0) + str(1)
    3: paradigm_string = str(0) + str(1) + str(2)
    4: paradigm_string = str + str(1) + str(2) + str(3)
    else: paradigm_string = str + str(1) + str(2) + str(3) + str(4)
endcase
lenstr = strlen(str)
lenstr = lenstr[0]
paradigm = fltarr(lenstr+1)
for t=1,lenstr do begin
    code = strmid(paradigm_string,t-1,1) ; Extract character
    if(n_elements(code) gt 1) then begin
    cd = code(0)
    for j=1,n_elements(code)-1 do $
        cd = cd + code(j)
    code = cd
      endif
    case code(0) of
    '-': paradigm(t-1) = -1
    '+': paradigm(t-1) =  1
    'x': paradigm(t-1) = 0
    else: begin
        paradigm(*) = 1.
        paradigm(0) = -2
      end
    endcase
endfor

return,paradigm
end
