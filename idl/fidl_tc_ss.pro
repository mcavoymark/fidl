;Copyright 12/13/02 Washington University.  All Rights Reserved.
;fidl_tc_ss.pro  $Revision: 1.84 $
pro fidl_tc_ss,fi,dsp,wd,glm,help,stc,pref,FANO=fano

;scrap=get_button(['Continue','Exit'],BASE_TITLE='fidl_tc_ss',TITLE='Trialwise timecourses are extracted from the time series.' $
;    +string(10B)+string(10B)+'This program accepts either imgs/concs or glms.'+string(10B) $
;    +'If glms, timecourses are extracted from the residual time series.'+string(10B)+'You must provide an event ' $
;    +'file which need not be the one used in the glm.')
;START161219
;scrap=get_button(['Continue','Exit'],BASE_TITLE='fidl_tc_ss',TITLE='Trialwise timecourses are extracted from the time series.' $
;    +string(10B)+'This program can also provide means/standard deviations for different trial types/epochs.' $ 
;    +string(10B)+string(10B)+'This program accepts either imgs/concs or glms.'+string(10B) $
;    +'If glms, timecourses are extracted from the residual time series.'+string(10B)+'You must provide an event ' $
;    +'file which need not be the one used in the glm.')
;START200107
scrap=get_button(['Continue','Exit'],BASE_TITLE='fidl_tc_ss',TITLE='Trialwise timecourses or resting state values are extracted from the time series.' $
    +string(10B)+'This program can also provide means/standard deviations for different trial types/epochs.' $ 
    +string(10B)+string(10B)+'This program accepts either 4dfps, niftis or glms.'+string(10B) $
    +'We recommended using a list with concs, event files and region files.'+string(10B) $
    +'If glms, timecourses are extracted from the residual time series.'+string(10B)+'You must provide an event ' $
    +'file which need not be the one used in the glm.')

if scrap eq 1 then return
if not keyword_set(FANO) then fano=0
directory=getenv('PWD')
region_str = ''
roi_str = ''
anova_output_str = ''
wfiles = ''
regtc_type_str = ''
output = 0
regtypestr = '' 
gohere=0
analtype=0
outstr=''
tcstr=''
catstr=''
cleanupstr=''
regval_str=''
lcwmparc=0

goback1:
widget_control,/HOURGLASS

gg=get_glms(pref,GET_THIS='concs/imgs/glms',FILTER='*.glm')

if gg.msg eq 'GO_BACK' or gg.msg eq 'EXIT' then return
load = gg.load
nlists = gg.nlists
total_nsubjects = gg.total_nsubjects
t4select = gg.t4select
concselect = gg.concselect
evselect = gg.evselect
glmfiles = gg.glmfiles
glm_space = gg.glm_space
bold_space = gg.bold_space
ifh1 = gg.ifh
morethanone = gg.morethanone
goback1a:
if morethanone eq 0 then begin
    nsubject = total_nsubjects
    nglm_per_subject = make_array(nsubject,/INTEGER,VALUE=1)
endif else begin
    if glmfiles[0] ne '' then begin
        rtn_get_morethanone=get_morethanone(glmfiles,total_nsubjects,1,'',t4select,concselect,evselect,glm_space,bold_space,ifh1)
        if rtn_get_morethanone.msg eq 'GO_BACK' then goto,goback1 else if rtn_get_morethanone.msg eq 'ERROR' then return
        glmfiles = rtn_get_morethanone.files_per_group
        concselect = rtn_get_morethanone.concselect
    endif else begin
        rtn_get_morethanone=get_morethanone(concselect,total_nsubjects,1,'',t4select,'',evselect,glm_space,bold_space,ifh1)
        if rtn_get_morethanone.msg eq 'GO_BACK' then goto,goback1 else if rtn_get_morethanone.msg eq 'ERROR' then return
        concselect = rtn_get_morethanone.files_per_group
    endelse
    nglm_per_subject = rtn_get_morethanone.nglm_per_subject
    total_nsubjects = rtn_get_morethanone.total_nsubjects
    nsubject = rtn_get_morethanone.nsubject
    t4select = rtn_get_morethanone.t4select
    evselect = rtn_get_morethanone.evselect
    glm_space = rtn_get_morethanone.glm_space
    bold_space = rtn_get_morethanone.bold_space
    ifh1 = rtn_get_morethanone.ifh
    subject_names = rtn_get_morethanone.subject_names
    if glmfiles[0] ne '' then begin
        gg.glmfiles = glmfiles
        gg.total_nsubjects = total_nsubjects
        gg.t4select = t4select
        gg.concselect = concselect
        gg.evselect = evselect
        gg.glm_space = glm_space
        gg.bold_space = bold_space
        gg.ifh = ifh1
    endif
endelse
if glmfiles[0] ne '' then begin
    compute_residuals_struct=compute_residuals(fi,dsp,wd,glm,help,stc,pref,GET_GLMS_STRUCT=gg,/FIDL_TC_SS)
    if compute_residuals_struct.msg eq 'GO_BACK' then goto,goback1
    concselect = compute_residuals_struct.concselect
    analysis_type = compute_residuals_struct.analysis_type
    region_files = compute_residuals_struct.region_files
    roi_str = compute_residuals_struct.roi_str
    t4select=compute_residuals_struct.t4select
endif
if evselect[0] eq '' or gohere eq 1 then begin
    gohere = 1
    goback0:
    widget_control,/HOURGLASS
    gf = get_files(FILTER='*fi??',TITLE='Please enter event file filter.',EXTRALABEL='Enter NONE if none.')
    if gf.msg eq 'GO_BACK' then goto,goback1 else if gf.msg eq 'EXIT' then return
    evselect = gf.files
    if evselect[0] ne 'NONE' then begin
        if n_elements(concselect) eq 1 and n_elements(evselect) eq 1 and gf.msg eq 'DONTCHECK' then begin
            ;do nothing
        endif else begin
            evselect = match_files(concselect,evselect,TITLE='Please select event file for ',/GO_BACK)
            if evselect[0] eq 'GO_BACK' then goto,goback0
        endelse
    endif else $
        evselect = make_array(total_nsubjects,/STRING,VALUE='NONE')
endif else begin
    print,'fidl_tc_ss evselect'
    print,evselect

    ;scrap = expand_filenames(fi,evselect,glmfiles)
    ;START170712
    scrap = expand_filenames(fi,evselect,glmfiles,'Event files')

    if scrap[0] eq 'GO_BACK' then goto,goback1a else if scrap[0] eq 'EXIT' then return
    evselect = scrap
endelse
t4_str=make_array(nsubject,/STRING,VALUE='')
space = intarr(total_nsubjects)
stc2 = replicate({Stitch},total_nsubjects)
sng = replicate({Eventfile},total_nsubjects)
widget_control,/HOURGLASS
for m=0,total_nsubjects-1 do begin
    print,'Loading ',concselect[m]
    rload_conc=load_conc(fi,stc,dsp,help,wd,concselect[m],/DONT_PUT_IMAGE)
    if rload_conc.msg eq 'GOBACK' then goto,goback1a else if rload_conc.msg eq 'EXIT' then return
    stc2[m]=rload_conc.stc1 
    space[m]=get_space(rload_conc.ifh.matrix_size_1,rload_conc.ifh.matrix_size_2,rload_conc.ifh.matrix_size_3)
    sng[m] = define_single_trial(fi,pref,help,EVENT_FILE=evselect[m])
    if sng[m].name eq 'ERROR' then return
