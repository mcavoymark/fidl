;Copyright 2/9/04 Washington University.  All Rights Reserved.
;fidl_autocorrelation.pro  $Revision: 1.10 $
pro fidl_autocorrelation,fi,dsp,wd,glm,help,stc,pref
goback0:
region_str = ''
roi_str = ''
racar_str = ''
option_str = ''
fwhm_str = ''
mask_str = ''
scrap=get_glms()
if scrap.msg ne !OK then return
load = scrap.load
glmfiles = scrap.glmfiles
nlists = scrap.nlists
total_nsubjects = scrap.total_nsubjects
t4select = scrap.t4select
glm_list_str = scrap.glm_list
concselect = scrap.concselect
event_files = scrap.evselect
retain_estimates_str = make_array(total_nsubjects,/STRING,VALUE='')
print_frames_str = make_array(total_nsubjects,/STRING,VALUE='')
ifh = replicate({InterFile_Header},total_nsubjects)
glm_space = intarr(total_nsubjects)
widget_control,/HOURGLASS
for i=0,total_nsubjects-1 do begin
    ifh[i] = read_mri_ifh(glmfiles[i],/GLM)
    if ifh[i].glm_rev gt -17 then begin
        stat=dialog_message('Revision number for '+strmid(glmfiles[i],1)+' too old. Update file by resaving.',/ERROR)
        return
    endif
    glm_space[i] = get_space(ifh[i].glm_xdim,ifh[i].glm_ydim,ifh[i].glm_zdim)
endfor
if total(glm_space - glm_space[0]) ne 0 then begin
    stat=dialog_message('Not all GLMs are in the same space. Abort!',/ERROR)
    return
endif
if n_elements(concselect) eq 0 then begin

    ;concfiles = get_files(TITLE='Need conc files. Please enter filter.',FILTER='*.conc')
    ;concselect = match_files(glmfiles,concfiles,TITLE='Please select conc file for ')
    ;START131015
    gf = get_files(TITLE='Need conc files. Please enter filter.',FILTER='*.conc')
    if gf.msg eq 'GO_BACK' then goback0 else if gf.msg eq 'EXIT' then return
    concfiles = gf.files
    if n_elements(glmfiles) eq 1 and n_elements(concfiles) eq 1 and gf.msg eq 'DONTSELECT' then $
        concselect = concfiles $
    else $
        concselect = match_files(glmfiles,concfiles,TITLE='Please select conc file for ')
   

endif else $
    concselect = get_str(total_nsubjects,glmfiles,concselect,TITLE='Please check concs.',/ONE_COLUMN,/BELOW)
if glm_space[0] eq !SPACE_111 or glm_space[0] eq !SPACE_222 or glm_space[0] eq !SPACE_333 then begin
    scrap = ifh[*].glm_fwhm 
    index = where(scrap gt 0.,count)
    if count eq 0 then $
        fwhm = 2. $
    else begin
        if total(scrap - scrap[0]) ne 0. then begin
            print,'GLMs not all smoothed the same.'
            print,'    Could be different amounts of smoothing or GLMs made before this parameter was saved.' 
            fwhm = 2.
        endif else $ 
            fwhm = scrap[0]
    endelse
    goback10:
    fwhm = float(get_str(1,'FWHM in voxels ',strtrim(fwhm,2),WIDTH='50',TITLE='Have your GLMs been smoothed?', $
        LABEL='Enter 0 if they have not been smoothed.'))
    if fwhm[0] gt 0. then fwhm_str = ' -gauss_smoth ' + string(fwhm[0],FORMAT='(f3.1)')
    rl=read_list(concselect[0])

    ;if rl.msg eq 'ERROR' then return else if rl.msg eq 'GOBACK' then goto,goback10
    ;START150219
    if rl.msg eq 'EXIT' then return else if rl.msg eq 'GOBACK' then goto,goback10

    files=rl.img
    bold_ifh = read_mri_ifh(files[0])
    if (get_space(bold_ifh.matrix_size_1,bold_ifh.matrix_size_2,bold_ifh.matrix_size_3) ne glm_space[0]) and $
        (t4select[0] eq '') then begin

        ;t4files = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_711-2?_t4')
        ;t4select = match_files(glmfiles,t4files,TITLE='Please select t4 file for ')
        ;START131015
        gf = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_711-2?_t4')
        if gf.msg eq 'GO_BACK' then goto,goback10 else if gf.msg eq 'EXIT' then return
        t4files = gf.files
        if n_elements(glmfiles) eq 1 and n_elements(t4files) eq 1 and gf.msg eq 'DONTCHECK' then $
            t4select = t4files $
        else $
            t4select = match_files(glmfiles,t4files,TITLE='Please select t4 file for ')

    endif
