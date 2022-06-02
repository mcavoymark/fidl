;Copyright 12/31/99 Washington University.  All Rights Reserved.
;display.pro  $Revision: 12.96 $

;******************************************************************************
;pro display,imgin,min,max,pref,ZOOM=zoom,NPANES=npanes,SCL=scl,ORIENT=orient, $
;    FIXED_MIN=fixed_min,ANNOTATE=annotate,NCOLORS=ncolors,INTERP=interp
pro display,imgin,min,max,pref,ZOOM=zoom,NPANES=npanes,SCL=scl,ORIENT=orient, $
    FIXED_MIN=fixed_min,ANNOTATE=annotate,NCOLORS=ncolors,INTERP=interp,VOLSCL=volscl,CONADJ=conadj
;******************************************************************************
nargs=n_params()
if(nargs eq 0) then begin
     print,'Usage: display,image[,ZOOM=zoom_factor]
     print,'Displays all 2d images in the 3D array "image".'
     return
endif

xdim = n_elements(imgin(*,0,0))
ydim = n_elements(imgin(0,*,0))
zdim = n_elements(imgin(0,0,*))

;print,'xdim=',xdim,' ydim=',ydim,' zdim=',zdim

if keyword_set(orient) then orientation = orient else orientation = !NEURO
if keyword_set(INTERP) then interp = interp else interp = !BILINEAR
if keyword_set(NCOLORS) then len_colortab = ncolors else len_colortab = !LEN_COLORTAB1
if keyword_set(zoom) then begin
     zoomx = xdim*zoom
     zoomy = ydim*zoom
endif else begin
     zoomx = xdim
     zoomy = ydim
endelse
if keyword_set(NPANES) eq 0 then npanes = zdim
if not keyword_set(CONADJ) then conadj = 0
if not keyword_set(VOLSCL) then volscl = 0 
;print,'display conadj=',conadj,' volscl=',volscl


;if keyword_set(SCL) then begin
;    lscale = !FALSE 
;    min = 0.
;endif else begin
;    lscale = !TRUE
;endelse


old = 0
if old eq 1 then begin
    if keyword_set(SCL) then begin
        lscale = !FALSE
        min = 0.
    endif else begin
        lscale = !TRUE
    endelse
endif else begin
    if keyword_set(SCL) then begin
        lscale = !FALSE
        min = 0.
    endif else begin
        lscale = !TRUE
        if volscl eq 1 then begin
            max = float(max(imgin,MIN=min))
            min = float(min)
            if conadj eq 1 then begin
                nlevels = 0L
                ilevels = -1L
                icount = -1L
                goose = fix(imgin)
                widget_control,/HOURGLASS
                for i=min,max do begin
                    index = where(goose eq i,count)
                    if count ne 0 then begin 
                        nlevels = nlevels + 1
                        ilevels = [ilevels,index] 
                        icount = [icount,count]
                    endif
                endfor
                if nlevels gt 0 then begin
                    ilevels = ilevels[1:*]
                    icount = icount[1:*]
                    ;print,'n_elements(imgin)=',n_elements(imgin)
                    ;print,'n_elements(levels)=',n_elements(ilevels)
                    ;print,'total(icount)=',total(icount)
                    j = 0
                    for i=0,nlevels-1 do begin
                        imgin[ilevels[j:j+icount[i]-1]] = float(i)
                        j = j + icount[i]
                    endfor
                    min = 0
                    max = nlevels-1
                    max = float(max(imgin,MIN=min))
                    min = float(min)
                endif
            endif
        endif
    endelse
endelse
if keyword_set(FIXED_MIN) then min = fixed_min

;img = fltarr(xdim,ydim)
;for z=0,npanes-1 do begin
;    if lscale eq !TRUE then begin
;        min = float(min(imgin[*,*,z]))
;        max = float(max(imgin[*,*,z]))
;	if(max ne min) then begin
;            scl = (len_colortab-1)/(max - min)
;            img = scl(0)*(imgin(*,*,z) - min)
;	endif else begin
;	    img(*,*) = 0.
;	endelse
;    endif else begin
;	if(scl(0) ne 0.) then begin
;            img = scl(0)*(imgin(*,*,z) - min)
;	    img = img*(img gt 0)
;	    img = img*(img lt len_colortab) + (img ge len_colortab)*(len_colortab-1)
;	endif else begin
;	    img(*,*) = 0.
;	endelse
;    endelse
;    if(zoomx ne xdim) or (zoomy ne ydim) then begin
;         case interp of
;            !NEAREST_NEIGHBOR: img = rebin(img,zoomx,zoomy,/SAMPLE)
;;;;            !NEAREST_NEIGHBOR: img = zoom_by_2n(img,zoom,!NEAREST_NEIGHBOR)
;            !BILINEAR: img = rebin(img,zoomx,zoomy)
;;;;            !BILINEAR: img = zoom_by_2n(img,zoom,!BILINEAR)
;            !CUBIC_SPLINE: img = congrid(img,zoomx,zoomy,CUBIC=-.5)
;            else: begin
;                print,'Invalid zoom algorithm specified, using bilinear.'
;                img = rebin(img,zoomx,zoomy)
;;;;		img = zoom_by_2n(img,zoom,!BILINEAR)
;            end
;        endcase
;    endif
;    if orientation eq !NEURO then img = reverse(img,1)
;    tv,img,z,/ORDER
;endfor

