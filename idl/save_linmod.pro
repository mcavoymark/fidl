;Copyright 12/31/99 Washington University.  All Rights Reserved.
;save_linmod.pro  $Revision: 12.141 $
function save_linmod,fi,dsp,wd,glm,help,stc,pref,idx,hdr_model,swapbytes,FILENAME=filename,DESIGN_ONLY=design_only, $
    NO_LOAD=no_load,IFH=ifh,CLOSELU=closelu,GLM1=glm1
if not keyword_set(GLM1) then glm1=glm[hdr_model]
if not keyword_set(IFH) then begin
    hd = *fi.hdr_ptr[idx]
    hdr = *fi.hdr_ptr[hd.mother]
    ifh = hdr.ifh
endif 
if ptr_valid(glm1.sd) then dsgn_only = !FALSE else dsgn_only = !TRUE
if keyword_set(DESIGN_ONLY) then dsgn_only = !TRUE
nc = glm1.nc
if nc gt 0 then begin
    c = *glm1.c
    cnorm = *glm1.cnorm
    if ptr_valid(glm1.contrast_labels) then contrast_labels = *glm1.contrast_labels else contrast_labels = ''
endif
tot_eff = glm1.tot_eff

;START160915
;if dsgn_only eq !FALSE then begin
;    ATAm1 = *glm1.ATAm1
;    sd = *glm1.sd
;    var = *glm1.var
;    if ptr_valid(glm1.fstat) then fstat = float(*glm1.fstat) else fstat = fltarr(glm1.xdim,glm1.ydim,glm1.zdim,glm1.nF)
;    if ptr_valid(glm1.fzstat) then fzstat = float(*glm1.fzstat) else fzstat = fltarr(glm1.xdim,glm1.ydim,glm1.zdim,glm1.nF)
;    grand_mean = *glm1.grand_mean
;    b = *glm1.b
;endif

if not keyword_set(FILENAME) then begin
    filename = fi.list[idx]
    get_dialog_pickfile,'*.glm',getenv('PWD'),'Please specify GLM',rtn_filename,rtn_nfiles,rtn_path,FILE=filename
    filename = rtn_filename
    if filename eq 'GOBACK' or filename eq 'EXIT' then return,'ERROR'
endif
if !BATCH eq !FALSE then widget_control,/HOURGLASS
tdim = fix(n_elements(valid_frms))
spawn,!BINEXECUTE+'/fidl_timestr2',timestr
lizard = 'fidl_save_linmod_'+timestr[0]+'.glm'
openw,lu,lizard,/GET_LUN,ERROR=err
if err ne 0 then begin
    print,'!ERROR_STATE.MSG=',!ERROR_STATE.MSG
    return,'ERROR'
endif
glm1.rev = !GLM_FILE_REV
if n_elements(swapbytes) eq 0 then swapbytes=shouldiswap(ifh.bigendian)
ifh.glm_rev = glm1.rev
ifh.glm_nF = glm1.nF
ifh.glm_n_files = glm1.n_files
put_mri_ifh,lu,ifh,glm1
printf,lu,'START_BINARY'
if ptr_valid(glm1.cifti_xmldata) then writeu_swap,lu,*glm1.cifti_xmldata,SWAPBYTES=swapbytes
if nc ne 0 then begin
    len=fix(strlen(contrast_labels))
    for i=0,nc-1 do begin
        writeu_swap,lu,len[i],SWAPBYTES=swapbytes
        writeu_swap,lu,contrast_labels[i],SWAPBYTES=swapbytes
    endfor
endif
scrappos = 0L
point_lun,-lu,scrappos
if ptr_valid(glm1.A) then begin
    A=*glm1.A
    writeu_swap,lu,A[0:glm1.t_valid-1,0:glm1.N-1],SWAPBYTES=swapbytes
endif
scrappos = 0L
point_lun,-lu,scrappos
if nc gt 0 then begin
    writeu_swap,lu,c,SWAPBYTES=swapbytes
    writeu_swap,lu,cnorm,SWAPBYTES=swapbytes
