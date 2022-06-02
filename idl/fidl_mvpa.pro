;Copyright 12/8/11 Washington University.  All Rights Reserved.
;fidl_mvpa.pro  $Revision: 1.36 $
pro fidl_mvpa,fi,dsp,wd,glm,help,stc,pref,FANO=fano

;START170224
spawn,!BINEXECUTE+'/fidl_timestr2',timestr

if not keyword_set(FANO) then fano=0
directory=getenv('PWD')
roi_str = ''
wfiles = ''
regtc_type_str = ''
mask_str = ''
regtypestr = '' 
gohere=0
analtype=0
outstr = ''
tcstr=''
catstr=''
cleanupstr=''
normstr = ''
scratchdir_str = ''
atlas_str = ''
compress_str = ''
goback1:
widget_control,/HOURGLASS
get_glms_struct=get_glms(pref,GET_THIS='concs/imgs/glms',FILTER='*.glm')
if get_glms_struct.msg eq 'GO_BACK' then goto,goback1 else if get_glms_struct.msg eq 'EXIT' then return
load = get_glms_struct.load
nlists = get_glms_struct.nlists
t4select = get_glms_struct.t4select
evselect = get_glms_struct.evselect
total_nsubjects = get_glms_struct.total_nsubjects
concselect = get_glms_struct.concselect
glmfiles = get_glms_struct.glmfiles
glm_space = get_glms_struct.glm_space
bold_space = get_glms_struct.bold_space
ifh1 = get_glms_struct.ifh
morethanone = get_glms_struct.morethanone
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
    if glmfiles[0] ne '' then begin
        get_glms_struct.glmfiles = glmfiles
        get_glms_struct.total_nsubjects = total_nsubjects
        get_glms_struct.t4select = t4select
        get_glms_struct.concselect = concselect
        get_glms_struct.evselect = evselect
        get_glms_struct.glm_space = glm_space
        get_glms_struct.bold_space = bold_space
        get_glms_struct.ifh = ifh1
    endif
endelse
goback11:
lctrial=0
lcrun=0
lcsession=0
lcsubject=0

;lizard = ['trial','run']
;if morethanone eq 1 then begin
;    lizard = [lizard,'session']
;    title1 = 'leave one out by trial, run and session'
;endif else $
;    title1 = 'leave one out by trial and run'
;START170216
nsession0=0
nrun0=0
if morethanone eq 1 then begin
    lizard = ['trial','run']
    nrun0=1
    for i=0,total_nsubjects-1 do begin
        if ifh1[i].glm_n_files gt 1 then begin
            lizard = [lizard,'session']
            nsession0=1
            goto,wallace0
        endif
    endfor         
endif else begin
    lizard = ['trial']
    for i=0,total_nsubjects-1 do begin
        if ifh1[i].glm_n_files gt 1 then begin
            lizard = [lizard,'run']
            nrun0=1
            goto,wallace0
        endif
    endfor
endelse
wallace0:


if nsubject gt 1 then lizard = [lizard,'subject']
rtn = get_bool_list(lizard,TITLE='Leave one out by',/GO_BACK,/EXIT,BASE_TITLE='Please select')
if rtn.list[0] eq -1 then goto,goback1 else if rtn.list[0] eq -2 then return
loostr = ''
if rtn.list[0] eq 1 then begin
    loostr = loostr + ' -trial'
    lctrial=1
endif


;if rtn.list[1] eq 1 then begin
;    loostr = loostr + ' -run'
;    lcrun=1
;endif
;j = 2
;if morethanone eq 1 then begin
;    if rtn.list[2] eq 1 then begin
;        loostr = loostr + ' -session'
;        lcsession=1
;        j = j+1
;    endif
;endif
;START170216
j=1
if nrun0 eq 1 then begin
    if rtn.list[1] eq 1 then begin
        loostr = loostr + ' -run'
        lcrun=1
    endif
    j=j+1
endif
if nsession0 eq 1 then begin
    if rtn.list[2] eq 1 then begin
        loostr = loostr + ' -session'
        lcsession=1
        j=j+1
    endif
endif



loostrall = loostr 
if nsubject gt 1 then begin
    if rtn.list[j] eq 1 then begin
        loostrall = loostrall + ' -subject'
        lcsubject=1
    endif
