;Copyright 4/29/02 Washington University.  All Rights Reserved.
;fidl_ttest.pro  $Revision: 1.89 $
function fidl_ttest,fi,dsp,wd,glm,help,stc,pref,REGRESS=regress,SLOPES=slopes,SS=ss
nglm_2 = 0
gauss_str = ''
gauss_str_ttest=''
compress_str = ''
region_str = ''
roi_str = ''
glm_list_str = ''
clean_up_str = ''
delay_str = ''
region_or_uncompress_str = ''
Nimage_mask_str = ''
threshold_extent_str = ''
fwhm = 0.
monte_carlo_fwhm = 0.
avgstat_output_str = ''
Nimage_name_str = ''
files_per_test = 1
two_glm_lists = 'No'
region_file = ''
unscaled_str = ''
print_unscaled_mag_str = ''
magnorm_str = ''
magstr = ''
group_names_ttest = ''
glm_list_str_ttest = ''
scratchdir_str = ''
scratchdir=''
lc_tc = 0
time = 1
normstr=''
nglms_per_sub = 1 
t4_files = ''
t4_files_2 = ''
glm_files_2 = ''
var_thresh_str = ''
nglm_list_str = 0
bf_str = ''
fnstr = ''
nullstr = ''
atlas_str = ''
ntests = 1
task_label = ''
prependstr = ''
total_invvar_poly = 0 
tc_weights_str = ''
analysis_type=0
ttest_output_str=''
time_label=''
files_per_test_time=0
pairs = 1
lc_glm=0
nbehav_pts=0
lcdontcheckt4again=0
get_this = ''
paired=0
region_names=''
loadslk=0
mask=''
glm_list=''
append=0

;START160527
nreg=0

spawn,'whoami',whoami
whoamistr = 'Hey '+whoami[0]+'!'
get_directory,directory,offset_spawn
goback16:
if keyword_set(REGRESS) then $
    test_type = !LINEARREGRESS $
else if keyword_set(SLOPES) then begin 
    test_type = !PAIRED_COMPARISON 
    glm_list=slopes.glmlist
    atlasspace=slopes.atlasspace
    action=1
    fidl_ttest_csh=slopes.fidl_ttest_csh
    append=1
    driver=slopes.driver
    scratchdir_str = ' -scratchdir ' + slopes.scratchdir
    scratchdir=slopes.scratchdir 
    region_or_uncompress_str = slopes.region_or_uncompress_str
    if region_or_uncompress_str ne '' then analysis_type=1
endif else if keyword_set(SS) then begin 
    test_type = get_button(['Paired (Two-sample)','Exit'],BASE_TITLE='Please select',TITLE="For a single glm with assumed " $
        +"responses, please try 'Average statistics' or 'Compute Z statistics'.")
    case test_type of
        0: test_type = !GROUP_COMPARISON
        else: return,rtn={msg:'EXIT'}
    endcase
endif else begin 
    test_type = get_button(['One Sample (Random effects)','Paired (Two-sample, matched pairs, random effects)', $
        'Group (Two-sample, independent samples, random effects)','Correlation','Partial correlation', $
        'Correlation/Partial correlation time series','Single subject vs group','Type II regression','Exit'],TITLE='Please select.')
    case test_type of
        0: test_type = !UNPAIRED_COMPARISON
        1: test_type = !PAIRED_COMPARISON
        2: test_type = !GROUP_COMPARISON
        3: test_type = !CORRELATION
        4: test_type = !PARTIAL_CORRELATION
        5: test_type = !SS_PARTIAL_CORRELATION
        6: test_type = !SS_VS_GROUP
        7: test_type = !TYPE_II_REGRESSION
        else: return,rtn={msg:'EXIT'} 
    endcase
endelse
if test_type eq !SS_PARTIAL_CORRELATION then begin
    fidl_pc,fi,dsp,wd,glm,help,stc,pref
    print,'DONE'
    return,rtn={msg:'EXIT'}
endif
goback160:
if test_type eq !CORRELATION or test_type eq !PARTIAL_CORRELATION or test_type eq !LINEARREGRESS then begin
    rtn_slk=get_slk(help)
    if rtn_slk.msg eq 'GO_BACK' or rtn_slk.msg eq 'EXIT' then return,rtn={msg:'EXIT'}
    slkfile = rtn_slk.slkfile
    column_labels = rtn_slk.column_labels
    sdata = rtn_slk.behav_data

    ;print,'rtn_slk.behav_data'
    ;print,rtn_slk.behav_data

    subject_id = rtn_slk.subject_id
    nbehav_pts = rtn_slk.nbehav_pts
    if test_type eq !CORRELATION then begin
        rtn = select_files(column_labels,TITLE='Please select behavioral variables.',/GO_BACK,/EXIT)
        if rtn.files[0] eq 'GO_BACK' then goto,goback160 else if rtn.files[0] eq 'EXIT' then return,rtn={msg:'EXIT'}
        behav_str = column_labels[rtn.index]
        behav_data = sdata[*,rtn.index]
        nbehav_var = rtn.count
        ntests = rtn.count
    endif else if test_type eq !LINEARREGRESS then begin 
        indvar = intarr(100,n_elements(column_labels))
        nindvar = intarr(100)
        behav_str = ''
        behav_str2 = ''
        ntests = 0
        repeat begin
            list = get_bool_list(column_labels,TITLE='Please select independent variables.',BASE_TITLE='Model '+strtrim(ntests+1,2))
            list = list.list
            index = where(list eq 1,count)
            if count ne 0 then begin
                indvar[ntests,0:count-1] = index
                nindvar[ntests] = count
                behav_str = [behav_str,strjoin(column_labels[index],'_',/SINGLE)]
                behav_str2 = [behav_str2,strjoin(column_labels[index],string(9B),/SINGLE)]
                ntests = ntests + 1
            endif
        endrep until get_button(['yes','no'],TITLE='Would you like to set up another model?') eq 1 
        indvar = indvar[0:ntests-1,0:max(nindvar)-1]
        nindvar = nindvar[0:ntests-1]
        behav_str = behav_str[1:*]
        behav_str2 = behav_str2[1:*]
        behav_data = sdata[*,*]
        nbehav_var = ntests
    endif else begin
        ncolumn_labels = n_elements(column_labels)
        icolumn_labels = indgen(ncolumn_labels)
        indvar = intarr(100,ncolumn_labels)
        ncov = intarr(100)
        ntests = 0
        repeat begin
            scrap = get_button(column_labels,TITLE='Please select variate.')
            index = where(icolumn_labels ne scrap,count)
            if count eq 0 then begin
                stat = dialog_message('Error HERE0. Abort!')
                return,rtn={msg:'EXIT'} 
            endif
            scraplabels = column_labels[index]
            list = get_bool_list(scraplabels,TITLE='Please select covariates.',BASE_TITLE='Hit OK if none.')
            list = list.list
            scrapindex = where(list eq 1,scrapcount)
            if scrapcount ne 0 then $
                indvar[ntests,0:1+scrapcount-1] = [scrap,index[scrapindex]] $
            else $
                indvar[ntests,0:1+scrapcount-1] = scrap
            ncov[ntests] = scrapcount
            ntests = ntests + 1
        endrep until get_button(['yes','no'],TITLE='Would you like to set up another partial correlation?') eq 1 
        indvar = indvar[0:ntests-1,0:max(ncov)] ;one extra for variate
        ncov = ncov[0:ntests-1]
        behav_data = sdata[*,*]
        task_label = strarr(ntests)
        for i=0,ntests-1 do begin
            task_label[i] = column_labels[indvar[i,0]] + '.' + column_labels[indvar[i,1]]
            for j=2,ncov[i] do task_label[i] = task_label[i] + '_' + column_labels[indvar[i,j]]
        endfor
        nbehav_var = ntests
        behav_str = task_label
    endelse
endif else begin
    nbehav_var = 1
    subject_id = string(indgen(1000)+1)
endelse
if test_type eq !GROUP_COMPARISON then begin
    if keyword_set(SS) then begin
        group_names=['session1','session2'] 
        title = 'Please name sessions'
    endif else begin
        group_names=['group1','group2']
        title = 'Please name groups'
    endelse
    group_names = get_str(2,group_names,group_names,TITLE=title,/GO_BACK)
    if group_names[0] eq 'GO_BACK' then goto,goback16
    group_names = fix_script_name(group_names,/SPACE_ONLY)
endif else if test_type eq !SS_VS_GROUP then begin
    group_names = get_str(2,['group 1','group 2'],['patients','controls'],TITLE='Please name groups.',/GO_BACK, $
        LABEL='Each patient you select becomes a separate T test.')
    if group_names[0] eq 'GO_BACK' then goto,goback16
    group_names = fix_script_name(group_names,/SPACE_ONLY)
endif
if test_type ne !CORRELATION and test_type ne !PARTIAL_CORRELATION and test_type ne !PAIRED_COMPARISON $
    and test_type ne !TYPE_II_REGRESSION and test_type ne !UNPAIRED_COMPARISON and test_type ne !GROUP_COMPARISON and $
    test_type ne !LINEARREGRESS and test_type ne !SS_VS_GROUP then begin
    goback161:
    scrapstr = ['glm','4dfp','go back','exit']
    scrap = 3
    lc_glm = get_button(scrapstr,TITLE='Compute from')
    if lc_glm eq scrap then return,rtn={msg:'EXIT'} else if lc_glm eq (scrap-1) then goto,goback16
    goback162:
    lc_interactive = get_button(['list','individual files','go back','exit'],TITLE='Load')
    if lc_interactive eq 3 then return,rtn={msg:'EXIT'} else if lc_interactive eq 2 then goto,goback161
endif
goback0:
if test_type eq !PAIRED_COMPARISON then begin
    if not keyword_set(SLOPES) then begin
        lc_glm = get_button(['glm','4dfp','go back','exit'],TITLE='Compute from')
        if lc_glm eq 3 then return,rtn={msg:'EXIT'} else if lc_glm eq 2 then goto,goback16
    endif
    if lc_glm ne 0 then begin
        group_names = get_str(2,['task','control'],['task','control'],TITLE='Please name conditions.',WIDTH=50)
        group_names = fix_script_name(group_names,/SPACE_ONLY)
        get_this = 'imgs'
    endif
    paired=1
endif
goback14:
if n_elements(lc_interactive) ne 0 then begin
    if lc_interactive eq !FALSE then $
        filter = '*list' $
    else if lc_glm eq 0 then $
        filter = '*.glm' $
    else $
        filter = '*.4dfp.img'
endif
if test_type eq !CORRELATION or test_type eq !PARTIAL_CORRELATION or test_type eq !PAIRED_COMPARISON or $
    test_type eq !TYPE_II_REGRESSION or test_type eq !UNPAIRED_COMPARISON or test_type eq !GROUP_COMPARISON or $
    test_type eq !LINEARREGRESS or test_type eq !SS_VS_GROUP then begin
    goback14a1:
    if get_this eq '' then begin
        if test_type eq !UNPAIRED_COMPARISON or test_type eq !GROUP_COMPARISON then $
            get_this='GLMs/4dfps/slks' $
        else $
            get_this='GLMs/4dfps' 
    endif
    if keyword_set(SS) then begin
        notmorethanone=1 
        howmany=2
    endif else begin
        notmorethanone=0
        howmany=0
    endelse
    gg=get_glms(GET_THIS=get_this,GROUPNAMES=group_names,PAIRED=paired,GLM_LIST=glm_list,NOTMORETHANONE=notmorethanone, $
        HOWMANY=howmany)
    if gg.msg ne 'OK' then begin
        if keyword_set(REGRESS) then goto,goback16 else return,rtn={msg:'EXIT'}
    endif
    if gg.glmfiles[0] ne '' then begin
        files = gg.glmfiles
        glm_space = gg.glm_space
        lc_glm=0
        if strmid(gg.glmfiles[0],strlen(gg.glmfiles[0])-3) eq 'slk' then lc_glm=2
    endif else begin
        files = gg.imgselect
        glm_space = gg.bold_space
        lc_glm=1
    endelse
    ifh = gg.ifh
    lc_interactive = gg.load
    nglm = gg.total_nsubjects
    if nglm eq 1 and not keyword_set(SS) then begin
        junk = get_button('ok',TITLE='Not set up to handle a single glm. Try average statistics with a contrast.')
        return,rtn={msg:'EXIT'}
    endif
    morethanone = gg.morethanone
    if gg.t4select[0] ne '' then t4files = gg.t4select
    glm_list = gg.glm_list
    goback1a:


    ;if nbehav_pts gt 0 then begin
    ;    rtn=automatch(files,t4files,subject_id,slkfile,nbehav_pts,behav_data,dummygrptag,column_labels,/DONTSELECT)
    ;    if rtn.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rtn.msg eq 'GO_BACK' then goto,goback14a1
    ;    nglm = rtn.nfiles
    ;    glm_files = rtn.files
    ;    t4_files = rtn.t4_files
    ;    behav_data = rtn.behav_data
    ;    lcdontcheckt4again=1
    ;endif else if morethanone eq 1 then begin
    ;    rtn_get_morethanone=get_morethanone(files,nglm,1,'')
    ;    if rtn_get_morethanone.msg eq 'GO_BACK' then goto,goback14a1 else if rtn_get_morethanone.msg eq 'ERROR' then $
    ;        return,rtn={msg:'EXIT'} 
    ;    nglm_per_subject = rtn_get_morethanone.nglm_per_subject
    ;    nglm = rtn_get_morethanone.total_nsubjects
    ;    nsubject = rtn_get_morethanone.nsubject
    ;    glm_files = rtn_get_morethanone.files_per_group
    ;    subject_names = rtn_get_morethanone.subject_names
    ;endif else begin
    ;    if test_type eq !GROUP_COMPARISON or test_type eq !SS_VS_GROUP then begin
    ;        nglm = gg.nglm[0]
    ;        glm_files = files[0:gg.nglm[0]-1]
    ;        if gg.t4select[0] ne '' then t4_files = t4files[0:gg.nglm[0]-1]
    ;        if n_elements(gg.nglm) eq 1 and keyword_set(SS) then begin
    ;            nglm_2 = 1
    ;            glm_files_2 = glm_files
    ;            if gg.t4select[0] ne '' then t4_files_2 = t4files
    ;            ifh = [ifh,ifh]
    ;        endif else begin
    ;            nglm_2 = gg.nglm[1]
    ;            glm_files_2 = files[gg.nglm[0]:gg.total_nsubjects-1]
    ;            if gg.t4select[0] ne '' then t4_files_2 = t4files[gg.nglm[0]:gg.total_nsubjects-1]
    ;        endelse
    ;    endif else if test_type eq !PAIRED_COMPARISON then begin
    ;        glm_files = files
    ;        if gg.t4select[0] ne '' then t4_files = t4files
    ;        lcdontcheckt4again=1
    ;        if lc_glm eq 1 then nsubject = nglm/2
    ;    endif else begin
    ;        glm_files = files
    ;        if gg.t4select[0] ne '' then t4_files = t4files
    ;        lcdontcheckt4again=1
    ;    endelse
    ;endelse
    ;START181004
    if nbehav_pts gt 0 then begin

        if morethanone eq 1 then begin
            rtn_get_morethanone=get_morethanone(files,nglm,1,'')
            if rtn_get_morethanone.msg eq 'GO_BACK' then goto,goback14a1 else if rtn_get_morethanone.msg eq 'ERROR' then $
                return,rtn={msg:'EXIT'}
            nglm_per_subject = rtn_get_morethanone.nglm_per_subject
            nglm = rtn_get_morethanone.total_nsubjects
            nsubject = rtn_get_morethanone.nsubject
            glm_files = rtn_get_morethanone.files_per_group
            subject_names = rtn_get_morethanone.subject_names
    
            ;print,'here0 subject_names=',subject_names
            ;print,'here0 subject_id=',subject_id

            ;print,'before behav_data'
            ;print,behav_data
            ;print,'before size(behav_data)=',size(behav_data)

            ;START181005
            rtn=automatch(files,t4files,subject_id,slkfile,nbehav_pts,behav_data,dummygrptag,column_labels,nglm_per_subject, $
                /DONTSELECT,/REUSE)
            if rtn.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rtn.msg eq 'GO_BACK' then goto,goback14a1
            nglm = rtn.nfiles
            glm_files = rtn.files
            t4_files = rtn.t4_files
            lcdontcheckt4again=1
            nglm_per_subject=rtn.nglmpersubject

            superbird=intarr(n_elements(rtn.nglmpersubject))
            for i=0,n_elements(rtn.nglmpersubject)-2 do superbird[i+1]=superbird[i]+rtn.nglmpersubject[i]
            ;print,'superbird=',superbird
            behav_data=rtn.behav_data[superbird,*]

            ;print,'rtn.behav_data'
            ;print,rtn.behav_data
            ;print,'size(rtn.behav_data)=',size(rtn.behav_data)

            ;print,'behav_data'
            ;print,behav_data
            ;print,'size(behav_data)=',size(behav_data)

            undefine,rtn,superbird

;            behav_data = rtn.behav_data
;            subject_id=rtn.subject_id_select


;            rtn=automatch(files,t4files,subject_id,slkfile,nbehav_pts,behav_data,dummygrptag,column_labels,/DONTSELECT)
;            if rtn.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rtn.msg eq 'GO_BACK' then goto,goback14a1
;            nglm = rtn.nfiles
;            glm_files = rtn.files
;            t4_files = rtn.t4_files
;            behav_data = rtn.behav_data
;            lcdontcheckt4again=1


;                    am=automatch(files_per_group[filei],t4select[filei],gs.subject_id,gs.slkfile,gs.nbehav_pts,gs.behav_data,$
;                        grptag,slkcollab,nglm_per_subject[subi],/DONTSELECT,/REUSE)
;                    if am.msg eq 'EXIT' then return else if am.msg eq 'GO_BACK' then goto,goback12