if old eq 1 then begin
    img = fltarr(xdim,ydim)
    for z=0,npanes-1 do begin
        if lscale eq !TRUE then begin
            min = float(min(imgin[*,*,z]))
            max = float(max(imgin[*,*,z]))
           if(max ne min) then begin
                scl = (len_colortab-1)/(max - min)
                img = scl(0)*(imgin(*,*,z) - min)
           endif else begin
               img(*,*) = 0.
           endelse
        endif else begin
           if(scl(0) ne 0.) then begin
                img = scl(0)*(imgin(*,*,z) - min)
               img = img*(img gt 0)
               img = img*(img lt len_colortab) + (img ge len_colortab)*(len_colortab-1)
           endif else begin
               img(*,*) = 0.
           endelse
        endelse
        if(zoomx ne xdim) or (zoomy ne ydim) then begin
             case interp of
                !NEAREST_NEIGHBOR: img = rebin(img,zoomx,zoomy,/SAMPLE)
    ;;;            !NEAREST_NEIGHBOR: img = zoom_by_2n(img,zoom,!NEAREST_NEIGHBOR)
                !BILINEAR: img = rebin(img,zoomx,zoomy)
    ;;;            !BILINEAR: img = zoom_by_2n(img,zoom,!BILINEAR)
                !CUBIC_SPLINE: img = congrid(img,zoomx,zoomy,CUBIC=-.5)
                else: begin
                    print,'Invalid zoom algorithm specified, using bilinear.'
                    img = rebin(img,zoomx,zoomy)
    ;;;            img = zoom_by_2n(img,zoom,!BILINEAR)
                end
            endcase
        endif
        if orientation eq !NEURO then img = reverse(img,1)
        tv,img,z,/ORDER
    endfor
endif else begin
    img = fltarr(xdim,ydim)
    for z=0,npanes-1 do begin
        if lscale eq !TRUE then begin
            if volscl eq 0 then begin
                max = float(max(imgin[*,*,z],MIN=min))
                min = float(min)
                ;print,'z=',z,' max=',max,' min=',min
            endif
            if max ne min then begin
                scl = (len_colortab-1)/(max - min)
                img = imgin[*,*,z]
            endif else begin
                img[*,*] = 0.
            endelse
            if(zoomx ne xdim) or (zoomy ne ydim) then begin
                case interp of
                    !NEAREST_NEIGHBOR: img = rebin(img,zoomx,zoomy,/SAMPLE)
                    !BILINEAR: img = rebin(img,zoomx,zoomy)
                    !CUBIC_SPLINE: img = congrid(img,zoomx,zoomy,CUBIC=-.5)
                    else: begin
                        print,'Invalid zoom algorithm specified, using bilinear.'
                        img = rebin(img,zoomx,zoomy)
                    end
                endcase
            endif
            if max ne min then img = scl[0]*((img < max) - min) > 0.
        endif else begin
            if(scl(0) ne 0.) then begin
                img = scl(0)*(imgin(*,*,z) - min)
                img = img*(img gt 0)
                img = img*(img lt len_colortab) + (img ge len_colortab)*(len_colortab-1)
            endif else begin
                img(*,*) = 0.
            endelse
        endelse
        if(zoomx ne xdim) or (zoomy ne ydim) then begin
             case interp of
                !NEAREST_NEIGHBOR: img = rebin(img,zoomx,zoomy,/SAMPLE)
    ;;;            !NEAREST_NEIGHBOR: img = zoom_by_2n(img,zoom,!NEAREST_NEIGHBOR)
                !BILINEAR: img = rebin(img,zoomx,zoomy)
    ;;;            !BILINEAR: img = zoom_by_2n(img,zoom,!BILINEAR)
                !CUBIC_SPLINE: img = congrid(img,zoomx,zoomy,CUBIC=-.5)
                else: begin
                    print,'Invalid zoom algorithm specified, using bilinear.'
                    img = rebin(img,zoomx,zoomy)
    ;;;             img = zoom_by_2n(img,zoom,!BILINEAR)
                end
            endcase
        endif
        if orientation eq !NEURO then img = reverse(img,1)

        ;petsun24 bombs on this command
        tv,img,z,/ORDER

    endfor
endelse



;if(keyword_set(ANNOTATE)) then begin
;    if(orientation eq !NEURO) then $
;        xyouts,0,0,'Neuro',/DEVICE,COLOR= !YELLOW,CHARSIZE=1.1 $
;    else $
;        xyouts,0,0,'Rad',/DEVICE,COLOR= !YELLOW,CHARSIZE=1.1
;endif

if keyword_set(ANNOTATE) and pref.orientation_hide_label eq 0 then begin
    if orientation eq !NEURO then $
        xyouts,0,0,'Neuro',/DEVICE,COLOR= !YELLOW,CHARSIZE=1.1 $
    else $
        xyouts,0,0,'Rad',/DEVICE,COLOR= !YELLOW,CHARSIZE=1.1
endif

	
return
end
