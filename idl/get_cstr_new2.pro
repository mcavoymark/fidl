;Copyright 1/3/11 Washington University.  All Rights Reserved.
;get_cstr_new2.pro  $Revision: 1.5 $
function get_cstr_new2,cstr,sum_contrast_for_treatment,effect_length,conditions_time,sumrows_time,space_char
scrapchar = space_char
index = where(sum_contrast_for_treatment ne 0,count)
for q=0,count-1 do begin
    scrap = 0
    if sum_contrast_for_treatment[q] gt 1 then for r=1,sum_contrast_for_treatment[q]-1 do scrap = scrap + effect_length[r-1]
    scrap = strtrim(scrap+conditions_time,2)

    ;cstr = cstr + scrapchar + scrap[0]
    ;if n_elements(effect_length) gt 0 then $
    ;    goose = sumrows_time < effect_length[sum_contrast_for_treatment[q]-1] $
    ;else $
    ;    goose = sumrows_time
    ;for r=1,goose-1 do cstr = cstr + '+' + scrap[r]
    ;START121221
    if n_elements(effect_length) gt 0 then $
        goose = sumrows_time < effect_length[sum_contrast_for_treatment[q]-1] $
    else $
        goose = sumrows_time
    cstr = cstr + scrapchar + strjoin(scrap[0:goose-1],'+') 

    scrapchar = '+'
endfor
return,rtn={present:count<1}
end
