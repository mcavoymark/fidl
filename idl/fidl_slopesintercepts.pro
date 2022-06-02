;Copyright 1/7/13 Washington University.  All Rights Reserved.
;fidl_slopesintercepts.pro  $Revision: 1.4 $
pro fidl_slopesintercepts,fi,dsp,wd,glm,help,stc,pref
swapbytes=0
mode=0
scrap = get_button(['ok','exit'],BASE_TITLE='Slopes and intercepts', $
    TITLE="A pair of slopes and intercepts are compared with a Student's paired T test."+string(10B)+'Input is either a glm or ' $
    +'conc for each subject.'+string(10B)+'If a glm, you will build the two time series, from which the slopes and intercepts are ' $
    +'estimated from, on the fly.'+string(10B)+'If a conc, it must include two time series.'+string(10B)+'Output is paired ' $
    +'T test of slopes and intercepts, respectively.')
if scrap eq 1 then return
rtn=compute_avg_zstat(fi,dsp,wd,glm,help,stc,pref,/SLOPES)
if rtn.msg ne 'OK' then return
widget_control,/HOURGLASS
spawn,rtn.script_namesonly,result
if n_elements(result) ne rtn.nsubject*2 then begin
     stat=dialog_message_long('Expecting '+trim(rtn.nsubject*2)+' files, instead there are '+trim(n_elements(result))+' files.' ,$
         result)
     return
endif
idx=where(strpos(result,'fidlError') ne -1,cnt)
if cnt ne 0 then begin
     stat=dialog_message_long('fidlError',result[idx])
     return
endif
glmnamesin = rtn.scratchdir+rtn.roots+'.glm'
glmlist = rtn.scratchdir+'glm.list'
openw,lu,glmlist,/GET_LUN
for i=0,rtn.nsubject-1 do printf,lu,glmnamesin[i]
close,lu
free_lun,lu
nrun = 2L
ifh = replicate({InterFile_Header},rtn.nsubject,nrun)
conc = strarr(rtn.nsubject,nrun)
openw,lu,rtn.script,/GET_LUN,/APPEND
k = 0
for i=0,rtn.nsubject-1 do begin
    for j=0,nrun-1 do begin
        scraplabel = strsplit(result[k],/EXTRACT)
        scraplabel = scraplabel[n_elements(scraplabel)-1]
        ifh[i,j] = read_mri_ifh(scraplabel)
        conc[i,j] = scraplabel

        ;START130123
        if rtn.mask ne '' then begin
            if ptr_valid(ifh[i,j].glm_mask_file) then ptr_free,ifh[i,j].glm_mask_file
            ifh[i,j].glm_mask_file = ptr_new(rtn.mask)
        endif

        k = k + 1
    endfor

    ;START130123
    ifh[i,*].glm_fwhm = rtn.monte_carlo_fwhm

    tdim_file = [ifh[i,0].matrix_size_4,ifh[i,1].matrix_size_4]
    tdim = long(total(tdim_file))
    valid_frms = make_array(tdim,/FLOAT,VALUE=1.)
    slope = dblarr(tdim)
    ;print,'nrun=',nrun
    ;print,'tdim=',tdim
    ;print,'size(tdim)=',size(tdim)
    ;print,'tdim_file=',tdim_file
    stat=call_external(!SHARE_LIB,'_trend',nrun,tdim,tdim_file,valid_frms,slope,VALUE=[1,1,0,0,0])
    if stat eq 0L then begin
        print,'Error in _trend'
        return
    endif
    slope = float(slope)
    A = fltarr(tdim,4) 
    A[0:tdim_file[0]-1,0] = slope[0:tdim_file[0]-1] 
    A[tdim_file[0]:tdim-1,1] = slope[tdim_file[0]:tdim-1] 
    A[0:tdim_file[0]-1,2] = make_array(tdim_file[0],/FLOAT,VALUE=1.) 
    A[tdim_file[0]:tdim-1,3] = make_array(tdim_file[1],/FLOAT,VALUE=1.) 
    A = transpose(A)
    nmain = 0 
    N = 4 
    tot_eff = 0 
    xdim = ifh[i,0].matrix_size_1
    ydim = ifh[i,0].matrix_size_2
    zdim = ifh[i,0].matrix_size_3
    dxdy = ifh[i,0].scale_1
    dz = ifh[i,0].scale_3
    tcomp = tdim 
    num_trials = tdim 
    TR = 1.
    all_eff = 2
    effect_column = [0,2] 
    effect_label = ['Trend','Baseline']
    effect_length = make_array(2,/INTEGER,VALUE=2)
    glm1 = {General_linear_model}

    ;set_glm1,glm1,A,valid_frms,nmain,N,tot_eff,xdim,ydim,zdim,tdim,dxdy,dz,nrun,tcomp, $
    ;    num_trials,TR,stimlen,lcfunc,functype,DELAY=delay,FUNCLEN=funclen, $
    ;    NF=1,all_eff,effect_column,effect_label,effect_length,EFFECT_TR=effect_TR,EFFECT_SHIFT_TR=effect_shift_TR
    ;START141201
    set_glm1,glm1,A,valid_frms,nmain,N,tot_eff,xdim,ydim,zdim,tdim,dxdy,dz,nrun,tcomp, $
        num_trials,TR,stimlen,lcfunc,functype,DELAY=delay,funclen, $
        NF=1,all_eff,effect_column,effect_label,effect_length,EFFECT_TR=effect_TR,EFFECT_SHIFT_TR=effect_shift_TR

    glm_file = save_linmod(fi,dsp,wd,glm,help,stc,pref,fi.n,fi.n,swapbytes,FILENAME=glmnamesin[i],IFH=ifh[i,0],GLM1=glm1,/DESIGN_ONLY)
    if glm_file eq 'ERROR' then return
    print_files_to_csh,lu,nrun,conc[i,*],'BOLD_FILES','bold_files',/NO_NEWLINE
    printf,lu,'set INPUT_GLM_FILE = ','"','-input_glm_file ',glmnamesin[i],'"',FORMAT='(a,a,a,a,a)'
    printf,lu,'set OUTPUT_GLM_FILE = ','"','-output_glm_file ',glmnamesin[i],'"',FORMAT='(a,a,a,a,a)'
    printf,lu,'set MASK_FILE'
    printf,lu,'set MODE = ','"','-mode ',mode,'"',FORMAT='(a,a,a,1x,i1,a)'
    printf,lu,'nice +19 $BIN/compute_glm2 $BOLD_FILES $INPUT_GLM_FILE $OUTPUT_GLM_FILE $MASK_FILE $MODE'
    printf,lu,''
endfor
close,lu
free_lun,lu

;slopes = {glmlist:glmlist,atlasspace:rtn.atlasspace,fidl_ttest_csh:rtn.script,driver:rtn.scratchdir+'fidl_ttest.dat', $
;    scratchdir:rtn.scratchdir}
;START130128
slopes = {glmlist:glmlist,atlasspace:rtn.atlasspace,fidl_ttest_csh:rtn.script,driver:rtn.scratchdir+'fidl_ttest.dat', $
    scratchdir:rtn.scratchdir,region_or_uncompress_str:rtn.region_or_uncompress_str}

rtn1=fidl_ttest(fi,dsp,wd,glm,help,stc,pref,SLOPES=slopes)
if rtn1.msg ne 'OK' then return
print,'DONE'
end