;                    slkdata=am.behav_data[*,sf.index]
;                    slkid=am.subject_id_select
;
;                    ;print,'here2 slkdata=',slkdata
;                    ;print,'here2 n_elements(slkdata)=',n_elements(slkdata)
;                    ;print,'here2 slkid=',slkid
;                    ;print,'here2 n_elements(slkid)=',n_elements(slkid)
;
;                    if am.subjectinot[0] ne -1 then begin
;                        print,'******************* HERE ********************'
;                        use_these[subi[am.subjectinot]]=0
;                    endif
;                    if am.nglmpersubject[0] ne -1 then begin
;                        slknglmpersubject=am.nglmpersubject
;                        for i=0,n_elements(am.nglmpersubject)-1 do begin
;                            if nglm_per_subject[subi[am.subjecti[i]]] ne slknglmpersubject[i] then begin
;                                idx=get_button(['Go back','Exit'],BASE_TITLE='Problem',TITLE='nglm_per_subject['$
;                                    +trim(subi[am.subjecti[i]])+']='+trim(nglm_per_subject[subi[am.subjecti[i]]])$
;                                    +' slknglmpersubject['+trim(i)+']='+trim(slknglmpersubject[i])$
;                                    +' Should be equal. Code needs to be added to handle this.')
;                                if idx eq 0 then goto,goback12a else return
;                            endif
;                        endfor
;                    endif
;                    undefine,am





        endif else begin
            rtn=automatch(files,t4files,subject_id,slkfile,nbehav_pts,behav_data,dummygrptag,column_labels,/DONTSELECT)
            if rtn.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rtn.msg eq 'GO_BACK' then goto,goback14a1
            nglm = rtn.nfiles
            glm_files = rtn.files
            t4_files = rtn.t4_files
            behav_data = rtn.behav_data
            lcdontcheckt4again=1
        endelse
    endif else if morethanone eq 1 then begin
        rtn_get_morethanone=get_morethanone(files,nglm,1,'')
        if rtn_get_morethanone.msg eq 'GO_BACK' then goto,goback14a1 else if rtn_get_morethanone.msg eq 'ERROR' then $
            return,rtn={msg:'EXIT'}
        nglm_per_subject = rtn_get_morethanone.nglm_per_subject
        nglm = rtn_get_morethanone.total_nsubjects
        nsubject = rtn_get_morethanone.nsubject
        glm_files = rtn_get_morethanone.files_per_group
        subject_names = rtn_get_morethanone.subject_names
    endif else begin
        if test_type eq !GROUP_COMPARISON or test_type eq !SS_VS_GROUP then begin
            nglm = gg.nglm[0]
            glm_files = files[0:gg.nglm[0]-1]
            if gg.t4select[0] ne '' then t4_files = t4files[0:gg.nglm[0]-1]
            if n_elements(gg.nglm) eq 1 and keyword_set(SS) then begin
                nglm_2 = 1
                glm_files_2 = glm_files
                if gg.t4select[0] ne '' then t4_files_2 = t4files
                ifh = [ifh,ifh]
            endif else begin
                nglm_2 = gg.nglm[1]
                glm_files_2 = files[gg.nglm[0]:gg.total_nsubjects-1]
                if gg.t4select[0] ne '' then t4_files_2 = t4files[gg.nglm[0]:gg.total_nsubjects-1]
            endelse
        endif else if test_type eq !PAIRED_COMPARISON then begin
            glm_files = files
            if gg.t4select[0] ne '' then t4_files = t4files
            lcdontcheckt4again=1
            if lc_glm eq 1 then nsubject = nglm/2
        endif else begin
            glm_files = files
            if gg.t4select[0] ne '' then t4_files = t4files
            lcdontcheckt4again=1
        endelse
    endelse






endif else begin
    if lc_interactive eq 1 then begin
        if lc_glm eq 0 then begin
            if test_type eq !GROUP_COMPARISON then $
                str = 'Please select GLMs for '+group_names[0]+'.' $
            else $
                str = 'Please select GLMs.'
            glm_files = get_list_of_files('*.glm',directory,str,subject_id,/BELOW)
            nglm = n_elements(glm_files)/nglms_per_sub
            if nglms_per_sub eq 2 then nglm_2 = nglm
            nmag = nglm
            if test_type eq !GROUP_COMPARISON then begin
                str = 'Please select GLMs for '+group_names[1]+'.'
                glm_files_2 = get_list_of_files('*.glm',directory,str,subject_id,/BELOW)
                nglm_2 = n_elements(glm_files_2)
                nmag2 = nglm_2
            endif
        endif else begin
            ntests = 1
            task_label = strarr(ntests)
            ctl_label = strarr(ntests)
            title = strarr(2)
            if lc_glm eq 1 then filter_4dfp = '*.4dfp.img' else filter_4dfp = '*.slk' 
            if test_type eq !UNPAIRED_COMPARISON or test_type eq !CORRELATION then $
                title[0] = 'Please specify files.' $
            else if test_type eq !PAIRED_COMPARISON then begin
                title[0] = 'Please specify files for ' + group_names[0]
                title[1] = 'Please specify files for ' + group_names[1]
            endif else begin ;!GROUP_COMPARISON
                title[0] = 'Please specify files for ' + group_names[0]
                title[1] = 'Please specify files for ' + group_names[1]
            endelse
            glm_files = get_list_of_files(filter_4dfp,directory,title[0],subject_id)
            nglm = n_elements(glm_files)
            if test_type eq !GROUP_COMPARISON or !SS_VS_GROUP then begin
                glm_files_2 = get_list_of_files(filter_4dfp,directory,title[1],subject_id)
                nglm_2 = n_elements(glm_files_2)
            endif else if test_type eq !PAIRED_COMPARISON then begin
                scrap2 = get_list_of_files(filter_4dfp,directory,title[1],subject_id)
                nglm_2 =  n_elements(scrap2)
                if nglm_2 ne nglm then begin
                    stat = dialog_message('Must specify the same number of files for a paired comparison. Abort!',/ERROR)
                    return,rtn={msg:'EXIT'}
                endif
                scrap = glm_files
                glm_files = strarr(nglm+nglm_2)
                j = 0
                for i=0,nglm-1 do begin
                    glm_files[j] = scrap[i]
                    j = j + 2
                endfor
                j = 1
                for i=0,nglm-1 do begin
                    glm_files[j] = scrap2[i] 
                    j = j + 2
                endfor
            endif
        endelse
    endif else begin
        if two_glm_lists eq 'No' then begin
            goback4:
            dummy = '*.*list'
            if lc_glm eq 0 then dummy2 = 'glms' else dummy2 = 'files'
            get_dialog_pickfile,dummy,directory,'Please select list file.',list_file,rtn_nfiles,rtn_path
            if list_file eq 'GOBACK' then $
                goto,goback162 $
            else if list_file eq 'EXIT' then $
                return,rtn={msg:'EXIT'} 
            goback5a:
            glm_list_str = list_file
            rl=read_list(glm_list_str)

            ;if rl.msg eq 'ERROR' then return,rtn={msg:'EXIT'} else if rl.msg eq 'GOBACK' then goto,goback4
            ;START150219
            if rl.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rl.msg eq 'GOBACK' then goto,goback4

            if rl.nglm gt 0 then begin
                nfiles=rl.nglm 
                files=rl.glm
            endif else begin
                nfiles=rl.nimg 
                files=rl.img
            endelse 
            nt4s=rl.nt4
            t4files=rl.t4 
            if test_type eq !GROUP_COMPARISON or nglms_per_sub eq 2 then begin
                scrap = 1
                title = strarr(2)
                if test_type eq !GROUP_COMPARISON then $
                    for i=0,scrap do title[i] = 'Please select '+dummy2+' for ' + group_names[i] + '.' $
                else begin
                    title[0] = 'Please select first set of GLMs.
                    title[1] = 'Please select second set of GLMs.
                endelse
            endif else begin
                scrap = 0
                title = 'Please select '+dummy2+'.'
            endelse
            ne_t4files = n_elements(t4files)
            ne_nfiles = n_elements(nfiles)
            for i=0,scrap do begin
                if i eq 0 then begin 
                    files1 = files[0:nfiles[0]-1]
                    if ne_t4files gt 0 then t4files1 = t4files[0:nfiles[0]-1] 
                endif else if ne_nfiles eq 2 then begin
                    files1 = files[nfiles[0]:nfiles[0]+nfiles[1]-1]
                    if ne_t4files gt 0 then t4files1 = t4files[nfiles[0]:nfiles[0]+nfiles[1]-1] 
                endif
                rtn = select_files(files1,TITLE=title[i],/GO_BACK,MIN_NUM_TO_SELECT=2,/ONE_COLUMN)
                if rtn.files[0] eq 'GO_BACK' then goto,goback162
                if i eq 0 then begin
                    nglm = rtn.count
                    glm_files = rtn.files
                    if ne_t4files gt 0 then begin
                        t4_files = t4files1[rtn.index]
                        nt4 = nglm
                    endif
                endif else begin
                    nglm_2 = rtn.count
                    glm_files_2 = rtn.files
                    if ne_t4files gt 0 then begin
                        nt4_2 = nglm_2
                        t4_files_2 = t4files1[rtn.index]
                    endif
                endelse
                if ne_nfiles ne 2 then begin
                    if rtn.count eq nfiles then begin
                        if scrap eq 1 and i eq 0 then begin
                            stat=dialog_message('No GLM files left for the next group. Please try again.',/ERROR)
                            goto,goback5a
                        endif
                    endif else begin
                        if i eq 0 then begin
                            index = where(rtn.list eq 0)
                            files1 = files1[index]
                            if ne_t4files gt 0 then t4files1 = t4files1[index] 
                        endif
                    endelse
                endif
            endfor
        endif else begin
            if lc_glm eq 0 then dummy2 = 'GLMs' else dummy2 = 'files'
            scraptitle1 = strarr(2)
            scraptitle2 = strarr(2)
            if test_type eq !GROUP_COMPARISON then begin 
                scraptitle1[0] = 'Please select list for group: '+group_names[0]
                scraptitle1[1] = 'Please select list for group: '+group_names[1]
                scraptitle2[0] = 'Please select '+dummy2+' for group: '+group_names[0]
                scraptitle2[1] = 'Please select '+dummy2+' for group: '+group_names[1]
            endif else begin
                dummy = 'condition'
                if dialog_message('FIDL assumes that each list is properly ordered.'+string(10B)+'Thus the ' $
                    +'first file in each list should be the first matched pair.'+string(10B)+'The second file in each list ' $
                    +'should be the second matched pair and so on.',/INFORMATION,/CANCEL) eq 'Cancel' then return,rtn={msg:'EXIT'} 
                if lc_glm ne 0 then begin
                    scraptitle1[0] = 'Please select list for condition: '+group_names[0]
                    scraptitle1[1] = 'Please select list for condition: '+group_names[1]
                    scraptitle2[0] = 'Please select '+dummy2+' for condition: '+group_names[0]
                    scraptitle2[1] = 'Please select '+dummy2+' for condition: '+group_names[1]
                endif else begin
                    scraptitle1[0] = 'Please select first list.' 
                    scraptitle1[1] = 'Please select second list.' 
                    scraptitle2[0] = 'Please select '+dummy2+' from first list.'
                    scraptitle2[1] = 'Please select '+dummy2+' from second list.'
                endelse
            endelse
            two_glm_lists_str = strarr(2)
            for i= 0,1 do begin
                get_dialog_pickfile,'*.*list',directory,scraptitle1[i],list_file,rtn_nfiles,rtn_path
                if list_file eq 'GOBACK' then $
                    goto,goback162 $
                else if list_file eq 'EXIT' then $
                    return,rtn={msg:'EXIT'} 
                glm_list_str = list_file
                two_glm_lists_str[i] = glm_list_str
                rl=read_list(glm_list_str)

                ;if rl.msg eq 'ERROR' then return,rtn={msg:'EXIT'} else if rl.msg eq 'GOBACK' then goto,goback0
                ;START150219
                if rl.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rl.msg eq 'GOBACK' then goto,goback0

                if rl.nglm gt 0 then begin
                    nfiles=rl.nglm
                    files=rl.glm
                endif else begin
                    nfiles=rl.nimg
                    files=rl.img
                endelse
                nt4s=rl.nt4
                t4files=rl.t4
                goback5b:
                rtn = select_files(files,TITLE=scraptitle2[i],/GO_BACK,MIN_NUM_TO_SELECT=2,/ONE_COLUMN)
                if rtn.files[0] eq 'GO_BACK' then goto,goback162
                if i eq 0 then begin
                    nglm = rtn.count
                    glm_files = rtn.files
                    if nlists eq 2 then begin
                        t4_files = t4files[rtn.index]
                        nt4 = nglm
                    endif
                endif else begin
                    nglm_2 = rtn.count
                    glm_files_2 = rtn.files
                    if nlists eq 2 then begin
                        t4_files_2 = t4files[rtn.index]
                        nt4_2 = nglm_2
                    endif
                endelse
            endfor
            if test_type eq !PAIRED_COMPARISON then begin
                if nglm ne nglm_2 then begin
                    stat=dialog_message(two_glm_lists_str[0]+' has '+strtrim(nglm,2)+' files.'+string(10B)+two_glm_lists_str[1] $
                        +' has '+strtrim(nglm_2,2)+' files.'+string(10B)+'They both must have the same number of files. Abort!')
                    return,rtn={msg:'EXIT'} 
                endif
                glm_files_2 = get_str(nglm,glm_files,glm_files_2,TITLE='Please check pairs.',/ONE_COLUMN,/BELOW)
            endif
        endelse
    endelse
endelse 
goback15:
total_nsubjects = nglm + nglm_2
if n_elements(nsubject) eq 0 then nsubject = total_nsubjects
if n_elements(glm_space) eq 0 then glm_space = intarr(total_nsubjects)
glm_files = [glm_files,glm_files_2]
glm_files = glm_files[0:total_nsubjects-1]
if t4_files[0] ne '' and lcdontcheckt4again eq 0 then $
    t4_files = get_str(total_nsubjects,glm_files,[t4_files,t4_files_2],TITLE='Please check t4s.',/ONE_COLUMN,/BELOW)
if lc_glm eq 0 then begin
    if n_elements(nglm_per_subject) eq 0 then begin
        if nglms_per_sub eq 1 then begin
            nsubject = total_nsubjects
            nglm_per_subject = make_array(nsubject,/INTEGER,VALUE=1)
        endif else begin
            nsubject = total_nsubjects/2
            nglm_per_subject = make_array(nsubject,/INTEGER,VALUE=2)
        endelse
    endif
    subject_get_labels_struct = replicate({Get_labels_struct},nsubject)
    j = 0
    for i=0,nsubject-1 do begin
        subject_get_labels_struct[i] = get_labels_from_glm(fi,dsp,wd,glm,help,pref,dummyifh, $
            glm_files[j:j+nglm_per_subject[i]-1],CONTRASTS_ONLY=time,START_INDEX_GLM=j)
        j = j + nglm_per_subject[i]
    endfor
    ncontrastlabels=max(subject_get_labels_struct[*].ncontrastlabels,which_onec)
    if ncontrastlabels ne 0 then contrastlabels=*subject_get_labels_struct[which_onec].contrastlabels
    neffectlabels=max(subject_get_labels_struct[*].neffectlabels,which_onee)
    effectlabels=*subject_get_labels_struct[which_onee].effectlabels
    for i=0,nsubject-1 do begin
        if i ne which_onee then begin
            scrap=*subject_get_labels_struct[i].effectlabels
            for j=0,subject_get_labels_struct[i].neffectlabels-1 do begin
                if total(strcmp(effectlabels,scrap[j])) eq 0 then effectlabels = [effectlabels,scrap[j]]
            endfor
        endif
    endfor
    effect_length = *subject_get_labels_struct[which_onee].effect_length
    effect_length_ts = make_array(nsubject,neffectlabels,/INT,VALUE=1)
    for i=0,nsubject-1 do $
        effect_length_ts[i,0:subject_get_labels_struct[i].neffectlabels-1] = *subject_get_labels_struct[i].effect_length

    ;START160527
    nreg=subject_get_labels_struct[which_onee].ifh.nreg

    if not keyword_set(SLOPES) then exclude_effects,effectlabels,/EXCLUDEHZ
    goback2:
    if keyword_set(SLOPES) then begin 
        BOLD_or_PET = 0
        time = 0
    endif else begin 
        BOLD_or_PET = get_button(['BOLD','PET','GO BACK'],TITLE='What type of data?')
        if BOLD_or_PET eq 2 then goto,goback162
        goback21:
        if test_type eq !CORRELATION then $
            title = '' $
        else $
            title = 'If you have asssumed a shape for the HRF in the GLM then always select timecourses.'
        time = get_button(['timecourses (ie effects)','magnitudes (ie contrasts)','go back'], $
            BASE_TITLE='Which do you wish to analyze?',TITLE=title)
        if time eq 2 then begin
            time = 1
            goto,goback2
        endif
    endelse
    if time eq 0 then begin
        contrast_labels = effectlabels
        length_label = neffectlabels
        which_one = which_onee
    endif else begin
        contrast_labels = contrastlabels
        length_label = ncontrastlabels
        which_one = which_onec
        spider = get_button(['yes','no','go back'],BASE_TITLE='Magnitude normalization', $
            TITLE='Contrasts are normalized to have a magnitude of 1.'+string(10B)+'If contrasts are averages, hit no.' $
            +string(10B)+'If the number of condtions in the contrast varies from subject to subject, then yes.')
        if spider eq 2 then goto,goback21
        if spider eq 0 then magnorm_str = ' -magnorm'
    endelse
    if BOLD_or_PET eq 1 then begin
        unscaled_str = ' -unscaled'
        if time eq 0 then $
            print_unscaled_mag_str = ' -print_unscaled_tc -dont_print_scaled_tc' $
        else $
            print_unscaled_mag_str = ' -print_unscaled_mag'
    endif
