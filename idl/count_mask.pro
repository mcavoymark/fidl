;Copyright 12/31/99 Washington University.  All Rights Reserved.
;count_mask.pro  $Revision: 12.88 $

;********************************
pro count_mask,fi,wd,dsp,help,stc
;********************************

; Compute statistic for labeled regions in a mask

hdr = *fi.hdr_ptr(fi.n)
hdr_data = *fi.hdr_ptr(hdr.mother)
if(hdr.array_type ne !LOGICAL_ARRAY) then begin
    stat = widget_message('Image must be a mask.')
    return
endif
print,'Summing regions over ',fi.tails(hdr.mother)

reg_mask = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
for z=0,hdr.zdim-1 do begin
    reg_mask(*,*,z) = get_image(z,fi,stc,FILNUM=fi.n+1)
endfor
test_mask = reg_mask gt 1
non_empty_list = intarr(hdr.zdim)
num_non_empty = 0
for z=0,hdr.zdim-1 do begin
    if(total(test_mask[*,*,z]) gt 0) then begin
        non_empty_list[num_non_empty] = z
        num_non_empty = num_non_empty + 1
    endif
endfor
mask1 = fltarr(hdr.xdim,hdr.ydim,num_non_empty)
for z=0,num_non_empty-1 do begin
    mask1[*,*,z] = reg_mask[*,*,non_empty_list[z]]
endfor
reg_mask = mask1

max_reg = max(reg_mask) - 2
if(max_reg lt 0) then begin
    print,'No regions specified'
    return
endif
if(ptr_valid(hdr.ifh.region_names)) then begin
    region_names = *hdr.ifh.region_names
endif else begin
    print,'No regions specified'
    return
endelse

labels = ['Supress region labels','Suppress frame labels','Columns=regions','Select output file name','Append new column to output file','Append new row to output file','Append tag to region name']

;options = get_bool_list(labels,TITLE='Select output options')
;START1
rtn = get_bool_list(labels,TITLE='Select output options')
options = rtn.list

if(options[3] ne 0) then begin
    file = dialog_pickfile(/READ,FILTER='*.txt',PATH=fi.path(0),/NOCONFIRM,FILE='regions.txt')
    if(n_elements(file) gt 1) then $
        filnam = file(0)+file(1) $
    else $
        filnam = file
    dot = rstrpos(filnam_stats,'.')
    if(dot gt 0) then $
        stem = strmid(filnam_stats,0,dot) $
    else $
        stem = filnam_stats
    filnam_stats = strcompress(string(stem,"_stats.txt"),/REMOVE_ALL)
endif else begin
    filnam = 'regional_mean.txt'
    filnam_stats = 'regional_stats.txt'
endelse
if(options[4] ne 0) then begin ; Append new data as a column.
    options[1] = 1 ; Force output in column format.
    options[2] = 1 ; Suppress frame labels.
    options[6] = 1 ; Append tag to region name
    filnam_paste = filnam
    filnam = "tmp_extract.txt"
endif
if(options[5] ne 0) then begin ; Append new data as a column.
    options[1] = 1 ; Suppress frame labels.
    options[6] = 1 ; Append tag to region name.
endif
if(options[6] ne 0) then begin
    tag1 = get_str(1,'Tag to append to region name:','')
    tag = string(tag1[0],FORMAT='("_",a)')
endif else begin
    tag = ''
endelse

