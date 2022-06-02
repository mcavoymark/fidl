;Copyright 10/5/01 Washington University.  All Rights Reserved.
;t4_atlas.pro  $Revision: 1.6 $
function t4_atlas,image,xdim,ydim,zdim,lpxy,lpz,orientation,t4,twoAis0_twoBis1_unknown2,atlas_image,atlas,center_in
if get_atlas_param(atlas,mmppix,center,ap_xdim,ap_ydim,ap_zdim,area,vol,voxel_size,str) eq !ERROR then return,!ERROR
atlas_image = fltarr(ap_xdim,ap_ydim,ap_zdim)
passtype = bytarr(12)
passtype[3] = 1
passtype[4] = 1
passtype[5] = 1
passtype[8] = 1
passtype[9] = 1
passtype[10] = 1
widget_control,/HOURGLASS

;stat = call_external(!SHARE_LIB,'_t4_atlas',image,atlas_image,t4,xdim,ydim,zdim,lpxy,lpz,fix(twoAis0_twoBis1_unknown2), $
;    orientation,atlas,VALUE=passtype,/I_VALUE)
;START120523
;if n_elements(center_in) eq 0 then $
;    stat = call_external(!SHARE_LIB,'_t4_atlas',image,atlas_image,t4,xdim,ydim,zdim,lpxy,lpz,fix(twoAis0_twoBis1_unknown2), $
;        orientation,atlas,VALUE=passtype[0:10],/I_VALUE) $
;else $
;    stat = call_external(!SHARE_LIB,'_t4_atlas',image,atlas_image,t4,xdim,ydim,zdim,lpxy,lpz,fix(twoAis0_twoBis1_unknown2), $
;        orientation,atlas,center_in,VALUE=passtype,/I_VALUE)
;START150212
stat=0L
if n_elements(center_in) eq 0 then $
    stat = call_external(!SHARE_LIB,'_t4_atlas',image,atlas_image,t4,xdim,ydim,zdim,lpxy,lpz,fix(twoAis0_twoBis1_unknown2), $
        orientation,atlas,VALUE=passtype[0:10]) $
else $
    stat = call_external(!SHARE_LIB,'_t4_atlas',image,atlas_image,t4,xdim,ydim,zdim,lpxy,lpz,fix(twoAis0_twoBis1_unknown2), $
        orientation,atlas,center_in,VALUE=passtype)


return,stat
end
