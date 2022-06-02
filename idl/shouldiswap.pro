;Copyright 4/23/07 Washington University.  All Rights Reserved.
;shouldiswap.pro  $Revision: 1.2 $

;*****************************
function shouldiswap,bigendian
;*****************************
passtype = bytarr(2)
passtype[0] = 1
passtype[1] = 1
return,call_external(!SHARE_LIB,'_shouldiswap',!SunOS_Linux,bigendian,VALUE=passtype)
end
