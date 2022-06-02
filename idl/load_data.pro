;Copyright 12/31/99 Washington University.  All Rights Reserved.
;load_data.pro $Revision: 12.151 $

;function load_data,fi,st,dsp,wd,stc,file_type,help,glm,stc1,FILENAME=filename,MODEL=model,FILTER=filter, $
;    NOLOAD_COLOR=noload_color,WHEREISIT=whereisit,TITLE=title,DONT_PUT_IMAGE=dont_put_image,MULTIPLE_FILES=multiple_files
;START150219
function load_data,fi,st,dsp,wd,stc,file_type,help,glm,FILENAME=filename,MODEL=model,FILTER=filter, $
    NOLOAD_COLOR=noload_color,WHEREISIT=whereisit,TITLE=title,DONT_PUT_IMAGE=dont_put_image,MULTIPLE_FILES=multiple_files

if fi.nfiles ge !NUM_FILES-1 then begin
    stat=dialog_message('Too many files loaded.',/ERROR)
    st.error = !TRUE
    return,!ERROR
endif
scl = 1.
mother = -1

;START150731
;paradigm = 0

smoothness = 0.
uvalue = 0
loaded = !FALSE
if not keyword_set(MODEL) then model = fi.nfiles
if keyword_set(FILTER) then filter_img = filter else filter_img = '*.img'
if not keyword_set(NOLOAD_COLOR) then noload_color = 0
if not keyword_set(DONT_PUT_IMAGE) then dont_put_image = 0
if not keyword_set(MULTIPLE_FILES) then multiple_files=0
case file_type of

;!ECAT_DATA: begin
;    if(help.enable eq !TRUE) then begin
;        widget_control,help.id,SET_VALUE=string('Loads atlas space (ECAT rev 6)  images.')
;    endif
;    get_dialog_pickfile,'*.img',fi.path[0],'Please select file.',filnam,rtn_nfiles,rtn_path
;    if filnam eq 'GO_BACK' or filnam eq 'EXIT' then return,!ERROR
;    fi.path = rtn.path
;    widget_control,/HOURGLASS
;    imgin = read_ecat(filnam,xdim,ydim,zdim,tdim,dxdy,dz)
;    ;Flip image to conform with 4dfp coordinates.
;    img = fltarr(xdim,ydim,zdim)
;    for z=0,zdim-1 do img[*,*,z] = reverse(imgin(*,*,zdim-z-1),1)
;    widget_control,HOURGLASS=0
;    if(n_elements(img) eq 1) then begin
; 	stat=widget_message('Error reading ecat image.',/ERROR)
;        return, !ERROR
;    endif
;    type = !FLOAT_ARRAY
;    num_type = !FLOAT
;    scl = 1.
;    mother = fi.nfiles
;end

