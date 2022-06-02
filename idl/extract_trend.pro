;Copyright 5/9/01 Washington University.  All Rights Reserved.
;extract_trend.pro  $Revision: 1.5 $
;***************************************
pro extract_trend,fi,dsp,wd,glm,help,stc
;***************************************

hd = *fi.hdr_ptr[fi.n]
mom = hd.mother
hdr = *fi.hdr_ptr[mom]

if(not ptr_valid(glm[mom].b)) then begin
    stat = widget_message('Model estimates are not defined.',/ERROR)
    return
endif

effect_label = *glm[mom].effect_label 
effect_length = *glm[mom].effect_length 
effect_column = *glm[mom].effect_column
n_files = glm[mom].n_files 
all_eff = glm[mom].all_eff 
xdim = glm[mom].xdim
ydim = glm[mom].ydim
zdim = glm[mom].zdim
b = *glm[mom].b
ATAm1 = *glm[mom].ATAm1

index = where(strcmp(strtrim(effect_label,2),'Trend'),count)
if count eq 0 then begin
    stat = widget_message('Trend not found.',/ERROR)
    return
endif else if count gt 1 then begin
    stat = widget_message('More than one trend found.',/ERROR)
    return
endif

trend = fltarr(hdr.xdim,hdr.ydim,n_files)

start_loop = effect_column[index] + effect_length[index]/n_files - 1
start_loop = start_loop[0] 
end_loop = effect_column[index]+effect_length[index] - 1
end_loop = end_loop[0] 
increment_loop = effect_length[index]/n_files
increment_loop = increment_loop[0] 

tc = fltarr(xdim,ydim,zdim,n_files)
widget_control,/HOURGLASS
t = 0
for i=start_loop,end_loop,increment_loop do begin
    for z=0,zdim-1 do tc[*,*,z,t] = tc[*,*,z,t] + get_assoc(b[*,*,i*zdim+z],hdr.ifh.bigendian)
    t = t + 1
endfor
name = strcompress(string(fi.tails[mom],FORMAT='(a,"_trend")'),/REMOVE_ALL)
widget_control,/HOURGLASS

;put_image,reform(tc,xdim,ydim,zdim*n_files),fi,wd,dsp,name,xdim,ydim,zdim,n_files,!FLOAT,hdr.dxdy,hdr.dz,1., $
;    !FLOAT_ARRAY,mom,-1,IFH=hdr.ifh,MODEL=mom
;START150731
put_image,reform(tc,xdim,ydim,zdim*n_files),fi,wd,dsp,name,xdim,ydim,zdim,n_files,!FLOAT,hdr.dxdy,hdr.dz,1., $
    !FLOAT_ARRAY,mom,IFH=hdr.ifh,MODEL=mom

end
