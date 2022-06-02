;Copyright 12/31/99 Washington University.  All Rights Reserved.
;define_pet_dsgn_matrix.pro  $Revision: 12.85 $

;**********************************************************
function define_pet_dsgn_matrix,dsp,fi,stc,pref,wd,help,glm
;**********************************************************

hdr = *fi.hdr_ptr[fi.n]
if((hdr.array_type ne !ASSOC_ARRAY) and (hdr.array_type ne !STITCH_ARRAY)) then begin
     stat = widget_message('Select data file and try again.')
     return,-1
endif

if(keyword_set(DEFAULT)) then $
    lcdefault = !TRUE $
else $
    lcdefault = !FALSE

;Get image header.
hdr = *fi.hdr_ptr(fi.n)

file = dialog_pickfile(/READ,FILTER='*.slk',PATH=fi.path(0), $
                            /NOCONFIRM,TITLE='Sylk file defining design matrix')
if(n_elements(img) eq 1) then begin
    stat=widget_message('Invalid ecat file.',/ERROR)
    return,-1
endif
sz = reform(read_sylk(file,NROWS=1,NCOLS=3,/ARRAY))
nsub = sz[0]
nscan = sz[1]
neff_interest = sz[2]
effect_label = reform(read_sylk(file,NROWS=1,NCOLS=neff_interest,/ARRAY,STARTROW=1))
design_data = read_sylk(file,NROWS=nscan,NCOLS=nsub,/ARRAY,STARTROW=2)

neff_nointerest = nsub
tot_eff = neff_interest
ntotal = neff_interest + neff_nointerest
lcfunc = intarr(tot_eff)
mom = fi.n
glm1 = {General_linear_model}
tdim = nsub*nscan

if(hdr.array_type eq !STITCH_ARRAY) then begin
    tdim_file = stc(mom).tdim_all
    nfile = stc(mom).n
endif else begin
    nfile = 1
    tdim_file = hdr.tdim
endelse

effect_column = indgen(tot_eff)
effect_length = intarr(tot_eff)
effect_length[*] = 1
all_eff = tot_eff
A = fltarr(tdim,ntotal)
valid_frms = fltarr(tdim)
valid_frms[*] = 1
funclen = intarr(tot_eff)
functype = intarr(tot_eff)
funclen[*] = 1

for t=0,tdim-1 do begin
    sub = long(t/nscan)
    scan = t - sub*nscan
    if(design_data[scan,sub] ge 0) then begin
        A[t,design_data[scan,sub]] = 1.
        A[t,neff_interest+sub] = 1.
    endif else begin
        valid_frms[t] = 0  ; Skip bad scans.
    endelse
endfor
nsub = sub
neff_nointerest = nsub
ntotal = neff_interest + neff_nointerest
A = A[*,0:ntotal-1]

; Delete invalid frames.
idx = where(valid_frms)
idx = idx[0:tdim_file-1]
AA = A[idx,*]
A = AA
tdim = n_elements(idx)
Ysim = fltarr(tdim,tot_eff)
valid_frms = valid_frms[idx]
valid_frms = valid_frms[0:tdim-1]

ntotal = neff_interest + neff_nointerest
tcomp = total(valid_frms ne 0)

; Constrain parameters to remove singularity.
A[*,0:neff_interest-1] = A[*,0:neff_interest-1] - 1./float(neff_interest)

ATA = transpose(A)#A
cond_number = cond(ATA,/DOUBLE)
if(lcdefault eq !FALSE) then print,strcompress(string(cond_number,FORMAT='("Condition number:",g12.4)'))

if((cond_number gt 10000) or (cond_number lt 0)) then begin
    svdc,double(ATA),W,U,V,/DOUBLE
    W_scl = W/max(W)
    W_msk = W_scl lt 1.e-11
    n_depend = fix(total(W_msk))
    if(n_depend gt 1) then begin
        print,strcompress(string('Design matrix is rank deficient by ',n_depend,' columns'))
        bad = where(W_msk)
        print,'Dependencies exist among the following columns:',bad
        stat = widget_message('Design matrix contains too many dependencies. The design matrix must be modified.')
    endif
endif


if(ptr_valid(glm1.A) eq !TRUE) then $
    ptr_free,glm1.A
if(ptr_valid(glm1.valid_frms) eq !TRUE) then $
    ptr_free,glm1.valid_frms
if(ptr_valid(glm1.Ysim) eq !TRUE) then $
    ptr_free,glm1.Ysim
glm1.all_eff = all_eff
if(ptr_valid(glm1.effect_column)) then $
    ptr_free,glm1.effect_column
glm1.effect_column = ptr_new(effect_column)
if(ptr_valid(glm1.effect_label)) then $
    ptr_free,glm1.effect_label
glm1.effect_label = ptr_new(effect_label)
if(ptr_valid(glm1.effect_length)) then $
    ptr_free,glm1.effect_length
glm1.effect_length = ptr_new(effect_length)
glm1.A = ptr_new(A)
glm1.valid_frms = ptr_new(valid_frms)
glm1.n_interest = tot_eff
glm1.N = ntotal
glm1.tot_eff = tot_eff
glm1.xdim = hdr.xdim
glm1.ydim = hdr.ydim
glm1.zdim = hdr.zdim
glm1.tdim = tdim
glm1.n_files = nfile
glm1.t_valid = tcomp
glm1.period = 1
glm1.num_trials = -1
glm1.Ysim = ptr_new(Ysim)
glm1.TR = -1
stimlen_dummy = fltarr(tot_eff)
glm1.stimlen = ptr_new(stimlen_dummy)
effect_group = intarr(tot_eff)
glm1.effect_group = ptr_new(effect_group)
glm1.lcfunc = ptr_new(lcfunc)
glm1.funclen = ptr_new(funclen)
glm1.functype = ptr_new(functype)

paradigm = valid_frms[0:tdim-1]
fi.paradigm(mom) = ptr_new(paradigm)
fi.paradigm_loaded[mom] = !TRUE

name = strcompress(string(fi.tails[fi.n],FORMAT='(a,"_glm")'),/REMOVE_ALL)
put_image,image,fi,wd,dsp,name,hdr.xdim,hdr.ydim,1,1,!FLOAT,hdr.dxdy,hdr.dz,scl,!LINEAR_MODEL,mom,paradigm,MODEL=fi.nfiles

glm[fi.n] = glm1

return,0
end