!SPM_MAT: begin
    if(help.enable eq !TRUE) then begin
        widget_control,help.id,SET_VALUE=string('Loads results of an analysis by SPM96.')
    endif
    get_dialog_pickfile,'SPM*.mat',fi.path[0],'Please select file.',zfile,rtn_nfiles,rtn_path
    if zfile eq 'GOBACK' or zfile eq 'EXIT' then return,!ERROR
    fi.path = rtn_path

    dot = rstrpos(zfile,'.')
    dash = rstrpos(zfile,'_')
    if(dash gt 0) then begin
        code = strmid(file,dash+1,dot-dash-1)
        xyzfile = strcompress(string(code,FORMAT='("idlXYZ_",a,".mat")'),/REMOVE_ALL)
        xyzsigfile = strcompress(string(code,FORMAT='("idlXYZsig_",a,".mat")'),/REMOVE_ALL)
        ffile = strcompress(string(code,FORMAT='("SPMF_",a,".mat")'),/REMOVE_ALL)
        betafile = strcompress(string(code,FORMAT='("BETA_",a,".mat")'),/REMOVE_ALL)
        xafile = strcompress(string(code,FORMAT='("xafile_",a,".mat")'),/REMOVE_ALL)
        spmfile = strcompress(string(code,FORMAT='("SPM_",a,".mat")'),/REMOVE_ALL)
        spmtfile = strcompress(string(code,FORMAT='("SPMt_",a,".mat")'),/REMOVE_ALL)
        spmzsigfile = strcompress(string(code,FORMAT='("SPMzsig_",a,".mat")'),/REMOVE_ALL)
    endif else begin
        xyzfile = 'idlXYZ.mat'
        xyzsigfile = 'idlXYZsig.mat'
        ffile = 'SPMF.mat'
        betafile = 'BETA.mat'
        xafile = 'XA.mat'
        spmfile = 'SPM.mat'
        spmtfile = 'SPMt.mat'
        spmzsigfile = 'SPMzsig.mat'
    endelse
    value = strarr(4)
    labels = strarr(4)
    value(0) = string('64')
    value(1) = string('64')
    value(2) = string('16')
    value(3) = string('3')
    labels(0) = string("X dimension: ")
    labels(1) = string("Y dimension: ")
    labels(2) = string("Z dimension: ")
    labels(3) = string("Frames skipped: ")
    dims = get_str(4,labels,value)
    xdim = long(dims(0))
    ydim = long(dims(1))
    zdim = long(dims(2))
    skip = long(dims(3))
    indices = mat_read(xyzfile)
    if(n_elements(indices) eq 1) then begin
 	print,xyzfile
 	stat=widget_message('Error opening SPM raw indices.',/ERROR)
        return, !ERROR
    endif
    indices(*,*) = indices(*,*) - 1
    sig_indices = mat_read(xyzsigfile)
    if(n_elements(sig_indices) eq 1) then begin
 	print,xyzsigfile
 	stat=widget_message('Error opening SPM significant indices.',/ERROR)
        return, !ERROR
    endif
    labels = strarr(fi.nfiles + 1)
    labels(0:fi.nfiles-1) = fi.tails(0:fi.nfiles-1)
    labels(fi.nfiles) = 'Skip'
    idx = get_button(labels,TITLE='Data corresponding to SPM parameters.')
    sig_indices(*,*) = sig_indices(*,*) - 1
    zsig_scores = mat_read(spmzsigfile)
    if(n_elements(zsig_scores) eq 1) then begin
	print,spmzsigfile
 	stat=widget_message('Error opening SPM significant Z scores.',/ERROR)
        return, !ERROR
    endif
    img = scatter(sig_indices,zsig_scores,xdim,ydim,zdim)
    type = !FLOAT_ARRAY
    num_type = !FLOAT
    scl = 1.
    tdim = 1
    dxdy = 1.
    dz = 1.

    if(idx lt fi.nfiles) then begin
        mother = idx
	hdr = *fi.hdr_ptr(idx)
        spm = mat_read(spmfile)
        print,'Degrees of freedom: ',spm.df,' Sigma: ',spm.sigma
        if(n_tags(spm) eq 0) then begin
 	    stat=widget_message('Error opening SPM Z scores.',/ERROR)
            return, !ERROR
        endif
        ncond = n_elements(spm.contrast(0,*))
        ncov = n_elements(spm.c(0,*))
        nblk = n_elements(spm.b(0,*))
        ncnf = n_elements(spm.g(0,*))
        nfrm = n_elements(spm.c(*,0))
;	a = fltarr(nfrm,ncond)
	a = fltarr(nfrm,ncov+ncnf)
;        a(*,0:ncov-1) = spm.c
	for i=0,ncov-1 do begin
            mask = spm.c(*,i) ne 0
	    min = min(spm.c(*,i))
	    a(*,i) = mask*(spm.c(*,i) - min)
        endfor
        a(*,ncov:ncov+ncnf-1) = spm.g
	if(hdr.array_type ne !STITCH_ARRAY) then begin
	    valid_frms = fltarr(hdr.tdim)
            valid_frms(*) = 1
            valid_frms(0:skip-1) = 0
	endif else begin
	    valid_frms = fltarr(stc(idx).tdim_all)
            valid_frms(*) = 1
	    t = 0
            for i=0,stc(idx).n-1 do begin
                valid_frms(t:t+skip-1) = 0
	        hdr = *stc(idx).hdr_ptr(i)
		t = t + stc(idx).tdim_file[i]
	    endfor
	endelse
	glm(idx).spm = ptr_new(spm)
	glm(idx).a = ptr_new(a)
        nc = n_elements(spm.contrast(*,0))
        if(nc eq 1) then $
            c = reform(spm.contrast,ncond) $
	else $
	    c = spm.contrast
	glm(idx).c = ptr_new(c(*,0:ncov+ncnf-1))
	glm(idx).valid_frms = ptr_new(valid_frms)
    endif else begin
        mother = fi.n
    endelse

    ;put_image,img,fi,wd,dsp,spmzsigfile,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,mother,paradigm
    ;START150731
    put_image,img,fi,wd,dsp,spmzsigfile,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,mother

    filnam = spmzsigfile
    z_scores = mat_read(spmtfile)
    if(n_elements(z_scores) eq 1) then begin
 	stat=widget_message('Error opening SPM Z scores.',/ERROR)
        return, !ERROR
    endif
    img = scatter(indices,z_scores,xdim,ydim,zdim)
    if(n_elements(img) eq 1) then begin
 	stat=widget_message('Error importing SPM results.',/ERROR)
        return, !ERROR
    endif
    filnam = string(zfile,FORMAT='(a," (Really a Z map)")')