endfor
index_lc_t4=where(space eq !SPACE_DATA,count_lc_t4)
if count_lc_t4 gt 0 and count_lc_t4 ne total_nsubjects then begin
    scrap=get_button(['yes','no','not sure','go back'],BASE_TITLE='Images are not all of the same dimension', $
        TITLE='Do you know what you are doing?')
    if scrap eq 1 then $
        return $
    else if scrap eq 2 then begin
        scrap=get_button(['ok','go back','exit'],BASE_TITLE='hehehe',TITLE='We will just have see what happens.')
        if scrap eq 1 then goto,goback1 else if scrap eq 2 then return
    endif else if scrap eq 3 then goto,goback1
endif
ne_max = max(sng[*].num_levels,ne_max_index)
factor_labels = *sng[ne_max_index].factor_labels
for i=0,total_nsubjects-1 do begin
    if i ne ne_max_index then begin
        scrap=*sng[i].factor_labels
        for j=0,sng[i].num_levels-1 do if total(strcmp(factor_labels,scrap[j])) eq 0 then factor_labels = [factor_labels,scrap[j]]
    endif
endfor
ne_max = n_elements(factor_labels)
ntc = ne_max
factorial_ne_max = factorial(ne_max)
for i=2,ne_max do ntc = ntc + factorial_ne_max/(factorial(i)*factorial(ne_max-i))
if ntc gt 10 then ntc = 10
if ntc lt ne_max then ntc = ne_max
treatment_str = strarr(ntc)
for i=0,ntc-1 do treatment_str[i] = 'timecourse ' + strtrim(i+1,2)
goback8:
widget_control,/HOURGLASS
if fano eq 0 then $
    dummy = 'Multiple conditions are summed. Blank columns are ignored.' $
else $
    dummy = 'Multiple selections are treated as a single event type.' 
rtn_ic = identify_conditions_new(ne_max,factor_labels,ntc,treatment_str,0,0,dummy,/SKIP_CHECKS,/SET_DIAGONAL, $
    TOP_TITLE='Please select an event type for each timecourse.')
if rtn_ic.special eq 'GO_BACK' then goto,goback1 
ntc = n_elements(rtn_ic.sumrows)
sumrows = rtn_ic.sumrows
index_conditions = rtn_ic.index_conditions
nc = total(sumrows)
scraplabels = replicate(ptr_new(),total_nsubjects)
for m=0,total_nsubjects-1 do scraplabels[m] = sng[m].factor_labels 
scrap = cft_and_scft(total_nsubjects,ntc,ne_max,sumrows,index_conditions,factor_labels,get_labels_struct,0, $
    evselect,treatment_str,0,dummy,scraplabels,/REPEATS)
if scrap.sum_contrast_for_treatment[0,0,0] eq -2 then $
    return $
else if scrap.sum_contrast_for_treatment[0,0,0] eq -1 then $
    goto,goback8
sum_contrast_for_treatment = scrap.sum_contrast_for_treatment
counts=scrap.counts

;print,'sum_contrast_for_treatment'
;print,sum_contrast_for_treatment
;print,'size(sum_contrast_for_treatment)=',size(sum_contrast_for_treatment)
;print,'counts'
;print,counts
;print,'size(counts)=',size(counts)

if fano eq 1 then begin
    index_tc_str = ' -tc_cond'
    for i=0,ntc-1 do index_tc_str = index_tc_str+' '+strjoin(trim(rtn_ic.index_conditions[i,0:rtn_ic.sumrows[i]-1]),',',/SINGLE) 
    index_tc = -1
    for i=0,ntc-1 do begin
        scrap = rtn_ic.index_conditions[i,0:rtn_ic.sumrows[i]-1]
        if size(scrap,/N_DIMENSIONS) gt 0 then scrap = reform(scrap)
        index_tc = [index_tc,scrap]
    endfor
    index_tc = index_tc[1:*]
endif
names_tc_big = strarr(ntc)
scrap = intarr(total_nsubjects)
ncond_ts = intarr(ntc)
for i=0,ntc-1 do begin
    goose = ''
    for j=0,total_nsubjects-1 do begin
        scrapstr = *sng[j].factor_labels
        ncond_ts[i] = ncond_ts[i] + counts[j,i] 
        goose = [goose,reform(scrapstr[sum_contrast_for_treatment[j,i,0:counts[j,i]-1]-1])]
    endfor
    goose = goose[1:*]
    neg = n_elements(goose)
    keepers = -1
    for j=0,neg-1 do begin
        for k=j+1,neg-1 do if goose[j] eq goose[k] then goto,continue
        keepers = [keepers,j]
        continue:
    endfor
    names_tc_big[i] = strjoin(goose[keepers[1:*]],'+',/SINGLE)
endfor
names_tc_str = ' -tc_names ' + strjoin(names_tc_big,' ',/SINGLE)
if fano eq 0 then begin
    rtn = get_htr_qtr(ne_max,total_nsubjects,sng)
    hTR = rtn.hTR
    qTR = rtn.qTR
    dummy = strarr(3,ntc)
    dummy[0,*] = 'TR'
    dummy[1,*] = 'half TR'
    dummy[2,*] = 'quarter TR'
    dummy_lengths = make_array(ntc,/INT,VALUE=1)
    for i=0,ntc-1 do begin
        scrap = rtn_ic.index_conditions[i,0:rtn_ic.sumrows[i]-1]
        count_qTR = 0
        if total(hTR[scrap]) eq ncond_ts[i] then begin
            dummy_lengths[i] = 2
            if total(qTR[scrap]) eq ncond_ts[i] then dummy_lengths[i] = 3
        endif
    endfor
    scrapindex = where(dummy_lengths gt 1,nearTRcount)
    if nearTRcount gt 0 then begin
        goback3:
        widget_control,/HOURGLASS
        rtn = get_choice_list_many(names_tc_big,dummy,TOP_TITLE='Please select.', $
            /COLUMN_LAYOUT,TITLE='Place timecourses at the nearest',LENGTHS=dummy_lengths,/GO_BACK)
        if rtn.special eq 'GO_BACK' then begin
            if lc_t4 eq !TRUE then goto,goback4a else goto,goback4
        endif
        nearTRlist = rtn.list
    endif
endif
if count_lc_t4 gt 0 and t4select[0] eq '' then begin
    goback10:
    analtype = get_button(['Atlas space analysis','Data space analysis','Go back'], $
        TITLE='Do you wish to do an atlas space or a data space analysis?')
    if analtype eq 2 then goto,goback8
    if analtype eq 0 then begin
        if t4select[0] eq '' then begin
            goback4a:
            gf = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_711-2?_t4')
            if gf.msg eq 'GO_BACK' then goto,goback10 else if gf.msg eq 'EXIT' then return
            if n_elements(t4files) eq 1 and total_nsubjects gt 1 then $
                t4select = make_array(total_nsubjects,/STRING,VALUE=t4files[0]) $
            else if n_elements(t4files) eq 1 and total_nsubjects eq 1 and gf.msg eq 'DONTCHECK' then $
                t4select = t4files $
            else $
                t4select[index_lc_t4] = match_files(concselect[index_lc_t4],t4files,TITLE='Please select t4 file for ')
        endif else $
            t4select = get_str(total_nsubjects,concselect,t4select,TITLE='Please check t4s.',/ONE_COLUMN,/BELOW)
        index=where(t4select ne '',count)
        if count ne 0 then t4_str[index] = ' -xform_file ' + t4select[index]
        rtn=select_space()
        space[index_lc_t4]=rtn.space
    endif
