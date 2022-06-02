;Copyright 12/31/99 Washington University.  All Rights Reserved.
;linmod_c.pro  $Revision: 12.124 $
pro linmod_c,fi,dsp,wd,glm,help,stc,pref,DEFAULT=default,GLM_IDX=glm_idx
common fidl_batch,macro_commands,num_macro_cmd,lcbatch,batch_base, $
        batch_id,batch_name,macro_descriptor,num_macro_dsc,batch_pro
t4_identify_str = ''
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE=' Select file containing BOLD runs.' 
if keyword_set(DEFAULT) then begin
    lcdefault = !TRUE
    lcexec = !FALSE
    if(pref.glm_comp_new eq !TRUE) then $
        lcopen = !CREATE $
    else $
        lcopen = !APPEND
    conc_file = pref.glm_comp_concfile
    glm_file  = pref.glm_comp_glmfile
    lc_atlas = pref.glm_comp_xform
    lc_gauss = pref.glm_comp_smoth
    lc_smoothness_estimate = pref.glm_comp_smoth
    t4_file = pref.glm_comp_t4_file
    fwhm = pref.glm_comp_fwhm
    if(keyword_set(GLM_IDX)) then begin
        nF = tot_eff + 1
        F_names = strarr(nF)
        F_names[0:nF-2] = effect_labels[0:tot_eff-1]
        F_names[nF-1] = 'Omnibus'
    endif else begin
        nF = 1
        F_names = 'Omnibus'
    endelse
    if(strpos(conc_file,'.4dfp.img') gt 0) then begin
        num_files = 1
        files = conc_file
    endif else begin
        lines = ''
        spawn,'cat ' + conc_file,lines
        num_files = n_elements(lines) - 2
        files = strarr(num_files)
        for i=0,num_files-1 do $
            files[i] = strmid(lines[i+1],strpos(lines[i+1],':')+1)
    endelse
endif else begin
    lcdefault = !FALSE
    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Please select data to be modeled.'
    if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,file_idx,'BOLD data to be processed',/GET_CONC) ne !OK then return
    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Please select *.glm file that defines the model to be used.'
    if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,glm_hdr,glm_idx,'linear model',/GETGLMS) ne !OK then return
    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='"Transform to atlas space" uses a precomputed _t4 file ' $
        +'to transform the data to atlas space for processing.  "Smooth with a Gaussian filter" smooths the data with ' $
        +'a 3D Gaussian filter before processing.  "Compute smoothness estimate" computes the smoothness of the Gaussian ' $
        +'random field composed of the residuals of the model.  This smoothness is used in the SPM multiple comparisons ' $
        +'correction method.  Select all buttons if you want to do a smoothed analysis across subject.  Do not select any ' $
        +'buttons to do an unsmoothed, within-subject analysis.'
    labels = strarr(2)
    labels[0] = 'Transform to atlas space'
    labels[1] = 'Smooth with Gaussian filter'
    option_list = get_bool_list(labels,TITLE='Select options. (None for unsmoothed analysis.)')
    option_list = option_list.list
    lc_atlas = option_list[0]
    lc_gauss = option_list[1]
    if lc_gauss then begin
        str = get_str(1,'FWHM of Gaussian filter: ','2.')
        fwhm = float(str[0])
    endif
    num_files = stc[file_idx].n
    if lc_atlas then begin
        get_dialog_pickfile,'*_anat_ave_to_711-2B_t4',fi.path,'Please select t4(s).',t4_file,nt4_file,rtn_path,/MULTIPLE_FILES
        if nt4_file eq 0 then return
        fi.path = rtn_path
        if nt4_file gt 1 then begin
            scrap = num_files/nt4_file 
            dummy = strarr(nt4_file)
            start = 1 
            for i=0,nt4_file-1 do begin
                if i eq nt4_file-1 then last = num_files else last = start-1+scrap
                dummy[i] = strtrim(start,2)+'-'+strtrim(last,2)
                start = start + scrap
            endfor
            scrap = get_str(nt4_file,t4_file,dummy,TITLE='Please assign runs to t4s. First run is 1.', $
                LABEL='Elements can be separated by spaces, tabs, or commas. Example 1-3,5',/ONE_COLUMN,/BELOW)
            t4_identify = intarr(num_files)
            for i=0,nt4_file-1 do begin
                segments = strsplit(scrap[i],'[ '+string(9B)+',]',/REGEX,/EXTRACT)
                for seg=0,n_elements(segments)-1 do begin
                    limits = long(strsplit(segments[seg],'-',/EXTRACT)) - 1
                    if n_elements(limits) eq 1 then limits = long(strsplit(segments[seg],/EXTRACT)) - 1
                    if n_elements(limits) eq 1 then limits = [limits[0],limits[0]]
                    if limits[0] lt 0 then limits[0] = 0
                    if limits[1] gt num_files-1 then limits[1] = num_files-1
                    t4_identify[limits[0]:limits[1]] = i 
                endfor
            endfor 
            t4_identify = strtrim(t4_identify+1,2)
            t4_identify_str = ' -t4'
            for i=0,num_files-1 do t4_identify_str = t4_identify_str + ' ' + t4_identify[i] 
        endif
    endif















    effect_label = *glm[glm_idx].effect_label
    nlab = n_elements(effect_label)
    glab = strarr(nlab+1)
    nc = glm[glm_idx].nc
    lcstop = !FALSE
    nF = 1
    F_names = 'Omnibus'

    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='This dialog allows you to compute F statistics for any ' $
        +'combination of effects.  "Define another F statistic" lets you select effects to be included.  Listings below ' $
        +'"Exit loop" describe F statistics that will be computed.  An omnibus F statistic (over all effects of interest) ' $
        +'is always computed.  Select "Exit loop" to stop defining F statistics.'

    repeat begin
        labels = strarr(nF+2)
        labels[0] = 'Define another F statistic'
        labels[1] = 'Exit loop'
        labels[2:nF+1] = F_names
        action = get_button(labels,TITLE='List of F statistics and Actions')
        case action of
            0: begin ; Define New F statistic.
                   list = get_bool_list(effect_label[0:nlab-1],TITLE='Specify effects to include')
                   list = list.list
                   F_name = ''
                   nn = 0
                   for neff=0,nlab-1 do begin
