;Copyright 12/31/99 Washington University.  All Rights Reserved.
;errorfc.pro  $Revision: 12.83 $

;******************
function errorfc,rv
;******************

length = n_elements(rv)
rv1 = double(rv)
integral = dblarr(length)
passtype = bytarr(3)
passtype(1) = 0
passtype(2) = 1
stat=call_external(!SHARE_LIB,'_errorfc',integral,rv1,length,VALUE=passtype)

return,integral
end
