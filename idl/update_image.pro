;Copyright 12/31/99 Washington University.  All Rights Reserved.
;update_image.pro  $Revision: 12.99 $
function update_image,fi,dsp,wd,stc,pref
    ;print,'update_image top fi.nfiles=',fi.nfiles
    ;print,'fi.names=',fi.names
    dsp_image = 0
    if fi.nfiles gt 0 then begin
        selected_files = widget_info(wd.files,/LIST_SELECT)
        fi.nselected = n_elements(selected_files)
        fi.selected_files[0:fi.nselected-1] = selected_files
        
        index = fi.selected_files[0]
        if index lt fi.nfiles then begin
            fi.n = index
            dsp[fi.cw].file_num = fi.n
            fi.current_colors = fi.n
        endif else begin
            widget_control,wd.files,SET_LIST_SELECT=fi.n
            return,0
        endelse
        
        if ptr_valid(fi.hdr_ptr[dsp[fi.cw].file_num]) then $
            hdr = *fi.hdr_ptr(dsp[fi.cw].file_num) $
        else $
            return,0
        
        if dsp[fi.cw].plane ge hdr.zdim then dsp[fi.cw].plane = hdr.zdim
        if dsp[fi.cw].frame ge hdr.tdim then dsp[fi.cw].frame = hdr.tdim

        ;dsp_image = get_image(hdr.zdim*(dsp[fi.cw].frame-1)+dsp[fi.cw].plane-1,fi,stc,FILNUM=dsp[fi.cw].file_num+1)
        ;START160915
        dsp_image = get_image(hdr.zdim*(dsp[fi.cw].frame-1)+dsp[fi.cw].plane-1,fi,stc,dsp[fi.cw].plane-1,dsp[fi.cw].frame-1, $
            FILNUM=dsp[fi.cw].file_num+1)
        
        if n_elements(dsp_image) gt 1 then begin
            dsp[fi.cw].zdim = hdr.zdim
            dsp[fi.cw].tdim = hdr.tdim
        endif else begin
            dsp[fi.cw].zdim = 1
            if dsp[fi.cw].tdim lt 1 then dsp[fi.cw].tdim = 1
        endelse
        dsp[fi.cw].xdim = hdr.xdim
        dsp[fi.cw].ydim = hdr.ydim
        dsp[fi.cw].type = hdr.array_type
        dsp[fi.cw].zdim = hdr.zdim
        dsp[fi.cw].tdim = hdr.tdim
        control_pln_and_frm_sliders,fi,dsp,wd,stc
        refresh_display,fi,dsp,stc,wd,pref,IMAGE=dsp_image
    endif
    ;print,'update_image bottom'
    return,dsp_image
end