endif else if lc_glm eq 1 then begin
    if n_elements(ifh) eq 0 then begin
        ifh_dim4 = intarr(total_nsubjects)
        ifh_fwhm = fltarr(total_nsubjects)
        for i=0,total_nsubjects-1 do begin
            ifh = read_mri_ifh(glm_files[i])
            glm_space[i] = get_space(ifh.matrix_size_1,ifh.matrix_size_2,ifh.matrix_size_3)
            ifh_dim4[i] = ifh.matrix_size_4
            ifh_fwhm[i] = ifh.fwhm
        endfor
    endif else begin
        ifh_dim4 = ifh[*].matrix_size_4
        ifh_fwhm = ifh[*].fwhm
    endelse
    dummy = 'files'
    if time eq 0 then begin
        if total(ifh_dim4 - ifh_dim4[0]) ne 0 then begin
            stat=dialog_message('Not all images have the same time dimension. Abort!',/ERROR)
            return,rtn={msg:'EXIT'}
        endif
    endif
    if ifh_dim4[0] gt 1 then time=0
    if total(ifh_fwhm - ifh_fwhm[0]) ne 0 then begin
        stat=dialog_message('Not all images have been smoothed the same amount. Abort!',/ERROR)
        return,rtn={msg:'EXIT'}
    endif
endif else begin
    slkreg = ''
    nslkreg = intarr(total_nsubjects)
    widget_control,/HOURGLASS
    for i=0,total_nsubjects-1 do begin
        scrap = fix_script_name(strtrim(read_sylk_str(glm_files[i],NROWS=1,/ARRAY),2),/SPACE_ONLY)
        nslkreg[i] = n_elements(scrap)
        slkreg = [slkreg,scrap]
    endfor
    slkreg = slkreg[1:*]
    glm_space[*] = !UNKNOWN
    if total(nslkreg-nslkreg[0]) ne 0 then begin
        stat=dialog_message('Not all slkfiles are of the same dimension. Abort!',/ERROR)
        return,rtn={msg:'EXIT'}
    endif
    slkreg = reform(slkreg,nslkreg[0],total_nsubjects)
    print,'slkreg'        
    print,slkreg
    for i=0,nslkreg[0]-1 do begin
        index = where(slkreg[i,0] ne slkreg[i,*],count)
        if count ne 0 then begin
            stat=dialog_message('The regions in all slkfiles must be ordered identically. Abort!',/ERROR)
            return,rtn={msg:'EXIT'}
        endif
    endfor
    ntests=(nslkreg[0]*(nslkreg[0]-1))/2
    task_label = strarr(ntests)
    widget_control,/HOURGLASS
    l = 0
    for j=0,nslkreg[0]-1 do begin
        for k=j+1,nslkreg[0]-1 do begin
            task_label[l] = slkreg[j] + '_' + slkreg[k] 
            l = l + 1
        endfor
    endfor
    region_or_uncompress_str = ' -regions "' + strjoin(slkreg[*,0],'" "',/SINGLE) + '"' + ' -matrices'
endelse
if total(glm_space - glm_space[0]) ne 0 then begin
    print,'glm_space=',glm_space
    stat=dialog_message('Not all '+dummy+' are in the same space. Abort!',/ERROR)
    return,rtn={msg:'EXIT'}
endif

;START150402
;boldtype = subject_get_labels_struct[0].ifh.glm_boldtype
;idx=where(subject_get_labels_struct[*].ifh.glm_boldtype ne boldtype,cnt)
;if cnt ne 0 then begin
;    scrap=get_button(['Exit'],TITLE='Not all glms are formed from the same type of bolds.' $
;        +string(10B)+string(10B)+'Expecting '+boldtype)
;    return,rtn={msg:'EXIT'}
;endif
;START150422
if lc_glm eq 0 then begin
    boldtype = subject_get_labels_struct[0].ifh.glm_boldtype
    idx=where(subject_get_labels_struct[*].ifh.glm_boldtype ne boldtype,cnt)
    if cnt ne 0 then begin
        scrap=get_button(['Exit'],TITLE='Not all glms are formed from the same type of bolds.' $
            +string(10B)+string(10B)+'Expecting '+boldtype)
        return,rtn={msg:'EXIT'}
    endif
endif else boldtype='img'


;if lc_interactive eq !TRUE then begin
;START150402
if lc_interactive eq 1 and boldtype eq 'img' then begin

    if lc_glm eq 0 then begin
        if glm_space[0] eq !SPACE_111 or glm_space[0] eq !SPACE_222 or glm_space[0] eq !SPACE_333 then begin
            ;do nothing
        endif else if glm_space[0] eq !UNKNOWN then begin
            ;do nothing
        endif else begin
            if test_type eq !GROUP_COMPARISON then $
                str = 'Please select t4s for '+group_names[0]+'.' $
            else $
                str = 'Please select t4s.'
            str = str + " Just hit 'Done' if none."
            t4_files = get_list_of_files('*anat_ave_to_711-2?_t4',directory,str,subject_id)
        endelse
        if test_type eq !GROUP_COMPARISON then begin
            if t4_files[0] ne '' then begin
                t4_files_2 = get_list_of_files('*anat_ave_to_711-2?_t4',directory,'Please select t4s for '+group_names[1]+'.' $
                    +" Just hit 'Done' if none.",subject_id)
                t4_files = [t4_files,t4_files_2]
            endif
        endif
    endif
endif
if not keyword_set(SLOPES) then atlasspace = glm_space[0]
if glm_space[0] eq !SPACE_DATA then begin
    if t4_files[0] ne 'NONE' and t4_files[0] ne '' then begin
        rtn=select_space()
        atlasspace=rtn.space
        atlas=rtn.atlas
        atlas_str=rtn.atlas_str
    endif
endif
goback169:
if lc_glm eq 0 then begin
    if (test_type eq !UNPAIRED_COMPARISON or test_type eq !GROUP_COMPARISON or test_type eq !SS_VS_GROUP) and not keyword_set(SS) $ 
    then begin
        if time eq 0 then begin
            goback155:
            ntests = get_str(1,'How many T-tests?','1',/GO_BACK)
            if ntests[0] eq 'GO_BACK' then begin
                if lc_glm eq 0 then goto,goback21 else goto,goback14
            endif
            ntests = fix(ntests[0])
            scrapstr = 't-test ' + strtrim(indgen(ntests)+1,2)
            sumrows = intarr(ntests)
            index_conditions = intarr(ntests,length_label)
            goback156:
            title = 'Please select.'
            widget_control,/HOURGLASS
            for i=0,ntests-1 do begin
                goback156a:
                rtn_gol = get_ordered_list(contrast_labels,indgen(n_elements(contrast_labels))+1,TITLE=scrapstr[i]+': '+title, $
                    /NO_RMPATH)
                if rtn_gol.count eq 0 then begin
                    goose = get_button(['try again','go back','exit'],TITLE='You failed to make a selection.')
                    if goose eq 0 then goto,goback156a else if goose eq 1 then goto,goback155 else return,rtn={msg:'EXIT'} 
                endif
                sumrows[i] = rtn_gol.count 
                index_conditions[i,0:rtn_gol.count-1] = rtn_gol.isorted
            endfor 
            goback157:
        endif else begin
            goback14d:
            list = get_bool_list(contrast_labels,BASE_TITLE='Please select contrasts.', $
               TITLE='Each contrast is a separate T test.',/GO_BACK,/ONE_COLUMN)
            list = list.list
            if list[0] eq -1 then goto,goback14
            index_conditions = where(list,ntests)
            if ntests eq 0 then begin
                if dialog_message('You failed to select a contrast. Please try again.'+string(10B)+'Cancel to exit.', $
                    /ERROR,/CANCEL) eq 'Cancel' then return,rtn={msg:'EXIT'} else goto,goback14d
            endif
            sumrows = make_array(ntests,/INT,VALUE=1);
        endelse
        task_label = strarr(ntests)
    endif else if test_type eq !PAIRED_COMPARISON then begin
        if time eq 1 and n_elements(contrast_labels) eq 1 then begin
            scrap = get_button(['Yes, one sampe T test.','exit'],TITLE='You have only a single contrast. '+contrast_labels[0] $
                +string(10B)+'It is impossible to do a paired T test on a single contrast.'+string(10B)+'You probably want to ' $
                +'do a one sample T test.',BASE_TITLE=whoamistr)
            if scrap eq 0 then begin
                test_type = !UNPAIRED_COMPARISON
                goto,goback169
            endif else $
                return,rtn={msg:'EXIT'}
        endif
        goback141:
        if keyword_set(SLOPES) then begin
            ntests = 2
            sumrows = make_array(ntests*2,/INTEGER,VALUE=1)
            index_conditions = intarr(ntests*2)
            index_conditions[2:3] = 1
        endif else begin
            ntests = get_str(1,'How many T-tests?','1',/GO_BACK)
            if ntests[0] eq 'GO_BACK' then goto,goback21
            ntests = fix(ntests[0])
            scrapstr = strarr(ntests*2)
            j = 1
            for i=0,ntests*2-1,2 do begin
                scrapstr[i] = 't-test ' + strtrim(j,2) + ' task'
                j = j + 1
            endfor
            j = 1
            for i=1,ntests*2-1,2 do begin
                scrapstr[i] = 't-test ' + strtrim(j,2) + ' control'
                j = j + 1
            endfor
            if time eq 0 then begin
                top_title = 'Please select event types.'
                title = 'Multiple event types are combined as a weighted mean by the inverse of their variance.'
            endif else begin
                top_title = 'Please select contrasts.'
                title = top_title
            endelse
            widget_control,/HOURGLASS
            rtn_ic = identify_conditions_new(n_elements(contrast_labels),contrast_labels,ntests*2,scrapstr,time,0,title, $
                TOP_TITLE=top_title,/SKIP_CHECKS)
            if rtn_ic.special eq 'GO_BACK' then goto,goback141
            sumrows = rtn_ic.sumrows
            index_conditions = rtn_ic.index_conditions
        endelse
        task_label = strarr(ntests)
    endif else if test_type eq !TYPE_II_REGRESSION then begin
        if time eq 1 and n_elements(contrast_labels) eq 1 then begin
            scrap = get_button(['go back','exit'],TITLE='You have only a single contrast. '+contrast_labels[0] $
                +string(10B)+'It is impossible to do a type II regression on a single contrast.',BASE_TITLE=whoamistr)
            if scrap eq 0 then goto,goback16 else return,rtn={msg:'EXIT'}
        endif
        goback142:
        ntests = get_str(1,'How many type II regressions?','1',/GO_BACK)
        if ntests[0] eq 'GO_BACK' then goto,goback21
        ntests = fix(ntests[0])
        scrapstr = strarr(ntests*2)
        j = 1
        for i=0,ntests*2-1,2 do begin
            scrapstr[i] = strtrim(j,2) + ': L seed'
            j = j + 1
        endfor
        j = 1
        for i=1,ntests*2-1,2 do begin
            scrapstr[i] = strtrim(j,2) + ': R seed'
            j = j + 1
        endfor
        if time eq 0 then begin
            top_title = 'Please select event types.'
            title = 'Multiple event types are combined as a weighted mean by the inverse of their variance.'
        endif else begin
            top_title = 'Please select contrasts.'
            title = top_title
        endelse
        widget_control,/HOURGLASS
        rtn_ic = identify_conditions_new(length_label,contrast_labels,ntests*2,scrapstr,time,0,title,TOP_TITLE=top_title,/SKIP_CHECKS)
        if rtn_ic.special eq 'GO_BACK' then goto,goback141
        sumrows = rtn_ic.sumrows
        index_conditions = rtn_ic.index_conditions
        task_label = strarr(ntests)

    ;START140319
    endif else if keyword_set(SS) then begin

        ntests = get_str(1,'How many T-tests?','1',/GO_BACK)
        if ntests[0] eq 'GO_BACK' then goto,goback21
        ntests = fix(ntests[0])
        if time eq 0 then begin
            top_title = 'Please select event types.'
            title = 'Multiple event types are combined as a weighted mean by the inverse of their variance.'
        endif else begin
            top_title = 'Please select contrasts.'
            title = top_title
        endelse
        scrapstr = strarr(ntests*2)
        j = 1
        for i=0,ntests*2-1,2 do begin
            scrapstr[i] = 't-test ' + trim(j) + ' ' + group_names[0]
            j = j + 1
        endfor
        j = 1
        for i=1,ntests*2-1,2 do begin
            scrapstr[i] = 't-test ' + trim(j) + ' ' + group_names[1]
            j = j + 1
        endfor
        widget_control,/HOURGLASS
        rtn_ic = identify_conditions_new(subject_get_labels_struct[0].ncontrastlabels,*subject_get_labels_struct[0].contrastlabels, $
            ntests*2,scrapstr,time,0,title,TOP_TITLE=top_title,/SKIP_CHECKS, $
            LENGTH_LABEL2=subject_get_labels_struct[1].ncontrastlabels,LABELS2=*subject_get_labels_struct[1].contrastlabels)
        if rtn_ic.special eq 'GO_BACK' then goto,goback141
        sumrows = rtn_ic.sumrows
        index_conditions = rtn_ic.index_conditions
    endif else begin ;!CORRELATION !PARTIAL_CORRELATION !LINEARREGRESS 
        goback170:
        if time eq 0 then begin
            if test_type eq !CORRELATION then label='How many correlations?' $
            else if test_type eq !PARTIAL_CORRELATION then label='How many partial correlations?' $
            else label='How many models?'
            scrap = get_str(nbehav_var,behav_str,make_array(nbehav_var,/STRING,VALUE='1'),LABEL=label,FRONT='1',/GO_BACK)
            if scrap[0] eq 'GO_BACK' then begin
                if test_type eq !CORRELATION then goto,goback169 $
                else if test_type eq !PARTIAL_CORRELATION then goto,goback169 $
                else begin
                    goto,goback21
                endelse
            endif
            loop2 = fix(scrap)
            behav_str1 = ''
            for i=0,nbehav_var-1 do behav_str1 = [behav_str1,make_array(loop2[i],/STRING,VALUE=behav_str[i])]
            behav_str1 = behav_str1[1:*]
            behav_str = behav_str1
        endif
        goback171:
        if time eq 0 then begin
            top_title = 'Please select timecourses.'
            title='Multiple timecourses are summed. Next window will provide options.'
        endif else begin
            top_title = ''
            title = 'Please select contrasts.'
        endelse
        widget_control,/HOURGLASS
        rtn_ic = identify_conditions_new(n_elements(contrast_labels),contrast_labels,n_elements(behav_str),behav_str,0,0,title, $
            /SKIP_CHECKS,TOP_TITLE=top_title)

        ;if rtn_ic.special eq 'GO_BACK' then goto,goback2
        ;START130423
        if rtn_ic.special eq 'GO_BACK' then begin
            if time eq 0 then goto,goback170 else goto,goback2
        endif

        sumrows = rtn_ic.sumrows
        index_conditions = rtn_ic.index_conditions
        ntests = n_elements(behav_str) 

        print,'*************** HERE ********************'

        task_label = behav_str
        ctl_label = strarr(ntests)


        ;if (test_type eq !CORRELATION or test_type eq !LINEARREGRESS or test_type eq !PARTIAL_CORRELATION) and time eq 0 then begin
        ;    for i=0,n_elements(sumrows)-1 do $ 
        ;        task_label[i]=behav_str[i]+ '_' +strjoin(strtrim(contrast_labels[index_conditions[i,0:sumrows[i]-1]],2),'+',/SINGLE)
        ;    scrapstr = task_label
        ;endif
        ;START130628
        if (test_type eq !CORRELATION or test_type eq !PARTIAL_CORRELATION) and time eq 0 then begin
            for i=0,n_elements(sumrows)-1 do $ 
                task_label[i]=behav_str[i]+ '_' +strjoin(strtrim(contrast_labels[index_conditions[i,0:sumrows[i]-1]],2),'+',/SINGLE)
        endif else if test_type eq !LINEARREGRESS then begin
            if time eq 0 then begin
                for i=0,n_elements(sumrows)-1 do $
                    task_label[i]=strjoin(strtrim(contrast_labels[index_conditions[i,0:sumrows[i]-1]],2),'+',/SINGLE)+'_'+behav_str[i]
            endif else begin
                task_label = strarr(total(sumrows))
                k=0
                for i=0,n_elements(sumrows)-1 do begin
                    for j=0,sumrows[i]-1 do begin
                        task_label[k] = strtrim(contrast_labels[index_conditions[i,j]],2)+'_'+behav_str[i]
                        k = k + 1
                    endfor
                endfor
            endelse
        endif
        scrapstr = task_label


    endelse
    nmag = nglm
    if time eq 1 then begin
        cnorm = *subject_get_labels_struct[which_one].cnorm
        count_eff_len_eq_1 = intarr(ntests)
        for i=0,ntests-1 do begin
            index = where(cnorm[*,index_conditions[i,0]],count)
            if count eq 0 then begin
                print,'which_one=',which_one
                print,'cnorm=',cnorm
                print,'cnorm[*,index_conditions[i,0]]=',cnorm[*,index_conditions[i,0]]
                count_eff_len_eq_1[i] = 1
            endif else begin 
                index = where(effect_length[index] eq 1,count)
                count_eff_len_eq_1[i] = count
            endelse
        endfor
        index = where(count_eff_len_eq_1 eq 0,count)
        if count gt 0 then begin
            stat = get_delays(delay_str)
            if stat eq 'GOBACK' then goto,goback15
        endif

    ;endif else if test_type eq !UNPAIRED_COMPARISON or test_type eq !GROUP_COMPARISON or test_type eq !CORRELATION or $
    ;    test_type eq !LINEARREGRESS or test_type eq !PARTIAL_CORRELATION then begin
    ;START131104
    endif else if test_type eq !UNPAIRED_COMPARISON or test_type eq !GROUP_COMPARISON or test_type eq !CORRELATION or $
        test_type eq !LINEARREGRESS or test_type eq !PARTIAL_CORRELATION or test_type eq !SS_VS_GROUP then begin

        loadslk=0
        if max(sumrows) gt 3 then begin
            dummy = strarr(6,ntests)
            dummy[0,*] = 'weighted mean by the inverse of their variance'
            dummy[1,*] = 'linear'
            dummy[2,*] = 'quadratic'
            dummy[3,*] = 'cubic'
            dummy[4,*] = 'quartic'
            dummy[5,*] = 'quintic'
            m1 = (sumrows gt 10) or (sumrows lt 3)
            m2 = (sumrows ge 8) and (sumrows le 10)
            m3 = (sumrows eq 6) or (sumrows eq 7)
            goose = m1 + m2*6 + m3*5 + abs(m1+m2+m3-1)*sumrows
            loadslk=0
            rtn = get_choice_list_many(scrapstr,dummy,TOP_TITLE='Please select.',TITLE='Event types should be weighted', $
                LENGTHS=goose,/GO_BACK,SPECIAL='load slk')
            if rtn.special eq 'GO_BACK' then goto,goback156
            if rtn.special eq 'SPECIAL' then $
                loadslk=1 $
            else begin
                invvar_poly = rtn.list
                total_invvar_poly = total(invvar_poly)
            endelse
        endif else if max(sumrows) gt 1 then begin
            loadslk = get_button(['weighted mean by the inverse of their variance','load slk','go back','exit'], $
                TITLE='Event types should be weighted')
            if loadslk eq 2 then goto,goback169 else if loadslk eq 3 then return,rtn={msg:'EXIT'}
        endif
        if loadslk eq 1 then begin
            goback158:
            loadslk=1
            rtn_slk1=get_slk(help)
            if rtn_slk1.msg eq 'GO_BACK' then goto,goback157 else if rtn_slk1.msg eq 'EXIT' then return,rtn={msg:'EXIT'}
            column_labels1 = rtn_slk1.column_labels
            behav_data1 = rtn_slk1.behav_data
            subject_id1 = rtn_slk1.subject_id
            nbehav_pts1 = rtn_slk1.nbehav_pts
            ncolumn_labels1 = rtn_slk1.ncolumn_labels
            invvar_poly = intarr(ntests)
            total_invvar_poly = total(invvar_poly)
            goback159:
            if subject_id1[0] eq 'ALL' then begin 
                size_behav_data1 = size(behav_data1)
                scrap = strarr(nglm+nglm_2,size_behav_data1[2])
                for i=0,nglm+nglm_2-1 do scrap[i,*] = behav_data1[0,*] 
                behav_data1 = scrap
            endif else begin
                grptag = intarr(nglm+nglm_2)
                if test_type eq !GROUP_COMPARISON then grptag[nglm:*]=1

                ;rtn=automatch(glm_files,t4_files,subject_id1,slkfile1,nbehav_pts1,behav_data1,grptag,/DONTSELECT)
                ;START131125
                rtn=automatch(glm_files,t4_files,subject_id1,slkfile1,nbehav_pts1,behav_data1,grptag,column_labels1,/DONTSELECT)

                if rtn.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rtn.msg eq 'GO_BACK' then goto,goback158
                if test_type ne !GROUP_COMPARISON then $
                    nglm = rtn.nfiles $
                else begin
                    penguin = ''
                    idx = where(grptag eq 0,nglm)
                    if nglm eq 0 then penguin = penguin+' '+group_names[0]
                    idx = where(grptag eq 1,nglm_2)
                    if nglm_2 eq 0 then penguin = penguin+' '+group_names[1]
                    if penguin ne '' then begin
                        idx = get_button(['go back','exit'],BASE_TITLE='Big problem',TITLE='No subjects left for:'+penguin)
                        if idx eq 0 then goto,goback158 else return,rtn={msg:'EXIT'}
                    endif
                endelse
                glm_files = rtn.files
                t4_files = rtn.t4_files
                behav_data1 = rtn.behav_data
                lcdontcheckt4again=1
            endelse
            checked = intarr(ncolumn_labels1)
            idx1first=-1
            idx1=-1
            idx1n=-1
            i=0
            repeat begin
                idx = where(column_labels1[i[0]] eq column_labels1,cnt)
                checked[idx]=1
                idx1first = [idx1first,idx[0]]
                idx1 = [idx1,idx]
                idx1n = [idx1n,cnt] 
                i = where(checked eq 0,cnt)
            endrep until cnt eq 0
            idx1first=idx1first[1:*]
            idx1=idx1[1:*]
            idx1n=idx1n[1:*] 
            column_labels1first = column_labels1[idx1first]
            goback159a:
            indvar1 = intarr(ntests,max(sumrows))
            for i=0,ntests-1 do begin
                scrap = strtrim(contrast_labels[index_conditions[i,0:sumrows[i]-1]],2)
                scrap1 = column_labels1first
                scrap1i = indgen(n_elements(column_labels1first))
                for j=0,sumrows[i]-1 do begin
                    idx = where(scrap1 eq scrap[j],cnt)
                    if cnt eq 0 or cnt gt 1 then begin
                        if cnt eq 0 then base_title = 'Not found' else base_title = 'More than one match found'
                        idx = get_button([scrap1,'go back','exit'],BASE_TITLE=base_title, $
                            TITLE='Please select a column for '+scrap[j])
                        lizard = n_elements(scrap1)
                        if idx eq lizard then goto,goback159a else if idx eq lizard+1 then return,rtn={msg:'EXIT'}
                   endif
                   indvar1[i,j] = scrap1i[idx]
                   scrap2 = intarr(n_elements(scrap1))
                   scrap2[idx] = 1
                   idx = where(scrap2 eq 0,cnt)
                   if cnt ne 0 then begin
                       scrap1 = scrap1[idx]
                       scrap1i = scrap1i[idx]
                   endif
                endfor
            endfor
            if (test_type eq !CORRELATION or test_type eq !LINEARREGRESS) and subject_id1[0] eq 'ALL' then begin
                task_label = strarr(ntests)
                for i=0,ntests-1 do begin
                    task_label[i] = behav_str[i]+ '_'
                    for j=0,sumrows[i]-1 do begin
                        spider = strmid(contrast_labels[index_conditions[i,j]],0,1)
                        if spider eq '-' or spider eq '0' or spider eq '1' or spider eq '2' or spider eq '3' or spider eq '4' $
                            or spider eq '5' or spider eq '6' or spider eq '7' or spider eq '8' or spider eq '9' then $
                            bunny = '_' else bunny = ''
                        lizard=''
                        if j ne 0 then begin
                            if float(behav_data1[0,indvar1[i,j]]) ge 0. then lizard='+'
                        endif
                        task_label[i]=task_label[i]+lizard+trim(behav_data1[0,indvar1[i,j]])+bunny $
                            +contrast_labels[index_conditions[i,j]] 
                    endfor
                endfor
            endif
        endif
    endif
    dummy = 'GLMs'