;                      Build concatenated string of effect names
                       if(list[neff] eq 1) then begin
                           if(nn eq 0) then $
                               F_name = effect_label[neff] $
                           else $
                               F_name = F_name + '\&' + effect_label[neff]
                           nn = nn + 1
                       endif
                   endfor
                   tmp = F_names
                   nF = nF + 1
                   F_names = strarr(nF)
                   F_names[0:nF-2] = tmp
                   F_names[nF-1] = strcompress(F_name,/REMOVE_ALL)
               end
            1: begin
                   glm[glm_idx].nF = nF
                   if(ptr_valid(glm[glm_idx].F_names)) then $
                       ptr_free,glm[glm_idx].F_names
                   glm[glm_idx].F_names = ptr_new(F_names)
                   lcstop = !TRUE
               end
            else: begin
                  print,'Select one of the first two options.'
                  widget_control,wd.error,SET_VALUE=string('Select one of the first two options.')
               end
        endcase
    endrep until (lcstop eq !TRUE)
    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Specifiy file name for design matrix (*.glm).  The ' $
        +'design matrix specified earlier will be stored in this file.  The model estimates will be added to this file ' $
        +'after they are computed by the C program.'
    glm_file = save_linmod(fi,dsp,wd,glm,help,stc,pref,glm_idx,glm_idx,/DESIGN_ONLY)
    if glm_file eq 'ERROR' then return
    labels = strarr(4)
    labels[0] = 'Initialize, write script, and execute'
    labels[1] = 'Initialize, write script, and return'
    labels[2] = 'Append to script and execute'
    labels[3] = 'Append to script and return'
    action = get_button(labels,TITLE='Please select.')
    case action of
        0: begin
           lcexec = !TRUE
           lcopen = !CREATE
        end
        1: begin
           lcexec = !FALSE
           lcopen = !CREATE
        end
        2: begin
           lcexec = !TRUE
           lcopen = !APPEND
        end
        3: begin
           lcexec = !FALSE
           lcopen = !APPEND
        end
        else: begin
            print,'Invalid action.'
            return
        end
    endcase
endelse
cd,CURRENT=dir
if(lcdefault eq !TRUE) then begin
    csh_file = dir + '/glm_compute'
