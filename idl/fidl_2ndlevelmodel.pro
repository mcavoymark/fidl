;Copyright 7/24/12 Washington University.  All Rights Reserved.
;fidl_2ndlevelmodel.pro  $Revision: 1.28 $
pro fidl_2ndlevelmodel,fi,dsp,wd,glm,help,stc,pref
title='Values read from a slk are the independent variables (ie regressors). Imaging data are the dependent variables.'
scrap = get_button(['Ok','Exit'],BASE_TITLE='Second level model',TITLE=title,/ALIGN_LEFT)
if scrap eq 1 then return
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE=title
swapbytes=0
mode=0
rtn=fidl_ttest(fi,dsp,wd,glm,help,stc,pref,/REGRESS)
if rtn.msg ne 'OK' then return
concs = rtn.concs
designs = rtn.designs
glmnames = rtn.glmnames
ifh = rtn.ifh[0]
fwhm = rtn.fwhm
mask = rtn.mask
scratchdir = rtn.scratchdir
region_file = rtn.region_file
region_names = rtn.region_names
xdim = ifh.matrix_size_1
ydim = ifh.matrix_size_2
zdim = ifh.matrix_size_3
dxdy = ifh.scale_1
dz = ifh.scale_3
ifh.glm_fwhm = fwhm
if region_file[0] ne '' then begin
    ifh.nregfiles = n_elements(region_file)
    if ptr_valid(ifh.regfiles) then ptr_free,ifh.regfiles
    ifh.regfiles = ptr_new(region_file)
    ifh.nreg = n_elements(region_names)
    if ptr_valid(ifh.region_names) then ptr_free,ifh.region_names
    ifh.region_names = ptr_new(region_names)
    if ptr_valid(ifh.glm_mask_file) then ptr_free,ifh.glm_mask_file
    ifh.matrix_size_1 = ifh.nreg
    xdim = ifh.nreg
    ydim = 1
    zdim = 1
    rtn_gr = get_root(glmnames,'.glm')
    maskstr = ' -regional_avgstat_name '+rtn_gr.file+'.txt'
endif else begin
    if rtn.glm_space eq !SPACE_111 then $
        ifh.matrix_size_1 = !LENBRAIN_111 $
    else if rtn.glm_space eq !SPACE_222 then $
        ifh.matrix_size_1 = !LENBRAIN_222 $
    else if rtn.glm_space eq !SPACE_333 then $
        ifh.matrix_size_1 = !LENBRAIN_333 $
    else if mask ne '' then begin
        ifh.matrix_size_1=call_external(!SHARE_LIB,'_read_mask',mask,VALUE=1)
        if ifh.matrix_size_1 eq 0 then begin
            scrap=get_button(['Exit'],BASE_TITLE='Problem with _read_mask',TITLE='Returned value is zero.')
            return
        endif
    endif

    ;if mask ne '' then scrap=' -mask '+mask else scrap = '' 
    ;START160106
    if mask ne '' then begin
        scrap=' -mask '+mask 
        ifh.glm_mask_file=ptr_new(mask)
    endif else scrap = '' 

    maskstr=make_array(n_elements(designs),/STRING,VALUE=scrap)