endif
goback17:
if test_type ne !CORRELATION and test_type ne !PARTIAL_CORRELATION and test_type ne !NONLINEAR and n_elements(task_label) gt 0 $
    then task_label = strcompress(task_label,/REMOVE_ALL)
cstr = make_array(total_nsubjects,/STRING,VALUE='')
cstrwts = make_array(total_nsubjects,/STRING,VALUE='')
goback172:


if keyword_set(SS) then $
    loop = 2*ntests $
else if test_type eq !PAIRED_COMPARISON or test_type eq !TYPE_II_REGRESSION then begin 
    loop = 2*ntests 
    pairs = 2
endif else $ 
    loop = ntests
loop1 = loop
glmroot=''
if lc_glm eq 0 then begin
    cstr_uniq = make_array(total_nsubjects,/STRING,VALUE='')
    if not keyword_set(SS) then begin
        if n_elements(subject_names) eq 0 then begin
            junk = get_root(glm_files,'.glm')
            glmroot = junk.file
            junk = get_root(glm_files)
            junk = junk.file
            subject_names = strarr(nsubject)
            j=0
            for i=0,nsubject-1 do begin
                subject_names[i] = ['sub'+strtrim(i+1,2)+' ex glm:'+junk[j]]
                j=j+nglm_per_subject[i]
            endfor
        endif
        scrap = cft_and_scft(nsubject,loop,length_label,sumrows,index_conditions,contrast_labels,subject_get_labels_struct, $
            time,subject_names,scrapstr,nfactors,dummy,/REPEATS,/NOTPRESENT)

        ;START161020
        if scrap.sum_contrast_for_treatment[0,0,0] eq -2 then $
            return,rtn={msg:'EXIT'} $
        else if scrap.sum_contrast_for_treatment[0,0,0] eq -1 then begin 
            undefine,scrap
            goto,goback169
        endif

        sum_contrast_for_treatment = scrap.sum_contrast_for_treatment

        ;START161020
        undefine,scrap

    endif else begin
        sum_contrast_for_treatment = intarr(nsubject,ntests*2,length_label)
        sum_contrast_for_treatment[0,indgen(ntests)*2,0]=index_conditions[indgen(ntests)*2,0]+1
        sum_contrast_for_treatment[1,indgen(ntests)*2,0]=index_conditions[indgen(ntests)*2+1,0]+1
    endelse
    if test_type eq !PAIRED_COMPARISON then begin
        time_label=strarr(loop)
        time_label1=strarr(loop)
        j=0
        spider=strtrim(indgen(loop/2)+1,2)
        for i=0,loop-1,2 do begin
            time_label[i] = 't'+spider[j]+':'+strjoin(strtrim(contrast_labels[index_conditions[i,0:sumrows[0]-1]],2),'+',/SINGLE)
            time_label1[i] = time_label[i]
            time_label[i+1]='c'+spider[j]+':'+strjoin(strtrim(contrast_labels[index_conditions[i+1,0:sumrows[1]-1]],2),'-',/SINGLE)
            time_label1[i+1]='c'+spider[j]+':'+strjoin(strtrim(contrast_labels[index_conditions[i+1,0:sumrows[1]-1]],2),'+',/SINGLE)
            j=j+1
        endfor
    endif else if test_type eq !TYPE_II_REGRESSION then begin
        time_label=strarr(loop)
        for i=0,loop-1,2 do begin
            time_label[i] = 'L seed:'+strjoin(strtrim(contrast_labels[index_conditions[i,0:sumrows[0]-1]],2),'+',/SINGLE)
            time_label[i+1] = 'R seed:'+strjoin(strtrim(contrast_labels[index_conditions[i+1,0:sumrows[1]-1]],2),'-',/SINGLE)
        endfor
    endif else if (test_type eq !CORRELATION or test_type eq !PARTIAL_CORRELATION or test_type eq !LINEARREGRESS) and time ne 0 $
        then begin
        loop1 = total(sumrows)
    endif else if test_type ne !CORRELATION and time ne 0 then begin
        if n_elements(invvar_poly) eq 0 then invvar_poly = intarr(ntests)
        if total_invvar_poly gt 0 then tc_weights_str = ' -tc_weights'
        rtn_cop = replicate({Cop_struct},ntests)
        for i=0,ntests-1 do begin
            if invvar_poly[i] eq 0 then begin
                if total_invvar_poly gt 0 then tc_weights_str = tc_weights_str + ' 1e-37'
            endif else begin
                rtn_cop[i] = coeff_orthog_poly(invvar_poly[i],sumrows[i])
                coeffstr = *rtn_cop[i].coeffstr
                task_label[i] = strjoin(coeffstr+'_'+contrast_labels[index_conditions[i,0:sumrows[i]-1]],/SINGLE)
                tc_weights_str = tc_weights_str + ' ' + strjoin(coeffstr,',',/SINGLE)
            endelse
        endfor
    endif else if n_elements(task_label) eq 0 then begin 
        task_label = strarr(ntests)
        for i=0,ntests-1 do task_label[i] = strjoin(contrast_labels[index_conditions[i,0:sumrows[i]-1]],'+',/SINGLE)
    endif
endif
if test_type eq !PAIRED_COMPARISON or test_type eq !TYPE_II_REGRESSION then begin
    files_per_test = 2
    files_per_test_time = 1
endif
if keyword_set(SS) then files_per_test = 2
goback17c:
if time eq 0 then begin
    if lc_glm ne 0 then begin
        max_effect_length_max = ifh_dim4[0]
    endif else begin
        effect_length_min = intarr(loop)
        for i=0,loop-1 do effect_length_min[i] = min(effect_length[index_conditions[i,0:sumrows[i]-1]])
        effect_length_max = intarr(loop)
        for i=0,loop-1 do begin
            for j=0,nsubject-1 do begin
                index = where(sum_contrast_for_treatment[j,i,*] gt 0,count)
                if count ne 0 then begin
                    penguin = effect_length_ts[j,sum_contrast_for_treatment[j,i,index]-1]
                    pmax = max(penguin)
                    if pmax gt effect_length_max[i] then effect_length_max[i] = pmax
                endif
            endfor
        endfor
        max_effect_length_max = max(effect_length_max)
    endelse
    goback17a:
    lc_tc=0
    if lc_glm eq 1 then begin
        if lc_tc eq 0 then begin
            goback17b:
            ntests=1
            if ifh_dim4[0] gt 1 then begin

                ;ntests = get_button([strtrim(indgen(ifh_dim4[0])+1,2),'GO BACK'],TITLE='How many T tests?')
                ;START150827
                if not keyword_set(REGRESS) then begin
                    ntests = get_button([trim(indgen(ifh_dim4[0])+1),'GO BACK'],TITLE='How many T tests?')
                endif else begin
                    ntests = get_button([trim(indgen(ifh_dim4[0])+1),'GO BACK'],TITLE='How many data sets? A model is '$
                        +'computed for each data set.',BASE_TITLE='Dependent variables')
                endelse

                if ntests eq ifh_dim4[0] then goto,goback17a
                ntests = ntests+1
            endif

            ;task_label = ['test'+strtrim(indgen(ntests)+1,2)]
            ;START150827
            if not keyword_set(REGRESS) then $
                task_label=['test'+trim(indgen(ntests)+1)] $
            else $
                task_label=['data'+trim(indgen(ntests)+1)]

            if test_type eq !PAIRED_COMPARISON then begin
                loop = 2*ntests
                nglms_per_sub = 2
            endif else $
                loop = ntests
        endif
        effect_length_min = make_array(loop,/INT,VALUE=ifh_dim4[0])
        effect_length_max = effect_length_min 
        if lc_tc eq 0 then begin
            length_label = 1
            period = ifh_dim4[0]
            cstr_uniq = make_array(total_nsubjects,/STRING,VALUE='')
            if test_type eq !PAIRED_COMPARISON then begin
                sum_contrast_for_treatment = intarr(nsubject,ntests*2,length_label)
                sum_contrast_for_treatment[*,indgen(ntests)*2,0]=1
                sum_contrast_for_treatment[*,(indgen(ntests)*2)+1,0]=2
            endif else begin
                sum_contrast_for_treatment = intarr(total_nsubjects,ntests,length_label)
                sum_contrast_for_treatment[*,*,0]=1
            endelse
            effect_length_ts = make_array(nsubject,loop,/INT,VALUE=period)
        endif
    endif
