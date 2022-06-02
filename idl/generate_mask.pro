;Copyright 12/31/99 Washington University.  All Rights Reserved.
;generate_mask.pro  $Revision: 12.93 $

;pro generate_mask,fi,wd,dsp,help,stc,pref
;START151016
pro generate_mask,fi,st,dsp,wd,stc,help,glm,pref

regname=''
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='A specified threshold wil be applied to the specified image.' $
    +' Two binary images will be generated. One will have a user specified value at every location where the voxel value was ' $
    +'above the threshold. The other will have the specified value at every location where the voxel value was below the ' $
    +'negative of the threshold. A third image includes both labeled +1 and -1, respectively.'


;ntails=0
;repeat begin
;    get_filelist_labels,fi,ntails,tails,index,ntails_glm,tails_glm,index_glm,ntails_conc,tails_conc,index_conc
;    if ntails eq 0 then begin
;        repeat begin
;            stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,/NOLOAD_COLOR,/MULTIPLE_FILES)
;            if stat eq !OK then begin
;                scrap = 1
;                if !D.WINDOW eq -1 then dsp_image = update_image(fi,dsp,wd,stc,pref)
;            endif else if stat eq !ERROR then begin
;                return
;            endif else begin
;                print,' *** Error while loading file. ***'
;                scrap = get_button(['Try again.','Exit'],TITLE='Error loading '+image_type_str+'.')
;                if scrap eq 1 then return
;            endelse
;        endrep until scrap eq 1
;    endif
;endrep until ntails ne 0
;goback0:
;rtn = select_files(tails,TITLE='Image(s) to be thresholded.',/EXIT,/ONE_COLUMN)
;if rtn.files[0] eq 'EXIT' then return
;START151016
goback0:
rtn=get_hdr_4dfp2(fi,st,dsp,wd,stc,help,glm,pref,'Please select files.')
;rtn=get_hdr_4dfp2(fi,st,dsp,wd,stc,help,glm,pref,TITLE='Please select files.')
if rtn.msg eq 'EXIT' then return


goback1:
strs = get_str(2,['Threshold','All voxels in mask will have value'],['0','1'],/GO_BACK,/EXIT)
if strs[0] eq 'EXIT' then return else if strs[0] eq 'GO_BACK' then goto,goback0
threshold = float(strs[0])
if threshold eq 0. then threshold=float(!UNSAMPLED_VOXEL)
assign = float(strs[1])
if assign eq 2. then begin
    regname = get_str(1,'region name','reg',TITLE='Please name the region.',LABEL='Leave blank to not make a region file.', $
        WIDTH=50,/GO_BACK)
    if regname[0] eq 'GO_BACK' then goto,goback1 
    regname = regname[0]
endif
threshstr = strtrim(string(threshold,FORMAT='(f5.1)'),2)

;for i=0,rtn.count-1 do begin
;    hdr = *fi.hdr_ptr[rtn.index[i]]
;START151016
for i=0,n_elements(rtn.idx)-1 do begin
    hdr = *fi.hdr_ptr[rtn.idx[i]]

    widget_control,/HOURGLASS
    profmask_plus = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
    profmask_minus = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
    image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)

    ;for z=0,hdr.zdim-1 do image[*,*,z] = get_image(z,fi,stc,FILNUM=rtn.index[i]+1)
    ;START151016
    for z=0,hdr.zdim-1 do image[*,*,z] = get_image(z,fi,stc,FILNUM=rtn.idx[i]+1)

    profmask_minus = (image lt -threshold)*assign
    profmask_plus = (image gt threshold)*assign
    tpp = total(profmask_plus gt 0.)
    tpm = total(profmask_minus gt 0.)
    print,'Non zero voxels  positive mask:',tpp,' negative mask:',tpm
    if tpp gt 0 then begin

        ;name = tails[rtn.index[i]]+'_pos_mask_t'+threshstr
        ;START151016
        name = fi.tails[rtn.idx[i]]+'_pos_mask_t'+threshstr

        ifh = hdr.ifh
        if ifh.nreg gt 0 then ifh.nreg = 0
        if regname ne '' then assign_region_names,ifh,profmask_plus,1,REGION_NAMES=regname
        put_image,profmask_plus,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!LOGICAL_ARRAY, $
            hdr.mother,IFH=ifh
        fi.color_scale1[fi.n] = !GRAY_SCALE
        fi.color_scale2[fi.n] = !NO_SCALE
    endif
    if tpm gt 0 then begin

        ;name = tails[rtn.index[i]]+'_neg_mask_t'+threshstr
        ;START151016
        name = fi.tails[rtn.idx[i]]+'_neg_mask_t'+threshstr

        ifh = hdr.ifh
        if ifh.nreg gt 0 then ifh.nreg = 0
        if regname ne '' then assign_region_names,ifh,profmask_minus,1,REGION_NAMES=regname
        put_image,profmask_minus,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!LOGICAL_ARRAY, $
            hdr.mother,IFH=ifh
        fi.color_scale1[fi.n] = !GRAY_SCALE
        fi.color_scale2[fi.n] = !NO_SCALE
    endif

    ;if assign eq 1. and tpp gt 0 and tpm gt 0 then begin
    ;START151016
    if assign ne 2. and tpp gt 0 and tpm gt 0 then begin

        profmask = -1*profmask_minus+profmask_plus 

        ;name = tails[rtn.index[i]]+'_all_mask_t'+threshstr
        ;START151016
        name = fi.tails[rtn.idx[i]]+'_all_mask_t'+threshstr

        ifh = hdr.ifh
        put_image,profmask,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!LOGICAL_ARRAY, $
            hdr.mother,IFH=ifh
        fi.color_scale1[fi.n] = !GRAY_SCALE
        fi.color_scale2[fi.n] = !NO_SCALE
    endif

endfor
end
