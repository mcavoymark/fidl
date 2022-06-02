;Copyright 12/31/99 Washington University.  All Rights Reserved.
;print_all.pro  $Revision: 12.86 $

;*******************
pro print_all,dsp,fi
;*******************

hdr =*fi.hdr_ptr(fi.n)
img = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
dezoom = 1
case hdr.xdim of 
    256: begin
	labels = strarr(2)
	labels(0) = 'Fast (relatively) printing'
	labels(1) = 'Better quality'
	type = get_button(labels,TITLE='Speed or quality?')
        if(type eq 0) then $
            dezoom = 2 $
	else $
            dezoom = 1
        if(hdr.zdim eq 17) then begin
            ncol = 5 
            img = fltarr(1280/dezoom,1024/dezoom)
        endif else begin
            ncol = 4
            img = fltarr(1024/dezoom,1024/dezoom)
        endelse
    end
    64: begin
        if(hdr.zdim eq 17) then begin
            ncol = 5 
            img = fltarr(320,256)
        endif else begin
            ncol = 4
            img = fltarr(256,256)
        endelse
    end
    48: begin
	case hdr.zdim of
	    17: begin
                ncol = 6 
                img = fltarr(288,192)
	    end
	    16: begin
                ncol = 8
                img = fltarr(384,128)
	    end
	    48: begin
                ncol = 8
                img = fltarr(384,384)
	    end
	    else: begin
        	stat=widget_message('Cannot format these image dimensions.',/ERROR)
		set_plot,'X'
        	return
	    end
	endcase
    end
    else: begin
        stat=widget_message('Cannot format these image dimensions.',/ERROR)
	set_plot,'X'
        return
    end
endcase
if(dezoom ne 1) then begin
    hdr.xdim = hdr.xdim/dezoom
    hdr.ydim = hdr.ydim/dezoom
endif
zoff = hdr.zdim*(dsp[fi.cw].frame-1)
for z=0,hdr.zdim-1 do begin
    image = get_image(z+zoff,fi,stc)
    if(dezoom ne 1) then $
        image = rebin(image,hdr.xdim,hdr.ydim)
    row = hdr.ydim*(z/ncol)
    col = hdr.xdim*(z - ncol*(row/hdr.ydim))
    if(dsp[fi.cw].orientation eq !NEURO) then $
        image = reverse(image,1)
    if(dsp[fi.cw].scale ne !FIXED) then begin
        min = float(min(image(*,*)))
        max = float(max(image(*,*)))
        scl = !LEN_COLORTAB1/(max - min)
        img(col:col+hdr.xdim-1,row:row+hdr.ydim-1) = scl*(image(*,*) - min)
    endif else begin
        scl = !LEN_COLORTAB1/float(dsp[fi.cw].fixed_max - dsp[fi.cw].fixed_min)
	min = dsp[fi.cw].fixed_min
        image = scl*(image(*,*) - min)
        mask = image le !LEN_COLORTAB1
        image = image*(image le !LEN_COLORTAB1) + (image gt !LEN_COLORTAB1)*!LEN_COLORTAB1
        img(col:col+hdr.xdim-1,row:row+hdr.ydim-1) = image
    endelse
end

labels = strarr(2)
labels(0) = 'Black and White'
labels(1) = 'Color'
color = get_button(labels,TITLE='Multiple comparisons-corrected image.')

set_plot,'ps'
plot_file = strcompress(string(getenv("HOME"),"/idl.ps"),/REMOVE_ALL)




;if(color eq 0) then begin
;    device,YSIZE=18,XSIZE=22.5,FILENAME=fi.printfile,/LANDSCAPE
;    cmd = string('lpr -P',pref.printer_name,' ',fi.printfile)
;endif else begin
;    device,YSIZE=18,XSIZE=22.5,FILENAME=fi.printfile,/LANDSCAPE,/COLOR,BITS=8
;    cmd = string('lpr -P',pref.printer_name,' ',fi.printfile)
;endelse

if color eq 0 then $
    device,YSIZE=18,XSIZE=22.5,FILENAME=fi.printfile,/LANDSCAPE $
else $
    device,YSIZE=18,XSIZE=22.5,FILENAME=fi.printfile,/LANDSCAPE,/COLOR,BITS=8
cmd = get_printer_cmd(pref.printer_name,fi.printfile)






tv,img,/ORDER

if(dsp[fi.cw].orientation eq !NEURO) then $
    xyouts,0,0,'Neuro',/DEVICE $
else $
    xyouts,0,0,'Rad',/DEVICE

device,/CLOSE
label = 'Revise printer command: '
revised_cmd = get_str(1,label,cmd,WIDTH=60)
spawn,revised_cmd
set_plot,'X'

return
end
