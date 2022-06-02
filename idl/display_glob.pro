;Copyright 12/31/99 Washington University.  All Rights Reserved.
;display_glob.pro  $Revision: 12.84 $

;**********************************************************************************************************
pro display_glob,imgin,min,max,pref,ZOOM=zoom,NPANES=npanes,ORIENT=orient,ANNOTATE=annotate,NCOLORS=ncolors
;**********************************************************************************************************

nargs=n_params()
if(nargs eq 0) then begin
     print,'Usage: display,image[,ZOOM=zoom_factor]
     print,'Displays all 2d images in the 3D array "image".'
     return
endif

if keyword_set(orient) then $
    orientation = orient $
else $
    orientation = !NEURO

if keyword_set(NCOLORS) then $
    len_colortab = ncolors $
else $
    len_colortab = !D.N_COLORS

xdim = n_elements(imgin(*,0,0))
ydim = n_elements(imgin(0,*,0))
zdim = n_elements(imgin(0,0,*))

if keyword_set(zoom) then begin
     zoomx = xdim*zoom
     zoomy = ydim*zoom
endif else begin
     zoomx = xdim
     zoomy = ydim
endelse

min = min(imgin)
max = max(imgin)
scl = len_colortab/(max - min)

if (keyword_set(npanes) eq 0) then npanes = zdim

img = fltarr(xdim,ydim)
for z=0,npanes-1 do begin
    img = scl(0)*(imgin(*,*,z) - min)
    if(orientation eq !NEURO) then $
	img = reverse(img,1)
    if(zoomx ne xdim) then begin
	img = rebin(img,zoomx,zoomy)
    endif
    tv,img,z,/ORDER
endfor


;if(keyword_set(ANNOTATE)) then begin
;    if(orientation eq !NEURO) then $
;        xyouts,0,0,'Neuro',/DEVICE,COLOR=!YELLOW,CHARSIZE=1.1 $
;    else $
;        xyouts,0,0,'Rad',/DEVICE,COLOR=!YELLOW,CHARSIZE=1.1
;endif

if keyword_set(ANNOTATE) and pref.orientation_hide_label eq 0 then begin
    if orientation eq !NEURO then $
        xyouts,0,0,'Neuro',/DEVICE,COLOR= !YELLOW,CHARSIZE=1.1 $
    else $
        xyouts,0,0,'Rad',/DEVICE,COLOR= !YELLOW,CHARSIZE=1.1
endif


return
end
