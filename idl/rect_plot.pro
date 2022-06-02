;Copyright 12/31/99 Washington University.  All Rights Reserved.
;rect_plot.pro $Revision: 12.86 $

;*******************************************************************************
pro rect_plot,x1,x2,y1,y2,zoom,COLOR=color,ORIENTATION=orientation,DRAW_SIZE=draw_size
;*******************************************************************************
 
;if (not keyword_set(COLOR)) then $
;    color = !GREEN $
;else $
;    color = colr

if(not keyword_set(COLOR)) then color = !GREEN 

if keyword_set(DRAW_SIZE) then $
    drawsize = draw_size $
else $
    drawsize = !DRAW_SIZEX

;if(not keyword_set(ORIENTATION)) then begin
;    if(orient eq !NEURO) then begin
;        xx = drawsize - 1  - x1
;        x1 = drawsize - 1  - x2
;        x2 = xx
;    endif
;endif

if(keyword_set(ORIENTATION)) then begin
    if(orientation eq !NEURO) then begin
        xx = drawsize - 1  - x1
        x1 = drawsize - 1  - x2
        x2 = xx
    endif
endif

 
x = intarr(5)
y = intarr(5)
 
x(0) = x1
y(0) = y1
x(1) = x1
y(1) = y2
x(2) = x2
y(2) = y2
x(3) = x2
y(3) = y1
x(4) = x1
y(4) = y1

plots,x,y,/DEV,/NOCLIP,COLOR=color
 
return
end
