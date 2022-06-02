;Copyright 12/31/99 Washington University.  All Rights Reserved.
;define_block_dsgn_matrix.pro  $Revision: 12.127 $
function define_block_dsgn_matrix,dsp,fi,stc,pref,wd,help,glm,DEFAULT=default,EVENT_FILE=event_file,PARAMS=params
spawn,'whoami',whoami
whoamistr = 'Hey '+whoami[0]+'!'
if get_button(['ok','exit'],BASE_TITLE='Block design',TITLE='You should consider analyzing your data as an event related ' $
    +'design with assumed responses.'+string(10B)+string(10B)+'The first column (time) is in frames, with the first frame being 0.' $
    +string(10B)+'The third column (duration) is also in frames.') eq 1 then return,-1
if fi.nfiles eq 0 then begin 
    if load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,FILTER='*.conc',TITLE='Please select concatenated set.') eq !OK then $
        dsp_image = update_image(fi,dsp,wd,stc,pref) $
    else $
        return,-1
endif
if(keyword_set(DEFAULT)) then begin
    lcdefault = !TRUE 
    nfile = params.nrun
    xdim = params.xdim
    ydim = params.ydim
    zdim = params.zdim
    tdim = params.tdim
    dxdy = params.dxdy
    dz = params.dz
    hdr = {name:'',xdim:xdim,ydim:ydim,zdim:zdim,tdim:tdim, $
                type:!FLOAT,dxdy:dxdy,dz:dz,scl:1.,array_type:!ASSOC_ARRAY, $
                mother:0,uval:0,model:0,file_type:!FLOAT_ARRAY,ifh:0,smoothness:0.} 
    mom = 0
endif else begin
    lcdefault = !FALSE
    if(ptr_valid(fi.hdr_ptr[fi.n])) then begin
        hdr = *fi.hdr_ptr[fi.n]
        if((hdr.array_type ne !ASSOC_ARRAY) and (hdr.array_type ne !STITCH_ARRAY)) then begin
             stat = widget_message('Select data file and try again.')
             return,-1
        endif
    endif else begin
        print,'Invalid pointer to data header. Aborting define_block_dsgn_matrix.'
        return,-1
    endelse
    ;Get image header.
    hdr = *fi.hdr_ptr(fi.n)
    mom = fi.n
endelse
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Defines the paradigm based on a file supplied by the user.' $
    + ' This file can be defined using Excel templates at http://imaging.wustl.edu/Ollinger/fidl/fmri_analysis.htm'
if lcdefault eq !FALSE then begin
    get_dialog_pickfile,'*.*fi??',fi.path,'Please select event file.',filename,/DONT_ASK
    if filename eq 'GOBACK' or filename eq 'EXIT' or filename eq '' then return,-1
    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Enter the number of frames to skip at the beginning of run.'
    strs = get_str(1,"Number of frames to skip: ",'4')
    skip = fix(strs[0])
endif else begin
    skip = 4
    filename = event_file
endelse

cnt = readf_ascii_file(filename)
num_trials = cnt.NR
nfh = n_elements(cnt.hdr)
TR = float(cnt.hdr[0])
effects = intarr(cnt.NR)
frames  = reform(fix(cnt.data[0,0:cnt.NR-1]),cnt.NR)
effects = fix(cnt.data[1,0:cnt.NR-1])
stimlen = fix(cnt.data[2,0:cnt.NR-1])
if cnt.NF eq 4 then begin
    control_len =fix(cnt.data[3,0:cnt.NR-1])
    explicit_control = !TRUE
endif else begin
    control_len = stimlen
    explicit_control = !FALSE
endelse
total_len = stimlen + control_len
tot_eff = 2*(max(effects) + 1)
labels = strarr(tot_eff)
labels[0:nfh-2] = cnt.hdr[1:nfh-1]
for i=0,nfh-2 do labels[i+nfh-1] = labels[i] + '_ctl'
lcfunc = intarr(tot_eff)
lcfunc[*] = !BLOCK_DESIGN