endif



if glmfiles[0] ne '' then begin
    compute_residuals_struct=compute_residuals(fi,dsp,wd,glm,help,stc,pref,GET_GLMS_STRUCT=get_glms_struct,/MVPA)
    if compute_residuals_struct.msg eq 'GO_BACK' then goto,goback1 else if compute_residuals_struct.msg eq 'EXIT' then return
    scratchdir_str = ' -scratchdir '+ compute_residuals_struct.scratchdir

    ;if compute_residuals_struct.mask ne '' then $
    ;    mask_str = ' -mask '+ compute_residuals_struct.mask $
    ;else $
    ;    mask_str = ' -xdim '+trim(ifh1.matrix_size_1)+' -ydim '+trim(ifh1.matrix_size_2)+' -zdim '+trim(ifh1.matrix_size_3)
    ;START170217
    ;mask_str=strarr(total_nsubjects)
    ;idx=where(compute_residuals_struct.mask eq '',cnt,COMPLEMENT=idx0,NCOMPLEMENT=idx0)
    ;if cnt0 ne 0 then mask_str[idx0]=' -mask '+compute_residuals_struct.mask
    ;if cnt ne 0 then begin
    ;    mask=get_mask(glm_space[0],fi,BASE_TITLE='Not all glms include masks')
    ;    if mask ne '' then begin
    ;        mask_str[idx]=' -mask '+mask
    ;    endif else begin
    ;        mask_str[idx]=' -xdim '+trim(ifh1.matrix_size_1)+' -ydim '+trim(ifh1.matrix_size_2)+' -zdim '+trim(ifh1.matrix_size_3)
    ;    endelse
    ;endif
    ;START170217
    if compute_residuals_struct.mask[0] ne '' then $
        mask_str = ' -mask '+ compute_residuals_struct.mask[0] $
    else $
        mask_str = ' -xdim '+trim(ifh1.matrix_size_1)+' -ydim '+trim(ifh1.matrix_size_2)+' -zdim '+trim(ifh1.matrix_size_3)

    if compute_residuals_struct.atlas_str ne '' then atlas_str = compute_residuals_struct.atlas_str
    analysis_type = compute_residuals_struct.analysis_type
    region_files = compute_residuals_struct.region_files
    roi_str = compute_residuals_struct.roi_str
    concselect = compute_residuals_struct.concselect
endif
if evselect[0] eq '' or gohere eq 1 then begin
    gohere = 1
    goback1a:
    widget_control,/HOURGLASS
    gf = get_files(FILTER='*fi??',TITLE='Please enter event file filter.',EXTRALABEL='Enter NONE if none.')
    if gf.msg eq 'GO_BACK' then goto,goback1 else if gf.msg eq 'EXIT' then return
    evselect = gf.files
    if evselect[0] ne 'NONE' then begin
        if n_elements(concselect) eq 1 and n_elements(evselect) eq 1 and gf.msg eq 'DONTCHECK' then begin
            ;do nothing
        endif else begin 
            evselect = match_files(concselect,evselect,TITLE='Please select event file for ',/GO_BACK)
            if evselect[0] eq 'GO_BACK' then goto,goback1a
        endelse
    endif else $
        evselect = make_array(total_nsubjects,/STRING,VALUE='NONE')
endif else begin
    evselect = get_str(total_nsubjects,glmfiles,evselect,TITLE='Please check event files.',/ONE_COLUMN,/BELOW,SPECIAL='Load evs', $
        LABEL='You can use a different event file for MVPA then used for the GLM.'+string(10B)+'This allows you to customize your ' $
        +'trial structure.')
    if evselect[0] eq 'SPECIAL' then begin
        goback2:
        gf = get_files(TITLE='Need event files. Please enter filter.',FILTER='*.fidl')
        if gf.msg eq 'GO_BACK' then goto,goback11 else if gf.msg eq 'EXIT' then return
        evfiles = gf.files
        if n_elements(evfiles) eq 1 and total_nsubjects gt 1 then begin
            scrap = get_button(['ok','go back','exit'],BASE_TITLE='Please select.',TITLE='We have found a single ev'+string(10B)+ $
                string(10B)+evfiles[0]+string(10B)+string(10B)+'All residuals will use this event file.')
            if scrap eq 1 then goto,goback2 else if scrap eq 2 then return
            evselect = make_array(total_nsubjects,/STRING,VALUE=evfiles[0])
        endif else if n_elements(evfiles) eq 1 and total_nsubjects eq 1 and gf.msg eq 'DONTCHECK' then $ 
            evselect = evfiles $
        else $
            evselect = match_files(glmfiles,evfiles,TITLE='Please select event file for ')
    endif
