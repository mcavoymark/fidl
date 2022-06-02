;Copyright 7/04/02 Washington University.  All Rights Reserved.
;compute_avg_zstat_csh.pro  $Revision: 1.44 $
pro compute_avg_zstat_csh,compute_avg_zstat_csh_str,ngroups,load,nfiles_per_group,files_per_group,t4files,time,ntreatments, $
    group_names,magstr,gauss_str,region_file,cols_or_rows_str,roi_str,avgstat_output_str,Nimage_name_str, $
    Nimage_mask_str,delay_str,within_subject_sd_str,magnorm_str, $
    nframes,effect_length_ts,conditions_time,sumrows_time,sum_contrast_for_treatment, $ ;timecourse parameters
    tc_type,print_unscaled_mag_str,lcmag,glm_list_str,directory_str, $ ;strings
    contrast,wfiles,lcsd,extra_str,nglms_per_sub,mail_str,append,binary_name,nsubject_per_group,behav_data1,indvar
if n_elements(print_unscaled_tc_str) eq 0 then print_unscaled_tc_str = ''
if n_elements(tc_type) eq 0 then tc_type = ''
if n_elements(print_unscaled_mag_str) eq 0 then print_unscaled_mag_str = ''
if n_elements(lcmag) eq 0 then lcmag = ''
if n_elements(glm_list_str) eq 0 then glm_list_str = ''
if n_elements(directory_str) eq 0 then directory_str = ''
if n_elements(extra_str) eq 0 then extra_str = ''
if n_elements(nglms_per_sub) eq 0 then nglms_per_sub = 1 
if n_elements(mail_str) eq 0 then mail_str = ' |& mail `whoami`'
if n_elements(append) eq 0 then append = 0
if n_elements(binary_name) eq 0 then binary_name = 'compute_avg_zstat' 
if n_elements(nsubject_per_group) eq 0 then nsubject_per_group = nfiles_per_group
t4string = ''
region_str = ''
cstr = ''
tcstr = ''
if n_elements(lcsd) gt 0 then sd_str = lcsd else sd_str = ' -sem'
nelements_ntreatments = n_elements(ntreatments)
if nelements_ntreatments lt 2 then begin
    if n_elements(conditions_time) gt 0 then begin ;use timecourses
        ntreatments = [ntreatments,0]
    endif else begin
        contrast = sum_contrast_for_treatment
        ntreatments = [0,ntreatments]
    endelse
endif
ntests = ntreatments/nglms_per_sub
if n_elements(nframes) gt 0 then size_conditions_time = size(conditions_time) 
openw,lu_csh,compute_avg_zstat_csh_str,/GET_LUN,APPEND=append
if append eq 0 then top_of_script,lu_csh 
if region_file[0] ne '' then begin
    printf,lu_csh,''
    print_files_to_csh,lu_csh,n_elements(region_file),region_file,'REGION_FILE','region_file',/NO_NEWLINE
    region_str = ' $REGION_FILE'
end
roi_str1=''
if roi_str[0] ne '' then begin
    printf,lu_csh,'set ROI = (-regions_of_interest \'
    printf,lu_csh,'        '+roi_str+')'
    roi_str1 = ' $ROI'
endif
if n_elements(wfiles) gt 0 then begin
    if wfiles[0] ne '' then begin
        print_files_to_csh,lu_csh,n_elements(wfiles),wfiles,'WEIGHT_FILES','weight_files'
        wfiles = ' $WEIGHT_FILES'
    endif
endif else $
    wfiles = ''
