;Copyright 12/31/99 Washington University.  All Rights Reserved.
;compute_residuals.pro  $Revision: 1.141 $
function compute_residuals,fi,dsp,wd,glm,help,stc,pref,AUTOCORR=autocorr,CROSSCORR=crosscorr,CROSSCOV=crosscov,COV=cov,PCA=pca, $
    GET_GLMS_STRUCT=get_glms_struct,MVPA=mvpa,FIDL_TC_SS=fidl_tc_ss,DONTCHECKCONC=dontcheckconc,DONTLOADCONC=dontloadconc, $
    DONTASKSMOOTH=dontasksmooth
spawn,'whoami',whoami
unbias_maxlik_str = ''
pca_str = ''
no_ac_cc_str=''
no_ac_cc_str2=''
if keyword_set(AUTOCORR) then begin
    no_ac_cc=1 
    no_ac_cc_str=' -autocorr'
endif else if keyword_set(CROSSCORR) then begin
    no_ac_cc=2 
    no_ac_cc_str=' -crosscorr'
    no_ac_cc_str2=' -crosscorr'
endif else if keyword_set(CROSSCOV) then begin
    no_ac_cc=2 
    no_ac_cc_str=' -crosscov'
endif else if keyword_set(COV) then begin
    no_ac_cc=3
    no_ac_cc_str=' -cov'
    if no_ac_cc eq 3 then begin
        scrap = get_button(['unbiased estimates (divide by N-1)','maximum likelihood estimates (divide by N)','exit'], $
            BASE_TITLE='Please select.',TITLE='Variance-covariance')
        if scrap eq 2 then return,rtn={msg:'EXIT'} 
        if scrap eq 0 then unbias_maxlik_str = ' -unbiased' else unbias_maxlik_str = ' -maximumlikelihood'
    endif
endif else if keyword_set(PCA) then begin
    no_ac_cc=4 
    pca_str = ' -voxels'
endif else $
    no_ac_cc=0
racar_str = ''
option_str = ''
fwhm_str = ''
regtype = 0
globcovstr = ''
lc_retain = 1
nandbytrial = 0
lc_all = 0
lc_tempcov = 0
fidl_ac_optionstr=''
lcaddreg=0
lcseedreg=0
nlagsstr=''
fidlpca_str = ''
atlas_str=''
region_files = ''
roi_str = ''
scratchdir = ''
scratchdir_str = ''
lcscratch=1
t4select=''
goback0a:
if not keyword_set(GET_GLMS_STRUCT) then begin
    scrap=get_glms(/NOTMORETHANONE)
    if scrap.msg ne 'OK' then return,rtn={msg:'EXIT'}
    load = scrap.load
    glmfiles = scrap.glmfiles
    total_nsubjects = scrap.total_nsubjects
    t4select = scrap.t4select
    glm_list_str = scrap.glm_list
    concselect = scrap.concselect
    event_files = scrap.evselect
    ifh = scrap.ifh 
    glm_space = scrap.glm_space 
    bold_space = scrap.bold_space
    framesperrun = intarr(total_nsubjects)
    widget_control,/HOURGLASS
    for i=0,total_nsubjects-1 do framesperrun[i] = ifh[i].glm_t_valid/ifh[i].glm_n_files
endif else begin
    load = get_glms_struct.load
    glmfiles = get_glms_struct.glmfiles
    total_nsubjects = get_glms_struct.total_nsubjects
    t4select = get_glms_struct.t4select
    glm_list_str = get_glms_struct.glm_list
    concselect = get_glms_struct.concselect
    event_files = get_glms_struct.evselect
    glm_space = get_glms_struct.glm_space 
    bold_space = get_glms_struct.bold_space 
    ifh = get_glms_struct.ifh
endelse
if no_ac_cc gt 0 then begin
    if total(ifh[*].glm_TR-ifh[0].glm_TR) ne 0. then begin
        stat=dialog_message('Not all data acquired at the same TR. Abort!',/ERROR)
        return,rtn={msg:'EXIT'}
    endif
    TR = ifh[0].glm_TR
endif
if total(glm_space - glm_space[0]) ne 0 then begin
    stat=dialog_message('Not all GLMs are in the same space. Abort!',/ERROR)
    return,rtn={msg:'EXIT'}
endif
if glm_space[0] eq !SPACE_111 or glm_space[0] eq !SPACE_222 or glm_space[0] eq !SPACE_333 then begin
    if (bold_space[0] ne glm_space[0]) and (t4select[0] eq '') then begin
        t4select=make_array(total_nsubjects,/STRING,VALUE='')
        for i=0,total_nsubjects-1 do if ptr_valid(ifh[i].glm_t4files) then t4select[i] = *ifh[i].glm_t4files
        idx = where(t4select ne '',cnt)
        if cnt ne 0 then begin 
            t4select = get_str(total_nsubjects,glmfiles,t4select,TITLE='Please check t4s.',/ONE_COLUMN,/BELOW,/GO_BACK)
            if t4select[0] eq 'GO_BACK' then goto,goback0a
        endif else begin
            gf = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_*_t4')
            if gf.msg eq 'GO_BACK' then goto,goback0a else if gf.msg eq 'EXIT' then return,rtn={msg:'EXIT'}
            t4files=gf.files
            if gf.msg eq 'DONTCHECK' and n_elements(t4files) eq 1 then $
                t4select=t4files $
            else $
                t4select = match_files(glmfiles,t4files,TITLE='Please select t4 file for ')
        endelse
    endif
endif
ne_max = max(ifh[*].glm_N,ne_max_index,MIN=ne_min)
effect_label = *ifh[ne_max_index].glm_effect_label
effect_length = *ifh[ne_max_index].glm_effect_length

;for i=0,total_nsubjects-1 do begin
;    if i ne ne_max_index then begin
;        scrap=*ifh[i].glm_effect_label
;        lizard=*ifh[i].glm_effect_length
;        for j=0,ifh[i].glm_all_eff-1 do begin
;            superbird = strcmp(effect_label,scrap[j])
;            if total(superbird) eq 0 then begin
;                effect_label = [effect_label,scrap[j]]
;                effect_length = [effect_length,lizard[j]]
;            endif else begin
;                idx = where(superbird eq 1,cnt)
;                if effect_length[idx[0]] lt lizard[j] then effect_length[idx]=lizard[j]
;            endelse
;        endfor
;    endif
;endfor
;START160104
if n_elements(ifh) gt 1 then begin  
    for i=0,total_nsubjects-1 do begin
        if i ne ne_max_index then begin
            scrap=*ifh[i].glm_effect_label
            lizard=*ifh[i].glm_effect_length
            for j=0,ifh[i].glm_all_eff-1 do begin
                superbird = strcmp(effect_label,scrap[j])
                if total(superbird) eq 0 then begin
                    effect_label = [effect_label,scrap[j]]
                    effect_length = [effect_length,lizard[j]]
                endif else begin
                    idx = where(superbird eq 1,cnt)
                    if effect_length[idx[0]] lt lizard[j] then effect_length[idx]=lizard[j]
                endelse
            endfor
        endif
    endfor
endif

region_str = make_array(total_nsubjects,/STRING,VALUE='')
roi_str = make_array(total_nsubjects,/STRING,VALUE='')
retain_estimates_str = make_array(total_nsubjects,/STRING,VALUE='')
print_frames_str = make_array(total_nsubjects,/STRING,VALUE='')
tc_str = make_array(total_nsubjects,/STRING,VALUE='')
bf_str = make_array(total_nsubjects,/STRING,VALUE='')
if concselect[0] eq '' then concselect = ifh[*].data_file 
goback99:
if not keyword_set(DONTCHECKCONC) then begin
    scrap = get_str(total_nsubjects,glmfiles,concselect,TITLE='Please check concs.',/ONE_COLUMN,/BELOW,SPECIAL='Load concs',/GO_BACK, $
        FRONT=getenv('PWD')+'/')
    if scrap[0] eq 'GO_BACK' then return,rtn={msg:'GO_BACK'}
    concselect = scrap
    if concselect[0] eq 'SPECIAL' then begin
        goback100:
        gf = get_files(TITLE='Need conc files. Please enter filter.',FILTER='*.conc')
        if gf.msg eq 'GO_BACK' then goto,goback99 else if gf.msg eq 'EXIT' then return,rtn={msg:'EXIT'}
        concfiles = gf.files
        if n_elements(concfiles) eq 1 and total_nsubjects gt 1 then begin
            scrap = get_button(['ok','go back','exit'],BASE_TITLE='Please select.',TITLE='We have found a single conc'+string(10B)+ $
                string(10B)+concfiles[0]+string(10B)+string(10B)+'All GLMs will use this conc.')
            if scrap eq 1 then goto,goback100 else if scrap eq 2 then return,rtn={msg:'EXIT'} 
            concselect = make_array(total_nsubjects,/STRING,VALUE=concfiles[0])
        endif else if n_elements(concfiles) eq 1 and gf.msg eq 'DONTCHECK' then $
            concselect = concfiles $
        else $
            concselect = match_files(glmfiles,concfiles,TITLE='Please select conc file for ')
    endif
