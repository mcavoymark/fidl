;Copyright 10/28/02 Washington University.  All Rights Reserved.
;fidl_anova_ss.pro  $Revision: 1.53 $
pro fidl_anova_ss,fi,dsp,wd,glm,help,stc,pref
if dialog_message('This is for a timecourse analysis of widely spaced data.',/INFORMATION,/CANCEL) eq 'Cancel' then return
get_directory,directory,offset_spawn
region_str = ''
roi_str = ''
t4_str = ''
mask_str = ''
region_or_uncompress_str = ''
threshold_extent_str = ''
time = 0
lc_t4 = !FALSE
chunks = 1
chunks_str = ''
glm_file = ''
fwhm_past = 0.
gauss_str = ''
mailstr = ''
volstr = ''

data_idx = get_bolds(fi,dsp,wd,glm,help,stc)
if data_idx lt 0 then return
nrun = stc[data_idx].n
files = *stc[data_idx].filnam 
tdim_file = *stc[data_idx].tdim_file
tdim_sum = *stc[data_idx].tdim_sum
t_to_file = *stc[data_idx].t_to_file
tdim = stc[data_idx].tdim_all
sng = define_single_trial(fi,pref,help)

;glm_list_str = ' -glm_list_file "' + fi.names[data_idx] + '"' ;Tells me the conc file used in the script.
;START4
glm_list_str = ' -glm_list_file "'+fi.names[data_idx]+','+sng.event_file+'"'

goback2: 
rtn_within = get_factor_levels(help,time,0,rtn_within)
if rtn_within.nfactors eq -1 then return 
rtn_within = get_treatment_str(rtn_within)

if rtn_within.nfactors eq 0 then begin
    dummy = 'Please select a condition for the simple main effect of time.'
endif else begin
    dummy = 'Please select a condition for each treatment.'
endelse

widget_control,/HOURGLASS
rtn_ic = identify_conditions_new(sng.num_levels,*sng.factor_labels,rtn_within.treatments[0],rtn_within.treatment_str,0, $
    rtn_within.nfactors,'Multiple selections are permitted.',TOP_TITLE=dummy)
if rtn_ic.special eq 'GO_BACK' then goto,goback2
sumrows = rtn_ic.sumrows
index_conditions = rtn_ic.index_conditions

index = -1
ic = -1
scrapcount = intarr(rtn_within.treatments[0])
for i=0,rtn_within.treatments[0]-1 do begin
    for j=0,sumrows[i]-1 do begin

        ;scrap = where(sng.conditions eq index_conditions[i,j],count)
        ;HERE
        scrap = where(*sng.conditions eq index_conditions[i,j],count)

        index = [index,scrap]
        scrapcount[i] = scrapcount[i] + count
        ic = [ic,index_conditions[i,j]]
    endfor
endfor
index = index[1:*]
ic = ic[1:*]
driver_levels_index = intarr(total(scrapcount))
;print,'total(scrapcount)=',total(scrapcount)
j = 0
for i=0,rtn_within.treatments[0]-1 do begin
    driver_levels_index[j:j+scrapcount[i]-1] = i
    j = j + scrapcount[i]
endfor
sort_index = sort(index)
driver_levels_index = driver_levels_index[sort_index]
index = index[sort_index]

;boldframes = sng.frames[index]
;HERE
boldframes = *sng.frames
boldframes = boldframes[index]
times = *sng.times

scrap = sort(boldframes)-indgen(n_elements(boldframes))
if total(abs(scrap)) ne 0 then begin
    scrapindex = where(scrap ne 0,scrapcount)

    ;scrap = sng.times[index]
    ;HERE
    scrap = times[index]

    scrap = scrap[scrapindex]
    spawn,'whoami',whoami
    stat=dialog_message('Hey '+whoami+'.'+string(10B)+string(10B)+'It appears the times in your event file are a bit out of ' $
        +'order.'+string(10B)+'Please check from '+strtrim(scrap[0],2)+ ' s.',/ERROR)
    return
endif


;frames_ss,sng,nrun,tdim_file,tdim_sum,trialframes,conditions_max
;trialframes = trialframes[index]
;tp_max = max(conditions_max[ic])

