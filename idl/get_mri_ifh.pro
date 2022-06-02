;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_mri_ifh.pro  $Revision: 12.125 $
pro get_mri_ifh,lu,ifh
ifh = {InterFile_Header}
ifh.glm_fwhm = 0.
ifh.glm_fwhmx = 0.
ifh.glm_fwhmy = 0.
ifh.glm_fwhmz = 0.
ifh.bigendian = 1
ifh.fwhm = 0.
ifh.nsub = 0
ifh.nreg = 0
region_names = '' 
harolds_num = -1 
behavior_names = '' 
nbehav = 0
eof = 0
ifh.nregfiles = 0
regfiles = ''
ifh.glm_nt4files = 0
t4files = ''
line = '' 
jF = 0
jF2 = 0
ifh.glm_boldtype = 'img' 
while eof eq 0 do begin
    readf,lu,line
    if strpos(line,'START_BINARY') ge 0 then goto,bottom 
    s = strtrim(strsplit(line,':=',/EXTRACT),2)
    if n_elements(s) lt 2 then begin 
        ;do nothing 
    endif else if(strpos(s[0],"image modality") ge 0) then $
	ifh.modality = s[1] $
    else if(strpos(s[0],"originating system") ge 0) then $
	ifh.orig_system = s[1] $
    else if(strpos(s[0],"conversion program") ge 0) then $
	ifh.conversion_program = s[1] $
    else if(strpos(s[0],"program version") ge 0) then $
	ifh.program_version = s[1] $
    else if(strpos(s[0],"program date") ge 0) then $
	ifh.program_date = s[1] $
    else if(strpos(s[0],"original institution") ge 0) then $
	ifh.original_institution = s[1] $
    else if(strpos(s[0],"name of data file") ge 0) then $
	ifh.data_file = s[1] $
    else if(strpos(s[0],"patient ID") ge 0) then $
	ifh.patient_ID = s[1] $
    else if(strpos(s[0],"study date") ge 0) then $
	ifh.study_date = s[1] $
    else if(strpos(s[0],"number format") ge 0) then $
	ifh.number_format = s[1] $
    else if(strpos(s[0],"number of bytes per pixel") ge 0) then $
	ifh.bytes_per_pixel = fix(s[1]) $
    else if(strpos(s[0],"orientation") ge 0) then $
	ifh.orientation = fix(s[1]) $
    else if(strpos(s[0],"time series flag") ge 0) then $
	ifh.time_series_flag = fix(s[1]) $
    else if(strpos(s[0],"number of dimensions") ge 0) then $
	ifh.num_dimensions = fix(s[1]) $
    else if(strpos(s[0],"matrix size [1]") ge 0) then $
	ifh.matrix_size_1 = long(s[1]) $
    else if(strpos(s[0],"matrix size [2]") ge 0) then $
	ifh.matrix_size_2 = long(s[1]) $
    else if(strpos(s[0],"matrix size [3]") ge 0) then $
	ifh.matrix_size_3 = long(s[1]) $
    else if(strpos(s[0],"matrix size [4]") ge 0) then $
	ifh.matrix_size_4 = long(s[1]) $
    else if(strpos(s[0],"scaling factor (mm/pixel) [1]") ge 0) then $
	ifh.scale_1 = float(s[1]) $
    else if(strpos(s[0],"scaling factor (mm/pixel) [2]") ge 0) then $
	ifh.scale_2 = float(s[1]) $
    else if(strpos(s[0],"scaling factor (mm/pixel) [3]") ge 0) then $
	ifh.scale_3 = float(s[1]) $
    else if(strpos(s[0],"slice thickness (mm/pixel)") ge 0) then $
	ifh.slice_thickness = float(s[1]) $
    else if(strpos(s[0],"matrix initial element [1]") ge 0) then $
	ifh.mat_init_element_1 = s[1] $
    else if(strpos(s[0],"matrix initial element [2]") ge 0) then $
	ifh.mat_init_element_2 = s[1] $
    else if(strpos(s[0],"matrix initial element [3]") ge 0) then $
	ifh.mat_init_element_3 = s[1] $
    else if(strpos(s[0],"global minimum") ge 0) then $
	ifh.global_min = float(s[1]) $
    else if(strpos(s[0],"global maximum") ge 0) then $
	ifh.global_max = float(s[1]) $
    else if(strpos(s[0],"Gaussian field smoothness") ge 0) then $
	ifh.smoothness = float(s[1]) $
    else if(strpos(s[0],"mri parameter file name") ge 0) then $
	ifh.mri_param_file = s[1] $
    else if(strpos(s[0],"mri sequence file name") ge 0) then $
	ifh.mri_seq_file = s[1] $
    else if(strpos(s[0],"mri sequence description") ge 0) then $
	ifh.mri_seq_description = s[1] $
    else if(strpos(s[0],"paradigm format") ge 0) then $ 
	ifh.mri_paradigm_format = s[1] $ 
    else if(strpos(s[0],"center") ge 0) then begin
        str = strsplit(s[1],/EXTRACT)
        ifh.center[0] = float(str[0])
        ifh.center[1] = float(str[1])
        ifh.center[2] = float(str[2])
    endif else if(strpos(s[0],"mmppix") ge 0) then begin
        str = strsplit(s[1],/EXTRACT)
        ifh.mmppix[0] = float(str[0])
        ifh.mmppix[1] = float(str[1])
        ifh.mmppix[2] = float(str[2])

    ;START191018
    endif else if(strpos(s[0],"c_orient") ge 0) then begin
        str = strsplit(s[1],/EXTRACT)
        c_orient=lonarr(3)
        c_orient[0] = long(str[0])
        c_orient[1] = long(str[1])
        c_orient[2] = long(str[2])
        ifh.c_orient=ptr_new(c_orient) 

    endif else if(strpos(s[0],"glm fwhm in voxels") ge 0) then $ ;this must precede
        ifh.glm_fwhm = float(s[1]) $
    else if(strpos(s[0],"fwhm in voxels") ge 0) then $
        ifh.fwhm = float(s[1]) $
    else if(strpos(s[0],"number of subjects") ge 0) then $
	ifh.nsub = fix(s[1]) $
    else if(strpos(s[0],"data description") ge 0) then $
        ifh.datades = s[1] $
    else if s[0] eq 'mask file' then $
        ifh.mask = s[1] $
    else if(strpos(s[0],"region names") ge 0) then begin
        str = strtrim(strsplit(s[1],/EXTRACT),2)
        harolds_num = [harolds_num,ifh.nreg+1] 
        region_names = [region_names,str[1]]
        ifh.nreg = ifh.nreg + 1
    endif else if(strpos(s[0],"behavior names") ge 0) then begin
        str = strtrim(strsplit(s[1],/EXTRACT),2)
	behavior_names = [behavior_names,str[1]] 
        nbehav = nbehav + 1
    endif else if(strpos(s[0],"region file") ge 0) then begin
        str = strtrim(strsplit(s[1],/EXTRACT),2)
	regfiles = [regfiles,str[0]] 
        ifh.nregfiles = ifh.nregfiles + 1
    endif else if(strpos(s[0],"degrees of freedom condition") ge 0) then $
        ifh.df1 = float(s[1]) $
    else if(strpos(s[0],"degrees of freedom error") ge 0) then $
        ifh.df2 = float(s[1]) $
    else if(strpos(s[0],"imagedata byte order") ge 0) then begin 
        if s[1] ne 'bigendian' then ifh.bigendian=0
    endif else if(strpos(s[0],"glm revision number") ge 0) then $
        ifh.glm_rev = fix(s[1]) $
    else if(strpos(s[0],"glm total number of estimates") ge 0) then $
        ifh.glm_N = fix(s[1]) $
    else if(strpos(s[0],"glm estimates of interest") ge 0) then $
        ifh.glm_n_interest = fix(s[1]) $
    else if(strpos(s[0],"glm column dimension of estimates") ge 0) then $
        ifh.glm_xdim = long(s[1]) $
    else if(strpos(s[0],"glm row dimension of estimates") ge 0) then $
        ifh.glm_ydim = long(s[1]) $
    else if(strpos(s[0],"glm axial dimension of estimates") ge 0) then $
        ifh.glm_zdim = long(s[1]) $
    else if(strpos(s[0],"glm number of frames in raw") ge 0) then $
        ifh.glm_tdim = long(s[1]) $
    else if(strpos(s[0],"glm number of usable frames") ge 0) then $
        ifh.glm_t_valid = long(s[1]) $
    else if(strpos(s[0],"glm degrees of freedom") ge 0) then $
        ifh.glm_df = float(s[1]) $
    else if(strpos(s[0],"glm number of contrasts") ge 0) then $
        ifh.glm_nc = long(s[1]) $
    else if(strpos(s[0],"glm number of trials") ge 0) then $
        ifh.glm_num_trials = fix(s[1]) $
    else if(strpos(s[0],"glm temporal sampling rate") ge 0) then $
        ifh.glm_TR = float(s[1]) $
    else if strpos(s[0],"glm number of effects of interest") ge 0 then begin
        ifh.glm_tot_eff = fix(s[1])
        if ifh.glm_tot_eff gt 0 then begin
            funclen = intarr(ifh.glm_tot_eff)
            functype = intarr(ifh.glm_tot_eff)
            effect_TR = fltarr(ifh.glm_tot_eff)
            effect_shift_TR = fltarr(ifh.glm_tot_eff)
        endif
    endif else if(strpos(s[0],"glm number of bold files") ge 0) then $
        ifh.glm_n_files = fix(s[1]) $
    else if(strpos(s[0],"glm random field smoothness") ge 0) then $
        ifh.glm_W = float(s[1]) $
    else if(strpos(s[0],"glm transverse voxel dimension") ge 0) then $
        ifh.glm_dxdy = float(s[1]) $
    else if(strpos(s[0],"glm axial voxel dimension") ge 0) then $
        ifh.glm_dz = float(s[1]) $
    else if strpos(s[0],"glm number of F statistics") ge 0 then begin
        ifh.glm_nF = fix(s[1])
        if ifh.glm_nF gt 0 then begin
            F_names = strarr(ifh.glm_nF)
            F_names2 = strarr(ifh.glm_nF)
        endif
    endif else if strpos(s[0],"glm description of F statistic") ge 0 then begin
        F_names[jF] = s[1]
        jF = jF + 1
    endif else if strpos(s[0],"glm name of F statistic") ge 0 then begin
        F_names2[jF2] = s[1]
        jF2 = jF2 + 1
    endif else if(strpos(s[0],"glm event file") ge 0) then $ 
        ifh.glm_event_file = ptr_new(s[1]) $
    else if strpos(s[0],"glm mask file") ge 0 then $ 
        ifh.glm_mask_file = ptr_new(s[1]) $
    else if strpos(s[0],"glm bold type") ge 0 then $
        ifh.glm_boldtype = s[1] $
    else if strpos(s[0],"glm cifti xml number of bytes") ge 0 then $
        ifh.glm_cifti_xmlsize = long(s[1]) $
    else if strpos(s[0],"glm t4 file") ge 0 then begin
        str = strtrim(strsplit(s[1],/EXTRACT),2)
        t4files = [t4files,str[0]]
        ifh.glm_nt4files = ifh.glm_nt4files + 1
    endif else if(strpos(s[0],"glm total number of effects") ge 0) then begin
        ifh.glm_all_eff = fix(s[1])
        if ifh.glm_all_eff gt 0 then begin
            jlab = 0
            jlen = 0
            jcol = 0
            effect_label = strarr(ifh.glm_all_eff)
            effect_length = intarr(ifh.glm_all_eff)
            effect_column = intarr(ifh.glm_all_eff)
        endif
    endif else if(strpos(s[0],"glm effect label") ge 0) then begin
        effect_label[jlab] = s[1] 
        jlab = jlab + 1
        index = ((jlab-1) < (ifh.glm_tot_eff-1)) > 0
    endif else if(strpos(s[0],"glm effect length") ge 0) then begin
        effect_length[jlen] = fix(s[1])
        jlen = jlen + 1
    endif else if(strpos(s[0],"glm effect column") ge 0) then begin
        effect_column[jcol] = fix(s[1])
        jcol = jcol + 1
    endif else if(strpos(s[0],"glm length of encoded function") ge 0) then $
        funclen[index] = fix(s[1]) $
    else if(strpos(s[0],"glm type of encoded function") ge 0) then $
        functype[index] = fix(s[1]) $
    else if(strpos(s[0],"glm effective TR") ge 0) then $
        effect_TR[index] = float(s[1]) $
    else if(strpos(s[0],"glm shift TR") ge 0) then $
        effect_shift_TR[index] = float(s[1]) $

    ;START160909
    else if strpos(s[0],"glm is masked") ge 0 then $
        ifh.glm_masked = fix(s[1]) 


    eof = eof(lu)
