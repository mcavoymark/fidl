;Copyright 4/29/02 Washington University.  All Rights Reserved.
;get_labels_from_glm.pro  $Revision: 1.23 $

;function get_labels_from_glm,fi,dsp,wd,glm,help,stc,pref,ifh,glmname,CONTRASTS_ONLY=contrasts_only,START_INDEX_GLM=start_index_glm
;START150429
function get_labels_from_glm,fi,dsp,wd,glm,help,pref,ifh,glmname,CONTRASTS_ONLY=contrasts_only,START_INDEX_GLM=start_index_glm

rtn_get_labels = {Get_labels_struct}
scraplabels = ''
contrastlabels = ''
effectlabels = ''
sum_all_cl = -1
sum_all_el = -1
ncontrastlabels = 0
neffectlabels = 0
index_glm_cl = -1
index_glm_el = -1
effect_length=-1
stimlen=-1
if not keyword_set(START_INDEX_GLM) then start_index_glm=0
for i=0,n_elements(glmname)-1 do begin
    rtn = load_linmod(fi,dsp,wd,glm,help,FILENAME=glmname[i],ifh,/GET_CNORM)
    if rtn.ifh.glm_nc gt 0 then begin
        contrastlabels = [contrastlabels,strtrim(rtn.contrast_labels,2)]
        sum_all_cl = [sum_all_cl,make_array(rtn.ifh.glm_nc,/INT,VALUE=ncontrastlabels)]
        index_glm_cl = [index_glm_cl,make_array(rtn.ifh.glm_nc,/INT,VALUE=i)]
        ncontrastlabels = ncontrastlabels + rtn.ifh.glm_nc 
        if i eq 0 then begin 
            cnorm = rtn.cnorm
        endif else begin
            rtncnorm = rtn.cnorm
            size_cnorm = size(cnorm)
            if size_cnorm[1] lt ifh.glm_tot_eff then begin
                goose = cnorm
                cnorm = fltarr(ifh.glm_tot_eff,size_cnorm[2])
                cnorm[0:size_cnorm[1]-1,*] = goose
            endif else if size_cnorm[0] gt 0 and size_cnorm[1] gt ifh.glm_tot_eff then begin
                rtncnorm = fltarr(size_cnorm[1],ifh.glm_nc)
                rtncnorm[0:ifh.glm_tot_eff-1,*] = rtn.cnorm
            endif
            cnorm = [[cnorm],[rtncnorm]]
        endelse
    endif
    effectlabels = [effectlabels,*rtn.ifh.glm_effect_label]
    sum_all_el = [sum_all_el,make_array(rtn.ifh.glm_all_eff,/INT,VALUE=neffectlabels)]
    index_glm_el = [index_glm_el,make_array(rtn.ifh.glm_all_eff,/INT,VALUE=i)]
    neffectlabels = neffectlabels + rtn.ifh.glm_all_eff 
    effect_length = [effect_length,*rtn.ifh.glm_effect_length]
    stimlen = [stimlen,rtn.stimlen]
endfor
if ncontrastlabels ne 0 then begin
    rtn_get_labels.contrastlabels = ptr_new(strtrim(contrastlabels[1:*],2))
    rtn_get_labels.ncontrastlabels = ncontrastlabels 
    rtn_get_labels.sum_all_cl = ptr_new(sum_all_cl[1:*])
    rtn_get_labels.index_glm_cl = ptr_new(index_glm_cl[1:*] + start_index_glm)
    rtn_get_labels.cnorm = ptr_new(cnorm) ;Used by Average Statistics, T tests
endif
rtn_get_labels.effectlabels = ptr_new(strtrim(effectlabels[1:*],2))
rtn_get_labels.neffectlabels = neffectlabels 
rtn_get_labels.sum_all_el = ptr_new(sum_all_el[1:*])
rtn_get_labels.index_glm_el = ptr_new(index_glm_el[1:*] + start_index_glm)
rtn_get_labels.ifh = rtn.ifh
rtn_get_labels.effect_length = ptr_new(effect_length[1:*])
rtn_get_labels.stimlen = ptr_new(stimlen[1:*])
if keyword_set(CONTRASTS_ONLY) then begin
    rtn_get_labels.nscraplabels = ncontrastlabels
    if ncontrastlabels ne 0 then begin
        rtn_get_labels.scraplabels = rtn_get_labels.contrastlabels 
        rtn_get_labels.sum_all = rtn_get_labels.sum_all_cl 
        rtn_get_labels.index_glm = rtn_get_labels.index_glm_cl 
    endif
endif else begin
    rtn_get_labels.nscraplabels = rtn_get_labels.neffectlabels 
    rtn_get_labels.scraplabels = rtn_get_labels.effectlabels 
    rtn_get_labels.sum_all = rtn_get_labels.sum_all_el 
    rtn_get_labels.index_glm = rtn_get_labels.index_glm_el 
endelse
return,rtn_get_labels
end
