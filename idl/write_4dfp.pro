;Copyright 12/31/99 Washington University.  All Rights Reserved.
;write_4dfp.pro  $Revision: 12.95 $
function write_4dfp,filnam_4dfp,fi,wd,dsp,help,stc,tdim,IMAGE=image,INDEX=index,IFH=ifh
if not keyword_set(INDEX) then index = fi.n
if keyword_set(image) then begin
    lcread = !FALSE
    sz = size(image)
    xdim = sz[1]
    ydim = sz[2]
    zdim = sz[3]
    if(sz[0] eq 4) then begin
        image = reform(image,xdim,ydim,zdim*tdim)
    endif
endif else begin
    hdr = *fi.hdr_ptr(index)
    if(hdr.array_type eq !FLOAT_ARRAY) then begin
        image = *fi.data_ptr[index]
        lcread = !FALSE
    endif else begin
        lcread = !TRUE
    endelse
    xdim = hdr.xdim
    ydim = hdr.ydim
    zdim = hdr.zdim
endelse

filnam_4dfp = strcompress(filnam_4dfp,/REMOVE_ALL)
if strpos(filnam_4dfp,'.4dfp.img',/REVERSE_SEARCH) eq -1 then filnam_4dfp = filnam_4dfp + '.4dfp.img'


;openw,lun,filnam_4dfp,/GET_LUN
;START160415
repeat begin
    openw,lun,filnam_4dfp,/GET_LUN,ERROR=err
    if err ne 0 then begin
        print,!ERROR_STATE.MSG
        print,'strlen(filnam_4dfp)=',strlen(filnam_4dfp)
        if strlen(filnam_4dfp) gt 255 then begin
            filnam_4dfp=get_str(1,'new filename',filnam_4dfp,TITLE='Error: Filename is too long',$
                LABEL='Please rename file. Filenames are limited to 255 characters.',/RESET,/GO_BACK,/EXIT)
            if filnam_4dfp[0] eq 'EXIT' then return,'EXIT' else if filnam_4dfp[0] eq 'GO_BACK' then return,'GO_BACK'
        endif
    endif
endrep until n_elements(lun) ne 0


if(lcread eq !TRUE) then begin
    for t=0,tdim-1 do begin
        for z=0,zdim-1 do begin
            img = get_image(z+t*zdim,fi,stc,FILNUM=index+1)
            if(!VERSION.OS_FAMILY eq 'Windows') then $
                img = swap_endian(img)
            writeu,lun,float(img)
        endfor
    endfor
endif else begin
    if(!VERSION.OS_FAMILY eq 'Windows') then begin
;       Convert endian and write in single volume blocks.
        for t=0,tdim-1 do begin
            img = image[*,*,t*zdim:t*zdim+zdim-1]
            if(!VERSION.OS_FAMILY eq 'Windows') then $
                img = swap_endian(img)
            writeu,lun,float(img)
        endfor
    endif else begin
;       Write it all in one fell swoop.
        writeu,lun,float(image)
    endelse
endelse
close,lun
free_lun,lun

if((tdim gt 1) and (lcread eq !FALSE)) then $
    image = reform(image,xdim,ydim,zdim,tdim)

dot = rstrpos(filnam_4dfp,'.')
if(dot gt 0) then $
    stem = strmid(filnam_4dfp,0,dot) $
else $
    stem = filnam_4dfp
filnam_ifh = strcompress(string(stem,".ifh"),/REMOVE_ALL)
if(n_elements(filnam_ifh) gt 1) then filnam_ifh = filnam_ifh[0] + filnam_ifh[1]



;if(n_elements(hdr) gt 0) then begin
;    ifh = hdr.ifh
;    ifh.smoothness = hdr.smoothness
;endif else begin
;    ifh = {InterFile_Header}
;endelse

if not keyword_set(IFH) then begin
    if n_elements(hdr) gt 0 then begin
        ifh = hdr.ifh
        ifh.smoothness = hdr.smoothness
    endif else begin
        ifh = {InterFile_Header}
    endelse
endif





ifh.matrix_size_1 = xdim
ifh.matrix_size_2 = ydim
ifh.matrix_size_3 = zdim
ifh.matrix_size_4 = tdim
ifh.mmppix[0] = fi.atlas_pix[0,index]
ifh.mmppix[1] = fi.atlas_pix[1,index]
ifh.mmppix[2] = fi.atlas_pix[2,index]
ifh.center[0] = fi.atlas_off[0,index]
ifh.center[1] = fi.atlas_off[1,index]
ifh.center[2] = fi.atlas_off[2,index]
ifh.bigendian=abs(!SunOS_Linux-1)

if(ptr_valid(fi.paradigm(index))) then begin
    paradigm = *fi.paradigm(index)
endif else begin
    paradigm = fltarr(tdim)
endelse
if(tdim gt 1) then $
    ifh.mri_paradigm_format = encode_paradigm(paradigm,tdim) $
else $
    ifh.mri_paradigm_format = ' '
write_mri_ifh,filnam_ifh,ifh

if(!BATCH eq !FALSE) then $
    widget_control,wd.error,SET_VALUE= string(filnam_4dfp(0), $
                FORMAT='("Image written to ",a)')

;error = !FALSE
;return,error
;START160415
return,'OK'

end
