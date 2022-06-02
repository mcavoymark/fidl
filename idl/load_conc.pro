;Copyright 12/31/99 Washington University.  All Rights Reserved.
;load_conc.pro $Revision: 12.133 $

;function load_conc,fi,stc,dsp,help,wd,filename,nfiles0,files0,DONT_PUT_IMAGE=dont_put_image,NOLOAD_COLOR=noload_color
;START170216
function load_conc,fi,stc,dsp,help,wd,filename,nfiles0,files0,DONT_PUT_IMAGE=dont_put_image,NOLOAD_COLOR=noload_color,SKIP=skip

if n_elements(nfiles0) eq 0 then nfiles0=0
goback0:
nt4s=0
t4s=''
lc0=0
lcnii=0
if not keyword_set(DONT_PUT_IMAGE) then dont_put_image=0

;START170216
if not keyword_set(SKIP) then skip=0

for i=0,n_elements(filename)-1 do begin
    lizard = strmid(filename[i],strlen(filename[i])-3)
    if lizard eq 'img' or lizard eq 'nii' or lizard eq '.gz' then begin
        gf=get_filetype(filename[i])
        if gf.msg ne 'OK' then return,rtn={msg:gf.msg} 
        nfiles=1
        files = filename[i]
        filetype=gf.filetype
        identify = ''
        lcnii=gf.lcnii
    endif else if nfiles0 ne 0 then begin
        nfiles=nfiles0
        files=files0
        gf=get_filetype(files)
        if gf.msg ne 'OK' then return,rtn={msg:gf.msg} 
        filetype=gf.filetype
        identify = ''
        lcnii=gf.lcnii
    endif else begin

        ;rl=read_list(filename[i])
        ;START170216
        rl=read_list(filename[i],SKIP=skip)

        if rl.msg ne 'OK' then return,rtn={msg:rl.msg}
        files=''
        filetype=''
        if rl.nimg ne 0 then begin
            files=[files,rl.img]
            filetype=[filetype,rl.imgt]
        endif
        if rl.ndtseries ne 0 then begin
            files=[files,rl.dtseries]
            filetype=[filetype,rl.dtseriest]
        endif
        if rl.ndscalar ne 0 then begin
            files=[files,rl.dscalar]
            filetype=[filetype,rl.dscalart]
        endif
        if rl.nnii ne 0 then begin
            files=[files,rl.nii]
            filetype=[filetype,rl.niit]
        endif

        ;START210127
        if rl.nwmparc ne 0 then begin
            files=[files,rl.wmparc]
            filetype=[filetype,rl.wmparct]
        endif

        lcnii=rl.lcnii
        nfiles=n_elements(files)-1
        if nfiles gt 0 then begin
            files=files[1:*]
            filetype=filetype[1:*]
        endif
        nt4s=rl.nt4
        t4s=rl.t4
        identify=rl.identify
    endelse
    ls=load_stitch(fi,stc,help,nfiles,files,filename[i],dont_put_image,identify,filetype)


    ;if ls.msg ne 'OK' then return,rtn={msg:ls.msg}
    ;ls.ifh.data_file = filename[i]
    ;if dont_put_image eq 0 then begin
    ;    if filetype[0] eq 'dtseries' or filetype[0] eq 'dscalar' then begin
    ;        xdim=ceil(sqrt(ls.ifh.matrix_size_1))
    ;        ydim=xdim
    ;    endif else begin
    ;        xdim=ls.ifh.matrix_size_1
    ;        ydim=ls.ifh.matrix_size_2
    ;    endelse
    ;    put_image,img,fi,wd,dsp,filename[i],xdim,ydim,ls.ifh.matrix_size_3,ls.stc1.tdim_all, $
    ;        ls.num_type,ls.ifh.scale_1,ls.ifh.scale_3,1.,ls.type,fi.nfiles,ls.ifh.bigendian,FILE_TYPE=!MRI_4DFP, $
    ;        IFH=ls.ifh,/DONTCHANGEIFH
    ;    if not keyword_set(NOLOAD_COLOR) then begin 
    ;        fi.color_scale1[fi.n] = !GRAY_SCALE
    ;        fi.color_scale2[fi.n] = !NO_SCALE
    ;        load_colortable,fi,dsp,TABLE1=!GRAY_SCALE,TABLE2=!NO_SCALE
    ;    endif
    ;endif
    ;START160112
    if ls.msg ne 'OK' then begin
        if ls.msg ne 'SKIP' then return,rtn={msg:ls.msg}
    endif else begin
        ls.ifh.data_file = filename[i]
        if dont_put_image eq 0 then begin
            if filetype[0] eq 'dtseries' or filetype[0] eq 'dscalar' then begin
                xdim=ceil(sqrt(ls.ifh.matrix_size_1))
                ydim=xdim
            endif else begin
                xdim=ls.ifh.matrix_size_1
                ydim=ls.ifh.matrix_size_2
            endelse
            put_image,img,fi,wd,dsp,filename[i],xdim,ydim,ls.ifh.matrix_size_3,ls.stc1.tdim_all, $
                ls.num_type,ls.ifh.scale_1,ls.ifh.scale_3,1.,ls.type,fi.nfiles,ls.ifh.bigendian,FILE_TYPE=!MRI_4DFP, $
                IFH=ls.ifh,/DONTCHANGEIFH
            if not keyword_set(NOLOAD_COLOR) then begin
                fi.color_scale1[fi.n] = !GRAY_SCALE
                fi.color_scale2[fi.n] = !NO_SCALE
                load_colortable,fi,dsp,TABLE1=!GRAY_SCALE,TABLE2=!NO_SCALE
            endif
        endif
    endelse


endfor
return,rtn={msg:'OK',nt4s:nt4s,t4s:t4s,stc1:ls.stc1,ifh:ls.ifh,lcnii:lcnii}
end