endif
if ptr_valid(glm1.valid_frms) then begin
    valid_frms=*glm1.valid_frms
    writeu_swap,lu,valid_frms,SWAPBYTES=swapbytes
endif
if tot_eff gt 0 then begin
    if ptr_valid(glm1.delay) then begin
        delay=*glm1.delay
        writeu_swap,lu,delay,SWAPBYTES=swapbytes
    endif
    if ptr_valid(glm1.stimlen) then begin
        stimlen=*glm1.stimlen
        writeu_swap,lu,stimlen,SWAPBYTES=swapbytes
    endif
    if ptr_valid(glm1.lcfunc) then begin
        lcfunc=*glm1.lcfunc
        writeu_swap,lu,lcfunc,SWAPBYTES=swapbytes
    endif
endif
end_dsgn_pos = 0L
point_lun,-lu,end_dsgn_pos
start_data = end_dsgn_pos + 4
;print,'end_dsgn_pos=',end_dsgn_pos,' start_data=',start_data,' swapbytes=',swapbytes
glm1.start_data = start_data
writeu_swap,lu,glm1.start_data,SWAPBYTES=swapbytes
if dsgn_only eq 0 then begin
    point_lun,lu,glm1.start_data

    ;START160915
    if ifh.glm_masked eq 1 then begin
        writeu_swap,lu,*glm1.maskimg,SWAPBYTES=swapbytes
    endif

    ;writeu_swap,lu,ATAm1,SWAPBYTES=swapbytes
    ;START160915
    if ifh.glm_df ne !UNSAMPLED_VOXEL then begin
        ATAm1=*glm1.ATAm1
        len1=0L
        for i=1,glm1.N do len1=len1+i
        temp_float=fltarr(len1)
        i=0L
        for j=0,glm1.N-1 do begin
            for k=j,glm1.N-1 do begin
                temp_float[i]=float(ATAm1[j,k])
                i=i+1L
            endfor
        endfor
        writeu_swap,lu,temp_float,SWAPBYTES=swapbytes
        undefine,temp_float,ATAm1
    endif else begin
        ATAm1vox=*glm1.ATAm1vox
        for i=0,len1-1 do writeu_swap,lu,ATAm1vox[*,*,*,i],SWAPBYTES=swapbytes
        undefine,ATAm1vox
        writeu_swap,lu,*glm1.dfvox,SWAPBYTES=swapbytes
    endelse

    ;writeu_swap,lu,float(sd),SWAPBYTES=swapbytes
    ;writeu_swap,lu,double(var),SWAPBYTES=swapbytes
    ;START160915
    if ifh.glm_masked eq 0 then begin
        writeu_swap,lu,float(*glm1.var),SWAPBYTES=swapbytes
    endif else begin
        var=reform(*glm1.var,glm1.xdim*glm1.ydim*glm1.zdim)
        temp_float=float(var[*glm1.maski])
        writeu_swap,lu,temp_float,SWAPBYTES=swapbytes
        undefine,var,temp_float
    endelse

    ;for i=0,glm1.nF-1 do writeu_swap,lu,float(fstat[*,*,*,i]),SWAPBYTES=swapbytes
    ;for i=0,glm1.nF-1 do writeu_swap,lu,float(fzstat[*,*,*,i]),SWAPBYTES=swapbytes
    ;START160915
    if glm1.nF ne 0 then begin
        if ptr_valid(glm1.fstat) then fstat=*glm1.fstat else fstat=fltarr(glm1.xdim,glm1.ydim,glm1.zdim,glm1.nF)
        if ptr_valid(glm1.fzstat) then fzstat=*glm1.fzstat else fzstat=fltarr(glm1.xdim,glm1.ydim,glm1.zdim,glm1.nF)
        if ifh.glm_masked eq 0 then begin
            for i=0,glm1.nF-1 do writeu_swap,lu,fstat[*,*,*,i],SWAPBYTES=swapbytes
            for i=0,glm1.nF-1 do writeu_swap,lu,fzstat[*,*,*,i],SWAPBYTES=swapbytes
        endif else begin
            for i=0,glm1.nF-1 do begin
                temp_float=reform(fstat[*,*,*,i],glm1.xdim*glm1.ydim*glm1.zdim)
                temp_float=temp_float[*glm1.maski]
                writeu_swap,lu,temp_float,SWAPBYTES=swapbytes
            endfor
            for i=0,glm1.nF-1 do begin
                temp_float=reform(fzstat[*,*,*,i],glm1.xdim*glm1.ydim*glm1.zdim)
                temp_float=temp_float[*glm1.maski]
                writeu_swap,lu,temp_float,SWAPBYTES=swapbytes
            endfor
            undefine,temp_float
        endelse
        undefine,fstat,fzstat
    endif 

    ;writeu_swap,lu,grand_mean,SWAPBYTES=swapbytes
    ;START160915
    if ifh.glm_masked eq 0 then begin
        writeu_swap,lu,*glm1.grand_mean,SWAPBYTES=swapbytes
    endif else begin
        temp_float=reform(*glm1.grand_mean,glm1.xdim*glm1.ydim*glm1.zdim)
        temp_float=temp_float[*glm1.maski]
        writeu_swap,lu,temp_float,SWAPBYTES=swapbytes
        undefine,temp_float
    endelse

    ;for i=0L,glm1.zdim*glm1.N-1 do begin
    ;    x = b[*,*,i]
    ;    writeu_swap,lu,x,SWAPBYTES=0
    ;endfor
    ;START160915
    b=*glm1.b
    if ptr_valid(glm1.cifti_xmldata) then begin
        for i=0L,glm1.N-1 do writeu_swap,lu,b[*,i],SWAPBYTES=0
    endif else if ifh.glm_masked eq 0 then begin
        for i=0L,glm1.zdim*glm1.N-1 do writeu_swap,lu,b[*,*,i],SWAPBYTES=0
    endif else begin
        for i=0L,glm1.N-1 do writeu_swap,lu,b[*,i],SWAPBYTES=0
    endelse


    if keyword_set(CLOSELU) then begin
        close,closelu
        free_lun,closelu
    endif