endelse
t4_str=make_array(total_nsubjects,/STRING,VALUE='')
space = intarr(total_nsubjects)
stc2 = replicate({Stitch},total_nsubjects)
ifh2 = replicate({InterFile_Header},total_nsubjects)
sng = replicate({Eventfile},total_nsubjects)
widget_control,/HOURGLASS
for m=0,total_nsubjects-1 do begin
    print,'Loading ',concselect[m]
    rtn=load_conc(fi,stc,dsp,help,wd,concselect[m],/DONT_PUT_IMAGE)
    if rtn.msg eq 'GOBACK' then goto,goback11 else if rtn.msg eq 'EXIT' then return 
    stc2[m]=rtn.stc1
    ifh2[m] = rtn.ifh
    space[m] = get_space(ifh2[m].matrix_size_1,ifh2[m].matrix_size_2,ifh2[m].matrix_size_3)
    sng[m] = define_single_trial(fi,pref,help,EVENT_FILE=evselect[m])
    if sng[m].name eq 'ERROR' then return
endfor
index_lc_t4=where(space ne !SPACE_111 and space ne !SPACE_222 and space ne !SPACE_333,count_lc_t4)
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
goback8:
scrap = get_str(1,'number of classes','2',WIDTH=40,TITLE='How many classes?',/FRONT,/GO_BACK)
if scrap[0] eq 'GO_BACK' then goto,goback1
ntc = fix(scrap[0])

;START130925
goback9:
treatment_str = 'class' + strtrim(indgen(ntc)+1,2)

;START130925
;scrap = get_str(ntc,treatment_str,treatment_str,WIDTH=100,TITLE='Please name classes',/GO_BACK)
;if scrap[0] eq 'GO_BACK' then goto,goback1
;treatment_str = scrap
;names_tc_str = ' -class_names ' + strjoin(scrap,' ',/SINGLE)

dummy = 'Multiple selections are treated as a single event type.'
rtn_ic = identify_conditions_new(ne_max,factor_labels,ntc,treatment_str,0,0,dummy,/SKIP_CHECKS,/SET_DIAGONAL, $
    TOP_TITLE='Please select')
if rtn_ic.special eq 'GO_BACK' then goto,goback8
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
;print,'here0 names_tc_big=',names_tc_big

;names_tc_str = ' -tc_names ' + strjoin(names_tc_big,' ',/SINGLE)
;START130925
scrap = get_str(ntc,treatment_str,names_tc_big,WIDTH=100,TITLE='Please name classes',/GO_BACK)
if scrap[0] eq 'GO_BACK' then goto,goback9
treatment_str = scrap


index_tc = -1
for i=0,ntc-1 do begin
    scrap = rtn_ic.index_conditions[i,0:rtn_ic.sumrows[i]-1]
    if size(scrap,/N_DIMENSIONS) gt 0 then scrap = reform(scrap)
    index_tc = [index_tc,scrap]
