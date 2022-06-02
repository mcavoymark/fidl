;Copyright 12/31/99 Washington University.  All Rights Reserved.
;fidl_anova.pro  $Revision: 12.84 $

;***********************
pro load_overlay_map,dsp
;***********************

; Load color map for activation maps overlaid on anatomy.

tvlct,red,green,blue,/GET
nc = n_elements(red)
scl = .02
C = !LEN_COLORTAB1-21-1/alog(1+scl*235)
for i=0,!LEN_COLORTAB1-21-1 do begin
    red(i) = alog(1+scl*i)*C
    green(i) = alog(1+scl*i)*C
    blue(i) = alog(1+scl*i)*C
endfor
r = [ 0, 8, 6, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5, 7, 9,11,13,15,13,13,14]
g = [12, 8, 6,15,13,11, 9, 7, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 9,13,14]
b = [12,14,11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 5]
for i=0,20 do begin
    red(!LEN_COLORTAB1-21+i) = 17*r(i)
    green(!LEN_COLORTAB1-21+i) = 17*g(i)
    blue(!LEN_COLORTAB1-21+i) = 17*b(i)
end
tvlct,red,green,blue
;set_colors,dsp
;for i=0,nc-1 do $
;    print,i,red(i),green(i),blue(i)

return
end

