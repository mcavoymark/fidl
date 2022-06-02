;Copyright 12/23/10 Washington University.  All Rights Reserved.
;fidl_r_to_z.pro  $Revision: 1.7 $
pro fidl_r_to_z,fi,wd,dsp,help,pref,dsp_image,stc,CVERSION=cversion
if not keyword_set(CVERSION) then cversion=0
goback0:
if cversion eq 0 then begin
    if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx,'r map to convert to zstat') eq !ERROR then return
    nsub=hdr.ifh.nsub
    if(idx lt fi.nfiles) and (fi.whereisit[idx] eq !INTERNAL) then begin
        files = get_filename_from_index(fi,idx)
    endif else begin
        files = fi.names[idx]
    endelse
    nfiles = 1
endif else begin
    rtn_get_glms=get_glms(pref,fi,GET_THIS='imgs')
    if rtn_get_glms.msg eq 'GO_BACK' or rtn_get_glms.msg eq 'EXIT' then return
    nfiles = rtn_get_glms.total_nsubjects
    files = rtn_get_glms.imgselect
    nsub = rtn_get_glms.ifh[*].nsub
endelse
scrap = strtrim(nsub,2)
repeat begin
    scrap = get_str(nfiles,files,scrap,TITLE='Number of subjects or frames that went into each map.', $
        LABEL='Each map needs a minimum of 3.',/ONE_COLUMN,/BELOW,/GO_BACK,FRONT=3)
    if scrap[0] eq 'GO_BACK' then goto,goback0
    index = where(fix(scrap) lt 3,count)
endrep until count eq 0 
nsub = fix(scrap)
if cversion eq 0 then begin
    hdr.ifh.nsub = nsub[0]
endif
if cversion eq 0 then begin
    z_image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim)
    passtype = bytarr(4)
    passtype[2:3] = 1
    timg = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
    zimg = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
    vol = long(hdr.xdim)*long(hdr.ydim)*long(hdr.zdim)
    widget_control,/HOURGLASS
    for t=0,hdr.tdim-1 do begin
        widget_control,wd.error,SET_VALUE=strcompress(string(t+1,FORMAT='("Processing frame #",i4)'))
        for z=0,hdr.zdim-1 do timg[*,*,z] = get_image(z+t*hdr.zdim,fi,stc,FILNUM=idx+1)
        stat=call_external(!SHARE_LIB,'_r_to_z',timg,zimg,vol,hdr.ifh.nsub,VALUE=passtype)
        z_image[*,*,*,t] = zimg
    endfor
    name = fi.tails[idx] + '_zstat'

    ;paradigm= -1.
    ;put_image,reform(z_image,hdr.xdim,hdr.ydim,hdr.zdim*hdr.tdim),fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim, $
    ;    hdr.tdim,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother,paradigm,IFH=hdr.ifh
    ;START150731
    put_image,reform(z_image,hdr.xdim,hdr.ydim,hdr.zdim*hdr.tdim),fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim, $
        hdr.tdim,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother,IFH=hdr.ifh

endif else begin
    csh = 'fidl_r_to_z.csh'
    openw,lu,csh,/GET_LUN
    top_of_script,lu
    nsub = strtrim(nsub,2)
    for i=0,nfiles-1 do printf,lu,'nice +19 $BIN'+'/fidl_r_to_z -r '+files[i]+' -nsub '+nsub[i]
    close,lu
    free_lun,lu
    spawn,'chmod +x '+csh
    spawn_cover,csh,fi,wd,dsp,help,stc
endelse
print,'DONE'
end
