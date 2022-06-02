;Copyright 8/5/03 Washington University.  All Rights Reserved.
;check_delays.pro  $Revision: 1.4 $

;*******************************************************************************
function check_delays,num_contrasts,clist,num_delays,cnorm,tot_eff,effect_length
;*******************************************************************************
for j=0,num_contrasts-1 do begin

    ;index = where(abs(reform(cnorm[*,clist[j]],tot_eff)) eq 1,count)
    index = where(abs(reform(cnorm[*,clist[j]],tot_eff)) gt .01,count)

    index = where(num_delays gt effect_length[index],count)
    if count ne 0 then begin
        stat = dialog_message('You have specified more delays than then number of estimated frames in the condition.' $
            +string(10B)+'Is your design a block design? Delays can only be applied to single trial designs.',/ERROR)
        return,!ERROR
    endif
endfor
return,!OK
end
