;Copyright 12/31/99 Washington University.  All Rights Reserved.
;image_to_atlas_space.pro  $Revision: 1.11 $

;function image_to_atlas_space,atlas_space,x_in,y_in,z_in,view
;START150709
function image_to_atlas_space,atlas_space,x_in,y_in,z_in,view,ifh

;print,'image_to_atlas_space x_in=',x_in,' y_in=',y_in,' z_in=',z_in

;START150709
;atlas_pix = fltarr(3)
;atlas_off = fltarr(3)

if atlas_space eq !SPACE_111 then begin
    z = !ATLAS_111_Z - z_in

    ;START150709
    atlas_pix = fltarr(3)
    atlas_off = fltarr(3)

    atlas_pix[0] = !MMPPIX_X_111
    atlas_pix[1] = !MMPPIX_Y_111
    atlas_pix[2] = !MMPPIX_Z_111
    atlas_off[0] = !CENTER_X_111
    atlas_off[1] = !CENTER_Y_111
    atlas_off[2] = !CENTER_Z_111

    ;START150709
    ;xdim = !ATLAS_111_X

    ydim = !ATLAS_111_Y
    zdim = !ATLAS_111_Z
endif else if atlas_space eq !SPACE_222 then begin
    z = !ATLAS_222_Z - z_in

    ;START150709
    atlas_pix = fltarr(3)
    atlas_off = fltarr(3)

    atlas_pix[0] = !MMPPIX_X_222
    atlas_pix[1] = !MMPPIX_Y_222
    atlas_pix[2] = !MMPPIX_Z_222
    atlas_off[0] = !CENTER_X_222
    atlas_off[1] = !CENTER_Y_222
    atlas_off[2] = !CENTER_Z_222

    ;START150709
    ;xdim = !ATLAS_222_X

    ydim = !ATLAS_222_Y
    zdim = !ATLAS_222_Z
endif else if atlas_space eq !SPACE_333 then begin
    z = !ATLAS_333_Z - z_in

    ;START150709
    atlas_pix = fltarr(3)
    atlas_off = fltarr(3)

    atlas_pix[0] = !MMPPIX_X_333
    atlas_pix[1] = !MMPPIX_Y_333
    atlas_pix[2] = !MMPPIX_Z_333
    atlas_off[0] = !CENTER_X_333
    atlas_off[1] = !CENTER_Y_333
    atlas_off[2] = !CENTER_Z_333

    ;START150709
    ;xdim = !ATLAS_333_X

    ydim = !ATLAS_333_Y
    zdim = !ATLAS_333_Z
endif else if atlas_space eq !SPACE_MNI222 then begin
    z = !MNI222_Z - z_in
    atlas_pix=ifh.mmppix
    atlas_off=ifh.center
    ydim = !MNI222_Y
    zdim = !MNI222_Z
endif else if atlas_space eq !SPACE_MNI333 then begin
    z = !MNI333_Z - z_in
    atlas_pix=ifh.mmppix
    atlas_off=ifh.center
    ydim = !MNI333_Y
    zdim = !MNI333_Z

;START170620
endif else if atlas_space eq !SPACE_MNI111 then begin
    z = !MNI111_Z - z_in
    atlas_pix=ifh.mmppix
    atlas_off=ifh.center
    ydim = !MNI111_Y
    zdim = !MNI111_Z

endif else if atlas_space eq !SPACE_DATA then begin
    v = [0,0,0]
    return,v
endif else begin
    print,'Invalid space'
    return,-1
endelse

;print,'image_to_atlas_space view=',view
;print,'image_to_atlas_space x_in=',x_in,' y_in=',y_in,' z=',z
;print,'image_to_atlas_space atlas_pix=',atlas_pix
;print,'image_to_atlas_space atlas_off=',atlas_off


case view of
    !TRANSVERSE: v = float([x_in+1,y_in+1,z])
    !SAGITTAL: v = float([ydim-(y_in+1),zdim-z,x_in+1])
    !CORONAL: v = float([x_in+1,zdim-z+1,y_in+1])
    !VIEW_COLOR_SCALE: v = [-off[0],off[1],off[2]]
    else: begin
        print,'Unknown image orientation.',view
        return,-1
    end
endcase

;pix = atlas_pix[*]
;pix = reform(pix,3)
;off = atlas_off[*]
;v[0] = -pix[0]*v[0] + off[0]
;v[1] =  pix[1]*v[1] - off[1]
;v[2] =  pix[2]*v[2] - off[2]
;START150701
v[0] = -atlas_pix[0]*v[0] + atlas_off[0]
v[1] =  atlas_pix[1]*v[1] - atlas_off[1]
v[2] =  atlas_pix[2]*v[2] - atlas_off[2]

return,v
end
