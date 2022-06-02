;Copyright 4/21/05 Washington University.  All Rights Reserved.
;get_htr_qtr.pro  $Revision: 1.4 $

;;************************************************************
;;function get_htr_qtr,max_num_levels,total_nsubjects,sng,lc_gf
;function get_htr_qtr,max_num_levels,total_nsubjects,sng
;;************************************************************
;hTR = intarr(max_num_levels)
;qTR = intarr(max_num_levels)
;for m=0,total_nsubjects-1 do begin
;    conditions = *sng[m].conditions
;    index_present = *sng[m].index_present
;    stimlen_vs_t = *sng[m].stimlen_vs_t
;    ;if lc_gf eq 0 then begin
;    ;    print,'gaurav flip is off'
;    ;    offsets = *sng[m].offsets
;    ;endif else begin
;    ;    print,'gaurav flip is on'
;    ;    offsets = *sng[m].offsets_gf
;    ;endelse
;    offsets = *sng[m].offsets
;    for eff=0,sng[m].count_present-1 do begin
;        index_conditions = where(conditions eq index_present[eff],count)
;        ihalfTR_o0 = where(offsets[index_conditions,1] eq 0,chTRo0)
;        ihalfTR_o1 = where(offsets[index_conditions,1] eq 1,chTRo1)
;        iquarterTR_o0 = where(offsets[index_conditions,2] eq 0,cqTRo0)
;        iquarterTR_o1 = where(offsets[index_conditions,2] eq 1,cqTRo1)
;        iquarterTR_o2 = where(offsets[index_conditions,2] eq 2,cqTRo2)
;        iquarterTR_o3 = where(offsets[index_conditions,2] eq 3,cqTRo3)
;        if chTRo0 gt 0 and chTRo1 gt 0 then hTR[index_present[eff]] = hTR[index_present[eff]] + 1
;        if cqTRo0 gt 0 and cqTRo1 gt 0 and cqTRo2 gt 0 and cqTRo3 gt 0 then $
;            qTR[index_present[eff]] = qTR[index_present[eff]] + 1
;    endfor
;endfor
;return,rtn={hTR:hTR,qTR:qTR}
;end

;******************************************************
function get_htr_qtr,max_num_levels,total_nsubjects,sng
;******************************************************
hTR = intarr(max_num_levels)
qTR = intarr(max_num_levels)

qTRo1 = intarr(max_num_levels)
qTRo2 = intarr(max_num_levels)
qTRo3 = intarr(max_num_levels)

;effect_shift = intarr(max_num_levels)
for m=0,total_nsubjects-1 do begin
    conditions = *sng[m].conditions
    index_present = *sng[m].index_present
    stimlen_vs_t = *sng[m].stimlen_vs_t
    offsets = *sng[m].offsets
    for eff=0,sng[m].count_present-1 do begin
        index_conditions = where(conditions eq index_present[eff],count)
        ihalfTR_o0 = where(offsets[index_conditions,1] eq 0,chTRo0)
        ihalfTR_o1 = where(offsets[index_conditions,1] eq 1,chTRo1)
        iquarterTR_o0 = where(offsets[index_conditions,2] eq 0,cqTRo0)
        iquarterTR_o1 = where(offsets[index_conditions,2] eq 1,cqTRo1)
        iquarterTR_o2 = where(offsets[index_conditions,2] eq 2,cqTRo2)
        iquarterTR_o3 = where(offsets[index_conditions,2] eq 3,cqTRo3)



        if chTRo0 gt 0 and chTRo1 gt 0 then hTR[index_present[eff]] = hTR[index_present[eff]] + 1

        ;if chTRo0 gt 0 and chTRo1 gt 0 then $
        ;    hTR[index_present[eff]] = hTR[index_present[eff]] + 1 $
        ;else if chTRo1 gt 0 then $
        ;    effect_shift[index_present[eff]] = .5

        ;if chTRo0 gt 0 and chTRo1 gt 0 then $
        ;    hTR[index_present[eff]] = hTR[index_present[eff]] + 1 $
        ;else if chTRo1 gt 0 then $
        ;    hTR01R[index_present[eff]] = hTR01R[index_present[eff]] + 1 



        ;if cqTRo0 gt 0 and cqTRo1 gt 0 and cqTRo2 gt 0 and cqTRo3 gt 0 then $
        ;    qTR[index_present[eff]] = qTR[index_present[eff]] + 1

        ;if cqTRo0 gt 0 and cqTRo1 gt 0 and cqTRo2 gt 0 and cqTRo3 gt 0 then $
        ;    qTR[index_present[eff]] = qTR[index_present[eff]] + 1 $
        ;else if cqTRo0 eq 0 and cqTRo1 gt 0 and cqTRo2 eq 0 and cqTRo3 eq 0 then $
        ;    effect_shift[index_present[eff]] = .25 $
        ;else if cqTRo0 eq 0 and cqTRo1 eq 0 and cqTRo2 gt 0 and cqTRo3 eq 0 then $
        ;    effect_shift[index_present[eff]] = .5 $
        ;else if cqTRo0 eq 0 and cqTRo1 eq 0 and cqTRo2 eq 0 and cqTRo3 gt 0 then $
        ;    effect_shift[index_present[eff]] = .75

        ;if events not distributed throughout TR, then look for a single systematic shift
        if cqTRo0 gt 0 and cqTRo1 gt 0 and cqTRo2 gt 0 and cqTRo3 gt 0 then $
            qTR[index_present[eff]] = qTR[index_present[eff]] + 1 $
        else if cqTRo0 eq 0 and cqTRo1 gt 0 and cqTRo2 eq 0 and cqTRo3 eq 0 then $
            qTRo1[index_present[eff]] = qTRo1[index_present[eff]] + 1 $
        else if cqTRo0 eq 0 and cqTRo1 eq 0 and cqTRo2 gt 0 and cqTRo3 eq 0 then $
            qTRo2[index_present[eff]] = qTRo2[index_present[eff]] + 1 $
        else if cqTRo0 eq 0 and cqTRo1 eq 0 and cqTRo2 eq 0 and cqTRo3 gt 0 then $
            qTRo3[index_present[eff]] = qTRo3[index_present[eff]] + 1




    endfor
endfor
;return,rtn={hTR:hTR,qTR:qTR}
return,rtn={hTR:hTR,qTR:qTR,qTRo1:qTRo1,qTRo2:qTRo2,qTRo3:qTRo3}
end