endif

if total(ifh[*].nreg-ifh[0].nreg) ne 0 then begin
    stat=dialog_message('Not all glms contain the same number of regions. Abort!',/ERROR)
    return,rtn={msg:'EXIT'}
endif
ifhnreg = ifh[*].nreg
if ifhnreg[0] gt 0 then begin
    goback991:
    scrap = '' 
    scrap1 = ''
    for i=0,total_nsubjects-1 do begin
        scrap = [scrap,make_array(ifh[i].nregfiles,/STRING,VALUE=glmfiles[i])]
        scrap1 = [scrap1,*ifh[i].regfiles]
    endfor
    scrap = scrap[1:*]
    scrap1 = scrap1[1:*]
    regfilesselect = get_str(total(ifh[*].nregfiles),scrap,scrap1,TITLE='Please check region files.',/ONE_COLUMN,/BELOW, $
        SPECIAL='Load region files.',/GO_BACK)
    if regfilesselect[0] eq 'GO_BACK' then goto,goback99 
    if regfilesselect[0] eq 'SPECIAL' then begin
        goback1001:
        gf = get_files(TITLE='Load region files. Please enter filter.',FILTER='*.conc')
        if gf.msg eq 'GO_BACK' then goto,goback991 else if gf.msg eq 'EXIT' then return,rtn={msg:'EXIT'} 
        regfiles = gf.files
        if n_elements(regfiles) lt total_nsubjects then begin
            scrap1 = strjoin(regfiles,string(10B),/SINGLE)
            scrap = get_button(['ok','go back','exit'],BASE_TITLE='Please select.',TITLE=strjoin(regfiles,string(10B),/SINGLE) $
                +string(10B)+'All GLMs will use these region files.')
            if scrap eq 1 then goto,goback1001 else if scrap eq 2 then return,rtn={msg:'EXIT'} 
            regfilesselect = ''
            for i=0,total_nsubjects-1 do regfilesselect = [regfilesselect,regfiles]
            regfilesselect = regfilesselect[1:*] 
        endif else if n_elements(regfiles) eq 1 and n_elements(glmfiles) eq 1 and gf.msg eq 'DONTCHECK' then begin 
            regfilesselect = regfiles
        endif else $
            regfilesselect = match_files(glmfiles,regfiles,TITLE='Please select region file for ')
    endif
    ifhnregfiles = ifh[*].nregfiles
    ii = 0
    for i=0,total_nsubjects-1 do begin
        rtn = get_regions(fi,wd,dsp,help,glmfiles[i],regfilesselect[ii:ii+ifhnregfiles[i]-1])
        if rtn.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rtn.msg eq 'GO_BACK' then goto,goback991
        if ifh[i].mmppix[0] ne rtn.space then begin
            print,'glm space=',glm_space[i],' region space=',rtn.space
            stat=dialog_message('Space incompatibility! Abort! Abort! Abort!',/ERROR)
            return,rtn={msg:'EXIT'}
        endif
        glmregionnames = *ifh[i].region_names
        index = intarr(ifh[i].nreg)
        for j=0,ifh[i].nreg-1 do begin
            index[j]=where(strmatch(rtn.region_names,glmregionnames[j]) eq 1,count)
            if count ne 1 then begin
                if count eq 0 then begin
                    scrap = glmregionnames[j]+' is not present in'+string(10B)+string(10B) $
                    +strjoin(regfilesselect[ii:ii+ifhnregfiles[i]-1],string(10B),/SINGLE)+string(10B)+string(10B) $
                    +'Please load different region files'
                endif else begin
                    scrap = glmregionnames[j]+' is present '+strtrim(count,2)+' times in'+string(10B)+string(10B) $
                    +strjoin(regfilesselect[ii:ii+ifhnregfiles[i]-1],string(10B),/SINGLE)+string(10B)+string(10B) $
                    +'Please load different region files'
                endelse
                stat=dialog_message(scrap,/ERROR)
                goto,goback1001
            endif
        endfor
        roi_str[i] = ' -regions_of_interest ' + strjoin(strtrim(index+1,2),' ',/SINGLE)
        ii = ii + ifhnregfiles[i]
    endfor
    nreg = ifhnreg[0]
    region_names = *ifh[0].region_names 
endif 
rev = ifh[*].glm_rev
index = where(rev gt -25,count)
if count eq 0 then rev25=1 else rev25=0
goback101:
fwhm = ifh[*].glm_fwhm

;if rev25 eq 0 then begin
;START160106
if rev25 eq 0 and not keyword_set(DONTASKSMOOTH) then begin

    index = where(fwhm gt 0.,count)
    if count ne 0 then begin
        if total(fwhm - fwhm[0]) ne 0. then begin
            print,'GLMs not all smoothed the same.'
            print,'    Could be different amounts of smoothing or GLMs made before this parameter was saved.'
            scrap = 2.
        endif else $
            scrap = fwhm[0]
    endif else $
        scrap = 0
    scrap = float(get_str(1,'FWHM in voxels ',strtrim(scrap,2),WIDTH='50',TITLE='Have your GLMs been smoothed?', $
        LABEL='Enter 0 if they have not been smoothed.'))
    fwhm[*] = float(scrap[0])
endif
fwhm_str = make_array(total_nsubjects,/STRING,VALUE='')
index = where(fwhm gt 0.,count)
if count ne 0 then fwhm_str[index] = ' -gauss_smoth ' + string(fwhm[index],FORMAT='(f3.1)')
goback102:
if no_ac_cc gt 0 or keyword_set(GET_GLMS_STRUCT) then $
    scrap = 0 $
else begin
    scrap = get_button(['compute residuals','check model fit','compute temporal covariance matrices','go back','exit'], $
        BASE_TITLE='Please select.',TITLE='I want to')
endelse
if scrap eq 0 or scrap eq 2 then begin
    lc_resid = 1
    if scrap eq 2 then begin
        lc_all = 1
        lc_tempcov = 1
    endif
endif else if scrap eq 1 then begin
    lc_resid = 0
    lc_retain = 0
    option_str = option_str + ' -store_model_fit'
    scrap = get_button(['yes','no','go back','exit'],BASE_TITLE='Please select.', $
        TITLE='Do you want to check the model fit over all estimated parameters?')
    if scrap eq 2 then goto,goback102 else if scrap eq 3 then return,rtn={msg:'EXIT'}
    if scrap eq 0 then lc_all = 1
