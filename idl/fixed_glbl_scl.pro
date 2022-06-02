;Copyright 12/31/99 Washington University.  All Rights Reserved.
;fixed_glbl_scl.pro  $Revision: 12.83 $

;*********************************
pro fixed_glbl_scl,dsp,help,fi,stc
;*********************************

    if(help.enable eq !TRUE) then begin
        value='Images are scaled to (global_max - global_min)/NUMBER_OF_COLORS where max and min are specified by the user.  Overflows and underflows are set to 0 and NUMBER_OF_COLORS respectively'
        widget_control,help.id,SET_VALUE=value
    endif
    dsp[fi.cw].scale = !FIXED
    max = 0.
    min = 1e20
    for pln=0,dsp[fi.cw].zdim-1 do begin
        img = get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + pln,fi,stc)
        max1 = max(img)
  	if(max1 gt max) then max = max1
        min1 = min(img)
  	if(min1 lt max) then min = min1
    end
    label = strarr(2)
    label(0) = string('Symmetric limits')
    label(1) = string('Use global min and max')
    title = string('Set symmetric limits (min = -max)?')
    lcminmax = get_boolean(title,label)
    if(lcminmax eq !TRUE) then begin
        if(abs(max) gt abs(min)) then begin
    	    dsp[fi.cw].fixed_min = -abs(max)
            dsp[fi.cw].fixed_max =  abs(max)
        endif else begin
	    dsp[fi.cw].fixed_min = -abs(min)
            dsp[fi.cw].fixed_max =  abs(min)
        endelse
    endif

return
end
