;Copyright 12/31/99 Washington University.  All Rights Reserved.
;apply_linmod_lag.pro  $Revision: 12.114 $
pro apply_linmod_lag,fi,dsp,wd,glm,help,stc,pref,APPLY_LINMOD=apply_linmod
delay_inc = 0
if get_hdr_model(fi,glm,hdr,model) eq 'ERROR' then return
if hdr.ifh.glm_boldtype eq 'cifti' or hdr.ifh.glm_boldtype eq 'nifti' then begin
    compute_zstat,fi,dsp,wd,glm,help,stc,pref 
    return
endif
if help.enable eq !TRUE then begin
    if keyword_set(APPLY_LINMOD) then $
        scrap = 'This program computes z statistics using the contrasts defined with the Contrast Manager. ' $
    else $
        scrap = 'This routine will compute z-statistics for contrasts with several different delays. The contrasts defined ' $
            +'with the "Contrast Manager" are ignored. Instead, new contrasts using the Boynton model are computed on the ' $
            +'fly for each specified delay. '
    scrap = scrap + 'The magnitude and std. dev. will computed if this option is chosen under the "Files | Preferences" menu.'
    widget_control,help.id,SET_VALUE=scrap
endif
cnorm = *glm[model].cnorm
b = *glm[model].b
ATAm1 = *glm[model].ATAm1
sd = *glm[model].sd
N = glm[model].N
df = glm[model].df
effect_length = *glm[model].effect_length
effect_TR = *glm[model].effect_TR
effect_shift_TR = *glm[model].effect_shift_TR
tot_eff = glm[model].tot_eff
contrast_labels = *glm[model].contrast_labels
stimlen = *glm[model].stimlen
c_all = *glm[model].c
nc = glm[model].nc
c = fltarr(N)
rtn = select_files(contrast_labels,TITLE='Please select contrasts.',MIN_NUM_TO_SELECT=1,/EXIT)
if rtn.files[0] eq 'EXIT' then return
num_contrasts = rtn.count
iclist = rtn.index
iccontrastlabels = rtn.files
;for i=0,num_contrasts-1 do print,contrast_labels[iclist[i]],' ',cnorm[*,iclist[i]]
neff_per_contrast = total(abs(cnorm[*,iclist]),1)
index = where(neff_per_contrast eq 0,count)
if count eq nc and not keyword_set(APPLY_LINMOD) then begin
    stat = dialog_message('Your GLM file is too old. You need to go into the contrast manager and select "Default Contrasts". ' $
        +string(10B)+' Next use any of the other options to create the desired contrasts. Save the GLM and you should be good ' $
        +'to go.',/ERROR)
    return
endif
if not keyword_set(APPLY_LINMOD) then begin
    stat = get_delays(delay_str,num_delays,delay_inc,init_delay,type_str,/DONT_ASK)
    if check_delays(num_contrasts,iclist,num_delays,cnorm,tot_eff,effect_length) eq !ERROR then return
endif else $
    num_delays = 1
lc_magnorm = 'No'
if pref.glm_mag eq !TRUE then begin
    lc_magnorm = dialog_message('Do you want the contrast weights normalized to have a magnitude of 1?',/QUESTION)
endif
zstr = '_zstat'
mstr = '_mag'
sdstr = '_sd'
tstr = '_tstat'
if lc_magnorm eq 'Yes' then begin
    mstr = mstr + '1'
    sdstr = sdstr + '1'
endif
if not keyword_set(APPLY_LINMOD) then begin
    scrap = '_d' + strtrim(fix(num_delays),2)
    zstr = zstr + scrap
    mstr = mstr + scrap
    sdstr = sdstr + scrap
    tstr = tstr + scrap
    dstr = '_delaysec' + scrap
    if num_delays gt 1 then begin
        if type_str eq 'largest_tstat' then scrap = 'LT' else scrap = 'LM'
        zstr = zstr + scrap
        mstr = mstr + scrap
        sdstr = sdstr + scrap
        tstr = tstr + scrap
        dstr = dstr + scrap
    endif