endelse
ifh.matrix_size_2 = 1
ifh.matrix_size_3 = 1
ifh.matrix_size_4 = 1
ifh.bigendian = abs(!SunOS_Linux-1) 
glmnamesin = scratchdir+glmnames
openw,lu,rtn.csh,/GET_LUN,/APPEND
ndesigns=n_elements(designs)
i1=0
for i=0,ndesigns-1 do begin
    er = readf_ascii_file(designs[i])
    print,'size(er.data)=',size(er.data) 
    lcskipdes=0
    Ap=float(er.data)
    skipcol=intarr(er.NF)
    ncol=er.NF
    icol=indgen(er.NF)
    for j=0,er.NF-1 do if total(Ap[j,*]-Ap[j,0]) eq 0. then skipcol[j]=1  
    idx=where(skipcol eq 1,cnt)
    if cnt ne 0 then begin
        if cnt eq er.NF then begin
            if ndesigns eq 1 then begin 
                scrap=get_button(['Exit'],BASE_TITLE='Invalid design',TITLE='Variates are identical across subjects.') 
                return
            endif else begin 
                scrap=get_button(['Skip this one','Exit'],BASE_TITLE='Invalid design',TITLE='Variates are identical across subjects.') 
                if scrap eq 1 then return else lcskipdes=1
            endelse
        endif else begin
            if cnt eq 1 then $ 
                scrap=get_button(['Remove this column','Exit'],BASE_TITLE='Invalid design',TITLE='Variates in column '+trim(idx+1) $
                    +' are identical across subjects.') $
            else $ 
                scrap=get_button(['Remove these columns','Exit'],BASE_TITLE='Invalid design',TITLE='Variates in columns ' $
                    +strjoin(trim(idx+1),', ',/SINGLE)+' are identical across subjects.')
            if scrap eq 1 then return else begin
                icol=where(skipcol eq 0,ncol) 
                Ap=Ap[icol,*]
            endelse
        endelse
    endif
    if lcskipdes eq 0 then begin
        print,'size(Ap)=',size(Ap) 
        A = transpose([Ap,transpose(make_array(er.NR,/FLOAT,VALUE=1.))])
        tcomp = er.NR 
        N = ncol+1
        cond_number_thresh=10000.
        cond_number=double(0.)
        cond_number_svd=double(0.)
        n_depend=0L
        stat=0L
        stat=call_external(!SHARE_LIB,'_Acond_norm1',A,long(tcomp),long(N),cond_number,VALUE=[0,1,1,0])
        if stat eq 8L then begin
            print,'Error in _Acond_norm1 "GSL_ENOMEM   = 8,   /* malloc failed */"'
            scrap=get_button(['Continue','Skip this one','Exit'],BASE_TITLE='Not enough memory',TITLE='Not enough memory to compute ' $
                +'the condition number.'+string(10B)+'What do you want to do?')
            if scrap eq 1 then lcskipdes=1 else if scrap eq 2 then return
        endif else begin
            if stat eq 0L then print,'_Acond_norm1 cond_number=',cond_number
            if cond_number lt 0.00001 or cond_number gt cond_number_thresh then begin
                stat=call_external(!SHARE_LIB,'_AtoATAgsl_svd',A,long(tcomp),long(N),cond_number_svd,n_depend,VALUE=[0,1,1,0,0])
                if stat eq 0L then begin
                    print,'Error in _AtoATAgsl_svd'
                    return
                endif
                print,'_AtoATAgsl_svd cond_number_svd=',cond_number_svd,' n_depend=',n_depend
            endif
            if n_depend gt 0 then begin
                msg='Design matrix is rank deficient by '+trim(n_depend)+' columns'
                print,msg
                if ndesigns eq 1 then begin
                    scrap=get_button(['Exit'],BASE_TITLE='Invalid design',TITLE=msg)
                    return
                endif else begin
                    scrap=get_button(['Skip this one','Exit'],BASE_TITLE='Invalid design',TITLE=msg)
                    if scrap eq 1 then return else lcskipdes=1
                endelse
            endif
        endelse
        if lcskipdes eq 0 then begin
            valid_frms = make_array(er.NR,/FLOAT,VALUE=1.)
            tdim = er.NR

            ;nrun = 1
            ;START160107
            nrun = er.NR 

            num_trials = er.NR
            TR = 1.
            nmain = ncol 
            tot_eff = ncol+1
            delay = fltarr(ncol+1)
            stimlen = make_array(ncol+1,/FLOAT,VALUE=1.)
            lcfunc = make_array(ncol+1,/INTEGER,VALUE=!BEHAVIORAL)
            all_eff = ncol+1
            effect_column = indgen(all_eff)
            effect_label = [er.hdr[icol],'Baseline']
            effect_length = make_array(all_eff,/INTEGER,VALUE=1)
            rtn1=default_contrasts_noglm(effect_label,effect_length,tot_eff,N,effect_TR,effect_shift_TR,stimlen,delay,lcfunc, $
                hrf_param,nmain,hrf_model)
            glm1 = {General_linear_model}
            set_glm1,glm1,A,valid_frms,nmain,N,tot_eff,xdim[0],ydim[0],zdim[0],tdim,dxdy[0],dz[0],nrun,tcomp, $
                num_trials,TR,stimlen,lcfunc,functype,DELAY=delay,funclen, $
                NF=1,all_eff,effect_column,effect_label,effect_length,EFFECT_TR=effect_TR,EFFECT_SHIFT_TR=effect_shift_TR
            glm1.nc = rtn1.nc 
            glm1.c = ptr_new(rtn1.c)
            glm1.cnorm = ptr_new(rtn1.cnorm)
            glm1.contrast_labels = ptr_new(rtn1.contrast_labels)
            if ncol eq 1 then begin 
                F_names = effect_label[0] 
                nF = 1
            endif else begin
                rtn2=fidl_combinations(ncol,effect_label,scratchdir)
                if rtn2.msg eq 'ERROR' then return
                if rtn.fzstat eq 0 then begin
                    nF = 1
                    F_names = rtn2.F_names[rtn2.nF-1]
                endif else begin 
                    nF = rtn2.nF
                    F_names = rtn2.F_names
                endelse
            endelse
            glm1.nF = nF
            glm1.F_names = ptr_new(F_names)
            glm1.F_names2 = ptr_new(F_names)

            ;START160104
            ;ifh[0].glm_effect_label = ptr_new(effect_label)
            ;ifh[0].glm_effect_length = ptr_new(effect_length)
            ;START160106
            ifh.glm_effect_label = ptr_new(effect_label)
            ifh.glm_effect_length = ptr_new(effect_length)

            ;START151231
            i1start=i1

            for j=0,rtn.nconcs[i]-1 do begin

                ;glm_file=save_linmod(fi,dsp,wd,glm,help,stc,pref,fi.n,fi.n,swapbytes,FILENAME=glmnamesin[i1],IFH=ifh[0],GLM1=glm1, $
                ;    /DESIGN_ONLY)
                ;START160106
                glm_file=save_linmod(fi,dsp,wd,glm,help,stc,pref,fi.n,fi.n,swapbytes,FILENAME=glmnamesin[i1],IFH=ifh,GLM1=glm1, $
                    /DESIGN_ONLY)

                if glm_file eq 'ERROR' then return
                print_files_to_csh,lu,1,concs[i1],'BOLD_FILES','bold_files',/NO_NEWLINE
                printf,lu,'set INPUT_GLM_FILE = ','"','-input_glm_file ',glmnamesin[i1],'"',FORMAT='(a,a,a,a,a)'
                printf,lu,'set OUTPUT_GLM_FILE = ','"','-output_glm_file ',glmnames[i1],'"',FORMAT='(a,a,a,a,a)'
                if mask ne '' then print_files_to_csh,lu,1,mask,'MASK_FILE','mask',/NO_NEWLINE else printf,lu,'set MASK_FILE'
                printf,lu,'set MODE = ','"','-mode ',mode,'"',FORMAT='(a,a,a,1x,i1,a)'
                printf,lu,'nice +19 $BIN/compute_glm2 $BOLD_FILES $INPUT_GLM_FILE $OUTPUT_GLM_FILE $MASK_FILE $MODE'
                printf,lu,''

                ;wallace='nice +19 $BIN/fidl_avg_zstat -glm_files '+glmnames[i1]+' -contrasts '+ $
                ;    strjoin(strtrim(indgen(glm1.nc)+1,2),' ',/SINGLE)+maskstr[i]+' -zstat -glmpersub 1'
                ;START161213
                wallace='nice +19 $BIN/fidl_avg_zstat2 -glm_files '+glmnames[i1]+' -contrasts '+ $
                    strjoin(strtrim(indgen(glm1.nc)+1,2),' ',/SINGLE)+maskstr[i]+' -zstat -glmpersub 1'

                if i eq ndesigns-1 and j eq rtn.nconcs[i]-1 and rtn.lcresid eq 0 then wallace=wallace+' -cleanup '+scratchdir
                printf,lu,wallace
                printf,lu,''
                i1=i1+1
            endfor

            ;START151231
            i1cnt=i1-i1start
            i1end=i1-1
            ;print,'here0 i1=',i1,' i1cnt=',i1cnt,' i1start=',i1start,' i1end=',i1end

            if rtn.lcresid eq 1 then begin
                goback10:
                gg={load:1,glmfiles:glmnames[i1start:i1end],total_nsubjects:i1cnt,t4select:'',glm_list:'', $
                    concselect:concs[i1start:i1end],evselect:'',glm_space:rtn.glm_space,bold_space:rtn.glm_space,ifh:ifh, $
                    scratchdir:getenv('PWD')+'/'+scratchdir}
                cr=compute_residuals(fi,dsp,wd,glm,help,stc,pref,GET_GLMS_STRUCT=gg,/DONTCHECKCONC,/DONTLOADCONC,/DONTASKSMOOTH)

                if cr.msg ne 'OK' then begin
                    scrap=get_button(['Exit'],BASE_TITLE='Problem in compute_residuals.pro',TITLE='Problem in compute_residuals.pro')
                    return
                endif
                for j=0,n_elements(cr.cmdstr)-1 do begin
                    printf,lu,cr.cmdstr[j] 
                    printf,lu,''
                endfor 

                ;print,'cr.concselect[0]=',cr.concselect[0]
                ;print,'cr.acconc[0]=',cr.acconc[0]

                ;START160107
                gr=get_root(rtn.files,'.4dfp.img')
                widget_control,/HOURGLASS
                names=get_str(n_elements(gr.file),trim(indgen(n_elements(gr.file))+1),gr.file+'_res', $
                    TITLE='Residual root names',/GO_BACK,/EXIT,/REPLACE,/RESET,/FRONT,/BACK,/ONE_COLUMN)
                if names[0] eq 'EXIT' then return else if names[0] eq 'GO_BACK' then goto,goback10

                print_files_to_csh,lu,n_elements(cr.acconc),cr.acconc,'FILES','files',/NO_NEWLINE
                print_files_to_csh,lu,n_elements(gr.file),names,'ROOTS','roots',/NO_NEWLINE
                printf,lu,'nice +19 $BIN/fidl_assemble $FILES $ROOTS'+maskstr[i]+' -clean_up '+scratchdir

            endif
        endif
    endif
endfor

;START160107
if rtn.lcresid eq 1 then $
    action=get_button(['execute','return'],TITLE='Please select action.',BASE_TITLE=rtn.csh) $
else $
    action=rtn.action

;START160106
printf,lu,''

printf,lu,'#'+rtn.slkfile
for i=0,n_elements(rtn.glm_list)-1 do printf,lu,'#'+rtn.glm_list[i]
close,lu
free_lun,lu

;if rtn.action eq 0 then begin
;START160107
if action eq 0 then begin

    spawn,rtn.csh+'>'+rtn.csh+'.log &'
    stats = dialog_message(rtn.csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE fidl_2ndlevelmodel'
end
