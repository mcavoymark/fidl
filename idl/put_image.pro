;Copyright 12/31/99 Washington University.  All Rights Reserved.
;put_image.pro  $Revision: 12.106 $
pro put_image,image,fi,wd,dsp,file,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,mother,bigendian,UVALUE=uvalue, $
    MODEL=model,FILE_TYPE=file_type,IFH=ifh,SMOOTHNESS=smoothness,VIEW=view,DONTCHANGEIFH=dontchangeifh
if fi.nfiles ge !NUM_FILES-1 then begin
    stat=dialog_message('Too many files loaded.',/ERROR)
    st.error = !TRUE
    return
endif
if keyword_set(UVALUE) then usr_val = uvalue else usr_val = 0.
if not keyword_set(MODEL) then model = mother
if not keyword_set(FILE_TYPE) then file_type = !UNKNOWN
if not keyword_set(IFH) then ifh = {InterFile_Header}
if not keyword_set(DONTCHANGEIFH) then begin
    ifh.matrix_size_1 = xdim
    ifh.matrix_size_2 = ydim
    ifh.matrix_size_3 = zdim
    ifh.matrix_size_4 = tdim
    ifh.scale_1 = dxdy
    ifh.scale_2 = dxdy
    ifh.scale_3 = dz
    if n_elements(bigendian) ne 0 then ifh.bigendian=bigendian
endif
if(not keyword_set(SMOOTHNESS)) then smoothness = 0.
fi.n = fi.nfiles
dsp[fi.cw].file_num = fi.n
fi.nfiles = fi.nfiles + 1
fi.names[fi.n] = file
fi.name = file
if mother lt 0 then mother = fi.n
xx = {name:'',xdim:xdim(0),ydim:ydim(0),zdim:zdim(0),tdim:tdim(0), $
		type:num_type,dxdy:dxdy,dz:dz,scl:1.,array_type:type, $
		mother:mother,uval:usr_val,model:model,file_type:file_type, $
 	        ifh:ifh,smoothness:smoothness}
fi.hdr_ptr[fi.n] = ptr_new(xx)
if not keyword_set(VIEW) then fi.view[fi.n] = !TRANSVERSE else fi.view[fi.n] = view
pos = strpos(file,'/',/REVERSE_SEARCH)
len = strlen(file)
if pos ge 0 then tail = strmid(file,pos+1,len-pos-1) else tail = file
pos = strpos(tail,".4dfp")
if pos gt 0 then tail = strmid(tail,0,pos)
fi.tails[fi.n] = tail
fi.tail = tail
fi.list[fi.n] = tail
if type eq !LINEAR_MODEL then begin 
    if strpos(fi.list[fi.n],'.glm') eq -1 then fi.list[fi.n] = fi.list[fi.n] + '.glm'
endif
if type ne !STITCH_ARRAY then begin
    fi.paths[fi.n] = fi.path
    sz = size(image)
    if sz[0] ne 0 then begin
        fi.data_ptr[fi.n] = ptr_new(image)
    endif else begin
        if ptr_valid(fi.data_ptr[fi.n]) then ptr_free,fi.data_ptr[fi.n]
    endelse
endif
if(xdim(0) gt ydim(0)) then $
    dim = xdim(0) $
else $
    dim = ydim(0)
if(dsp[fi.cw].low_bandwidth eq !TRUE) then $
    zoom = 1 $
else $
    zoom = float(fix(256/float(dim)))
if(zoom lt 1) then zoom = 1
fi.zoom(fi.n) = zoom
dsp[fi.cw].zoom = fi.zoom(fi.n)
dsp[fi.cw].xdim = xdim(0)
dsp[fi.cw].ydim = ydim(0)
dsp[fi.cw].zdim = zdim(0)
dsp[fi.cw].tdim = tdim(0)
dsp[fi.cw].type = type
dsp[fi.cw].sizex = dsp[fi.cw].zoom*dsp[fi.cw].xdim
dsp[fi.cw].sizey = dsp[fi.cw].zoom*dsp[fi.cw].ydim
fi.space[fi.n] = get_space(dsp[fi.cw].xdim,dsp[fi.cw].ydim,dsp[fi.cw].zdim,VIEW=fi.view[fi.n])
assign_fi_atlas_pix_and_off,fi,ifh
control_pln_and_frm_sliders,fi,dsp,wd,stc
widget_control,wd.files,SET_VALUE=fi.list
widget_control,wd.files,SET_LIST_SELECT=fi.n
end