image = fltarr(hdr.xdim,hdr.ydim,num_non_empty)
tdimlab = strarr(hdr_data.tdim)
means = fltarr(hdr_data.tdim,max_reg+1)
get_lun,lu
openw,lu,filnam_stats
for t=0,hdr_data.tdim-1 do begin
    widget_control,wd.error,SET_VALUE=string('Frame: ',t+1)
    tdimlab[t] = string(t,FORMAT='(i4)')
    for z=0,num_non_empty-1 do begin
        zz = non_empty_list[z]
        image[*,*,z] = get_image(zz+t*hdr.zdim,fi,stc,FILNUM=hdr.mother+1)
    endfor
    for reg=0,max_reg do begin
        voxels = where(reg_mask eq (reg+2),nvox)
        mimg = image[voxels]
        min = min(mimg,MAX=max)
        sum = total(mimg)
        sumsq = total(mimg^2)
        mean = sum/nvox
        if(hdr_data.array_type eq !VARIANCE) then $
            mean = mean/sqrt(nvox)  ; Assume std deviation being averaged.
        var = (nvox*sumsq - sum^2)/(nvox*(nvox-1))
        means[t,reg] = mean
        line = strcompress(string(t,region_names[reg],mean,sqrt(var),min,max,nvox, $
        FORMAT='("Frame:",i4," Region: ",a," Mean: ",f10.4," Std. Dev.: ",f10.4," Minimum: ",f10.4," Maximum: ",f10.4," No. voxels:",i6)'))
        print,line
        printf,lu,line
    endfor
endfor
close,lu

if((hdr.tdim eq 1) and (max_reg eq 0)) then begin
    widget_control,wd.error,SET_VALUE=strcompress(string(region_names[0],mean,sqrt(var),nvox, $
    FORMAT='("Region: ",a," Mean: ",f10.4," SD: ",f10.4," Voxels:",i6)'))
endif

if(options[2] eq 0) then begin
;   Columns represent time.
    if(options[5] ne 0) then begin
;       Append new regions to the end of the output file.
        openw,lu,filnam,/APPEND
    endif else begin
        openw,lu,filnam
    endelse
    if(options[1] eq 0) then begin
;       Write frame labels.
        if(options[0] eq 0) then begin
;           Also write region labels.
            printf,lu,'Region',FORMAT='(" ",a," ",$)'
        endif
        printf,lu,tdimlab[0],FORMAT='(a,$)'
        for t=1,hdr_data.tdim-1 do $
            printf,lu,tdimlab[t],FORMAT='(" ",a,$)'
        printf,lu,''
    endif
    for reg=0,max_reg do begin
        if(options[0] eq 0) then $
            printf,lu,region_names[reg],tag,FORMAT='(a,a," ",$)' $
        else $
            printf,lu,' ',FORMAT='(a,$)'
        for t=0,hdr_data.tdim-1 do $
            printf,lu,means[t,reg],FORMAT='(f," ",$)'
        printf,lu,''
    endfor
endif else begin
;   Columns represent regions.
    openw,lu,filnam
    if(options[0] eq 0) then begin
;       Write region labels.
        if(options[1] eq 0) then begin
;           Also write frame labels.
            printf,lu,' Frame',FORMAT='(a," ",$)'
        endif
        printf,lu,region_names[0],tag,FORMAT='(a,a,$)'
        for reg=1,max_reg do $
            printf,lu,region_names[reg],FORMAT='(" ",a,$)'
        printf,lu,''
    endif
    for t=0,hdr_data.tdim-1 do begin
        if(options[1] eq 0) then $
            printf,lu,tdimlab[t],FORMAT='(a," ",$)' $
        else $
            printf,lu,' ',FORMAT='(a,$)'
        for reg=0,max_reg do $
            printf,lu,means[t,reg],FORMAT='(f," ",$)'
        printf,lu,''
    endfor
endelse
close,lu

if(options[4] ne 0) then begin ; Append new data as a column.
    openw,lu,"tmp_extract.csh"
    printf,lu,'#!/bin/csh'
    printf,lu,'paste ',filnam_paste,' tmp_extract.txt > tmp1_extract.txt
    printf,lu,'mv tmp1_extract.txt ',filnam_paste
    printf,lu,'echo Values pasted to ',filnam_paste
    printf,lu,'/bin/rm tmp_extract.txt tmp_extract.csh'
    close,lu
    spawn,'pwd',pwd
    cmd = string(pwd,FORMAT='("csh ",a,"/tmp_extract.csh")')
    spawn,cmd
endif

print,'Mean values written to ',filnam
print,'All statistics written to ',filnam_stats
free_lun,lu

return
end