scrap = frames_ss(sng,nrun,tdim_sum)
trialframes = scrap.trialframes[index]
tp_max = max(scrap.conditions_max[ic])




goback4:
nframes = get_button([string(indgen(tp_max)+1),'GO BACK'],TITLE='Time is a factor at how many levels?') + 1
if nframes eq tp_max+1 then goto,goback2
if nframes eq 1 then scrap = 0 else scrap = 1
widget_control,/HOURGLASS
rtn_ic = identify_conditions_new(tp_max,string(indgen(tp_max)+1),nframes,'time point '+strtrim(indgen(nframes)+1,2),time, $
    scrap,'Please select a time point for each level of time. Multiple selections are summed.',/TIME_FRAMES,SET_BUTTONS=nframes)
if rtn_ic.special eq 'GO_BACK' then goto,goback4
sumrows_time = rtn_ic.sumrows
conditions_time = rtn_ic.index_conditions + 1
index_conditions_time = rtn_ic.index_conditions

print,'sumrows_time=',sumrows_time

goback4a:
lc_t4 = !FALSE
space = fi.space[data_idx]
analysis_type = get_button(['voxel by voxel','specfic regions'],TITLE='Please select analysis type.')
if analysis_type eq 1 then begin
    rtn = get_regions(fi,wd,dsp,help)


    ;if rtn.goback eq !TRUE then $
    ;    goto,goback4a $
    ;else if rtn.error_flag eq !TRUE then $
    ;    return $
    ;else begin
    ;    region_names = rtn.region_names
    ;    region_str = rtn.region_str
    ;    region_space = rtn.space
    ;endelse

    if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then goto,goback4a
    region_names = rtn.region_names
    region_str = rtn.region_str
    region_space = rtn.space



    rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.')
    scrap = strtrim(rtn.index+1,2)
    roi_str = ' -regions_of_interest'
    for i=0,rtn.count-1 do roi_str = roi_str + ' ' + scrap[i]

    region_or_uncompress_str = ' -regions '
    for i=0,rtn.count-1 do region_or_uncompress_str = region_or_uncompress_str + string(34B) + rtn.files[i] + string(34B) + ' '

    if space ne region_space then begin
        if space eq !SPACE_DATA and region_space eq !SPACE_222 then begin
            space = !SPACE_222
            lc_t4 = !TRUE
        endif else begin
            stat=dialog_message('Space incompatibility! Abort! Abort! Abort!',/ERROR)
            return
        endelse
    endif
endif else begin
    if space eq !SPACE_111 then begin
        mask_str = ' -compress ' + !MASK_FILE_111
        region_or_uncompress_str = ' -uncompress ' + !MASK_FILE_111
    endif else if space eq !SPACE_222 then begin
        mask_str = ' -compress ' + !MASK_FILE_222
        region_or_uncompress_str = ' -uncompress ' + !MASK_FILE_222
    endif else if space eq !SPACE_333 then begin
        mask_str = ' -compress ' + !MASK_FILE_333
        region_or_uncompress_str = ' -uncompress ' + !MASK_FILE_333
    endif else begin
        lc_t4 = !TRUE
        space = !SPACE_222
        mask_str = ' -compress ' + !MASK_FILE_222
        region_or_uncompress_str = ' -uncompress ' + !MASK_FILE_222
    endelse
endelse    
if lc_t4 eq 1 then begin
    get_dialog_pickfile,'*_anat_ave_to_711-2?_t4',directory,'Please select t4. Enter NONE if none.',t4_file
    if t4_file eq 'GOBACK' then $
        goto,goback4 $
    else if t4_file eq 'EXIT' then  $
        return $
    else if t4_file ne 'NONE' then $
        t4_str = ' -xform_file ' + t4_file $
    else begin 
        lc_t4 = !FALSE
        space = fi.space[data_idx]
        mask_str = ''
        region_or_uncompress_str = ''
    endelse
