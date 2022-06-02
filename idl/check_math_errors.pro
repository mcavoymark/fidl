;Copyright 12/31/99 Washington University.  All Rights Reserved.
;check_math_errors.pro  $Revision: 1.2 $

;***************************************
pro check_math_errors,report_math_errors
;***************************************

math_errors = check_math() ;If check_math is not called then an error message automatically appears.

if report_math_errors eq !TRUE then begin

    if((math_errors and 1) ne 0) then $
        print,'*** Integer divide by zero. *** '
    if((math_errors and 2) ne 0) then $
        print,'*** Integer overflow *** '
    if((math_errors and 16) ne 0) then $
        print,'*** Floating point divide by zero. ***'
    ;;;if((math_errors and 32) ne 0) then $
    ;;;    print,'*** Floating point underflow. ***'
    if((math_errors and 64) ne 0) then $
        print,'*** Floating point overflow. ***'
    if((math_errors and 128) ne 0) then $
        print,'*** Floating point operand error. ***'

endif

return
end