if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Applying a high-pass filter will remove low-frequency components ' $
    +'in the data due to motion artifacts.  This reduces the variance in the estimates, but also decreases the signal in ' $
    +'rapidly presented single trials.  This option is recommended if there is significant low-frequency noise in the data.  ' $
    +'This is most likely to be true of block design studies.'+string(10B)+string(10B)+'Coding the control task as a separate ' $
    +'effect yields higher z-scores.  Only  click no if you want to maintain compatibility with old glm files computed before ' $
    +'this option was implemented.'

if lcdefault eq !FALSE then begin
    labs = strarr(2,4)
    labs[0,0] = 'Apply high-pass filter'
    labs[1,0] = 'No high-pass filter'
    labs[0,1] = 'Lump all control periods'
    labs[1,1] = 'Separate control periods for each effect'
    labs[0,2] = 'Estimate linear trend'
    labs[1,2] = 'Linear trend already removed'
    labs[0,3] = 'Omit specified frames from the analysis?'
    labs[1,3] = "Don't omit specified frames from the analysis?"
    dflt = [!FALSE,!TRUE,!TRUE,!FALSE]
    text = string('Lumping the control periods yields a single variable for all control periods. ' $
        +'Not lumping them yields a separate control variable for every modeled effect.' $
        +' Always estimate the linear trend unless you already did it during a preprocessing step.')
    lclist = get_choice_list(labs,TITLE='Select options',DEFAULT=dflt,TEXT=text)
    hipass = lclist[0]
    lump_controls = lclist[1]
    remove_trend = lclist[2]
    lc_omit_frames = lclist[3]
endif else begin
    hipass = !FALSE
    lump_controls = !TRUE
    remove_trend = !TRUE
endelse

if lump_controls eq !TRUE then labels[nfh-1] = 'ctl' 

glm1 = {General_linear_model}

if hdr.array_type eq !STITCH_ARRAY then begin
    nslope = stc[mom].n
    tdim = stc[mom].tdim_all
    nfile = stc[mom].n
    tdim_sum = *stc[mom].tdim_sum
    tdim_file = *stc[mom].tdim_file
    t_to_file = *stc[mom].t_to_file
endif else begin
    nslope = 1
    nfile = 1
    tdim = hdr.tdim
    tdim_file = tdim
    t_to_file = intarr(tdim)
endelse
;print,'tdim=',tdim,' frames[n_elements(frames)-1]+1=',frames[n_elements(frames)-1]+1,' n_elements(frames)=',n_elements(frames)
separate_control = !TRUE
hrf = fltarr(tdim)
G = fltarr(tdim,tot_eff)
tnext = tdim_file[0]
widget_control,/HOURGLASS
if separate_control eq !TRUE then begin
    t0 = 0
    i = 0
    for file=0,nfile-1 do begin
        t1 = (frames[i]-1) < (tdim-1)
        if t1 gt t0+skip then G[t0+skip:t1,tot_eff/2+effects[i]] = 1
        t0 = t0 + tdim_file[file]
        if file ne nfile -1 then begin

            ;repeat begin
            ;    i = i + 1
            ;endrep until frames[i] ge t0

            repeat begin
                i = i + 1
                if i eq cnt.NR then begin
                    spawn,'whoami',scrap
                    stat=dialog_message('Hey '+scrap+'!'+string(10B)+'Your event file is shorter than the amount of data. Abort!', $
                        /ERROR)
                    return,-1
                endif
            endrep until frames[i] ge t0

        endif
    endfor
endif