endif
close,lu
free_lun,lu
if(dsgn_only eq !FALSE) and (not keyword_set(NO_LOAD)) then begin
    if strpos(fi.names[hdr_model]+'.glm',filename) eq 0 then begin $ ;only delete if filenames are the same
        delete_image,wd,fi,dsp,glm,stc,hdr_model
    endif
endif
if !BATCH eq !FALSE then widget_control,/HOURGLASS
if filename ne lizard then begin
    cmd = 'mv -f '+lizard+' '+filename
    spawn,cmd,result
    openr,lu,filename,/GET_LUN,ERROR=err
    if err ne 0 then begin
        stat = dialog_message('Unable to open '+filename+string(10B)+'You must not have write permission to this directory.', $
            /ERROR)
        spawn,'rm '+lizard,result
        return,'ERROR'
    endif
    close,lu
    free_lun,lu
endif

;IDL 5.4
;spawn,cmd,result,errresult 
;if errresult[0] ne '' then begin
;    stat = dialog_message(errresult+string(10B)+'You must not have write permission to this directory.',/ERROR)
;    spawn,'rm scratch.glm',result
;    return,'ERROR'
;endif

print,'Linear model written to ',filename
if(dsgn_only eq !FALSE) and (not keyword_set(NO_LOAD)) then begin
    filename = load_linmod(fi,dsp,wd,glm,help,ifh,FILENAME=filename)
    dsp_image = update_image(fi,dsp,wd,stc,pref) ;This call to update_image prevents the big bad black screen
                                                 ;from flying up if the user has not invoked a working window.
endif
return,filename
end