endif
goback4:
if glmfiles[0] eq '' then analysis_type=get_button(['voxel by voxel','specific regions','go back'],TITLE='Please select analysis type.')
if analysis_type eq 2 then begin
    if nearTRcount gt 0 then goto,goback3 else goto,goback8
endif 
goback40:
mask_str = make_array(total_nsubjects,/STRING,VALUE='');
pooledsdstr=''
if analysis_type eq 1 then begin
    output=1
    if glmfiles[0] eq '' then begin 
        if total(space-space[0]) ne 0 then begin
            stat=dialog_message('Not all images are in the same space. Abort!',/ERROR)
            return
        endif


        ;rtn = get_regions(fi,wd,dsp,help)
        ;if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then goto,goback4
        ;region_names = rtn.region_names
        ;region_space = rtn.space
        ;region_files = rtn.region_file
        ;nregion_files = rtn.nregfiles
        ;goback98:
        ;rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.')
        ;roi_str = ' -regions_of_interest ' + strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)
        ;if space[0] ne region_space then begin
        ;    stat=dialog_message('Space incompatibility! Abort! Abort! Abort!',/ERROR)
        ;    return
        ;endif
        ;START191025
        rtn = get_regions(fi,wd,dsp,help,'',gg.niiselect[0])
        if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then goto,goback4
        region_names = rtn.region_names
        region_space = rtn.space
        region_files = rtn.region_file
        nregion_files = rtn.nregfiles

        ;START200107
        regval = rtn.regval

        goback98:

        ;rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.')
        ;START200311
        rtn = select_files([region_names,'all freesurfer regions'],TITLE='Please select regions you wish to analyze.')
        lcwmparc=0
        if rtn.count eq 1 and rtn.index[0] eq n_elements(region_names) then lcwmparc=1

        ;roi_str = ' -regions_of_interest ' + strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)
        ;regval_str = ' -regval ' + strjoin(strtrim(regval[rtn.index],2),' ',/SINGLE)
        ;START200311
        if lcwmparc eq 0 then begin
            roi_str = ' -regions_of_interest ' + strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)
            regval_str = ' -regval ' + strjoin(strtrim(regval[rtn.index],2),' ',/SINGLE)
        endif else begin
            regval_str = ' -lut '+!FreeSurferColorLUT
        endelse


        if space[0] ne region_space then begin
            stat=dialog_message('Space incompatibility! Abort! Abort! Abort!',/ERROR)
            return
        endif
        if n_elements(gg.niiselect) eq nsubject then begin
            region_files = gg.niiselect    
        endif

    endif
    goback99:
    regtype = get_button(['yes','no','go back'],TITLE='In addition to regional timecourses,'+string(10B)+' do you want ' $
        +'voxel timecourses?')
    if regtype eq 2 then begin
        if glmfiles[0] ne '' then goto,goback8 else goto,goback98
    endif
    if regtype eq 0 then regtypestr = ' -voxels'
    goback100:
    regavg_type = get_button(['arithmetic mean','weighted mean'],TITLE='Regional averages should be the')
    if regavg_type eq 1 then begin
        wfiles = get_list_of_files('*.img',directory,'Please select weight files.',string(indgen(1000)+1))
    endif

    ;START191014
    ;rtn = get_bool_list(['averaged across trials','for all trials','GO BACK'],TITLE='I want regional timecourses')
    ;regtc_type = rtn.list 
    ;if regtc_type[2] eq 1 then goto,goback100
    ;if regtc_type[1] eq 1 then begin
    ;    regtc_type_str = ' -trials'
    ;    if regtc_type[0] eq 0 then begin
    ;        regtc_type_str = regtc_type_str + ' ONLY'
    ;        output = 0
    ;    endif
    ;endif

endif else if analysis_type eq 0 then begin 

    ;START151120
    ;if fano eq 0 then begin
    ;    rtn = select_files(['mean','sem','sd','var','pooledsd','trials'],TITLE='Please select',BASE_TITLE='Output options', $
    ;        /GO_BACK,/EXIT)
    ;    if rtn.files[0] eq 'GO_BACK' then goto,goback4 else if rtn.files[0] eq 'EXIT' then return
    ;    idx = where(rtn.files ne 'pooledsd',cnt)
    ;    if cnt ne 0 then outstr = ' -' + strjoin(rtn.files[idx],' -',/SINGLE)
    ;    idx = where(rtn.files eq 'pooledsd',cnt)
    ;    if cnt ne 0 then pooledsdstr = ' -pooledsd '
    ;    if rtn.count eq 1 and rtn.files[0] eq 'trials' then outstr = outstr + ' ONLY'
    ;    if rtn.files[0] eq 'pooledsd' then outstr = outstr + ' -trials ONLY'
    ;endif

    ;if glmfiles[0] ne '' then mask_str=compute_residuals_struct.mask_str else mask_str[*]  = ' -mask ' + get_mask(space[0],fi)
    ;START190920
    brainmask='*brainmask*'
    if gg.imgselect[0] ne '' and n_elements(gg.imgselect) eq 1 then begin
        mfile=gg.imgselect[0]
    endif else if gg.niiselect[0] ne '' then begin
        superbird=gg.niiselect[indgen(n_elements(gg.niiselect)/total_nsubjects)*total_nsubjects]
        brainmaski=where(strmatch(superbird,brainmask) eq 1,cnt)
        if cnt ne 0 then begin
            mfile=brainmask
        endif
    endif
    if glmfiles[0] ne '' then begin
        mask_str=compute_residuals_struct.mask_str
    endif else begin
        scrap=get_mask(space[0],fi,mfile)
        if scrap eq 'GO_BACK' then goto,goback4
        mask_str[*]  = ' -mask ' + gg.niiselect[indgen(total_nsubjects)+brainmaski*total_nsubjects]
        print,'mask_str'
        print,mask_str
    endelse
endif
if fano eq 1 then begin
    goback41:
    tcselect=strarr(total_nsubjects,ntc)
    for i=0,ntc-1 do begin
        widget_control,/HOURGLASS
        gf=get_files(FILTER='*'+names_tc_big[i]+'*.4dfp.img',TITLE=names_tc_big[i]+': '+'Please enter extracted timecourse filter.')
        if gf.msg eq 'GO_BACK' then goto,goback4 else if gf.msg eq 'EXIT' then return
        scrap = gf.files
        if n_elements(scrap) eq 1 and n_elements(concselect) eq 1 and gf.msg eq 'DONTCHECK' then begin 
            ;do nothing 
        endif else begin
            scrap = match_files(concselect,scrap,TITLE=names_tc_big[i]+': '+'Please select timecourse for ',/GO_BACK)
            if scrap[0] eq 'GO_BACK' then goto,goback41
        endelse
        tcselect[*,i]=scrap
    endfor
    goback42:
    varselect=strarr(total_nsubjects,ntc)
    for i=0,ntc-1 do begin
        widget_control,/HOURGLASS
        gf=get_files(FILTER='*'+names_tc_big[i]+'*var.4dfp.img',TITLE=names_tc_big[i]+': '+'Please enter residual variance filter.')
        if gf.msg eq 'GO_BACK' then goto,goback41 else if gf.msg eq 'EXIT' then return
        scrap = gf.files
        if n_elements(scrap) eq 1 and n_elements(concselect) eq 1 and gf.msg eq 'DONTCHECK' then begin 
            ;do nothing 
        endif else begin
            scrap = match_files(concselect,scrap,TITLE=names_tc_big[i]+': '+'Please select variance for ',/GO_BACK)
            if scrap[0] eq 'GO_BACK' then goto,goback42
        endelse
        varselect[*,i]=scrap
    endfor
    tcifhdim4 = intarr(total_nsubjects,ntc)
    varifhdim4 = intarr(total_nsubjects,ntc)
    for i=0,ntc-1 do begin
        for j=0,total_nsubjects-1 do begin
            ifh = read_mri_ifh(tcselect[j,i])
            tcifhdim4[j,i] = ifh.matrix_size_4
            tcspace = get_space(ifh.matrix_size_1,ifh.matrix_size_2,ifh.matrix_size_3)
            ifh = read_mri_ifh(varselect[j,i])
            varifhdim4[j,i] = ifh.matrix_size_4
            varspace = get_space(ifh.matrix_size_1,ifh.matrix_size_2,ifh.matrix_size_3)
            if(tcspace-varspace ne 0) or (tcspace-space[j]) ne 0 then begin
                stat=dialog_message('Images are not in the same space.'+string(10B)+string(10B)+tcselect[j,i] $
                    +string(10B)+varselect[j,i]+string(10B)+concselect[j],/ERROR)
                return
            endif
            if tcifhdim4[j,i] ne varifhdim4[j,i] then begin
                stat=dialog_message('Images do not have the same time dimension.'+string(10B)+string(10B)+tcselect[j,i] $
                    +string(10B)+varselect[j,i],/ERROR)
                return
            endif
        endfor
    endfor
    for i=0,ntc-1 do begin
        if total(tcifhdim4[*,0]-tcifhdim4[*,i]) ne 0 then begin
            stat=dialog_message('Time dimension is not consistent across subjects for '+names_tc_big[i],/ERROR)
            return
        endif
    endfor