endif
if lc_all eq 0 then begin
    if lc_resid eq 1 then begin 
        title = 'Please select effects to retain in residuals.'
        if no_ac_cc gt 0 then title = title + string(10B) + 'Do not retain trend or baseline.'
    endif else $
        title = 'Please select effects for model fit.'
    goback3:
    scraparr = strarr(max(effect_length)+2,n_elements(effect_label))
    for i=0,n_elements(effect_label)-1 do $
        scraparr[0:effect_length[i]+1,i] = [strtrim(indgen(effect_length[i])+1,2),'ALL','and by trial']
    widget_control,/HOURGLASS
    rtn = get_choice_list_many(effect_label,scraparr,TITLE=title,/LABEL_TOP,/NONEXCLUSIVE,/GO_BACK,LENGTHS=effect_length+2, $
        SET_BUTTONS=n_elements(effect_label))
    if rtn.special eq 'GO_BACK' then begin
        if keyword_set(GET_GLMS_STRUCT) then return,rtn={msg:'GO_BACK'} else goto,goback102
    endif
    scraplist = rtn.list
    andbytrial = intarr(n_elements(effect_label))
    for i=0,n_elements(effect_label)-1 do begin
        if scraplist[effect_length[i],i] eq 1 then begin
            scraplist[0:effect_length[i]-1,i] = 1
            scraplist[effect_length[i],i] = 0
        endif
        if scraplist[effect_length[i]+1,i] eq 1 then begin
            andbytrial[i] = 1
            scraplist[effect_length[i]+1,i] = 0
        endif
    endfor
    nframes = total(scraplist,1)
    scraplist_index = where(nframes ge 1,ntc)
    if ntc ne 0 then begin
        nframes = nframes[scraplist_index]
        conditions_time = intarr(ntc,max(nframes))
        for i=0,ntc-1 do begin
            index = where(scraplist[0:effect_length[scraplist_index[i]]-1,scraplist_index[i]],count)
            if count eq 0 then begin
                stat = dialog_message('Something is wrong hereA.',/ERROR)
                return,rtn={msg:'EXIT'}
            endif
            conditions_time[i,0:nframes[i]-1] = index + 1
        endfor
        goback8:
        search_str = strarr(ntc,10)
        for i=0,ntc-1 do search_str[i,0] = effect_label[scraplist_index[i]]
        sumrows = make_array(ntc,/INT,VALUE=1)


        sum_contrast_for_treatment = intarr(total_nsubjects,ntc,1)
        labels = replicate(ptr_new(),total_nsubjects)

        ;for m=0,total_nsubjects-1 do labels[m] = ifh[m].glm_effect_label
        ;START160303
        if n_elements(ifh) gt 1 then $
            for m=0,total_nsubjects-1 do labels[m] = ifh[m].glm_effect_label $
        else $
            for m=0,total_nsubjects-1 do labels[m] = ifh[0].glm_effect_label

        if get_cft_and_scft(sumrows,search_str,sum_contrast_for_treatment,0,total_nsubjects,glmfiles,ntc,labels,0,0,1) eq -1 then begin
            stat = dialog_message('Since you want to do this manually, you will need to restart the program and do the GLMs ' $
                +'one by one.',/INFORMATION)
            return,rtn={msg:'EXIT'}
        endif
        iscrap = where(andbytrial[scraplist_index] eq 0,nscrap)
        if nscrap ne 0 then begin

            ;START160303
            effect_length=*ifh[0].glm_effect_length

            for m=0,total_nsubjects-1 do begin

                ;effect_length = *ifh[m].glm_effect_length
                ;START160303
                if n_elements(ifh) gt 1 then effect_length = *ifh[m].glm_effect_length

                cstr = ' -retain_estimates'
                for i=0,nscrap-1 do begin
                    if sum_contrast_for_treatment[m,iscrap[i]] gt 0 then begin
                        for k=0,effect_length[sum_contrast_for_treatment[m,iscrap[i]]-1,0]-1 do begin
                               rtn=get_cstr_new2(cstr,sum_contrast_for_treatment[m,iscrap[i],0],effect_length, $
                                conditions_time[iscrap[i],k],1,' ') ;This must be a space. ' '
                        endfor
                    endif
                endfor
                retain_estimates_str[m] = cstr
            endfor
        endif
        iscrap = where(andbytrial[scraplist_index] eq 1,nscrap)
        if nscrap ne 0 then begin
            scraparr = make_array(nscrap,/INT,VALUE=1)
            for m=0,total_nsubjects-1 do begin
                effect_length = *ifh[m].glm_effect_length
                cstr = ' -tc'
                for i=0,nscrap-1 do begin
                    spacechar = ' '
                    for k=0,nframes[iscrap[i]]-1 do begin
                        get_cstr_new,cstr,sum_contrast_for_treatment[m,iscrap[i],0],effect_length,conditions_time[iscrap[i],k],1, $
                            spacechar ;This must be a space. ' '
                        spacechar = ','
                    endfor
                endfor
                tc_str[m] = cstr
            endfor
        endif
    endif
endif
goback:
if lc_tempcov eq 1 then begin
    analysis_type = 0
    loop = 1
    scrap = 'temporal covarinace computation'
    goto,gotempcov0
endif
atlas_space = glm_space[0]

;if keyword_set(MVPA) then begin
;    if keyword_set(FIDL_TC_SS) then darby='voxel' else darby='searchlight'
;    analysis_type = get_button([darby,'regional','go back','exit'],BASE_TITLE='Analysis type',WIDTH=55,TITLE='Please select')
;    if analysis_type eq 3 then return,rtn={msg:'EXIT'} else if analysis_type eq 2 then goto,goback3
;START151130
if keyword_set(FIDL_TC_SS) then begin

    ;analysis_type = get_button(['voxel','regional','go back','exit'],BASE_TITLE='Analysis type',WIDTH=55,TITLE='Please select')
    ;START161117
    analysis_type=get_button(['voxel','regional','go back','exit'],BASE_TITLE='Please select analysis type', $
        TITLE='If your ultimate goal is to extract subject averaged regional means and varianaces,'+string(10B) $
        +' then select "voxel" and input the maps into "Average Statistics".') 


    if analysis_type eq 3 then return,rtn={msg:'EXIT'} else if analysis_type eq 2 then goto,goback3
endif else if keyword_set(MVPA) then begin
    analysis_type = get_button(['searchlight','regional','go back','exit'],BASE_TITLE='Analysis type',WIDTH=55,TITLE='Please select')
    if analysis_type eq 3 then return,rtn={msg:'EXIT'} else if analysis_type eq 2 then goto,goback3

endif else if no_ac_cc eq 4  then begin
    analysis_type = 1
endif else if ifhnreg[0] gt 0 or keyword_set(GET_GLMS_STRUCT) then begin 
    analysis_type = 0
endif else begin
    goback35:
    goose = ['voxel by voxel','specfic regions','specific regions by subject - one region file per subject', $
        'specific regions by subject - more than one region file per subject','go back']
    scrap = 0
    analysis_type = get_button(goose[scrap:*],TITLE='Please select analysis type.')
    if analysis_type eq n_elements(goose)-1-scrap then goto,goback3
    analysis_type = analysis_type + scrap
    if analysis_type eq 0 then begin
        if no_ac_cc eq 2 then begin
            lcseedreg=1
            no_ac_cc_str = ''
            nlags = get_str(1,'How many lags?',strtrim(min(framesperrun)-2,2),TITLE='Total number of points = 2 x nlags + 1', $
                LABEL='max = dim4-skip-2',WIDTH=50,/GO_BACK)
            if nlags[0] eq 'GO_BACK' then goto,goback35
            nlagsstr = ' -nlags '+strtrim(nlags[0],2)
        endif else if no_ac_cc eq 3 then begin
            no_ac_cc_str=''
            lcscratch=1
            lizard=['Pooled standard deviation: Variance computed for each run. Runs weighted by number of frames.', $
                'Single variance computed across all runs']
            rtn=select_files(lizard,TITLE='Variance options',BASE_TITLE='Please select',/GO_BACK,/EXIT)
            if rtn.files[0] eq 'EXIT' then return,rtn={msg:'EXIT'} else if rtn.files[0] eq 'GO_BACK' then goto,goback35
            if rtn.list[0] eq 1 then no_ac_cc_str = ' -var' else lcscratch=0
            if rtn.list[1] eq 1 then no_ac_cc_str = no_ac_cc_str+' -varall'
        endif
    endif