endif
goback16:
if analysis_type eq 0 then begin
    out = ['Uncorrected F-maps','Uncorrected Z-maps']
    if space eq !SPACE_111 or space eq !SPACE_222 or space eq !SPACE_333 then out = [out,'Monte Carlo Corrected Z-maps']
    rtn = get_bool_list(out,TITLE='Please select output options.')
    out = rtn.list
    if space ne !SPACE_111 and space ne !SPACE_222 and space ne !SPACE_333 then out = [out,0]
    out_str = ' -output'
    if out[0] eq 1 then out_str = out_str + ' F_uncorrected'
    if out[1] eq 1 then out_str = out_str + ' Z_uncorrected'
    if out[2] eq 1 then begin
        out_str = out_str + ' Z_monte_carlo'
        scrap = get_str(1,'FWHM in voxels ','0',TITLE='How much has your data been smoothed?', $
            LABEL='Enter 0 if unsmoothed.',WIDTH=50)
        fwhm_past = double(scrap[0])
    endif
    if fwhm_past eq 0. then begin
        title = 'You may smooth your data if you wish.'
        dummy = '2'
        scrap2 = ''
    endif else begin
        title = 'You may do some additional smoothing if you wish.'
        dummy = '0'
        scrap2 = ' additional'
    endelse
    scrap = get_str(1,'FWHM in voxels ',dummy,/GO_BACK,TITLE=title,LABEL='Enter 0 for no'+scrap2+' smoothing.',WIDTH=50)
    if scrap[0] eq 'GO_BACK' then  $
        goto,goback16 $
    else if float(scrap[0]) gt 0.01 then $
        gauss_str = ' -gauss_smoth '+ strtrim(scrap[0],2)
    if out[2] eq 1 then begin
        rtn = threshold_extent(!F_STAT,round(sqrt(fwhm_past^2+(double(scrap[0]))^2)),space)

        ;threshold = *rtn.threshold
        ;extent = *rtn.extent
        ;threshold_extent_str = ' -threshold_extent'
        ;threshold = strtrim(threshold,2)
        ;extent = strtrim(extent,2)
        ;for i=0,n_elements(threshold)-1 do threshold_extent_str = threshold_extent_str + ' "'+ threshold[i] +' '+ extent[i]+'"'
        threshold_extent_str = rtn.threshold_extent_str
    endif
endif

goback5:
dummy = 3 + analysis_type
scrap = strarr(dummy)
scraplabels = strarr(dummy)
scrap[0] = 'driving file'
scrap[1] = 'bold frames file'
scrap[2] = 'anova program script'
scraplabels[0] = 'driver.dat'
scraplabels[1] = 'boldframes.dat'
scraplabels[2] = 'fidl_anova_ss.csh'
if analysis_type eq 1 then begin
    scrap[3] = 'anova output'
    scraplabels[3] = 'fidl_anova_ss.txt'
endif
names = get_str(dummy,scrap,scraplabels,WIDTH=50,TITLE='Please enter desired filenames.',/GO_BACK)
if names[0] eq 'GO_BACK' then goto,goback4
driver = names[0]
bf_str = directory+'/'+names[1]
fidl_anova_csh = fix_script_name(names[2])
if analysis_type eq 1 then out_str = ' -regional_anova_name "' + names[3] + '"'
print,'nframes=',nframes
print,'sumrows_time=',sumrows_time



;print_boldframes,nframes,sumrows_time,index_conditions_time,trialframes,boldframes,bf_str,rindex,rcount,rindex_which_level
rtn_bf=print_boldframes(nframes,sumrows_time,index_conditions_time,trialframes,boldframes,bf_str,rindex_which_level)
rindex = rtn_bf.index
rcount = rtn_bf.count




driver_levels_index = driver_levels_index[rindex]
;print,'trialframes=',trialframes
;print,'conditions_time=',conditions_time
bf_str = ' -frames "' + bf_str + '"'
tnof = total(trialframes)
;print,'total number of files = ',tnof
print,'tnof = ',tnof




;if tnof gt 1000 and (space eq !SPACE_222 or lc_t4 eq !TRUE or space eq !SPACE_333) and analysis_type eq 0 then begin
;    print,'Compressed files will be in chunks.'
;    chunks = 5
;    chunks_str = ' -chunks'
;endif