endif
timept_label = make_array(loop,/STRING,VALUE='')
if time eq 0 then begin
    if lc_tc eq 2 then $
        goto,goback169 $
    else if lc_tc eq 1 then begin
        c = read_contrast_file(constr,wstr,float_cdata)
        if c.name eq 'EXIT' then return,rtn={msg:'EXIT'} else if c.name eq 'GO_BACK' then goto,goback17a 
        scrap = ''
        dummy = ''
        for i=0,c.NF-1 do begin
            scrap = scrap + ' ' + constr[i]
            dummy = dummy + ' ' + wstr[i]
        endfor
        if files_per_test eq 2 and nglms_per_sub eq 1 then begin
            for i=0,c.NF-1 do begin
                scrap = scrap + ' ' + constr[i]
                dummy = dummy + ' ' + wstr[i]
            endfor
        endif
        if lc_glm ne 0 then begin
            task_label = c.hdr
            ntests = c.NF
            bf_str = ' -frames_of_interest' + scrap + ' -tc_weights' + dummy
        endif else begin
            bf_str = ' -tc_weights' + dummy
            if nglms_per_sub eq 1 then begin
                filecounteach = make_array(total_nsubjects,loop,/INTEGER,VALUE=c.NF)
                for j=0,total_nsubjects-1 do begin
                    cstr[j] = ''
                    scrap = make_array(loop*c.NF,/STRING,VALUE='')
                    n = 0
                    m = 0
                    for i=0,ntests-1 do begin
                        for k=0,c.NF-1 do begin
                            start = 0
                            spider = ' '
                            dummy = ''
                            for o=0,sumrows[m]-1 do begin
                                goose = effect_length_ts[j,sum_contrast_for_treatment[j,m,o]-1]
                                index = where(float_cdata[k,start:start+goose-1] ne 0.,count)
                                rtn=get_cstr_new2(dummy,sum_contrast_for_treatment[j,m,o],effect_length_ts[j,*],index+1,count,spider)
                                start = start + goose
                                spider = '+'
                            endfor
                            cstr[j] = cstr[j] + dummy
                            scrap[n] = dummy
                            n = n + 1
                            if files_per_test eq 2 then begin
                                start = 0
                                spider = ' '
                                dummy = ''
                                for o=0,sumrows[m]-1 do begin
                                    goose = effect_length_ts[j,sum_contrast_for_treatment[j,m,o]-1]
                                    index = where(float_cdata[k,start:start+goose-1] ne 0.,count)
                                    rtn=get_cstr_new2(dummy,sum_contrast_for_treatment[j,m+1,o],effect_length_ts[j,*],index+1, $
                                        count,spider)
                                    start = start + goose
                                    spider = '+'
                                endfor
                                cstr[j] = cstr[j] + dummy
                                scrap[n] = dummy
                                n = n + 1
                            endif
                        endfor
                        m = m + files_per_test
                    endfor
                    scrap = scrap[uniq(scrap,sort(scrap))]
                    for i=0,n_elements(scrap)-1 do cstr_uniq[j] = cstr_uniq[j] + ' ' + strtrim(scrap[i],2)
                endfor
            endif else begin
                for j=0,nglm-1 do begin
                    cstr[j*2] = ''
                    scrap = make_array(loop*c.NF,/STRING,VALUE='')
                    n = 0
                    m = 0
                    for i=0,ntests-1 do begin
                        for k=0,c.NF-1 do begin
                            start = 0
                            spider = ' '
                            dummy = ''
                            for o=0,sumrows[index1[i]]-1 do begin
                                goose = effect_length_ts[j,sum_contrast_for_treatment1[j,i,o]-1]
                                index = where(float_cdata[k,start:start+goose-1] ne 0.,count)
                                rtn=get_cstr_new2(dummy,sum_contrast_for_treatment1[j,i,o],effect_length_ts[glmindex1[j],*], $
                                    index+1,count,spider)
                                start = start + goose
                                spider = '+'
                            endfor
                            cstr[j*2] = cstr[j*2] + dummy
                            scrap[n] = dummy
                            n = n + 1
                        endfor
                    endfor
                    scrap = scrap[uniq(scrap,sort(scrap))]
                    for i=0,n_elements(scrap)-1 do cstr_uniq[j*2] = cstr_uniq[j*2] + ' ' + strtrim(scrap[i],2)
                endfor
                for j=0,nglm-1 do begin
                    cstr[j*2+1] = ''
                    scrap = make_array(loop*c.NF,/STRING,VALUE='')
                    n = 0
                    m = 0
                    for i=0,ntests-1 do begin
                        for k=0,c.NF-1 do begin
                            start = 0
                            spider = ' '
                            dummy = ''
                            for o=0,sumrows[index2[i]]-1 do begin
                                goose = effect_length_ts[j,sum_contrast_for_treatment2[j,i,o]-1]
                                index = where(float_cdata[k,start:start+goose-1] ne 0.,count)
                                rtn=get_cstr_new2(dummy,sum_contrast_for_treatment2[j,i,o],effect_length_ts[glmindex2[j],*],index+1, $
                                    count,spider)
                                start = start + goose
                                spider = '+'
                            endfor
                            cstr[j*2+1] = cstr[j*2+1] + dummy
                            scrap[n] = dummy
                            n = n + 1
                        endfor
                    endfor
                    scrap = scrap[uniq(scrap,sort(scrap))]
                    for i=0,n_elements(scrap)-1 do cstr_uniq[j*2+1] = cstr_uniq[j*2+1] + ' ' + strtrim(scrap[i],2)
                endfor
            endelse
        endelse
    endif else if lc_tc eq 0 then begin
        if keyword_set(SLOPES) then begin
            conditions_time = [1,2,1,2]
            sumrows_time = [1,1,1,1] 
            timept_label = ['_1','_2','_1','_2']
        endif else begin 

            ;START151230
            goback17d:

            conditions_time = intarr(loop,max_effect_length_max)
            sumrows_time = intarr(loop)
            if max_effect_length_max gt 1 then begin
                if keyword_set(REGRESS) and loop eq max_effect_length_max then begin 
                    conditions_time[*,0]=indgen(loop)+1
                    sumrows_time[*] = 1

                    ;timept_label[*]='_'+trim(indgen(loop)+1)
                    ;START151230
                    timept_label[*]=trim(indgen(loop)+1)

                endif else begin

                    ;scraparr = strarr(max_effect_length_max+1,loop)
                    ;for i=0,loop-1 do scraparr[0:effect_length_max[i],i] = [strtrim(indgen(effect_length_max[i])+1,2),'ALL']
                    ;START161221
                    scraparr=strarr(max_effect_length_max,loop)
                    for i=0,loop-1 do scraparr[0:effect_length_max[i]-1,i]=trim(indgen(effect_length_max[i])+1)

                    if lc_glm eq 0 then begin
                        title = 'Multiple timepoints are combined as a weighted mean'+string(10B)
                        if loadslk eq 0 then $
                            title = title + 'by the inverse of their variance.' $
                        else $
                            title = title + 'by the weighting in your slk file.'
                    endif else $
                        title = ''
                    scraplabel = strarr(loop)
                    if test_type eq !PAIRED_COMPARISON then begin
                        if n_elements(time_label) ne 0 and lc_glm eq 0 then begin
                            if n_elements(time_label1) ne 0 then scraplabel = time_label1 else scraplabel = time_label

                            ;print,'here0 group_names=',group_names,'END'
                            ;print,'here0 n_elements(group_names)=',n_elements(group_names)

                            ;if n_elements(group_names) ne 0 then begin
                            ;START170713
                            if keyword_set(group_names[0]) then begin

                                k = 0
                                for i=0,ntests-1 do begin
                                    for j=0,1 do begin
                                        scraplabel[k] = scraplabel[k] + ' ' + group_names[j]
                                        k = k + 1
                                    endfor
                                endfor
                            endif
                        endif else begin
                            k = 0
                            for i=0,ntests-1 do begin
                                for j=0,1 do begin
                                    scraplabel[k] = task_label[i] + ' ' + group_names[j]
                                    k = k + 1
                                endfor
                            endfor
                        endelse
                    endif else if test_type eq !UNPAIRED_COMPARISON and n_elements(contrast_labels) ne 0 then begin
                        for i=0,loop-1 do $
                            scraplabel[i] = strjoin(strtrim(contrast_labels[index_conditions[i,0:sumrows[i]-1]],2),'+',/SINGLE)
                    endif else begin
                        if time_label[0] eq '' then scraplabel[*]=task_label else scraplabel[*]=time_label
                    endelse
                    widget_control,/HOURGLASS

                    ;rtn = get_choice_list_many(scraplabel,scraparr,TOP_TITLE='Please select timepoints.',TITLE=title, $
                    ;    /LABEL_TOP,/NONEXCLUSIVE,/GO_BACK,LENGTHS=effect_length_max+1,/SET_DIAGONAL)
                    ;START161221
                    rtn = get_choice_list_many(scraplabel,scraparr,TOP_TITLE='Please select timepoints.',TITLE=title, $
                        /LABEL_TOP,/NONEXCLUSIVE,/GO_BACK,LENGTHS=effect_length_max,/SET_DIAGONAL,/SET_UNSET,/CHECK)

                    if rtn.special eq 'GO_BACK' then begin
                        if (test_type eq !UNPAIRED_COMPARISON or test_type eq !GROUP_COMPARISON or test_type eq !CORRELATION) and $
                            loadslk eq 1 then goto,goback159a else goto,goback17a
                    endif

                    ;START161221
                    ;scraplist = rtn.list

                    if files_per_test_time eq 0 then files_per_test_time=files_per_test
                    for i=0,loop-1 do begin

                        ;if scraplist[effect_length_max[i],i] eq 1 then scraplist[0:effect_length_max[i]-1,i] = 1
                        ;index = where(scraplist[0:effect_length_max[i]-1,i],count)
                        ;START161221
                        index=where(rtn.list[0:effect_length_max[i]-1,i] gt 0,count)

                        if count ne 0 then begin
                            conditions_time[files_per_test_time*i,0:count-1] = index + 1
                            sumrows_time[files_per_test_time*i] = count
                            timept_label[i]=strjoin(strtrim(index+1,2),'+',/SINGLE)
                            if files_per_test_time eq 2 then begin
                                conditions_time[files_per_test_time*i+1,0:count-1] = index + 1
                                sumrows_time[files_per_test_time*i+1] = count
                            endif
                        endif
                    endfor

                    ;START151230
                    ;index = where(timept_label ne '',count)
                    ;if count ne 0 then timept_label[index] = '_' + timept_label[index]

                endelse


                ;START151230
                if keyword_set(REGRESS) then begin
                    goback17e:
                    scrap=get_str(n_elements(timept_label),timept_label,timept_label,TITLE='Map labels',LABEL='Maps will have ' $ 
                        +'these labels.'+string(10B)+'You may edit these labels or load a file with the labels.',/REPLACE,/RESET, $
                        FRONT='1',BACK='1',SPECIAL='Load file',/GO_BACK,/EXIT)
                    if scrap[0] eq 'EXIT' then return,rtn={msg:'EXIT'} else if scrap[0] eq 'GO_BACK' then begin
                        if keyword_set(REGRESS) and loop eq max_effect_length_max then goto,goback17c else goto,goback17d 
                    endif
                    if scrap[0] ne 'SPECIAL' then $

                        ;timept_label=fix_script_name(scrap,/SPACE_ONLY) $
                        ;START160106
                        timept_label='data'+fix_script_name(scrap,/SPACE_ONLY) $

                    else begin
                        goback17f:
                        get_dialog_pickfile,'*.txt',fi.path,'Please select file.',rtn_file,rtn_nfiles,rtn_path
                        if rtn_nfiles eq 0 then goto,goback17e
                        rtn=readf_ascii_file(rtn_file,/NOHDR)
                        if rtn.NR lt n_elements(timept_label) then begin
                            scrap=get_button(['go back','exit'],BASE_TITLE=rtn_file, $
                                TITLE='You need '+trim(n_elements(timept_label))+' but you only have '+trim(rtn.NR)+' labels.')
                            if scrap eq 1 then return,rtn={msg:'EXIT'} else goto,goback17f
                        endif else if rtn.NR eq n_elements(timept_label) then begin
                            scrap=get_str(n_elements(timept_label),timept_label,rtn.data[0,*],TITLE='Map labels', $
                                LABEL='Maps will have these labels.'+string(10B)+'You may edit these labels or load a file with ' $
                                +'the labels.',/REPLACE,/RESET,FRONT='1',BACK='1',SPECIAL='Load file',/GO_BACK,/EXIT,/ONE_COLUMN)
                            if scrap[0] eq 'EXIT' then return,rtn={msg:'EXIT'} else if scrap[0] eq 'GO_BACK' then goto,goback17f
                            timept_label=fix_script_name(scrap)
                        endif else begin
                            scrap=get_str(n_elements(timept_label),timept_label,rtn.data[0,*],TITLE='Map labels', $
                                LABEL='Maps will have these labels.'+string(10B)+'You may edit these labels or load a file with ' $
                                +'the labels.',/REPLACE,/RESET,FRONT='1',BACK='1',SPECIAL='Load file',/GO_BACK,/EXIT,/ONE_COLUMN)
                            if scrap[0] eq 'EXIT' then return,rtn={msg:'EXIT'} else if scrap[0] eq 'GO_BACK' then goto,goback17f
                            timept_label=fix_script_name(scrap)
                        endelse
                    endelse
                endif
                index = where(timept_label ne '',count)
                if count ne 0 then timept_label[index] = '_' + timept_label[index]


            endif else begin
                conditions_time[*,*] = 1
                sumrows_time[*] = 1
            endelse
        endelse
        if lc_glm eq 0 then junk = total_nsubjects else junk = 1
        ne_scrap = loop
        filecounteach = intarr(nsubject,ne_scrap)
        if test_type eq !PAIRED_COMPARISON or test_type eq !TYPE_II_REGRESSION then begin
            for j=0,nsubject-1 do begin
                cstr[j] = ''
                scrap = make_array(ne_scrap,/STRING,VALUE='')
                i = 0
                for m=0,ntests-1 do begin
                    c1 = total(sum_contrast_for_treatment[j,i,*])
                    c2 = total(sum_contrast_for_treatment[j,i+1,*])
                    if c1 eq 0 or c2 eq 0 then $
                        i = i + 2 $
                    else begin
                        for n=0,pairs-1 do begin
                            dummy = ''
                            rtn = get_cstr_new2(dummy,sum_contrast_for_treatment[j,i,*],effect_length_ts[j,*],conditions_time[i,*], $
                                sumrows_time[i],' ') ;This must be a space. ' '
                            filecounteach[j,i] = rtn.present
                            cstr[j] = cstr[j] + dummy
                            scrap[i] = dummy
                            i = i + 1
                        endfor
                    endelse
                endfor
                if total_invvar_poly gt 0 then $
                    cstr_uniq[j] = cstr[j] $
                else begin
                    scrap = scrap[uniq(scrap,sort(scrap))]
                    cstr_uniq[j] = cstr_uniq[j] + ' ' + strjoin(strtrim(scrap,2),' ',/SINGLE)
                endelse
            endfor
        endif else begin
            for j=0,nsubject-1 do begin
                cstr[j] = ''
                scrap = make_array(ne_scrap,/STRING,VALUE='')
                for i=0,loop-1 do begin

                    ;print,'j=',j,' i=',i,' sum_contrast_for_treatment[j,i,*]=',sum_contrast_for_treatment[j,i,*]
                    ;print,'conditions_time[i,*]=',conditions_time[i,*]
                    ;print,'sumrows_time[i]=',sumrows_time[i]

                    dummy = ''
                    rtn = get_cstr_new2(dummy,sum_contrast_for_treatment[j,i,*],effect_length_ts[j,*],conditions_time[i,*], $
                        sumrows_time[i],' ') ;This must be a space. ' '
                    filecounteach[j,i] = rtn.present
                    cstr[j] = cstr[j] + dummy
                    scrap[i] = dummy
                endfor
             
                ;print,'j=',j,' cstr=',cstr[j]    

                if total_invvar_poly gt 0 or loadslk eq 1 then $
                    cstr_uniq[j] = cstr[j] $
                else begin
                    scrap = scrap[uniq(scrap,sort(scrap))]
                    cstr_uniq[j] = cstr_uniq[j] + ' ' + strjoin(strtrim(scrap,2),' ',/SINGLE)
                endelse
            endfor
            if loadslk eq 1 then begin
                s0=0
                for i=0,ntests-1 do begin
                    scrap1 = strarr(sumrows_time[i])
                    scraparr = intarr(nglm+nglm_2)
                    for j=0,sumrows[i]-1 do scraparr = scraparr + (behav_data1[*,indvar1[i,j]] eq '')
                    index = where(scraparr eq 0,count)
                    if count ne 0 then begin
                        for j=0,count-1 do begin
                            scrap2 = ''
                            for k=0,sumrows[i]-1 do begin
                                scrap1[*] = behav_data1[index[j],indvar1[i,k]]
                                scrap2 = [scrap2,scrap1]
                            endfor
                            cstrwts[index[j]] = cstrwts[index[j]] + ' ' + strcompress(strjoin(scrap2[1:*],',',/SINGLE),/REMOVE_ALL)
                        endfor
                    endif
                    s0 = s0 + sumrows[i]
                endfor
                index = where(cstrwts ne '',count)
                if count ne 0 then cstrwts[index] = ' -tc_weights'+cstrwts[index]
            endif
        endelse
        if ne_scrap eq 1 then filecount = filecounteach else filecount = total(filecounteach,2)
        if lc_glm ne 0 then bf_str = ' -frames_of_interest ' + cstr
    endif
endif else begin
    if lc_glm eq 0 then begin
        ne_scrap = loop1/nglms_per_sub
        filecounteach = intarr(total_nsubjects,ne_scrap)
        if nglms_per_sub eq 1 then begin
            loopi = intarr(loop,max(sumrows))
            loopc = intarr(loop)
            k = 0
            for j=0,loop-1 do begin
                for m=0,sumrows[j]-1 do begin
                    idx = where(sum_contrast_for_treatment[*,j,m] ne 0,cnt)
                    if cnt ge 2 then begin
                        loopi[j,loopc[j]] = m
                        loopc[j] = loopc[j] + 1
                    endif
                    k = k + 1
                endfor
            endfor
            filecount = intarr(total_nsubjects)
            scftstr = strtrim(sum_contrast_for_treatment,2)
            for j=0,total_nsubjects-1 do begin
                cstr[j] = ''
                scrap = make_array(total(sumrows),/STRING,VALUE='')
                k = 0
                i1 = 0
                for m=0,loop-1 do begin
                    if loopc[m] gt 0 then begin
                        index = where(sum_contrast_for_treatment[j,m,loopi[m,0:loopc[m]-1]] ne 0,count)
                        if count ne 0 then begin
                            cstr[j] = cstr[j] + ' ' + strjoin(scftstr[j,m,index],' ',/SINGLE)
                            scrap[k:k+count-1] = scftstr[j,m,index]
                            filecounteach[j,i1+index] = 1
                            filecount[j] = filecount[j] + count
                            k = k + count
                        endif
                    endif
                    i1 = i1 + sumrows[m]
                endfor
                scrap = scrap[uniq(scrap,sort(scrap))]
                for i=0,n_elements(scrap)-1 do cstr_uniq[j] = cstr_uniq[j] + ' ' + strtrim(scrap[i],2)
            endfor

            ;START140324
            if keyword_set(SS) then begin
                filecount = filecount*2
                filecounteach = filecounteach*2
            endif

        endif else begin
            filecounteach[*,*] = 1 
            scftstr = strtrim(sum_contrast_for_treatment1,2)
            for j=0,nglm-1 do begin
                cstr[j*2] = ''
                scrap = make_array(total(sumrows[index1]),/STRING,VALUE='')
                k = 0
                for m=0,ntests-1 do begin
                    cstr[j*2] = cstr[j*2] + ' ' + strjoin(scftstr[j,m,0:sumrows[index1[m]]-1],' ',/SINGLE)
                    scrap[k:k+sumrows[index1[m]]-1] = scftstr[j,m,0:sumrows[index1[m]]-1]
                    k = k + sumrows[index1[m]]
                endfor
                scrap = scrap[uniq(scrap,sort(scrap))]
                for i=0,n_elements(scrap)-1 do cstr_uniq[j*2] = cstr_uniq[j*2] + ' ' + strtrim(scrap[i],2)
            endfor
            scftstr = strtrim(sum_contrast_for_treatment2,2)
            for j=0,nglm-1 do begin
                cstr[j*2+1] = ''
                scrap = make_array(total(sumrows[index2]),/STRING,VALUE='')
                k = 0
                for m=0,ntests-1 do begin
                    cstr[j*2+1] = cstr[j*2+1] + ' ' + strjoin(scftstr[j,m,0:sumrows[index2[m]]-1],' ',/SINGLE)
                    scrap[k:k+sumrows[index2[m]]-1] = scftstr[j,m,0:sumrows[index2[m]]-1]
                    k = k + sumrows[index2[m]]
                endfor
                scrap = scrap[uniq(scrap,sort(scrap))]
                for i=0,n_elements(scrap)-1 do cstr_uniq[j*2+1] = cstr_uniq[j*2+1] + ' ' + strtrim(scrap[i],2)
            endfor
        endelse
    endif