end

!MRI_4DFP: begin
    if help.enable eq 1 then widget_control,help.id,SET_VALUE='Loads 4DFPs using associated variables.' $
        +' The file should be on a local disk or all operations will be slowed.'
    if not keyword_set(FILENAME) then begin
        if not keyword_set(FILTER) then filter = fi.image_filter
        if not keyword_set(TITLE) then begin
            if not keyword_set(MULTIPLE_FILES) then title = 'Please select file.' else title = 'Please select files.'
        endif
        get_dialog_pickfile,filter,fi.path[0],title,filename,rtn_nfiles,rtn_path,/DONT_ASK,MULTIPLE_FILES=multiple_files
        if filename[0] eq 'GOBACK' or filename[0] eq 'EXIT' or filename[0] eq '' then return,!ERROR
        fi.path = rtn_path
    endif
    rload_conc=load_conc(fi,stc,dsp,help,wd,filename,DONT_PUT_IMAGE=dont_put_image,NOLOAD_COLOR=noload_color)
    if rload_conc.msg ne 'OK' then return,!ERROR
    loaded=1
end

;START160112
!LIST: begin
    if help.enable eq 1 then widget_control,help.id,SET_VALUE='Loads all files listed in a text file. All files are listed.'
    if not keyword_set(FILENAME) then begin
        if not keyword_set(FILTER) then filter = '*.list' 
        if not keyword_set(TITLE) then title = 'Please select a list.' 
        get_dialog_pickfile,filter,fi.path[0],title,filename,rtn_nfiles,rtn_path,/DONT_ASK,MULTIPLE_FILES=multiple_files
        if filename[0] eq 'GOBACK' or filename[0] eq 'EXIT' or filename[0] eq '' then return,!ERROR
        fi.path = rtn_path
    endif
    rl=read_list(filename)
    if rl.msg ne 'OK' then return,!ERROR
    files=''
    ;filetype=''
    if rl.nimg ne 0 then begin
        files=[files,rl.img]
        ;filetype=[filetype,rl.imgt]
    endif
    if rl.ndtseries ne 0 then begin
        files=[files,rl.dtseries]
        ;filetype=[filetype,rl.dtseriest]
    endif
    if rl.ndscalar ne 0 then begin
        files=[files,rl.dscalar]
        ;filetype=[filetype,rl.dscalart]
    endif
    if rl.nnii ne 0 then begin
        files=[files,rl.nii]
        ;filetype=[filetype,rl.niit]
    endif
    nfiles=n_elements(files)-1
    if nfiles gt 0 then begin
        filename=files[1:*]
        ;filetype=filetype[1:*]
        rload_conc=load_conc(fi,stc,dsp,help,wd,filename,DONT_PUT_IMAGE=dont_put_image,NOLOAD_COLOR=noload_color)
        if rload_conc.msg ne 'OK' then return,!ERROR
        loaded=1
    endif
end


!STITCH: begin
    str = get_str(1,'Name of entry',strcompress(string('Conc_',fi.nfiles),/REMOVE_ALL))
    name = str[0]
    files = get_list_of_files('*_xr3d.4dfp.img',fi.path,'Select files',indgen(1000)+1)
    rload_conc=load_conc(fi,stc,dsp,help,wd,name,n_elements(files),files,DONT_PUT_IMAGE=dont_put_image,NOLOAD_COLOR=noload_color)
    if rload_conc.msg ne 'OK' then return,!ERROR
    loaded=1
end
!RAW: begin

    ;stat = load_raw(fi,st,dsp,wd,stc,file_type,help,filnam,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,paradigm,img)
    ;START150731
    stat = load_raw(fi,st,dsp,wd,stc,file_type,help,filnam,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,0,img)

    if(stat eq !ERROR) then $
	return, !ERROR
    mother = fi.nfiles
end
;!MATRIX7V: begin
;    get_dialog_pickfile,'*.v',fi.path[0],'Please select file.',filnam,rtn_nfiles,rtn_path
;    if filnam eq 'GO_BACK' or filnam eq 'EXIT' then return,!ERROR
;    fi.path = rtn_path
;    num_type = !SHORT
;    type = !ASSOC_ARRAY
;    img = read_matrix7v(filnam,xdim,ydim,zdim,tdim,dxdy,dz,lun,scl,mh,sh)
;    fi.n = fi.nfiles
;    fi.lun(fi.n) = lun
;    mother = fi.nfiles
;end

