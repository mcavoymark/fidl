;Copyright 10/17/02 Washington University.  All Rights Reserved.
;fidl_cc.pro  $Revision: 1.15 $

;**************************************
pro fidl_cc,fi,dsp,wd,glm,help,stc,pref
;**************************************
spawn,'whoami',whoami
;print,'whoami=',whoami
;print,'size(whoami)=',size(whoami)
if whoami[0] ne 'giovanni' and whoami[0] ne 'ayelet' and whoami[0] ne 'mcavoy' then begin
    stat=dialog_message('Procedure aborted due to error.'+string(10B)+'Null pointer to vector X.',/ERROR)
    return
endif 

directory = getenv('PWD')
region_str = ''
roi_str = ''
t4_str = ''
anova_output_str = ''
wfiles = ''
mask_str = ''
all_or_ROC = 0
cctypestr = ''
atlas_str = ''
mailstr = ''
lr_yesno = 1
abs_rel = 0

data_idx = get_bolds(fi,dsp,wd,glm,help,stc)
if data_idx lt 0 then return
files = *stc[data_idx].filnam
tdim_file = *stc[data_idx].tdim_file
nrun = stc[data_idx].n
tdim_sum = *stc[data_idx].tdim_sum
t_to_file = *stc[data_idx].t_to_file
tdim = stc[data_idx].tdim_all

goback1:
sng = define_single_trial(fi,pref,help)
if sng.event_file eq '' or sng.event_file eq 'NONE' then return

goback1a1:
within_between = get_button(['within trial','between trial','go back'],BASE_TITLE='Correlation', $
    TITLE='To correlate across trials select between.')
if within_between eq 2 then goto,goback1

goback1a:
analysis_type = get_button(['voxel by voxel','region with voxels','go back'],TITLE='Please select analysis type.')
if analysis_type eq 2 then goto,goback1a1
if analysis_type eq 1 then begin
    factor_names = ['regions','voxels']
    factor_str = 'regions and voxels'
    cctypestr = ' -regwithvox'
endif else begin
    goback2:
    if within_between eq 0 then begin
        factor_names = get_str(2,['Correlate','with'],['cue','target'],WIDTH=50,TITLE='Please enter names.',/GO_BACK)
        if factor_names[0] eq 'GO_BACK' then goto,goback1a
        factor_names = fix_script_name(factor_names,/SPACE_ONLY)
        factor_str = factor_names[0]+' and '+factor_names[1]
    endif
endelse

goback3a:
if within_between eq 0 then begin
    nlevels = 1
endif else begin
    nlevels = 2
    factor_str = ['previous trial','next trial']
    factor_names = factor_str
endelse
ntreatments = 2
treatment_str = factor_str
widget_control,/HOURGLASS
rtn_ic = identify_conditions_new(sng.num_levels,*sng.factor_labels,nlevels,factor_str,0,1, $
    'Multiple selections are permitted.',TOP_TITLE='Please select conditions for correlates.',/SKIP_CHECKS)
if rtn_ic.special eq 'GO_BACK' then begin
    if analysis_type eq 1 then goto,goback1a else goto,goback2
endif
sumrows = rtn_ic.sumrows
index_conditions = rtn_ic.index_conditions
print,'sumrows=',sumrows
print,'index_conditions=',index_conditions
print,'size(index_conditions)=',size(index_conditions)

factor_labels = *sng.factor_labels
condlab = strarr(nlevels)
for i=0,nlevels-1 do condlab[i] = strjoin(factor_labels[index_conditions[i,0:sumrows[i]-1]],'+',/SINGLE)
print,'condlab=',condlab

if within_between eq 0 then wbstr = condlab else wbstr = 'prev'+condlab[0]+'_next'+condlab[1] 

nc = total(sumrows)
rtn_gi = get_index(nlevels,nlevels,index_conditions,sumrows,nc,sng)
index = rtn_gi.index
boldframes = rtn_gi.boldframes
index_which_level = rtn_gi.index_which_level
boldconditions = rtn_gi.boldconditions
index_whichc = rtn_gi.index_whichc
ic = rtn_gi.ic
ic_by_levels = rtn_gi.ic_by_levels
nic_by_levels = rtn_gi.nic_by_levels
;print,'index=',index
;print,'boldframes=',boldframes
;print,'boldconditions=',boldconditions
rtn_frames_ss = frames_ss(sng,nrun,tdim_sum)
trialframes = rtn_frames_ss.trialframes[index]
conditions_max = rtn_frames_ss.conditions_max
print,'conditions_max=',conditions_max
tp_max = max(conditions_max[ic])
;print,'tp_max=',tp_max



