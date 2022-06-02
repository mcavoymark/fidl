;Copyright 12/31/99 Washington University.  All Rights Reserved.
;smobutter.pro  $Revision: 12.88 $

;************************************
pro smobutter,fi,dsp,wd,help,stc,pref
;************************************

btype = get_button(['lowpass','bandpass','highpass','bandstop'],TITLE='Please select the filter type.')
if (btype eq 0) or (btype eq 2) then begin
    Wn = fltarr(1) 
    value = strarr(3)
    label = strarr(3)
    value[0] = string('5')
    label[0] = string("Order of butterworth filter: ")
    value[1] = string('0.5')
    label[1] = string("Cutoff (0.0 < Wn < 1.0, 1.0 corresponds to half the sampling frequency): ")
    value[2] = '4'
    label[2] = 'Number of frames to skip: '
    dims = get_str(3,label,value)
    order = float(dims[0])
    Wn[0] = float(dims[1])
    delay = float(dims[2])
endif else begin
    Wn = fltarr(2)
    value = strarr(4)
    label = strarr(4)
    value[0] = string('5')
    label[0] = string("Order of butterworth filter: ")
    value[1] = string('0.2')
    label[1] = string("Lower Cutoff (0.0 < Wn < 1.0, 1.0 corresponds to half the sampling frequency): ")
    value[2] = string('0.8')
    label[2] = string("Upper Cutoff (0.0 < Wn < 1.0, 1.0 corresponds to half the sampling frequency): ")
    value[3] = '4'
    label[3] = 'Number of frames to skip: '
    dims = get_str(4,label,value)
    order = float(dims[0])
    Wn[0] = float(dims[1])
    Wn[1] = float(dims[2])
    delay = float(dims[3])
endelse

widget_control,/HOURGLASS
hdr = *fi.hdr_ptr(fi.n)

tdim = hdr.tdim - delay

H = butter_smoth(order,Wn,tdim,btype)


widget_control,/HOURGLASS

series = fltarr(tdim)
image = fltarr(hdr.xdim,hdr.ydim,tdim)
filtered = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,tdim)
for z=0,hdr.zdim-1 do begin
    widget_control,wd.error,SET_VALUE=string('Slice: ',z+1)
    t = 0
    for t1=delay,hdr.tdim-1 do begin
        image[*,*,t] = get_image(z+t1*hdr.zdim,fi,stc)
        ;image[*,*,t] = get_image(z+t1*hdr.zdim,fi,stc,/SUBTRACT_TREND)
	t = t + 1
    endfor
    for x=0,hdr.xdim-1 do begin
        for y=0,hdr.ydim-1 do begin
	    series[0:tdim-1] = image[x,y,*]
	    mean = total(series)/tdim
	    series = series - mean 
	    ftfil = fft([series,fltarr(tdim)])*H
 	    result = fft(ftfil,/INVERSE)
	    filtered[x,y,z,0:tdim-1] = float(result[0:tdim-1]) + mean
        endfor
    endfor
endfor

case btype of
    0: btype_str = 'butlow'
    1: btype_str = 'butbband'
    2: btype_str = 'buthigh'
    3: btype_str = 'butstop'
    else: print,'Something wrong here.'
endcase
if (btype eq 0) or (btype eq 2) then $
    filnam_4dfp = fi.path+fi.tail+'_'+btype_str+'_N'+dims[0]+'_Wn'+dims[1]+'.4dfp.img' $
else $
    filnam_4dfp = fi.path+fi.tail+'_'+btype_str+'_N'+dims[0]+'_Wn'+dims[1]+'to'+dims[2]+'.4dfp.img'
stat = write_4dfp(filnam_4dfp,fi,wd,dsp,help,stc,tdim,IMAGE=filtered)

stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,FILENAME=filnam_4dfp)
if stat eq !OK then begin
    dsp_image = update_image(fi,dsp,wd,stc,pref)
endif


widget_control,HOURGLASS=0
;beep
return
end
