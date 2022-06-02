;Copyright 8/21/03 Washington University.  All Rights Reserved.
;get_cft_and_scft.pro  $Revision: 1.11 $
function get_cft_and_scft,sumrows,search_str,sum_contrast_for_treatment,time,nsubjects, $
    glmfiles,ntreatments,labels,repeats,match,notpresent
n_search_str = sumrows
for m=0,nsubjects-1 do begin
    junk = strsplit(glmfiles[m],'/',/EXTRACT)
    junk = junk[n_elements(junk)-1]
    scraplabels = *labels[m]
    scraplabels_index = indgen(n_elements(scraplabels))
    for i=0,ntreatments-1 do begin
        if repeats eq 1 then begin
            scraplabels = *labels[m]
            scraplabels_index = indgen(n_elements(scraplabels))
        endif
        temp = search_str[i,*] ;necessary for search_str to be updated

        ;j = assign_cft_and_scft_new(sumrows[i],temp,sum_contrast_for_treatment,m,i,time, $
        ;    n_search_str[i],scraplabels,scraplabels_index,choice,match,GLM_NAME=junk)
        ;START55
        j = assign_cft_and_scft_new(sumrows[i],temp,sum_contrast_for_treatment,m,i,time, $
            n_search_str[i],scraplabels,scraplabels_index,choice,match,notpresent,GLM_NAME=junk)

        if j le 0 then return,j
        search_str[i,*] = temp
        if j gt n_search_str[i] then n_search_str[i] = j
    endfor
endfor
return,0
end
