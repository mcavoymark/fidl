;Copyright 2/10/01 Washington University.  All Rights Reserved.
;get_regional_avg.pro  $Revision: 1.7 $

;*****************************************************
function get_regional_avg,indices,fi,stc,FILNUM=filnum
;*****************************************************
;print,'get_regional_avg top'

if keyword_set(filnum) then begin
    filnum = filnum - 1
endif else begin
    filnum = fi.n
endelse

hdr = *fi.hdr_ptr[filnum]
idx = indices[sort(indices)] ; Arrange in ascending order.
nidx = n_elements(idx)
xdim = long(hdr.xdim)
ydim = long(hdr.ydim)
zdim = long(hdr.zdim)
xydim = xdim*ydim
xyzdim = xydim*zdim
sum = 0.

if hdr.array_type eq !STITCH_ARRAY then begin
    t_to_file = *stc[filnum].t_to_file
    tdim_sum = *stc[filnum].tdim_sum
    hdr_ptr = *stc[filnum].hdr_ptr
    filnam = *stc[filnum].filnam

    t_total = idx/xyzdim        ;Overall time indices.
    ifile = t_to_file[t_total]
    tfile = t_total - tdim_sum[ifile]
    zfile = idx/xydim - tdim_sum[ifile]*zdim ; Slice position within file.
    vox = idx - tfile*xyzdim
    z = vox/xydim
    pix = vox - z*xydim
    y = pix/xdim
    x = pix - y*xdim 
    ifilem1 = -1
    get_lun,lun
    z = idx/(xydim)
    files = ifile[uniq(ifile)]
    nfiles = n_elements(files)
    for jfile=0,nfiles-1 do begin

        ;hdr = hdr_ptr[ifile[jfile]]
        hdr = *hdr_ptr[jfile]

        xxx = strpos(filnam[ifile[jfile]],!SLASH)
        if(xxx lt 0) then begin
            path = fi.paths(filnum)
            file_name = path + !SLASH + filnam[ifile[jfile]]
        endif else begin
            file_name = filnam[ifile[jfile]]
        endelse
        if(ifilem1 ne -1) then $
            close,lun
        openr,lun,file_name
        if(hdr.type eq !FLOAT) then $
            img = assoc(lun,fltarr(hdr.xdim,hdr.ydim,/NOZERO),0) $
        else $
            img = assoc(lun,dblarr(hdr.xdim,hdr.ydim,/NOZERO),0)
        idf = where(ifile eq files[jfile])
        x1 = x[idf]
        y1 = y[idf]
        z1 = zfile[idf]
        slices = z1[uniq(z1)]
        nslc = n_elements(slices)
        for islc=0,nslc-1 do begin
            idz = where(z1 eq slices[islc])
            x2 = x1[idz]
            y2 = y1[idz]
            z2 = z1[idz]
            sum = sum + hdr.scl*total(img[x2,y2,z2[0]])
        endfor
    endfor
    close,lun
    free_lun,lun
endif else begin
    ;if(not ptr_valid(fi.data_ptr[filnum])) then return,-1
    ;img = *fi.data_ptr[filnum]
    ;z = idx/(xydim)
    ;idx_slc = idx - z*(xydim)
    ;slices = z[uniq(z)]
    ;nslc = n_elements(slices)
    ;for islc =0,nslc-1 do begin
    ;    idxtmp = idx_slc[where(z eq slices[islc])]
    ;    y = idxtmp/xdim
    ;    x = idxtmp - y*xdim 
    ;    sum = sum + hdr.scl*total(img[x,y,slices[islc]])
    ;endfor

    if(not ptr_valid(fi.data_ptr[filnum])) then return,-1
    img = *fi.data_ptr[filnum]
    z = idx/(xydim)
    idx_slc = idx - z*(xydim)
    slices = z[uniq(z)]
    nslc = n_elements(slices)
    for islc =0,nslc-1 do begin
        idxtmp = idx_slc[where(z eq slices[islc],count)]
        y = idxtmp/xdim
        x = idxtmp - y*xdim
        voxel_sum = 0.
        for i=0,count-1 do voxel_sum = voxel_sum + img[x[i],y[i],slices[islc]]
        sum = sum + hdr.scl*voxel_sum
    endfor
endelse

sz = size(image)
if(sz[n_elements(sz)-2] eq 6) then begin
;   Complex data.
    image = abs(image)
endif
 
;print,'nidx=',nidx,'  sum=',sum,'  sum/float(nidx)=',sum/float(nidx)

;print,'get_regional_avg bottom'
return,sum/float(nidx)
end