endwhile
bottom:
if jF ne 0 then begin
    if ptr_valid(ifh.glm_F_names) then ptr_free,ifh.glm_F_names
    ifh.glm_F_names = ptr_new(F_names)
    if jF2 ne 0 then begin
        if ptr_valid(ifh.glm_F_names2) then ptr_free,ifh.glm_F_names2
        ifh.glm_F_names2 = ptr_new(F_names2)
    endif else $
        ifh.glm_F_names2 = ptr_new(F_names)
    undefine,F_names,F_names2,jF,jF2
endif
if n_elements(effect_label) ne 0 then ifh.glm_effect_label = ptr_new(effect_label)
if n_elements(effect_length) ne 0 then ifh.glm_effect_length = ptr_new(effect_length)
if n_elements(effect_column) ne 0 then ifh.glm_effect_column = ptr_new(effect_column) 
if n_elements(funclen) ne 0 then ifh.glm_funclen = ptr_new(funclen)
if n_elements(functype) ne 0 then ifh.glm_functype = ptr_new(functype)
if ifh.glm_tot_eff gt 0 then begin
    index = where(effect_TR eq 0,count)
    if count gt 0 then effect_TR[index] = ifh.glm_TR
endif
if n_elements(effect_TR) ne 0 then ifh.glm_effect_TR = ptr_new(effect_TR)
if n_elements(effect_shift_TR) ne 0 then ifh.glm_effect_shift_TR = ptr_new(effect_shift_TR)
if ifh.nreg gt 0 then begin
    ifh.region_names = ptr_new(region_names[1:*])
    ifh.harolds_num = ptr_new(harolds_num[1:*])
endif
if nbehav gt 0 then ifh.behavior_names = ptr_new(behavior_names[1:*])
if ifh.nregfiles gt 0 then ifh.regfiles = ptr_new(regfiles[1:*])
if ifh.glm_nt4files gt 0 then ifh.glm_t4files = ptr_new(t4files[1:*])
ifh.msg='OK'
end