;for i=0,tdim-1 do print,i,G[i,0],G[i,1],G[i,2]
file = 0
for trial=0,num_trials-1 do begin
;   Define signal as in Boynton et al.
    hrf[*] = -1.
    for i=0,total_len[trial]-1 do begin
        t = TR*(i + .5)
        hrf[i]  = hemo_resp(t,TR*stimlen[trial])
    endfor
    if(separate_control eq !TRUE) then begin
    ;if lump_controls eq !FALSE then begin
	hrf = hrf/max(hrf[0:stimlen[trial]])
        hrf = hrf*(hrf ge .1*max(hrf))
	fixation = float(hrf lt .1*max(hrf))
    endif else begin
        hrf = 2.*hrf/max(hrf[0:total_len[trial]]-1) - 1
    endelse
    ;print,'trial=',trial
    ;print,'hrf=',hrf
    ;print,''
    max = max(hrf)
    t0 = frames[trial]
    if t0 ge tnext then begin
        file = file + 1
        if hdr.array_type eq !STITCH_ARRAY then tnext = tnext + tdim_file[file]
    endif



    ;if trial lt num_trials-1 then t1 = frames[trial+1] else t1 = tnext - 1
    ;;print,'before t0=',t0,' t1=',t1
    ;if t0 lt tdim-1 then begin
    ;    t1 = t1 < (tnext-1)
    ;    if trial lt num_trials-1 then t1 = t1 < frames[trial+1]
    ;    eff = effects[trial]
    ;    G[t0:t1,eff] = G[t0:t1,eff] + hrf[0:t1-t0]
    ;    if(separate_control eq !TRUE) then $ ;if lump_controls eq !FALSE then $
    ;	    G[t0:t1,eff+tot_eff/2] = G[t0:t1,eff+tot_eff/2] + fixation[0:t1-t0]
    ;    ;print,'trial=',trial
    ;    ;for i=t0,t1 do print,i,G[i,0],G[i,1],G[i,2]
    ;    print,''
    ;    ;Ysim[t0,eff] = Ysim[t0,eff] + 1.
    ;endif else begin
    ;    trial = num_trials
    ;endelse

    if trial lt num_trials-1 then t1 = frames[trial+1]-1 else t1 = tnext - 1
    if t0 lt tdim-1 then begin
        t1 = t1 < (tnext-1)
        eff = effects[trial]
        G[t0:t1,eff] = G[t0:t1,eff] + hrf[0:t1-t0]
        if separate_control eq !TRUE then G[t0:t1,eff+tot_eff/2] = G[t0:t1,eff+tot_eff/2] + fixation[0:t1-t0]
    endif else begin
        trial = num_trials
    endelse






endfor
;for i=0,tdim-1 do print,i,G[i,0],G[i,1],G[i,2]

;print,'G='
;for i=0,tdim-1 do print,i,G[i,0],G[i,1],G[i,2],G[i,3],G[i,4],G[i,5],G[i,6],G[i,7],G[i,8],G[i,9],G[i,10],G[i,11]

if(tot_eff gt 2) then begin
    if(lump_controls eq !TRUE) then begin
        GG = fltarr(tdim,tot_eff/2 + 1)
        GG[*,0:tot_eff/2-1] = G[*,0:tot_eff/2-1]
        GG[*,tot_eff/2] = total(G[*,tot_eff/2:tot_eff-1],2)
        G = GG
        tot_eff = tot_eff/2 + 1
    endif else begin
        ; Fix up mistakes.
        for eff=tot_eff/2,tot_eff-1 do begin
            xm1 = 0
            for t=0,tdim-1 do begin
                if((xm1 gt 0) and (total(G[t,tot_eff/2:tot_eff-1]) gt 0)) then begin
                    G[t,tot_eff/2:tot_eff-1] = 0
                    G[t,eff] = 1
                endif
                xm1 = G[t,eff]
            endfor
        endfor
    endelse
endif
effect_column = findgen(tot_eff)
effect_length = intarr(tot_eff)
effect_length[*] = 1
effect_label = labels[0:tot_eff-1]
all_eff = tot_eff
if(remove_trend eq !TRUE) then $
    ntotal = tot_eff + 2*nslope $
else $
    ntotal = tot_eff + nslope
slpcol = tot_eff

valid_frms = make_array(tdim,/FLOAT,VALUE=1.)
for t=0,tdim-1 do begin
    ifile = t_to_file[t]
    if ifile gt 0 then $
        tfile = t - tdim_sum[ifile] $
    else $
        tfile = t
    if tfile lt skip then valid_frms[t] = 0
endfor
if lc_omit_frames eq !TRUE then begin
    rtn = get_frames(tdim,nfile,tdim_file,len_hrf,valid_frms)
    omit_reference = rtn.omit_reference
    ;if omit_reference eq !TRIAL then valid_trial_frms = rtn.valid_trial_frms
    if omit_reference eq !TRIAL then begin
        stat = dialog_message('This program is currently not set up to handle this option.'+string(10B)+'To instantiate this ' $
            +'option just ask McAvoy and he will do it.',/INFORMATION,/CANCEL)
        if stat eq 'Cancel' then return,-1
    endif
