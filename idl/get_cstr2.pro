;Copyright 8/17/09 Washington University.  All Rights Reserved.
;get_cstr2.pro  $Revision: 1.2 $
function get_cstr2,sum_contrast_for_treatment,effect_length,conditions_time,sumrows_time,space_char
dummy = sum_contrast_for_treatment
scrapchar = space_char
index = where(dummy ne 0,count)
cstr = ''
for q=0,count-1 do begin
    scrap = 0
    if dummy[q] gt 1 then for r=1,dummy[q]-1 do scrap = scrap + effect_length[r-1]
    scrap = strtrim(scrap+conditions_time,2)
    cstr = cstr + scrapchar + scrap[0]
    if n_elements(effect_length) gt 0 then $
        goose = sumrows_time < effect_length[dummy[q]-1] $
    else $
        goose = sumrows_time
    for r=1,goose-1 do cstr = cstr + '+' + scrap[r]
    scrapchar = '+'
endfor
msg='OK'
return,rtn={msg:msg,cstr:cstr}
end
