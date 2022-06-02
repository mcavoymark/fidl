;Copyright 7/9/01 Washington University.  All Rights Reserved.
;set_glm1.pro  $Revision: 1.16 $
pro set_glm1,glm1,A,valid_frms,nmain,N,tot_eff,xdim,ydim,zdim,tdim,dxdy,dz,nrun,tcomp,num_trials, $
    TR,stimlen,lcfunc,functype,DELAY=delay,funclen,NF=nF,all_eff, $
    effect_column,effect_label,effect_length,CONTRAST_LABELS=contrast_labels,ATAM1=ATAm1, $
    EFFECT_TR=effect_TR,EFFECT_SHIFT_TR=effect_shift_TR
if ptr_valid(glm1.A) then ptr_free,glm1.A
if ptr_valid(glm1.valid_frms) then ptr_free,glm1.valid_frms
glm1.A = ptr_new(A)
glm1.valid_frms = ptr_new(valid_frms)
glm1.n_interest = nmain
glm1.N = N
glm1.tot_eff = tot_eff
glm1.xdim = xdim
glm1.ydim = ydim
glm1.zdim = zdim
glm1.tdim = tdim
glm1.dxdy = dxdy
glm1.dz = dz
glm1.n_files = nrun
glm1.t_valid = tcomp
glm1.num_trials = num_trials
glm1.TR = TR
if n_elements(delay) gt 0 then glm1.delay = ptr_new(delay)
if n_elements(stimlen) gt 0 then glm1.stimlen = ptr_new(stimlen)

;glm1.lcfunc = ptr_new(lcfunc)
;START160120
if n_elements(lcfunc) gt 0 then glm1.lcfunc = ptr_new(lcfunc)

if n_elements(funclen) gt 0 then glm1.funclen = ptr_new(funclen)
if n_elements(functype) gt 0 then glm1.functype = ptr_new(functype)
if keyword_set(NF) then begin
    glm1.nF = 1
    F_names = 'Omnibus'
    glm1.F_names = ptr_new(F_names)
    glm1.F_names2 = ptr_new(F_names)
    undefine,F_names
endif
glm1.all_eff = all_eff
if ptr_valid(glm1.effect_column) then ptr_free,glm1.effect_column
glm1.effect_column = ptr_new(effect_column)
if ptr_valid(glm1.effect_label) then ptr_free,glm1.effect_label
glm1.effect_label = ptr_new(effect_label)
if ptr_valid(glm1.effect_length) then ptr_free,glm1.effect_length
glm1.effect_length = ptr_new(effect_length)
if keyword_set(CONTRAST_LABELS) then begin
    if ptr_valid(glm1.contrast_labels) then ptr_free,glm1.contrast_labels
    glm1.contrast_labels = ptr_new(contrast_labels)
endif
if keyword_set(ATAm1) then begin
    if(n_elements(ATAm1) gt 0) then begin
        if ptr_valid(glm1.ATAm1) then ptr_free,glm1.ATAm1
        glm1.ATAm1 = ptr_new(ATAm1)
    endif
endif

if keyword_set(EFFECT_TR) then begin
    if ptr_valid(glm1.effect_TR) then ptr_free,glm1.effect_TR
    glm1.effect_TR = ptr_new(effect_TR)
endif
if keyword_set(EFFECT_SHIFT_TR) then begin
    if ptr_valid(glm1.effect_shift_TR) then ptr_free,glm1.effect_shift_TR
    glm1.effect_shift_TR = ptr_new(effect_shift_TR)
endif

end