;START0
if tnof gt 1000 and analysis_type eq 0 then begin
    print,'Compressed files will be in chunks.'
    chunks = 5
    chunks_str = ' -chunks'
    if space eq !SPACE_222 or lc_t4 eq !TRUE or space eq !SPACE_333 then begin 
        ;do nothing 
    endif else begin
        ifh = read_mri_ifh(files[0])
        vol = ifh.matrix_size_1*ifh.matrix_size_2*ifh.matrix_size_3
        print,'vol=',vol
        chunks_str = chunks_str + ' ' + strtrim(ceil(float(vol)/5.),2)
        ;volstr = ' -vol '+strtrim(vol,2)
        volstr = ' -dimxyz '+strtrim(ifh.matrix_size_1,2)+' '+strtrim(ifh.matrix_size_2,2)+' '+strtrim(ifh.matrix_size_3,2)
    endelse
endif






fidl_bolds_str = '/fidl_bolds $TC_FILES'+region_str+roi_str+t4_str+gauss_str+mask_str+bf_str+chunks_str
fidl_bolds_csh = directory+'/fidl_bolds.csh'
openw,lu_csh,fidl_bolds_csh,/GET_LUN
printf,lu_csh,FORMAT='("#!/bin/csh")'
printf,lu_csh,FORMAT='("unlimit")'
printf,lu_csh,''
;print_files_to_csh,lu_csh,num_files,files,'TC_FILES','tc_files'
print_files_to_csh,lu_csh,nrun,files,'TC_FILES','tc_files'
printf,lu_csh,''
printf,lu_csh,!BIN+fidl_bolds_str+' -names_only'
close,lu_csh
free_lun,lu_csh
spawn,'chmod +x '+fidl_bolds_csh
widget_control,/HOURGLASS
spawn,fidl_bolds_csh,result
nfiles = n_elements(result) - offset_spawn
result = result[offset_spawn:nfiles+offset_spawn-1]
print,'nfiles=',nfiles,' tnof*chunks=',tnof*chunks


if nfiles ne tnof*chunks then begin
;if nfiles ne tnof*chunks+1 then begin
    stat = dialog_message('Error occurred while extracting timecourse names.',/ERROR)
    stat = spawn_check_and_load(result,fi,wd,dsp,help,nfiles_loaded,files_loaded,stc)
    return
endif



spawn,'rm '+fidl_bolds_csh
framestr = strarr(1,nframes)
for n=0,0 do begin
    for m=0,nframes-1 do begin
        framestr[n,m] = strcompress(conditions_time[m,0],/REMOVE_ALL)
        for r=1,sumrows_time[m]-1 do $
            framestr[n,m] = string(framestr[n,m],strcompress(conditions_time[m,r],/REMOVE_ALL),FORMAT='(a,"+",a)')
    endfor
    scrap = strlen(reform(framestr[n,*]))
    maxlength = max(scrap)
    for m=0,nframes-1 do framestr[n,m] = framestr[n,m] + strmid(space,0,maxlength-scrap[m]+1)
endfor


;action = get_button(['execute','return','GO BACK'],TITLE='Please select action.'+string(10B)+string(10B)+'To run on linux, ' $
;    +'please select return. Login to a linux machine and enter the script on the command line.'+string(10B)+'Maybe something ' $
;    +'like this'+string(10B)+fidl_anova_csh+'>'+fidl_anova_csh+'.log',BASE_TITLE=fidl_anova_csh)
;if action eq 2 then goto,goback5
;if action eq 0 then mailstr = ' |& mail `whoami`'
;START110330
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=fidl_anova_csh)
if action eq 2 then goto,goback5


rtn_within = get_treatment_str(rtn_within)
driver = directory+'/'+driver
openw,lu_driver,driver,/GET_LUN
scrap = 'trial '
for i=0,rtn_within.nfactors-1 do scrap = scrap + rtn_within.factor_names[i] + ' '
if nframes gt 1 then scrap = scrap + 'time '
if chunks eq 1 then $
    scrap = scrap + '*.4dfp.img' $
else $
    scrap = scrap + 'CHUNKS'

printf,lu_driver,scrap
maxlength = strlen(strtrim(tnof,2))
scrap = strtrim(indgen(tnof)+1,2)
scrapcount = maxlength-strlen(scrap)+1
scrapspace = strmid(space,0,scrapcount)
level_name = strarr(tnof)



;i = 0
;for m=0,nboldframes-1 do begin
;    for k=0,trialframes[m]-1 do begin
;        level_name[i] = rtn_within.driver_levels[driver_levels_index[m]]
;        i = i + 1
;    endfor
;endfor