goback99:
if within_between eq 1 then begin
    abs_rel = get_button(['absolute','relative','go back'],TITLE='Currently set up to correlate the target period of the preceeding ' $
        +'with the next trial.'+string(10B)+'Absolute - Fixed number of frames preceeding the next trial.'+string(10B)+'Relative - ' $
        +'Fixed number of frames from target.',BASE_TITLE='Offset for previous trial.')
    if abs_rel eq 2 then goto,goback3a
endif

goback100:
min_lengths_by_levels = intarr(nlevels)
max_lengths_by_levels = intarr(nlevels)
if abs_rel eq 0 then begin
    offset = intarr(sumrows[0])
    starti = 0
endif else begin
    scraparr = strarr(tp_max,sumrows[0])
    for i=0,sumrows[0]-1 do $
        scraparr[0:conditions_max[index_conditions[0,i]]-1,i] = strtrim(indgen(conditions_max[index_conditions[0,i]])+1,2)


    ;widget_control,/HOURGLASS
    ;scraplist = get_choice_list_many(factor_labels[index_conditions[0,0:sumrows[0]-1]],scraparr,TITLE='Target appeared on frame', $
    ;    /LABEL_TOP,/GO_BACK,LENGTHS=conditions_max[index_conditions[0,0:sumrows[0]-1]])
    ;START30
    widget_control,/HOURGLASS
    rtn = get_choice_list_many(factor_labels[index_conditions[0,0:sumrows[0]-1]],scraparr,TITLE='Target appeared on frame', $
        /LABEL_TOP,/GO_BACK,LENGTHS=conditions_max[index_conditions[0,0:sumrows[0]-1]])
    if rtn.special eq 'GO_BACK' then goto,goback100 
    scraplist = rtn.list


    offset = scraplist
    min_lengths_by_levels[0] = min(conditions_max[index_conditions[0,0:sumrows[0]-1]] - scraplist,MAX=scrap)
    max_lengths_by_levels[0] = scrap
    starti = 1
endelse
for i=starti,nlevels-1 do begin
    min_lengths_by_levels[i] = min(conditions_max[ic_by_levels[i,0:nic_by_levels[i]-1]],MAX=scrap)
    max_lengths_by_levels[i] = scrap
endfor
scraparr = strarr(tp_max,nlevels)
for i=0,nlevels-1 do scraparr[0:min_lengths_by_levels[i]-1,i] = strtrim(indgen(min_lengths_by_levels[i])+1,2)
if nlevels eq 1 then begin
    scraparr = reform(scraparr)
    scraparr = [[scraparr],[scraparr]]
    min_lengths_by_levels = reform(min_lengths_by_levels)
    min_lengths_by_levels = [min_lengths_by_levels,min_lengths_by_levels]
    max_lengths_by_levels = reform(max_lengths_by_levels)
    max_lengths_by_levels = [max_lengths_by_levels,max_lengths_by_levels]
endif



;widget_control,/HOURGLASS
;scraplist = get_choice_list_many(factor_names,scraparr,TITLE='How many timepoints?',/LABEL_TOP,/GO_BACK, $
;    LENGTHS=min_lengths_by_levels)
;if size(scraplist,/N_DIMENSIONS) ne 2 then $
;    result = scraplist[0] $
;else $
;    result = scraplist[0,0]
;if result eq -1 then return
;START30
widget_control,/HOURGLASS
rtn = get_choice_list_many(factor_names,scraparr,TITLE='How many timepoints?',/LABEL_TOP,/GO_BACK, $
    LENGTHS=min_lengths_by_levels)
if rtn.special eq 'GO_BACK' then begin
    if abs_rel eq 0 then goto,goback99 else goto,goback100
endif
scraplist = rtn.list


nframes = scraplist + 1
print,'nframes=',nframes
print,'offset=',offset

