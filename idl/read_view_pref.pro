;Copyright 2/27/01 Washington University.  All Rights Reserved.
;read_view_pref.pro  $Revision: 1.27 $
pro read_view_pref,prf,sprf,file,pref_path,display_mode,reg,DEFAULTS=defaults
if keyword_set(DEFAULTS) then begin
    prf = {View_window_pref}
    reg = {Reg}
endif
prf.color_min1 = 0
prf.color_max1 = 100
prf.color_gamma1 = -2.
prf.color_min2 = 0
prf.color_max2 = 100
prf.color_gamma2 = 0.
prf.black_background = !TRUE
prf.black_background_mask = !FALSE
prf.scale_anat_min = 0.
prf.scale_anat_max = 0.
prf.anat_local_global = 0
prf.crop_anat = 0.
prf.display_units = !DISPLAY_Z
prf.coding = 0
prf.color_bar_type = !NO_COLOR_BAR
;prf.label_color = !YELLOW
prf.label_color = 1 
prf.points_shape = 1 
prf.points_color = !RED 
prf.points_size = 1.
prf.overlay_zoom_type = !BILINEAR
sprf.mask = !TRUE
prf.lcf = 0
prf.orientation = !NEURO
openr,lu,file,/GET_LUN
label = ''
repeat begin
    str = ''
    readf,lu,label,str,FORMAT='(a25,a)'

    ;label = strcompress(label,/REMOVE_ALL)
    ;str = str_sep(strcompress(strtrim(str[0],2)),' ',/TRIM)
    ;START130219
    label = trim(label)
    str = trim(str)

    case label of 
        'sup_to_inf': sprf.sup_to_inf = long(str[0])
        'slice_not_z': sprf.slice_not_z = long(str[0])
        'view': sprf.view = long(str[0])
        'sagittal_face_left': sprf.sagittal_face_left = long(str[0])
        'first_slice': sprf.first_slice = long(str[0])
        'last_slice': sprf.last_slice = long(str[0])
        'slice_incr': sprf.slice_incr = long(str[0])
        'slice_type': sprf.slice_type = long(str[0])
        'mask': sprf.mask = long(str[0])
        else: 
    endcase
    if keyword_set(DEFAULTS) then begin
        case label of 
            'xdim_draw': prf.xdim_draw = long(str[0])
            'ydim_draw': prf.ydim_draw = long(str[0])
            'xgap': prf.xgap = long(str[0])
            'ygap': prf.ygap = long(str[0])
            'transverse_width': prf.transverse_width = long(str[0])
            'transverse_height': prf.transverse_height = long(str[0])
            'sagittal_width': prf.sagittal_width = long(str[0])
            'sagittal_height': prf.sagittal_height = long(str[0])
            'coronal_width': prf.coronal_width = long(str[0])
            'coronal_height': prf.coronal_height = long(str[0])
            'sx': prf.sx = long(str[0])
            'sy': prf.sy = long(str[0])
            'show_filename': prf.lcf = fix(str[0])
            'fx': prf.fx = long(str[0])
            'fy': prf.fy = long(str[0])
            'display_units': prf.display_units = long(str[0])
            'coding': prf.coding = long(str[0])
            'font_family': prf.font_family = str[0]
            'font_size': prf.font_size = long(str[0])
            'font_bold': prf.font_bold = long(str[0])
            'font_italic': prf.font_italic = long(str[0])
            'lc_act_type': prf.lc_act_type = long(str[0])
            'lc_auto_scale': prf.lc_auto_scale = long(str[0])
            'orientation': prf.orientation = long(str[0])
            'zoom': prf.zoom = long(str[0])
            'overlay_zoom_type': prf.overlay_zoom_type = long(str[0])
            'scale_min': prf.scale_pos_min = float(str[0])
            'scale_max': prf.scale_pos_max = float(str[0])
            'scale_neg_min': prf.scale_neg_min = float(str[0])
            'scale_neg_max': prf.scale_neg_max = float(str[0])
            'scale_anat_min': prf.scale_anat_min = float(str[0])
            'scale_anat_max': prf.scale_anat_max = float(str[0])
            'anat_local_global': prf.anat_local_global = long(str[0])
            'crop_anat': prf.crop_anat = float(str[0])
            'color_bar_type': prf.color_bar_type = float(str[0])
            'color_x': prf.color_x = long(str[0])
            'color_y': prf.color_y = long(str[0])
            'color_max1': prf.color_max1 = float(str[0])
            'color_min1': prf.color_min1 = float(str[0])
            'color_gamma1': prf.color_gamma1 = float(str[0])
            'color_max2': prf.color_max2 = float(str[0])
            'color_min2': prf.color_min2 = float(str[0])
            'color_gamma2': prf.color_gamma2 = float(str[0])
            'color_table1': prf.color_table1 = long(str[0])
            'color_table2': prf.color_table2 = long(str[0])
            'black_background': prf.black_background = long(str[0])
            'black_background_mask': prf.black_background_mask = long(str[0])
            'pref_path': pref_path = str[0]
            'display_mode': display_mode = long(str[0])
            'print_xdim': prf.print_xdim = long(str[0])
            'print_ydim': prf.print_ydim = long(str[0])
            'print_x0': prf.print_x0 = long(str[0])
            'print_y0': prf.print_y0 = long(str[0])
            'print_orientation': prf.print_orientation = long(str[0])
            'print_color': prf.print_color = long(str[0])
            'printer_name': prf.printer_name = str[0]
            'points_shape': prf.points_shape = long(str[0])
            'points_color': prf.points_color = long(str[0])
            'points_size': prf.points_size = float(str[0])
            'label_color': prf.label_color = long(str[0])

            ;'reg_checked': reg.checked = ptr_new(fix(strsplit(str[0],/EXTRACT)))
            ;'reg_color_index': reg.color_index = ptr_new(fix(strsplit(str[0],/EXTRACT)))
            ;'reg_colorbarlabels': reg.colorbarlabels = ptr_new(strsplit(str[0],/EXTRACT))
            ;START130219
            'reg_checked': reg.checked = ptr_new(fix(strsplit(str,/EXTRACT)))
            'reg_color_index': reg.color_index = ptr_new(fix(strsplit(str,/EXTRACT)))
            'reg_colorbarlabels': reg.colorbarlabels = ptr_new(strsplit(str,/EXTRACT))
            'reg_val': reg.val = ptr_new(fix(strsplit(str,/EXTRACT)))

            else: ;;;print,'Invalid label: ',label
    endcase
  endif
endrep until(eof(lu))
close,lu
free_lun,lu
if prf.transverse_width then prf.transverse_width = !TRANSVERSE_WIDTH_DFLT
if prf.transverse_height then prf.transverse_height = !TRANSVERSE_HEIGHT_DFLT
if prf.sagittal_width then prf.sagittal_width = !SAGITTAL_WIDTH_DFLT
if prf.sagittal_height then prf.sagittal_height = !SAGITTAL_HEIGHT_DFLT
if prf.coronal_width then prf.coronal_width = !CORONAL_WIDTH_DFLT
if prf.coronal_height then prf.coronal_height = !CORONAL_HEIGHT_DFLT
if prf.label_color ge !LEN_COLORTAB then prf.label_color = prf.label_color - !LEN_COLORTAB
if prf.label_color ge !NUM_LINECOLORS then prf.label_color = prf.label_color - !NUM_LINECOLORS
end
