;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_assoc.pro  $Revision: 12.84 $

;;*******************
;function get_assoc,x
;;*******************
;if(!VERSION.OS_FAMILY eq 'Windows') then begin
;   xp = swap_endian(x)
;endif else begin
;   xp = float(x)
;endelse
;return,xp
;end

function get_assoc,x,bigendian
xp = x
if !VERSION.OS_FAMILY eq 'Windows' then $ 
    xp = swap_endian(xp) $
else if shouldiswap(bigendian) then $
    xp = swap_endian(xp)
return,xp
end
