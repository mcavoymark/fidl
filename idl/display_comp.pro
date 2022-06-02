;Copyright 12/31/99 Washington University.  All Rights Reserved.
;display_comp.pro  $Revision: 12.92 $

;************************************
pro display_comp,dsp,fi,help,stc,pref
;************************************

labels = strarr(fi.nfiles)
labels(0:fi.nfiles-1) = fi.tails(0:fi.nfiles-1)
idx1 = get_button(labels,TITLE='First image')
file1 = fi.tails(idx1)
 
labels = strarr(fi.nfiles)
labels(0:fi.nfiles-1) = fi.tails(0:fi.nfiles-1)
idx2 = get_button(labels,TITLE='Image to be subtracted')
file2 = fi.tails(idx2)
 
hdr1 = *fi.hdr_ptr(idx1)
hdr2 = *fi.hdr_ptr(idx2)
hdr = hdr1

if(hdr1.xdim*hdr1.ydim ne hdr2.xdim*hdr2.ydim) then begin
    stat=widget_message('Images must have the same dimensions.')
    return
endif

disp = widget_base(title=fi.tail,/COLUMN)
zoom1 = 128/hdr.xdim
if(zoom1 lt 1) then zoom1 = 1
if(dsp[fi.cw].zoom < zoom1) then zoom1 = dsp[fi.cw].zoom
zoom1 = dsp[fi.cw].zoom
if hdr.xdim*hdr.ydim*hdr.zdim eq !ATLAS_222_X*!ATLAS_222_Y*!ATLAS_222_Z then $
    zoom1 = 1.

dx = hdr.xdim*zoom1
dy = hdr.ydim*zoom1
if hdr.xdim*hdr.ydim*hdr.zdim eq !ATLAS_222_X*!ATLAS_222_Y*!ATLAS_222_Z then begin
    nrow = 15
    ncol = 5
endif else if hdr.zdim eq !ATLAS_333_Z then begin
    nrow = 8
    ncol = 6
endif else begin
    ncol = fix(1024/dx)
    nrow = fix(hdr.zdim/ncol)
endelse

if(nrow*ncol lt hdr.zdim) then nrow = nrow + 1
nrow = 2*nrow	; Account for two image files.
xsize = ncol*dx
ysize = nrow*dy

if(xsize lt 1024) then begin
    if(ysize lt 850) then begin
        wimg = widget_draw(disp,XSIZE=ncol*dx+4,YSIZE=nrow*dy+4,RETAIN=2, $
           KILL_NOTIFY=string('displayall_dead'))
    endif else begin
        wimg = widget_draw(disp,XSIZE=ncol*dx+4,YSIZE=nrow*dy+4,RETAIN=2, $
           X_SCROLL_SIZE=xsize,Y_SCROLL_SIZE=800,/SCROLL, $
           KILL_NOTIFY=string('displayall_dead'))
    endelse
endif else begin
    if(ysize lt 850) then begin
        wimg = widget_draw(disp,XSIZE=ncol*dx+4,YSIZE=nrow*dy+4,RETAIN=2, $
            X_SCROLL_SIZE=1000,Y_SCROLL_SIZE=ysize,/SCROLL, $
            KILL_NOTIFY=string('displayall_dead'))
    endif else begin
        wimg = widget_draw(disp,XSIZE=ncol*dx+4,YSIZE=nrow*dy+4,RETAIN=2, $
            X_SCROLL_SIZE=1000,Y_SCROLL_SIZE=800,/SCROLL, $
            KILL_NOTIFY=string('displayall_dead'))
    endelse
endelse

widget_control,disp,/REALIZE
widget_control,GET_VALUE=index,wimg
wset,index
dsp[fi.cw].cur_window = dsp[fi.cw].nwindows
dsp[fi.cw].nwindows = dsp[fi.cw].nwindows + 1
dsp[fi.cw].id(dsp[fi.cw].cur_window) = index
dsp[fi.cw].base(dsp[fi.cw].cur_window) = disp
dsp[fi.cw].draw(dsp[fi.cw].cur_window) = wimg
img = fltarr(hdr.xdim,hdr.ydim,2*hdr.zdim)
zoff = hdr.zdim*(dsp[fi.cw].frame-1)
widget_control,/HOURGLASS

z = 0
while(z lt 2*hdr.zdim) do begin
    irow = z/ncol
    icol = z - irow*ncol
    jrow = irow/2
    zz = jrow*ncol + icol
    ;print,'z = ',z,'  zz = ',zz
    if zz le hdr.zdim-1 then begin
        if(2*jrow eq irow) then $
            img(*,*,z) = get_image(zz+zoff,fi,stc,FILNUM=idx1+1) $
        else $
            img(*,*,z) = get_image(zz+zoff,fi,stc,FILNUM=idx2+1)
    endif
    z = z + 1
endwhile
widget_control,HOURGLASS=0

if(dsp[fi.cw].type eq !OVERLAY_ARRAY) then begin
    display,img,min,max,pref,ZOOM=zoom1,/ANNOTATE,ORIENT=dsp[fi.cw].orientation,NCOLORS=!LEN_COLORTAB,SCL=1.
endif else begin
    case dsp[fi.cw].scale of
        !GLOBAL: display_glob,img,min,max,pref,ZOOM=zoom1,/ANNOTATE,ORIENT=dsp[fi.cw].orientation,NCOLORS=!LEN_COLORTAB1
        !LOCAL: display,img,min,max,pref,ZOOM=zoom1,/ANNOTATE,ORIENT=dsp[fi.cw].orientation,NCOLORS=!LEN_COLORTAB1
        !FIXED: begin
	    scale = !LEN_COLORTAB1/float(dsp[fi.cw].fixed_max - dsp[fi.cw].fixed_min)
            display,img,min,max,pref,ZOOM=ZOOM1,SCL=scale,FIXED_MIN=dsp[fi.cw].fixed_min, $
		/ANNOTATE,ORIENT=dsp[fi.cw].orientation,NCOLORS=!LEN_COLORTAB1
            end
        else: stat=widget_message('Illegal display mode.',/ERROR)
    endcase
endelse

;device,set_graphics=6

for i=1,nrow do $
    plots,[0,xsize],[i*dy,i*dy],COLOR=dsp[fi.cw].green,/DEV,/NOCLIP
for i=1,ncol do $
    plots,[i*dx,i*dx],[0,ysize],COLOR=dsp[fi.cw].green,/DEV,/NOCLIP
z = 1
for i=0,nrow-1 do begin
    y = (nrow -i - 1)*dy + 5
    if(2*(i/2) eq i) then $
        xyouts,5,y,file1,/DEVICE,CHARSIZE=1.15,COLOR= !YELLOW $
    else $
        xyouts,5,y,file2,/DEVICE,CHARSIZE=1.15,COLOR= !YELLOW
    for j=0,ncol-1 do begin
        z = j + ncol*(i/2) + 1
        x = (j+1)*dx - 5
        str = strcompress(string(z),/REMOVE_ALL)
        xyouts,x,y,str,/DEVICE,ALIGNMENT=1.,CHARSIZE=1.15,COLOR= !YELLOW
    endfor
endfor

wset,dsp[fi.cw].image_index[fi.cw]

return
end
