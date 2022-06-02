;Copyright 5/3/02 Washington University.  All Rights Reserved.
;get_hdr_model.pro  $Revision: 1.5 $
function get_hdr_model,fi,glm,hdr,model
hd = *fi.hdr_ptr[fi.n]
hdr = *fi.hdr_ptr[hd.mother]
if hd.model eq fi.n then $
    model = hd.model $ ; Use current file if it is a model.
else $
    model = hdr.model  ; Otherwise use model attached to data file.
hdr = *fi.hdr_ptr[model]
if model lt 0 then begin
    stat = dialog_message('Model estimates are not defined.',/ERROR)
    return,'ERROR'
endif
if not ptr_valid(glm[model].b) then begin
    stat = dialog_message('Model estimates are not defined.',/ERROR)
    return,'ERROR'
endif
if hdr.array_type ne !LINEAR_MODEL then begin
    stat = dialog_message('Designated file is not a glm file.',/ERROR)
    return,'ERROR'
endif
return,'OK'
end
