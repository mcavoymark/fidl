;Copyright 12/31/99 Washington University.  All Rights Reserved.
;put_mri_ifh.pro  $Revision: 12.112 $
pro put_mri_ifh,lu,ifh,glm,IFH_ONLY=ifh_only
if keyword_set(IFH_ONLY) then ifh_only = !TRUE else ifh_only = !FALSE
printf,lu,FORMAT='("INTERFILE                         := ")'
printf,lu,FORMAT='("version of keys                   := 3.3")'
printf,lu,FORMAT='("image modality                    := ",a)',ifh.modality
printf,lu,FORMAT='("originating system                := ",a)',ifh.orig_system
printf,lu,FORMAT='("conversion program                := ",a)',ifh.conversion_program
printf,lu,FORMAT='("program version                   := ",a)',string('fidl revision ',!FIDL_REV)
printf,lu,FORMAT='("program date                      := ",a)',ifh.program_date 
printf,lu,FORMAT='("original institution              := ",a)',ifh.original_institution
printf,lu,FORMAT='("name of data file                 := ",a)',ifh.data_file
printf,lu,FORMAT='("patient ID                        := ",a)',ifh.patient_ID
printf,lu,FORMAT='("study date                        := ",a)',ifh.study_date 
printf,lu,FORMAT='("number format                     := ",a)',ifh.number_format
printf,lu,FORMAT='("number of bytes per pixel         := ",a)',string(ifh.bytes_per_pixel)
if ifh.bigendian eq 1 then scrap='bigendian' else scrap='littleendian' 
printf,lu,FORMAT='("imagedata byte order              := ",a)',scrap
printf,lu,FORMAT='("orientation                       := ",a)',string(ifh.orientation)
printf,lu,FORMAT='("time series flag                  := ",a)',string(ifh.time_series_flag)
printf,lu,FORMAT='("number of dimensions              := ",a)',string(ifh.num_dimensions)
printf,lu,FORMAT='("matrix size [1]                   := ",a)',string(ifh.matrix_size_1)
printf,lu,FORMAT='("matrix size [2]                   := ",a)',string(ifh.matrix_size_2)
printf,lu,FORMAT='("matrix size [3]                   := ",a)',string(ifh.matrix_size_3)
printf,lu,FORMAT='("matrix size [4]                   := ",a)',string(ifh.matrix_size_4)
printf,lu,FORMAT='("scaling factor (mm/pixel) [1]     := ",a)',string(ifh.scale_1)
printf,lu,FORMAT='("scaling factor (mm/pixel) [2]     := ",a)',string(ifh.scale_2)
printf,lu,FORMAT='("scaling factor (mm/pixel) [3]     := ",a)',string(ifh.scale_3)
printf,lu,FORMAT='("slice thickness (mm/pixel)        := ",a)',string(ifh.slice_thickness)
printf,lu,FORMAT='("matrix initial element [1]        := ",a)',ifh.mat_init_element_1
printf,lu,FORMAT='("matrix initial element [2]        := ",a)',ifh.mat_init_element_2
printf,lu,FORMAT='("matrix initial element [3]        := ",a)',ifh.mat_init_element_3
printf,lu,FORMAT='("global minimum                    := ",a)',string(ifh.global_min)
printf,lu,FORMAT='("global maximum                    := ",a)',string(ifh.global_max)
printf,lu,FORMAT='("mri parameter file name           := ",a)',ifh.mri_param_file
printf,lu,FORMAT='("mri sequence file name            := ",a)',ifh.mri_seq_file
printf,lu,FORMAT='("mri sequence description          := ",a)',ifh.mri_seq_description
printf,lu,FORMAT='("paradigm format                   := ",a)',ifh.mri_paradigm_format
if(ifh.matrix_size_1 eq 48) and (ifh.matrix_size_2 eq 64) and (ifh.matrix_size_3 eq 48) then begin
    ifh.mmppix[0] = !MMPPIX_X_333 
    ifh.mmppix[1] = !MMPPIX_Y_333 
    ifh.mmppix[2] = !MMPPIX_Z_333 
    ifh.center[0] = !CENTER_X_333
    ifh.center[1] = !CENTER_Y_333    
    ifh.center[2] = !CENTER_Z_333    
