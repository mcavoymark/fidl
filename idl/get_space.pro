;Copyright 8/10/01 Washington University.  All Rights Reserved.
;get_space.pro  $Revision: 1.7 $
function get_space,xdim,ydim,zdim,VIEW=view
if not keyword_set(VIEW) then view = !TRANSVERSE
if view eq !TRANSVERSE then begin
    x = xdim
    y = ydim
    z = zdim
endif else if view eq !SAGITTAL then begin
    x = zdim
    y = xdim
    z = ydim
endif else begin ;!CORONAL
    x = xdim
    y = zdim
    z = ydim
endelse
if x eq !ATLAS_111_X and y eq !ATLAS_111_Y and z eq !ATLAS_111_Z then $
    space = !SPACE_111 $
else if x eq !ATLAS_222_X and y eq !ATLAS_222_Y and z eq !ATLAS_222_Z then $
    space = !SPACE_222 $
else if x eq !ATLAS_333_X and y eq !ATLAS_333_Y and z eq !ATLAS_333_Z then $
    space = !SPACE_333 $
else if x eq !MNI222_X and y eq !MNI222_Y and z eq !MNI222_Z then $
    space = !SPACE_MNI222 $
else if x eq !MNI333_X and y eq !MNI333_Y and z eq !MNI333_Z then $
    space = !SPACE_MNI333 $

;START170614
else if x eq !MNI111_X and y eq !MNI111_Y and z eq !MNI111_Z then $
    space = !SPACE_MNI111 $

else $ 
    space = !SPACE_DATA
return,space
end