sumrows_time = intarr(ntreatments,max(nframes))
index_time = intarr(ntreatments,max(nframes),max(min_lengths_by_levels))
title = 'Please select a frame for each time point. Multiple selections are summed.'
for i=0,ntreatments-1 do begin
    scrap = indgen(min_lengths_by_levels[i])+1


    ;if abs_rel eq 1 and i eq 0 then dummy = title + string(10B) + 'Target appears on timepoint 1.' else dummy = title

    ;if abs_rel eq 1 and i eq 0 then begin
    ;    dummy = title + string(10B) + 'Target appears on timepoint 1.' 
    ;    scrap = scrap + offset[i]
    ;endif else $
    ;    dummy = title
    
    if abs_rel eq 1 and i eq 0 then offset1 = offset[i] else offset1 = 0
       

    widget_control,/HOURGLASS
    rtn_ic = identify_conditions_new(min_lengths_by_levels[i],string(scrap+offset1),nframes[i],'timepoint '+strtrim(scrap,2),0,0, $
        title,offset1,TOP_TITLE=factor_names[i],/TIME_FRAMES,/SET_DIAGONAL)
    if rtn_ic.special eq 'GO_BACK' then goto,goback100

    index_time[i,0:nframes[i]-1,0:min_lengths_by_levels[i]-1] = rtn_ic.index_conditions
    ;HERE0
    ;goose = rtn_ic.index_conditions
    ;if abs_rel eq 1 and i eq 0 then goose = goose + offset[i] 
    ;index_time[i,0:nframes[i]-1,0:min_lengths_by_levels[i]-1] = goose 

    print,'rtn_ic.index_conditions=',rtn_ic.index_conditions
    ;if abs_rel eq 1 and i eq 0 then $
    ;    index_time[i,0:nframes[i]-1,0:min_lengths_by_levels[i]-1] = rtn_ic.index_conditions + offset[i] $
    ;else $
    ;    index_time[i,0:nframes[i]-1,0:min_lengths_by_levels[i]-1] = rtn_ic.index_conditions



    sumrows_time[i,0:nframes[i]-1] = rtn_ic.sumrows
endfor
print,'index_time=',index_time
print,'sumrows_time=',sumrows_time

names_tc_str = ' -tc_names "'+factor_names[0]+'" "'+factor_names[1]+'"'
;print,'names_tc_str=',names_tc_str
cstr = ' -tc_frames'
scrap = index_time + 1
for i=0,ntreatments-1 do begin
    spacechar = make_array(nframes[i],/STRING,VALUE=',')
    spacechar[0] = ' '
    for j=0,nframes[i]-1 do begin
        ;get_cstr_new,cstr,1,effect_length,scrap[i,j,*],sumrows_time[i,j],sum_contrast_for_treatment,spacechar[j]
        get_cstr_new,cstr,1,effect_length,scrap[i,j,*],sumrows_time[i,j],spacechar[j]
    endfor
endfor
index_tc_str = cstr
print,'index_tc_str=',index_tc_str

goback4a:
lc_t4 = !FALSE
space = fi.space[data_idx]
nreg = 0
if analysis_type eq 1 then begin
    goback = 0
    goback4a1:
    rtn = analysis_type1(fi,dsp,wd,help,space,goback)
    if rtn.goback eq -1 then return else if rtn.goback eq 1 then goto,goback4a
    nreg = rtn.nreg
    nreg = nreg[0]
    region_names = rtn.region_names
    region_str = rtn.region_str
    roi_str = rtn.roi_str
    lc_t4 = rtn.lc_t4
    wfiles = rtn.wfiles
endif
if space ne !SPACE_111 and space ne !SPACE_222 and space ne !SPACE_333 then begin
    ;select_space,space,atlas,atlas_str
    rtn=select_space()
    space=rtn.space
    atlas=rtn.atlas
    atlas_str=rtn.atlas_str

    lc_t4 = !TRUE
endif
if lc_t4 eq !TRUE then begin
    goback4b:
    get_dialog_pickfile,'*_anat_ave_to_711-2?_t4',directory,'Please select transformation matrix.',t4_file
    if t4_file eq 'GOBACK' then goto,goback4a else if t4_file eq 'EXIT' then return
    t4_str = ' -xform_file ' + t4_file
