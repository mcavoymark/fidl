;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_glm.pro  $Revision: 12.84 $

;*****************************************
function get_glm,prof,glm,idx,mom
;*****************************************

A = *glm[mom].A
b = *glm[mom].b
xdim = long(glm[mom].xdim)
ydim = long(glm[mom].ydim)
zdim = long(glm[mom].zdim)
xydim = xdim*ydim
valid_frms = *glm[mom].valid_frms
tcomp = n_elements(A[*,0])

idx = idx[sort(idx)]  ; Make sure indices are in ascending order.
z = idx/(xydim)
pix = idx - z*xydim
y = pix/xdim
x = pix - y*xdim
nind = n_elements(idx1)

if(ptr_valid(glm[mom].mean)) then begin
    mean = *glm[mom].mean
    explicit_mean = !TRUE
endif else begin
    explicit_mean = !FALSE
endelse

tdim_file = glm[mom].tdim/glm[mom].n_files
slices = z[uniq(z)]
nslc = n_elements(slices)
nidx = lonarr(nslc)
for islc=0,nslc-1 do $
    nidx[islc] = total(z eq slices[islc])
maxnidx = max(nidx)

;Retrieve average parameter values.
b1 = fltarr(glm[mom].N)
for i=0L,glm[mom].N-1 do begin
    for islc =0,nslc-1 do begin
        pixtmp = pix[where(z eq slices[islc])]
        y = pixtmp/xdim
        x = pixtmp - y*xdim
        b1[i] = b1[i] + total(b[x,y,slices[islc]+i*zdim])
    endfor
endfor
b1 = b1/n_elements(pix)

xhat = fltarr(glm[mom].tdim)
t = 0
for t1=0,glm[mom].tdim-1 do begin
    ifile = t1/tdim_file
    if(valid_frms[t1] gt 0) then begin
        if(explicit_mean eq !TRUE) then $
            xhat[t1] = mean[x,y,plane-1,ifile] $
        else $
            xhat[t1] = 0.
        for i=0,glm[mom].N-1 do begin
            xhat[t1] = xhat[t1] + A[t,i]*b1[i]
        endfor
        t = t + 1
    endif
endfor

return,xhat
end