j = 0
j1 = 0
for n=0,ngroups-1 do begin
    start_j = j
    start_j1 = j1
    if load lt 2 then $
        print_files_to_csh,lu_csh,nfiles_per_group[n],files_per_group[j:j+nfiles_per_group[n]-1],'GLM_FILES','glm_files',/NO_NEWLINE $
    else $
        print_files_to_csh,lu_csh,nfiles_per_group[n],files_per_group[j:j+nfiles_per_group[n]-1],'FILES','files'
    if n_elements(t4files) gt 0 then begin
        if t4files[0] ne '' then begin
            j = start_j

            ;print_files_to_csh,lu_csh,nfiles_per_group[n],t4files[j:j+nfiles_per_group[n]-1],'T4_FILES','xform_files'
            ;START130109
            print_files_to_csh,lu_csh,nfiles_per_group[n],t4files[j:j+nfiles_per_group[n]-1],'T4_FILES','xform_files',/NO_NEWLINE

            t4string = ' $T4_FILES'
        endif
    endif
    if ntreatments[1] gt 0 then begin ;use magnitudes
        printf,lu_csh,'set CONTRASTS = (-contrasts \'
        print_contrasts_to_csh,lu_csh,ntreatments[1],nsubject_per_group[n],start_j1,contrast,ntests[1]
        cstr = ' $CONTRASTS'
    endif
    if ntreatments[0] gt 0 then begin ;use timecourses
        printf,lu_csh,'set TIME_COURSES = (-tc \'
        print_tc_to_csh,lu_csh,ntreatments[0],nframes,nsubject_per_group[n],start_j1,conditions_time,sumrows_time, $
            sum_contrast_for_treatment,effect_length_ts,ntests[0]
        tcstr = ' $TIME_COURSES'
        if n_elements(behav_data1) ne 0 and n_elements(indvar) ne 0 then begin
            printf,lu_csh,'set TC_WEIGHTS = (-tc_weights \'
            print_tcwts_to_csh,lu_csh,ntreatments[0],nframes,nsubject_per_group[n],start_j1,conditions_time,sumrows_time, $
                sum_contrast_for_treatment,effect_length_ts,ntests[0],behav_data1,indvar
            tcstr = tcstr + ' $TC_WEIGHTS'
        endif
    endif

    ;if group_names[0] ne '' then scrap = fix_script_name(group_names[n])
    ;START130308
    if n_elements(group_names) ne 0 then scrap = fix_script_name(group_names[n])

    if load lt 2 then begin

        ;if group_names[0] ne '' then $
        ;START310308
        if n_elements(group_names) ne 0 then $

            group_str = ' -group_name "' + scrap + '"' $
        else $
            group_str = ''
        if n_elements(glm_list_str) eq 2 then $
           scrapstr = glm_list_str[n] $
        else $
           scrapstr = glm_list_str 
        printf,lu_csh,'nice +19 $BIN/'+binary_name+' $GLM_FILES'+tcstr+cstr+magstr+t4string+gauss_str+region_str $
            +cols_or_rows_str+roi_str1+avgstat_output_str[n]+Nimage_name_str+Nimage_mask_str+group_str+delay_str $
            +within_subject_sd_str+print_unscaled_tc_str+tc_type+magnorm_str+print_unscaled_mag_str+lcmag+scrapstr $
            +directory_str+wfiles+sd_str+extra_str+mail_str
    endif else begin
        if time eq 1 then begin
            avg_str = ' -avg "'+directory+'/avg_mag_'+scrap+'.4dfp.img"'
            sd_str = ' -sd "'+directory+'/stderr_mag_'+scrap+'.4dfp.img"'
        endif else begin
            avg_str = ' -avg "'+directory+'/avg_anat_'+scrap+'.4dfp.img"'
            sd_str = ' -sd "'+directory+'/stderr_anat_'+scrap+'.4dfp.img"'
        endelse
        mail_str =  ' |& mail `whoami`'
        printf,lu_csh,FORMAT='("avg_4dfp $FILES",a,a,a)',avg_str,sd_str,mail_str
    endelse
    printf,lu_csh,''

    ;START121231
    ;printf,lu_csh,''

    j = start_j + nfiles_per_group[n]
    j1 = start_j1 + nsubject_per_group[n]
endfor
close,lu_csh
free_lun,lu_csh
spawn,'chmod +x '+compute_avg_zstat_csh_str
;print,'compute_avg_zstat_csh bottom'
end