!GIF: begin
    get_dialog_pickfile,'*.gif',fi.path[0],'Please select file.',filnam,rtn_nfiles,rtn_path
    if filnam eq 'GOBACK' or filnam eq 'EXIT' then return,!ERROR
    fi.path = rtn_path
    read_gif,filnam,jmg
    img = float(reverse(reverse(jmg,1),2))
    num_type = !SHORT
    type = !ASSOC_ARRAY
    get_lun,lun
    xdim = n_elements(img[*,0,0])
    ydim = n_elements(img[0,*,0])
    zdim = 1
    tdim = 1
    dxdy = 1.
    dz = 1.
    scl = 1.
    fi.n = fi.nfiles
    fi.lun(fi.n) = lun
    mother = fi.nfiles
end

!TIFF: begin
    get_dialog_pickfile,'*.tif*',fi.path[0],'Please select file.',filnam,rtn_nfiles,rtn_path
    if filnam eq 'GOBACK' or filnam eq 'EXIT' then return,!ERROR
    fi.path = rtn_path
    img = read_tiff(filnam,r,g,b)
    num_type = !SHORT
    type = !ASSOC_ARRAY
    get_lun,lun
    xdim = n_elements(img[*,0,0])
    ydim = n_elements(img[0,*,0])
    zdim = 1
    tdim = 1
    dxdy = 1.
    dz = 1.
    scl = 1.
    fi.n = fi.nfiles
    fi.lun(fi.n) = lun
    mother = fi.nfiles
end
!XWD: begin
    get_dialog_pickfile,'*.xwd',fi.path[0],'Please select file.',filnam,rtn_nfiles,rtn_path
    if filnam eq 'GOBACK' or filnam eq 'EXIT' then return,!ERROR
    fi.path = rtn_path
    jmg = read_xwd(filnam,r,g,b)
    img = float(reverse(reverse(jmg,1),2))
    num_type = !FLOAT
    type = !FLOAT_ARRAY
    get_lun,lun
    xdim = n_elements(img[*,0,0])
    ydim = n_elements(img[0,*,0])
    zdim = 1
    tdim = 1
    dxdy = 1.
    dz = 1.
    scl = 1.
    fi.n = fi.nfiles
    fi.lun(fi.n) = lun
    mother = fi.nfiles
    tvlct,r,g,b
    lct = intarr(256,3)
    lct[*,0] = r
    lct[*,1] = g
    lct[*,2] = b
    if(ptr_valid(fi.color_lct1[fi.n])) then $
        ptr_free,fi.color_lct1[fi.n]
    fi.color_lct1[fi.n] = ptr_new(lct)
    fi.color_scale1[fi.n] = !NO_SCALE
end

else: begin
 	stat=widget_message('Unknown file type.',/ERROR)
        print,'Unknown file type.'
        st.error_msg = string('Unknown file type.')
        st.error = !TRUE
	return,!ERROR
end

endcase





if not keyword_set(DONT_PUT_IMAGE) then begin
    if loaded eq !FALSE then begin
        if n_elements(ifh) gt 0 then begin
            if ptr_valid(ifh.region_names) then type = !LOGICAL_ARRAY

            ;put_image,img,fi,wd,dsp,filnam,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,mother,paradigm,IFH=ifh, $
            ;    FILE_TYPE=file_type,SMOOTHNESS=smoothness,MODEL=model,UVALUE=uvalue
            ;START150731
            put_image,img,fi,wd,dsp,filnam,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,mother,IFH=ifh, $
                FILE_TYPE=file_type,SMOOTHNESS=smoothness,MODEL=model,UVALUE=uvalue

        endif else begin

            ;put_image,img,fi,wd,dsp,filnam,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,mother,paradigm,FILE_TYPE=file_type, $
            ;    SMOOTHNESS=smoothness,MODEL=model,UVALUE=uvalue
            ;START150731
            put_image,img,fi,wd,dsp,filnam,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,mother,FILE_TYPE=file_type, $
                SMOOTHNESS=smoothness,MODEL=model,UVALUE=uvalue

        endelse
    endif 
    fi.whereisit[fi.n] = !ON_DISK
    if not keyword_set(NOLOAD_COLOR) then begin
        fi.color_scale1[fi.n] = !GRAY_SCALE
        fi.color_scale2[fi.n] = !NO_SCALE
        load_colortable,fi,dsp,TABLE1=fi.color_scale1[fi.n],TABLE2=fi.color_scale2[fi.n]
    endif
endif



;print,'load_data bottom'
return,!OK
end