endelse
ntests1 = ntests
goback162a:
lc162a=0
if lc_glm eq 0 and test_type ne !LINEARREGRESS then begin
    title = 'Please name.'
    if keyword_set(SS) then begin
        wallace = *subject_get_labels_struct[0].contrastlabels
        darby = *subject_get_labels_struct[1].contrastlabels
        task_label1 = strarr(loop/2)
        j = 0
        for i=0,loop-1,2 do begin
            scrap = darby[index_conditions[i+1,0:sumrows[i+1]-1]] 
            lizard1=strpos(scrap,'-')
            lizard2=strpos(scrap,'+')
            effect_label=*subject_get_labels_struct[1].effectlabels
            idx = where(scrap eq effect_label,cnt)
            if cnt ne 0 and (lizard1[0] ne -1 or lizard2[0] ne -1) then begin
                task_label1[j] = strjoin(strtrim(wallace[index_conditions[i,0:sumrows[i]-1]],2),'+',/SINGLE)+timept_label[i] $
                    +'_-_'+strjoin(strtrim(darby[index_conditions[i+1,0:sumrows[i+1]-1]],2),'-',/SINGLE)+timept_label[i+1]
            endif else begin
                scrap = scrap[0]
                plus = strsplit(scrap,'+')
                minus = strsplit(scrap,'-')
                for k=0,n_elements(plus)-1 do if plus[k] ne 0 then strput,scrap,'-',plus[k]-1
                for k=0,n_elements(minus)-1 do if minus[k] ne 0 then strput,scrap,'+',minus[k]-1
                task_label1[j] = strjoin(strtrim(wallace[index_conditions[i,0:sumrows[i]-1]],2),'+',/SINGLE)+timept_label[i] $
                    +'-'+scrap+timept_label[i+1]
            endelse
            j = j + 1
        endfor
    endif else if test_type eq !PAIRED_COMPARISON then begin
        task_label1 = strarr(loop/2)
        j = 0
        for i=0,loop-1,2 do begin
            task_label1[j] = strjoin(strtrim(contrast_labels[index_conditions[i,0:sumrows[i]-1]],2),'+',/SINGLE)+timept_label[i] $
                +'-'+strjoin(strtrim(contrast_labels[index_conditions[i+1,0:sumrows[i+1]-1]],2),'-',/SINGLE)+timept_label[i+1]
            j = j + 1
        endfor
    endif else if test_type eq !TYPE_II_REGRESSION then begin
        task_label1 = strarr(loop/2)
        title = 'Please name type II regressions.'
        j = 0
        for i=0,loop-1,2 do begin
            task_label1[j] = strjoin(strtrim(contrast_labels[index_conditions[i,0:sumrows[i]-1]],2),'+',/SINGLE)+timept_label[i] $
                +'vs'+strjoin(strtrim(contrast_labels[index_conditions[i+1,0:sumrows[i+1]-1]],2),'+',/SINGLE)+timept_label[i+1]
            j = j + 1
        endfor
    endif else if test_type eq !UNPAIRED_COMPARISON or test_type eq !GROUP_COMPARISON or test_type eq !SS_VS_GROUP then begin
        task_label1 = strarr(loop)
        for i=0,loop-1 do $
            task_label1[i] = strjoin(strtrim(contrast_labels[index_conditions[i,0:sumrows[i]-1]],2),'+',/SINGLE)+timept_label[i]
    endif else if test_type eq !CORRELATION then begin
        if time eq 0 then begin
            task_label1=task_label+timept_label
        endif else begin
            task_label1 = strarr(total(sumrows))
            k=0
            for i=0,ntests-1 do begin
                for j=0,sumrows[i]-1 do begin
                    task_label1[k]=contrast_labels[index_conditions[i,j]]+'_and_'+behav_str[i]
                    k=k+1
                endfor
            endfor
        endelse
    endif else if test_type eq !PARTIAL_CORRELATION then begin
        task_label1=task_label+timept_label
    endif
    scrap = get_str(n_elements(task_label1),task_label1,task_label1,TITLE=title,/FRONT,/BACK,/GO_BACK,WIDTH=50)
    if scrap[0] eq 'GO_BACK' then begin
        if time eq 0 then goto,goback17c else goto,goback169
    endif
    task_label = scrap
    lc162a=1
endif
goback163:
lc163=0



;if atlasspace eq !UNKNOWN or test_type eq !TYPE_II_REGRESSION or keyword_set(SLOPES) then begin
;    ;do nothing
;endif else if boldtype eq 'img' then begin
;    if nreg gt 0 then begin
;        analysis_type = 0
;    endif else begin
;        analysis_type = get_button(['voxel by voxel','specfic regions','GO BACK'],TITLE='Please select analysis type.')
;        if analysis_type eq 2 then goto,goback17 
;        lc163=1
;    endelse
;endif
;START191017
if test_type eq !TYPE_II_REGRESSION or keyword_set(SLOPES) then begin
    ;do nothing
endif else begin
    if nreg gt 0 then begin
        analysis_type = 0
    endif else begin
        analysis_type = get_button(['voxel by voxel','specfic regions','GO BACK'],TITLE='Please select analysis type.')
        if analysis_type eq 2 then goto,goback17
        lc163=1
    endelse
endelse



if lc_glm eq 0 then cstr = strtrim(cstr,1)
goback16a:
lc16a=0
if analysis_type eq 1 then begin
    if not keyword_set(SLOPES) then begin
        rtn = get_regions(fi,wd,dsp,help)
        if rtn.msg eq 'EXIT' then return,rtn={msg:'EXIT'} else if rtn.msg eq 'GO_BACK' then goto,goback163
        region_names = rtn.region_names
        region_str = rtn.region_str
        region_file = rtn.region_file
        harolds_num = rtn.harolds_num
        goback16b:
        rtn = select_files(region_names,TITLE='Please select regions.',/GO_BACK,MIN_NUM_TO_SELECT=1)
        if rtn.files[0] eq 'GO_BACK' then goto,goback16a
        region_names = rtn.files
        num_roi = rtn.count
        scrap = strcompress(rtn.index + 1,/REMOVE_ALL)
        roi_str = ' -regions_of_interest'
        for i=0,num_roi-1 do roi_str = roi_str + ' ' + scrap[i]
        region_or_uncompress_str = ' -regions'
        for i=0,num_roi-1 do region_or_uncompress_str = region_or_uncompress_str +' '+ string(34B) + region_names[i] + string(34B)
        region_or_uncompress_str = region_or_uncompress_str + ' -harolds_num'
        harolds_num = strtrim(harolds_num,2)
        for i=0,num_roi-1 do region_or_uncompress_str = region_or_uncompress_str +' '+ harolds_num[i]
    endif
endif else if glm_space[0] ne !UNKNOWN then begin


    ;START170323
    ;if boldtype eq 'img' then begin

        ;if test_type ne !TYPE_II_REGRESSION then begin
        ;START160527
        if test_type ne !TYPE_II_REGRESSION and nreg eq 0 then begin

            fwhm0 = 0.
            if lc_glm eq 0 then begin
                fwhm0 = subject_get_labels_struct[0].ifh.glm_fwhm
            endif else if lc_glm eq 1 then $
                fwhm0 = ifh_fwhm[0]
            if lc_glm eq 0 then scrap = 'GLMs' else scrap = 'images'
            goback16a1:
            if lc_glm ne 0 and fwhm eq 0. then begin

                ;START170821
                if fwhm0 lt 0 then fwhm0=0. 

                fwhm0 = get_str(1,'FWHM in voxels ',fwhm0,WIDTH='50',TITLE='Have your images been smoothed?', $
                    LABEL='Enter 0 if they have not been smoothed.',/GO_BACK)
                if fwhm0[0] eq 'GO_BACK' then goto,goback163
                fwhm0 = double(fwhm0[0])
                lc16a=1
            endif
            goback16a1a:
            if fwhm0 eq 0. then begin
                title = 'You may smooth your '+scrap+' if you wish.'
                label = 'Enter 0 for no smoothing.'
                if nsubject eq 1 or keyword_set(SS) then scrap = '0' else scrap = '2'
            endif else begin
                title = 'You have smoothed '+trim(fwhm0)+' voxels. You may do some additional smoothing if you wish.'
                label = 'Enter 0 for no additional smoothing.'
                scrap = '0'
            endelse
            if keyword_set(SLOPES) then begin
                lizard=trim(fwhm0)
                if fwhm0 gt 0. then label=label+string(10B)+'This is smoothing after slope and intercept estimation.'+string(10B) $
                    +'You are already smoothing '+lizard[0]+' voxels prior to estimation.'
            endif
            scrap = get_str(1,'3D Gaussian kernel FWHM in voxels ',scrap,/GO_BACK,TITLE=title,LABEL=label,WIDTH=50)
            if scrap[0] eq 'GO_BACK' then begin
                if lc16a eq 1 then goto,goback16a1 else if lc162a eq 1 then goto,goback162a else goto,goback169
            endif
            fwhm = double(scrap[0])
            gauss_str=''
            gauss_str_ttest=''
            if fwhm gt 0.01 then begin
                if not keyword_set(SS) then gauss_str=' -gauss_smoth '+trim(fwhm) else gauss_str_ttest=' -gauss_smoth '+trim(fwhm)
            endif
            monte_carlo_fwhm = round(sqrt(fwhm0^2+fwhm^2))
        endif
        goback16a2:
        mask = ''
        if lc_glm eq 0 then begin
            if ptr_valid(subject_get_labels_struct[0].ifh.glm_mask_file) then mask = *subject_get_labels_struct[0].ifh.glm_mask_file
        endif
        if keyword_set(SLOPES) then $
            region_or_uncompress_str = ' -uncompress ' + mask $
        else if nreg eq 0 then begin

            ;if mask eq '' then begin
            ;    mask = get_mask(atlasspace,fi)
            ;    if mask eq 'GO_BACK' then goto,goback16a1a
            ;endif
            ;START170524
            mask=get_mask(atlasspace,fi,mask)
            if mask eq 'GO_BACK' then goto,goback16a1a

            if mask ne '' then begin
                compress_str = ' -compress ' + mask
                region_or_uncompress_str = ' -uncompress ' + mask
                Nimage_mask_str = ' -Nimage_mask ' + mask
            endif
        endif

    ;START170323
    ;endif


    if test_type ne !TYPE_II_REGRESSION and test_type ne !LINEARREGRESS and nreg eq 0 then begin
        goback16a3:

        ;if mask ne '' and (atlasspace eq !SPACE_111 or atlasspace eq !SPACE_222 or atlasspace eq !SPACE_333) then begin 
        ;START170323
        if mask ne '' and atlasspace ne !UNKNOWN then begin 

            option_list = get_bool_list(['Uncorrected T-maps','Uncorrected Z-maps','Monte Carlo Corrected T-maps', $
                'Monte Carlo Corrected Z-maps'],TITLE='Please select output options.',/GO_BACK)
            option_list = option_list.list
            if option_list[0] eq -1 then goto,goback16a2
            goback16a4:
            if option_list[2]+option_list[3] gt 0 then begin
                rtn = threshold_extent(!T_STAT,monte_carlo_fwhm,atlasspace,mask)
                threshold_extent_str = rtn.threshold_extent_str
                if threshold_extent_str eq 'GO_BACK' then goto,goback16a3
                if threshold_extent_str eq 'ERROR' then option_list[[1,2,3]] = [1,0,0]
            endif
        endif else begin
            scrap = get_bool_list(['Uncorrected T-maps','Uncorrected Z-maps'],TITLE='Please select output options.',/GO_BACK)
            scrap = scrap.list
            if scrap[0] eq -1 then goto,goback16a2
            option_list = intarr(4)
            option_list[[0,1]] = scrap
        endelse
        ttest_output_str = ' -output '
        if option_list[0] eq 1 then ttest_output_str = ttest_output_str + 'T_uncorrected '
        if option_list[1] eq 1 then ttest_output_str = ttest_output_str + 'Z_uncorrected '
        if option_list[2] eq 1 then ttest_output_str = ttest_output_str + 'T_monte_carlo '
        if option_list[3] eq 1 then ttest_output_str = ttest_output_str + 'Z_monte_carlo '
    endif
endif else begin
    if subject_get_labels_struct[0].ifh.nreg gt 0 then $
        region_or_uncompress_str = ' -regions ' + strjoin(*subject_get_labels_struct[0].ifh.region_names,' ',/SINGLE)
endelse
if lc_glm eq 1 then begin
    if test_type eq !LINEARREGRESS then begin
        task_label = behav_str 
    endif else begin
        if test_type eq !UNPAIRED_COMPARISON then begin
            if task_label[0] eq '' then task_label[0] = 'task'
        endif else if test_type eq !PAIRED_COMPARISON or test_type eq !GROUP_COMPARISON or test_type eq !SS_VS_GROUP then begin
            for i=0,ntests-1 do begin
                if task_label[i] ne '' then task_label[i] = task_label[i] + '_'
                task_label[i] = task_label[i]+group_names[0]+'-'+group_names[1]
            endfor
        endif else if test_type eq !CORRELATION then begin
            task_label = behav_str
            if n_elements(task_label) lt ntests then task_label=make_array(ntests,/STRING,VALUE=task_label[0])+trim(indgen(ntests)+1)
        endif
        task_label = get_str(ntests,'test'+trim(indgen(ntests)+1),task_label,WIDTH=50,TITLE='Please name T tests.')
    endelse
endif




goback18:
if analysis_type eq 0 and (test_type eq !UNPAIRED_COMPARISON or test_type eq !PAIRED_COMPARISON or test_type eq  $
    !GROUP_COMPARISON or test_type eq !SS_VS_GROUP) then begin
    scrap = get_str(1,'variance threshold','1e-10',WIDTH=50,LABEL='Square root of the variance goes into the denominator ' $
        +'of the T statistic.'+string(10B)+'If the variance is smaller than this number, the voxel is set to 1e-37.', $
        TITLE='Units are percent change squared for BOLD data.',/GO_BACK)
    if scrap[0] eq 'GO_BACK' then begin
        if glm_space[0] ne !UNKNOWN then goto,goback16a3 else goto,goback16a
    endif
    var_thresh_str = ' -var_thresh "'+ scrap[0] + '"'
endif
goback18a:
if test_type eq !UNPAIRED_COMPARISON or test_type eq !PAIRED_COMPARISON then begin
    if lc_glm ne 2 then begin
        scrap = 'test' + strtrim(indgen(ntests)+1,2) + '    ' + task_label
        scrap2 = make_array(ntests,/STRING,VALUE='0')
        scrap = get_str(ntests,scrap,scrap2,TITLE='Please enter the value associated with the null hypothesis.', $
            LABEL='If you do not know what this value should be, then keep it at zero.',/GO_BACK)
        if scrap[0] eq 'GO_BACK' then goto,goback18
    endif 
endif


;START160317
if scratchdir eq '' then begin
    scratchdir = fidl_scratchdir(/NOPATH)
    if scratchdir eq 'ERROR' then return,rtn={msg:'EXIT'}
    print,'scratchdir = ',scratchdir
    scratchdir_str = ' -scratchdir ' + scratchdir

    ;times0=strmid(scratchdir,transpose(strpos(scratchdir,'SCRATCH_')+8))
    ;times0=strmid(times0,0,transpose(strlen(times0)-1))
    ;START160317
    times0=strmid(scratchdir,strpos(scratchdir,'SCRATCH_')+8)
    times0=strmid(times0,0,strlen(times0)-1)

endif


goback19:
if test_type ne !LINEARREGRESS and not keyword_set(SLOPES) then begin
    scrap = ['driving file','t-test script']
    scraplabels = ['fidl_ttest_'+times0+'.dat','fidl_ttest_'+times0+'.csh']
    if lc_glm eq 0 and test_type ne !TYPE_II_REGRESSION and not keyword_set(SS) then begin
        scrap = [scrap,'average statistics script']
        if time eq 0 then scraplabels = [scraplabels,'fidl_ttest_avgtc.csh'] else scraplabels = [scraplabels,'fidl_ttest_avgmag.csh']
    endif


    ;if analysis_type eq 1 or atlasspace eq !UNKNOWN or test_type eq !TYPE_II_REGRESSION then begin
    ;START160527
    if analysis_type eq 1 or atlasspace eq !UNKNOWN or test_type eq !TYPE_II_REGRESSION or nreg ne 0 then begin

        scrap = [scrap,'t-test output']

        ;scraplabels = [scraplabels,'fidl_ttest_region_results.txt']
        ;START160527
        scraplabels = [scraplabels,'fidl_ttest_region_results_'+times0+'.txt']

        if lc_glm eq 0 and test_type ne !TYPE_II_REGRESSION then begin
            if test_type eq !GROUP_COMPARISON then begin
                scrap = [scrap,'average statistics output, group 1','average statistics output, group 2']
                scraplabels = $
                    [scraplabels,'avg_stats_for_regions_'+group_names[0]+'.txt','avg_stats_for_regions_'+group_names[1]+'.txt']
            endif else begin
                scrap = [scrap,'average statistics output']
                scraplabels = [scraplabels,'avg_stats_for_regions.txt']
            endelse
        endif

    endif else if not keyword_set(SS) then begin
    ;START1160527
    ;endif else if not keyword_set(SS) and nreg eq 0 then begin

        scrap = [scrap,'Number of subjects at each voxel. If blank, no output.']
        scraplabels = [scraplabels,'fidl_ttest_Nimage.4dfp.img']
    endif

    ;if glm_list_str[0] eq '' and not keyword_set(SS) then begin
    ;START160527
    if glm_list_str[0] eq '' and not keyword_set(SS) and lc_interactive eq 1 then begin

        scrap = [scrap,'list']
        scraplabels = [scraplabels,'fidl_ttest.list']
    endif
    if analysis_type eq 0 then begin
        scrap = [scrap,'prepend - if path only, then end with /']
        scraplabels = [scraplabels,'']
    endif
    names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=50,TITLE='Please enter desired filenames.',/GO_BACK)
    if names[0] eq 'GO_BACK' then goto,goback16a4
    driver = directory+'/'+names[0]
    fidl_ttest_csh = directory+'/'+fix_script_name(names[1])
    j = 1
    if lc_glm eq 0 and test_type ne !TYPE_II_REGRESSION and not keyword_set(SS) then begin
        j = j + 1
        compute_avg_zstat_csh_str = directory+'/'+fix_script_name(names[j])
    endif

    ;if analysis_type eq 1 or atlasspace eq !UNKNOWN or test_type eq !TYPE_II_REGRESSION then begin
    ;START160527
    if analysis_type eq 1 or atlasspace eq !UNKNOWN or test_type eq !TYPE_II_REGRESSION or nreg ne 0 then begin

        j = j + 1
        ttest_output_str = ' -regional_ttest_name "' + names[j] + '"'
        if lc_glm eq 0 and test_type ne !TYPE_II_REGRESSION then begin
            if test_type eq !GROUP_COMPARISON then begin
                avgstat_output_str = strarr(2)
                j = j + 1
                avgstat_output_str[0] = ' -regional_avgstat_name "' + names[j] + '"'
                j = j + 1
                avgstat_output_str[1] = ' -regional_avgstat_name "' + names[j] + '"'
            endif else begin
                j = j + 1
                avgstat_output_str = ' -regional_avgstat_name "' + names[j] + '"'
            endelse
        endif

    endif else if not keyword_set(SS) then begin
    ;START160527
    ;endif else if not keyword_set(SS) and nreg eq 0 then begin

        if test_type eq !GROUP_COMPARISON or test_type eq !SS_VS_GROUP or nglms_per_sub eq 2 then avgstat_output_str = ['','']
        j = j + 1
        if names[j] ne '' then begin
            if rstrpos(names[j],'.4dfp.img') eq -1 then names[j] = names[j] + '.4dfp.img'
            Nimage_name_str = ' -Nimage_name "' + names[j] + '"'
        endif
    endif

    ;if glm_list_str[0] eq '' and not keyword_set(SS) then begin
    ;START160527
    if glm_list_str[0] eq '' and not keyword_set(SS) and lc_interactive eq 1 then begin

        j = j + 1
        glm_list_str = directory+'/'+names[j]
    endif
    if analysis_type eq 0 then begin
        j = j + 1
        scrap = strtrim(names[j],2)
        if scrap ne '' then prependstr = ' -prepend ' + scrap
    endif
