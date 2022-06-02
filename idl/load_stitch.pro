;Copyright 12/31/99 Washington University.  All Rights Reserved.
;load_stitch.pro $Revision: 12.113 $
function load_stitch,fi,stc,help,nfiles,files,name,dont_put_image,identify,filetype
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Stitches together an arbitrary number of 4DFP stacks.  ' $
    +'All images are loaded via associated variables (i.e., images are loaded as they are accessed by the software).  ' $
    +'Data sets are stitched together by using a data structure that maps specific frames to the correct files.'
tdim_file = lonarr(nfiles)
hdr_ptr = ptrarr(nfiles)
tdim_sum = lonarr(nfiles+1)
for nn=0,nfiles-1 do begin
    rh=read_header(files[nn],filetype[nn])
    if rh.msg ne 'OK' then return,rtn={msg:rh.msg} 
    xx = {name:'',xdim:rh.ifh.matrix_size_1,ydim:rh.ifh.matrix_size_2,zdim:rh.ifh.matrix_size_3,tdim:rh.ifh.matrix_size_4, $
        type:!FLOAT,dxdy:rh.ifh.scale_1,dz:rh.ifh.scale_3,scl:1.,array_type:!ASSOC_ARRAY,ifh:rh.ifh}
    hdr_ptr[nn] = ptr_new(xx)
    tdim_file[nn] = rh.ifh.matrix_size_4
    tdim_sum[nn+1] = tdim_sum[nn] + rh.ifh.matrix_size_4 
endfor
tdim_all = total(tdim_file)
t_to_file = -1 
for i=0,nfiles-1 do t_to_file = [t_to_file,make_array(tdim_file[i],/INT,VALUE=i)]
t_to_file = t_to_file[1:*]
tdim_sum_new = lonarr(nfiles)
tdim_sum_new[0] = long(tdim_file[0])
for i=1,nfiles-1 do tdim_sum_new[i] = long(tdim_sum_new[i-1]) + long(tdim_file[i])
stc1 = {Stitch}
stc1.name = name
stc1.tdim_sum = ptr_new(tdim_sum)
stc1.filnam = ptr_new(files)
stc1.tdim_file = ptr_new(tdim_file)
stc1.hdr_ptr = ptr_new(hdr_ptr)
stc1.tdim_all = tdim_all 
stc1.t_to_file = ptr_new(t_to_file)
stc1.n = nfiles
stc1.tdim_sum_new = ptr_new(tdim_sum_new)
stc1.identify = ptr_new(identify)
stc1.filetype=ptr_new(filetype)
if dont_put_image eq 0 then begin
    fi.n = fi.nfiles
    fi.lun[fi.n] = -1
    fi.paths[fi.n] = fi.path
    stc[fi.n] = stc1
endif
return,rtn={msg:'OK',stc1:stc1,ifh:rh.ifh,num_type:!FLOAT,type:!STITCH_ARRAY}
end