endelse
goback4:
if analysis_type ge 1 or lcseedreg eq 1 then begin
    if lcseedreg eq 1 then begin 
        get_regions_title = 'Please select seed files' 
        title = 'Please select seeds.
    endif else begin 
        get_regions_title = 0
        title = 'Please select regions you wish to analyze'
    endelse
    if analysis_type eq 1 or lcseedreg eq 1 then begin
        loop = 1 
        scrap = ''
    endif else begin
        loop = total_nsubjects
        scrap = glmfiles
    endelse
    gotempcov0:
    nreg = intarr(total_nsubjects)
    nregion_files = intarr(total_nsubjects)
    if analysis_type eq 1 or analysis_type eq 3 or lc_tempcov eq 1 or lcseedreg eq 1 then begin
        region_names = ''
        region_files = ''
        for i=0,loop-1 do begin
            rtn = get_regions(fi,wd,dsp,help,scrap[i],TITLE=get_regions_title)
            if rtn.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rtn.msg eq 'GO_BACK' then goto,goback
            region_space = rtn.space
            region_files = [region_files,rtn.region_file]
            nregion_files[i] = rtn.nregfiles
            region_str[i] = rtn.region_str
            rtn = select_files(rtn.region_names,TITLE=title)
            roi_str[i] = ' -regions_of_interest ' + strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)
            nreg[i] = rtn.count
            region_names = [region_names,rtn.files]
            if glm_space[0] ne region_space then begin
                if glm_space[0] eq !SPACE_DATA and (region_space eq !SPACE_111 or region_space eq !SPACE_222 or $
                    region_space eq !SPACE_333) then begin
                    if t4select[0] eq '' then begin
                        gf = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_*_t4')
                        if gf.msg eq 'GO_BACK' then goto,goback4 else if gf.msg eq 'EXIT' then return,rtn={msg:'EXIT'}
                        t4files = gf.files
                        if total_nsubjects eq 1 and n_elements(t4files) eq 1 and gf.msg eq 'DONTCHECK' then $
                            t4select = t4files $
                        else $
                            t4select = match_files(glmfiles,t4files,TITLE='Please select t4 file for ')
                    endif
                endif else begin
                    stat=dialog_message('Space incompatibility! Abort! Abort! Abort!',/ERROR)
                    return,rtn={msg:'EXIT'}
                endelse
            endif
        endfor
        region_names = region_names[1:*]
        region_files = region_files[1:*]
        if loop eq 1 and total_nsubjects gt 1 then begin
            nreg[1:*] = nreg[0]
            scrap = region_names
            for i=1,total_nsubjects-1 do region_names = [region_names,scrap]
            region_str[*] = region_str[0]
            roi_str[*] = roi_str[0]
            nregion_files[*] = nregion_files[0]
        endif
    endif else begin
        gf = get_files(TITLE='Need region files. Please enter filter.',FILTER='*_anat_ave_seg.4dfp.img')
        if gf.msg eq 'GO_BACK' then goto,goback4 else if gf.msg eq 'EXIT' then return,rtn={msg:'EXIT'}
        regionfiles = gf.files
        if total_nsubjects eq 1 and n_elements(regionfiles) eq 1 and gf.msg eq 'DONTCHECK' then $
            region_files = regionfiles $
        else $
            region_files = match_files(glmfiles,regionfiles,TITLE='Please select region file for ')
        rtn = get_regions(fi,wd,dsp,help,'',region_files[0])
        if rtn.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rtn.msg eq 'GO_BACK' then goto,goback
        region_space = rtn.space
        region_names = rtn.region_names
        rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.')
        nreg[*] = rtn.count
        sumrows = make_array(nreg[0],/INT,VALUE=1)
        index_conditions = rtn.index
        scraplabels = replicate(ptr_new(),total_nsubjects)
        if region_names[0] ne 'CSF' then begin
            widget_control,/HOURGLASS
            for m=0,total_nsubjects-1 do begin
                ifh = read_mri_ifh(region_files[i])
                scraplabels[m] = ptr_new(ifh.region_names)
            endfor
        endif else begin
            for m=0,total_nsubjects-1 do scraplabels[m] = ptr_new(region_names) 
            option_str = option_str + ' -avi_seg'
        endelse 
        scrap=cft_and_scft(total_nsubjects,nreg[0],1,sumrows,index_conditions,rtn.files,0,1,region_files,rtn.files,0,'',scraplabels)
        for m=0,total_nsubjects-1 do roi_str[m] = ' -regions_of_interest ' $
            + strjoin(strtrim(scrap.sum_contrast_for_treatment[m,*,0],2),' ',/SINGLE)
        region_str = ' -region_file ' + region_files 
    endelse
    atlas_space = region_space
    goback2:
    if no_ac_cc gt 0 then begin 
        if keyword_set(PCA) and nreg[0] gt 1 then begin
            regtype = get_button(['regional timecourses','voxel timecourses','go back'],TITLE='I want to perform PCA on')
            if regtype eq 2 then goto,goback4
            if regtype eq 0 then pca_str = '' else if regtype eq 1 then pca_str = ' -voxels'
        endif
        regtype = 0
    endif else if lc_retain eq 1 and lc_tempcov eq 0 then begin
        if keyword_set(MVPA) then $
            regtype = 1 $
        else $
            regtype = get_button(['regional residuals','residuals for all voxels within the regions', $
                'residuals for all voxels within the regions with PCA','go back'],TITLE='I want')
        if regtype eq 3 then goto,goback4
        if regtype ge 1 then pca_str = ' -voxels'
        if keyword_set(MVPA) then pca_str = pca_str + ' ALL'
        if regtype eq 2 then pca_str = pca_str + ' -pca'
    endif
endif
goback20:
lcmask=0
mask=make_array(total_nsubjects,/STRING,VALUE='')
mask_str=make_array(total_nsubjects,/STRING,VALUE='')
if analysis_type eq 0 or lcseedreg eq 1 then begin
    idx=where(ptr_valid(ifh[*].glm_mask_file),cnt)
    if cnt ne 0 then begin

        ;for i=0,cnt-1 do begin 
        ;    mask[idx[i]] = *ifh[idx[i]].glm_mask_file
        ;    mask_str[idx[i]] = ' -mask '+*ifh[idx[i]].glm_mask_file
        ;endfor
        ;START160107
        if n_elements(ifh) gt 1 then begin
            for i=0,cnt-1 do begin 
                mask[idx[i]] = *ifh[idx[i]].glm_mask_file
                mask_str[idx[i]] = ' -mask '+*ifh[idx[i]].glm_mask_file
            endfor
        endif else begin
            mask[*]=*ifh[idx[0]].glm_mask_file
            mask_str[*]=' -mask '+*ifh[idx[0]].glm_mask_file
        endelse


    endif else begin
        lcmask=1
        lizard = get_mask(atlas_space,fi)
        if lizard eq 'GO_BACK' then goto,goback4
        if lizard ne '' then begin
            mask[*] = lizard
            mask_str[*] = ' -mask '+lizard
        endif
    endelse
endif
if t4select[0] ne '' then begin
    if atlas_space eq !SPACE_111 then begin
        atlas_str = ' -atlas 111'
    endif else if atlas_space eq !SPACE_222 then begin
        atlas_str = ' -atlas 222'
    endif else if atlas_space eq !SPACE_333 then begin
        atlas_str = ' -atlas 333'
    endif else begin
        scrap = get_button(['111','222','333','data space','go back'],TITLE='Please select')
        if scrap eq 4 then begin
            if lcmask eq 1 then goto,goback20 else goto,goback
        endif
        if scrap eq 0 then  $
            atlas_str = ' -atlas 111' $
        else if scrap eq 1 then  $
            atlas_str = ' -atlas 222' $
        else if scrap eq 2 then  $
            atlas_str = ' -atlas 333' $
        else $ 
            t4select=''
    endelse
endif
goback5:
if lc_retain eq 1 and (no_ac_cc eq 0 or keyword_set(COV) or keyword_set(CROSSCOV) or keyword_set(PCA)) then begin

    ;if keyword_set(MVPA) then $
    ;START151130
    if keyword_set(MVPA) or keyword_set(FIDL_TC_SS) then $

        option_str = option_str + ' -percent_change' $
    else begin

        ;scrap = get_button(['percent change','MR units','go back','exit'],BASE_TITLE='Please select.',TITLE='Residuals should be in')
        ;START160107
        scrap = get_button(['percent change (scaled to the intercept)','MR units (unscaled)','go back','exit'], $
            BASE_TITLE='Please select.',TITLE='Residuals should be in')

        if scrap eq 2 then goto,goback4 else if scrap eq 3 then return,rtn={msg:'EXIT'} 
        if scrap eq 0 then begin
            option_str = option_str + ' -percent_change'
            if keyword_set(PCA) then fidlpca_str = fidlpca_str + ' -percent_change'
        endif
    endelse
endif
if no_ac_cc eq 3 or no_ac_cc eq 4 then begin
    selectframes = 1
endif else if lc_tempcov eq 1 or (no_ac_cc gt 0 and lcseedreg eq 0) then begin
    scrap = get_button(['yes','no'],TITLE='Do you want the standard error of the mean?',BASE_TITLE='Output options')
    if scrap eq 0 then fidl_ac_optionstr = fidl_ac_optionstr + ' -sem'
    selectframes = 1