endfor
index_tc = index_tc[1:*]
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
if count_lc_t4 gt 0 and glmfiles[0] eq '' then begin
    goback4a1:
    analtype = get_button(['Atlas space analysis','Data space analysis','Go back'], $
        TITLE='Do you wish to do an atlas space or a data space analysis?')
    if analtype eq 2 then goto,goback8
    if analtype eq 0 then begin
        if t4select[0] eq '' then begin
            goback4a:
            gf = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_711-2?_t4')
            if gf.msg eq 'GO_BACK' then goto,goback4a1 else if gf.msg eq 'EXIT' then return
            t4files = gf.files
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
m=0
mm=0
for m1=0,nsubject-1 do begin
    mm1=0
    for m2=0,nglm_per_subject[m1]-1 do begin
        sampling_str = ''
        offset_str = ''
        print,'NEXTNEXTNEXTNEXTNEXT ',concselect[m],' NEXTNEXTNEXTNEXTNEXT'
        tdim_sum = *stc2[m].tdim_sum
        nrun = stc2[m].n
        lcround_tc_index = intarr(sng[m].num_levels)
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
        endfor
        index_raw = index[1:*]
        uisi = uniq(index_raw,sort(index_raw))
        index = index_raw[uisi]
        boldconditions = *sng[m].conditions
        boldconditions = boldconditions[index]
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
            ;print,'sampling_str=',sampling_str
            ;print,'offset_str=',offset_str
        endif
        scrap = frames_ss(sng[m],nrun,tdim_sum,lcround_tc_index)
        boldframes = scrap.boldframes[index]
        nboldframes = n_elements(boldframes)
        trialframesall = scrap.trialframes
        trialframes = scrap.trialframes[index]
        trialframes_overlap = scrap.trialframes_overlap[index]
        conditions_max = scrap.conditions_max
        run = scrap.run
        tp_max = conditions_max[index_tc]
        tp_max_big = intarr(ntc)
        k = 0
        for i=0,ntc-1 do begin
            tp_max_big[i] = max(tp_max[k:k+counts[m,i]-1])
            k = k + rtn_ic.sumrows[i]
        endfor
        if m eq 0 then begin
            goback6:
            max_tp_max_big = max(tp_max_big)
            scraparr = strarr(max_tp_max_big+1,nc)
            scraplabel = strarr(nc)
            scrapstr = *sng[m].factor_labels
            k = 0
            l = 0
            for i=0,ntc-1 do begin
                for j=0,counts[m,i]-1 do begin
                    scraparr[0:tp_max[k],k] = [strtrim(indgen(tp_max[k])+1,2),'ALL']
                    k = k + 1
                endfor
                scraplabel[l:l+counts[m,i]-1] = scrapstr[sum_contrast_for_treatment[m,i,0:counts[m,i]-1]-1]
                l = l + counts[m,i]
            endfor
            widget_control,/HOURGLASS
            rtn = get_choice_list_many(scraplabel,scraparr,/LABEL_TOP,/NONEXCLUSIVE,/GO_BACK,LENGTHS=tp_max+1, $
                TITLE='Multiple timepoints are averaged.',TOP_TITLE='Please select timepoints.')
            if rtn.special eq 'GO_BACK' then goto,goback8
            scraplist = rtn.list
            for i=0,nc-1 do begin
                if scraplist[tp_max[i],i] eq 1 then begin 
                    scraplist[0:tp_max[i]-1,i] = 1
                    scraplist[tp_max[i],i] = 0
                endif
            endfor
            length_tc_big = make_array(nc,/INTEGER,VALUE=1)
            index_time = intarr(nc,1,max_tp_max_big)
            sumrows_time = intarr(nc,1)
            for i=0,nc-1 do begin
                idx = where(scraplist[*,i],cnt)
                index_time[i,0,0:cnt-1] = idx
                sumrows_time[i,0] = cnt
            endfor
            goback7:
            diameter_str = ''
            goback71:
            if glmfiles[0] eq '' then begin
                analysis_type = get_button(['searchlight','regional','go back','exit'],BASE_TITLE='Analysis type.',WIDTH=55, $
                    TITLE='Please select.')
                if analysis_type eq 3 then return else if analysis_type eq 2 then goto,goback6
            endif
            goback71a:
            if analysis_type eq 0 then begin
                if glmfiles[0] eq '' then begin
                    mask  = get_mask(glm_space[0],fi)
                    if mask eq 'GO_BACK' then goto,goback7
                    if mask ne '' then begin
                        compress_str = ' -compress ' + mask
                        mask_str = ' -mask ' + mask
                    endif
                endif
                goback72:
                scrap = get_str(1,'diameter in mm','12',WIDTH=40,TITLE='Searchlight sphere.',/ONE_COLUMN,/GO_BACK)
                if scrap[0] eq 'GO_BACK' then begin
                    if glmfiles[0] eq '' then goto,goback71 else goto,goback7
                endif
                diameter = strtrim(scrap[0],2)
                diameter_str = ' -diameter '+diameter
                diameterf = float(diameter)
                nvox=call_external(!SHARE_LIB,'_sphere_nvox',ifh2[0].matrix_size_1*ifh2[0].matrix_size_2*ifh2[0].matrix_size_3, $
                    ifh2[0].matrix_size_1,ifh2[0].matrix_size_2,ifh2[0].matrix_size_3,diameterf,ifh2[0].scale_1,VALUE=[1,1,1,1,0,0])
                print,'nvox=',nvox
                if nvox eq 0 then begin
                    stat=dialog_message("Error in _sphere_nvox.",/ERROR)
                    return
                endif
            endif else begin
                if glmfiles[0] eq '' then begin
                    rtn = get_regions(fi,wd,dsp,help)
                    if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then goto,goback7
                    region_names = rtn.region_names
                    region_files = rtn.region_file
                    nregion_files = rtn.nregfiles
                    region_space = rtn.space
                    if glm_space[0] ne region_space then begin
                        print,'glm_space[0]=',glm_space[0],' region_space=',region_space
                        scrap = get_button(['Try again','Exit'],BASE_TITLE='ERROR',WIDTH=200, $
                            TITLE='Region file and imgs are not in the same space.')
                        if scrap eq 0 then goto,goback71
                        return
                    endif
                    goback73:
                    rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.',/GO_BACK)
                    if rtn.files[0] eq 'GO_BACK' then goto,goback71
                    roi_str = ' -regions_of_interest ' + strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)
                endif
                diameter_str = ' $REGION_FILE $ROI'
            endelse
            goback74:

            ;START170220
            ;algstr=''

            alg = get_str(3,['LDA with regularization','Logistic regression with regularization','SVM cost'],[0.0,0.0,1.0], $
                TITLE='Algorithms',/GO_BACK,LABEL='Leave blank to not use the algorithm.'+string(10B) $
                +'Zero for no regularization. Minka uses 0.01 in his paper.'+string(10B)+'Cost = 1.0 is the default for SVM.')
            if alg[0] eq 'GO_BACK' then goto,goback74

            ;if alg[0] ne '' then algstr = algstr + ' -lda '+trim(alg[0])
            ;if alg[1] ne '' then algstr = algstr + ' -lr '+trim(alg[1])
            ;if alg[2] ne '' then algstr = algstr + ' -svmtrainc '+trim(alg[2])
            ;START170220
            idx=where(alg ne '',algcnt)
            algstr=strarr(algcnt)
            darby=[' -lda ',' -lr ',' -svmtrainc ']
            alg=trim(alg)
            for i=0,algcnt-1 do algstr[i]=darby[idx[i]]+alg[idx[i]]
            alglab=['lda','lr','svm']
            alglab=alglab[idx]


            goback75:
            scrap = 'mvpa script'

            ;scraplabels = 'fidl_mvpa.csh'
            ;START170224
            scraplabels = 'fidl_mvpa_'+timestr[0]+'.csh'

            if strmid(concselect[0],strlen(concselect[0])-3) eq 'img' then lc_concs = 0 else lc_concs = 1
            if lc_concs eq 1 then ext='.' else ext='.4dfp.img'


            rtn_gr = get_root(concselect,ext)
            wallace = rtn_gr.file
            flag=0
            for i=0,total_nsubjects-2 do begin
                idx=where(strmatch(wallace[i+1:*],wallace[i]) eq 1,cnt)
                if cnt ne 0 then begin
                    flag=1
                    goto,goback76
                endif
            endfor 
            goback76:
            ;if flag eq 1 then wallace = wallace + '_' + strtrim(indgen(total_nsubjects)+1,2)
            if flag eq 1 then wallace = wallace + '_' + trim(indgen(total_nsubjects)+1)
            ;START170222
            ;idx1=intarr(nsubject)
            ;for i=1,nsubject-1 do idx1[i]=idx1[i-1]+nglm_per_subject[i-1]
            ;rtn_gr=get_root(concselect[idx1],ext)
            ;wallace=rtn_gr.file
            ;flag=0
            ;for i=0,nsubject-2 do begin
            ;    idx=where(strmatch(wallace[i+1:*],wallace[i]) eq 1,cnt)
            ;    if cnt ne 0 then begin
            ;        flag=1
            ;        goto,goback76
            ;    endif
            ;endfor
            ;goback76:
            ;if flag eq 1 then wallace=wallace+'_'+trim(indgen(nsubject)+1)


            ;bf_str = compute_residuals_struct.scratchdir+wallace[*]+'_bf.dat' ;doesn't work on goya
            bf_str = strarr(n_elements(wallace))
            ;for i=0,n_elements(wallace)-1 do bf_str[i]=compute_residuals_struct.scratchdir+wallace[i]+'_bf.dat'
            ;START170224
            for i=0,n_elements(wallace)-1 do bf_str[i]=compute_residuals_struct.scratchdir+wallace[i]+'_bf'+timestr[0]+'.dat'

            ;print,'here0 bf_str'
            ;print,bf_str

            lizard=0
            if lctrial eq 1 or lcrun eq 1 or lcsession eq 1 then begin

                ;scrap = [scrap,'output root subject ' + strtrim(indgen(nsubject)+1,2)]
                ;idx = intarr(nsubject)
                ;for i=1,nsubject-1 do idx[i]=idx[i-1]+nglm_per_subject[i-1]
                ;scraplabels = [scraplabels,wallace[idx]]
                ;START170222
                ;scrap=[scrap,'output root subject '+trim(indgen(nsubject)+1)]
                ;scraplabels=[scraplabels,wallace]
                ;START170222
                scrap = [scrap,'output root subject '+trim(indgen(nsubject)+1)]
                idx=intarr(nsubject)
                for i=1,nsubject-1 do idx[i]=idx[i-1]+nglm_per_subject[i-1]
                scraplabels = [scraplabels,rtn_gr.file[idx]]

                lizard=nsubject
            endif
            if nsubject gt 1 then begin
                scrap = [scrap,'output root all subjects']
                scraplabels = [scraplabels,'allsub']
                lizard=lizard+1
            endif
            if n_elements(scrap) gt 2 then label="Leave blank any output you don't want." else label=''
            names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter names.',/ONE_COLUMN,/GO_BACK, $
                FRONT=directory+'/',/REPLACE,LABEL=label)
            if names[0] eq 'GO_BACK' then goto,goback74
            script = directory+'/'+fix_script_name(names[0])
            idx = where(names[1:*] ne '',cnt)
            if cnt eq 0 then return
            outstr = make_array(lizard,/STRING,VALUE='')
            outstr[idx] = ' -out '+names[1+idx]
            if analysis_type eq 0 then outstr[idx]=outstr[idx]+'.4dfp.img' else outstr[idx]=outstr[idx]+'.txt'
            bf_str_mvpa = make_array(nsubject,/STRING,VALUE='')
            j=1
            if lctrial eq 1 or lcrun eq 1 or lcsession eq 1 then begin
                idx = where(names[1:nsubject] ne '',cnt)

                ;bf_str_mvpa[idx] = names[1+idx]+'_bf.dat'
                ;START170224
                bf_str_mvpa[idx]=names[1+idx]+'_bf'+timestr[0]+'.dat'

                j=nsubject+1
            endif
            bf_str_mvpaall=''
            if nsubject gt 1 then begin

                ;if names[j] ne '' then bf_str_mvpaall = names[j]+'_bf.dat
                ;START170224
                if names[j] ne '' then bf_str_mvpaall=names[j]+'_bf'+timestr[0]+'.dat

            endif
            openw,lu,script,/GET_LUN
            top_of_script,lu
            if wfiles[0] ne '' then begin
                print_files_to_csh,lu,n_elements(wfiles),wfiles,'WEIGHT_FILES','weight_files'
                wfiles = ' $WEIGHT_FILES'
            endif
            if analysis_type eq 1 then begin
                print_files_to_csh,lu,n_elements(region_files),region_files,'REGION_FILE','region_file',/NO_NEWLINE
                printf,lu,'set ROI = ('+strtrim(roi_str[0],2)+')'
                printf,lu,''
            endif
            action = get_button(['execute','return','GO BACK'],TITLE='Please select action.',BASE_TITLE=script)
            if action eq 2 then begin
                close,lu
                free_lun,lu
                spawn,'rm -f '+script
                goto,goback75
            endif
        endif
        rtn_gi = get_index2(ntc,index_conditions,sumrows,sng[m],trialframesall,run)
        index_which_level = rtn_gi.index_which_level
        boldconditions = rtn_gi.boldconditions
        boldframes = rtn_gi.boldframes
        boldtf = rtn_gi.boldtf
        boldrun = rtn_gi.boldrun
        rtn_bf=print_boldframes(length_tc_big,sumrows_time,index_time,boldtf,boldframes,bf_str[m],index_which_level, $
            noboldconditions,boldrun,trialframes_in,dummy_filenames,dummmy_treatment_str,*stc2[m].tdim_sum_new,/LOGREG)
        scrap='+'
        if offset_str ne '' then begin
            printf,lu,offset_str
            printf,lu,''
            offset_str = ' $OFFSET'
        endif
        if glmfiles[0] ne '' then begin
            printf,lu,compute_residuals_struct.cmdstr[m]
            concstr = compute_residuals_struct.acconc[m]
            cleanupstr = ' -clean_up'
        endif else $
            concstr = concselect[m]


        ;cmd = '/fidl_bolds -tc_files "'+concstr+'"'+t4_str[m]+compress_str+atlas_str+normstr+scratchdir_str $
        ;    +' -frames "'+bf_str[m]+'"'
        ;START170217
        if m2 eq 0 then begin
            scratchdir0=strarr(algcnt)
            for i=0,algcnt-1 do begin

                ;scratchdir0[i]=fidl_scratchdir()            
                ;START170224
                scratchdir0[i]=fidl_scratchdir(/NOPATH)            

                if scratchdir0[i] eq 'ERROR' then return
            endfor
            ;170221
            ;scratchdir0=' -scratchdir '+strjoin(scratchdir0,' ',/SINGLE)
        endif
        cmd='/fidl_bolds -tc_files "'+concstr+'"'+t4_str[m]+compress_str+atlas_str+normstr+' -frames "'+bf_str[m]+'"' $
            +' -scratchdir '+strjoin(scratchdir0,' ',/SINGLE)


        dummy = !BINEXECUTE + cmd + ' -names_only '
        print,dummy
        widget_control,/HOURGLASS
        spawn,dummy,result
        ;print,result
        printf,lu,''

        ;START170224
        if m1 eq nsubject-1 and m2 eq nglm_per_subject[m1]-1 then cmd=cmd+' -cleanup '+compute_residuals_struct.scratchdir

