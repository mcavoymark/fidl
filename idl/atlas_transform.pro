;Copyright 12/31/99 Washington University.  All Rights Reserved.
;atlas_transform.pro  $Revision: 12.102 $
pro atlas_transform,fi,dsp,wd,stc,help,pref
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx_img,'image to be transformed') eq !ERROR then return
if idx_img lt 0 then return
get_dialog_pickfile,'*anat_ave_to_*_t4',fi.path[0],'Please select transformation matrix. Enter NONE if none.',t4_file,rtn_nfiles, $
    rtn_path
if t4_file eq 'GOBACK' or t4_file eq 'EXIT' then return
if t4_file eq 'NONE' then begin 
    print,'No t4 file entered, assuming the identity matrix.'
    space_in = get_space(hdr.xdim,hdr.ydim,hdr.zdim)
    if space_in eq !SPACE_111 then $
        atlas_in = 111 $
    else if space_in eq !SPACE_222 then $
        atlas_in = 222 $
    else if space_in eq !SPACE_333 then $
        atlas_in = 333 $
    else $
        atlas_in = 0
    if get_atlas_param(atlas_in,mmppix_in,center_in,ap_xdim_in,ap_ydim_in,ap_zdim_in,area_in,vol_in,ap_voxel_size_in,ap_str_in) $
        eq !ERROR then return
    twoAis0_twoBis1_unknown2 = 1
    t4 = fltarr(16)
endif else begin
    twoAis0_twoBis1_unknown2 = get_t4_type(t4_file)
    if twoAis0_twoBis1_unknown2 eq 2. then return
    t4 = read_xform(t4_file)
endelse
rtn=select_space()
space=rtn.space
atlas=rtn.atlas
atlas_str=rtn.atlas_str
if get_atlas_param(atlas,mmppix,center,ap_xdim,ap_ydim,ap_zdim,area,vol,ap_voxel_size,ap_str) eq !ERROR then return
if hdr.tdim gt 1 then begin
    tdim = hdr.tdim
    t4_image = fltarr(ap_xdim,ap_ydim,ap_zdim,tdim)
endif else begin
    tdim = 1
    t4_image = fltarr(ap_xdim,ap_ydim,ap_zdim)
endelse
image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
widget_control,/HOURGLASS
for t=0,tdim-1 do begin
    if tdim gt 0 then widget_control,wd.error,SET_VALUE=strcompress(string(t+1,FORMAT='("Processing frame #",i4)'))
    for z=0,hdr.zdim-1 do image[*,*,z] = get_image(z+t*hdr.zdim,fi,stc,FILNUM=idx_img+1)
    if t4_atlas(image,hdr.xdim,hdr.ydim,hdr.zdim,hdr.dxdy,hdr.dz,hdr.ifh.orientation,t4,twoAis0_twoBis1_unknown2, $
        t4img,atlas,center_in) eq 0L then return
    t4_image[*,*,*,t] = t4img
endfor
hdr.dxdy = ap_voxel_size[0] 
hdr.dz = ap_voxel_size[0] 
hdr.tdim = tdim
name = strcompress(string(fi.tails(idx_img),ap_str,FORMAT='(a,a)'),/REMOVE_ALL)

;put_image,reform(t4_image,ap_xdim,ap_ydim,ap_zdim*tdim),fi,wd,dsp,name,ap_xdim,ap_ydim,ap_zdim,tdim, $
;    !FLOAT,ap_voxel_size[0],ap_voxel_size[0],hdr.scl,!FLOAT_ARRAY,hdr.mother,paradigm,IFH=hdr.ifh
;START150731
put_image,reform(t4_image,ap_xdim,ap_ydim,ap_zdim*tdim),fi,wd,dsp,name,ap_xdim,ap_ydim,ap_zdim,tdim, $
    !FLOAT,ap_voxel_size[0],ap_voxel_size[0],hdr.scl,!FLOAT_ARRAY,hdr.mother,IFH=hdr.ifh

print,'DONE'
end
