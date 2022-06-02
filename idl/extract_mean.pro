;Copyright 3/31/15 Washington University.  All Rights Reserved.
;extract_mean.pro  $Revision: 1.3 $
pro extract_mean_guts,fi,wd,dsp,glm,tail,hdr
    grand_mean = *glm.grand_mean
    name =tail+'_mean'
    if hdr.ifh.glm_boldtype ne 'cifti' then begin

        ;put_image,grand_mean,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother, $
        ;    -1,MODEL=hdr.mother,IFH=hdr.ifh,/DONTCHANGEIFH
        ;START150731
        put_image,grand_mean,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother, $
            MODEL=hdr.mother,IFH=hdr.ifh,/DONTCHANGEIFH

    endif else begin     
        zdim=long(ceil(double(hdr.ifh.glm_xdim)/double(hdr.xdim*hdr.ydim)))
        gm = fltarr(hdr.xdim*hdr.ydim*zdim)
        gm[0:hdr.ifh.glm_xdim-1]=grand_mean
        gm=reform(gm,hdr.xdim,hdr.ydim,zdim,/OVERWRITE)

        ;put_image,gm,fi,wd,dsp,name,hdr.xdim,hdr.ydim,zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother, $
        ;    -1,MODEL=hdr.mother,IFH=hdr.ifh,/DONTCHANGEIFH
        ;START150731
        put_image,gm,fi,wd,dsp,name,hdr.xdim,hdr.ydim,zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother, $
            MODEL=hdr.mother,IFH=hdr.ifh,/DONTCHANGEIFH

    endelse
end
pro extract_mean,fi,dsp,wd,glm,help,stc,pref
    if fi.nfiles eq 0 then wd1_load_linmod,fi,dsp,wd,glm,help
    if fi.nfiles eq 0 then return
    hdr = *fi.hdr_ptr[fi.n]
    if hdr.array_type eq !LINEAR_MODEL then begin
        glm0=glm[fi.n]
        tail=fi.tails[fi.n]
    endif else begin
        hd1 = *fi.hdr_ptr[hdr.model]
        if ptr_valid(glm[hd1.model].A) then begin 
            glm0=glm[hdr.model] 
            tail=fi.tails[hdr.model]
        endif else begin
            stat = dialog_message('Linear model is not defined for this file.')
            return
        endelse
    endelse
    if ptr_valid(glm0.grand_mean) then begin
        extract_mean_guts,fi,wd,dsp,glm0,tail,hdr
    endif else begin
        stat = dialog_message('Mean has not been calculated.')
    endelse
end
