;Copyright 12/31/99 Washington University.  All Rights Reserved.
;draw_pulldown.pro  $Revision: 12.92 $

;*************************
pro draw_pulldown_event,ev
;*************************

common draw_pulldown_comm,idx,button_ids

idx = ev.id
widget_control,ev.top,/DESTROY

return
end

;****************************************************
function draw_pulldown,wd,wd1,pr,fi,glm,help,pref,dsp
;****************************************************

common draw_pulldown_comm

pdbase = widget_base(GROUP_LEADER=wd.wkg_base[fi.cw],/FLOATING,/COLUMN,FRAME=0, $
	/MODAL)

sum_regions = widget_button(pdbase,VALUE='Count regions')
link_image = widget_button(pdbase,VALUE='Link image to image')
link_model = widget_button(pdbase,VALUE='Link image to model')
zprof = widget_button(pdbase,VALUE='Time profile')
def_contrast = widget_button(pdbase,VALUE='Define Contrasts')
apply_linmod = widget_button(pdbase,VALUE='Compute Z from contrasts (IDL)')
load_4dfp = widget_button(pdbase,VALUE='Load 4dfp')
load_glm = widget_button(pdbase,VALUE='Load GLM')
refresh = widget_button(pdbase,VALUE='Refresh display')
hprof = widget_button(pdbase,VALUE='Horizontal profile')
vprof = widget_button(pdbase,VALUE='Vertical profile')
tbox = widget_button(pdbase,VALUE='Time profile size')
cancel = widget_button(pdbase,VALUE='Cancel')
widget_control,/REALIZE,pdbase
xmanager,'draw_pulldown',pdbase

case idx of
    hprof: idx = wd.hprof
    vprof: idx = wd.vprof
    zprof: idx = wd.zprof
    def_contrast: begin
        hdr = *fi.hdr_ptr(fi.n)
        define_contrast,glm,hdr.model,fi,nc,help,dsp,wd,stc,pref
        idx = -1
    end
    apply_linmod: begin
        apply_linmod_lag,fi,dsp,wd,glm,help,stc,pref,/APPLY_LINMOD
        dsp_image = update_image(fi,dsp,wd,stc,pref)
        idx = -1
    end
    load_4dfp: begin
        stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm)
        if(stat eq !OK) then begin
            dsp_image = get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + dsp[fi.cw].plane-1,fi,stc)
            refresh_display,fi,dsp,stc,wd,pref,IMAGE=dsp_image
        endif
        idx = -1
    end
    load_glm: begin
        ;file_name = load_linmod(fi,dsp,wd,glm,help,stc)
        file_name = load_linmod(fi,dsp,wd,glm,help)
        idx = -1
    end
    refresh: idx = wd.refresh
    tbox: idx = wd.t_box_size
    sum_regions: idx = wd1.count_mask
    cancel: idx = -1
    link_image: begin
        hdr = *fi.hdr_ptr(fi.n)
        idxdata = get_button(fi.tails(0:fi.nfiles-1),TITLE='Link current image to:.')
        hdr = *fi.hdr_ptr(fi.n)
        ptr_free,fi.hdr_ptr(fi.n)
        hdr.mother = idxdata
        fi.hdr_ptr(fi.n) = ptr_new(hdr)
        idx = -1
    end
    link_model: begin
        labels = strarr(fi.nfiles)
        index = intarr(fi.nfiles)
        nglm = 0
        for idx=0,fi.nfiles-1 do begin
            if(ptr_valid(glm(idx).A)) then begin
                labels[nglm] = fi.tails[idx]
                index[nglm] = idx
                nglm = nglm + 1
            endif
        endfor
        labels = labels[0:nglm-1]
        idxmodel = get_button(labels,TITLE='Link current image to:.')
        hdr = *fi.hdr_ptr(fi.n)
        ptr_free,fi.hdr_ptr(fi.n)
        hdr.model = index[idxmodel]
        fi.hdr_ptr(fi.n) = ptr_new(hdr)
        idx = -1
    end
    else: print,'Invalid event in draw_pulldown'
endcase

return,idx
end