endif
mask_str = ' -mask ' + get_mask(space,fi)
if analysis_type eq 1 then begin
    lr_yesno = get_button(['yes','no','go back'],TITLE='Apply logreg weights to regions?')
    if lr_yesno eq 2 then begin
        if lr_yesno eq !TRUE then $
            goto,goback4b $
        else begin
            goback = 1
            goto,goback4a1
        endelse
    endif
endif

goback5:
dummy = 3 + ((nreg-1)>0)
if lr_yesno eq 0 then dummy = dummy+2
print,'dummy=',dummy
scrap = strarr(dummy)
scraplabels = strarr(dummy)
scrap[0] = 'script'
if lr_yesno eq 0 then scraplabels[0] = 'fidl_lrcc.csh' else scraplabels[0] = 'fidl_cc.csh'
scrap[1] = 'cancorn bold frames file'
scraplabels[1] = 'fidl_bfcc.dat'
j = 2
if lr_yesno eq 0 then begin
    scrap[j] = 'logreg bold frames file'
    scraplabels[j] = 'fidl_bflr.dat'
    j = j + 1
    scrap[j] = 'logreg output file'
    scraplabels[j] = 'fidl_lr.txt'
    j = j + 1
endif




;if analysis_type eq 1 then begin
;    print,'j=',j,' nreg=',nreg
;    for i=0,nreg-1 do begin
;        scrap[j] = 'output root for ' + region_names[i]
;        scraplabels[j] =  condlab + '_' + region_names[i]
;        if lr_yesno eq 0 then scraplabels[j] = scraplabels[j] + '_lrw'
;        j = j + 1
;    endfor
;endif else begin
;    scrap[j] = 'output root'
;    if within_between eq 0 then $
;        scraplabels[j] = factor_names[0] + '_and_' + factor_names[1] + '_' + condlab $
;    else $
;        scraplabels[j] = 'prev'+condlab[0]+'_next'+condlab[1] 
;endelse

;START0
if analysis_type eq 1 then begin
    for i=0,nreg-1 do begin
        scrap[j] = 'output root for ' + region_names[i]
        scraplabels[j] =  wbstr + '_' + region_names[i]
        if lr_yesno eq 0 then scraplabels[j] = scraplabels[j] + '_lrw'
        j = j + 1
    endfor
endif else begin
    scrap[j] = 'output root'
    if within_between eq 0 then $
        scraplabels[j] = factor_names[0] + '_and_' + factor_names[1] + '_' + condlab $
    else $
        scraplabels[j] = wbstr 
endelse





names = get_str(dummy,scrap,scraplabels,WIDTH=50,TITLE='Please enter names.',/GO_BACK,FRONT=directory)
if names[0] eq 'GO_BACK' then goto,goback4a
csh = fix_script_name(names[0])
bf_str = directory+'/'+names[1]
j = 2
if lr_yesno eq 0 then begin
    lr_bf_str = directory+'/'+names[j]
    j = j + 1
    lr_out_str = names[j]
    cctypestr = cctypestr+' "'+directory+'/'+lr_out_str+'"'
    j = j + 1
endif
for i=j,dummy-1 do names[i] = fix_script_name(names[i])
rootstr = ' -root ' + strjoin(names[j:*],' ',/SINGLE)

widget_control,/HOURGLASS
if within_between eq 0 then $
    rtn_bf=print_boldframes(nframes,sumrows_time,index_time,trialframes,boldframes,bf_str,index_which_level,/CC) $
