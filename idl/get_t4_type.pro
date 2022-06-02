;Copyright 10/5/01 Washington University.  All Rights Reserved.
;get_t4_type.pro  $Revision: 1.3 $

;***************************
function get_t4_type,t4_file
;***************************

twoAis0_twoBis1_unknown2 = 2.
passtype = bytarr(2)
passtype[0] = 1
passtype[1] = 0
stat=call_external(!SHARE_LIB,'_twoA_or_twoB',t4_file,twoAis0_twoBis1_unknown2,VALUE=passtype)
if twoAis0_twoBis1_unknown2 eq 2. then begin
    stat = dialog_message('t4 filename is unrecognizable. Must include either 2A or 2B, but not both.',/ERROR)
endif

return,twoAis0_twoBis1_unknown2
end