endif else if keyword_set(SLOPES) then begin
    names = get_str(1,'output','fidl_ttest_slopes.txt',WIDTH=50,TITLE='Please enter desired filename.',/GO_BACK)
    if names[0] eq 'GO_BACK' then goto,goback16a4
    ttest_output_str = ' -regional_ttest_name "' + names[0] + '"'
endif

;START160317
;if scratchdir eq '' then begin 
;    scratchdir = fidl_scratchdir(/NOPATH)
;    if scratchdir eq 'ERROR' then return,rtn={msg:'EXIT'}
;    print,'scratchdir = ',scratchdir
;    scratchdir_str = ' -scratchdir ' + scratchdir
;endif

if test_type eq !LINEARREGRESS then begin
    spawn,!BINEXECUTE+'/fidl_timestr2',timestr
    goback18b:
    root=''
    if glm_list[0] ne '' then begin
        rtn=get_root(glm_list,'.')
        root=strjoin(rtn.file,'_',/SINGLE) + '_'
    endif
    if lc_glm eq 0 then begin 
        concs=scratchdir+task_label+'.conc'
        glmnames=root+task_label+'.glm'
    endif else begin 

        ;concs=scratchdir+strmid(timept_label,1)+'.conc'
        ;START170224
        if timept_label[0] ne '' then $ 
            concs=scratchdir+strmid(timept_label,1)+'.conc' $
        else if n_elements(timept_label) eq 1 then begin 
            gr=get_root(gg.glm_list[0])
            concs=scratchdir+gr.file[0]
        endif

        glmnames=strarr(n_elements(task_label)*n_elements(timept_label))
        k=0
        widget_control,/HOURGLASS
        for i=0,n_elements(task_label)-1 do begin
            glmnames[k:k+n_elements(timept_label)-1]=root+task_label[i]+timept_label+'.glm'
            k=k+n_elements(timept_label)
        endfor
    endelse
    csh='fidl_2ndlevelmodel_'+timestr[0]+'.csh'
    designs = scratchdir+task_label+'.dat'

    ;names = get_str(n_elements(glmnames)+1,['script','glm '+strtrim(indgen(n_elements(glmnames))+1,2)],[csh,glmnames],WIDTH=50, $
    ;    TITLE='Please enter desired filenames.',/GO_BACK,/REPLACE,/RESET)
    ;if names[0] eq 'GO_BACK' then goto,goback18a
    ;START151231
    widget_control,/HOURGLASS
    names = get_str(n_elements(glmnames)+1,['script','glm '+strtrim(indgen(n_elements(glmnames))+1,2)],[csh,glmnames],WIDTH=50, $
        TITLE='Please enter desired filenames.',/GO_BACK,/EXIT,/REPLACE,/RESET,/FRONT,/BACK,/ONE_COLUMN)
    if names[0] eq 'EXIT' then return,rtn={msg:'EXIT'} else if names[0] eq 'GO_BACK' then goto,goback16a2

    names = fix_script_name(names)
    fidl_ttest_csh = names[0]
    glmnames = names[1:*]
endif 
goback18c:
if not keyword_set(SLOPES) then begin
    lcresid=0
    if test_type eq !LINEARREGRESS then begin
        fzstat=get_button(['full model only','all','go back'],TITLE='Please select fzstats.',BASE_TITLE='fzstats')
        if fzstat eq 2 then goto,goback18b 
        goback18d:
        ;Karimi change, ok for 4dfps, she was computing residuals from func conc maps
        lcresid=get_button(['yes','no','go back','exit'],TITLE='Do you want to compute residuals?',BASE_TITLE='Residuals')
        if lcresid eq 3 then return,rtn={msg:'EXIT'} else if lcresid eq 2 then goto,goback18c 
        lcresid=1-lcresid 
    endif 
    action=1
    if lcresid eq 0 then begin
        action=get_button(['execute','return','go back'],TITLE='Please select.',BASE_TITLE=fidl_ttest_csh)
        if action eq 2 then begin
            if test_type ne !LINEARREGRESS then goto,goback19 else goto,goback18d
        endif
    endif
endif
if test_type eq !PAIRED_COMPARISON then begin
    if n_elements(ctl_label) gt 0 then ctl_label = strcompress(ctl_label,/REMOVE_ALL)
endif
if test_type ne !CORRELATION and test_type ne !PARTIAL_CORRELATION and test_type ne !LINEARREGRESS then begin
    dummy = ntests
    if files_per_test eq 2 then ctl_mag_files = make_array(ntests,total_nsubjects,/STRING,VALUE='')
endif else if test_type eq !CORRELATION then begin
    if lc_glm eq 0 then begin
        if time eq 0 then $
            dummy = ntests $
        else $
            dummy = total(sumrows)
    endif else $
        dummy = nbehav_var*ntests
endif else begin
    if lc_glm eq 0 then begin
        if time eq 0 then $
            dummy = n_elements(behav_str) $
        else $
            dummy = total(sumrows)
    endif else $
        dummy = nbehav_var*ntests
endelse
varstr = ''
task_mag_files = make_array(dummy,total_nsubjects,/STRING,VALUE='')
if n_elements(filecount) eq 0 then filecount = make_array(total_nsubjects,/INTEGER,VALUE=dummy*files_per_test) 
openw,lu_csh,fidl_ttest_csh,/GET_LUN,APPEND=append
if append eq 0 then top_of_script,lu_csh

;START151119
;if not keyword_set(REGRESS) and not keyword_set(SLOPES) then printf,lu_csh,'if($#argv != 1) then'
;printf,lu_csh,''

if lc_glm eq 0 then begin
    if time eq 0 then begin 
        scrapstr = ' -tc '
    endif else begin
        scrapstr = ' -contrasts '
        magstr = ' -mag ONLY'
    endelse
    if keyword_set(SS) then varstr = ' -var'
    jj=0
    if n_elements(cstr_uniq) ne 0 then spider = cstr_uniq else spider = cstr
    cnt1=0
    for iglm=0,nsubject-1 do begin
        if cstr[iglm] ne '' then begin
            scrap = ''
            if t4_files[0] ne '' then scrap = ' -xform_file ' + strjoin(t4_files[jj:jj+nglm_per_subject[iglm]-1],' ',/SINGLE) 
            filename = strjoin(glm_files[jj:jj+nglm_per_subject[iglm]-1],' ',/SINGLE)

            ;goose = '/fidl_zstat -glm_file ' +filename+magstr+gauss_str+scrap+compress_str+atlas_str+delay_str+region_str+roi_str $
            ;    +magnorm_str+unscaled_str+bf_str+scratchdir_str+tc_weights_str+cstrwts[iglm]+varstr
            ;START160922
            goose = '/fidl_zstat2 -glm_file ' +filename+magstr+gauss_str+scrap+compress_str+atlas_str+delay_str+region_str+roi_str $
                +magnorm_str+unscaled_str+bf_str+scratchdir_str+tc_weights_str+cstrwts[iglm]+varstr

            cmd = !BINEXECUTE+goose+scrapstr+cstr[iglm]
            cmd_uniq = 'nice +19 $BIN'+goose+scrapstr+spider[iglm]
            cmd = cmd + ' -names_only '
            print,cmd
            widget_control,/HOURGLASS
            spawn,cmd,result
            nfiles = n_elements(result) - offset_spawn
            result = result[offset_spawn:nfiles+offset_spawn-1]
            if strpos(result[nfiles-1],'fidlError') ne -1 then begin
                stat=dialog_message_long('ERROR',result)
                return,rtn={msg:'EXIT'} 
            endif
            if nfiles ne filecount[iglm] then begin
                print,'nfiles=',nfiles,' iglm=',iglm,' filecount[iglm]=',filecount[iglm]
                stat = dialog_message('Error occurred while extracting magnitude names.',/ERROR)
                stat = spawn_check_and_load(result,fi,wd,dsp,help,nfiles_loaded,files_loaded,stc)
                return,rtn={msg:'EXIT'}
            endif

            ;START151215
            ;if scratchdir_str eq '' then begin
            ;    scrap = strpos(result[nfiles-1],'SCRATCH')
            ;    scratchdir_str = ' -scratchdir '+strmid(result[nfiles-1],scrap,strpos(result[nfiles-1],'/',/REVERSE_SEARCH)-scrap+1)
            ;    cmd_uniq = cmd_uniq + scratchdir_str
            ;endif

            printf,lu_csh,cmd_uniq
            printf,lu_csh,''
            scrap = 0
            i = 0
            for j=0,n_elements(task_label)-1 do begin
                if total(filecounteach[iglm,i:i+files_per_test-1]) eq files_per_test then begin
                    strs = strsplit(result[scrap],/EXTRACT)
                    task_mag_files[j,iglm] = strs[n_elements(strs)-1]
                    ;print,'    iglm=',iglm,' j=',j,' task_mag_files=',task_mag_files[j,iglm]
                    if files_per_test eq 2 then begin
                        strs = strsplit(result[scrap+1],/EXTRACT)
                        ctl_mag_files[j,iglm] = strs[n_elements(strs)-1]
                        ;print,'    iglm=',iglm,' j=',j,' ctl_mag_files=',ctl_mag_files[j,iglm]
                    endif
                    scrap = scrap + files_per_test
                endif
                i = i + files_per_test
            endfor
            cnt1 = cnt1 + 1
        endif
        jj = jj + nglm_per_subject[iglm]
    endfor
endif else if lc_glm eq 1 then begin
    if ifh_dim4[0] le 1 then begin
        bf_str = ' -frames_of_interest ' +  strjoin(strtrim(indgen(total_nsubjects)+1,2),' ',/SINGLE)
        goose = '/fidl_bolds $TC_FILES'+region_str+roi_str+compress_str+atlas_str+gauss_str+bf_str+scratchdir_str
        fidl_bolds_csh = scratchdir+'fidl_bolds.csh'
        openw,lu,fidl_bolds_csh,/GET_LUN
        printf,lu,FORMAT='("#!/bin/csh")'
        printf,lu,FORMAT='("unlimit")'
        printf,lu,''
        print_files_to_csh,lu,total_nsubjects,glm_files,'TC_FILES','tc_files'
        printf,lu,''
        printf,lu,!BINEXECUTE+goose+' -names_only'
        close,lu
        free_lun,lu
        spawn,'chmod +x '+fidl_bolds_csh
        widget_control,/HOURGLASS
        spawn,fidl_bolds_csh,result
        nfiles = n_elements(result) - offset_spawn
        result = result[offset_spawn:nfiles+offset_spawn-1]
        if nfiles ne total_nsubjects then begin
            stat = dialog_message('Error occurred while extracting names.',/ERROR)
            stat = spawn_check_and_load(result,fi,wd,dsp,help,nfiles_loaded,files_loaded,stc)
            return,rtn={msg:'EXIT'}
        endif
        print_files_to_csh,lu_csh,total_nsubjects,glm_files,'TC_FILES','tc_files',/NO_NEWLINE,/QUOTES
        printf,lu_csh,''
        printf,lu_csh,'nice +19 $BIN'+goose
        printf,lu_csh,''
        i = 0
        wallace = nfiles/files_per_test
        for iglm=0,wallace-1 do begin
            strs = strsplit(result[iglm],/EXTRACT)
            task_mag_files[i,iglm] = strs[n_elements(strs)-1]
            if files_per_test eq 2 then begin
                strs = strsplit(result[iglm+wallace],/EXTRACT)
                ctl_mag_files[i,iglm] = strs[n_elements(strs)-1]
            endif
        endfor
    endif else begin
        if ifh_dim4[0] gt 1 and time_label[0] ne '' then begin
            if dialog_message('Do your timecourses need to be normalized?'+string(10B)+string(10B)+'If you answer "yes", they will '$
                +'be normalized to the mean of all frames in the file, not just the ones you have selected for the T test.' $
                +string(10B)+'If this is not what you want, then tell McAvoy and he will code whatever you want.',/QUESTION) $
                eq 'Yes' then normstr = ' -normalize ALL'
        endif
        for j=0,nsubject-1 do begin
            scrap = ''
            if t4_files[0] ne '' then begin 
                scrap = t4_files[j]
                if test_type eq !PAIRED_COMPARISON then scrap = scrap + ' ' + t4_files[j+nsubject] 
            endif
            scrap1 = glm_files[j]
            if test_type eq !PAIRED_COMPARISON then scrap1 = scrap1 + ' ' + glm_files[j+nsubject] 
            command = '/fidl_bolds -tc_files '+scrap1+scrap+region_str+roi_str+compress_str+atlas_str+gauss_str $
                +bf_str[j]+normstr+scratchdir_str
            dummy = !BINEXECUTE+command+' -names_only '
            print,dummy
            print,''
            widget_control,/HOURGLASS
            spawn,dummy,result
            nfiles = n_elements(result) - offset_spawn
            result = result[offset_spawn:nfiles+offset_spawn-1]
            if nfiles ne loop then begin
                stat = dialog_message('Error occurred while extracting names.',/ERROR)
                stat = spawn_check_and_load(result,fi,wd,dsp,help,nfiles_loaded,files_loaded,stc)
                return,rtn={msg:'EXIT'}
            endif
            if scratchdir_str eq '' then begin
                dummy = strpos(result[nfiles-1],'/')
                scratchdir_str = ' -scratchdir '+strmid(result[nfiles-1],dummy,strpos(result[nfiles-1],'/',/REVERSE_SEARCH)-dummy+1)
                command = command + scratchdir_str
            endif
            printf,lu_csh,'nice +19 $BIN'+command 
            printf,lu_csh,''
            if test_type ne !PAIRED_COMPARISON and test_type ne !TYPE_II_REGRESSION then begin
                for i=0,ntests-1 do begin
                    strs = strsplit(result[i],/EXTRACT)
                    task_mag_files[i,j] = strs[n_elements(strs)-1]
                endfor
            endif else begin
                k=0
                for i=0,loop-1 do begin
                    strs = strsplit(result[i],/EXTRACT)
                    print,'strs=',strs
                    print,'n_elements(strs)=',n_elements(strs)
                    if i mod 2 eq 0 then $
                        task_mag_files[k,j] = strs[n_elements(strs)-1] $
                    else begin 
                        ctl_mag_files[k,j] = strs[n_elements(strs)-1]
                        k=k+1
                    endelse
                endfor
            endelse
        endfor
    endelse
endif else begin
    widget_control,/HOURGLASS
    for i=0,total_nsubjects-1 do begin
        sdata = read_sylk_str(glm_files[i],STARTROW=1,/ARRAY,/USEDOUBLES)
        l = 0
        for j=0,nslkreg[i]-1 do begin
            for k=j+1,nslkreg[i]-1 do begin
                task_mag_files[l,i] = sdata[j,k]
                l = l + 1
            endfor
        endfor
    endfor