else begin

    try_one = 0
    if try_one eq 1 then begin
        ;print,'trialframes=',trialframes
        previ = where(index_which_level eq 0,nprevi)
        ;print,'previ=',previ
        nexti = where(index_which_level[previ+1] eq 1,nnexti)
        ;print,'nexti=',nexti
        previ = previ[nexti]
        ;print,'previ[nexti]=',previ
        prevbf = boldframes[previ]
        prevtf = trialframes[previ]
        ;print,'prevbf=',prevbf
        ;print,'prevtf=',prevtf
        nextbf = prevbf + prevtf
        subtract = nextbf - boldframes[previ+1] 
        ;print,'subtract=',subtract
        prevseqnexti = where(subtract eq 0,nprevseqnexti)
        if nprevseqnexti eq 0 then begin
            stat=dialog_message(condlab[0]+' is never followed by '+condlab[1],/ERROR)
            return
        endif
        previ = previ[prevseqnexti]
        ;print,'previ[prevseqnexti]=',previ
        ;print,'t_to_file[boldframes[previ]]=',t_to_file[boldframes[previ]]
        ;print,'t_to_file[boldframes[previ+1]]=',t_to_file[boldframes[previ+1]]
        subtract = t_to_file[boldframes[previ]] - t_to_file[boldframes[previ+1]]
        ;print,'subtract=',subtract
        sameruni = where((t_to_file[boldframes[previ]]-t_to_file[boldframes[previ+1]]) eq 0,nsameruni)
        ;print,'sameruni=',sameruni
        if nsameruni eq 0 then begin
            stat=dialog_message(condlab[0]+' followed by '+condlab[1]+' never appears in the same run.',/ERROR)
            return
        endif
        previ = previ[sameruni]
        ;print,'previ[sameruni]=',previ
    
        previ = [previ,previ+1]
        ;print,'[previ,previ+1]=',previ
        previ = previ[sort(previ)]
        print,'previ[sort(previ)]=',previ
        
        ;boldframes = boldframes[previ]
        ;trialframes = trialframes[previ]
        ;index_which_level = index_which_level[previ]
        ;print,'boldframes[previ]=',boldframes
        ;print,'trialframes[previ]=',trialframes
        ;print,'index_which_level[previ]=',index_which_level
    
        ;rtn_bf=print_boldframes(nframes,sumrows_time,index_time,trialframes,boldframes,bf_str,index_which_level,/CC,/PAIRS)
    endif


    previ = where(index_which_level eq 0,nprevi)
    print,'previ=',previ
    index_previ = index[previ]
    print,'index_previ=',index_previ 
    nexti = where(index_which_level eq 1,nnexti)
    print,'nexti=',nexti
    index_nexti = index[nexti]
    print,'index_nexti=',index_nexti 
    index_previp1 = index_previ+1
    print,'index_previp1=',index_previp1
    wprevi = -1
    wnexti = -1
    for i=0,nprevi-1 do begin
        scrap = where(index_nexti eq index_previp1[i],nscrap)
        if nscrap ne 0 then begin
            wprevi = [wprevi,i]
            wnexti = [wnexti,scrap]
        endif 
    endfor
    wprevi = wprevi[1:*]
    wnexti = wnexti[1:*]
    print,'wprevi=',wprevi
    print,'wnexti=',wnexti
    print,'index_previ[wprevi]=',index_previ[wprevi]
    print,'index_nexti[wnexti]=',index_nexti[wnexti]
    print,'index[previ[wprevi]]=',index[previ[wprevi]]
    print,'index[nexti[wnexti]]=',index[nexti[wnexti]]

    print,'t_to_file[boldframes[previ[wprevi]]]=',t_to_file[boldframes[previ[wprevi]]]
    print,'t_to_file[boldframes[nexti[wnexti]]]=',t_to_file[boldframes[nexti[wnexti]]]
    subtract = t_to_file[boldframes[previ[wprevi]]] - t_to_file[boldframes[nexti[wnexti]]]
    print,'subtract=',subtract
    sameruni = where(subtract eq 0,nsameruni)
    print,'sameruni=',sameruni
    if nsameruni eq 0 then begin
        stat=dialog_message(condlab[0]+' followed by '+condlab[1]+' never appears in the same run.',/ERROR)
        return
    endif

    print,'previ[wprevi[sameruni]]=',previ[wprevi[sameruni]]
    print,'nexti[wnexti[sameruni]]=',nexti[wnexti[sameruni]]
    print,'index[previ[wprevi[sameruni]]]=',index[previ[wprevi[sameruni]]]
    print,'index[nexti[wnexti[sameruni]]]=',index[nexti[wnexti[sameruni]]]

    indices = [previ[wprevi[sameruni]],nexti[wnexti[sameruni]]] 
    indices = indices[sort(indices)]
    print,'indices=',indices
    print,'index[indices]=',index[indices]

    trialframes = trialframes[indices]
    boldframes = boldframes[indices]
    index_which_level = index_which_level[indices]
    rtn_bf=print_boldframes(nframes,sumrows_time,index_time,trialframes,boldframes,bf_str,index_which_level,/CC,/PAIRS)
endelse








bf_str = ' -frames "' + bf_str + '"'

