;Copyright 12/31/99 Washington University.  All Rights Reserved.
;gauss_smoth.pro  $Revision: 12.84 $
function gauss_smoth,image,fwhm
fwhmxy = fwhm
fwhmz = fwhm
info = size(image)
xdim = long(info(1))
ydim = long(info(2))
if(info[0] gt 2) then $
    zdim = long(info(3)) $
else $
    zdim = 1
if(info[0] eq 4) then $
    img = float(image[*,*,*,0]) $
else $
    img = float(image)
smoth_image = fltarr(xdim,ydim,zdim)
fwhmxyf = float(fwhmxy)
fwhmzf = float(fwhmz)
passtype = bytarr(7)
passtype(0) = 0
passtype(1) = 0
passtype(2) = 1
passtype(3) = 1
passtype(4) = 1
passtype(5) = 0
passtype(6) = 0

;stat=call_external(!SHARE_LIB,'_gauss_smoth',img,smoth_image,xdim,ydim,zdim,fwhmxyf,fwhmzf,VALUE=passtype)
;START150323
stat=call_external(!SHARE_LIB,'_gauss_smoth2',img,smoth_image,xdim,ydim,zdim,fwhmxyf,fwhmzf,VALUE=passtype)

return,smoth_image
end