endif else begin
    ts_tp_max_big = intarr(total_nsubjects,ntc)
    for m=0,total_nsubjects-1 do begin
        print,'*** '+evselect[m]+' ***'
        tdim_sum = *stc2[m].tdim_sum
        nrun = stc2[m].n

        print,'nrun=',nrun

        lcround_tc_index = intarr(sng[m].num_levels)
        scrap = frames_ss(sng[m],nrun,tdim_sum,lcround_tc_index)
        for i=0,ntc-1 do begin
            index_tc1 = sum_contrast_for_treatment[m,i,0:counts[m,i]-1]-1
            ts_tp_max_big[m,i] = max(scrap.conditions_max[index_tc1])
            ;print,'scrap.conditions_max[index_tc1]'
            ;print,scrap.conditions_max[index_tc1]
            ;print,'max(scrap.conditions_max[index_tc1])=',max(scrap.conditions_max[index_tc1])
        endfor
    endfor


    ;tp_max_big = intarr(ntc)
    ;for i=0,ntc-1 do tp_max_big[i] = min(ts_tp_max_big[*,i])
    ;START151112
    tp_max_big=min(ts_tp_max_big,DIMENSION=1,MAX=tp_max_big_max)

    ;print,'ts_tp_max_big'
    ;print,ts_tp_max_big
    ;print,'tp_max_big=',tp_max_big
    ;print,'tp_max_big_max=',tp_max_big_max
    
    goback60:
    repeat begin
        stat = 'OK'

        ;scrap = get_str(ntc,names_tc_big,tp_max_big,WIDTH=40,TITLE='How many timepoints?',/FRONT,/GO_BACK)
        ;START161219
        scrap = get_str(ntc,names_tc_big,tp_max_big,WIDTH=40,TITLE='How many timepoints?',/FRONT,/GO_BACK,LABEL='To get ' $
            +'means/standard deviations over over trials/epochs, enter 1, then select all frames in the next window.')

        if scrap[0] eq 'GO_BACK' then goto,goback40
        length_tc_big = fix(scrap)
        if total((length_tc_big-tp_max_big)>0) gt 0 then begin
            ;print,'length_tc_big=',length_tc_big,' tp_max_big=',tp_max_big
            stat = dialog_message('Your BOLD responses will overlap.',/CANCEL)
        endif
    endrep until stat eq 'OK'
    ;print,'length_tc_big=',length_tc_big

    ;if pooledsdstr ne '' then pooledsdstr = pooledsdstr+strjoin(trim(length_tc_big),' ',/SINGLE)
    ;START151120
    pooledsdstr0=strjoin(trim(length_tc_big),' ',/SINGLE)

    goback61:
    max_tp_max_big = max(tp_max_big)
    index_time = intarr(ntc,max(length_tc_big),max_tp_max_big)
    sumrows_time = intarr(ntc,max(length_tc_big))
    timept_label = make_array(ntc,/STRING,VALUE='')
    if max_tp_max_big eq 1 then $
        sumrows_time[*,*] = 1 $
    else if total(length_tc_big-tp_max_big) eq 0 then begin
        sumrows_time[*,*] = 1
        for i=0,ntc-1 do index_time[i,0:length_tc_big[i]-1,0] = indgen(length_tc_big[i]);
    endif else begin
        if total((length_tc_big-tp_max_big)>0) gt 0 then lcoverlap=1 else lcoverlap=0
        if lcoverlap eq 1 then begin
            sumrows_time[*,*] = 1
            for i=0,ntc-1 do index_time[i,0:length_tc_big[i]-1,0] = indgen(length_tc_big[i]);
        endif else begin
            tlength_tc_big = total(length_tc_big)



            ;scraparr = strarr(max_tp_max_big+1,tlength_tc_big)
            ;scraplabel = ''
            ;lengths = -1
            ;k=0
            ;for i=0,ntc-1 do begin
            ;    for j=0,length_tc_big[i]-1 do begin
            ;        scraparr[0:tp_max_big[i],k] = [strtrim(indgen(tp_max_big[i])+1,2),'ALL']
            ;        k = k + 1
            ;    endfor
            ;    scraplabel = [scraplabel,names_tc_big[i]+' '+strtrim(indgen(length_tc_big[i])+1,2)]
            ;    lengths = [lengths,make_array(length_tc_big[i],/INTEGER,VALUE=tp_max_big[i]+1)]
            ;endfor
            ;scraplabel = scraplabel[1:*]
            ;lengths = lengths[1:*]
            ;widget_control,/HOURGLASS
            ;rtn = get_choice_list_many(scraplabel,scraparr,TOP_TITLE='Please select timepoints.',TITLE='Multiple ' $
            ;    +'timepoints are averaged.',/LABEL_TOP,/NONEXCLUSIVE,/GO_BACK,LENGTHS=lengths,/SET_DIAGONAL, $
            ;    SPECIAL='One at a time',/SET_UNSET)
            ;if rtn.special eq 'GO_BACK' then goto,goback60
            ;if rtn.special ne 'SPECIAL' then $
            ;    scraplist = rtn.list $
            ;else begin
            ;    scraplist = intarr(max_tp_max_big+1,tlength_tc_big)
            ;    l=0
            ;    for i=0,ntc-1 do begin
            ;        scraparr = strarr(tp_max_big[i]+1,length_tc_big[i])
            ;        k=0
            ;        for j=0,length_tc_big[i]-1 do begin
            ;            scraparr[0:tp_max_big[i],k] = [strtrim(indgen(tp_max_big[i])+1,2),'ALL']
            ;            k = k + 1
            ;        endfor
            ;        scraplabel = names_tc_big[i]+' '+strtrim(indgen(length_tc_big[i])+1,2)
            ;        lengths = make_array(length_tc_big[i],/INTEGER,VALUE=tp_max_big[i]+1)
            ;        rtn = get_choice_list_many(scraplabel,scraparr,TOP_TITLE='Please select timepoints.', $
            ;            TITLE='Multiple timepoints are averaged.',/LABEL_TOP,/NONEXCLUSIVE,/GO_BACK,LENGTHS=lengths,/SET_DIAGONAL)
            ;        scraplist[0:tp_max_big[i],l:l+length_tc_big[i]-1] = rtn.list
            ;        l=l+length_tc_big[i]
            ;    endfor
            ;endelse
            ;k=0
            ;for i=0,ntc-1 do begin
            ;    for j=0,length_tc_big[i]-1 do begin
            ;        if scraplist[tp_max_big[i],k] eq 1 then scraplist[0:tp_max_big[i]-1,k] = 1
            ;        index = where(scraplist[0:tp_max_big[i]-1,k],count)
            ;        if count ne 0 then begin
            ;            index_time[i,j,0:count-1] = index
            ;            sumrows_time[i,j] = count
            ;            timept_label[i]= timept_label[i]+strjoin(strtrim(index+1,2),'+',/SINGLE)
            ;        endif
            ;        k = k + 1
            ;    endfor
            ;endfor
            ;timept_label = strtrim(timept_label,2)
            ;START151112
            useallvalidframes=0

            ;idx=where(length_tc_big ne 1,cnt)
            ;if cnt eq 0 then begin
            ;START151208
            idx=where(length_tc_big ne 1,cnt_ne_1)
            if cnt_ne_1 eq 0 then begin

                idx=where(tp_max_big-tp_max_big_max ne 0,cnt)
                if cnt ne 0 then begin
                    scrap=get_button(['yes','no, let me select','go back','exit'],TITLE='Do you wish to use all the valid frames?',$
                        BASE_TITLE='Timecourses have variable number of frames.')
                    if scrap eq 3 then return else if scrap eq 2 then goto,goback60 else if scrap eq 0 then useallvalidframes=1
                    index_time = intarr(ntc,max(length_tc_big),max(tp_max_big_max))
                    for i=0,ntc-1 do begin
                        for j=0,length_tc_big[i]-1 do begin
                            index_time[i,j,0:tp_max_big_max[i]-1]=indgen(tp_max_big_max[i])
                        endfor
                    endfor
                    undefine,sumrows_time
                endif
            endif
            if useallvalidframes eq 0 then begin
                scraparr = strarr(max_tp_max_big+1,tlength_tc_big)
                scraplabel = ''
                lengths = -1
                k=0
                for i=0,ntc-1 do begin
                    for j=0,length_tc_big[i]-1 do begin
                        scraparr[0:tp_max_big[i],k] = [strtrim(indgen(tp_max_big[i])+1,2),'ALL']
                        k = k + 1
                    endfor
                    scraplabel = [scraplabel,names_tc_big[i]+' '+strtrim(indgen(length_tc_big[i])+1,2)]
                    lengths = [lengths,make_array(length_tc_big[i],/INTEGER,VALUE=tp_max_big[i]+1)]
                endfor
                scraplabel = scraplabel[1:*]
                lengths = lengths[1:*]
                widget_control,/HOURGLASS
                rtn = get_choice_list_many(scraplabel,scraparr,TOP_TITLE='Please select timepoints.',TITLE='Multiple ' $
                    +'timepoints are averaged.',/LABEL_TOP,/NONEXCLUSIVE,/GO_BACK,LENGTHS=lengths,/SET_DIAGONAL, $
                    SPECIAL='One at a time',/SET_UNSET)
                if rtn.special eq 'GO_BACK' then goto,goback60
                if rtn.special ne 'SPECIAL' then $
                    scraplist = rtn.list $
                else begin
                    scraplist = intarr(max_tp_max_big+1,tlength_tc_big)
                    l=0
                    for i=0,ntc-1 do begin
                        scraparr = strarr(tp_max_big[i]+1,length_tc_big[i])
                        k=0
                        for j=0,length_tc_big[i]-1 do begin
                            scraparr[0:tp_max_big[i],k] = [strtrim(indgen(tp_max_big[i])+1,2),'ALL']
                            k = k + 1
                        endfor
                        scraplabel = names_tc_big[i]+' '+strtrim(indgen(length_tc_big[i])+1,2)
                        lengths = make_array(length_tc_big[i],/INTEGER,VALUE=tp_max_big[i]+1)
                        rtn = get_choice_list_many(scraplabel,scraparr,TOP_TITLE='Please select timepoints.', $
                            TITLE='Multiple timepoints are averaged.',/LABEL_TOP,/NONEXCLUSIVE,/GO_BACK,LENGTHS=lengths,/SET_DIAGONAL)
                        scraplist[0:tp_max_big[i],l:l+length_tc_big[i]-1] = rtn.list
                        l=l+length_tc_big[i]
                    endfor
                endelse
                k=0
                for i=0,ntc-1 do begin
                    for j=0,length_tc_big[i]-1 do begin
                        if scraplist[tp_max_big[i],k] eq 1 then scraplist[0:tp_max_big[i]-1,k] = 1
                        index = where(scraplist[0:tp_max_big[i]-1,k],count)
                        if count ne 0 then begin
                            index_time[i,j,0:count-1] = index
                            sumrows_time[i,j] = count
                            timept_label[i]= timept_label[i]+strjoin(strtrim(index+1,2),'+',/SINGLE)
                        endif
                        k = k + 1
                    endfor
                endfor
                timept_label=trim(timept_label)
            endif




        endelse
    endelse

    ;START151120
    ;goback62:
    ;outstr=''
    ;pooledsdstr=''
    ;if analysis_type eq 0 then begin
    ;    scrap=['mean','sem','sd','var','pooledsd','trials']
    ;    if useallvalidframes eq 0 then begin 
    ;        rtn = select_files(scrap,TITLE='Please select',BASE_TITLE='Output options', $
    ;            /GO_BACK,/EXIT)
    ;        if rtn.files[0] eq 'GO_BACK' then goto,goback61 else if rtn.files[0] eq 'EXIT' then return
    ;        idx = where(rtn.files ne 'pooledsd',cnt)
    ;        if cnt ne 0 then outstr = ' -' + strjoin(rtn.files[idx],' -',/SINGLE)
    ;        idx = where(rtn.files eq 'pooledsd',cnt)
    ;        if cnt ne 0 then pooledsdstr = ' -pooledsd '+pooledsdstr0
    ;        if rtn.count eq 1 and rtn.files[0] eq 'trials' then outstr = outstr + ' ONLY'
    ;        if rtn.files[0] eq 'pooledsd' then outstr = outstr + ' -trials ONLY'
    ;    endif else begin
    ;        rtn=select_files(['mean across trials','sem across trials','sd across trials','var across trials',$
    ;            'pooledsd across trials','trials','mean across TRs','standard deviation across TRs'],TITLE='Please select',$
    ;            BASE_TITLE='Output options',/GO_BACK,/EXIT)
    ;        if rtn.files[0] eq 'GO_BACK' then goto,goback61 else if rtn.files[0] eq 'EXIT' then return
    ;        scrap=[scrap,'meanTR','sdTR']
    ;        files=scrap[rtn.index]
    ;        idx = where(files ne 'pooledsd' and files ne 'trials',cnt)
    ;        if cnt ne 0 then outstr = ' -' + strjoin(files[idx],' -',/SINGLE)
    ;        idx = where(files eq 'pooledsd',cnt)
    ;        if cnt ne 0 then pooledsdstr=' -pooledsd '+pooledsdstr0
    ;        if files[0] eq 'trials' then begin
    ;            outstr=outstr+' -trials'
    ;            if rtn.count eq 1 then outstr=outstr+' ONLY'
    ;        endif
    ;        if files[0] eq 'pooledsd' then outstr=outstr+' -trials ONLY'
    ;    endelse
    ;endif
    ;START151208
    ;goback62:
    ;outstr=''
    ;pooledsdstr=''
    ;if analysis_type eq 0 then begin
    ;    if cnt_ne_1 ne 0 then scrap=['mean','sem','sd','var','pooledsd','trials'] else scrap=['mean','sem','sd','var','trials']
    ;    if useallvalidframes eq 0 then begin
    ;        rtn = select_files(scrap,TITLE='Please select',BASE_TITLE='Output options', $
    ;            /GO_BACK,/EXIT)
    ;        if rtn.files[0] eq 'GO_BACK' then goto,goback61 else if rtn.files[0] eq 'EXIT' then return
    ;        idx = where(rtn.files ne 'pooledsd',cnt)
    ;        if cnt ne 0 then outstr = ' -' + strjoin(rtn.files[idx],' -',/SINGLE)
    ;        idx = where(rtn.files eq 'pooledsd',cnt)
    ;        if cnt ne 0 then pooledsdstr = ' -pooledsd '+pooledsdstr0
    ;        if rtn.count eq 1 and rtn.files[0] eq 'trials' then outstr = outstr + ' ONLY'
    ;        if rtn.files[0] eq 'pooledsd' then outstr = outstr + ' -trials ONLY'
    ;    endif else begin
    ;        if cnt_ne_1 ne 0 then $
    ;            wallace=['mean across trials','sem across trials','sd across trials','var across trials','pooledsd across trials', $
    ;                'trials','mean across TRs','standard deviation across TRs'] $
    ;        else $ 
    ;            wallace=['mean across trials','sem across trials','sd across trials','var across trials', $
    ;                'trials','mean across TRs','standard deviation across TRs']
    ;        rtn=select_files(wallace,TITLE='Please select',BASE_TITLE='Output options',/GO_BACK,/EXIT)
    ;        if rtn.files[0] eq 'GO_BACK' then goto,goback61 else if rtn.files[0] eq 'EXIT' then return
    ;        scrap=[scrap,'meanTR','sdTR']
    ;        files=scrap[rtn.index]
    ;        idx = where(files ne 'pooledsd' and files ne 'trials',cnt)
    ;        if cnt ne 0 then outstr = ' -' + strjoin(files[idx],' -',/SINGLE)
    ;        idx = where(files eq 'pooledsd',cnt)
    ;        if cnt ne 0 then pooledsdstr=' -pooledsd '+pooledsdstr0
    ;        if files[0] eq 'trials' then begin
    ;            outstr=outstr+' -trials'
    ;            if rtn.count eq 1 then outstr=outstr+' ONLY'
    ;        endif
    ;        if files[0] eq 'pooledsd' then outstr=outstr+' -trials ONLY'
    ;    endelse
    ;endif
    ;START190211
    goback62:
    outstr=''
    pooledsdstr=''


    ;START191014
    ;if analysis_type eq 0 then begin


        if cnt_ne_1 ne 0 then scrap=['mean','sem','sd','var','pooledsd','trials'] else scrap=['mean','sem','sd','var','trials']
        ;if useallvalidframes eq 0 then begin
        ;    rtn = select_files(scrap,TITLE='Please select',BASE_TITLE='Output options', $
        ;        /GO_BACK,/EXIT)
        ;    if rtn.files[0] eq 'GO_BACK' then goto,goback61 else if rtn.files[0] eq 'EXIT' then return
        ;    idx = where(rtn.files ne 'pooledsd',cnt)
        ;    if cnt ne 0 then outstr = ' -' + strjoin(rtn.files[idx],' -',/SINGLE)
        ;    idx = where(rtn.files eq 'pooledsd',cnt)
        ;    if cnt ne 0 then pooledsdstr = ' -pooledsd '+pooledsdstr0
        ;    if rtn.count eq 1 and rtn.files[0] eq 'trials' then outstr = outstr + ' ONLY'
        ;    if rtn.files[0] eq 'pooledsd' then outstr = outstr + ' -trials ONLY'
        ;endif else begin
            if cnt_ne_1 ne 0 then $
                wallace=['mean across trials','sem across trials','sd across trials','var across trials','pooledsd across trials', $
                    'trials','mean across TRs','standard deviation across TRs'] $
            else $
                wallace=['mean across trials','sem across trials','sd across trials','var across trials', $
                    'trials','mean across TRs','standard deviation across TRs']
            rtn=select_files(wallace,TITLE='Please select',BASE_TITLE='Output options',/GO_BACK,/EXIT)
            if rtn.files[0] eq 'GO_BACK' then goto,goback61 else if rtn.files[0] eq 'EXIT' then return
            scrap=[scrap,'meanTR','sdTR']
            files=scrap[rtn.index]
            idx = where(files ne 'pooledsd' and files ne 'trials',cnt)
            if cnt ne 0 then outstr = ' -' + strjoin(files[idx],' -',/SINGLE)
            idx = where(files eq 'pooledsd',cnt)
            if cnt ne 0 then pooledsdstr=' -pooledsd '+pooledsdstr0
            if files[0] eq 'trials' then begin
                outstr=outstr+' -trials'
                if rtn.count eq 1 then outstr=outstr+' ONLY'
            endif
            if files[0] eq 'pooledsd' then outstr=outstr+' -trials ONLY'
        ;endelse


    ;START191014
    ;endif


