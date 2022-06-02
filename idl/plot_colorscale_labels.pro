;Copyright 10/26/00 Washington University.  All Rights Reserved.
;plot_colorscale_labels.pro  $Revision: 1.14 $

;pro plot_colorscale_labels,min,max,x1,y1,x2,y2,colorbarlabels,TITLE=title,HORIZONTAL=horizontal,INVERT=invert, $
;    COLOR=color,DISPLAY_P=display_p,ROUNDED=rounded
;START51
pro plot_colorscale_labels,min,max,x1,y1,x2,y2,colorbarlabels,title,vert_hor,invert,display_p,rounded,COLOR=color


;if keyword_set(DISPLAY_P) or keyword_set(ROUNDED) then lc_prob = !TRUE else lc_prob = !FALSE
;START51
;if display_p eq 1 or rounded eq 1 then lc_prob = !TRUE else lc_prob = !FALSE
lc_prob = !FALSE
if n_elements(display_p) ne 0 then begin
    if display_p eq 1 then lc_prob = !TRUE
endif else begin
    display_p=0
endelse
if n_elements(rounded) ne 0 then begin
    if rounded eq 1 then lc_prob = !TRUE
endif else begin
    rounded=0
endelse


if not keyword_set(COLOR) then color = !WHITE
minmax = fltarr(2)


;if keyword_set(INVERT) then begin
;    lc_invert = !TRUE
;    minmax[0] = -max
;    minmax[1] = -min
;endif else begin
;    lc_invert = !FALSE
;    minmax[0] = min
;    minmax[1] = max
;endelse
;START51
if n_elements(invert) eq 0 then invert=0 
if invert eq 1 then begin
    minmax[0] = -max
    minmax[1] = -min
endif else begin
    minmax[0] = min
    minmax[1] = max
endelse





if n_elements(colorbarlabels) then if colorbarlabels[0] ne '' then labels = colorbarlabels 
if n_elements(labels) eq 0 then begin
    nval = 10
    x = alog10(minmax[1] - minmax[0])
    if x gt 0 then pow = fix(x) else pow = fix(x) - 1
    mantissa = x - float(pow)
    case !TRUE of
        (mantissa lt alog10(2)): inc = .2*10.^pow
        (mantissa lt alog10(5)): inc = .5*10.^pow
        else: inc = 1.*10.^pow
    endcase
    nval = fix((minmax[1]-minmax[0])/inc) + 2
    if nval lt 2 then nval = 2
    yvals = fltarr(nval)
    if lc_prob then begin
        fmt = '(e9.1)'
        fmt1 = '(e9.1)'
        min_spacing = 35
    endif else begin
        min_spacing = 20
        case !TRUE of
            (inc gt 10000): fmt = '(e8.0)'

            ;(inc ge 1): if not keyword_set(HORIZONTAL) then fmt = '(f6.0)' else fmt = '(i6)'
            ;START51
            (inc ge 1): if vert_hor eq 0 then fmt = '(f6.0)' else fmt = '(i6)'

            (inc ge .1): fmt = '(f6.1)'
            (inc gt .01): fmt = '(f6.2)'
            (inc gt .001): fmt = '(f6.3)'
            (inc gt .0001): fmt = '(f8.4)'
            else: fmt = '(e8.0)'
        endcase
        case !TRUE of ; Format for highest value.
            (inc gt 10000): fmt1 = '(e10.2)'
            (inc ge 1): fmt1 = '(f8.2)'
            (inc ge .1): fmt1 = '(f8.3)'
            (inc gt .01): fmt1 = '(f8.4)'
            (inc gt .001): fmt1 = '(f8.5)'
            (inc gt .0001): fmt1 = '(f8.6)'
            else: fmt1 = '(e10.2)'
        endcase
    endelse
    yvals[0] = minmax[0]
    yvalmin = inc*(fix(minmax[0]/inc))
    yvals[1] = inc*(fix(yvalmin/inc))
    for i=2,nval-1 do yvals[i] = yvalmin + inc*(i-1)
    ;print,'here0 yvals=',yvals


    ;ylabs = fltarr(nval+1)
    ;ylabs[0:nval-1] = yvals
    ;ylabs[nval] = minmax[1]
    ;ylabs[1] = yvalmin
    ;START41
    if yvals[nval-1] lt minmax[1] then begin
        ylabs = fltarr(nval+1)
        ylabs[0:nval-1] = yvals
        ylabs[nval] = minmax[1]
    endif else begin
        yvals[nval-1] = minmax[1]
        ylabs = yvals 
        nval = nval-1
    endelse
    ylabs[1] = yvalmin

    ;print,'here0a yvals=',yvals
    ;print,'here0a ylabs=',ylabs


    ;if lc_invert then ylabs = -ylabs
    ;START51
    if invert eq 1 then ylabs = -ylabs

    if lc_prob then ylabs = (2.*(ylabs gt 0) - 1.)*t_pdfc(ylabs,1.e8)
    labels = strarr(nval+1)

    ;if not keyword_set(HORIZONTAL) then begin
    ;START51
    if vert_hor eq 0 then begin

        labels[0] = strcompress(string(ylabs[0],FORMAT=fmt1),/REMOVE_ALL)
        labels[nval] = strcompress(string(ylabs[nval],FORMAT=fmt1),/REMOVE_ALL)
        for i=1,nval-1 do labels[i] = strcompress(string(ylabs[i],FORMAT=fmt),/REMOVE_ALL)
    endif else $
        for i=0,nval do labels[i] = strcompress(string(ylabs[i],FORMAT=fmt),/REMOVE_ALL)

    ;if keyword_set(ROUNDED) then begin
    ;START51
    if rounded eq 1 then begin

        for i=0,nval do begin 
            scrap = strsplit(labels[i],'e',/EXTRACT)
            labels[i] = strcompress(string(10.^double(fix(scrap[1])+1),FORMAT=fmt),/REMOVE_ALL)
        endfor
    endif
 
    amin = minmax[0]
    amax = minmax[1]
    denom = amax-amin

    ;if keyword_set(HORIZONTAL) then begin
    ;START51
    if vert_hor eq 1 then begin

        for ival = 0,nval-1 do begin
            ss = (yvals[ival]-amin)/denom
            if ss ge 0 and ss le 1 then begin
                x = x1 + (x2-x1)*ss
                if (x2-x) gt min_spacing then begin
                    plots,[x,x],[y1,y2],/DEVICE,COLOR=color
                    if lc_prob then $
                        xyouts,x+3,y2+8+!COLSCL_WIDTH,labels[ival],/DEVICE,CHARSIZE=1.5,COLOR=color,ORIENTATION=90,ALIGNMENT=0.5 $
                    else $
                        xyouts,x-5,y2+4,labels[ival],/DEVICE,CHARSIZE=1.5,COLOR=color
                endif
            endif
        endfor
        plots,[x2,x2],[y1,y2],/DEVICE,COLOR=color
        if lc_prob then $
            xyouts,x2+3,y2+8+!COLSCL_WIDTH,labels(nval),/DEVICE,CHARSIZE=1.5,COLOR=color,ORIENTATION=90,ALIGNMENT=0.5 $
        else $
            xyouts,x2,y2+4,labels[nval],/DEVICE,CHARSIZE=1.5,COLOR=color
    endif else begin

        ;print,'here10 labels=',labels
        ;print,'here10 nval=',nval

        for ival = 0,nval-1 do begin
            ss = (yvals(ival)-amin)/denom
            ;print,'before labels[ival]=',labels[ival],' ss=',ss
            if ss ge 0 and ss le 1 then begin
                y = y1 + (y2-y1)*ss
                ;print,'y2-y=',y2-y

                ;if (y2-y) gt 20 then begin
                ;START41
                if (y2-y) gt 15 then begin

                    plots,[x1,x2],[y,y],/DEVICE,COLOR=color
                    xyouts,x2+4,y-5,labels[ival],/DEVICE,CHARSIZE=1.5,COLOR=color
                    ;print,'labels[ival]=',labels[ival]
                endif
            endif
        endfor
        plots,[x1,x2],[y2,y2],/DEVICE,COLOR=color
        xyouts,x2+4,y2,labels[nval],/DEVICE,CHARSIZE=1.5,COLOR=color
    endelse