endif else begin
    if(lcopen eq !CREATE) then begin
        x = str_sep(strcompress(systime()),' ')
        y = str_sep(x[3],':')
        csh_file = dir + '/glm_compute_' + strmid(x[4],2,2)+x[1]+x[2]+'@'+y[0]+':'+y[1]
        fi.csh_file = csh_file
    endif else begin
        csh_file = fi.csh_file
    endelse
endelse
txt_file = csh_file + '.txt'
csh_file = csh_file + '.csh'

;START130206
;get_lun,lu

if lcopen eq !CREATE then begin
    err = 0

    ;openw,lu,csh_file,ERROR=err
    ;START130226
    openw,lu,csh_file,ERROR=err,/GET_LUN


    top_of_script,lu
    while err ne 0 do begin
        stat=dialog_message('Default script file (glm_compute.csh) is protected.  Please select another file name.')
        csh_file = dialog_pickfile(FILTER='*csh',PATH=fi.path, $
               GET_PATH=wrtpath,TITLE='Select or enter GLM file.')
        if(csh_file eq '') then begin
            stat=dialog_message('*** Invalid file.  ***',/ERROR)
            err = 0
        endif
        if n_elements(csh_file) gt 1 then csh_file = csh_file(0) + csh_file(1)
        print,csh_file
        openw,lu,csh_file,ERROR=err
    end

    ;START130206
    ;close,lu
    ;openw,lu,csh_file
    ;printf,lu,'#!/bin/csh

endif else begin

    ;openw,lu,csh_file,/APPEND
    ;START310206
    openw,lu,csh_file,/APPEND,/GET_LUN

endelse


printf,lu,''
print_files_to_csh,lu,num_files,*stc[file_idx].filnam,'BOLD_FILES','bold_files',/NO_NEWLINE
printf,lu,'set INPUT_GLM_FILE = ','"','-input_glm_file ',glm_file,'"',FORMAT='(a,a,a,a,a)'
printf,lu,'set OUTPUT_GLM_FILE = ','"','-output_glm_file ',glm_file,'"',FORMAT='(a,a,a,a,a)'
if lc_atlas then $
    print_files_to_csh,lu,nt4_file,t4_file,'T4_FILE','xform_file' $
else $
    printf,lu,'set T4_FILE
if lc_gauss then $
    printf,lu,'set FWHM = ','"','-fwhm ',fwhm,'"',FORMAT='(a,a,a,a,a)' $
else $
    printf,lu,'set FWHM'
if lc_atlas then $
    printf,lu,'set MASK_FILE = ','"','-mask_file ',!MASK_FILE_222,'"',FORMAT='(a,a,a,a,a,a)' $
else if fi.space[file_idx] eq !SPACE_111 then $
    printf,lu,'set MASK_FILE = ','"','-mask_file ',!MASK_FILE_111,'"',FORMAT='(a,a,a,a,a,a)' $
else if fi.space[file_idx] eq !SPACE_222 then $
    printf,lu,'set MASK_FILE = ','"','-mask_file ',!MASK_FILE_222,'"',FORMAT='(a,a,a,a,a,a)' $
else if fi.space[file_idx] eq !SPACE_333 then $
    printf,lu,'set MASK_FILE = ','"','-mask_file ',!MASK_FILE_333,'"',FORMAT='(a,a,a,a,a,a)' $
else $
    printf,lu,'set MASK_FILE'
print_files_to_csh,lu,nF,F_names,'Foptions','F'
lc_smoothness_estimate=0
mode = 4*lc_smoothness_estimate + 2.*lc_atlas + lc_gauss
printf,lu,'set MODE = ','"','-mode ',mode,'"',FORMAT='(a,a,a,1x,i1,a)'
printf,lu,''
printf,lu,'nice +19 $BIN/compute_glm2 $BOLD_FILES $INPUT_GLM_FILE $OUTPUT_GLM_FILE $T4_FILE $MASK_FILE $FWHM $MODE $Foptions' $
    +t4_identify_str
close,lu
spawn,'chmod 0777 '+ csh_file
if lcexec eq !TRUE then begin
    if set_batch(script) eq !ERROR then return
endif
free_lun,lu
print,'Script written to ',csh_file
print,'DONE'
end
