;$Id: define_dsgn_matrix.pro,v 12.82 1999/12/16 20:48:59 jmo Exp $
;******************************************
function define_dsgn_matrix,dsp,fi,stc,pref
;******************************************

;Get image header.
hd = *fi.hdr_ptr(fi.n)
hdr = *fi.hdr_ptr(hd.mother)

value = strarr(1)
label = strarr(1)
value(0) = string('1')
label(0) = string("Number of variables (columns): ")
vals = get_str(1,label,value)
ncols = fix(vals(0))
slpcol = ncols

value = strarr(ncols)
label = strarr(ncols)
value(*) = '0'
value(0) = '1'
for i=0,ncols-1 do $
    label(i) = string(i+1,FORMAT='("Contrast for column ",i2,": ")')
vals = get_str(ncols,label,value)

if(hd.array_type eq !STITCH_ARRAY) then begin
    N = ncols + 2*stc(hd.mother).n
    c = fltarr(N)
    for i=0,ncols-1 do $
        c(i) = fix(vals(i))
    G = fltarr(hdr.tdim,ncols)
    valid_frms = fltarr(hdr.tdim)
    tdim1 = stc(hd.mother).tdim
    for i=0,ncols-1 do begin
        rtn = define_paradigm(tdim1,pref)
        if(rtn.status ne !OK) then begin
            stat = widget_message(rtn.msg,/ERROR)
            i = i - 1
        endif else begin
            G(0:tdim1-1,i) = rtn.paradigm(0:tdim1-1)
        endelse
        valid_frms = valid_frms + abs(G(*,i))
    endfor
;   Repeat paradigm for each run.
    for file=1,stc(hd.mother).n-1 do begin
        valid_frms(file*tdim1:(file+1)*tdim1-1) = valid_frms(0:tdim1-1)
	G(file*tdim1:(file+1)*tdim1-1,*) = G(0:tdim1-1,*)
    endfor
    tcomp = total(valid_frms gt 0)
    A = fltarr(tcomp,N)
    j = 0
    for file=0,stc(hd.mother).n-1 do begin
        t0 = file*stc(hd.mother).tdim
        t1 = (file+1)*stc(hd.mother).tdim - 1
	k = 0
        for i=t0,t1 do begin
	    if(valid_frms(k) gt 0) then begin
		A(j,0:ncols) = G(k,*)
                A(j,slpcol) = k
                A(j,slpcol+1) = 1
		j = j + 1
	    endif
    	    k = k + 1
	endfor
	slpcol = slpcol + 2
    endfor
endif else begin
    N = ncols + 2
    c = fltarr(N)
    for i=0,ncols-1 do $
        c(i) = fix(vals(i))
    G = fltarr(hdr.tdim,ncols)
    valid_frms = fltarr(hdr.tdim)
    for i=0,ncols-1 do begin
        rtn = define_paradigm(hdr.tdim,pref)
        if(rtn.status ne !OK) then begin
            stat = widget_message(rtn.msg,/ERROR)
            i = i - 1
        endif else begin
            G(*,i) = rtn.paradigm(0:hdr.tdim-1)
        endelse
        valid_frms = valid_frms + abs(G(*,i))
    endfor
    tcomp = total(valid_frms gt 0)
    A = fltarr(tcomp,ncols+2)
    j = 0
    for i=0,hdr.tdim-1 do begin
	if(valid_frms(i) gt 0) then begin
	    A(j,0:ncols-1) = G(i,*)
            A(j,N-2) = i
	    j = j + 1
	endif
    endfor
    A(*,slpcol+1) = 1.
endelse

glm = {General_linear_model}
glm.A = ptr_new(A)
glm.c = ptr_new(c)
glm.valid_frms = ptr_new(valid_frms)
glm.n_interest = ncols
glm.N = N

return,glm
end