endif else begin
    omit_reference = 0
endelse

tcomp = total(valid_frms ne 0)
A = fltarr(tcomp,ntotal)
nuisance_col = slpcol
trend_col = slpcol

;dtrend_col = 1
if remove_trend eq !TRUE then $
    dtrend_col = 1 $
else $
    dtrend_col = 0

;print,'G='
;for i=0,tdim-1 do print,i,G[i,0],G[i,1],G[i,2],G[i,3],G[i,4],G[i,5],G[i,6],G[i,7],G[i,8],G[i,9],G[i,10],G[i,11]

base_col = slpcol + dtrend_col*nfile
dbase_col = 1
j0 = 0
t0 = 0
for file=0,nfile-1 do begin
    t1 = t0 + tdim_file[file] - 1
    if(t1 gt tdim-1) then $
        t1 = tdim - 1
    slp = 2.*findgen(t1-t0+1)/(t1-t0) - 1
    idx = where(valid_frms[t0:t1])
    nidx = n_elements(idx)
    slp = slp[idx]
    slp = slp - total(slp)/float(nidx)
    j1 = j0 + nidx

    ;print,'j0=',j0,' j1-1=',j1-1
    ;print,'idx+t0=',idx+t0

    A[j0:j1-1,0:tot_eff-1] = G[idx+t0,*]
    A[j0:j1-1,base_col] = 1.
    if remove_trend eq !TRUE then A[j0:j1-1,trend_col] = slp
    j0 = j1
    t0 = t0 + tdim_file[file]
    trend_col = trend_col + dtrend_col
    base_col = base_col + dbase_col
endfor
slpcol = slpcol + (dtrend_col + dbase_col)*nfile
lenslp = slpcol - nuisance_col
if(lenslp gt 0) then begin
    tmp1 = effect_column
    tmp2 = effect_length
    tmp3 = effect_label
    effect_column = intarr(all_eff+dbase_col+dtrend_col)
    effect_length = intarr(all_eff+dbase_col+dtrend_col)
    effect_label = strarr(all_eff+dbase_col+dtrend_col)
    effect_column[0:all_eff-1] = tmp1[0:all_eff-1]
    effect_length[0:all_eff-1] = tmp2[0:all_eff-1]
    effect_label[0:all_eff-1] = tmp3[0:all_eff-1]

    ;effect_column[all_eff] = effect_column[all_eff-1] + effect_length[all_eff-1]
    ;effect_length[all_eff] = nfile
    ;effect_label[all_eff] = 'Trend'
    ;all_eff = all_eff + 1

    if remove_trend eq !TRUE then begin
        effect_column[all_eff] = effect_column[all_eff-1] + effect_length[all_eff-1]
        effect_length[all_eff] = nfile
        effect_label[all_eff] = 'Trend'
        all_eff = all_eff + 1
    endif

    effect_column[all_eff] = effect_column[all_eff-1] + effect_length[all_eff-1]
    effect_length[all_eff] = nfile
    effect_label[all_eff] = 'Baseline'
    all_eff = all_eff + 1