;HERE
;ntrialframes = n_elements(trialframes) ;trialframes and boldframes are modified by print_boldframes
;i = 0
;for m=0,n_elements(trialframes)-1 do begin
;    for k=0,trialframes[m]-1 do begin
;        level_name[i] = rtn_within.driver_levels[driver_levels_index[m]]
;        i = i + 1
;    endfor
;    ;print,'here0' 
;endfor

ntrialframes = n_elements(trialframes) ;trialframes and boldframes are modified by print_boldframes
;print,'ntrialframes=',ntrialframes,' tnof=',tnof
i = 0
for m=0,ntrialframes-1 do begin
    for k=0,trialframes[m]-1 do begin
        level_name[i] = rtn_within.driver_levels[driver_levels_index[m]]
        i = i + 1
    endfor
    ;print,'here0'
endfor

;for m=0,ntrialframes-1 do print,'trialframes=',trialframes[m],' driver_levels_index=',driver_levels_index[m]





level_name = strcompress(level_name)
maxlength = max(strlen(level_name))
scrapcount = maxlength-strlen(level_name)+1
level_name_space = strmid(space,0,scrapcount)
widget_control,/HOURGLASS
if chunks eq 1 then begin
    i = 0
    ;for m=0,nboldframes-1 do begin
    for m=0,ntrialframes-1 do begin
        for k=0,trialframes[m]-1 do begin
            scraplabel = strsplit(result[i],/EXTRACT)
            scraplabel = scraplabel[n_elements(scraplabel)-1]
            printf,lu_driver,scrap[i]+scrapspace[i]+level_name[i]+level_name_space[i]+framestr[0,k]+scraplabel
            i = i + 1
        endfor
    endfor
endif else begin   
    i = 0L
    j = 0L
    ;for m=0,nboldframes-1 do begin
    for m=0,ntrialframes-1 do begin
        for k=0,trialframes[m]-1 do begin
            printf,lu_driver,scrap[i]+scrapspace[i]+level_name[i]+level_name_space[i]+framestr[0,k]+' CHUNKS'
            for n=0,chunks-1 do begin
                scraplabel = strsplit(result[j],/EXTRACT)
                printf,lu_driver,FORMAT='("          ",a)',scraplabel[n_elements(scraplabel)-1]
                j = j + 1
            endfor
            i = i + 1
        endfor
    endfor
endelse
close,lu_driver
free_lun,lu_driver




;scratchdir_str = ' -scratchdir '+result[nfiles-1]

;STARTA
dummy = strpos(result[nfiles-1],'/')
scratchdir_str = ' -scratchdir '+strmid(result[nfiles-1],dummy,strpos(result[nfiles-1],'/',/REVERSE_SEARCH)-dummy+1)
;command = command + scratchdir_str





fidl_anova_csh = directory+'/'+fidl_anova_csh
openw,lu_csh,fidl_anova_csh,/GET_LUN
top_of_script,lu_csh
print_files_to_csh,lu_csh,nrun,files,'TC_FILES','tc_files'
printf,lu_csh,''
printf,lu_csh,'nice +19 $BIN'+fidl_bolds_str+scratchdir_str+mailstr

;printf,lu_csh,''
;printf,lu_csh,'nice +19 $BIN/fidl_anova -driver "'+driver+'"'+out_str+region_or_uncompress_str+threshold_extent_str $
;    +glm_list_str+scratchdir_str+volstr+' -clean_up'+mailstr
;START110330
printf,lu_csh,'nice +19 $BIN/fidl_anova_new -driver "'+driver+'"'+out_str+region_or_uncompress_str+threshold_extent_str $
    +glm_list_str+scratchdir_str+volstr+' -classical -clean_up'+mailstr

close,lu_csh
free_lun,lu_csh
spawn,'chmod +x '+fidl_anova_csh
widget_control,/HOURGLASS
if action eq 0 then begin
    spawn,fidl_anova_csh+' &'

    ;stats = dialog_message(fidl_anova_csh+string(10B)+string(10B)+'Anova script submitted as batch job.'+string(10B) $
    ;    +'The log file(s) will be e-mailed to you upon completion.',/INFORMATION)
    ;START110330
    stats = dialog_message(fidl_anova_csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)

endif
print,'DONE'
end
