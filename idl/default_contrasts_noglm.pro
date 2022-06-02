;Copyright 12/05/11 Washington University.  All Rights Reserved.
;default_contrasts_noglm.pro  $Revision: 1.9 $
function default_contrasts_noglm,effect_label,effect_length,tot_eff,N,effect_TR,effect_shift_TR,stimlen,delay,lcfunc,hrf_param, $
    n_interest,hrf_model
contrast_labels = effect_label[0:tot_eff-1]
nc = tot_eff
widget_control,/HOURGLASS
if n_interest gt tot_eff then begin
    period_c = effect_length[0:tot_eff-1]
    if n_elements(hrf_model) eq 0 then begin
        idx = where(lcfunc eq !TIME_COURSE_EST,cnt)
        if cnt eq 0 then $
            hrf_model = !BOYNTON $
        else begin
            scrap = get_button(['Boynton','SPM "Cannonical"'],TITLE='Please select contrast.')
            if scrap eq 0 then hrf_model = !BOYNTON else hrf_model = !SPM_CANNONICAL
        endelse
    endif
    c = fltarr(N,nc)
    cnorm = fltarr(tot_eff,nc)
    i1 = 0
    in = 0
    for eff=0,tot_eff-1 do begin
        cnorm[in,eff] = 1.
        in = in + 1
        if lcfunc[eff] ne !TIME_COURSE_EST then begin
            case lcfunc[eff] of
                !BOYNTON: c[i1:i1+effect_length[eff]-1,eff] = 1.
                !FRISTON: c[i1:i1+effect_length[eff]-1,eff] = 1.
                !SPM_CANNONICAL: c[i1:i1+effect_length[eff]-1,eff] = 1.
                !GAMMA_DERIV: begin
                    c[i1,eff] = !GAMMA_DERIV_ASLOPE
                    c[i1+1,eff] = !GAMMA_DERIV_BSLOPE
                end
                !BOXCAR_STIM: c[i1:i1+effect_length[eff]-1,eff] = 1.

                ;START111221
                -1: print,effect_label[eff]+' is not present'

                else: print,'Invalid function type in default_contrasts_noglm eff=',eff,' lcfunc[eff]=',lcfunc[eff]
            endcase
        endif else begin
            i2 = i1 + period_c[eff] - 1
            if period_c[eff] eq 1 then $ ;behavioral
                c[i1:i2,eff] = 1 $
            else if hrf_model eq !BOYNTON then begin
                str = !BINEXECUTE+'/compute_boynton -hrf_delta '+strtrim(hrf_param[0,eff],2)+' -hrf_tau ' $
                    +strtrim(hrf_param[1,eff],2)+' -TR '+strtrim(effect_TR[eff],2)+' -num_timepoints '+strtrim(period_c[eff],2) $
                    +' -initial_delay_seconds '+strtrim(delay[eff],2)+' -stimlen_seconds '+strtrim(stimlen[eff],2) $
                    +' -magnorm -column -output HRF_ONLY'
                spawn,str,result
                ;print,'eff=',eff,' effect_label[eff]=',effect_label[eff]
                ;print,'str=',str
                ;print,'result=',result
                c[i1:i2,eff] = float(result)
            endif else begin
                c[i1:i2,eff] = get_hrf(hrf_model,period_c[eff],effect_TR[eff],effect_shift_TR[eff],delay[eff],stimlen[eff], $
                    hrf_param[*,eff])
                mean = total(c(i1:i2,eff))/period_c[eff]
                c[i1:i2,eff] = c[i1:i2,eff] - mean
                rsumsq = sqrt(total(c[i1:i2,eff]^2))
                c[i1:i2,eff] = c[i1:i2,eff]/rsumsq
            endelse
        endelse
        i1 = i1 + effect_length[eff]
    endfor
endif else begin
    index = where(strpos(effect_label,'ctl') ge 0,nctl)
    ntask = tot_eff - nctl
    if nctl gt 0 then nc = 2*tot_eff - nctl else nc = tot_eff
    c = fltarr(N,nc)
    cnorm = fltarr(tot_eff,nc)
    for i=0,nc-1 do begin
        j = i mod tot_eff
        c[j,i] = 1.
        cnorm[j,i] = 1.
    endfor
    if nctl gt 0 then begin
        contrast_labels = strarr(nc)
        for i=0,tot_eff-1 do contrast_labels[i] = effect_label[i] + '_ANOVA_OR_T'
        if nctl eq 1 then begin
            c[tot_eff-1,tot_eff:2*tot_eff-2] = -1
            cnorm[tot_eff-1,tot_eff-1:nc-1] = 1
        endif else begin
            for i=0,ntask-1 do begin
                if strpos(effect_label[i+ntask],effect_label[i],/REVERSE_SEARCH) ge 0 then begin
                    c[i+ntask,i+tot_eff] = -1
                    cnorm[i+ntask,i+tot_eff] = 1
                endif
            endfor
        endelse
        for i=nctl,tot_eff-1 do contrast_labels[i+tot_eff-nctl] = effect_label[i-nctl] + '-ctl'
    endif
endelse
check_math_errors,!FALSE
return,rtn={nc:nc,c:c,cnorm:cnorm,contrast_labels:contrast_labels}
end
