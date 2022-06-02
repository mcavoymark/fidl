;Copyright 12/31/99 Washington University.  All Rights Reserved.
;scatter.pro  $Revision: 12.83 $

;*******************************************
function scatter,coords,value,xdim,ydim,zdim
;*******************************************

npts = long(n_elements(coords(0,*)))
xdim = long(xdim)
ydim = long(ydim)
zdim = long(zdim)
value = float(value)
x = long(reform(fix(coords(0,*) + .0001),npts))
y = long(reform(fix(coords(1,*) + .0001),npts))
z = long(reform(fix(coords(2,*) + .0001),npts))

idx = x + xdim*(y + ydim*z)
v = idx gt long(xdim*ydim*zdim-1)
u = idx lt 0
if(total(u) + total(v) gt 0) then begin
    print,'Invalid data passed to scatter.pro'
    image = -1
    return, image
endif

image = fltarr(xdim*ydim*zdim)
passtype = bytarr(9)
passtype(5) = 1
passtype(6) = 1
passtype(7) = 1
passtype(8) = 1
stat=call_external(!SHARE_LIB,'_scatter', $
		image,x,y,z,value,npts,xdim,ydim,zdim,VALUE=passtype)

image = reform(image,xdim,ydim,zdim)

return,image
end