endif
if hipass eq !TRUE then begin
    tdim_min = 100000L
    for file=0,nfile-1 do begin
        if tdim_file[file] lt tdim_min then tdim_min = tdim_file[file]
        fmin = 1./(tdim_min*TR)
    endfor
    mean_period = 0.
    max_period = 0
    for trial=0,num_trials-1 do begin
        mean_period = mean_period + total_len[trial]*TR
        if(total_len[trial] gt max_period) then $
            max_period = TR*total_len[trial]
    endfor
    paradigm_freq = 1./max_period
    num_freq = fix(paradigm_freq/fmin) - 1
    labels = strcompress(string(num_freq,FORMAT='("Number of low frequencies to remove [1 to ",i2,"]")'))
    goback50:
    strs = get_str(1,labels,(num_freq < 5),TITLE='Select cutoff frequency')
    num_freqp = long(strs[0])
    if num_freqp gt num_freq then begin
        scrap= 'Warning: Specified cutoff frequency will remove signal energy.'
        print,scrap
        stat = get_button(['continue','go back','exit'],BASE_TITLE=whoamistr,TITLE=scrap)
        if stat eq 2 then return,-1 else if stat eq 1 then goto,goback50
    endif
    num_freq = num_freqp
    tmp1 = effect_column
    tmp2 = effect_length
    tmp3 = effect_label
    effect_column = intarr(all_eff+1)
    effect_length = intarr(all_eff+1)
    effect_label = strarr(all_eff+1)
    effect_column[0:all_eff-1] = tmp1
    effect_length[0:all_eff-1] = tmp2
    effect_label[0:all_eff-1] = tmp3
    effect_column[all_eff] = effect_column[all_eff-1] + effect_length[all_eff-1]
    effect_length[all_eff] = num_freq*2*nfile
    effect_label[all_eff] = 'Hi-pass'
    eff_filter = all_eff
    all_eff = all_eff + 1
    n1 = ntotal
    ntotal = ntotal + 2*num_freq*nfile
    Atmp = A


    ;ATA = transpose(A)#A
    ;ATAm1 = float(pinv(ATA,0.))
    ;START130205
    Ad = double(A)
    ATAd = transpose(Ad)#Ad
    ATA = float(ATAd)
    size_ATA = size(ATA)
    cond_number = double(0.)
    n_depend = 0L
    stat=0L
    stat=call_external(!SHARE_LIB,'_gsl_svd',ATA,size_ATA[1],size_ATA[2],cond_number,n_depend,VALUE=[0,1,1,0,0])
    ATAm1 = ATA


    var = fltarr(tot_eff)
    for i=0,tot_eff-1 do var[i] = ATAm1[i,i]
    minvar_nofltr = min(var,MAX=maxvar_nofltr)
    meanvar_nofltr = total(var)/tot_eff
    A = fltarr(tcomp,ntotal)
    A[*,0:n1-1] = Atmp
    j = 0
    k = 0
    t0 = 0
    for file=0,nfile-1 do begin
        t1 = t0 + tdim_file[file] - 1
        if(t1 gt tdim-1) then $
            t1 = tdim - 1
        fmin = 1./(tdim_file[file]*TR)
        t2 = 0
        fscl = 2*!PI/(tdim_file[file]-skip)
        for i=t0,t1 do begin
            if(valid_frms[k] gt 0) then begin
                for ifreq=0,num_freq-1 do begin
                   A(j,slpcol+2*ifreq) = sin((ifreq+1)*fscl*t2)
                   A(j,slpcol+2*ifreq+1) = cos((ifreq+1)*fscl*t2)
                endfor
                j = j + 1
                t2 = t2 + 1
            endif
            k = k + 1
        endfor
        slpcol = slpcol + 2*num_freq
        t0 = t0 + tdim_file[file]
    endfor
endif


;ATA = transpose(A)#A
;print,'Condition number: ',cond(ATA,/DOUBLE)
;ATAm1 = float(pinv(ATA,0.))
;A[*,0:tot_eff-1] = A[*,0:tot_eff-1] - 1./tot_eff
;START130205
Ad = double(A)
ATAd = transpose(Ad)#Ad
ATA = float(ATAd)
size_ATA = size(ATA)
cond_number = double(0.)
n_depend = 0L
stat=0L
stat=call_external(!SHARE_LIB,'_gsl_svd',ATA,size_ATA[1],size_ATA[2],cond_number,n_depend,VALUE=[0,1,1,0,0])
ATAm1 = ATA
;print,'ATAm1'
;print,ATAm1
if stat eq 0L then begin
    print,'Error in _gsl_svd_golubreinsch'
    return,!ERROR
endif
print,'_gsl_svd_golubreinsch cond_number=',cond_number,' n_depend=',n_depend
A[*,0:tot_eff-1] = A[*,0:tot_eff-1] - 1./tot_eff





if hipass eq !TRUE then begin
    var = fltarr(tot_eff)
    for i=0,tot_eff-1 do var[i] = ATAm1[i,i]
    minvar_fltr = min(var,MAX=maxvar_fltr)
    meanvar_fltr = total(var)/tot_eff
;;;    help,minvar_fltr,maxvar_fltr,meanvar_fltr
    releff_min_reduction = sqrt(minvar_nofltr/minvar_fltr)
    releff_max_reduction = sqrt(maxvar_nofltr/maxvar_fltr)
    releff_mean_reduction = sqrt(meanvar_nofltr/meanvar_fltr)
