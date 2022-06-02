;Copyright 12/31/99 Washington University.  All Rights Reserved.
;pinv.pro  $Revision: 12.84 $

;**********************
function pinv,A,epsilon
;**********************

; Set epsilon to zero to use the default value.

A1 = double(A)
Mcol = long(n_elements(A[*,0]))
Nrow = long(n_elements(A[0,*]))
pinv1 = dblarr(Mcol,Nrow)
eps = double(epsilon)
passtype = bytarr(5)
passtype(0) = 0
passtype(1) = 0
passtype(2) = 1
passtype(3) = 1
passtype(4) = 0
stat=call_external(!SHARE_LIB,'_pinv',A1,pinv1,Nrow,Mcol,eps,VALUE=passtype)

return,pinv1
end