endif
if t4select[0] ne '' then t4select = get_str(total_nsubjects,glmfiles,t4select,TITLE='Please check t4s.',/ONE_COLUMN,/BELOW)
goback:
atlas_space = glm_space[0]
analysis_type = get_button(['voxel by voxel','specfic regions'],TITLE='Please select analysis type.')
if analysis_type eq 1 then begin
    rtn = get_regions(fi,wd,dsp,help)
    if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then goto,goback
    region_names = rtn.region_names
    region_str = rtn.region_str
    region_space = rtn.space
    rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.')
    scrap = strtrim(rtn.index+1,2)
    roi_str = ' -regions_of_interest'
    for i=0,rtn.count-1 do roi_str = roi_str + ' ' + scrap[i]
    if glm_space[0] ne region_space then begin
        if glm_space[0] eq !SPACE_DATA and (region_space eq !SPACE_111 or region_space eq !SPACE_222 or $
            region_space eq !SPACE_333) then begin
            if t4select[0] eq '' then begin

                ;t4files = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_711-2?_t4')
                ;t4select = match_files(glmfiles,t4files,TITLE='Please select t4 file for ')
                ;START131015
                gf = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_711-2?_t4')
                if gf.msg eq 'GO_BACK' then goto,goback10 else if gf.msg eq 'EXIT' then return
                t4files = gf.files
                if n_elements(glmfiles) eq 1 and n_elements(t4files) eq 1 and gf.msg eq 'DONTCHECK' then $
                    t4select = t4files $
                else $
                    t4select = match_files(glmfiles,t4files,TITLE='Please select t4 file for ')


            endif
        endif else begin
            stat=dialog_message('Space incompatibility! Abort! Abort! Abort!',/ERROR)
            return
        endelse
    endif
    atlas_space = region_space
endif
if atlas_space eq !SPACE_111 then begin
    mask_str = ' -mask ' + !MASK_FILE_111 + ' -atlas 111'
endif else if atlas_space eq !SPACE_222 then begin
    mask_str = ' -mask ' + !MASK_FILE_222 + ' -atlas 222'
endif else if atlas_space eq !SPACE_333 then begin
    mask_str = ' -mask ' + !MASK_FILE_333 + ' -atlas 333'
endif


;lc_dont_average = !TRUE
;lc_dont_skip = !TRUE
;labels = strarr(2,1)
;labels[0,0] = 'Express autocorrelation as percent-change'
;labels[1,0] = "Express autocorrelation as raw MR units'
;dflt = [!TRUE]
;lclist = get_choice_list(labels,TITLE='Select options',DEFAULT=dflt)
;lc_pctchg = lclist[0]
;if lc_pctchg eq !TRUE then option_str = option_str + ' -percent_change'
option_str = option_str + ' -skip'



junk1 = strpos(glmfiles,'/',/REVERSE_SEARCH)+1
junk2 = strpos(glmfiles,'.',/REVERSE_SEARCH)-junk1
glmroots = strarr(total_nsubjects)
for i=0,total_nsubjects-1 do glmroots[i] = strmid(glmfiles[i],junk1[i],junk2[i])
;print,'glmroots=',glmroots

get_directory,directory
dummy = 1 + total_nsubjects
if load eq 1 or n_elements(concfiles) gt 0 then dummy = dummy + 1 
scrap = strarr(dummy)
scraplabels = strarr(dummy)
scrap[0] = 'script'
scraplabels[0] = directory + '/fidl_autocorrelation.csh'
j = 1
if load eq 1 or n_elements(concfiles) gt 0 then begin
    scrap[1] = 'list'
    if n_elements(glm_list_str) eq 0 then $
        scraplabels[1] = 'list.list' $
    else begin
        scraplabels[1] = glm_list_str
    endelse
    j = 2
