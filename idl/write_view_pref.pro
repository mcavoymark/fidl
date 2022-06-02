;Copyright 2/27/01 Washington University.  All Rights Reserved.
;write_view_pref.pro  $Revision: 1.21 $

;pro write_view_pref,file,prf,sprf,fi,pref_path,display_mode,fnt
;START130215
pro write_view_pref,file,win,fi,pref_path,display_mode,fnt
prf = win.prf
sprf = win.sprf[win.idxidx]

openw,lu,file,/GET_LUN
printf,lu,'xdim_draw',strtrim(prf.xdim_draw,2),FORMAT='(a25,4x,a)'
printf,lu,'ydim_draw',strtrim(prf.ydim_draw,2),FORMAT='(a25,4x,a)'
printf,lu,'xgap',strtrim(prf.xgap,2),FORMAT='(a25,4x,a)'
printf,lu,'ygap',strtrim(prf.ygap,2),FORMAT='(a25,4x,a)'
printf,lu,'transverse_width',strtrim(prf.transverse_width,2),FORMAT='(a25,4x,a)'
printf,lu,'transverse_height',strtrim(prf.transverse_height,2),FORMAT='(a25,4x,a)'
printf,lu,'sagittal_width',strtrim(prf.sagittal_width,2),FORMAT='(a25,4x,a)'
printf,lu,'sagittal_height',strtrim(prf.sagittal_height,2),FORMAT='(a25,4x,a)'
printf,lu,'coronal_width',strtrim(prf.coronal_width,2),FORMAT='(a25,4x,a)'
printf,lu,'coronal_height',strtrim(prf.coronal_height,2),FORMAT='(a25,4x,a)'
printf,lu,'sx',strtrim(prf.sx,2),FORMAT='(a25,4x,a)'
printf,lu,'sy',strtrim(prf.sy,2),FORMAT='(a25,4x,a)'
printf,lu,'show_filename',strtrim(prf.lcf,2),FORMAT='(a25,4x,a)'
printf,lu,'fx',strtrim(prf.fx,2),FORMAT='(a25,4x,a)'
printf,lu,'fy',strtrim(prf.fy,2),FORMAT='(a25,4x,a)'
printf,lu,'display_units',strtrim(prf.display_units,2),FORMAT='(a25,4x,a)'
printf,lu,'coding',strtrim(prf.coding,2),FORMAT='(a25,4x,a)'
printf,lu,'font_family',fnt.family,FORMAT='(a25,4x,a)'
printf,lu,'font_size',strtrim(fnt.size,2),FORMAT='(a25,4x,a)'
printf,lu,'font_bold',strtrim(fnt.bold,2),FORMAT='(a25,4x,a)'
printf,lu,'font_italic',strtrim(fnt.italic,2),FORMAT='(a25,4x,a)'
printf,lu,'sup_to_inf',strtrim(sprf.sup_to_inf,2),FORMAT='(a25,4x,a)'
printf,lu,'slice_not_z',strtrim(sprf.slice_not_z,2),FORMAT='(a25,4x,a)'
printf,lu,'view',strtrim(sprf.view,2),FORMAT='(a25,4x,a)'
printf,lu,'sagittal_face_left',strtrim(sprf.sagittal_face_left,2),FORMAT='(a25,4x,a)'
printf,lu,'slice_type',strtrim(sprf.slice_type,2),FORMAT='(a25,4x,a)'
printf,lu,'first_slice',strtrim(sprf.first_slice,2),FORMAT='(a25,4x,a)'
printf,lu,'last_slice',strtrim(sprf.last_slice,2),FORMAT='(a25,4x,a)'
printf,lu,'slice_incr',strtrim(sprf.slice_incr,2),FORMAT='(a25,4x,a)'
printf,lu,'mask',strtrim(sprf.mask,2),FORMAT='(a25,4x,a)'
printf,lu,'lc_act_type',strtrim(prf.lc_act_type,2),FORMAT='(a25,4x,a)'
printf,lu,'lc_auto_scale',strtrim(prf.lc_auto_scale,2),FORMAT='(a25,4x,a)'
printf,lu,'zoom',strtrim(prf.zoom,2),FORMAT='(a25,4x,a)'
printf,lu,'overlay_zoom_type',strtrim(prf.overlay_zoom_type,2),FORMAT='(a25,4x,a)'
printf,lu,'scale_min',strtrim(prf.scale_pos_min,2),FORMAT='(a25,4x,a)'
printf,lu,'scale_max',strtrim(prf.scale_pos_max,2),FORMAT='(a25,4x,a)'
printf,lu,'scale_neg_min',strtrim(prf.scale_neg_min,2),FORMAT='(a25,4x,a)'
printf,lu,'scale_neg_max',strtrim(prf.scale_neg_max,2),FORMAT='(a25,4x,a)'
printf,lu,'scale_anat_min',strtrim(prf.scale_anat_min,2),FORMAT='(a25,4x,a)'
printf,lu,'scale_anat_max',strtrim(prf.scale_anat_max,2),FORMAT='(a25,4x,a)'
printf,lu,'anat_local_global',strtrim(prf.anat_local_global,2),FORMAT='(a25,4x,a)'
printf,lu,'crop_anat',strtrim(prf.crop_anat,2),FORMAT='(a25,4x,a)'
printf,lu,'color_max1',strtrim(fi.color_max1[!NUM_FILES],2),FORMAT='(a25,4x,a)'
printf,lu,'color_min1',strtrim(fi.color_min1[!NUM_FILES],2),FORMAT='(a25,4x,a)'
printf,lu,'color_bar_type',strtrim(prf.color_bar_type,2),FORMAT='(a25,4x,a)'
printf,lu,'color_x',strtrim(prf.color_x,2),FORMAT='(a25,4x,a)'
printf,lu,'color_y',strtrim(prf.color_y,2),FORMAT='(a25,4x,a)'
printf,lu,'color_gamma1',strtrim(fi.color_gamma1[!NUM_FILES],2),FORMAT='(a25,4x,a)'
printf,lu,'color_max2',strtrim(fi.color_max2[!NUM_FILES],2),FORMAT='(a25,4x,a)'
printf,lu,'color_min2',strtrim(fi.color_min2[!NUM_FILES],2),FORMAT='(a25,4x,a)'
printf,lu,'color_gamma2',strtrim(fi.color_gamma2[!NUM_FILES],2),FORMAT='(a25,4x,a)'
printf,lu,'color_table1',strtrim(fi.color_scale1[!NUM_FILES],2),FORMAT='(a25,4x,a)'
printf,lu,'color_table2',strtrim(fi.color_scale2[!NUM_FILES],2),FORMAT='(a25,4x,a)'
printf,lu,'black_background',strtrim(prf.black_background,2),FORMAT='(a25,4x,a)'
printf,lu,'black_background_mask',strtrim(prf.black_background_mask,2),FORMAT='(a25,4x,a)'
printf,lu,'pref_path',pref_path,FORMAT='(a25,4x,a)'
printf,lu,'display_mode',strtrim(display_mode,2),FORMAT='(a25,4x,a)'
printf,lu,'print_xdim',strtrim(prf.print_xdim,2),FORMAT='(a25,4x,a)'
printf,lu,'print_ydim',strtrim(prf.print_ydim,2),FORMAT='(a25,4x,a)'
printf,lu,'print_x0',strtrim(prf.print_x0,2),FORMAT='(a25,4x,a)'
printf,lu,'print_y0',strtrim(prf.print_y0,2),FORMAT='(a25,4x,a)'
printf,lu,'print_orientation',strtrim(prf.print_orientation,2),FORMAT='(a25,4x,a)'
printf,lu,'print_color',strtrim(prf.print_color,2),FORMAT='(a25,4x,a)'
printf,lu,'printer_name',prf.printer_name,FORMAT='(a25,4x,a)'
printf,lu,'points_shape',strtrim(prf.points_shape,2),FORMAT='(a25,4x,a)'
printf,lu,'points_color',strtrim(prf.points_color,2),FORMAT='(a25,4x,a)'
printf,lu,'points_size',strtrim(prf.points_size,2),FORMAT='(a25,4x,a)'
printf,lu,'label_color',strtrim(prf.label_color,2),FORMAT='(a25,4x,a)'

;START130215
if win.nreg[win.idxidx] gt 0 then begin
    if ptr_valid(win.reg[win.idxidx].checked) then begin
        index = where(*win.reg[win.idxidx].checked gt 0,count)
        if count gt 0 then begin
            printf,lu,'reg_checked',strjoin(trim(*win.reg[win.idxidx].checked),' ',/SINGLE),FORMAT='(a25,4x,a)'
            if ptr_valid(win.reg[win.idxidx].color_index) then $ 
                printf,lu,'reg_color_index',strjoin(trim(*win.reg[win.idxidx].color_index),' ',/SINGLE),FORMAT='(a25,4x,a)'
            if ptr_valid(win.reg[win.idxidx].colorbarlabels) then $
                printf,lu,'reg_colorbarlabels',strjoin(trim(*win.reg[win.idxidx].colorbarlabels),' ',/SINGLE),FORMAT='(a25,4x,a)'

            ;START130219
            if ptr_valid(win.reg[win.idxidx].colorbarlabels) then $
                printf,lu,'reg_val',strjoin(trim(*win.reg[win.idxidx].val),' ',/SINGLE),FORMAT='(a25,4x,a)'

        endif
    endif
endif

close,lu
free_lun,lu
end
