;Copyright 10/15/01 Washington University.  All Rights Reserved.
;get_params.pro  $Revision: 1.5 $

;****************************************************
function get_params,fi,dsp,wd,glm,help,stc,pref,index
;****************************************************

hd = *fi.hdr_ptr[index]
hdr = *fi.hdr_ptr[hd.mother]

if hdr.array_type ne !LINEAR_MODEL then begin
    mom = hd.mother
    if hdr.array_type eq !STITCH_ARRAY then begin
        tdim = stc[hd.mother].tdim_all
        tdim_file = *stc[hd.mother].tdim_file
        nrun = stc[hd.mother].n
        t_to_file = *stc[hd.mother].t_to_file
        tdim_sum = *stc[hd.mother].tdim_sum
    endif else begin
        tdim = hdr.tdim
        tdim_file = hdr.tdim
        nrun = 1
        t_to_file = intarr(tdim)
        tdim_sum = hdr.tdim
    endelse
    xdim = hdr.xdim
    ydim = hdr.ydim
    zdim = hdr.zdim
    dxdy = hdr.dxdy
    dz   = hdr.dz
endif else begin
    mom = fi.nfiles
    data_type = get_button(['Data space','222 space','333 space'],TITLE='Select type of data')
    case data_type of
        0: begin
           xdim = !DATA_X
           ydim = !DATA_Y
           zdim = 16L
           dxdy = 3.75
           dz   = 8.
        end
        1: begin ; 333 space
           xdim = !ATLAS_333_X
           ydim = !ATLAS_333_Y
           zdim = !ATLAS_333_Z
           dxdy = 3.
           dz   = 3.
        end
        2: begin ; 222 space
           xdim = !ATLAS_222_X
           ydim = !ATLAS_222_Y
           zdim = !ATLAS_222_Z
           dxdy = 2.
           dz   = 2.
        end
        else: print,'Invalid value of data_type'
    endcase
    str = get_str(2,['Number of runs: ','Number of frames per run: '],['16','128'])
    nrun = long(str[0])
    tdim_file = lonarr(nrun)
    tdim_file[*] = long(str[1])
    tdim = nrun*tdim_file[0]
    labels = strarr(6+nrun)
    values = strarr(6+nrun)
    labels[0:5] = ['Number of runs','X dimension','Y dimension','Z dimension','In-plane pixel size','Slice thickness']
    values[0:5] = [fix(nrun),fix(xdim),fix(ydim),fix(zdim),dxdy,dz]
    for run=0,nrun-1 do begin
        labels[run+6] = string(run+1,FORMAT='("Length of run #",i2)')
        values[run+6] = tdim_file[run]
    endfor
    str = get_str(nrun+6,labels,values)
    nrun = long(str[0])
    xdim = long(str[1])
    ydim = long(str[2])
    zdim = long(str[3])
    dxdy = float(str[4])
    dz   = float(str[5])
    for run=0,nrun-1 do $
        tdim_file[run] = long(str[run+6])
    tdim = long(total(tdim_file))
    t_to_file = lonarr(tdim)
    tdim_sum = lonarr(nrun)
    t0 = 0
    for ifile=0,nrun-1 do begin
        t_to_file[t0:t0+tdim_file[ifile]-1] = ifile
        if ifile gt 0 then $
            tdim_sum[ifile] = tdim_sum[ifile] + tdim_file[ifile] $
        else $
            tdim_sum[ifile] = tdim_file[ifile]
        t0 = t0 + tdim_file[ifile]
    endfor
endelse
return,params = {name:'',xdim:xdim,ydim:ydim,zdim:zdim,tdim:tdim,dxdy:dxdy, $
    dz:dz,nrun:nrun,tdim_file:tdim_file,t_to_file:t_to_file,tdim_sum:tdim_sum,mom:mom}
end