;for m1=0,nsubject-1 do begin
;    mm1=0
;    for m2=0,nglm_per_subject[m1]-1 do begin


        printf,lu,'nice +19 $BIN' + cmd
        printf,lu,''
        scrap = strpos(result,'SCRATCH')
        for i=0,n_elements(result)-1 do result[i] = strmid(result[i],scrap[i])



        if bf_str_mvpa[m1] ne '' then begin

            ;rtn_bf=print_boldframes(length_tc_big,sumrows_time,index_time,boldtf,boldframes,bf_str_mvpa[m1],index_which_level, $
            ;    boldconditions[rtn_bf.index],boldrun[rtn_bf.index],trialframes_in,result,treatment_str,*stc2[m].tdim_sum_new, $
            ;    /LOGREG,MVPA=[m2+1,1,mm1],APPEND=m2)
            ;START170221
            for i=0,algcnt-1 do begin
                rtn_bf0=print_boldframes(length_tc_big,sumrows_time,index_time,boldtf,boldframes,alglab[i]+'_'+bf_str_mvpa[m1], $
                    index_which_level,boldconditions[rtn_bf.index],boldrun[rtn_bf.index],trialframes_in,result[i:*:algcnt], $
                    treatment_str,*stc2[m].tdim_sum_new,/LOGREG,MVPA=[m2+1,1,mm1],APPEND=m2)
            endfor

            mm1 = mm1 + nrun
        endif
        if bf_str_mvpaall ne '' then begin

            ;rtn_bf=print_boldframes(length_tc_big,sumrows_time,index_time,boldtf,boldframes,bf_str_mvpaall,index_which_level, $
            ;    boldconditions[rtn_bf.index],boldrun[rtn_bf.index],trialframes_in,result,treatment_str,*stc2[m].tdim_sum_new, $
            ;    /LOGREG,MVPA=[m+1,m1+1,mm],APPEND=m)
            ;START170221
            for i=0,algcnt-1 do begin
                rtn_bf0=print_boldframes(length_tc_big,sumrows_time,index_time,boldtf,boldframes,alglab[i]+'_'+bf_str_mvpaall, $
                    index_which_level,boldconditions[rtn_bf.index],boldrun[rtn_bf.index],trialframes_in,result[i:*:algcnt], $
                    treatment_str,*stc2[m].tdim_sum_new,/LOGREG,MVPA=[m+1,m1+1,mm],APPEND=m)
            endfor

            mm = mm + nrun
        endif
        if glmfiles[0] ne '' then begin
            rl = read_list(concstr)
            if rl.msg eq 'EXIT' then return else if rl.msg eq 'GOBACK' then goto,goback76

            ;START170202
            ;if rl.nglm gt 0 then files=rl.glm else files=rl.img 
            ;result = [files,concstr]
            ;scrap = strpos(result,'SCRATCH')
            ;for i=0,n_elements(result)-1 do printf,lu,'rm -f '+strmid(result[i],scrap[i])
            ;printf,lu,''

        endif
        printf,lu,'#event file '+sng[m].event_file
        printf,lu,''
        m=m+1
    endfor
    if bf_str_mvpa[m1] ne '' then begin

        ;str = 'nice +19 $BIN/fidl_mvpa4'+' -driver "'+bf_str_mvpa[m1]+'"'+mask_str+diameter_str+loostr+outstr[m1]+algstr
        ;if bf_str_mvpaall eq '' and m1 eq (nsubject-1) and glmfiles[0] ne '' then $
        ;    str = str + ' -cleanup '+compute_residuals_struct.scratchdir
        ;print,str
        ;printf,lu,str
        ;printf,lu,''
        ;START170221
        for i=0,algcnt-1 do begin
            str='nice +19 $BIN/fidl_mvpa4'+' -driver "'+alglab[i]+'_'+bf_str_mvpa[m1]+'"'+mask_str+diameter_str+loostr+outstr[m1] $
                +algstr[i]
            if bf_str_mvpaall eq '' and m1 eq (nsubject-1) and glmfiles[0] ne '' then str=str+' -cleanup '+scratchdir0[i]

            ;str=str+' >& '+scratchdir0[i]+alglab[i]+'.log & echo $! >& '+scratchdir0[i]+alglab[i]+'.pid'
            ;START170224
            str=str+' >& '+'fidl'+alglab[i]+timestr[0]+'.log & echo $! >& '+'fidl'+alglab[i]+timestr[0]+'.pid'

            print,str
            printf,lu,str
            printf,lu,''
        endfor
    endif
