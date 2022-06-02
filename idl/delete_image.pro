;Copyright 12/31/99 Washington University.  All Rights Reserved.
;delete_image.pro  $Revision: 12.98 $
pro delete_image,wd,fi,dsp,glm,stc,idx0,ALL=all
common linear_fit,fit_name,slope,intcpt,trend
if (fi.nfiles lt 1) or (idx0 lt 0) then begin
    stat = dialog_message('No files opened.',/ERROR)
    return
endif
if keyword_set(ALL) then idx0 = 0
for idx=idx0,fi.nfiles-1 do begin
    hdr = *fi.hdr_ptr[idx]
    if (hdr.array_type eq !ASSOC_ARRAY) or (hdr.array_type eq !LINEAR_MODEL) then begin
        if fi.lun[idx] gt 0 then free_lun,fi.lun[idx]
    endif
    if ptr_valid(fi.data_ptr[idx]) then ptr_free,fi.data_ptr[idx]
    if ptr_valid(fi.hdr_ptr[idx]) then ptr_free,fi.hdr_ptr[idx]
    if hdr.array_type eq !LINEAR_MODEL then begin

        if ptr_valid(glm[idx].event_file) then ptr_free,glm[idx].event_file
        if ptr_valid(glm[idx].effect_length) then ptr_free,glm[idx].effect_length
        if ptr_valid(glm[idx].effect_label) then ptr_free,glm[idx].effect_label
        if ptr_valid(glm[idx].effect_column) then ptr_free,glm[idx].effect_column
        if ptr_valid(glm[idx].F_names) then ptr_free,glm[idx].F_names
        if ptr_valid(glm[idx].lcfunc) then ptr_free,glm[idx].lcfunc
        if ptr_valid(glm[idx].effect_group) then ptr_free,glm[idx].effect_group
        if ptr_valid(glm[idx].stimlen) then ptr_free,glm[idx].stimlen
        if ptr_valid(glm[idx].delay) then ptr_free,glm[idx].delay
        if ptr_valid(glm[idx].Ysim) then ptr_free,glm[idx].Ysim
        if ptr_valid(glm[idx].valid_frms) then ptr_free,glm[idx].valid_frms
        if ptr_valid(glm[idx].ATAm1) then ptr_free,glm[idx].ATAm1
        if ptr_valid(glm[idx].stimlen_vs_t) then ptr_free,glm[idx].stimlen_vs_t
        ;START140826
        ;undefine,glm[idx]

    endif
    if idx lt fi.nfiles-1 and not keyword_set(ALL) then begin
        lizard = idx+1
        if ptr_valid(fi.data_ptr[lizard]) then begin
             data = *fi.data_ptr[lizard]
             fi.data_ptr[idx] = ptr_new(data)
        endif
        if ptr_valid(fi.hdr_ptr[lizard]) then begin
            hdr = *fi.hdr_ptr[lizard]
            fi.hdr_ptr[idx] = ptr_new(hdr)
        endif
        stc[idx].name = stc[lizard].name
        stc[idx].n = stc[lizard].n
        stc[idx].tdim_all = stc[lizard].tdim_all
        stc[idx].t_to_file = stc[lizard].t_to_file
        stc[idx].tdim_file = stc[lizard].tdim_file
        stc[idx].tdim_sum = stc[lizard].tdim_sum
        stc[idx].hdr_ptr = stc[lizard].hdr_ptr
        stc[idx].filnam = stc[lizard].filnam
        fi.names[idx] = fi.names[lizard]
        fi.tails[idx] = fi.tails[lizard]
        fi.list[idx] = fi.list[lizard]
        fi.paths[idx] = fi.paths[lizard]
        fi.zoom[idx] = fi.zoom[lizard]
        fi.lun[idx] = fi.lun[lizard]
        fi.color_scale1[idx] = fi.color_scale1[lizard]
        fi.color_min1[idx] = fi.color_min1[lizard]
        fi.color_max1[idx] = fi.color_max1[lizard]
        fi.color_gamma1[idx] = fi.color_gamma1[lizard]
        fi.color_scale2[idx] = fi.color_scale2[lizard]
        fi.color_min2[idx] = fi.color_min2[lizard]
        fi.color_max2[idx] = fi.color_max2[lizard]
        fi.color_gamma2[idx] = fi.color_gamma2[lizard]
        fi.secondary[idx] = fi.secondary[lizard]
        hdr1 = *fi.hdr_ptr[lizard]
        if hdr1.array_type eq !LINEAR_MODEL then glm[idx] = glm[lizard]
        for file=0,fi.nfiles-1 do begin
            if ptr_valid(fi.hdr_ptr[file]) and file ne idx then begin
                hdr = *fi.hdr_ptr[file]
                if hdr.mother eq lizard then hdr.mother = idx
                if hdr.model eq lizard then hdr.model = idx
                ptr_free,fi.hdr_ptr[file]
                fi.hdr_ptr[file] = ptr_new(hdr)
            endif
        endfor
    endif
    if !D.WINDOW gt -1 then erase
    widget_control,wd.files,SET_VALUE=fi.list
    widget_control,wd.files,SET_LIST_SELECT=idx
endfor
if not keyword_set(ALL) then begin
    fi.names[fi.nfiles-1] = ''
    fi.tails[fi.nfiles-1] = ''
    fi.list[fi.nfiles-1] = ''
    fi.paths[fi.nfiles-1] = ''
    fi.nfiles = fi.nfiles-1
    fi.n = idx0 
endif else begin
    fi.names[*] = ''
    fi.tails[*] = ''
    fi.list[*] = ''
    fi.paths[*] = ''
    fi.nfiles = 0
    fi.n = 0 
    widget_control,wd.sld_pln,SET_VALUE=1
    widget_control,wd.sld_frm,SET_VALUE=1
    widget_control,wd.sld_pln,SET_SLIDER_MAX=1
    widget_control,wd.sld_frm,SET_SLIDER_MAX=1
    widget_control,wd.sld_pln,SENSITIVE=0
    widget_control,wd.sld_frm,SENSITIVE=0
endelse
widget_control,wd.files,SET_VALUE=fi.list
widget_control,wd.files,SET_LIST_SELECT=fi.n
dsp[fi.cw].file_num = fi.n 
end
