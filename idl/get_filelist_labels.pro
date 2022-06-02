;Copyright 8/16/02 Washington University.  All Rights Reserved.
;get_filelist_labels.pro  $Revision: 1.10 $

;pro get_filelist_labels,fi,nlabels,labels,index,nlabels_glm,labels_glm,index_glm,nlabels_conc,labels_conc,index_conc
;START140825
pro get_filelist_labels,fi,nlabels,labels,index,nlabels_glm,labels_glm,index_glm,nlabels_conc,labels_conc,index_conc,NAMES=names

;print,'fi.tails'
;print,fi.tails
;print,'fi.list'
;print,fi.list
;print,'fi.names'
;print,fi.names

nlabels = 0
nlabels_glm = 0
nlabels_conc = 0
if fi.nfiles gt 0 then begin
    array_type = intarr(fi.nfiles)
    for i=0,fi.nfiles-1 do begin
        hdr = *fi.hdr_ptr[i]
        array_type[i] = hdr.array_type
    endfor

    index = where(array_type ne !LINEAR_MODEL,nlabels)
    index_glm = where(array_type eq !LINEAR_MODEL,nlabels_glm)
    index_conc = where(array_type eq !ASSOC_ARRAY or array_type eq !STITCH_ARRAY,nlabels_conc)

    ;index = where(array_type ne !LINEAR_MODEL,nlabels,COMPLEMENT=index_glm,NCOMPLEMENT=nlabels_glm) IDL 5.4

    ;if nlabels ne 0 then labels = fi.tails[index]
    ;START140825
    if nlabels ne 0 then begin
        if not keyword_set(NAMES) then labels = fi.tails[index] else labels = fi.names[index]
    endif


    if nlabels_glm ne 0 then labels_glm = fi.list[index_glm]
    if nlabels_conc ne 0 then labels_conc = fi.tails[index_conc]
endif
end