endif else begin
    x = alog10(minmax[1] - minmax[0])
    if x gt 0 then pow = fix(x) else pow = fix(x) - 1
    mantissa = x - float(pow)
    case !TRUE of
        (mantissa lt alog10(2)): inc = .2*10.^pow
        (mantissa lt alog10(5)): inc = .5*10.^pow
        else: inc = 1.*10.^pow
    endcase

    nval = n_elements(labels)
    yvals = fltarr(nval)
    yvals[0] = min + inc/2
    for i=1,nval-1 do yvals[i] = yvals[i-1] + inc
    fraction = 0 
    for i=1,nval-1 do begin
        yvals[i] = yvals[i-1] + inc*(1+fraction) 
        fraction = fraction + 3./(max-min) 
    endfor

    amin = minmax[0]
    amax = minmax[1]
    denom = amax-amin
    ss = (yvals-amin)/denom
    x = x1 + (x2-x1)*ss

    ;if keyword_set(HORIZONTAL) then begin
    ;START51
    if vert_hor eq 1 then begin

        for ival=0,nval-1 do begin
            if ival mod 2 eq 0 then begin
                plots,[x[ival],x[ival]],[y1,y2],/DEVICE,COLOR=color
                xyouts,x[ival],y2+4,labels[ival],/DEVICE,CHARSIZE=1.5,COLOR=color,ALIGNMENT=0.5
            endif else begin
                yy = y1 - !COLSCL_WIDTH
                plots,[x[ival],x[ival]],[yy,yy-5],/DEVICE,COLOR=color
                xyouts,x[ival],yy-20,labels[ival],/DEVICE,CHARSIZE=1.5,COLOR=color,ALIGNMENT=0.5
            endelse
        endfor
    endif else begin
        for ival = 0,nval-1 do begin
            ss = (yvals[ival]-amin)/denom
            if ss ge 0 and ss le 1 then begin
                y = y1 + (y2-y1)*ss
                if (y2-y) gt 20 then begin
                    plots,[x1,x2],[y,y],/DEVICE,COLOR=color
                    xyouts,x2+4,y-5,labels[ival],/DEVICE,CHARSIZE=1.5,COLOR=color
                endif
            endif
        endfor
    endelse

endelse


;if keyword_set(TITLE) then xyouts,x1,y1/2,title,/DEVICE,CHARSIZE=1.5,COLOR=color
;START51
if title ne '' then xyouts,x1,y1/2,title,/DEVICE,CHARSIZE=1.5,COLOR=color

end