endif
outstr = strarr(total_nsubjects)
for i=0,total_nsubjects-1 do outstr[i] = directory+'/'+glmroots[i]+'_'
if analysis_type eq 1 then for i=0,total_nsubjects-1 do outstr[i] = outstr[i] + 'regional_'
;if lc_dont_average eq !FALSE then for i=0,total_nsubjects-1 do outstr[i] = outstr[i] + 'avg'
for i=0,total_nsubjects-1 do outstr[i] = outstr[i] + 'autocor.4dfp.img'
if analysis_type eq 0 then begin
    k = 0
    for i=j,total_nsubjects+j-1 do begin
        scrap[i] = 'output ' + glmroots[k]
        scraplabels[i] = outstr[k]
        k = k + 1
    endfor
endif else begin
    k = 0
    for i=j,total_nsubjects+j-1 do begin
        scrap[i] = 'output '+ glmroots[k]
        scraplabels[i] = outstr[k] + '.txt'
        k = k + 1
    endfor
endelse
goback20:
names = get_str(dummy,scrap,scraplabels,WIDTH=150,TITLE='Please enter desired filenames.')
compute_residuals_csh = fix_script_name(names[0])
j = 1
if load eq 1 or n_elements(concfiles) gt 0 then begin
    glm_list_str = fix_script_name(names[1])
    j = 2
endif
if analysis_type eq 0 then begin 
    k = 0
    for i=j,total_nsubjects+j-1 do begin
        outstr[k] = ' -autocorrelation_name "' + names[i] + '"'
        k = k + 1
    endfor
endif else begin 
    k = 0 
    for i=j,total_nsubjects+j-1 do begin
        outstr[k] = ' -regional_residual_name "' + names[i] + '"'
        k = k + 1
    endfor
endelse
if load eq 1 or n_elements(concfiles) gt 0 then begin 
    if n_elements(event_files) eq 0 then begin
        if t4select[0] eq '' then $
            write_list,glm_list_str,[total_nsubjects,total_nsubjects],[glmfiles,concselect] $
        else $
            write_list,glm_list_str,[total_nsubjects,total_nsubjects,total_nsubjects],[glmfiles,t4select,concselect]
    endif else begin
        if t4select[0] eq '' then $
            write_list,glm_list_str,[total_nsubjects,total_nsubjects,total_nsubjects],[glmfiles,concselect,event_files] $
        else $
            write_list,glm_list_str,[total_nsubjects,total_nsubjects,total_nsubjects,total_nsubjects], $
                [glmfiles,t4select,concselect,event_files]
    endelse
endif
t4str = make_array(total_nsubjects,/STRING,VALUE='')
if t4select[0] ne '' then for i=0,total_nsubjects-1 do t4str[i] = ' -xform_file ' + t4select[i]
openw,lu,compute_residuals_csh,/GET_LUN
printf,lu,'#!/bin/csh'+string(10B)+'unlimit'
for i=0,total_nsubjects-1 do begin
    printf,lu,''
    print_files_to_csh,lu,1,glmfiles[i],'GLM_FILE','glm_file',/NO_NEWLINE
    widget_control,/HOURGLASS
    rl=read_list(concselect[i])

    ;if rl.msg eq 'ERROR' then return else if rl.msg eq 'GOBACK' then goto,goback20
    ;START150219
    if rl.msg eq 'EXIT' then return else if rl.msg eq 'GOBACK' then goto,goback20

    print_files_to_csh,lu,rl.nimg,rl.img,'BOLD_FILES','bold_files',/NO_NEWLINE
    printf,lu,'nice +19 '+!BIN+'/compute_residuals $GLM_FILE $BOLD_FILES'+print_frames_str[i]+t4str[i]+retain_estimates_str[i] $
        +option_str+region_str+roi_str+racar_str+outstr[i]+fwhm_str+mask_str
endfor
close,lu
free_lun,lu
spawn,'chmod +x '+compute_residuals_csh
action = get_button(['execute','return'],TITLE='Please select action.',BASE_TITLE=compute_residuals_csh)
if action eq 0 then begin
    spawn,compute_residuals_csh+' &'
    stats = dialog_message(compute_residuals_csh+' submitted as batch job.'+string(10B)+'The log file(s) will be e-mailed to ' $
        +'you upon completion.',/INFORMATION)
endif
print,'DONE'
end