endif
grand_mean = *glm[model].grand_mean ;grand_mean is sent as an argument and not overwritten by the routine
rtn = get_mean_and_mask(grand_mean,hdr.xdim,hdr.ydim,hdr.zdim)
grand_mean = rtn.mean
mean_mask = rtn.mean_mask
pct_chng_scl = rtn.pct_chng_scl
sd_mask = sd le 0
unsampled_mask = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
for z=0,hdr.zdim-1 do unsampled_mask[*,*,z] = get_assoc(b[*,*,z],hdr.ifh.bigendian)
unsampled_mask = unsampled_mask eq !UNSAMPLED_VOXEL
vol = long(hdr.xdim)*long(hdr.ydim)*long(hdr.zdim)
zstat = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
passtype_t_to_z = bytarr(4)
passtype_t_to_z[0] = 0
passtype_t_to_z[1] = 0
passtype_t_to_z[2] = 1
passtype_t_to_z[3] = 0
widget_control,/HOURGLASS
for jc=0,num_contrasts-1 do begin
    ic = iclist[jc]
    ;print,'ic=',ic
    ;tag = contrast_labels[ic]
    dt = 0.
    if not keyword_set(APPLY_LINMOD) then $
        ccnorm = reform(cnorm[*,ic],tot_eff) $
    else $
        c = reform(c_all[*,ic],N)

    for delay=0,num_delays-1 do begin
        if not keyword_set(APPLY_LINMOD) then begin
            passtype = bytarr(10)
            passtype[0] = 1
            passtype[8] = 1
            stat=call_external(!SHARE_LIB,'_boynton_contrast',tot_eff,effect_length,effect_TR,effect_shift_TR,dt,init_delay, $
                stimlen,ccnorm,N,c,VALUE=passtype)
        endif

        mag = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
        for i=0,N-1 do begin
            if c[i] ne 0. then begin
                for z=0,hdr.zdim-1 do begin

                    ;mag[*,*,z] = mag[*,*,z] +  c[i]*get_assoc(b[*,*,i*hdr.zdim+z])
                    ;START5
                    mag[*,*,z] = mag[*,*,z] +  c[i]*get_assoc(b[*,*,i*hdr.zdim+z],hdr.ifh.bigendian)

                endfor
            endif
        endfor

        tstat = (1-sd_mask)*mag/(sd + sd_mask)
        scl = sqrt(transpose(c)#ATAm1#c)
        print,'scl=',scl
        if scl[0] gt 0. then $
            tstat = tstat/scl[0] $
        else $
            tstat[*,*,*] = 0.
        mag = pct_chng_scl*(1-mean_mask)*mag/(grand_mean + mean_mask)

        magsd =  pct_chng_scl*scl[0]*(1-mean_mask)*sd/(grand_mean + mean_mask)
        if lc_magnorm eq 'Yes' then begin
            scl_mag = sqrt(transpose(c)#c)
            print,'scl_mag=',scl_mag
            mag = mag/scl_mag[0]
            magsd = magsd/scl_mag[0]
        endif

        if delay eq 0 then begin
            magmaxmin = mag
            tmaxmin = tstat
            sdmaxmin = magsd
            if num_delays gt 1 then begin
                dlymaxmin = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
                dlymaxmin[*,*,*] = init_delay + dt + !HRF_DELTA
            endif
        endif else begin
            if type_str eq 'largest_tstat' then $
                mask = abs(tstat) gt abs(tmaxmin) $
            else $
                mask = abs(mag) gt abs(magmaxmin)
            magmaxmin = (1-mask)*magmaxmin + mask*mag
            tmaxmin = (1-mask)*tmaxmin + mask*tstat
            sdmaxmin = (1-mask)*sdmaxmin + mask*magsd
            dlymaxmin = (1-mask)*dlymaxmin + mask*(init_delay+dt+!HRF_DELTA)
        endelse
        dt = dt + delay_inc
    endfor
    stat=call_external(!SHARE_LIB,'_t_to_z',float(tmaxmin),zstat,vol,double(df),VALUE=passtype_t_to_z)
    magmaxmin = (1-unsampled_mask)*magmaxmin + unsampled_mask*!UNSAMPLED_VOXEL
    sdmaxmin = (1-unsampled_mask)*sdmaxmin + unsampled_mask*!UNSAMPLED_VOXEL
    zstat = (1-unsampled_mask)*zstat + unsampled_mask*!UNSAMPLED_VOXEL
    if num_delays gt 1 then dlymaxmin = (1-unsampled_mask)*dlymaxmin + unsampled_mask*!UNSAMPLED_VOXEL
    scrap = fi.tails[model] + '_' + iccontrastlabels[jc]
    if num_delays gt 1 then begin
        name = scrap + dstr
        put_image,dlymaxmin,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,1.,!FLOAT_ARRAY, $
            hdr.mother,IFH=hdr.ifh
    endif
    if pref.glm_z eq !TRUE then begin
        name = scrap + zstr
        put_image,zstat,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,1.,!FLOAT_ARRAY, $
            hdr.mother,IFH=hdr.ifh,SMOOTHNESS=glm[model].W
    endif
    if pref.glm_mag eq !TRUE then begin
        name = scrap + mstr
        put_image,magmaxmin,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,1.,!FLOAT_ARRAY, $
            hdr.mother,IFH=hdr.ifh,SMOOTHNESS=glm[model].W
    endif
    if pref.glm_var eq !TRUE then begin
        name = scrap + sdstr
        put_image,sdmaxmin,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,1.,!FLOAT_ARRAY, $
            hdr.mother,UVALUE=df,IFH=hdr.ifh,SMOOTHNESS=glm[model].W
    endif
    if pref.glm_t eq !TRUE then begin
        name = scrap + tstr
        put_image,tmaxmin,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,1.,!FLOAT_ARRAY, $
            hdr.mother,IFH=hdr.ifh,SMOOTHNESS=glm[model].W
    endif
endfor
end