;    rtn = get_bool_list(['averaged across trials','for all trials','GO BACK'],TITLE='I want regional timecourses')
;    regtc_type = rtn.list
;    if regtc_type[2] eq 1 then goto,goback100
;    if regtc_type[1] eq 1 then begin
;        regtc_type_str = ' -trials'
;        if regtc_type[0] eq 0 then begin
;            regtc_type_str = regtc_type_str + ' ONLY'
;            output = 0
;        endif
;    endif








    scrap = get_str(ntc,names_tc_big,names_tc_big,WIDTH=40,TITLE='Please name timecourses',/GO_BACK)

    ;if scrap[0] eq 'GO_BACK' then goto,goback61
    ;START151120
    if scrap[0] eq 'GO_BACK' then begin
        if analysis_type eq 0 then goto,goback62 else goto,goback61
    endif

    names_tc_str = ' -tc_names ' + strjoin(scrap,' ',/SINGLE)
endelse


;goback5:
;if fano eq 0 then begin
;    scrap = 'timecourse script'
;    scraplabels = 'fidl_tc_ss.csh'
;endif else begin
;    scrap = 'fano factor script'
;    scraplabels = 'fidl_fano.csh'
;endelse
;START170712
proc=1
proceach=1
wallace0=''
;if total_nsubjects gt 1 then begin
;    gp=getproc(pref,total_nsubjects)
if nsubject gt 1 then begin
    gp=getproc(pref,nsubject)
    proc=gp.proc
    proceach=gp.proceach
    wallace0=gp.wallace0
    undefine,gp
