;Copyright 12/31/99 Washington University.  All Rights Reserved.
;define_cov_dsgn_matrix.pro  $Revision: 12.91 $

;******************************************************************************
function define_cov_dsgn_matrix,dsp,fi,stc,pref,wd,help,roi,glm,DEFAULT=default
;******************************************************************************

if(keyword_set(DEFAULT)) then $
    lcdefault = !TRUE $
else $
    lcdefault = !FALSE

nn = 0
labels = strarr(fi.nfiles+1)
index = intarr(fi.nfiles)
for idx=0,fi.nfiles-1 do begin
    hdr = *fi.hdr_ptr(idx)
    if((hdr.array_type eq !ASSOC_ARRAY) or (hdr.array_type eq !STITCH_ARRAY)) then begin
        labels[nn] = fi.tails[idx]
        index[nn] = idx
        nn = nn + 1
    endif
endfor
labels = labels[0:nn-1]
idx = get_button(labels,TITLE='Data.')
idx_data = index[idx]
hdr = *fi.hdr_ptr(idx_data)

if((hdr.array_type ne !ASSOC_ARRAY) and (hdr.array_type ne !STITCH_ARRAY)) then begin
     stat = widget_message('Select data file and try again.')
     return,-1
endif

if(help.enable eq !TRUE) then begin
    widget_control,help.id,SET_VALUE=string('Defines the paradigm based on a file supplied by the user.'+ ' This file can be defined using Excel templates at http://imaging.wustl.edu/Ollinger/fidl/fmri_analysis.htm')
endif
num_trials = 0
nfh = 0
effects = 0
frames = 0
stimlen = 0
lcfunc = 0
labels = ''

glm1 = {General_linear_model}

if(hdr.array_type eq !STITCH_ARRAY) then begin
    nslope = stc(idx_data).n
    tdim = stc(idx_data).tdim_all
    nfile = stc(idx_data).n
endif else begin
    nslope = 1
    nfile = 1
    tdim = hdr.tdim
endelse

hrf = fltarr(tdim)
Ysim = fltarr(tdim)
effect_column = 0
effect_length = 0
effect_length = 0
effect_label = ''

if(hdr.array_type eq !STITCH_ARRAY) then $
    t_to_file = *stc(idx_data).t_to_file $
else $
    t_to_file = intarr(tdim)

skip = 4
valid_frms = fltarr(tdim)
valid_frms[*] = 1
for t=0,tdim-1 do begin
    ifile = t_to_file[t]
    if(ifile gt 0) then $
        tfile = t - stc(idx_data).tdim_sum[ifile] $
    else $
        tfile = t
    if(tfile lt skip) then $
        valid_frms[t] = 0
endfor
for file=0,nfile-1 do begin
    if(hdr.array_type eq !STITCH_ARRAY) then $
        tdim_file = stc(idx_data).tdim_file[file] $
    else $
        tdim_file = tdim
endfor

effect_length = intarr(2)
effect_label = strarr(2)
effect_column = intarr(2)

image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
lcglob = get_boolean('Remove global covariate',['Yes','No'])
if(lcglob eq !TRUE) then begin
;    Compute mask of gray and white matter for global covariate.
    for z=0,hdr.zdim-1 do $
        image(*,*,z) = get_image(z,fi,stc,FILNUM=idx_data+1)

    mask_tmp = image gt 200
    mask_image = image*mask_tmp
    mean1 = total(mask_image)/total(mask_tmp)

    thresh1 = .5*mean1
    thresh2 = 1.8*mean1
    brain_mask = float((image gt thresh1)*(image lt thresh2))
    nbrain = total(brain_mask)
    effect_column[0] = 0
    effect_length[0] = nfile
    effect_label[0] = 'Global_Change'
    ntotal = nfile
    tot_eff = 1
    slpcol = nfile
endif else begin
    ntotal = 0
    tot_eff = 0
    slpcol = 0
endelse

str = get_str(2,['Number of frequency components','TR'],['4','2.50'])
num_freq = fix(str[0])
TR = float(str[1])
effect_column[tot_eff] = nfile
effect_length[tot_eff] = num_freq*2*nfile
effect_label[tot_eff] = 'High-pass_filter'
all_eff = tot_eff + 1
ntotal = ntotal + 2*num_freq*nfile

tcomp = 0
t0 = 0
k = 0
for file=0,nfile-1 do begin
    if(hdr.array_type eq !STITCH_ARRAY) then $
        tdim_file = stc(idx_data).tdim_file[file] $
    else $
        tdim_file = tdim
    t1 = t0 + tdim_file - 1
    for i=0,tdim_file-1 do begin
        if(valid_frms[k] gt 0) then $
            tcomp = tcomp + 1
        k = k + 1
    endfor
    t0 = t0 + tdim_file
endfor

