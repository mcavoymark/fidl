;Copyright 6/1/01 Washington University.  All Rights Reserved.
;get_delays.pro  $Revision: 1.5 $

;***************************************************************************************
function get_delays,delay_str,num_delays,delay_inc,init_delay,type_str,DONT_ASK=dont_ask
;***************************************************************************************
delay_str = ''
if not keyword_set(DONT_ASK) then begin
    lcdelays = get_button(['yes','no','GO_BACK'],TITLE='Do you wish to use delays?')
    if lcdelays eq 1 then $
        return,'no' $
    else if lcdelays eq 2 then $
        return,'GOBACK'
endif




;label = strarr(3)
;value = strarr(3)
;label[0] = 'Number of delays'
;label[1] = 'Delay increment in seconds'
;label[2] = strcompress(string(!HRF_DELTA,FORMAT='("Time-shift from default (",f4.2," seconds)")'))
;value[0] = 3
;value[1] = 1.
;value[2] = 0.
;str = get_str(3,label,value,LABEL=boy_str)
;num_delays = float(str[0])
;delay_inc = float(str[1])
;init_delay = float(str[2])
;type = get_button(['largest absolute tstat.','largest absolute magnitude.'],TITLE='Select delay that yields the', $
;    BASE_TITLE=boy_str)
;case type of
;    0: type_str = 'largest_tstat'
;    1: type_str = 'largest_mag'
;    else: stat=dialog_message('Unknown type in get_delays.',/ERROR)
;endcase

label = strarr(3)
value = strarr(3)
label[0] = 'Number of delays'
label[1] = 'Delay increment in seconds'
label[2] = strcompress(string(!HRF_DELTA,FORMAT='("Time-shift from default (",f4.2," seconds)")'))
value[0] = 3
value[1] = 1.
value[2] = 0.
str = get_str_bool(3,label,['largest absolute tstat.','largest absolute magnitude.'],value,!FALSE, $
    BOOL_TITLE='Select delay that yields the',TITLE='Delay Parameters',LABEL='Boynton model is used for delays.')
num_delays = float(str[0])
delay_inc = float(str[1])
init_delay = float(str[2])
case fix(str[3]) of
    0: type_str = 'largest_tstat'
    1: type_str = 'largest_mag'
    else: stat=dialog_message('Unknown type in get_delays.',/ERROR)
endcase




delay_str = ' -number_of_delays ' + strtrim(str[0],2) $
          + ' -delay_increment_in_seconds ' + strtrim(str[1],2) $
          + ' -initial_delay ' + strtrim(str[2],2) $
          + ' -' + type_str
return,'yes'
end