endif
if n_elements(times0) eq 0 then begin
    times0=strarr(proc)
    for i=0,proc-1 do begin
        ;spawn,!BINEXECUTE+'/fidl_timestr2',timestr
        ;START200312
        spawn,!BINEXECUTE+'/fidl_timestr3',timestr
        times0[i]=timestr
    endfor
endif
goback5:
if proc eq 1 then begin
    if fano eq 0 then begin
        scrap = 'timecourse script'
        scraplabels = 'fidl_tc_ss.csh'
    endif else begin
        scrap = 'fano factor script'
        scraplabels = 'fidl_fano.csh'
    endelse
endif else begin
    scrap='script'+wallace0
    ;scraplabels='fidl_tc_ss_'+times0+'.csh'
    ;START200312
    scraplabels='fidl_tc_ss'+times0+'.csh'
endelse




if strmid(concselect[0],strlen(concselect[0])-3) eq 'img' then lc_concs = 0 else lc_concs = 1
if lc_concs eq 1 then ext='.' else ext='.4dfp.img'
rtn_gr=get_root(concselect,ext)
scrap = [scrap,'output root subject ' + strtrim(indgen(nsubject)+1,2)]
idx = intarr(nsubject)
for i=1,nsubject-1 do idx[i]=idx[i-1]+nglm_per_subject[i-1]
scraplabels = [scraplabels,rtn_gr.file[idx]]
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter names.',/ONE_COLUMN,/GO_BACK,FRONT=directory+'/', $
    /REPLACE)