;if scrap eq 0 then fidl_logreg_ss,fi,dsp,wd,glm,help,stc,pref,data_idx,sng,nframes[0],region_str,roi_str,t4_str,wfiles,/CC
if lr_yesno eq 0 then begin
    ;if dialog_message('Entering logreg.'+string(10B)+string(10B)+'Cancel to proceed with a regular canonical correlation ' $
    ;    +'analysis.',/CANCEL,/INFORMATION) eq 'Cancel' then $
    ;    lr_yesno = 1 $
    ;else begin 
    ;    cc = {data_idx:data_idx,sng:sng,nframes:nframes[0],region_str:region_str,roi_str:roi_str,t4_str:t4_str,wfiles:wfiles, $
    ;        bf_str:lr_bf_str,out_str:lr_out_str,csh:csh}
    ;    ;fidl_logreg_ss,fi,dsp,wd,glm,help,stc,pref,cc
    ;    lu_csh = fidl_logreg_ss(fi,dsp,wd,glm,help,stc,pref,cc)
    ;endelse

    scrap=get_button(['ok','cancel - proceed with regular canonical correlation','go back'],BASE_TITLE='Please select.', $
        TITLE='Entering logreg.')
    if scrap eq 2 then goto,goback5
    if scrap eq 0 then begin
        cc = {data_idx:data_idx,sng:sng,nframes:nframes[0],region_str:region_str,roi_str:roi_str,t4_str:t4_str,wfiles:wfiles, $
            bf_str:lr_bf_str,out_str:lr_out_str,csh:csh}
        lu_csh = fidl_logreg_ss(fi,dsp,wd,glm,help,stc,pref,cc)
    endif else $
        lr_yesno = 1
endif





;action = get_button(['execute','return','GO BACK'],TITLE='Please select action.'+string(10B)+string(10B)+'To run on linux, ' $
;    +'please select return. Login to a linux machine and enter the script on the command line.',BASE_TITLE=csh)
;if action eq 2 then goto,goback5
;if action eq 0 then mailstr = ' |& mail `whoami`' else mailstr = ''




;csh = directory+'/'+csh
;openw,lu_csh,csh,/GET_LUN
;top_of_script,lu_csh
;print_files_to_csh,lu_csh,nrun,files,'TC_FILES','tc_files'
;if wfiles[0] ne '' then begin
;    print_files_to_csh,lu_csh,n_elements(wfiles),wfiles,'WEIGHT_FILES','weight_files'
;    wfiles = ' $WEIGHT_FILES'
;endif

if lr_yesno eq 1 then begin
    ;csh = directory+'/'+csh
    openw,lu_csh,csh,/GET_LUN
    top_of_script,lu_csh
    print_files_to_csh,lu_csh,nrun,files,'TC_FILES','tc_files'
    if wfiles[0] ne '' then begin
        print_files_to_csh,lu_csh,n_elements(wfiles),wfiles,'WEIGHT_FILES','weight_files'
        wfiles = ' $WEIGHT_FILES'
    endif
endif else $
    printf,lu_csh,''







printf,lu_csh,'nice +19 $BIN/fidl_cc $TC_FILES '+wfiles+names_tc_str+index_tc_str $
    +region_str+roi_str+t4_str+anova_output_str+mask_str+bf_str+rootstr+cctypestr+atlas_str $
    +' -event_file "'+sng.event_file+'" -conc_file "'+fi.names[data_idx]+'"'+' -clean_up'+mailstr
close,lu_csh
free_lun,lu_csh
spawn,'chmod +x '+csh

;widget_control,/HOURGLASS
;if action eq 0 then begin
;    spawn,fidl_anova_csh+' &'
;    stats = dialog_message(fidl_anova_csh+string(10B)+string(10B)+'CC script submitted as batch job.'+string(10B) $
;        +'The log file(s) will be e-mailed to you upon completion.',/INFORMATION)
;endif





action = get_button(['execute','return','GO BACK'],TITLE='Please select action.'+string(10B)+string(10B)+'To run on linux, ' $
    +'please select return. Login to a linux machine and enter the script on the command line.',BASE_TITLE=csh)
if action eq 2 then goto,goback5
if action eq 0 then begin
    spawn,csh+' > '+csh+'.log &'
    stat=dialog_message(csh+string(10B)+string(10B)+'Script submitted as batch job.',/INFORMATION)
endif


print,'DONE'
end
