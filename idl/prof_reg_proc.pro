;Copyright 12/31/99 Washington University.  All Rights Reserved.
;prof_reg_proc.pro  $Revision: 12.83 $

;******************************************************************************
function prof_reg_proc,prof_proc_mode,mother_image,image,mask,x1,x2,y1,y2,dsp,nind_out
;******************************************************************************

xd = x2 - x1 + 1
yd = y2 - y1 + 1
nind = xd*yd

xd = x2 - x1 + 1
yd = y2 - y1 + 1
nind = xd*yd

; Sort values in ascending order.
sind = sort(image[x1:x2,y1:y2])

pct = .5
;;;str = get_str(1,'Cutoff percentile','50')
;;;pct = float(str[0])/100.
cutoff = round(pct*nind)

num = 3
val = 0.
;;;for i=cutoff,nind-1 do begin
for i=nind-num-1,nind-1 do begin
    y0 = sind(i)/xd
    y = y0 + y1
    x = x1 + sind(i) - long(y0)*long(xd)
    val = val  + mother_image[x,y]
    xx = x*dsp[fi.cw].zoom
    yy = dsp[fi.cw].dsizey - 1 - long(y)*long(dsp[fi.cw].zoom)
    side = dsp[fi.cw].zoom/2
    rect_plot,xx-side,xx+side-1,yy-side,yy+side-1,dsp[fi.cw].zoom,COLOR=!GREEN, $
                                ORIENTATION=dsp[fi.cw].orientation
endfor
;;val = val/(nind-cutoff)
val = val/num

nind_out = nind - cutoff

return,val
end