endif else if(ifh.matrix_size_1 eq 128) and (ifh.matrix_size_2 eq 128) and (ifh.matrix_size_3 eq 75) then begin
    ifh.mmppix[0] = !MMPPIX_X_222 
    ifh.mmppix[1] = !MMPPIX_Y_222 
    ifh.mmppix[2] = !MMPPIX_Z_222 
    ifh.center[0] = !CENTER_X_222 
    ifh.center[1] = !CENTER_Y_222
    ifh.center[2] = !CENTER_Z_222
endif
if(total(ifh.center^2) gt 0.) then $
    printf,lu,FORMAT='("center                            := ",a,1x,a,1x,a)', $
	string(ifh.center[0]),string(ifh.center[1]),string(ifh.center[2])
if(total(ifh.mmppix^2) gt 0.) then $
    printf,lu,FORMAT='("mmppix                            := ",a,1x,a,1x,a)', $
	string(ifh.mmppix[0]),string(ifh.mmppix[1]),string(ifh.mmppix[2])

;START191018
if ptr_valid(ifh.c_orient) then begin 
    c_orient = *ifh.c_orient
    printf,lu,FORMAT='("c_orient                          := ",a,1x,a,1x,a)', $
	string(c_orient[0]),string(c_orient[1]),string(c_orient[2])
endif

if ifh.glm_fwhm ge 0. then $
    printf,lu,FORMAT='("glm fwhm in voxels                := ",a)',string(ifh.glm_fwhm)      ;glm.fwhm
if ifh.df1 gt 0. then $
    printf,lu,FORMAT='("degrees of freedom condition      := ",a)',string(ifh.df1)
if ifh.df2 gt 0. then $
    printf,lu,FORMAT='("degrees of freedom error          := ",a)',string(ifh.df2)
