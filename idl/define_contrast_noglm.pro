;Copyright 12/2/11 Washington University.  All Rights Reserved.
;define_contrast_noglm.pro  $Revision: 1.7 $
pro define_contrast_noglm,help,contrast_labels,nc,c,cnorm,N,tot_eff,effect_label,effect_length,effect_TR,effect_shift_TR,stimlen, $
    delay,lcfunc,hrf_param,n_interest,hrf_model
n_interest2 = -1
action2 = -1
lcstop = 0 
labels = ['Default contrasts','Form sum','Form difference','Form average','Exit loop']
widget_control,/HOURGLASS
repeat begin
    if action2 eq -1 then $
        action = get_button(labels,TITLE='Please select action.') $
    else begin
        action = action2
        action2 = -1
    endelse
    if n_interest2 eq -1 then begin
        n_interest2 = n_interest
        contrast_labels = ''
        hrf_param = fltarr(2,tot_eff)
        hrf_param[0,*] = !HRF_DELTA
        hrf_param[1,*] = !HRF_TAU
        if n_elements(nc) eq 0 then nc=0
        if nc eq 0 then begin
            if action gt 0 then begin
                action2 = action
                action = 0
            endif
        endif
    endif
    case action of
        0:begin
            widget_control,/HOURGLASS
            rtn=default_contrasts_noglm(effect_label,effect_length,tot_eff,N,effect_TR,effect_shift_TR,stimlen,delay,lcfunc, $
                hrf_param,n_interest,hrf_model)
            nc=rtn.nc
            c=rtn.c
            cnorm=rtn.cnorm
            contrast_labels=rtn.contrast_labels
          end
        1:sum_contrast,help,contrast_labels,nc,c,cnorm,N,tot_eff
        2:difference_contrast,help,contrast_labels,nc,c,cnorm,N,tot_eff,effect_label
        3:avg_contrast,help,contrast_labels,nc,c,cnorm,N,tot_eff
        4:lcstop=1
        else: print,'Invalid choice.'
    endcase
endrep until lcstop eq 1 
end
