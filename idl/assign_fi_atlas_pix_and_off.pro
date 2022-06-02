;Copyright 01/02/02 Washington University.  All Rights Reserved.
;assign_fi_atlas_pix_and_off.pro  $Revision: 1.5 $
pro assign_fi_atlas_pix_and_off,fi,ifh
if fi.space[fi.n] eq !SPACE_111 then begin
    fi.atlas_pix[*,fi.n] = [!MMPPIX_X_111,!MMPPIX_Y_111,!MMPPIX_Z_111]
    fi.atlas_off[*,fi.n] = [!CENTER_X_111,!CENTER_Y_111,!CENTER_Z_111]
endif else if fi.space[fi.n] eq !SPACE_222 then begin
    fi.atlas_pix[*,fi.n] = [!MMPPIX_X_222,!MMPPIX_Y_222,!MMPPIX_Z_222]
    fi.atlas_off[*,fi.n] = [!CENTER_X_222,!CENTER_Y_222,!CENTER_Y_222]
endif else if fi.space[fi.n] eq !SPACE_333 then begin
    fi.atlas_pix[*,fi.n] = [!MMPPIX_X_333,!MMPPIX_Y_333,!MMPPIX_Z_333]
    fi.atlas_off[*,fi.n] = [!CENTER_X_333,!CENTER_Y_333,!CENTER_Z_333]

;START160129
;endif else if fi.space[fi.n] eq !SPACE_MNI222 then begin
;    fi.atlas_pix[*,fi.n] = [!MMPPIX_X_MNI222,!MMPPIX_Y_MNI222,!MMPPIX_Z_MNI222]
;    fi.atlas_off[*,fi.n] = [!CENTER_X_MNI222,!CENTER_Y_MNI222,!CENTER_Z_MNI222]

endif else if n_elements(ifh) gt 0 then begin
    fi.atlas_pix[*,fi.n] = ifh.mmppix
    fi.atlas_off[*,fi.n] = ifh.center
endif
end