endelse
if n_elements(driver) ne 0 then begin
    if n_elements(filecounteach) eq 0 then filecounteach = make_array(total_nsubjects,loop,/INTEGER,VALUE=1)
    if lc_glm ne 2 and not keyword_set(SLOPES) then clean_up_str = ' -clean_up'
    driver_str = ' -driver "'+driver+'"'
    if lc_interactive eq 1 and not keyword_set(SS) then $
        write_list,glm_list_str,[total_nsubjects,n_elements(t4_files)],[glm_files,t4_files]
    if glm_list_str[0] ne '' then begin
        glm_list_str_ttest = ' -glm_list_file "'+strjoin(glm_list_str,'" "',/SINGLE)+'"' 
        if two_glm_lists eq 'Yes' then begin
            glm_list_str = strarr(2)
            glm_list_str[0] = ' -glm_list_file "' + two_glm_lists_str[0] + '"'
            glm_list_str[1] = ' -glm_list_file "' + two_glm_lists_str[1] + '"'
            glm_list_str_ttest = ' -glm_list_file "' + two_glm_lists_str[0] + ','+ two_glm_lists_str[1] + '"'
        endif
    endif
    glmstr=''

    ;if lc_glm eq 0 and boldtype ne 'img' then glmstr = ' -glm '+files[0] 
    ;START160602
    if lc_glm eq 0 and (boldtype ne 'img' or nreg ne 0) then glmstr = ' -glm '+files[0] 

    printf,lu_csh,'nice +19 $BIN/fidl_ttest'+driver_str+region_or_uncompress_str+ttest_output_str+Nimage_name_str $
        +threshold_extent_str+glm_list_str_ttest+group_names_ttest+scratchdir_str+var_thresh_str+nullstr+prependstr $
        +gauss_str_ttest+glmstr+clean_up_str
    printf,lu_csh,''

    ;if keyword_set(SLOPES) then $ 
    ;    printf,lu_csh,'rm -rf '+scratchdir $
    ;else begin
    ;    printf,lu_csh,'else'
    ;    printf,lu_csh,''
    ;    printf,lu_csh,'nice +19 $BIN/fidl_ttest'+driver_str+' -clean_up ONLY'
    ;    printf,lu_csh,''
    ;    printf,lu_csh,'endif'
    ;endelse
    ;START151119
    if keyword_set(SLOPES) then printf,lu_csh,'rm -rf '+scratchdir

    if n_elements(slkfile) ne 0 then begin
        printf,lu_csh,''
        printf,lu_csh,'#'+slkfile
    endif
    if glm_list_str[0] ne '' then for i=0,nglm_list_str-1 do printf,lu_csh,'#'+glm_list_str[i]
    if glm_list[0] ne '' then for i=0,n_elements(glm_list)-1 do printf,lu_csh,'#'+glm_list[i] 
    close,lu_csh
    free_lun,lu_csh
    spawn,'chmod +x '+fidl_ttest_csh
    if test_type eq !CORRELATION then begin
        if lc_glm ne 0 and time eq 0 then begin
            contrast_label = task_label
            ncontrast_label = ntests
        endif
        ntests = nbehav_var
    endif else if test_type eq !PARTIAL_CORRELATION and time eq 0 then $
        ntests = n_elements(ncov)
    kk = 0
    kkk = 0
    openw,lu_driver,driver,/GET_LUN
    for i=0,ntests-1 do begin
        if test_type eq !PAIRED_COMPARISON then begin
            scrap = 'PAIRED_COMPARISON' + ' ' + task_label[i]
            dummy = 1
        endif else if test_type eq !UNPAIRED_COMPARISON then begin
            scrap = 'UNPAIRED_COMPARISON' + ' ' + task_label[i]
            dummy = 1
        endif else if test_type eq !GROUP_COMPARISON then begin
            if not keyword_set(SS) then scrap = 'GROUP_COMPARISON' else scrap = 'SS_TWOINDSAM'    
            scrap = scrap + ' ' + task_label[i]
            dummy = 1
        endif else if test_type eq !SS_VS_GROUP then begin
            scrap=''
            dummy = nglm
        endif else if test_type eq !CORRELATION then begin
            scrap = 'CORRELATION '


            ;junk = behav_str[i]
            ;if lc_glm eq 0 then begin
            ;    if time ne 0 then dummy = sumrows[i] else dummy = loop2[i]
            ;endif else if time eq 0 then $
            ;    dummy = ncontrast_label $
            ;else $
            ;    dummy = 1
            ;START151215
            if lc_glm eq 0 then begin
                junk = behav_str[i]
                if time ne 0 then dummy = sumrows[i] else dummy = loop2[i]
            endif else if time eq 0 then begin 
                junk = behav_str[i]
                dummy = ncontrast_label 
            endif else begin 
                junk = task_label[i]
                dummy = 1
            endelse


            index = where(behav_data[*,i] ne '',count)
        endif else if test_type eq !PARTIAL_CORRELATION then begin
            scrap = 'PARTIAL_CORRELATION '
            junk = behav_str[i]
            if lc_glm eq 0 then begin
                if time ne 0 then dummy = sumrows[i] else dummy = loop2[i]
            endif else if time eq 0 then $
                dummy = ncontrast_label
        endif else begin
            scrap = ' TYPE_II_REGRESSION ' + ' ' + task_label[i]
            dummy = 1
        endelse
        for k=0,dummy-1 do begin
            if test_type ne !CORRELATION and test_type ne !PARTIAL_CORRELATION then $
                scrapstr = scrap $
            else if lc_glm eq 0 then begin
                if test_type eq !CORRELATION then begin
                    scrapstr = scrap + task_label[kkk]
                    kkk = kkk + 1
                endif else begin
                    if time eq 0 then begin
                        scrapstr = scrap + task_label[kkk]
                        kkk = kkk + 1
                    endif else begin
                        goose = contrast_labels[index_conditions[i,k]]
                        scrapstr = scrap + goose + '_and_' + junk
                    endelse
                endelse
            endif else if time eq 0 then $
                scrapstr = scrap + contrast_label[k] $
            else begin
                scrapstr = scrap
                if strpos(task_mag_files[0,0],'Asigma') ne -1 then $
                    scrapstr = scrapstr + 'Asigma_and_' + junk $
                else if strpos(task_mag_files[0,0],'div') ne -1 then $
                    scrapstr = scrapstr + 'div_and_' + junk $
                else if strpos(task_mag_files[0,0],'dbar') ne -1 then $
                    scrapstr = scrapstr + 'dbar_and_' + junk $
                else $
                    scrapstr = scrapstr + junk
            endelse
            if test_type ne !SS_VS_GROUP then printf,lu_driver,FORMAT='("TTEST  := ",a)',scrapstr
            case test_type of
                !PAIRED_COMPARISON: begin
                    for j=0,nsubject-1 do if task_mag_files[i,j] ne '' then printf,lu_driver,'FIRST  := ',task_mag_files[i,j], $
                        FORMAT='(a10,a)'
                    for j=0,nsubject-1 do if ctl_mag_files[i,j] ne '' then printf,lu_driver,'SECOND := ',ctl_mag_files[i,j], $
                        FORMAT='(a10,a)'
                end
                !UNPAIRED_COMPARISON: begin
                    for j=0,nsubject-1 do if filecounteach[j,i] ne 0 then printf,lu_driver,'FIRST  := '+task_mag_files[i,j]
                end
                !GROUP_COMPARISON: begin
                    if not keyword_set(SS) then begin
                        for j=0,nglm-1 do printf,lu_driver,'FIRST  := ',task_mag_files[i,j],FORMAT='(a10,a)'
                        for j=0,nglm_2-1 do printf,lu_driver,'SECOND := ',task_mag_files[i,nglm+j],FORMAT='(a10,a)'
                    endif else begin
                        for j=0,nglm-1 do printf,lu_driver,'FIRST  := ',task_mag_files[i,j],FORMAT='(a10,a)'
                        for j=0,nglm_2-1 do printf,lu_driver,'FIRST  := ',task_mag_files[i,nglm+j],FORMAT='(a10,a)'
                        for j=0,nglm-1 do printf,lu_driver,'SECOND := ',ctl_mag_files[i,j],FORMAT='(a10,a)'
                        for j=0,nglm_2-1 do printf,lu_driver,'SECOND := ',ctl_mag_files[i,nglm+j],FORMAT='(a10,a)'
                        l=0
                        for j=0,nglm-1 do begin
                            printf,lu_driver,'DF     := ',trim(ifh[l].glm_df)
                            l=l+1
                        endfor
                        for j=0,nglm_2-1 do begin
                            printf,lu_driver,'DF     := ',trim(ifh[l].glm_df)
                            l=l+1
                        endfor
                    endelse
                end
                !SS_VS_GROUP: begin
                    if glmroot[0] eq '' then $
                        printf,lu_driver,'TTEST := SS_VS_GROUP '+task_label[i] $
                    else $
                        printf,lu_driver,'TTEST := SS_VS_GROUP '+glmroot[k]+'_'+task_label[i]
                    printf,lu_driver,'FIRST  := '+task_mag_files[i,k]
                    for j=0,nglm_2-1 do printf,lu_driver,'SECOND := ',task_mag_files[i,nglm+j],FORMAT='(a10,a)'
                end
                !CORRELATION: begin
                    idx = where(task_mag_files[kk,index] ne '',cnt)
                    if count ne 0 then begin
                        if cnt ge 2 then begin
                            for j=0,cnt-1 do printf,lu_driver,'FIRST  := '+task_mag_files[kk,index[idx[j]]]
                            for j=0,cnt-1 do printf,lu_driver,'SECOND := '+behav_data[index[idx[j]],i]
                        endif else begin
                            stat=dialog_message('Only one subject for '+scrapstr,/ERROR)
                        endelse
                    endif else begin
                        stat=dialog_message('No behav_data found for '+scrapstr,/ERROR)
                    endelse
                    if lc_glm eq 0 then begin
                        kk = kk + 1
                    endif else if ifh_dim4[0] gt 1 then $
                        kk = kk + 1
                end

                ;!PARTIAL_CORRELATION: begin
                ;    scraparr = intarr(nglm)
                ;    for j=0,ncov[i] do scraparr = scraparr + (behav_data[*,indvar[i,j]] eq '')
                ;    index = where(scraparr eq 0,count)
                ;    if count ne 0 then begin
                ;        for j=0,count-1 do printf,lu_driver,'FIRST  := '+task_mag_files[kk,index[j]]
                ;        for j=0,count-1 do printf,lu_driver,'SECOND := '+behav_data[index[j],indvar[i,0]]
                ;        if ncov[i] gt 0 then begin
                ;            for j=0,count-1 do begin
                ;                scrapstr = 'COVARIATES :='
                ;                for l=1,ncov[i] do begin
                ;                    scrapstr = scrapstr + ' ' + behav_data[index[j],indvar[i,l]]
                ;                endfor
                ;                printf,lu_driver,scrapstr
                ;            endfor
                ;        endif
                ;    endif else begin
                ;        stat=dialog_message('No behav_data found HERE2. Abort!',/ERROR)
                ;        return,rtn={msg:'EXIT'}
                ;    endelse
                ;    kk = kk + 1
                ;end
                ;START150204
                !PARTIAL_CORRELATION: begin
                    scraparr = intarr(nglm)
                    for j=0,ncov[i] do scraparr = scraparr + (behav_data[*,indvar[i,j]] eq '')
                    index = where(scraparr eq 0,count)
                    idx = where(task_mag_files[kk,index] ne '',cnt)
                    if count ne 0 then begin
                        for j=0,cnt-1 do printf,lu_driver,'FIRST  := '+task_mag_files[kk,index[idx[j]]]
                        for j=0,cnt-1 do printf,lu_driver,'SECOND := '+behav_data[index[idx[j]],indvar[i,0]]
                        if ncov[i] gt 0 then begin
                            for j=0,cnt-1 do begin
                                scrapstr = 'COVARIATES :='
                                for l=1,ncov[i] do begin
                                    scrapstr = scrapstr + ' ' + behav_data[index[idx[j]],indvar[i,l]]
                                endfor
                                printf,lu_driver,scrapstr
                            endfor
                        endif
                    endif else begin
                        stat=dialog_message('No behav_data found HERE2. Abort!',/ERROR)
                        return,rtn={msg:'EXIT'}
                    endelse
                    kk = kk + 1
                end


                !TYPE_II_REGRESSION: begin
                    for j=0,nsubject-1 do if task_mag_files[i,j] ne '' then printf,lu_driver,'FIRST  := ',task_mag_files[i,j], $
                        FORMAT='(a10,a)'
                    for j=0,nsubject-1 do if ctl_mag_files[i,j] ne '' then printf,lu_driver,'SECOND := ',ctl_mag_files[i,j], $
                        FORMAT='(a10,a)'
                end
                else: begin
                    print,'Invalid comparison type in "fidl_ttest.pro.'
                    return,rtn={msg:'EXIT'}
                end
            endcase
        endfor
    endfor
    close,lu_driver
    free_lun,lu_driver
    if test_type eq !GROUP_COMPARISON or test_type eq !SS_VS_GROUP or nglms_per_sub eq 2 then begin
        ngroups = 2
        nglm = [nglm,nglm_2]
        nsubject = nglm
    endif else begin
        ngroups = 1
    endelse
    Nimage_name_str = ''
    if n_elements(compute_avg_zstat_csh_str) ne 0 and test_type ne !SS_VS_GROUP then begin
        if time eq 0 then extra_str=' -frames '+strjoin(make_array(n_elements(task_label),/STRING,VALUE='1'),' ',/SINGLE) $
            +' -tc_names '+strjoin('avgtc_'+task_label,' ',/SINGLE)
        compute_avg_zstat_csh,compute_avg_zstat_csh_str,ngroups,0,nglm,glm_files,t4_files,time,loop1,group_names,magstr, $
            gauss_str,region_file,'',roi_str,avgstat_output_str,Nimage_name_str,Nimage_mask_str,delay_str, $
            '',magnorm_str,1,effect_length_ts,conditions_time,sumrows_time,sum_contrast_for_treatment,tc_type, $
            print_unscaled_mag_str,lcmag,glm_list_str_ttest,directory_str,contrast,wfiles,lcsd,extra_str,nglms_per_sub,'',0, $
            'fidl_avg_zstat',nsubject,behav_data1,indvar1
    endif
    if action eq 0 then begin

        ;spawn,!BINEXECUTE+'/fidl_timestr',timestr
        ;START150610
        spawn,!BINEXECUTE+'/fidl_timestr2',timestr

        spawn,fidl_ttest_csh+'>'+fidl_ttest_csh+'_'+timestr+'.log &'
        stats = dialog_message(fidl_ttest_csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
    endif
    if not keyword_set(SLOPES) then print,'DONE'
    return,rtn={msg:'OK'}
endif else begin
    close,lu_csh
    free_lun,lu_csh
    spawn,'chmod +x '+fidl_ttest_csh

    ;print,'time=',time
    ;print,'task_label=',task_label,'END'
    ;;print,'size(task_label)=',size(task_label)
    ;print,'concs'
    ;print,concs
    ;print,'size(concs)=',size(concs)
    ;print,'task_mag_files'
    ;print,task_mag_files
    ;print,'size(task_mag_files)=',size(task_mag_files)
    ;print,'n_elements(nindvar)=',n_elements(nindvar)

    ;l = 0
    ;for i=0,n_elements(nindvar)-1 do begin
    ;    dummy = 1
    ;    if lc_glm eq 0 then begin
    ;        if time ne 0 then dummy = sumrows[i] else dummy = loop2[i]
    ;    endif else if time eq 0 then $
    ;        dummy = ncontrast_label
    ;    scraparr = intarr(nglm)
    ;    for j=0,nindvar[i]-1 do scraparr = scraparr + (behav_data[*,indvar[i,j]] eq '')
    ;    index = where(scraparr eq 0,count)
    ;    if count eq 0 then $
    ;        l = l + dummy $
    ;    else begin
    ;        idx = where(task_mag_files[l,index] ne '',cnt)
    ;        spider=strarr(cnt)
    ;        for j=0,cnt-1 do spider[j]=strjoin(trim(float(behav_data[index[idx[j]],indvar[i,0:nindvar[i]-1]])),string(9B),/SINGLE)
    ;        for k=0,dummy-1 do begin
    ;            openw,lu,concs[l],/GET_LUN
    ;            for j=0,cnt-1 do printf,lu,task_mag_files[l,index[idx[j]]]
    ;            close,lu
    ;            free_lun,lu
    ;            openw,lu,designs[l],/GET_LUN
    ;            printf,lu,behav_str2[i]
    ;            for j=0,cnt-1 do printf,lu,spider[j]
    ;            close,lu
    ;            free_lun,lu
    ;            l = l + 1
    ;        endfor
    ;    endelse
    ;endfor
    ;START150828
    nconcs=intarr(n_elements(nindvar))
    l = 0
    for i=0,n_elements(nindvar)-1 do begin
        dummy = 1
        if lc_glm eq 0 then begin
            if time ne 0 then dummy = sumrows[i] else dummy = loop2[i]
        endif else if time eq 0 then begin
            dummy = ntests
        endif
        scraparr = intarr(nglm)
        for j=0,nindvar[i]-1 do scraparr = scraparr + (behav_data[*,indvar[i,j]] eq '')
        index = where(scraparr eq 0,count)
        if count eq 0 then $
            l = l + dummy $
        else begin
            idx = where(task_mag_files[l,index] ne '',cnt)
            spider=strarr(cnt)
            for j=0,cnt-1 do spider[j]=strjoin(trim(float(behav_data[index[idx[j]],indvar[i,0:nindvar[i]-1]])),string(9B),/SINGLE)
            openw,lu,designs[i],/GET_LUN
            printf,lu,behav_str2[i]
            for j=0,cnt-1 do printf,lu,spider[j]
            close,lu
            free_lun,lu
            if lc_glm eq 0 or time eq 1 or (time eq 0 and i eq 0) then begin
                for k=0,dummy-1 do begin
                    print,'Writing '+concs[l]
                    openw,lu,concs[l],/GET_LUN
                    for j=0,cnt-1 do printf,lu,task_mag_files[l,index[idx[j]]]
                    close,lu
                    free_lun,lu
                    l = l + 1
                endfor
            endif else $
                concs=[concs,concs[0:dummy-1]]
            nconcs[i]=dummy
        endelse
    endfor
    print,'DONE fidl_ttest'
    return,rtn={msg:'OK',concs:concs,designs:designs,glmnames:glmnames,csh:fidl_ttest_csh,ifh:ifh,fwhm:fwhm,mask:mask, $
        scratchdir:scratchdir,region_file:region_file,region_names:region_names,action:action,glm_space:glm_space[0],fzstat:fzstat, $
        slkfile:slkfile,glm_list:glm_list,nconcs:nconcs,lcresid:lcresid,files:files}
endelse
end