if(ntotal gt 0) then begin
    A = fltarr(tdim,ntotal)
    j = 0
    k = 0
    t0 = 0
    widget_control,/HOURGLASS
    for file=0,nfile-1 do begin
        if(hdr.array_type eq !STITCH_ARRAY) then $
            tdim_file = stc(idx_data).tdim_file[file] $
        else $
            tdim_file = tdim
        t1 = t0 + tdim_file - 1
        if(t1 gt tdim-1) then $
            t1 = tdim - 1
        fmin = 1./(tdim_file*TR)
        t2 = 0
        fscl = 2*!PI/(tdim_file-skip)
        t3 = j
        for i=long(t0),t1 do begin
            if(((i+1) mod 25) eq 0) then $
                widget_control,wd.error,SET_VALUE=string('Frame: ',i+1)
            if(valid_frms[k] gt 0) then begin
                if(lcglob eq !TRUE) then begin
                    for z=0L,hdr.zdim-1 do $
                        image(*,*,z) = get_image(z+hdr.zdim*i,fi,stc,FILNUM=idx_data+1)
                    global_covariate = total(image*brain_mask)/nbrain
                    A[j,file] = global_covariate
                endif
                for ifreq=0,num_freq-1 do begin
                    A(j,slpcol+2*ifreq) = sin((ifreq+1)*fscl*t2)
                    A(j,slpcol+2*ifreq+1) = cos((ifreq+1)*fscl*t2)
                endfor
                j = j + 1
                t2 = t2 + 1
             endif
             k = k + 1
        endfor
        t4 = j-1
        xx = findgen(t4-t3+1)
        slpint = linfit(xx,A[t3:t4,0])
        A[t3:t4,0] = A[t3:t4,0] - (xx*slpint[1] + slpint[0])
        slpcol = slpcol + 2*num_freq
        mean = total(A[t3:t4,file])/(tdim_file - !GLM_SKIP)
        A[t3:t4,file] = A[t3:t4,file] - mean
        t0 = t0 + tdim_file
    endfor
    AA = A[0:tcomp-1,*]
    A = AA
endif

lctrend = !TRUE
lctrend = get_boolean('Remove linear trend',['Yes','No'])
if(lctrend eq !TRUE) then begin
    ncol = ntotal + 2*nfile
    Atmp = fltarr(tcomp,ncol)
    if(ntotal gt 0) then $
        Atmp[*,0:ntotal-1] = A
    A = Atmp
    ntotal = ncol
    j = 0
    k = 0
    t0 = 0
    for file=0,nfile-1 do begin
        if(hdr.array_type eq !STITCH_ARRAY) then $
            tdim_file = stc(idx_data).tdim_file[file] $
    else $
            tdim_file = tdim
        t1 = t0 + tdim_file - 1
        if(t1 gt tdim-1) then $
            t1 = tdim - 1
        slp = -1
        slpslp = 2./(t1-t0)
        for i=t0,t1 do begin
        if(valid_frms(k) gt 0) then begin
                A(j,slpcol) = 1.
                A(j,slpcol+1) = slp
            j = j + 1
            endif
            k = k + 1
            slp = slp + slpslp
        endfor
        slpcol = slpcol + 2
        t0 = t0 + tdim_file
    endfor
    tmp1 = effect_column
    tmp2 = effect_length
    tmp3 = effect_label
    effect_column = intarr(all_eff+1)
    effect_length = intarr(all_eff+1)
    effect_label = strarr(all_eff+1)
    effect_column[0:all_eff-1] = tmp1[0:all_eff-1]
    effect_length[0:all_eff-1] = tmp2[0:all_eff-1]
    effect_label[0:all_eff-1] = tmp3[0:all_eff-1]
    effect_column[all_eff] = effect_column[all_eff-1] + effect_length[all_eff-1]
    effect_length[all_eff] = 2*nslope
    effect_label[all_eff] = 'Trend'
    all_eff = all_eff + 1
endif

cond_number = cond(transpose(A)#A)
print,'Condition number: ',cond_number

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
if(ptr_valid(glm1.contrast_labels)) then $
    ptr_free,glm1.contrast_labels
contrast_labels = effect_label
glm1.contrast_labels = ptr_new(contrast_labels)
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
glm1.num_trials = num_trials
glm1.Ysim = ptr_new(Ysim)
glm1.TR = -1.
stimlen_dummy = fltarr(tot_eff)
effect_group = intarr(tot_eff)
glm1.stimlen = ptr_new(stimlen_dummy)
glm1.effect_group = ptr_new(effect_group)
glm1.lcfunc = ptr_new(lcfunc)

paradigm = valid_frms[0:tdim-1]
fi.paradigm(idx_data) = ptr_new(paradigm)
fi.paradigm_loaded[idx_data] = !TRUE
widget_control,HOURGLASS=0

image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
name = strcompress(string(fi.tails[idx_data],FORMAT='(a,"_glm")'),/REMOVE_ALL)
put_image,roi,image,fi,wd,dsp,name,hdr.xdim,hdr.ydim,1,1,!FLOAT,hdr.dxdy,hdr.dz,scl,!LINEAR_MODEL,idx_data,paradigm,MODEL=fi.nfiles

glm[fi.n] = glm1

return,0
end