endif else begin
    goback2a:
    if lcseedreg eq 1 then $
        selectframes = 1 $
    else if keyword_set(GET_GLMS_STRUCT) then $
        selectframes=0 $
    else $
        selectframes = get_button(['all frames.','all valid frames.','to manually specify frames.', $
            'to select frames for specific conditions.'],TITLE='I want',BASE_TITLE='Output options')
endelse
print,'selectframes=',selectframes
if selectframes eq 0 then begin
    if n_elements(andbytrial) gt 0 then begin
        iandbytrial = where(andbytrial ne 0,nandbytrial)
        if nandbytrial ne 0 then begin
            goback0:
            gf = get_files(FILTER='*fi??',TITLE='Please enter event file filter.', $
                EXTRALABEL='These event files should specify which trials to retain.')
            if gf.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if gf.msg eq 'GO_BACK' then goto,goback5
            evselect_andbytrial = gf.files
            goback1:
            if n_elements(concselect) eq 1 and n_elements(evselect_andbytrial) eq 1 and gf.msg eq 'DONTCHCECK' then begin
                ;do nothing
            endif else begin
                evselect_andbytrial = match_files(concselect,evselect_andbytrial,TITLE='Please select event file for ',/GO_BACK)
                if evselect_andbytrial[0] eq 'GO_BACK' then goto,goback0
            endelse
            sng = define_single_trial(fi,pref,help,EVENT_FILE=evselect_andbytrial[0])
            rtn_ic = identify_conditions_new(sng.num_levels,*sng.factor_labels,nandbytrial,effect_label[iandbytrial],0,1, $
                'Multiple selections are permitted.',TOP_TITLE='Please select a trial type for each condition.')
            if rtn_ic.goback eq !TRUE then goto,goback1
            sumrows_andbytrial = rtn_ic.sumrows
            ic_andbytrial = rtn_ic.index_conditions
            nc_andbytrial = total(sumrows_andbytrial)
            junk1 = strpos(evselect_andbytrial,'/',/REVERSE_SEARCH)+1
            junk2 = strpos(evselect_andbytrial,'.',/REVERSE_SEARCH)-junk1
            for m=0,total_nsubjects-1 do bf_str[m]=strmid(evselect_andbytrial[m],junk1[m],junk2[m])+'_boldframes.dat'
            nframes_iandbytrial = nframes[iandbytrial]
            sumrows_time = make_array(nandbytrial,max(nframes_iandbytrial),/INT,VALUE=1)
            index_time = conditions_time[iandbytrial,*] - 1
        endif
    endif
endif else if selectframes eq 1 then begin
    option_str = option_str + ' -skip'
    if lc_tempcov eq 0 and no_ac_cc eq 0 then begin 
        scrap = get_button(['yes','no'],TITLE='Do you wish to average across runs?' )
        if scrap eq 0 then option_str = option_str+' -average_across_runs'
    endif
endif else if selectframes eq 2 then begin
    if dialog_message('The same set of frames will be selected from each subject.'+string(10B)+string(10B)+'If this is ' $
        +'not what you want, then hit cancel and restart the program, loading only a single GLM.',/CANCEL) eq 'Cancel' then $
        return,rtn={msg:'EXIT'}
    valid_frms = make_array(ifh[0].glm_tdim,/FLOAT,VALUE=1)
    widget_control,/HOURGLASS
    rload_conc=load_conc(fi,stc,dsp,help,wd,concselect[0],/DONT_PUT_IMAGE)
    if rload_conc.msg eq 'GOBACK' then goto,goback5 else if rload_conc.msg eq 'EXIT' then return,rtn={msg:'EXIT'} 
    stc1=rload_conc.stc1
    rtn = get_frames(ifh[0].glm_tdim,ifh[0].glm_n_files,*stc1.tdim_file,max(*ifh[0].glm_effect_length),valid_frms,/KEEP)
    index = where(valid_frms eq 0,count)
    if count eq 0 then begin
        stat=dialog_message('No frames given! Abort! Abort! Abort!',/ERROR)
        return,rtn={msg:'EXIT'}
    endif
    stat = strtrim(index+1,2)
    print_frames_str[0] = ' -print_frames'
    for i=0,count-1 do print_frames_str[0] = print_frames_str[0] + ' ' + stat[i]
    for m=1,total_nsubjects-1 do print_frames_str[m] = print_frames_str[0]
endif else if selectframes eq 3 then begin
    if event_files[0] eq '' then begin
        event_files = strarr(total_nsubjects)
        for m=0,total_nsubjects-1 do event_files[m] = *ifh[m].glm_event_file
    endif
    event_files = strtrim(get_str(total_nsubjects,glmfiles,event_files,TITLE='Please check event files.',/ONE_COLUMN,/BELOW),2)
    sng = define_single_trial(fi,pref,help,EVENT_FILE=event_files[0])
    if sng.name eq 'ERROR' then return,rtn={msg:'EXIT'} 
    rtn = get_bool_list(*sng.factor_labels,TITLE='Please select conditions whose frames are to be provided at output.')
    scraplist = rtn.list
    scraplist_index = where(scraplist eq 1,ntc)
    labels = replicate(ptr_new(),total_nsubjects)
    for m=0,total_nsubjects-1 do labels[m] = ifh[m].glm_effect_label
    goose = cft_and_scft(total_nsubjects,ntc,1,sumrows,scraplist_index,effect_label,get_labels_struct,0,glmfiles, $
        treatment_str,nfactors,dummy,labels)
    sum_contrast_for_treatment = goose.sum_contrast_for_treatment
    sum_contrast_for_treatment = sum_contrast_for_treatment - 1
    scrap = get_button(['yes','no'],TITLE='Do you wish to average the condition across runs?' )
    if scrap eq 0 then $
        racar_str = ' -regional_avg_cond_across_runs' $
    else if analysis_type eq 0 then $
        option_str = option_str + ' -onefile'
    goback_add_frames:
    add_frames = fix(get_str(2,['Number of frames preceeding condition','Number of frames following condition'],['0','0'], $
        WIDTH=20,TITLE='Please specify additional frames for output.'))
    for m=0,total_nsubjects-1 do begin
        sng = define_single_trial(fi,pref,help,EVENT_FILE=event_files[m])
        widget_control,/HOURGLASS
        rload_conc=load_conc(fi,stc,dsp,help,wd,concselect[m],/DONT_PUT_IMAGE)
        if rload_conc.msg eq 'GOBACK' then goto,goback_add_frames else if rload_conc.msg eq 'EXIT' then return,rtn={msg:'EXIT'}
        stc1=rload_conc.stc1
        tdim_file = *stc1.tdim_file
        nrun = stc1.n
        tdim_sum = *stc1.tdim_sum
        tdim_sum = [tdim_sum,tdim_sum[nrun-1]+tdim_file[nrun-1]]
        frames_all = -1
        endframe_all = -1
        if ifh[m].glm_tot_eff gt sng.num_levels then begin ;block design
            print,'block design'
            sng_frames = *sng.frames_block
            stimlen = round(*sng.stimlen_vs_t) ;eliminates decimals
        endif else begin
            print,'event related design'
            sng_frames = *sng.frames
            sng_frames = sng_frames[*,0]
            stimlen = round(*sng.stimlen_vs_t/sng.TR)
        endelse
        for i=0,ntc-1 do begin
            index = where(*sng.conditions eq sum_contrast_for_treatment[m,i,0],count)
            frames = sng_frames[index]
            endframe = frames + stimlen[index] - 1 + add_frames[1]
            index = where(frames lt tdim_sum[nrun],count)
            if count eq 0 then begin
                stat=dialog_message('No frames lt tdim_sum[nrun]. Abort!',/ERROR)
                return,rtn={msg:'EXIT'}
            endif
            j = 0
            for k=0,count-1 do begin
                while frames[k] ge tdim_sum[j] do j = j + 1
                frames[k] = frames[k] - add_frames[0]
                if frames[k] lt tdim_sum[j-1] then begin
                    stat=dialog_message('Adding frames '+strtrim(add_frames[0],2)+' before the condition pulls it into ' $
                        +'the preceeding run. This is no good. Please try again.',/ERROR)
                    goto,goback_add_frames
                endif
                endframe[k] = endframe[k] < tdim_sum[j]
            endfor
            frames_all = [frames_all,frames]
            endframe_all = [endframe_all,endframe]
        endfor
        frames = frames_all[1:*]
        endframe = endframe_all[1:*]
        widget_control,/HOURGLASS
        print_frames_str[m] = ' -start ' + strjoin(strtrim(frames+1,2),' ',/SINGLE) + ' -length ' $
            + strjoin(strtrim(endframe-frames+1,2),' ',/SINGLE)
    endfor
