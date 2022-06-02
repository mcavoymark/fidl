;Copyright 3/8/05 Washington University.  All Rights Reserved.
;cft_and_scft.pro  $Revision: 1.15 $
function cft_and_scft,total_nsubjects,ntreatments,length,sumrows,index_conditions,labels,get_labels_struct,time,glmfiles, $
    treatment_str,nfactors,dummy,scraplabels,REPEATS=repeats,MATCH=match,SEARCH_STR=search_str,NOTPRESENT=notpresent
lcmess = 0
if not keyword_set(REPEATS) then repeats = 0 
if not keyword_set(MATCH) then match = 0 
if not keyword_set(SEARCH_STR) then begin
    search_str = strarr(ntreatments,100)
    for i=0,ntreatments[0]-1 do search_str[i,0:sumrows[i]-1] = labels[index_conditions[i,0:sumrows[i]-1]]
endif
if not keyword_set(NOTPRESENT) then notpresent=0
sum_contrast_for_treatment = intarr(total_nsubjects,ntreatments,length)


;if n_elements(scraplabels) eq 0 then begin
;    scraplabels = replicate(ptr_new(),total_nsubjects)
;    for m=0,total_nsubjects-1 do scraplabels[m] = get_labels_struct[m].scraplabels
;endif
;START110819
if n_elements(scraplabels) eq 0 then begin
    scraplabels = replicate(ptr_new(),total_nsubjects)
    if time eq 0 then $ 
        for m=0,total_nsubjects-1 do scraplabels[m] = get_labels_struct[m].effectlabels $
    else $ 
        for m=0,total_nsubjects-1 do scraplabels[m] = get_labels_struct[m].contrastlabels
endif


scrap = get_cft_and_scft(sumrows,search_str,sum_contrast_for_treatment,time,total_nsubjects, $
    glmfiles,ntreatments,scraplabels,repeats,match,notpresent)
if scrap eq -2 then $
    return,rtn={sum_contrast_for_treatment:-2} $
else if scrap eq -1 then begin ;SELECT MANUALLY
    lcmess = 1
    m = 0
    repeat begin
        goback13:
        labels = *get_labels_struct[m].scraplabels
        length_label = n_elements(labels) 
        labels_index = indgen(length_label)
        rtn_ic = identify_conditions_new(length_label,labels,ntreatments,treatment_str,time,nfactors,dummy,TOP_TITLE=glmfiles[m])
        if rtn_ic.special eq 'GO_BACK' then begin
            if m eq 0 then begin
                return,rtn={sum_contrast_for_treatment:-1}
            endif else begin
                m = m - 1
                goto,goback13
            endelse
        endif
        sumrows = rtn_ic.sumrows
        index_conditions = rtn_ic.index_conditions
        scraplabels = labels ;protects labels from being modified by assign_cft_and_scft_new
        for i=0,ntreatments-1 do begin
            search_str = strarr(10)
            search_str[0:sumrows[i]-1] = labels[index_conditions[i,0:sumrows[i]-1]]
            rtn = get_labels_struct[m]
            j = assign_cft_and_scft_new(sumrows[i],search_str,sum_contrast_for_treatment, $
                m,i,time,sumrows[i],scraplabels,labels_index,RTN=rtn)
        endfor
        m = m + 1
    endrep until m eq total_nsubjects
endif
counts=intarr(total_nsubjects,ntreatments)
for j=0,total_nsubjects-1 do begin
    for i=0,ntreatments-1 do begin
        index = where(sum_contrast_for_treatment[j,i,*] gt 0,count)
        counts[j,i]=count
    end
end
return,rtn={sum_contrast_for_treatment:sum_contrast_for_treatment,lcmess:lcmess,counts:counts}
end
