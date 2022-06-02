;Copyright 12/31/99 Washington University.  All Rights Reserved.
;apply_mask.pro $Revision: 12.91 $
pro apply_mask,fi,wd,dsp,help,stc,pref
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='The masked image consists of all voxels abs()>1e-37 ' $
    +'in the mask image. Thus the mask image need not be a binary image.' 
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,id_mask,'mask image', $
    TITLE='Please select mask image.') eq !ERROR then return
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hd,id_tgt,'image to be masked', $
    TITLE='Please select image to be masked.') eq !ERROR then return
result = fltarr(hd.xdim,hd.ydim,hd.zdim,hd.tdim)
for z=0,hd.zdim-1 do begin
    widget_control,wd.error,SET_VALUE=string('Plane: ',z+1)
    mask = get_image(z,fi,stc,FILNUM=id_mask+1)

    ;mask = mask ne 0.
    ;START120713
    mask = abs(mask) gt !UNSAMPLED_VOXEL 

    for t=0,hd.tdim-1 do begin
        tgt = get_image(t*hd.zdim+z,fi,stc,FILNUM=id_tgt+1)

        ;START120713
        tgt = (abs(tgt) gt !UNSAMPLED_VOXEL)*tgt

        result[*,*,z,t] = mask*tgt

    endfor
endfor
name = strcompress(string(fi.tails(id_tgt),FORMAT='(a,"_masked")'),/REMOVE_ALL)

;put_image,reform(result,hd.xdim,hd.ydim,hd.zdim*hd.tdim),fi,wd,dsp,name,hd.xdim,hd.ydim,hd.zdim,hd.tdim,!FLOAT,hd.dxdy,hd.dz,1.,!FLOAT_ARRAY,hd.mother,paradigm,IFH=hd.ifh
;START150731
put_image,reform(result,hd.xdim,hd.ydim,hd.zdim*hd.tdim),fi,wd,dsp,name,hd.xdim,hd.ydim,hd.zdim,hd.tdim,!FLOAT,hd.dxdy,hd.dz,1.,!FLOAT_ARRAY,hd.mother,IFH=hd.ifh

end