if ifh_only eq !FALSE then begin
    printf,lu,FORMAT='("glm revision number               := ",a)',string(glm.rev)	  ;ifh.glm_rev
    printf,lu,FORMAT='("glm total number of estimates     := ",a)',string(glm.N)	  ;ifh.glm_N
    printf,lu,FORMAT='("glm estimates of interest         := ",a)',string(glm.n_interest) ;ifh.glm_n_interest
    printf,lu,FORMAT='("glm column dimension of estimates := ",a)',string(glm.xdim)	  ;ifh.glm_xdim
    printf,lu,FORMAT='("glm row dimension of estimates    := ",a)',string(glm.ydim)	  ;ifh.glm_ydim
    printf,lu,FORMAT='("glm axial dimension of estimates  := ",a)',string(glm.zdim)	  ;ifh.glm_zdim
    printf,lu,FORMAT='("glm number of frames in raw       := ",a)',string(glm.tdim)	  ;ifh.glm_tdim
    printf,lu,FORMAT='("glm number of usable frames       := ",a)',string(glm.t_valid)	  ;ifh.glm_t_valid
    printf,lu,FORMAT='("glm degrees of freedom            := ",a)',string(glm.df)	  ;ifh.glm_df
    printf,lu,FORMAT='("glm number of contrasts           := ",a)',string(glm.nc)	  ;ifh.glm_nc
    printf,lu,FORMAT='("glm BOLD response duration        := ",a)',string(glm.period)	  ;ifh.glm_period
    printf,lu,FORMAT='("glm number of trials              := ",a)',string(glm.num_trials) ;ifh.glm_num_trials
    printf,lu,FORMAT='("glm temporal sampling rate        := ",a)',string(glm.TR)	  ;ifh.glm_TR
    printf,lu,FORMAT='("glm number of effects of interest := ",a)',string(glm.tot_eff)    ;ifh.glm_tot_eff
    printf,lu,FORMAT='("glm total number of effects       := ",a)',string(glm.all_eff)	  ;ifh.glm_all_eff 

    ;effect_label = *glm.effect_label
    ;effect_length = *glm.effect_length
    ;effect_column = *glm.effect_column
    ;if glm.tot_eff gt 0 then lcfunc = *glm.lcfunc
    ;START151027
    if ptr_valid(glm.effect_label) then effect_label=*glm.effect_label
    if ptr_valid(glm.effect_length) then effect_length=*glm.effect_length
    if ptr_valid(glm.effect_column) then effect_column=*glm.effect_column
    if ptr_valid(glm.lcfunc) then lcfunc=*glm.lcfunc

    if ptr_valid(glm.funclen) then begin ;ifh.glm_funclen
        funclen = trim(*glm.funclen)
        nfunclen = n_elements(funclen)
    endif else $
        nfunclen = 0
    if ptr_valid(glm.functype) then functype = trim(*glm.functype) ;ifh.glm_functype
    if ptr_valid(glm.effect_TR) then effect_TR = *glm.effect_TR
    if ptr_valid(glm.effect_shift_TR) then effect_shift_TR = *glm.effect_shift_TR
    for i=0,glm.all_eff-1 do begin
        printf,lu,FORMAT='("glm effect label                  := ",a)',effect_label[i]
        printf,lu,FORMAT='("glm effect length                 := ",a)',string(effect_length[i])
        printf,lu,FORMAT='("glm effect column                 := ",a)',string(effect_column[i])
        if i lt glm.tot_eff then begin
            if lcfunc[i] eq !BLOCK_DESIGN then begin
                printf,lu,FORMAT='("glm type of encoded function      := ",a)',string(lcfunc[i])
            endif else if (lcfunc[i] eq 1 or (effect_length[i] eq 1 and i lt nfunclen)) and nfunclen gt 0 then begin
                printf,lu,FORMAT='("glm length of encoded function    := ",a)',funclen[i]
                printf,lu,FORMAT='("glm type of encoded function      := ",a)',functype[i]
            endif
            if ptr_valid(glm.effect_TR) then $
                printf,lu,FORMAT='("glm effective TR                  := ",a)',string(effect_TR[i])
            if ptr_valid(glm.effect_shift_TR) then $
                printf,lu,FORMAT='("glm shift TR                      := ",a)',string(effect_shift_TR[i])
        endif
    endfor
    printf,lu,FORMAT='("glm number of bold files          := ",a)',string(ifh.glm_n_files)
    printf,lu,FORMAT='("glm transverse voxel dimension    := ",a)',string(glm.dxdy)	  ;ifh.glm_dxdy
    printf,lu,FORMAT='("glm axial voxel dimension         := ",a)',string(glm.dz)	  ;ifh.glm_dz
    printf,lu,FORMAT='("glm number of F statistics        := ",a)',string(glm.nF)	  ;ifh.glm_nF
    if ptr_valid(glm.F_names) then begin
        F_names = *glm.F_names
        for i=0,glm.nF-1 do printf,lu,FORMAT='("glm description of F statistic    :=  ",a)',F_names[i]	  ;ifh.glm_F_names[i]
        if ptr_valid(glm.F_names2) then F_names2 = *glm.F_names2 else F_names2 = F_names
        for i=0,glm.nF-1 do printf,lu,FORMAT='("glm name of F statistic           :=  ",a)',F_names2[i]	  ;ifh.glm_F_names2[i]
    endif else begin
        print,'No F statistics defined.'
    endelse
    if ptr_valid(ifh.glm_event_file) then printf,lu,FORMAT='("glm event file                    := ",a)',*ifh.glm_event_file
    if ptr_valid(ifh.glm_mask_file) then printf,lu,FORMAT='("glm mask file                     := ",a)',*ifh.glm_mask_file
    if ifh.glm_nt4files gt 0 then begin
        t4files = *ifh.glm_t4files
        for i=0,ifh.glm_nt4files-1 do printf,lu,'glm t4 file := '+t4files[i]
    endif
    if ifh.glm_boldtype ne '' then printf,lu,FORMAT='("glm bold type                     := ",a)',ifh.glm_boldtype
    if ptr_valid(glm.cifti_xmldata) then printf,lu,FORMAT='("glm cifti xml number of bytes     := ",a)',trim(ifh.glm_cifti_xmlsize)

    ;START160909
    if ifh.glm_masked eq 1 then printf,lu,FORMAT='("glm is masked                     := ",a)',trim(ifh.glm_masked)

endif
if ifh.nregfiles gt 0 then begin
    regfiles = *ifh.regfiles
    for i=0,ifh.nregfiles-1 do printf,lu,'region file := '+regfiles[i]
endif
if ifh.nreg gt 0 then begin
    region_names = *ifh.region_names
    for i=0,ifh.nreg-1 do printf,lu,'region names := '+strtrim(i,2)+' '+region_names[i]
endif
if(ptr_valid(ifh.behavior_names)) then begin
    behavior_names = *ifh.behavior_names
    nbehav = n_elements(behavior_names)
    for i=0,nbehav-1 do $
        printf,lu,i,behavior_names[i],FORMAT='("behavior names  := ",i3,1x,a)'
endif
end