;;;    help,releff_min_reduction,releff_max_reduction,releff_mean_reduction
    print,''
    print,'The high-pass filter increases the within-subject variance of each estimate.'  
    print,'Listed below are metrics that describe the reduction in z-scores for within-'
    print,'subject analyses.  These values may not hold for t-tests and ANOVAs used as'
    print,'random effects models, particularly if many frames are collected (>1000).  The '
    print,'high-pass filter makes the box correction less severe, so it is in general'
    print,'desirable for analyses using ANOVAs.'
    print,releff_min_reduction,FORMAT='("     Minimum reduction of z-scores by high-pass filter: ",f6.3)'
    print,releff_max_reduction,FORMAT='("     Maximum reduction of z-scores by high-pass filter: ",f6.3)'
    print,releff_mean_reduction,FORMAT='("     Mean reduction of z-scores by high-pass filter: ",f6.3)'
endif

;if(ptr_valid(glm1.A) eq !TRUE) then $
;    ptr_free,glm1.A
;if(ptr_valid(glm1.valid_frms) eq !TRUE) then $
;    ptr_free,glm1.valid_frms
;if(ptr_valid(glm1.Ysim) eq !TRUE) then $
;    ptr_free,glm1.Ysim
;glm1.all_eff = all_eff
;if(ptr_valid(glm1.effect_column)) then $
;    ptr_free,glm1.effect_column
;glm1.effect_column = ptr_new(effect_column)
;if(ptr_valid(glm1.effect_label)) then $
;    ptr_free,glm1.effect_label
;glm1.effect_label = ptr_new(effect_label)
;if(ptr_valid(glm1.contrast_labels)) then $
;    ptr_free,glm1.contrast_labels
;contrast_labels = effect_label
;glm1.contrast_labels = ptr_new(contrast_labels)
;if(ptr_valid(glm1.effect_length)) then $
;    ptr_free,glm1.effect_length
;glm1.effect_length = ptr_new(effect_length)
;glm1.A = ptr_new(A)
;glm1.valid_frms = ptr_new(valid_frms)
;glm1.n_interest = tot_eff
;glm1.N = ntotal
;glm1.tot_eff = tot_eff
;glm1.xdim = hdr.xdim
;glm1.ydim = hdr.ydim
;glm1.zdim = hdr.zdim
;glm1.tdim = tdim
;glm1.n_files = nfile
;glm1.t_valid = tcomp
;glm1.period = 1
;glm1.num_trials = num_trials
;glm1.Ysim = ptr_new(Ysim)
;glm1.TR = TR

;paradigm = valid_frms[0:tdim-1]
;fi.paradigm(mom) = ptr_new(paradigm)
;fi.paradigm_loaded[mom] = !TRUE
;put_image,image,fi,wd,dsp,fi.tails[fi.n],hdr.xdim,hdr.ydim,1,1,!FLOAT,hdr.dxdy,hdr.dz,scl,!LINEAR_MODEL,mom,paradigm, $
;    MODEL=fi.nfiles
;START150731
put_image,image,fi,wd,dsp,fi.tails[fi.n],hdr.xdim,hdr.ydim,1,1,!FLOAT,hdr.dxdy,hdr.dz,scl,!LINEAR_MODEL,mom,MODEL=fi.nfiles

stimlen = fltarr(tot_eff)
delay = fltarr(tot_eff)
set_glm1,glm1,A,valid_frms,tot_eff,ntotal,tot_eff,hdr.xdim,hdr.ydim,hdr.zdim,tdim,hdr.dxdy,hdr.dz,nfile,tcomp,num_trials,TR, $
    stimlen,lcfunc,functype,funclen,all_eff,effect_column,effect_label,effect_length,CONTRAST_LABELS=effect_label,DELAY=delay
glm[fi.n] = glm1
glm[fi.n].ATAm1 = ptr_new(ATAm1)
if tot_eff gt 0 then begin
    widget_control,/HOURGLASS
    define_contrast,glm,fi.n,fi,help,dsp,wd,stc,pref,hrf_model,/SETUPGLM
endif
print,'DONE'
return,0
end
