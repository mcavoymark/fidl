;Copyright 12/31/99 Washington University.  All Rights Reserved.
;zoom_by_2n.pro  $Revision: 1.5 $

;**********************************************
function zoom_by_2n,image,zoom_factor,zoom_type
;**********************************************
xdim = n_elements(image[*,0])
ydim = n_elements(image[0,*])
image_in = float(image)
if(zoom_factor le 1) then begin
    image_out = image
    return,image_out
endif else begin
    image_out = fltarr(xdim*zoom_factor,ydim*zoom_factor)
endelse
 
passtype = bytarr(6)
passtype[2] = 1
passtype[3] = 1
passtype[4] = 1
passtype[5] = 1
stat=call_external(!SHARE_LIB,'_zoom_by_2n',image_in,image_out,xdim,ydim,fix(zoom_factor),zoom_type,VALUE=passtype)

return,image_out
end
