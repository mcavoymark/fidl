;Copyright 12/31/99 Washington University.  All Rights Reserved.
;overlay_actmap.pro  $Revision: 12.91 $
pro overlay_actmap,fi,dsp,wd,st,stc,help,pref
frame =1
goback1:
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr_act,idx_zstat,'statistical map') eq !ERROR then return
if hdr_act.tdim gt 1 then begin
     idx_frame = get_button([string(indgen(hdr_act.tdim)+1),'GO BACK'],TITLE='Select frame to be overlaid')
     if idx_frame eq hdr_act.tdim then goto,goback1
     frame = idx_frame[0] + 1
endif
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr_img,idx_anat,'anatomy') eq !ERROR then return

value = strarr(4)
label = strarr(4)
value(0) = string('0')
value(1) = string('1')
value(2) = string(hdr_img.zdim)
xdimnew = 4*hdr_img.xdim
if(xdimnew gt 256) then $
    xdimnew = 256
value(3) = string(xdimnew)
label(0) = string("Boxcar filter width (pixels): ")
label(1) = string('First plane')
label(2) = string('Last plane')
label(3) = string('X Dimension')
dims = get_str(4,label,value)
fwhm = float(dims(0))
first_pln = fix(dims(1)) - 1
last_pln = fix(dims(2)) - 1
zoomx = float(dims(3))
zoomy = (zoomx/float(hdr_img.xdim))*float(hdr_img.ydim)
dxdy = hdr_img.dxdy*float(hdr_img.xdim)/float(zoomx)

anat_img    = fltarr(hdr_img.xdim,hdr_img.ydim,hdr_img.zdim)
act_img     = fltarr(hdr_act.xdim,hdr_act.ydim,hdr_act.zdim)
anat_img_zoom    = fltarr(zoomx,zoomy,hdr_img.zdim)
act_img_zoom     = fltarr(zoomx,zoomy,hdr_img.zdim)
overlay_img = fltarr(dsp[fi.cw].zoom*hdr_img.xdim,dsp[fi.cw].zoom*hdr_img.ydim,hdr_img.zdim)
mask_anat = fltarr(dsp[fi.cw].zoom*hdr_img.xdim,dsp[fi.cw].zoom*hdr_img.ydim,hdr_img.zdim)
mask        = fltarr(hdr_img.xdim,hdr_img.ydim,hdr_img.zdim)

widget_control,/HOURGLASS
for z=0,hdr_img.zdim-1 do begin
    anat_img(*,*,z) = get_image(z,fi,stc,FILNUM=idx_anat+1)
    mask(*,*,z) = anat_img(*,*,z) gt .05*max(anat_img(*,*,z))
    mean = total(anat_img(*,*,z)*mask(*,*,z))/total(mask(*,*,z))
    mask(*,*,z) = anat_img(*,*,z) gt .05*mean
endfor
anat_img = anat_img*mask

label = strarr(2)
label(0) = string('Default')
label(1) = string('User-specified map')
title = string('Display default map or user-specified map?')
lclogp = get_boolean(title,label)

if(lclogp eq !FALSE) then begin
    labels = strarr(fi.nfiles)
    labels(0:fi.nfiles-1) = fi.tails(0:fi.nfiles-1)
    idx1 = get_button(labels,TITLE='Activation map to be overlayed.')
    widget_control,/HOURGLASS
    mask_act = (act_img gt 0) + (act_img lt 0)
    for z=first_pln,last_pln do begin
        act_img(*,*,z)  = get_image(z+(frame-1)*hdr_act.zdim,fi,stc,FILNUM=idx1+1)
    endfor
    if(frame eq 1) then $
        name = strcompress(string(fi.tails(idx1),FORMAT='(a,"_overlay")'),/REMOVE_ALL) $
    else $
        name = strcompress(string(fi.tails(idx1),frame,FORMAT='(a,"_overlay_frm",i3)'),/REMOVE_ALL)
endif else begin
    widget_control,/HOURGLASS
    for z=first_pln,last_pln do begin
        act_img(*,*,z)  = get_image(z+(frame-1)*hdr_act.zdim,fi,stc,FILNUM=idx_zstat+1)
    endfor
    if(frame eq 1) then $
        name = strcompress(string(fi.tails(idx_zstat),FORMAT='(a,"_overlay")'),/REMOVE_ALL) $
    else $
        name = strcompress(string(fi.tails(idx_zstat),frame,FORMAT='(a,"_overlay_frm",i3)'),/REMOVE_ALL)
endelse

max_anat = max(anat_img)

act_img_smoth = box_smoth(act_img,fwhm)

;mask = 1 - ((act_img_smoth lt  1) * (act_img_smoth ge 0) + $
;       (act_img_smoth gt -1) * (act_img_smoth lt 0))

act_img = act_img < 6.
act_img = act_img > (-6.)
act_img = fix(1.666*mask*act_img)
max = max(act_img,MIN=min)
if(-min gt max) then $
    scl = -(!LEN_COLORTAB2-1)/(2*min) $
else $
    scl =  (!LEN_COLORTAB2-1)/(2*max)
act_img = scl*act_img

for z=0,hdr_img.zdim-1 do begin
    anat_img_zoom(*,*,z) = rebin(anat_img(*,*,z),zoomx,zoomy)
    act_img_zoom(*,*,z)  = rebin(act_img(*,*,z), zoomx,zoomy)
endfor

mask_anat = act_img_zoom eq 0
anat_img_zoom = mask_anat*anat_img_zoom*((!LEN_COLORTAB1-1)/max_anat)
act_img_zoom = (1-mask_anat)*(act_img_zoom + !LEN_COLORTAB1 + !LEN_COLORTAB2/2)

overlay_img = anat_img_zoom + act_img_zoom

widget_control,HOURGLASS=0

;paradigm = fltarr(5)
;paradigm(0) = -10
;put_image,overlay_img,fi,wd,dsp,name,zoomx,zoomy,hdr_img.zdim,1,!FLOAT,dxdy,hdr_img.dz,1,!OVERLAY_ARRAY,idx_anat,paradigm, $
;    UVALUE=idx_zstat,IFH=hdr_img.ifh
;START150731
put_image,overlay_img,fi,wd,dsp,name,zoomx,zoomy,hdr_img.zdim,1,!FLOAT,dxdy,hdr_img.dz,1,!OVERLAY_ARRAY,idx_anat, $
    UVALUE=idx_zstat,IFH=hdr_img.ifh

fi.color_gamma1[fi.n] = -2
fi.color_scale1[fi.n] = !OVERLAY_SCALE
fi.color_scale2[fi.n] = !DIFF_SCALE
fi.secondary[fi.n] = idx_zstat
load_colortable,fi,dsp,TABLE1=!OVERLAY_SCALE,TABLE2=!DIFF_SCALE
end
