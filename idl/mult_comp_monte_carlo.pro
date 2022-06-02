;Copyright 12/31/99 Washington University.  All Rights Reserved.
;mult_comp_monte_carlo.pro  $Revision: 1.24 $
pro mult_comp_monte_carlo,fi,wd,dsp,help,pref,dsp_image,stc,CVERSION=cversion
if not keyword_set(CVERSION) then cversion=0
stat_type = !F_STAT
act_type = !ALL_ACTIVATIONS 
if help.enable eq !TRUE then begin
    widget_control,help.id,SET_VALUE=string('Corrects for multiple comparisons using an algorithm based on the ' $
    +'height and spatial extent of a connected region. Regions consist of voxels connected through their faces.' $
    +string([10B,10B]) $
    +'This method is based on Monte Carlo simulations for a fixed p-value of 0.05.'+string([10B,10B]) $

    +'References:'+string(10B) $
    +'-----------'+string(10B) $
    +'M.P. McAvoy, J.M. Ollinger, R.L. Buckner. 2001. ' $
    +'Cluster size thresholds for Assessment of Significant Activation in fMRI. ' $
    +'NeuroImage 13(6-2): S198.'+string([10B,10B]) $

    +'S.D. Forman, J.D. Cohen, M. Fitzgerald, W.F. Eddy, M.A. Mintun, D.C. Noll. 1995.' $
    +'Improved assessment of significant activation in functional magnetic resonance imaging (FMRI): Use of a ' $
    +'cluster-size threshold. Magn. Reson. Med. 33:636-6475.'+string([10B,10B]) $

    +'NOTE: The height and spatial extents used for correction by this program were computed at NIL and are not ' $
    +'taken from Forman'+string(39B)+'s paper.')
endif
if cversion eq 0 then begin 
    if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx,'zmap to be corrected') eq !ERROR then return
    space = fi.space[idx]

    ;START140122
    fwhm = hdr.ifh.fwhm

endif else begin 
    goback1:
    gf = get_files('*.4dfp.img')
    if gf.msg eq 'GO_BACK' or gf.msg eq 'EXIT' then return 
    if n_elements(gf.files) eq 1 and gf.msg eq 'DONTCHECK' then begin 
        nfiles = n_elements(gf.files)
        files = gf.files
    endif else begin
        rtn = select_files(gf.files,TITLE='Please select.',/GO_BACK,MIN_NUM_TO_SELECT=1,/ONE_COLUMN)
        if rtn.files[0] eq 'GO_BACK' then goto,goback1
        nfiles = rtn.count
        files = rtn.files
    endelse
    space = intarr(nfiles)
    for i=0,nfiles-1 do begin
        ifh = read_mri_ifh(files[i])
        space = get_space(ifh.matrix_size_1,ifh.matrix_size_2,ifh.matrix_size_3)
    endfor
    if total(space[0]-space) ne 0 then begin
        stat=dialog_message('Not all images are in the same space. Abort!',/ERROR)
        return
    endif

    ;START140122
    fwhm = ifh.fwhm

endelse
goback4:
scrap = get_button(['T-statistic','F-statistic'],TITLE='What is the origin of the zmap?')
if scrap eq 0 then begin
    stat_type = !T_STAT
    scrap = get_button(['two sided (positives and negatives)','one sided (positives only)','one sided (negatives only)'], $
        TITLE='What type of correction do you wish to do?')
    if scrap eq 1 then $
        act_type = !POSITIVE_ACTIVATIONS $
    else if scrap eq 2 then $
        act_type = !NEGATIVE_ACTIVATIONS
endif

;START140122
if fwhm eq 0. then fwhm=2.

goback5:

;scrap = get_str(1,'FWHM in voxels: ','2',/GO_BACK)
;START140122
scrap = get_str(1,'FWHM in voxels: ',trim(fwhm),/GO_BACK)

if scrap[0] eq 'GO_BACK' then goto,goback4
fwhm = fix(scrap[0])

rtn = threshold_extent(stat_type,fwhm,space[0],ACT_TYPE=act_type)
threshold_extent_str = rtn.threshold_extent_str
if threshold_extent_str[0] eq 'GO_BACK' then goto,goback5
sz =size(rtn)
if(sz[n_elements(sz)-2] ne 8) then begin
    st = dialog_message('Monte Carlo simulations are only valid for images in 222 or 333 space')
    return
end
threshold = rtn.threshold
extent = rtn.extent
nthresh = rtn.count
case act_type of
    !ALL_ACTIVATIONS: act_type_str = 'all'
    !POSITIVE_ACTIVATIONS: act_type_str = 'pos'
    !NEGATIVE_ACTIVATIONS: act_type_str = 'neg'
    else: begin
        print,'Invalid index'
        return
    end
    endcase