endfor
if bf_str_mvpaall ne '' then begin

    ;str = 'nice +19 $BIN/fidl_mvpa4'+' -driver "'+bf_str_mvpaall+'"'+mask_str+diameter_str+loostrall+outstr[n_elements(outstr)-1] $
    ;    +algstr
    ;if glmfiles[0] ne '' then str = str + ' -cleanup '+compute_residuals_struct.scratchdir
    ;print,str
    ;printf,lu,str
    ;printf,lu,''
    ;START170221
    for i=0,algcnt-1 do begin
        str='nice +19 $BIN/fidl_mvpa4'+' -driver "'+alglab[i]+'_'+bf_str_mvpaall+'"'+mask_str+diameter_str+loostrall $
            +outstr[n_elements(outstr)-1]+algstr[i]
        if glmfiles[0] ne '' then str=str+' -cleanup '+scratchdir0[i]

        ;str=str+' >& '+scratchdir0[i]+alglab[i]+'_all.log & echo $! >& '+scratchdir0[i]+alglab[i]+'_all.pid'
        ;START170224
        str=str+' >& '+'fidl'+alglab[i]+'all'+timestr[0]+'.log & echo $! >& '+'fidl'+alglab[i]+'all'+timestr[0]+'.pid'

        print,str
        printf,lu,str
        printf,lu,''
    endfor

endif
close,lu
free_lun,lu
spawn,'chmod +x '+script
widget_control,/HOURGLASS
if action eq 0 then begin
    scrap='>'+script+'.log'

    ;spawn,script+' &'
    ;START170224
    cmd=script+' &'
    print,cmd
    spawn,cmd

    stats = dialog_message(script+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