endif
proc=1
proceach=1
wallace0=''
if total_nsubjects gt 1 and not keyword_set(AUTOCORR) and not keyword_set(CROSSCORR) and not keyword_set(CROSSCOV) and not $
    keyword_set(COV) and not keyword_set(PCA) and not keyword_set(MVPA) and not keyword_set(FIDL_TC_SS) and not $
    keyword_set(GET_GLMS_STRUCT) then begin

    ;spawn,'cat /proc/cpuinfo | grep processor | wc -l',processor
    ;print,'processor=',processor
    ;proc=(processor/2)>1
    ;proc=proc[0]
    ;proceach=ceil(float(total_nsubjects)/proc)
    ;pint,'proc='+trim(proc)+' proceach='+trim(proceach)+' glms will be assigned to each processor'
    ;wallace0=trim(indgen(proc)+1)
    ;idx=where(strlen(wallace0) eq 1,cnt)
    ;if cnt ne 0 then wallace0[idx]='0'+wallace0[idx]
    ;START170712
    gp=getproc(pref,total_nsubjects)
    proc=gp.proc
    proceach=gp.proceach
    wallace0=gp.wallace0
    undefine,gp

endif
if n_elements(times0) eq 0 then begin
    times0=strarr(proc)
    for i=0,proc-1 do begin
        spawn,!BINEXECUTE+'/fidl_timestr2',timestr
        times0[i]=timestr
    endfor
endif
rtn_gr0 = get_root(glmfiles,'.glm')
glmroots = rtn_gr0.file
if keyword_set(GET_GLMS_STRUCT) then $
    outstr = ' -root "' + glmroots + '_res' + '"' $
else begin
    glmrootspath = rtn_gr0.path
    directory = getenv('PWD')
    if proc eq 1 then begin
        scrap = ['script']
        if no_ac_cc eq 0 then begin
            if lc_tempcov eq 0 then $
                scraplabels = 'compute_residuals.csh' $
            else $
                scraplabels = 'compute_residtempcov.csh'
        endif else if no_ac_cc eq 1 then $
            scraplabels = 'fidl_ac.csh' $
        else if no_ac_cc eq 2 then $
            scraplabels = 'fidl_crosscorr.csh' $
        else $
            scraplabels = 'fidl_cov.csh'
    endif else begin
        scrap='script'+wallace0
        scraplabels='compute_residuals_'+times0+'.csh' 
    endelse
    if load eq 1 or n_elements(concfiles) gt 0 then begin
        scrap = [scrap,'list']
        if glm_list_str[0] eq '' then $
            scraplabels = [scraplabels,'list.list'] $
        else begin
            scraplabels = [scraplabels,glm_list_str]
        endelse
    endif
    outstr = glmroots
    if no_ac_cc eq 0 then outstr = glmrootspath + '/'+glmroots
    if lc_retain eq 0 then outstr = outstr + '_modfit' else outstr = outstr + '_res'
    if analysis_type eq 0 then begin
        scrap1 = 'output root   ' + glmroots
        scrap = [scrap,scrap1]
        scraplabels = [scraplabels,outstr]
    endif else if no_ac_cc eq 0 then begin
        s1 = 'output '+ glmroots
        if regtype eq 0 then begin
            scrap = [scrap,s1]
            scrap1 = outstr + '.txt'
            scraplabels = [scraplabels,scrap1]
        endif else begin
            j = 0
            k = 0
            widget_control,/HOURGLASS
            for i=0,total_nsubjects-1 do begin
                l = indgen(nreg[i])
                m = l + j
                n = l + k
                scrap1 = s1[i] + ' ' + region_names[n]
                scrap = [scrap,scrap1]
                scrap1 = outstr[i] + '_' + region_names[n] + '.txt'
                scraplabels = [scraplabels,scrap1]
                j = j + nreg[i]
                k = k + nreg[i]
            endfor
        endelse
    endif
    if directory ne '' then front = directory+'/' else front = ''
    names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter filenames.',/ONE_COLUMN,/GO_BACK,FRONT=front, $
        /REPLACE)
    if names[0] eq 'GO_BACK' then begin
        if lc_tempcov eq 0 then goto,goback2a else goto,goback5
    endif
    script=strarr(proc)
    for j=0,proc-1 do script[j] = fix_script_name(names[j])
    if load eq 1 or n_elements(concfiles) gt 0 then begin
        glm_list_str = fix_script_name(names[j])
        j = j+1 
    endif
    if no_ac_cc eq 0 then begin
        if analysis_type eq 0 then begin
            roots = names[j:*]
            outstr = ' -root "' + roots + '"'
            rtn_gr = get_root(roots)
            roots = rtn_gr.file
        endif else begin
            if regtype eq 0 then $
                outstr = ' -regional_residual_name "' + names[j:*] + '"' $
            else begin
                scrap = '"' + names[j:*] + '"'
                k = 0
                for i=0,total_nsubjects-1 do begin
                    l = indgen(nreg[i]) + k
                    outstr[i] = ' -regional_residual_name ' + strjoin(scrap[l],' ',/SINGLE)
                    k = k + nreg[i]
                endfor
            endelse
        endelse
    endif else $
        outstr = ' -root "' + outstr + '"'
endelse
if load eq 1 or n_elements(concfiles) gt 0 and glm_list_str ne '' then begin
    if event_files[0] eq '' then begin
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










if no_ac_cc gt 0 or keyword_set(GET_GLMS_STRUCT) then acconc = strarr(total_nsubjects)
t4str = make_array(total_nsubjects,/STRING,VALUE='')
if t4select[0] ne '' then for i=0,total_nsubjects-1 do t4str[i] = ' -xform_file ' + t4select[i]
ir=0
ii=0
if no_ac_cc gt 0 then is=replicate({Identify_struct},total_nsubjects)
if ifhnreg[0] gt 0 then regstr = ' $REGION_FILE $ROI' else regstr=''
if keyword_set(GET_GLMS_STRUCT) then cmdstr = strarr(total_nsubjects) 
if (keyword_set(MVPA) or no_ac_cc gt 0 or keyword_set(GET_GLMS_STRUCT)) and lcscratch eq 1 then begin
    idx=where(tag_names(get_glms_struct) eq 'SCRATCHDIR',cnt) 
    if cnt ne 0 then $
        scratchdir=get_glms_struct.scratchdir $
    else begin

        ;scratchdir=fidl_scratchdir()
        ;START170224
        scratchdir=fidl_scratchdir(/NOPATH)

        if scratchdir eq 'ERROR' then return,rtn={msg:'EXIT'}
    endelse
    scratchdir_str = ' -scratchdir '+ scratchdir
endif

;START160816
if not keyword_set(GET_GLMS_STRUCT) then begin
    action = get_button(['execute','return'],TITLE='Please select action.')
endif
proceach0=0
proceach1=0

