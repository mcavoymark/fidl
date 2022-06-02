;Copyright 9/27/06 Washington University.  All Rights Reserved.
;get_sic_get_str.pro  $Revision: 1.4 $

;**************************************************
;function get_sic_get_str,neffect_label,effect_label
function get_sic_get_str,neffect_label,effect_label,ntreatments
;**************************************************
stringentry:


;count = 0
;for i=0,1 do if effect_label[i] eq 'Trend' or effect_label[i] eq 'Baseline' then count = count + 1
;np645 = indgen(neffect_label-count)+1
;if count gt 0 then np645 = [intarr(count),np645]
;np645 = strtrim(np645,2)

count = 0
for i=0,1 do if effect_label[i] eq 'Trend' or effect_label[i] eq 'Baseline' then count = count + 1
np645 = neffect_label-count
count_end = 0
if n_elements(ntreatments) gt 0 then begin 
    count_end = np645 - ntreatments
    np645 = np645 < ntreatments
endif
np645 = indgen(np645)+1
if count gt 0 then np645 = [intarr(count),np645]
if count_end gt 0 then np645 = [np645,intarr(count_end)]
np645 = strtrim(np645,2)

scrap = get_str(neffect_label,effect_label,np645,TITLE=dummy,SPECIAL='Point and click please.', $
    LABEL='First timecourse is 1. Only numbers greater than 0 are considered.  ex. 1 2,3-5',/GO_BACK,FRONT='0')
if scrap[0] eq 'GO_BACK' or scrap[0] eq 'SPECIAL' then return,rtn={msg:scrap[0]}

index_conditions = make_array(neffect_label,neffect_label,/INT,VALUE=-1)
sumrows = intarr(neffect_label)
for i=0,neffect_label-1 do begin
    segments = strsplit(scrap[i],'[ '+string(9B)+',]',/REGEX,/EXTRACT)
    nseg = n_elements(segments)
    start = 0
    for seg=0,nseg-1 do begin
        limits = long(strsplit(segments[seg],'-',/EXTRACT)) - 1
        if n_elements(limits) eq 1 then limits = [limits[0],limits[0]]
        lmax = max(limits,MIN=lmin)
        if lmax gt -1 then begin
            if lmin le -1 then lmin=lmax
            for j=lmin,lmax do begin
                index_conditions[j,sumrows[j]] = i
                sumrows[j] = sumrows[j] + 1
            endfor
        endif
    endfor
endfor
index = where(sumrows gt 0,ntc)
if ntc eq 0 then begin
    scrap = get_button(['Try again','Try again with point and click','go back'], $
        TITLE='You have failed to assign any conditions to timecourses.')
    if scrap eq 0 then goto,stringentry else if scrap eq 1 then return,rtn={msg:'SPECIAL'} else return,rtn={msg:'GO_BACK'}
endif

return,rtn={msg:'',sumrows:sumrows[index],index_conditions:index_conditions[index,*],ntc:ntc}
end
