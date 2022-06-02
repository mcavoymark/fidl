;Copyright 12/18/01 Washington University.  All Rights Reserved.
;get_cstr.pro  $Revision: 1.2 $

;;*****************************************************************************************************
;pro get_cstr_new,cstr,sum_contrast_for_treatment,effect_length,conditions_time,sumrows_time,space_char
;;*****************************************************************************************************
;dummy = sum_contrast_for_treatment
;scrapchar = space_char
;index = where(dummy ne 0,count)
;for q=0,count-1 do begin
;    scrap = 0
;    if dummy[q] gt 1 then for r=1,dummy[q]-1 do scrap = scrap + effect_length[r-1]
;    scrap = strcompress(scrap+conditions_time,/REMOVE_ALL)
;    cstr = cstr + scrapchar + scrap[0]
;
;    if n_elements(effect_length) gt 0 then $
;        goose = sumrows_time < effect_length[dummy[q]-1] $
;    else $
;        goose = sumrows_time
;    for r=1,goose-1 do cstr = cstr + '+' + scrap[r]
;
;    scrapchar = '+'
;endfor
;end

function get_cstr,sum_contrast_for_treatment,effect_length,conditions_time,sumrows_time,space_char,index_glm
if n_elements(index_glm) eq 0 then index_glm = intarr(n_elements(effect_length))
dummy = sum_contrast_for_treatment
scrapchar = space_char
index = where(dummy ne 0,count)
cstr = ''
glmi = intarr(count)
for q=0,count-1 do begin

    ;scrap = 0
    ;if dummy[q] gt 1 then for r=1,dummy[q]-1 do scrap = scrap + effect_length[r-1]
    ;scrap = strtrim(scrap+conditions_time-sum_all_eff[dummy[q]-1],2)
    ;scrap = strtrim(scrap+conditions_time-sum_all_eff[dummy[q]-1],2)

    scrap = 0
    if dummy[q] gt 1 then begin
        for r=1,dummy[q]-1 do begin
            scrap = scrap + effect_length[r-1]
            if index_glm[r] ne index_glm[r-1] then scrap=0
        endfor
    endif
    scrap = strtrim(scrap+conditions_time,2)


    cstr = cstr + scrapchar + scrap[0]

    glmi[q] = index_glm[dummy[q]-1]

    if n_elements(effect_length) gt 0 then $
        goose = sumrows_time < effect_length[dummy[q]-1] $
    else $
        goose = sumrows_time
    for r=1,goose-1 do cstr = cstr + '+' + scrap[r]

    scrapchar = '+'
endfor
msg='OK'
if total(abs(glmi-glmi[0])) ne 0 then begin
    stat=dialog_message('All event types must be from the same glm.',/ERROR)
    msg='ERROR' 
endif
return,rtn={msg:msg,cstr:cstr,glmi:glmi[0]}
end