widget_control,/HOURGLASS
for i=0,total_nsubjects-1 do begin

    ;START160816
    if not keyword_set(GET_GLMS_STRUCT) and proceach0 eq 0 then begin
        print,'i=',i,' proceach1=',proceach1
        print,'***************** OPENW '+script[proceach1]+' *****************'
        openw,lu,script[proceach1],/GET_LUN
        top_of_script,lu
        if lc_tempcov eq 1 then begin
            print_files_to_csh,lu,nregion_files[0],region_files,'REGION_FILE','region_file',/NO_NEWLINE
            printf,lu,'set ROI = ('+strtrim(roi_str[0],2)+')'
        endif
    endif

    if not keyword_set(DONTLOADCONC) then begin
        rload_conc=load_conc(fi,stc,dsp,help,wd,concselect[i],/DONT_PUT_IMAGE)
        if rload_conc.msg eq 'GOBACK' then goto,goback5 else if rload_conc.msg eq 'EXIT' then return,rtn={msg:'EXIT'}
        stc1=rload_conc.stc1
    endif
    boldconditions_str = ''
    if selectframes eq 0 then begin
        if nandbytrial ne 0 then begin
            sng = define_single_trial(fi,pref,help,EVENT_FILE=evselect_andbytrial[i])
            rload_conc=load_conc(fi,stc,dsp,help,wd,concselect[i],/DONT_PUT_IMAGE)
            if rload_conc.msg eq 'GOBACK' then goto,goback5 else if rload_conc.msg eq 'EXIT' then return,rtn={msg:'EXIT'}
            tdim_file = *stc[fi.n].tdim_file
            nrun = stc[fi.n].n
            tdim_sum = *stc[fi.n].tdim_sum
            rtn_gi = get_index(nandbytrial,nandbytrial,ic_andbytrial,sumrows_andbytrial,nc_andbytrial,sng)
            index = rtn_gi.index
            boldframes = rtn_gi.boldframes
            index_which_level = rtn_gi.index_which_level
            boldconditions = rtn_gi.boldconditions
            scrap = frames_ss(sng,nrun,tdim_sum)
            trialframes = scrap.trialframes[index]
            rtn_bf=print_boldframes(nframes_iandbytrial,sumrows_time,index_time,trialframes,boldframes,bf_str[i],index_which_level)
            boldconditions_index = rtn_bf.index
            nboldconditions_index = rtn_bf.count
            bf_str[i] = ' -frames "'+bf_str[i]+'"'
            index_which_level = strcompress(index_which_level[boldconditions_index])
            str = 'set FRAMES_CODE = (-frames_code'
            for m=0,nboldconditions_index-1 do str = str + index_which_level[m]
            str = str + ')'
            boldconditions_str = str
        endif
    endif
    if not keyword_set(GET_GLMS_STRUCT) then begin 
        print_files_to_csh,lu,1,glmfiles[i],'GLM_FILE','glm_file',/NO_NEWLINE
        glmstr = ' $GLM_FILE'
    endif else $
        glmstr = ' -glm_file ' + glmfiles[i]
    if ifhnreg[0] gt 0 then begin
        print_files_to_csh,lu,ifhnregfiles[i],regfilesselect[ii:ii+ifhnregfiles[i]-1],'REGION_FILE','region_file',/NO_NEWLINE 
        printf,lu,'set ROI = ('+strtrim(roi_str[i],2)+')'
    endif
    widget_control,/HOURGLASS
    boldstr = ' -bold_files '+concselect[i]
    if boldconditions_str ne '' then begin
        printf,lu,boldconditions_str
        boldconditions_str = ' $FRAMES_CODE'
    endif
    identify_str=''
    if not keyword_set(DONTLOADCONC) then begin
        identify = *stc1.identify
        if identify[0] ne '' then identify_str = ' -identify '+strjoin(identify,' ',/SINGLE)
    endif
    if lc_tempcov eq 0 then begin 
        goose = t4str[i]+atlas_str+mask_str[i]
    endif else begin
        goose = ''
        spider = ''
        if bold_space[0] ne glm_space[0] then $ 
            goose = t4str[i]+atlas_str+mask_str[i] $
        else $ 
            spider = t4str[i]+atlas_str+mask_str[i]
    endelse

    ;cmd = '/compute_residuals'+glmstr+boldstr+regstr+print_frames_str[i]+retain_estimates_str[i]+option_str+goose+racar_str $
    ;    +outstr[i]+fwhm_str[i]+boldconditions_str+bf_str[i]+tc_str[i]+pca_str+globcovstr+identify_str+scratchdir_str+no_ac_cc_str $
    ;    +unbias_maxlik_str
    ;START160928
    cmd = '/fidl_residuals'+glmstr+boldstr+regstr+print_frames_str[i]+retain_estimates_str[i]+option_str+goose+racar_str $
        +outstr[i]+fwhm_str[i]+boldconditions_str+bf_str[i]+tc_str[i]+pca_str+globcovstr+identify_str+scratchdir_str+no_ac_cc_str $
        +unbias_maxlik_str

    if (no_ac_cc gt 0 or keyword_set(GET_GLMS_STRUCT)) and lcscratch eq 1 then begin
        spawn,!BINEXECUTE+'/fidl_timestr2',timestr
        scratchcsh = scratchdir+'fidl_scratch_'+timestr[0]+'.csh'
        openw,lu1,scratchcsh,/GET_LUN
        top_of_script,lu1
        if not keyword_set(GET_GLMS_STRUCT) then begin
            print_files_to_csh,lu1,1,glmfiles[i],'GLM_FILE','glm_file',/NO_NEWLINE
            if ifhnreg[0] gt 0 then begin
                print_files_to_csh,lu1,ifhnregfiles[i],regfilesselect[ii:ii+ifhnregfiles[i]-1],'REGION_FILE','region_file', $
                    /NO_NEWLINE
                printf,lu1,'set ROI = ('+strtrim(roi_str[i],2)+')'
            endif
            dummy = cmd
            if not keyword_set(COV) then dummy = dummy + ' -names_ifh'
        endif else begin
            if ifhnreg[0] gt 0 then $
                cmd = cmd + ' -region_file ' + strjoin(regfilesselect[ii:ii+ifhnregfiles[i]-1],' ',/SINGLE) + roi_str[i]
            cmd = cmd + ' -float'
            dummy = cmd
            if keyword_set(MVPA) then begin
                dummy = dummy + ' -names_ifh'
            endif
            if analysis_type gt 0 then begin
                print_files_to_csh,lu1,nregion_files[i],region_files[ir:ir+nregion_files[i]-1],'REGION_FILE','region_file', $
                    /NO_NEWLINE
                printf,lu1,'set ROI = ('+strtrim(roi_str[i],2)+')'
                dummy = dummy + ' $REGION_FILE $ROI'
                if analysis_type gt 1 then ir = ir + nregion_files[i]
            endif
        endelse

        ;printf,lu1,'$BIN' + dummy + ' -names_only'
        ;START160107
        cmd1='$BIN'+dummy+' -names_only'
        printf,lu1,cmd1

        close,lu1
        free_lun,lu1

        ;START160107
        print,cmd1
        print,''

        spawn,'chmod +x '+scratchcsh,result
        spawn,'csh '+scratchcsh,result

        if n_elements(result) ne 1 then begin
            stat = spawn_check_and_load(result,fi,wd,dsp,help,nfiles_loaded,files_loaded,stc)
            return,rtn={msg:'EXIT'}
        endif

        ;print,'compute_residuals here0 result=',result,'END'
        ;print,'compute_residuals here0 n_elements(result)=',n_elements(result)


        scraplabel = strsplit(result[0],/EXTRACT)
        acconc[i] = scraplabel[n_elements(scraplabel)-1]
        if not keyword_set(GET_GLMS_STRUCT) then begin
            if identify[0] ne '' then begin
                is[i] = get_identify(nfiles,identify)
                if n_tags(is[i]) eq 0 then return,rtn={msg:'EXIT'}
            endif
        endif
    endif
    if lc_tempcov eq 0 and analysis_type gt 0 then begin
        if not keyword_set(GET_GLMS_STRUCT) then begin
            print_files_to_csh,lu,nregion_files[i],region_files[ir:ir+nregion_files[i]-1],'REGION_FILE','region_file',/NO_NEWLINE
            printf,lu,'set ROI = ('+strtrim(roi_str[i],2)+')'
            if analysis_type gt 1 then ir = ir + nregion_files[i]
        endif
        cmd = cmd + ' $REGION_FILE $ROI'
    endif
    if not keyword_set(GET_GLMS_STRUCT) then begin
        printf,lu,'nice +19 $BIN' + cmd
        printf,lu,''
    endif else $
        cmdstr[i]='nice +19 $BIN'+cmd

    ;print,'here1 i=',i,' cmdstr=',cmdstr[i]

    if lc_tempcov eq 1 then begin
        print_files_to_csh,lu,nfiles,roots[i]+'_b'+strtrim(indgen(nfiles)+1,2)+'.4dfp.img','BOLD_FILES','bold_files',/NO_NEWLINE
        printf,lu,'nice +19 $BIN/compute_temporal_cov_111 $BOLD_FILES $REGION_FILE $ROI'+spider $
            +' -conc '+roots[i]+'_temporalcov.conc'
        printf,lu,'rm resid/*'
    endif
    if ifhnreg[0] gt 0 then ii = ii + ifhnregfiles[i]
    if lcseedreg eq 1 then begin
        cmd = '/fidl_stack_crosscov_seed -conc '+acconc[i]+' $REGION_FILE $ROI'+atlas_str+mask_str[i]+nlagsstr+no_ac_cc_str2
        spawn,!BINEXECUTE+'/fidl_timestr2',timestr
        fscratch = scratchdir+'fidl_scratch_'+timestr[0]+'.csh'
        openw,luac,fscratch,/GET_LUN
        top_of_script,luac
        print_files_to_csh,luac,nregion_files[i],region_files[ir:ir+nregion_files[i]-1],'REGION_FILE','region_file',/NO_NEWLINE
        printf,luac,'set ROI = ('+strtrim(roi_str[i],2)+')'
        printf,luac,!BINEXECUTE+cmd+' -names_only '+strjoin(region_names[0:nreg[0]-1],' ',/SINGLE)
        close,luac
        free_lun,luac
        spawn,'chmod +x '+fscratch,result
        spawn,'csh '+fscratch,result
        nresult = n_elements(result)
        if nresult ne nreg[0] then begin
            stat = spawn_check_and_load(result,fi,wd,dsp,help,nfiles_loaded,files_loaded,stc)
            return,rtn={msg:'EXIT'}
        endif
        print_files_to_csh,lu,nregion_files[i],region_files[ir:ir+nregion_files[i]-1],'REGION_FILE','region_file',/NO_NEWLINE
        printf,lu,'set ROI = ('+strtrim(roi_str[i],2)+')'
        printf,lu,'nice +19 $BIN' + cmd + ' -clean_up'
        printf,lu,''
        rootsstr = ''
        if identify[0] ne '' then begin
            rtn=print_identify(1,is[i],lu)
            if rtn.nsearchstr eq -1 then return,rtn={msg:'EXIT'}
            print_files_to_csh,lu,rtn.nsearchstr,rtn.searchstr,'ROOTS','roots',/NO_NEWLINE,/QUOTES
            rootsstr = ' $ROOTS'
        endif else begin 
            rload_conc=load_conc(fi,stc,dsp,help,wd,acconc[i],/DONT_PUT_IMAGE)
            if rload_conc.msg eq 'GOBACK' then goto,goback5 else if rload_conc.msg eq 'EXIT' then return,rtn={msg:'EXIT'}
            stc1=rload_conc.stc1
            nfiles = stc1.n
            print_files_to_csh,lu,1,strjoin(strtrim(indgen(nfiles)+1,2),'+',/SINGLE),'RUNS','runs',/NO_NEWLINE
        endelse
        for j=0,nresult-1 do begin
            scraplabel = strsplit(result[j],/EXTRACT)
            scraplabel = scraplabel[n_elements(scraplabel)-1]
            print_files_to_csh,lu,1,scraplabel,'CONCS','concs',/NO_NEWLINE
            cmd = 'nice +19 $BIN/fidl_crosscorr $CONCS $RUNS'+rootsstr+atlas_str+mask_str[i]+' -clean_up'
            printf,lu,cmd
        endfor
        printf,lu,''
    endif

    ;START160816
    proceach0=proceach0+1

    ;if not keyword_set(GET_GLMS_STRUCT) and ((proceach0 eq proceach) or (i eq total_nsubjects-1)) and proc gt 1 then begin
    ;START160927
    if not keyword_set(GET_GLMS_STRUCT) and ((proceach0 eq proceach) or (i eq total_nsubjects-1)) then begin

        if glm_list_str[0] ne '' then printf,lu,'# '+strjoin(glm_list_str,' ',/SINGLE)
        close,lu
        free_lun,lu
        print,'Script written to '+script[proceach1]
        spawn,'chmod 0777 '+script[proceach1]
        if action eq 0 then spawn,script[proceach1]+'>'+script[proceach1]+'.log'+' &'
        proceach0=0
        proceach1=proceach1+1
    endif

