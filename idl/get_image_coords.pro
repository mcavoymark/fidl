;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_image_coords.pro  $Revision: 1.2 $

;***********************************************************************************************************
function get_image_coords,xx,yy,zz,x0,y0,view,space,xpad,ypad,zoom,sagittal_face_left,orientation,xdim,ydim, $
    ydim_draw,display_mode
;***********************************************************************************************************

; Voxel coordinate system is zero at the upper left corner of the image at the base of the brain.

case view of
    !TRANSVERSE: begin
        if orientation eq !RADIOL then $
            xin = zoom*(xx-xpad) + x0 $
        else $
            xin = x0 + zoom*(xdim-1-xx+xpad) 
        yin = zoom*(yy-ypad)
        zin = zz
    end
    !SAGITTAL: begin
        if sagittal_face_left eq !FALSE then $
            xin = zoom*(xdim-yy+xpad-1) + x0 $ 
        else $
            xin = zoom*(yy-xpad) + x0
        yin = zoom*(ydim-zz+ypad-1)
        zin = xx
    end
    !CORONAL: begin
        if orientation eq !NEURO then $
            xin = zoom*(xdim-xx+xpad-1) + x0 $
        else $
            xin = x0 + zoom*(xx-xpad)
        yin = zoom*(ydim-zz+ypad-1) 
        zin = yy
    end
    else: begin
        print,'Error in case: get_image_coords'
    end
endcase

if display_mode eq !ANAT_OVER_ACT_MODE then begin
    if yin gt zoom*ydim then yin = yin - zoom*ydim
endif
yin = ydim_draw - yin - 1 - y0


return,v=[xin,yin,zin]
end
