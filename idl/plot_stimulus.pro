;Copyright 12/31/99 Washington University.  All Rights Reserved.
;plot_stimulus.pro  $Revision: 12.84 $

;***********************
pro plot_stimulus,fi,dsp
;***********************

hd = *fi.hdr_ptr(fi.n)
hdr = *fi.hdr_ptr(hd.mother)
tdim = hdr.tdim
if(tdim le 2) then begin
    stat=widget_message('*** Paradigm can only be defined for time-course data.  ***',/ERROR)
    return
endif
stimulus_base = widget_base(/COLUMN,TITLE=fi.tail)
plot = widget_base(stimulus_base,/COLUMN)
wplot = widget_draw(plot,XSIZE= !PARADIGM_WIDTH,YSIZE= !PARADIGM_HGHT)
widget_control,stimulus_base,/REALIZE
widget_control,GET_VALUE=plot_window,wplot
wset,plot_window
x_axis = findgen(tdim+1) + 1.

if(ptr_valid(fi.paradigm(hd.mother))) then begin
    paradigm = *fi.paradigm(hd.mother)
endif else begin
    paradigm = fltarr(hdr.tdim)
    paradigm[*] = 1
    paradigm[0:4] = 0
endelse
plot,x_axis,paradigm,XMARGIN=5,COLOR=dsp[fi.cw].white,XRANGE=[1,tdim+1], $
    YRANGE=[-2,1],XSTYLE=9,YSTYLE=20,PSYM=0,SYMSIZE=2,XTITLE='Frame', $
    POSITION=[ !PARADIGM_OFFSET, !PARADIGM_Y0, $
    !PARADIGM_WIDTH- !PARADIGM_OFFSET, !PARADIGM_Y1],/DEVICE
wset,dsp[fi.cw].image_index[fi.cw]

spm_type = intarr(tdim)
spm_len = intarr(tdim)
run = 1
type = 1
last_stim = paradigm(1)
j = 0
for i=1,tdim-1 do begin
    if(paradigm(i) ne last_stim) then begin
 	case paradigm(i-1) of
	    0: type = 0
	    -1: type = 1
	    1: type = 2
	    else: print,'Invalid paradigm code'
	endcase
        if(type ne 0) then begin
            spm_type(j) = type
            spm_len(j) = run
	    j = j + 1
	endif
        last_stim = paradigm(i)
	run = 1
    endif else begin
        run = run + 1
    endelse
endfor
case paradigm(i-1) of
    0: type = 0
    -1: type = 1
    1: type = 2
    else: print,'Invalid type'
endcase
if(type ne 0) then begin
    spm_type(j) = type
    spm_len(j) = run
endif

print,"SPM type: [ ",FORMAT='(a,$)'
for i=0,j do $
    print,spm_type(i),FORMAT='(i2,$)'
print," ]"
print,"SPM len: [ ",FORMAT='(a,$)'
for i=0,j do $
    print,spm_len(i),FORMAT='(i3,$)'
print," ]"
        

return
end