if names[0] eq 'GO_BACK' then goto,goback4


;script = directory+'/'+fix_script_name(names[0])
;START170712
script=strarr(proc)
for j=0,proc-1 do script[j] = fix_script_name(names[j])


;START170712
;j=1

if output eq 1 then out_str = ' -regional_name "' else out_str = ' -root "' 

;out_str = out_str + names[j:*]
;START170712
out_str=out_str+names[proc:*]

if output eq 1 then begin
    if fano eq 0 then out_str = out_str + '_fidl.txt' else out_str = out_str + '_fidl_fano.txt'
endif
out_str = out_str + '"'
if glmfiles[0] ne '' then wallace=compute_residuals_struct.scratchdir else wallace=directory+'/'

bf_str=wallace+names[proc:*]+'_bf.dat'
proceach0=0
proceach1=0
m=0
m3=0
idx_tc = strarr(ntc)
for m1=0,nsubject-1 do begin
    if proceach0 eq 0 then begin
        print,'m1=',m1,' proceach1=',proceach1
        print,'***************** OPENW '+script[proceach1]+' *****************'
        openw,lu,script[proceach1],/GET_LUN
        top_of_script,lu
        if wfiles[0] ne '' then begin
            print_files_to_csh,lu_csh,n_elements(wfiles),wfiles,'WEIGHT_FILES','weight_files'
            wfiles = ' $WEIGHT_FILES'
        endif
        if analysis_type eq 1 then begin

            ;if n_elements(gg.niiselect) ne nsubject then begin
            ;    print_files_to_csh,lu,n_elements(region_files),region_files,'REGION_FILE','region_file'
            ;endif
            ;printf,lu,'set ROI = ('+strtrim(roi_str[0],2)+')'
            ;printf,lu,'set REGVAL = ('+strtrim(regval_str[0],2)+')'
            ;region_str = ' $REGION_FILE $ROI $REGVAL'
            ;START200311
            if n_elements(gg.niiselect) ne nsubject then begin
                print_files_to_csh,lu,n_elements(region_files),region_files,'REGION_FILE','region_file'
            endif
            if roi_str[0] ne '' then printf,lu,'set ROI = ('+strtrim(roi_str[0],2)+')'
            printf,lu,'set REGVAL = ('+strtrim(regval_str[0],2)+')'
            if roi_str[0] ne '' then region_str = ' $REGION_FILE $ROI $REGVAL' else region_str = ' $REGION_FILE $REGVAL'



        endif
    endif

    ;START191025
    if n_elements(gg.niiselect) eq nsubject then begin 
        print_files_to_csh,lu,1,gg.niiselect[m1],'REGION_FILE','region_file'
    endif

    inc = 0
    idx_tc[*] = '' 
    idx_tcs = ''
    tdim_all = 0 
    concstr=''
    widget_control,/HOURGLASS
    for m2=0,nglm_per_subject[m1]-1 do begin
        sampling_str = ''
        offset_str = ''
        print,'*** '+concselect[m]+' ***'
        tdim_sum = *stc2[m].tdim_sum
        nrun = stc2[m].n
        lcround_tc_index = intarr(sng[m].num_levels)
        if fano eq 0 then begin
            index = -1
            ntrial_tc = intarr(ntc)
            for i=0,ntc-1 do begin
                scrap = -1
                for j=0,counts[m,i]-1 do begin
                    dummy = where(*sng[m].conditions eq (sum_contrast_for_treatment[m,i,j]-1),count)
                    scrap = [scrap,dummy]
                    ntrial_tc[i] = ntrial_tc[i] + count
                endfor
                index = [index,scrap[1:*]]
                idx_tc[i] = idx_tc[i] + idx_tcs + strjoin(trim(sum_contrast_for_treatment[m,i,0:counts[m,i]-1]-1+inc),'+',/SINGLE) 
            endfor
            index_raw = index[1:*]
            uisi = uniq(index_raw,sort(index_raw))
            index = index_raw[uisi]
            if nearTRcount ne 0 then begin
                sampling_str=-1
                for i=0,ntc-1 do begin
                    lcround_tc_index[sum_contrast_for_treatment[m,i,0:counts[m,i]-1]] = nearTRlist[i]
                    sampling_str = [sampling_str,make_array(counts[m,i],/INT,VALUE=nearTRlist[i])]
                endfor
                sampling_str = ' -placing ' + strjoin(strtrim((sampling_str[1:*]*2)>1,2),' ',/SINGLE)
                offsets = *sng[m].offsets
                offset = -1
                j = 0
                for i=0,ntc-1 do begin
                    offset = [offset,offsets[index_raw[j:j+ntrial_tc[i]-1],nearTRlist[i]]]
                    j = j + ntrial_tc[i]
                endfor
                offset = offset[1:*]
                offset = offset[uisi]
                offset_str = 'set OFFSET = (-offset ' + strjoin(strtrim(offset,2),' ',/SINGLE) + ')'
            endif
            scrap = frames_ss(sng[m],nrun,tdim_sum,lcround_tc_index)
            boldframes = scrap.boldframes[index]
            nboldframes = n_elements(boldframes)
            trialframesall = scrap.trialframes

            ;print,'here0 scrap.trialframes=',scrap.trialframes
            ;print,'here0 index=',index


            trialframes = scrap.trialframes[index]
            trialframes_overlap = scrap.trialframes_overlap[index]
            conditions_max = scrap.conditions_max
            run = scrap.run
            if total((length_tc_big-tp_max_big)>0) gt 0 then lcoverlap=1 else lcoverlap=0
            if lcoverlap eq 0 then tf_use = trialframes else tf_use = trialframes_overlap
            rtn_gi = get_index2(ntc,index_conditions,sumrows,sng[m],trialframesall,run)
            boldframes = rtn_gi.boldframes
            index_which_level = rtn_gi.index_which_level
            boldconditions = rtn_gi.boldconditions + inc
            boldtf = rtn_gi.boldtf
            boldrun = rtn_gi.boldrun

            ;print,'here0 m2=',m2,' boldrun=',boldrun

            rtn_bf=print_boldframes(length_tc_big,sumrows_time,index_time,boldtf,boldframes,bf_str[m1],index_which_level, $
                boldconditions,boldrun,dummy_trialframesindexed,dummy_filenames,dummmy_treatment_str,*stc2[m].tdim_sum_new, $
                APPEND=tdim_all)

            ;print,'here1'

            if nglm_per_subject[m1] gt 1 then begin
                idx = -1
                for i=0,ntc-1 do idx = [idx,reform(sum_contrast_for_treatment[m,i,0:counts[m,i]-1])]
                inc = inc + max(idx[1:*])
            endif
        endif else begin
            rtn_gi = get_index(ntc,1,index_conditions,sumrows,nc,sng[m])
            index = rtn_gi.index
            boldframes = rtn_gi.boldframes
            index_which_level = rtn_gi.index_which_level
            boldconditions = rtn_gi.boldconditions
            index_whichc = rtn_gi.index_whichc
            ic = rtn_gi.ic
            ic_by_levels = rtn_gi.ic_by_levels
            nic_by_levels = rtn_gi.nic_by_levels
            rtn_frames_ss = frames_ss(sng[m],nrun,tdim_sum)
            trialframes = rtn_frames_ss.trialframes[index]
            conditions_max = rtn_frames_ss.conditions_max
            tp_max = conditions_max[index_tc]
            tp_max_big = intarr(ntc)
            k = 0
            for i=0,ntc-1 do begin
                tp_max_big[i] = max(tp_max[k:k+counts[m,i]-1])
                k = k + rtn_ic.sumrows[i]
            endfor
            if m eq 0 then begin
                goback90:
                scraparr = strarr(max(tp_max_big)+1,ntc)
                for i=0,ntc-1 do scraparr[0:tp_max_big[i],i] = [strtrim(indgen(tp_max_big[i])+1,2),'ALL']
                title = 'Please select timepoints.'
                rtn = get_choice_list_many(names_tc_big,scraparr,TITLE=title,/LABEL_TOP,/NONEXCLUSIVE,/GO_BACK,LENGTHS=tp_max_big+1)
                if rtn.special eq 'GO_BACK' then goto,goback5
                scraplist = rtn.list
                for i=0,ntc-1 do begin
                    if scraplist[tp_max_big[i],i] eq 1 then begin
                        ;print,'SHOULD BE HERE'
                        scraplist[0:tp_max_big[i]-1,i] = 1
                        scraplist[tp_max_big[i],i] = 0
                    endif
                endfor
                nframes = total(scraplist,1)
                conditions_time = intarr(ntc,max(nframes))
                sumrows_time = make_array(ntc,max(nframes),/INT,VALUE=1)
                index_time = intarr(ntc,max(nframes),1)
                for i=0,ntc-1 do begin
                    index = where(scraplist[0:tp_max_big[i]-1,0],count)
                    if count eq 0 then begin
                        stat = dialog_message('Something is wrong hereA.',/ERROR)
                        return
                    endif
                    conditions_time[i,0:nframes[i]-1] = index + 1
                    index_time[i,0:nframes[i]-1,0] = index
                endfor
                i1=where((tcifhdim4[m,*]-nframes) lt 0,c1)
                if c1 ne 0 then begin
                    scrap = total(nframes-nframes[0])
                    if ntc eq 2 and total(tcifhdim4[m,*]) eq nframes[0] and scrap eq 0 then begin
                        scrap=get_button(['yes','go back','exit'],BASE_TITLE='Images have fewer time frames than selected', $
                            TITLE='Do you wish to concatenate the timecourses?')
                        if scrap eq 2 then return else if scrap eq 1 then goto,goback90
                        catstr = ' -concatenate'
                        tcstr=' -tc'
                        for i=0,ntc-1 do tcstr=tcstr+' '+strjoin(strtrim(indgen(tcifhdim4[m,i])+1,2),',',/SINGLE)
                    endif else begin
                        stat=dialog_message('Images have fewer time frames than selected.',/ERROR)
                        return
                    endelse
                endif else begin
                    tcstr=' -tc'
                    scrap = strtrim(conditions_time,2)
                    for i=0,ntc-1 do tcstr=tcstr+' '+strjoin(scrap[i,0:nframes[i]-1],',',/SINGLE)
                endelse
                scrap = get_button(['coefficient of variation','Fano factor','go back'],TITLE='Please select', $
                    BASE_TITLE='What do you want to compute?')
                if scrap eq 0 then catstr = catstr + ' -coef_of_var'
            endif
            rtn_bf=print_boldframes(nframes,sumrows_time,index_time,tf_use,boldframes,bf_str[m],index_which_level,boldconditions,/FANO)
        endelse
        if m eq 0 then begin

            ;action = get_button(['execute','return','GO BACK'],TITLE='Please select action.',BASE_TITLE=script)
            ;START170712
            action=get_button(['execute','return','go back'],TITLE='Please select action.')

            if action eq 2 then begin
                close,lu
                free_lun,lu
                spawn,'rm -f '+script
                goto,goback5
            endif
        endif
        if offset_str ne '' then begin
            printf,lu,offset_str
            printf,lu,''
            offset_str = ' $OFFSET'
        endif
        if glmfiles[0] ne '' then begin
            printf,lu,compute_residuals_struct.cmdstr[m]
            concstr = concstr+' '+compute_residuals_struct.acconc[m]
            cleanupstr = ' -clean_up'
        endif else $
            concstr = concstr+' '+concselect[m]
        idx_tcs = '+'
        tdim_all = tdim_all + stc2[m].tdim_all
        m=m+1
    endfor
    if fano eq 0 then begin
        str='nice +19 $BIN/fidl_tc_ss' 
        index_tc_str = ' -tc_cond ' + strjoin(idx_tc,' ',/SINGLE)
    endif else str='nice +19 $BIN/fidl_fano'
    str = str+wfiles+names_tc_str+index_tc_str+region_str+t4_str[m1]+out_str[m1]+regtc_type_str+' -tc_files'+concstr $
        +' -cond_and_frames "'+bf_str[m1]+'"'+mask_str[m3]+sampling_str+regtypestr+offset_str+outstr+pooledsdstr $
        +' -event_file '+sng[m3].event_file+cleanupstr


    ;if m1 eq (nsubject-1) then str=str+' -scratchdir '+compute_residuals_struct.scratchdir
    ;START170712
    ;if proc eq 1 then begin
    ;    if m1 eq (nsubject-1) then str=str+' -scratchdir '+compute_residuals_struct.scratchdir
    ;endif
    ;START190211
    if glmfiles[0] ne '' then begin
        if proc eq 1 then begin
            if m1 eq (nsubject-1) then str=str+' -scratchdir '+compute_residuals_struct.scratchdir
        endif
    endif


    if fano eq 1 then str=str+' -tc_file "'+strjoin(tcselect[m,*],'" "',/SINGLE)+'"'+' -var_file "' $
        +strjoin(varselect[m,*],'" "',/SINGLE)+'"'+tcstr+catstr
    print,str
    printf,lu,str
    printf,lu,''
    m3 = m3 + nglm_per_subject[m1]


    ;START170712
    proceach0=proceach0+1
    if ((proceach0 eq proceach) or (m1 eq nsubject-1)) then begin
        close,lu
        free_lun,lu
        print,'Script written to '+script[proceach1]
        spawn,'chmod 0777 '+script[proceach1]
        if action eq 0 then spawn,script[proceach1]+'>'+script[proceach1]+'.log'+' &'
        proceach0=0
        proceach1=proceach1+1
    endif


endfor ;for m1=0,nsubject-1 do begin


;START170712
;for m=0,total_nsubjects-1 do printf,lu,'# '+sng[m].event_file
;close,lu
;free_lun,lu
;spawn,'chmod +x '+script
;widget_control,/HOURGLASS
;if action eq 0 then begin
;    scrap='>'+script+'.log'
;    spawn,script+' &'
;    stats = dialog_message(script+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
;endif


print,'DONE'
end
