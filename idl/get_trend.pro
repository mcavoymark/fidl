;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_trend.pro  $Revision: 12.89 $

;*******************************************
pro get_trend,idx,fi,pr,dsp,wd,stc,help,pref
;*******************************************

common profiles,x1,x2,y1,y2,x1m1,x2m1,y1m1,y2m1,row,col,profdat

common linear_fit,fit_name,slope,intcpt,trend

if(help.enable eq !TRUE) then begin
    widget_control,help.id,SET_VALUE=string('Perform a least square fit to first, third, fifth etc. phases of the activation paradigm.  The results are stored in the file list for viewing and can be optionally used by other processing routines.')
endif

hdr = *fi.hdr_ptr(idx)

if(fi.paradigm_loaded(idx) ne !TRUE) then begin
    stat=widget_message('*** No stimulus time-course defined.  ***',/ERROR)
    return
endif
paradigm = *fi.paradigm[idx]

widget_control,/HOURGLASS

if hdr.array_type eq !STITCH_ARRAY then begin
    n_slope = stc[hdr.mother].n
    tdim_sum = *stc[hdr.mother].tdim_sum
    tdim_file = *stc[hdr.mother].tdim_file

    slope = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,nslope)
    intcpt = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,nslope)
    t_rest1 = intarr(nslope)
    for file=0,nslope-1 do begin
        t0 = tdim_sum[file]
        t1 = t0 + tdim_file[file] - 1
        fit_trend,hdr.mother,t0,t1,wd,fi,stc,hdr,paradigm,slp,int,t_rest1_tmp
        t_rest1[file] = t_rest1_tmp
        slope[*,*,*,file] = slp
        if file eq 0 then begin
            intcpt[*,*,*,file] = int
            x0 = int + t_rest1[0]*slope[*,*,*,0]
        endif else begin
            intcpt[*,*,*,file] = x0 - int - t_rest1[file]*slope[*,*,*,file]
        endelse
    endfor
    xx = {name:'',tdim_all:hdr.tdim}
endif else begin
    n_slope = 1
    fit_trend,hdr.mother,0,hdr.tdim-1,wd,fi,stc,hdr,paradigm,slope,intcpt,t_rest1
    xx = {name:'',tdim_all:hdr.tdim}
endelse
if(n_elements(slope) eq 1) then return

if hdr.array_type eq !STITCH_ARRAY then begin 
    trend.t_rest1[hdr.mother,0:nslope-1] = t_rest1
endif else begin
    trend.t_rest1(hdr.mother,0) = t_rest1
endelse
if(ptr_valid(trend.slope_ptr(hdr.mother))) then $
    ptr_free,trend.slope_ptr(hdr.mother)
if(ptr_valid(trend.intcpt_ptr(hdr.mother))) then $
    ptr_free,trend.intcpt_ptr(hdr.mother)
if(ptr_valid(trend.hdr_ptr(hdr.mother))) then $
    ptr_free,trend.hdr_ptr(hdr.mother)
trend.slope_ptr(hdr.mother) = ptr_new(slope)
trend.intcpt_ptr(hdr.mother) = ptr_new(intcpt)
trend.hdr_ptr(hdr.mother) = ptr_new(xx)

if(pref.slope eq 1) then begin
    fit_name = fi.tails(hdr.mother)
    filnum = hdr.mother
    slope = reform(slope,hdr.xdim,hdr.ydim,hdr.zdim*n_slope)
    name = strcompress(string(fit_name,FORMAT='(a,"_slope")'),/REMOVE_ALL)

    ;put_image,slope,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,n_slope, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,filnum,paradigm,IFH=hdr.ifh
    ;START150731
    put_image,slope,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,n_slope, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,filnum,IFH=hdr.ifh

endif

if(pref.intercept eq 1) then begin
    intcpt = reform(intcpt,hdr.xdim,hdr.ydim,hdr.zdim*n_slope)
name = strcompress(string(fit_name,FORMAT='(a,"_intcpt")'),/REMOVE_ALL)

    ;put_image,intcpt,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,n_slope, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,filnum,paradigm,IFH=hdr.ifh
    ;START150731
    put_image,intcpt,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,n_slope, !FLOAT,hdr.dxdy,hdr.dz,hdr.scl, !FLOAT_ARRAY,filnum,IFH=hdr.ifh

endif

widget_control,HOURGLASS=0

return
end
