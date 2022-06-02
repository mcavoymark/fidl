;Copyright 12/31/99 Washington University.  All Rights Reserved.
;$Id: f_to_z.pro,v 1.10 2015/07/31 20:46:25 mcavoy Exp $
pro f_to_z,fi,dsp,wd,stc,help
goback0:
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx,'F stat',TITLE='Please select 4dfp.') eq !ERROR then return
scrap = get_str(2,['numerator degrees of freedom','denominator degrees of freedom'], $
    strtrim([hdr.ifh.dof_condition,hdr.ifh.dof_error],2),LABEL='Please check degrees of freedom.',TITLE=fi.tails[idx],/GO_BACK)
if scrap[0] eq 'GO_BACK' then goto,goback0
hdr.ifh.dof_condition = float(scrap[0])
hdr.ifh.dof_error = float(scrap[1])
write_mri_ifh,fi.paths[idx]+fi.tails[idx]+'.4dfp.ifh',hdr.ifh
filename = fi.paths[idx]+fi.tails[idx]+'.4dfp.img'
*fi.hdr_ptr[idx] = hdr
passtype = bytarr(5)
passtype[2] = 1
fimg = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim)
zimg = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim)
lenvol = long(hdr.xdim)*long(hdr.ydim)*long(hdr.zdim)*long(hdr.tdim)
widget_control,/HOURGLASS
for t=0,hdr.tdim-1 do for z=0,hdr.zdim-1 do fimg[*,*,z,t] = get_image(z+t*hdr.zdim,fi,stc,FILNUM=idx+1)
stat=call_external(!SHARE_LIB,'_f_to_z',fimg,zimg,lenvol,double(hdr.ifh.dof_condition),double(hdr.ifh.dof_error),VALUE=passtype)
name = fi.tails[idx]
pos = strpos(name,'fstat')
if pos eq -1 then name = name + '_zstat' else strput,name,'z',pos

;put_image,reform(zimg,hdr.xdim,hdr.ydim,hdr.zdim*hdr.tdim),fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim, $
;    hdr.tdim,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother,paradigm,IFH=hdr.ifh
;START150731
put_image,reform(zimg,hdr.xdim,hdr.ydim,hdr.zdim*hdr.tdim),fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim, $
    hdr.tdim,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother,IFH=hdr.ifh

end