predicate = '_mcomp_monte_carlo_'+rtn.tepstr+'_'+act_type_str
if cversion eq 0 then begin
    widget_control,/HOURGLASS
    zimage = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,/NOZERO)
    for z=0,hdr.zdim-1 do zimage[*,*,z] = get_image(z,fi,stc,FILNUM=idx+1)
    if act_type eq !POSITIVE_ACTIVATIONS then $ 
        zimage = zimage*(zimage gt 0) $
    else if act_type eq !NEGATIVE_ACTIVATIONS then $
        zimage = zimage*(zimage lt 0)
    ;mask = zimage eq !UNSAMPLED_VOXEL
    widget_control,/HOURGLASS
    for i=0,nthresh-1 do begin
        ;actmask = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
        ;num_sig_reg = spatial_extent(zimage,actmask,hdr.xdim,hdr.ydim,hdr.zdim,threshold[i],extent[i])
        ;print,'threshold = ',strcompress(string(threshold[i],FORMAT='(f6.2)'),/REMOVE_ALL), $
        ;      '  extent = ',strcompress(string(extent[i]),/REMOVE_ALL), $
        ;      '  num_sig_reg = ',strcompress(string(num_sig_reg),/REMOVE_ALL)
        ;actmask_bin = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
        ;index = where(actmask gt 0,count)
        ;if count ne 0 then actmask_bin[index] = 1
        ;statmap = (1-mask)*zimage*actmask_bin + mask*!UNSAMPLED_VOXEL
        ;widget_control,HOURGLASS=0
        ;paradigm= -1.
        ;name = fi.tails[idx]+predicate
        ;put_image,statmap,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, $
        ;    !FLOAT_ARRAY,hdr.mother,paradigm,IFH=hdr.ifh
        ;ifh = hdr.ifh ;Without this line of code, region names are not assigned to the ifh.
        ;if num_sig_reg gt 0 then assign_region_names,ifh,actmask,num_sig_reg
        ;name = name + '_regions'
        ;put_image,actmask,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, $
        ;    !FLOAT_ARRAY,hdr.mother,paradigm,IFH=ifh
        ;fi.color_scale1[fi.n] = !DIFF_SCALE
        ;fi.color_scale2[fi.n] = !NO_SCALE
        ;load_colortable,fi,dsp,TABLE1=!GRAY_SCALE,TABLE2=!NO_SCALE



        name = fi.tails[idx]+predicate[i]
        rtn=threshold_cluster_guts(fi,wd,dsp,hdr,zimage,threshold[i],extent[i],name)
        num_sig_reg = rtn.num_sig_reg
        if num_sig_reg gt 0 then begin
            actmask = rtn.actmask
            ifh = hdr.ifh ;Without this line of code, region names are not assigned to the ifh.
            if num_sig_reg gt 0 then assign_region_names,ifh,actmask,num_sig_reg
            name = name + '_regions'
            put_image,actmask,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, $
                !FLOAT_ARRAY,hdr.mother,paradigm,IFH=ifh
            fi.color_scale1[fi.n] = !GRAY_SCALE 
            fi.color_scale2[fi.n] = !NO_SCALE
            load_colortable,fi,dsp,TABLE1=fi.color_scale1[fi.n],TABLE2=fi.color_scale2[fi.n]
        endif


    endfor
endif else begin

    ;roots = get_root(files,'.4dfp.img')
    ;START64
    rtn_gr = get_root(files,'.4dfp.img')
    roots = rtn_gr.file 

    csh = 'fidl_threshold_cluster.csh'
    openw,lu,csh,/GET_LUN
    top_of_script,lu
    for i=0,nfiles-1 do begin
        ;printf,lu,'nice +19 $BIN'+'/fidl_threshold_cluster'+' -zmap '+files[i]+threshold_extent_str+' -'+act_type_str $
        ;    +' -output "'+strtrim(strjoin(roots[i]+predicate+'.4dfp.img',/SINGLE),2)+'"'
        ;printf,lu,'nice +19 $BIN'+'/fidl_threshold_cluster'+' -zmap '+files[i]+threshold_extent_str+' -'+act_type_str $
        ;    +' -output '+strtrim(strjoin(roots[i]+predicate+'.4dfp.img',' ',/SINGLE),2)
        ;START160725
        printf,lu,'nice +19 $BIN'+'/fidl_threshclus'+' -zmap '+files[i]+threshold_extent_str+' -'+act_type_str $
            +' -output '+strtrim(strjoin(roots[i]+predicate+'.4dfp.img',' ',/SINGLE),2)
    endfor
    close,lu
    free_lun,lu
    spawn,'chmod +x '+csh
    spawn_cover,csh,fi,wd,dsp,help,stc
endelse
print,'DONE'
end
