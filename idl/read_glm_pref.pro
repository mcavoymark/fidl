;Copyright 9/25/15 Washington University.  All Rights Reserved.
;read_glm_pref.pro  $Revision: 1.6 $
function read_glm_pref,file
glmprf={dummy:''}
line=''
openr,lu,file,/GET_LUN
while not eof(lu) do begin
    readf,lu,line
    s=strsplit(line,/EXTRACT)
    if strmid(s[0],0,1) eq '#' then begin
        print,'read_glm_pref Ignoring '+line
    endif else begin
        case s[0] of
            'talk':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'list':scrap=s[1]
            'voxelwise':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0

            ;START160425
            'fwhm':scrap=float(s[1]) 
            ;'mask':scrap=s[1]
            ;START160426
            'mask':if n_elements(s) eq 1 then scrap='' else scrap=s[1]

            'skip':scrap=fix(s[1:*]) 
            'scanner_paused_during_trials':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'temporal_filter':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'interaction_terms':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'constrain_design_matrix':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'omit_frames':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'omit_frames_of_trial_types':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'linear_trend':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'intercept':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'model_all_event_types':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'mask_regressors':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'regional_regressors':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'external_regressors':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'set_up_contrasts':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'effect_label':scrap=s[1:*]
            'stimlen':scrap=float(s[1:*]) 
            'delay':scrap=float(s[1:*]) 

            ;'assumed_response':scrap=fix(s[1:*])
            ;START160429
            'hrf':scrap=s[1:*]

            'len_hrf':scrap=fix(s[1:*])

            ;START160429
            ;'functype':scrap=s[1:*]


            'contrast_glm':scrap=s[1]
            'fstat_glm':scrap=s[1]
            'R2':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            'execute':if strupcase(s[1]) eq 'YES' then scrap=1 else scrap=0
            else:print,'read_glm_pref No match for '+s[0]
        endcase
        if n_elements(scrap) ne 0 then begin 
            glmprf=create_struct(glmprf,s[0],scrap)
            undefine,scrap
        endif
    endelse
endwhile
free_lun,lu
return,glmprf
end
