;Copyright 11/4/15 Washington University.  All Rights Reserved.
;write_glm_pref.pro  $Revision: 1.4 $
pro write_glm_pref,parf,par
    openw,lu,parf,/GET_LUN
    printf,lu,'#Lines beginning with guess what are ignored'

    idx=where(tag_names(par) eq 'TALK',cnt)
    if cnt ne 0 then begin
        if par.talk eq 1 then scrap='yes' else scrap='no'
        printf,lu,'talk'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'LIST',cnt)
    if cnt ne 0 then printf,lu,'list'+string(9B)+par.list
    undefine,idx

    idx=where(tag_names(par) eq 'VOXELWISE',cnt)
    if cnt ne 0 then begin
        if par.voxelwise eq 1 then scrap='yes' else scrap='no'
        printf,lu,'voxelwise'+string(9B)+scrap
    endif
    undefine,idx


    ;START160425
    idx=where(tag_names(par) eq 'FWHM',cnt)
    if cnt ne 0 then printf,lu,'fwhm'+string(9B)+strjoin(trim(par.fwhm),' ',/SINGLE)
    undefine,idx
    idx=where(tag_names(par) eq 'MASK',cnt)
    if cnt ne 0 then printf,lu,'mask'+string(9B)+par.mask
    undefine,idx



    idx=where(tag_names(par) eq 'SKIP',cnt)
    if cnt ne 0 then printf,lu,'skip'+string(9B)+strjoin(trim(par.skip),' ',/SINGLE)
    undefine,idx

    idx=where(tag_names(par) eq 'SCANNER_PAUSED_DURING_TRIALS',cnt)
    if cnt ne 0 then begin
        if par.scanner_paused_during_trials eq 1 then scrap='yes' else scrap='no'
        printf,lu,'scanner_paused_during_trials'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'TEMPORAL_FILTER',cnt)
    if cnt ne 0 then begin
        if par.temporal_filter eq 1 then scrap='yes' else scrap='no'
        printf,lu,'temporal_filter'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'INTERACTION_TERMS',cnt)
    if cnt ne 0 then begin
        if par.interaction_terms eq 1 then scrap='yes' else scrap='no'
        printf,lu,'interaction_terms'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'CONSTRAIN_DESIGN_MATRIX',cnt)
    if cnt ne 0 then begin
        if par.constrain_design_matrix eq 1 then scrap='yes' else scrap='no'
        printf,lu,'constrain_design_matrix'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'OMIT_FRAMES',cnt)
    if cnt ne 0 then begin
        if par.omit_frames eq 1 then scrap='yes' else scrap='no'
        printf,lu,'omit_frames'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'OMIT_FRAMES_OF_TRIAL_TYPES',cnt)
    if cnt ne 0 then begin
        if par.omit_frames_of_trial_types eq 1 then scrap='yes' else scrap='no'
        printf,lu,'omit_frames_of_trial_types'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'LINEAR_TREND',cnt)
    if cnt ne 0 then begin
        if par.linear_trend eq 1 then scrap='yes' else scrap='no'
        printf,lu,'linear_trend'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'INTERCEPT',cnt)
    if cnt ne 0 then begin
        if par.intercept eq 1 then scrap='yes' else scrap='no'
        printf,lu,'intercept'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'MODEL_ALL_EVENT_TYPES',cnt)
    if cnt ne 0 then begin
        if par.model_all_event_types eq 1 then scrap='yes' else scrap='no'
        printf,lu,'model_all_event_types'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'MASK_REGRESSORS',cnt)
    if cnt ne 0 then begin
        if par.mask_regressors eq 1 then scrap='yes' else scrap='no'
        printf,lu,'mask_regressors'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'REGIONAL_REGRESSORS',cnt)
    if cnt ne 0 then begin
        if par.regional_regressors eq 1 then scrap='yes' else scrap='no'
        printf,lu,'regional_regressors'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'EXTERNAL_REGRESSORS',cnt)
    if cnt ne 0 then begin
        if par.external_regressors eq 1 then scrap='yes' else scrap='no'
        printf,lu,'external_regressors'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'SET_UP_CONTRASTS',cnt)
    if cnt ne 0 then begin
        if par.set_up_contrasts eq 1 then scrap='yes' else scrap='no'
        printf,lu,'set_up_contrasts'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'EFFECT_LABEL',cnt)
    if cnt ne 0 then printf,lu,'effect_label'+string(9B)+strjoin(par.effect_label,' ',/SINGLE)
    undefine,idx

    idx=where(tag_names(par) eq 'STIMLEN',cnt)
    if cnt ne 0 then printf,lu,'stimlen'+string(9B)+strjoin(trim(par.stimlen),' ',/SINGLE)
    undefine,idx

    idx=where(tag_names(par) eq 'DELAY',cnt)
    if cnt ne 0 then printf,lu,'delay'+string(9B)+strjoin(trim(par.delay),' ',/SINGLE) 
    undefine,idx

    ;idx=where(tag_names(par) eq 'ASSUMED_RESPONSE',cnt)
    ;if cnt ne 0 then printf,lu,'assumed_response'+string(9B)+strjoin(trim(par.assumed_response),' ',/SINGLE)
    ;undefine,idx
    ;START160429
    idx=where(tag_names(par) eq 'HRF',cnt)
    if cnt ne 0 then printf,lu,'hrf'+string(9B)+strjoin(par.hrf,' ',/SINGLE)
    undefine,idx

    idx=where(tag_names(par) eq 'LEN_HRF',cnt)
    if cnt ne 0 then printf,lu,'len_hrf'+string(9B)+strjoin(trim(par.len_hrf),' ',/SINGLE)
    undefine,idx

    ;START160429
    ;idx=where(tag_names(par) eq 'FUNCTYPE',cnt)
    ;if cnt ne 0 then printf,lu,'functype'+string(9B)+strjoin(par.functype,' ',/SINGLE)
    ;undefine,idx

    idx=where(tag_names(par) eq 'CONTRAST_GLM',cnt)
    if cnt ne 0 then printf,lu,'contrast_glm'+string(9B)+par.contrast_glm
    undefine,idx

    idx=where(tag_names(par) eq 'FSTAT_GLM',cnt)
    if cnt ne 0 then printf,lu,'fstat_glm'+string(9B)+par.fstat_glm
    undefine,idx

    idx=where(tag_names(par) eq 'R2',cnt)
    if cnt ne 0 then begin
        if par.R2 eq 1 then scrap='yes' else scrap='no'
        printf,lu,'R2'+string(9B)+scrap
    endif
    undefine,idx

    idx=where(tag_names(par) eq 'EXECUTE',cnt)
    if cnt ne 0 then begin 
        if par.execute eq 1 then scrap='yes' else scrap='no'
        printf,lu,'execute'+string(9B)+scrap
    endif
    undefine,idx,cnt,scrap
    close,lu
    free_lun,lu
end