endfor
if lcseedreg eq 1 then printf,lu,'rm -rf '+scratchdir
if no_ac_cc gt 0 and lcseedreg eq 0 and lcscratch eq 1 then begin
    print_files_to_csh,lu,total_nsubjects,acconc,'CONCS','concs',/NO_NEWLINE
    rootsstr = ''
    if identify[0] ne '' then begin
        rtn=print_identify(total_nsubjects,is,lu)
        if rtn.nsearchstr eq -1 then return,rtn={msg:'EXIT'}
        print_files_to_csh,lu,rtn.nsearchstr,rtn.searchstr,'ROOTS','roots',/NO_NEWLINE,/QUOTES
        rootsstr = ' $ROOTS'
    endif else begin 

        ;print_files_to_csh,lu,1,strjoin(strtrim(indgen(nfiles)+1,2),'+',/SINGLE),'RUNS','runs',/NO_NEWLINE
        ;START140724
        print_files_to_csh,lu,1,strjoin(trim(indgen(stc1.n)+1),'+',/SINGLE),'RUNS','runs',/NO_NEWLINE

    endelse

    if no_ac_cc eq 4 then begin
        if lc_retain eq 0 then scrap = glmroots + '_modfit' else scrap = glmroots + '_res'
        print_files_to_csh,lu,total_nsubjects,scrap,'SUBROOTS','subroots',/NO_NEWLINE,/QUOTES
        fidlpca_str = fidlpca_str + ' $SUBROOTS -scale_by_var'
    endif
    if no_ac_cc eq 1 then goose='fidl_ac' else if no_ac_cc eq 4 then goose = 'fidl_pca' else goose='fidl_crosscorr'
    cmd = 'nice +19 $BIN/'+goose+' $CONCS $RUNS'+rootsstr
    if analysis_type gt 0 or ifhnreg[0] gt 0 then begin
        print_files_to_csh,lu,nreg[0],region_names[0:nreg[0]-1],'REGIONS','regions',/NO_NEWLINE,/QUOTES
        cmd = cmd + ' $REGIONS'
    endif

    ;print_files_to_csh,lu,total_nsubjects,mask,'MASK','mask',/NO_NEWLINE
    ;cmd = cmd + ' $MASK'
    ;START140724
    if mask[0] ne '' then begin 
        print_files_to_csh,lu,total_nsubjects,mask,'MASK','mask',/NO_NEWLINE
        cmd = cmd + ' $MASK'
    endif

    if no_ac_cc eq 3 and analysis_type eq 0 and ifhnreg[0] eq 0 then cmd = cmd + ' -pooledsd'
    cmd = cmd + ' -TR '+strtrim(TR,2) +fidl_ac_optionstr+fidlpca_str+' -scratchdir '+scratchdir+' -clean_up'
    printf,lu,cmd
endif
if not keyword_set(GET_GLMS_STRUCT) then begin

    ;if glm_list_str[0] ne '' then printf,lu,'# '+strjoin(glm_list_str,' ',/SINGLE)
    ;close,lu
    ;free_lun,lu
    ;spawn,'chmod +x '+compute_residuals_csh
    ;action = get_button(['execute','return'],TITLE='Please select action.',BASE_TITLE=compute_residuals_csh)
    ;if action eq 0 then begin
    ;    spawn,'csh '+compute_residuals_csh+' &'
    ;    scrap = get_button('ok',BASE_TITLE=compute_residuals_csh,TITLE='Script has been executed.')
    ;endif
    ;START160816
    ;START160927
    ;if proc eq 1 then begin
    ;    if glm_list_str[0] ne '' then printf,lu,'# '+strjoin(glm_list_str,' ',/SINGLE)
    ;    close,lu
    ;    free_lun,lu
    ;    spawn,'chmod +x '+compute_residuals_csh
    ;    if action eq 0 then begin
    ;        spawn,'csh '+compute_residuals_csh+' &'
    ;        scrap = get_button('ok',BASE_TITLE=compute_residuals_csh,TITLE='Script has been executed.')
    ;    endif
    ;endif

    print,'DONE compute_residuals'
    return,rtn={msg:'OK'}
endif else begin 
    print,'DONE compute_residuals'
    return,rtn={msg:'OK',cmdstr:cmdstr,acconc:acconc,mask_str:mask_str,scratchdir:scratchdir,mask:mask,atlas_str:atlas_str, $
        analysis_type:analysis_type,region_files:region_files,roi_str:roi_str,concselect:concselect,t4select:t4select}
endelse
end
