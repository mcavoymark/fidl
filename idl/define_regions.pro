;Copyright 12/31/99 Washington University.  All Rights Reserved.
;define_regions.pro  $Revision: 1.79 $

;**************************
pro define_regions_event,ev
;**************************
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

common define_regions_comm,leader,menu_base,disp_base,wd_exit,wd_draw,mask_window,plane,anat_img,mask_img, $
    hdr_anat,hdr_mask,orientation,wd_nextpln,wd_prevpln,zoom,wd_plnnum,wd_delete,draw_mode,ifh,nreg,region_names, $
    current_reg,wd_regnum,wd_label,wd_delete_vox,wd_add_vox,name,wd_highlight,xdim,ydim,zdim,x0,y0,n_clicks,wd_refresh, $
    overlay_type,wd_coord,idx_zstat,wd_label_vox,wd_sel_label,wd_cur_label,wd_unlabel,wd_drawbase,wd_zoomin,wd_zoomout, $
    interp,dsp_sizex,dsp_sizey,old_ev,old_evid,wd_reslice,reslice_type,xm1a,ym1a,draw_box,wd_overlay,zstat_img,xd,yd, $
    zd,wd_sel_label_flag,first_time,mask_slice,zstat_slice_unzoomed,colortable1_zmap,colortable2_zmap, $
    colortable1_regions,colortable2_regions,wd_showcolors,lc_showcolors,wd_savename,wd_save, $
    wd_respect,respect,wd_thisslice,thisslice,wd_zminlab,wd_zmaxlab,wd_zmin,wd_zmax,zmin,zmax,wd_rzmap,wd_rmask,rzmap, $
    wd_bb,wd_bbx1l,wd_bbx1,wd_bbx2l,wd_bbx2,wd_bby1l,wd_bby1,wd_bby2l,wd_bby2,wd_bbz1l,wd_bbz1,wd_bbz2l,wd_bbz2, $
    bb,bbx1,bbx2,bby1,bby2,bbz1,bbz2,anat_slice_unzoomed,zstat_img_max,zstat_img_min,wd_scaling,scaling_type, $
    wd_thresh,thresh,wd_gt_ge,gt_ge,hdr_zstat,idx_mask,or_with_zmap

if mask_window gt 0 then wset,mask_window

if first_time eq 0 then begin 
    if old_ev.id ne wd_sel_label then case_event,fi,wd,dsp,help,stc,pref,old_ev 
endif
if overlay_type eq 0 then begin
    colortable1_regions = fi.color_scale1[fi.current_colors]
    colortable2_regions = fi.color_scale2[fi.current_colors]
endif else begin
    colortable1_zmap = fi.color_scale1[fi.current_colors]
    colortable2_zmap = fi.color_scale2[fi.current_colors]
endelse
case_event,fi,wd,dsp,help,stc,pref,ev

end

;START140214
pro get_mask_img
    common stats_comm
    common define_regions_comm
    for z=0,hdr_zstat.zdim-1 do begin
        zstat = zstat_img[*,*,z] 
        if idx_mask ge 0 then begin
            mask = get_image(z,fi,stc,FILNUM=idx_mask+1)
            if or_with_zmap eq 0 then mask = mask or (abs(zstat) gt thresh)
        endif else begin
            if(idx_mask eq -1) then $
                mask = abs(zstat) gt thresh 
            if(idx_mask eq -2) then $
                mask = fltarr(hdr_mask.xdim,hdr_mask.ydim)
            if(idx_mask eq -3) then begin
                mask = fltarr(hdr_mask.xdim,hdr_mask.ydim)
                mask[*,*] = 1
            endif
        endelse
        mask_img[*,*,z] = mask
    endfor
end


pro select_label,fi,wd,dsp,help,stc
    common define_regions_comm
    labels = strarr(nreg+1)
    if(nreg gt 0) then labels[0:nreg-1] = region_names[0:nreg-1]
    labels[nreg] = 'New'
    idx = get_button(labels,TITLE='Select region name',GROUP_LEADER=leader)
    if(idx eq nreg) then begin
        str = get_str(1,'Region Name: ','name',WIDTH=50)
        widget_control,leader,/SHOW
        region_names[nreg] = str[0]
        current_reg = nreg + 2
        nreg = nreg + 1
    endif else begin
        if(idx eq nreg+1) then $
            current_reg = 1 $
        else $
            current_reg = idx + 2
    endelse
    if(current_reg gt 1) then $
        region_name = string(region_names[current_reg-2],FORMAT='(a)') $
    else $
        region_name = 'Unlabel'
    widget_control,wd_cur_label,SET_VALUE=region_name
    wd_sel_label_flag = 1
end


;*******************
pro region_label,val
;*******************
common define_regions_comm

if(val gt 0) then begin
    reg = val - 2
    if(reg ge 0) then $
        region_name = string(region_names[reg],FORMAT='(a)') $
    else $
        region_name = 'Unlabeled'
endif else begin
    region_name = 'None'
endelse
widget_control,wd_regnum,SET_VALUE=region_name

;This piece of code stops the main application window from flying to the forefront when you "Select current label"
;if(dsp[fi.cw].image_index[fi.cw] gt 0) then wset,dsp[fi.cw].image_index[fi.cw]
;But including it moves the regions to the working window.

end

 

;****************************************
pro case_event,fi,wd,dsp,help,stc,pref,ev
;****************************************
common define_regions_comm

widget_control,GET_VALUE=i,wd_draw
mask_window = i
if mask_window gt 0 then wset,mask_window

;print,'here0 ev.id=',ev.id,' wd_thresh=',wd_thresh

case ev.id of
    wd_savename: begin
        widget_control,wd_savename,GET_VALUE=name
        ;print,'name=',name
        ;print,'size(name)=',size(name)
        if n_elements(name) gt 1 then begin
            name = name[n_elements(name)-1]
            widget_control,wd_savename,SET_VALUE=name
        endif
        name = name[0] ;necessarily for behavior of widget in IDL 5.3
        ;print,'wd_savename name=',name
        ;print,'wd_savename size(name)=',size(name)
    end
    wd_save: begin
        if nreg gt 0 then assign_region_names,ifh,mask_img,nreg,REGION_NAMES=region_names 
        ;widget_control,wd_savename,GET_VALUE=name
        print,*ifh.region_names
        error=write_4dfp(name,fi,wd,dsp,help,stc,1,IMAGE=mask_img,INDEX=idx_zstat,IFH=ifh)
    end

    wd_exit: begin
        if dsp[fi.cw].image_index[fi.cw] gt 0 then wset,dsp[fi.cw].image_index[fi.cw]
        widget_control,ev.top,/DESTROY
        if dsp[fi.cw].adjust_color_top gt 0 then widget_control,dsp[fi.cw].adjust_color_top,/DESTROY
        dsp[fi.cw].adjust_color_top = 0L
        if nreg gt 0 then assign_region_names,ifh,mask_img,nreg,REGION_NAMES=region_names 

        ;paradigm = -1
        ;put_image,mask_img,fi,wd,dsp,name,hdr_anat.xdim,hdr_anat.ydim,hdr_anat.zdim,1,!FLOAT,hdr_anat.dxdy, $
        ;    hdr_anat.dz,1.,!LOGICAL_ARRAY,hdr_anat.mother,paradigm,IFH=ifh
        ;START150731
        put_image,mask_img,fi,wd,dsp,name,hdr_anat.xdim,hdr_anat.ydim,hdr_anat.zdim,1,!FLOAT,hdr_anat.dxdy, $
            hdr_anat.dz,1.,!LOGICAL_ARRAY,hdr_anat.mother,IFH=ifh

        dsp_image = update_image(fi,dsp,wd,stc,pref) ;This needs to be here for image to be updated.
    end

    wd_respect: begin
        respect = ev.select
    end
    wd_thisslice: begin
        thisslice = ev.select
        set_respect,1
    end
    wd_rzmap: begin
        rzmap = ev.select
    end
    wd_rmask: begin
       ;do nothing
    end
    wd_zmin: begin
        widget_control,wd_zmin,GET_VALUE=zmin

        ;print,'here0 zmin=',zmin,'END'
        ;print,'here0 size(zmin)=',size(zmax)
        ;print,'here0 finite(zmin[0])=',finite(zmin[0])

        ;zmin = float(zmin[0])
        ;START140414
        if zmin[0] ne '' and zmin[0] ne '-' and zmin[0] ne '+' eq 1 then zmin = float(zmin[0])

        ;print,'here0a'

    end
    wd_zmax: begin
        widget_control,wd_zmax,GET_VALUE=zmax

        ;zmax = float(zmax[0])
        ;START140414
        if zmax[0] ne '' and zmax[0] ne '-' and zmax[0] ne '+' eq 1 then zmax = float(zmax[0])

    end

    ;START140213
    wd_thresh: begin
        widget_control,wd_thresh,GET_VALUE=thresh
        thresh = float(thresh[0])
        print,'here0 thresh=',thresh

        ;START1401214
        get_mask_img

        display_mask_new,dsp,fi,wd
    end
    wd_gt_ge: begin
        gt_ge = ev.value

        ;START1401214
        get_mask_img

        display_mask_new,dsp,fi,wd
    end



    wd_bb: begin
        bb = ev.select
    end



    ;wd_bbx1: begin
    ;    widget_control,wd_bbx1,GET_VALUE=scrap
    ;    bbx1 = float(scrap[0])
    ;end
    ;wd_bbx2: begin
    ;    widget_control,wd_bbx2,GET_VALUE=scrap
    ;    bbx2 = float(scrap[0])
    ;end
    ;wd_bby1: begin
    ;    widget_control,wd_bby1,GET_VALUE=scrap
    ;    bby1 = float(scrap[0])
    ;end
    ;wd_bby2: begin
    ;    widget_control,wd_bby2,GET_VALUE=scrap
    ;    bby2 = float(scrap[0])
    ;end
    ;wd_bbz1: begin
    ;    widget_control,wd_bbz1,GET_VALUE=scrap
    ;    bbz1 = float(scrap[0])
    ;end
    ;wd_bbz2: begin
    ;    widget_control,wd_bbz2,GET_VALUE=scrap
    ;    bbz2 = float(scrap[0])
    ;end

    wd_bbx1: begin
        widget_control,wd_bbx1,GET_VALUE=bbx1
    end
    wd_bbx2: begin
        widget_control,wd_bbx2,GET_VALUE=bbx2
    end
    wd_bby1: begin
        widget_control,wd_bby1,GET_VALUE=bby1
    end
    wd_bby2: begin
        widget_control,wd_bby2,GET_VALUE=bby2
    end
    wd_bbz1: begin
        widget_control,wd_bbz1,GET_VALUE=bbz1
    end
    wd_bbz2: begin
        widget_control,wd_bbz2,GET_VALUE=bbz2
    end








    wd_cur_label: begin
        widget_control,wd_cur_label,GET_VALUE=scrap
        region_names[current_reg-2] = scrap
    end

    wd_zoomin: begin
	zoom = 2*zoom
	refresh_display_mask_new,fi,wd,dsp,help,stc
    end

    wd_zoomout: begin
	zoom = .5*zoom
	refresh_display_mask_new,fi,wd,dsp,help,stc
    end

    wd_refresh: begin
	refresh_display_mask_new,fi,wd,dsp,help,stc
    end

    wd_overlay: begin
        overlay_type = ev.value

        ;if overlay_type eq 1 then load_colortable,fi,dsp,TABLE1=colortable1_zmap,TABLE2=colortable2_zmap
        

        display_mask_new,dsp,fi,wd
        ;if overlay_type eq 1 then begin
        ;    ;This prevents the working window from getting screwed up.
        ;    widget_control,GET_VALUE=mask_window,wd_draw
        ;    if mask_window gt 0 then wset,mask_window
        ;    display_mask_new,dsp,fi,wd
        ;endif
    end


    ;START20
    wd_scaling: begin
        scaling_type = ev.value
        display_mask_new,dsp,fi,wd
    end


    wd_showcolors: begin
        lc_showcolors = ev.value
        if lc_showcolors eq 0 then begin
            if dsp[fi.cw].adjust_color_top eq 0 then $
                adjust_colortable $
            else $
                widget_control,dsp[fi.cw].adjust_color_top,/SHOW
        endif else begin
            widget_control,dsp[fi.cw].adjust_color_top,/DESTROY
            if dsp[fi.cw].show_color_scale eq !TRUE then widget_control,wd.color_scale_base,/DESTROY
        endelse
    end

    wd_reslice: begin
        reslice_type = ev.value + 1
        refresh_display_mask_new,fi,wd,dsp,help,stc
    end


    wd_sel_label: begin
        select_label,fi,wd,dsp,help,stc
        draw_mode = !EDIT_MASK_NONE
        if first_time eq 0 then ev = old_ev
    end

    wd_plnnum: begin
        widget_control,wd_plnnum,GET_VALUE=pln
        plane = pln[0]
        if(plane lt 1) then $
            plane = 1
        ;if(plane gt hdr_anat.zdim) then $
        ;    plane = hdr_anat.zdim
        if plane gt zd then plane = zd
        display_mask_new,dsp,fi,wd
        widget_control,wd_plnnum,SET_VALUE=string(plane,FORMAT='(i3)')
    end

    wd_nextpln: begin
        plane = plane + 1
        ;if(plane gt hdr_anat.zdim) then $
        ;    plane = hdr_anat.zdim
        if plane gt zd then plane = zd
        display_mask_new,dsp,fi,wd
        widget_control,wd_plnnum,SET_VALUE=string(plane,FORMAT='(i3)')
    end

    wd_prevpln: begin
        plane = plane - 1
        if(plane lt 1) then $
            plane = 1
        display_mask_new,dsp,fi,wd
        widget_control,wd_plnnum,SET_VALUE=string(plane,FORMAT='(i3)')
    end

    wd_delete_vox: begin
        draw_mode = !EDIT_MASK_DELETE_VOXELS
        set_respect,0
    end
    wd_add_vox: begin
        draw_mode = !EDIT_MASK_ADD_VOXELS
        set_respect,0
    end
    wd_unlabel: begin
        draw_mode = !EDIT_MASK_UNLABEL
        set_respect,0
    end
    wd_label_vox: begin
        if nreg ge 0 then begin
            if wd_sel_label_flag eq 0 then begin
                select_label,fi,wd,dsp,help,stc
            endif
        endif
        draw_mode = !EDIT_MASK_LABEL_VOX
        set_respect,0
    end
    wd_label: begin
        if nreg ge 0 then begin
            if wd_sel_label_flag eq 0 then begin
                select_label,fi,wd,dsp,help,stc
            endif
        endif
        draw_mode = !EDIT_MASK_LABEL
        set_respect,1
    end
 
    wd_draw: begin
        widget_control,GET_VALUE=i,wd_draw
        mask_window = i
        if mask_window gt 0 then wset,mask_window

        ;v = get_atlas_coords(ev.x,dsp_sizey-ev.y-1,plane-1,0,0,reslice_type,fi.space[idx_zstat],0,0,zoom,$
        ;    !FALSE,orientation,xd,yd,/RETURN_XY)
        ;START150717
        ;v = get_atlas_coords(ev.x,dsp_sizey-ev.y-1,plane-1,0,0,reslice_type,fi.space[idx_zstat],0,0,zoom,!FALSE,orientation,xd,yd,$
        ;    fi,stc,/RETURN_XY)
        ;START150729
        v = get_atlas_coords(ev.x,dsp_sizey-ev.y-1,plane-1,0,0,reslice_type,fi.space[idx_zstat],0,0,zoom,!FALSE,orientation,xd,yd,$
            fi,stc,dsp,/RETURN_XY)

        case reslice_type of
            !TRANSVERSE: begin
                x = v[3]
                y = v[4]
            end
            !SAGITTAL: begin
                x = v[3] 
                y = yd-1-v[4] 
            end
            !CORONAL: begin
                x = v[3]
                y = yd-1-v[4] 
            end
        endcase
        x = round(x)
        y = round(y)
        val = mask_slice[x,y]
        v = [v[0:2],zstat_slice_unzoomed[x,y],val,anat_slice_unzoomed[x,y]]
        widget_control,wd_coord,SET_VALUE=get_space_str(fi.space[idx_zstat],v,LABEL='zmap',SECOND_LABEL='mask',THIRD_LABEL='anat')
        region_label,val
        case draw_mode of
            !EDIT_MASK_NONE: begin
                region_label,val
            end
            else: begin ; Cursor clicked or dragged on
                case ev.type of
                0: begin  ; Button press.
                    x0 = x
                    y0 = y
                    widget_control,wd_draw,/DRAW_MOTION_EVENTS
                    xxx = intarr(16)
                    if(!VERSION.OS_FAMILY eq 'Windows') then $
                        device,set_graphics=6,/CURSOR_CROSSHAIR $
                    else $
                        device,set_graphics=6,cursor_image=xxx
                    n_clicks = 0
                end
                1: begin  ; Button release.
                    device,SET_GRAPHICS=3,/CURSOR_CROSSHAIR
                    msk = fltarr(xd,yd)
                    if n_clicks gt 2 then begin ;Box larger than one voxel was drawn.
                        plots,xm1a,ym1a,COLOR=!GREEN,/DEV,/NOCLIP
                        if x0 gt x then begin
                            xx = x
                            x = x0
                            x0 = xx
                        endif
                        if y0 gt y then begin
                            yy = y
                            y = y0
                            y0 = yy
                        endif
                        x = x - 1
                        y = y - 1
                        msk[x0:x,y0:y] = 1
                        case reslice_type of
                            !TRANSVERSE: begin
                                msk1 = (msk*mask_img[*,*,plane-1] gt 0)
                            end
                            !SAGITTAL: begin
                                msk1 = (msk*mask_img[plane-1,*,*] gt 0)
                            end
                            !CORONAL: begin
                                msk1 = (msk*mask_img[*,plane-1,*] gt 0)
                            end
                        endcase
                    endif else begin
                        draw_box = !DRAW_LOCATION
                        x0 = x
                        y0 = y
                        msk[x0:x,y0:y] = 1  ;needed for !EDIT_MASK_ADD_VOXELS 
                        msk1 = msk          ;needed for !EDIT_MASK_ADD_VOXELS
                    endelse
                    n_clicks = -1
                    if(draw_box eq !DRAW_BOX) or (draw_mode eq !EDIT_MASK_ADD_VOXELS) then begin
                        case draw_mode of
                          !EDIT_MASK_LABEL: begin
                            case reslice_type of
                                !TRANSVERSE: begin
                                    mask_img[*,*,plane-1] = (1-msk)*mask_img[*,*,plane-1] + current_reg*msk1
                                end
                                !SAGITTAL: begin
                                    mask_img[plane-1,*,*] = (1-msk)*mask_img[plane-1,*,*] + current_reg*msk1
                                end
                                !CORONAL: begin
                                    mask_img[*,plane-1,*] = (1-msk)*mask_img[*,plane-1,*] + current_reg*msk1
                                end
                            endcase
                          end
                          !EDIT_MASK_LABEL_VOX: begin
                            case reslice_type of
                                !TRANSVERSE: begin
                                    mask_img[*,*,plane-1] = (1-msk)*mask_img[*,*,plane-1] + current_reg*msk1
                                end
                                !SAGITTAL: begin
                                    mask_img[plane-1,*,*] = (1-msk)*mask_img[plane-1,*,*] + current_reg*msk1
                                end
                                !CORONAL: begin
                                    mask_img[*,plane-1,*] = (1-msk)*mask_img[*,plane-1,*] + current_reg*msk1
                                end
                            endcase
                          end
                          !EDIT_MASK_ADD_VOXELS: begin
                            case reslice_type of
                                !TRANSVERSE: begin
                                    mask_img[*,*,plane-1] = (1-msk)*mask_img[*,*,plane-1] + msk
                                end
                                !SAGITTAL: begin
                                    mask_img[plane-1,*,*] = (1-msk)*mask_img[plane-1,*,*] + msk
                                end
                                !CORONAL: begin
                                    mask_img[*,plane-1,*] = (1-msk)*mask_img[*,plane-1,*] + msk
                                end
                            endcase
                          end
                          !EDIT_MASK_DELETE_VOXELS: begin
                            case reslice_type of
                                !TRANSVERSE: begin
                                    mask_img[*,*,plane-1] = (1-msk)*mask_img[*,*,plane-1]
                                end
                                !SAGITTAL: begin
                                    mask_img[plane-1,*,*] = (1-msk)*mask_img[plane-1,*,*]
                                end
                                !CORONAL: begin
                                    mask_img[*,plane-1,*] = (1-msk)*mask_img[*,plane-1,*]
                                end
                            endcase
                          end
                          !EDIT_MASK_UNLABEL: begin
                            case reslice_type of
                                !TRANSVERSE: begin
                                    mask_img[*,*,plane-1] = (1-msk1)*mask_img[*,*,plane-1] + msk1
                                end
                                !SAGITTAL: begin
                                    mask_img[plane-1,*,*] = (1-msk1)*mask_img[plane-1,*,*] + msk1
                                end
                                !CORONAL: begin
                                    mask_img[*,plane-1,*] = (1-msk1)*mask_img[*,plane-1,*] + msk1
                                end
                            endcase
                          end
                          else: print,'Invalid value of draw_mode.'
                        endcase
                        display_mask_new,dsp,fi,wd
                    endif else begin ; then draw_box must equal DRAW_LOCATION
                        case reslice_type of
                            !TRANSVERSE: begin
                                msk = mask_img[*,*,plane-1]
                            end
                            !SAGITTAL: begin
                                msk = reform(mask_img[plane-1,*,*])
                            end
                            !CORONAL: begin
                                msk = reform(mask_img[*,plane-1,*])
                            end
                        endcase
		        x1 = x
		        y1 = y
                        vox = y1*xd + x1
                        nvox = 1
                        case draw_mode of
                            !EDIT_MASK_LABEL: begin
                                if msk[x,y] gt 0 then begin
                                    if thisslice eq 0 then begin
                                        smask = fltarr(xdim,ydim,zdim)
                                        if respect eq 0 then begin
                                            smask[0:xdim-1,0:ydim-1,0:zdim-1] = float(mask_img eq 1)
                                            minl = 1
                                            maxl = max(mask_img) 
                                        endif else begin
                                            bbmask = intarr(xdim,ydim,zdim)
                                            if bb eq 0 then begin
                                                bbmask[*] = 1
                                            endif else begin
                                                xyz1=round(atlas_to_image_space(hdr,float(bbx1[0]),float(bby1[0]),float(bbz1[0]), $
                                                    !TRANSVERSE,fi.space[idx_zstat],XYZDIM=[xdim,ydim,zdim]))
                                                xyz2=round(atlas_to_image_space(hdr,float(bbx2[0]),float(bby2[0]),float(bbz2[0]), $
                                                    !TRANSVERSE,fi.space[idx_zstat],XYZDIM=[xdim,ydim,zdim]))
                                                print,'xyz1=',xyz1
                                                print,'xyz2=',xyz2
                                                bbx = [xyz1[0],xyz2[0]] 
                                                bby = [xyz1[1],xyz2[1]]
                                                bbz = [xyz1[2],xyz2[2]]
                                                bbx = bbx[sort(bbx)]
                                                bby = bby[sort(bby)]
                                                bbz = bbz[sort(bbz)]
                                                bbmask[bbx[0]:bbx[1],bby[0]:bby[1],bbz[0]:bbz[1]] = 1
                                            endelse

                                            ;START140417
                                            minl = zmin
                                            maxl = zmax

                                            if rzmap eq 1 then begin 

                                                ;smask[0:xdim-1,0:ydim-1,0:zdim-1] = bbmask*zstat_img*(mask_img eq 1)
                                                ;START140417
                                                smask[0:xdim-1,0:ydim-1,0:zdim-1] = bbmask*zstat_img*(mask_img ge 1)

                                                ;if smask[x,y,plane-1] gt maxl or smask[x,y,plane-1] lt minl then begin
                                                ;START140424
                                                case reslice_type of
                                                    !TRANSVERSE: begin 
                                                        x1 = x
                                                        y1 = y
                                                        z1 = plane-1
                                                    end
                                                    !SAGITTAL: begin
                                                        x1 = plane-1
                                                        y1 = x
                                                        z1 = y
                                                    end
                                                    !CORONAL: begin
                                                        x1 = x
                                                        y1 = plane-1
                                                        z1 = y
                                                    end
                                                endcase
                                                if smask[x1,y1,z1] gt maxl or smask[x1,y1,z1] lt minl then begin


                                                    stat=dialog_message('Value at click point is '+strtrim(msk[x,y])+string(10B) $
                                                        +'Your min is set at '+strtrim(minl,2)+string(10B)+'Your max is set at ' $
                                                        +strtrim(maxl,2)+string(10B)+string(10B)+'For a region to be drawn ' $
                                                        +'you must click at a location within these limits.',/ERROR)
                                                endif else begin

                                                    ;vox = search3d(smask,x,y,plane-1,minl,maxl)
                                                    ;START140424
                                                    vox = search3d(smask,x1,y1,z1,minl,maxl)

                                                    ;print,'here0 vox=',vox
                                                    ;print,'here0 n_elements(vox)=',n_elements(vox)

                                                    ;mask_img[vox] = current_reg
                                                    ;START140424
                                                    if get_button(['Ok','Go back'],TITLE='Region will have '+trim(n_elements(vox)) $
                                                        +' voxels') eq 0 then mask_img[vox] = current_reg

                                                endelse
                                            endif else begin
                                                ;smask[0:xdim-1,0:ydim-1,0:zdim-1] = bbmask*(mask_img eq 1)
                                                ;090518
                                                smask[0:xdim-1,0:ydim-1,0:zdim-1] = bbmask*mask_img

                                                ;START140417
                                                if msk[x,y] gt maxl or msk[x,y] lt minl then begin
                                                    stat=dialog_message('Value at click point is '+strtrim(msk[x,y])+string(10B) $
                                                        +'Your min is set at '+strtrim(minl,2)+string(10B)+'Your max is set at ' $
                                                        +strtrim(maxl,2)+string(10B)+string(10B)+'For a region to be drawn ' $
                                                        +'you must click at a location within these limits.',/ERROR)
                                                endif else begin
                                                    vox = search3d(smask,x,y,plane-1,minl,maxl)
                                                    mask_img[vox] = current_reg
                                                endelse

                                            endelse

                                            ;START140417
                                            ;minl = zmin
                                            ;maxl = zmax

                                        endelse
                                        ;print,'minl=',minl,' maxl=',maxl
                                        ;print,'x+1=',x+1,' zz+1=',zz+1,' plane-1=',plane-1,' smask=',smask[x+1,zz+1,plane-1]
                                        ;print,'x=',x,' zz=',zz,' plane-1=',plane-1,' smask=',smask[x,zz,plane-1]
                                        ;print,'mask_img=',mask_img[x,zz,plane-1]
                                        ;print,'current_reg=',current_reg

                                        ;START140417
                                        ;if msk[x,y] gt maxl or msk[x,y] lt minl then begin
                                        ;    stat=dialog_message('Value at click point is '+strtrim(msk[x,y]) $
                                        ;        +string(10B)+'Your min is set at '+strtrim(minl,2)+string(10B)+'Your max is ' $
                                        ;        +'set at '+strtrim(maxl,2)+string(10B)+string(10B)+'For a region to be drawn ' $
                                        ;        +'you must click at a location within these limits.',/ERROR)
                                        ;endif else begin
                                        ;    vox = search3d(smask,x,y,plane-1,minl,maxl)
                                        ;    mask_img[vox] = current_reg
                                        ;endelse



                                    endif else begin
                                        ;Pad because search2d breaks at edge voxels.
                                        smask = fltarr(xd+2,yd+2)
                                        if respect eq 0 then begin
                                            smask[1:xd,1:yd] = float(msk ge 1) ;ORIG
                                            ;vox = search2d(smask,x+1,zz+1,1,max(msk)) ;ORIG
                                            vox = search2d(smask,x+1,y+1,1,max(msk)) ;ORIG
                                        endif else begin
                                            if rzmap eq 1 then begin
                                                smask[1:xd,1:yd] = zstat_slice_unzoomed
                                                ;vox = search2d(smask,x+1,zz+1,zmin,zmax)
                                                vox = search2d(smask,x+1,y+1,zmin,zmax)
                                            endif else begin
                                                smask[1:xd,1:yd] = msk
                                                ;vox = search2d(smask,x+1,zz+1,zmin,zmax)
                                                vox = search2d(smask,x+1,y+1,zmin,zmax)
                                            endelse
                                        endelse
                                        nvox = n_elements(vox)
                                        ;Convert to unpadded coordinates.
                                        for i=0,nvox-1 do begin
                                            y = vox[i]/(xd+2)
                                            x = vox[i] - y*(xd+2)
                                            vox[i] = (y-1)*xd + (x-1)
                                        endfor
                                        mask = fltarr(xd*yd)
                                        for i=0,nvox-1 do mask[vox[i]] = 1
                                        mask = reform(mask,xd,yd)
                                        case reslice_type of
                                            !TRANSVERSE: begin
                                                mask_img[*,*,plane-1] = (1-mask)*mask_img[*,*,plane-1] + current_reg*mask
                                            end
                                            !SAGITAL: begin
                                                mask_img[plane-1,*,*] = (1-mask)*mask_img[plane-1,*,*] + current_reg*mask
                                            end
                                            !CORONAL: begin
                                                mask_img[*,plane-1,*] = (1-mask)*mask_img[*,plane-1,*] + current_reg*mask
                                            end
                                        endcase
                                    endelse
                                endif
                            end
                            !EDIT_MASK_LABEL_VOX: begin
                                if msk[x,y] gt 0 then begin
                                    case reslice_type of
                                        !TRANSVERSE: begin
                                            mask_img[x1,y1,plane-1] = current_reg
                                        end
                                        !SAGITTAL: begin
                                            mask_img[plane-1,x1,y1] = current_reg
                                        end
                                        !CORONAL: begin
                                            mask_img[x1,plane-1,y1] = current_reg
                                        end
                                    endcase
                                endif
                            end
                            !EDIT_MASK_UNLABEL: begin
                                if msk[x,y] gt 0 then begin
                                    case reslice_type of
                                        !TRANSVERSE: begin
                                            mask_img[x1,y1,plane-1] = 1
                                        end
                                        !SAGITAL: begin
                                            mask_img[plane-1,x1,y1] = 1
                                        end
                                        !CORONAL: begin
                                            mask_img[x1,plane-1,y1] = 1
                                        end
                                    endcase
                                    nvox = 0
                                endif
                            end
                            !EDIT_MASK_DELETE_VOXELS: begin
                                case reslice_type of
                                    !TRANSVERSE: begin
                                        mask_img[x1:x1,y1:y1,plane-1] = 0
                                    end
                                    !SAGITTAL: begin
                                        mask_img[plane-1,x1:x1,y1:y1] = 0
                                    end
                                    !CORONAL: begin
                                        mask_img[x1:x1,plane-1,y1:y1] = 0
                                    end
                                endcase
                                nvox = 0
                            end
                            else: print,'Invalid value of draw_mode.'
                        endcase
                        display_mask_new,dsp,fi,wd
                    endelse
                    draw_box = !DRAW_LOCATION
                    draw_mode = !EDIT_MASK_NONE
                end
                2: begin  ; Motion event
                    if n_clicks lt 0 then return
                    n_clicks = n_clicks + 1
                    if(x lt x0) then begin
                        x1 = x
                        x2 = x0
                    endif else begin
                        x1 = x0
                        x2 = x
                    endelse
                    if(y lt y0) then begin
                        y1 = y
                        y2 = y0
                    endif else begin
                        y1 = y0
                        y2 = y
                    endelse
                    x11 = zoom*x1
                    x22 = zoom*x2
                    if orientation eq !NEURO then begin
                         x11 = zoom*xd - x11 - 1
                         x22 = zoom*xd - x22 - 1
                    endif

                 
                    ;y11 = zoom*(yd - y1) - 1
                    ;y22 = zoom*(yd - y2) - 1

                    case reslice_type of
                        !TRANSVERSE: begin
                            y11 = zoom*(yd - y1) - 1
                            y22 = zoom*(yd - y2) - 1
                        end
                        !SAGITTAL: begin
                            y11 = zoom*y1
                            y22 = zoom*y2
                        end
                        !CORONAL: begin
                            y11 = zoom*y1
                            y22 = zoom*y2
                        end
                    endcase



                    if(draw_box eq !DRAW_BOX) then begin
                        plots,xm1a,ym1a,COLOR=!GREEN,/DEV,/NOCLIP
		    endif
                    xm1a = [x11,x11,x22,x22,x11]
                    ym1a = [y11,y22,y22,y11,y11]
                    plots,xm1a,ym1a,COLOR=!GREEN,/DEV,/NOCLIP
                    draw_box = !DRAW_BOX
                end
                else: print,'Unexpected button event in define_regions.'
            endcase
            end
        endcase
    end
    else: print,'Invalid event in define_regions_event.'
endcase

;This piece of code stops the main application window from flying to the forefront when you "Select current label".
;This piece of code also allows regions to be drawn on the "Define Regions" window. 
if(dsp[fi.cw].image_index[fi.cw] gt 0) then wset,dsp[fi.cw].image_index[fi.cw]

if (ev.id eq wd_delete_vox) or $
   (ev.id eq wd_add_vox) or $
   (ev.id eq wd_unlabel) or $
   (ev.id eq wd_label_vox) or $
   (ev.id eq wd_label) then begin 
    old_ev = ev
    first_time = 0
endif

;check_math_errors,!FALSE
end

;********************
pro set_respect,onoff
;********************
common define_regions_comm
widget_control,wd_respect,SENSITIVE=onoff
widget_control,wd_thisslice,SENSITIVE=onoff
widget_control,wd_rzmap,SENSITIVE=onoff
widget_control,wd_rmask,SENSITIVE=onoff
widget_control,wd_zminlab,SENSITIVE=onoff
widget_control,wd_zmaxlab,SENSITIVE=onoff
widget_control,wd_zmin,SENSITIVE=onoff
widget_control,wd_zmax,SENSITIVE=onoff



;widget_control,wd_bb,SENSITIVE=onoff
;widget_control,wd_bbx1l,SENSITIVE=onoff
;widget_control,wd_bbx1,SENSITIVE=onoff
;widget_control,wd_bbx2l,SENSITIVE=onoff
;widget_control,wd_bbx2,SENSITIVE=onoff
;widget_control,wd_bby1l,SENSITIVE=onoff
;widget_control,wd_bby1,SENSITIVE=onoff
;widget_control,wd_bby2l,SENSITIVE=onoff
;widget_control,wd_bby2,SENSITIVE=onoff
;widget_control,wd_bbz1l,SENSITIVE=onoff
;widget_control,wd_bbz1,SENSITIVE=onoff
;widget_control,wd_bbz2l,SENSITIVE=onoff
;widget_control,wd_bbz2,SENSITIVE=onoff

if thisslice eq 0 then scrap = onoff else scrap = 0
widget_control,wd_bb,SENSITIVE=scrap
widget_control,wd_bbx1l,SENSITIVE=scrap
widget_control,wd_bbx1,SENSITIVE=scrap
widget_control,wd_bbx2l,SENSITIVE=scrap
widget_control,wd_bbx2,SENSITIVE=scrap
widget_control,wd_bby1l,SENSITIVE=scrap
widget_control,wd_bby1,SENSITIVE=scrap
widget_control,wd_bby2l,SENSITIVE=scrap
widget_control,wd_bby2,SENSITIVE=scrap
widget_control,wd_bbz1l,SENSITIVE=scrap
widget_control,wd_bbz1,SENSITIVE=scrap
widget_control,wd_bbz2l,SENSITIVE=scrap
widget_control,wd_bbz2,SENSITIVE=scrap
end

;***********************
pro highlight_region_new
;***********************
common define_regions_comm
;scrap = !LEN_COLORTAB1+1
for i=0,nreg-1 do begin
    mask_slice_hl = mask_slice
    if reslice_type ge 2 then mask_slice_hl = reverse(mask_slice_hl,2)
    mask = mask_slice_hl eq (i+2) 
    mask = zoom_by_2n(mask,zoom,!NEAREST_NEIGHBOR) ;mask always zoomed by nearest neighbor
    vox = where(mask,nvox)
    ;for ivox=0,nvox-1 do begin
    for ivox=0L,nvox-1 do begin
        y = long(vox[ivox]/dsp_sizex)
        x = vox[ivox] - y*dsp_sizex
        x1 = zoom*long(x/zoom)
        y1 = zoom*long(y/zoom)
        if(((x mod zoom) eq 0)and((y mod zoom) eq 0)) then begin
            y1 = dsp_sizey - y1 - 1
            ;rect_plot,x1,x1+zoom,y1-zoom,y1,COLOR=!GREEN,ORIENTATION=orientation,DRAW_SIZE=dsp_sizex
            ;rect_plot,x1,x1+zoom,y1-zoom,y1,COLOR=!LEN_COLORTAB+i,ORIENTATION=orientation,DRAW_SIZE=dsp_sizex
            rect_plot,x1,x1+zoom,y1-zoom,y1,COLOR=!LEN_COLORTAB+(i mod !NUM_LINECOLORS),ORIENTATION=orientation,DRAW_SIZE=dsp_sizex
            ;rect_plot,x1,x1+zoom,y1-zoom,y1,COLOR=!LEN_COLORTAB1+1+i,ORIENTATION=orientation,DRAW_SIZE=dsp_sizex
            ;rect_plot,x1,x1+zoom,y1-zoom,y1,COLOR=scrap+i,ORIENTATION=orientation,DRAW_SIZE=dsp_sizex
        endif
    endfor
endfor
end

;*********************
pro display_dimensions 
;*********************
common define_regions_comm
widget_control,/HOURGLASS
case reslice_type of
    !TRANSVERSE: begin
        xd = xdim
        yd = ydim
        zd = zdim
    end
    !SAGITTAL: begin
        xd = ydim
        yd = zdim
        zd = xdim
    end
    !CORONAL: begin
        xd = xdim
        yd = zdim
        zd = ydim
    end
endcase
end


;*****************************
pro display_mask_new,dsp,fi,wd
;*****************************
common define_regions_comm

case reslice_type of
    !TRANSVERSE: begin
        anat_slice = reform(anat_img[*,*,plane-1])
        mask_slice = reform(mask_img[*,*,plane-1])
        zstat_slice = reform(zstat_img[*,*,plane-1])
    end
    !SAGITAL: begin
        anat_slice = reform(anat_img[plane-1,*,*])
        mask_slice = reform(mask_img[plane-1,*,*])
        zstat_slice = reform(zstat_img[plane-1,*,*])
    end
    !CORONAL: begin
        anat_slice = reform(anat_img[*,plane-1,*])
        mask_slice = reform(mask_img[*,plane-1,*])
        zstat_slice = reform(zstat_img[*,plane-1,*])
    end
endcase
zstat_slice_unzoomed = zstat_slice
anat_slice_unzoomed = anat_slice

max1 = max(anat_slice,MIN=min1)
scl1 = (!LEN_COLORTAB1-1)/(max1 - min1)

if overlay_type eq 0 then begin
    mask_binary = mask_slice ne 0
    mask = mask_binary*(mask_slice+!LEN_COLORTAB1-1)
    case interp of
        !NEAREST_NEIGHBOR: anat_slice = zoom_by_2n(anat_slice,zoom,!NEAREST_NEIGHBOR)
        !BILINEAR: anat_slice = zoom_by_2n(anat_slice,zoom,!BILINEAR)
        else: begin
            print,'Invalid interp. Something is amiss.'
        end
    endcase
    mask_binary = zoom_by_2n(mask_binary,zoom,!NEAREST_NEIGHBOR)
    anat = scl1*(anat_slice-min1)*(1-mask_binary)
    mask = zoom_by_2n(mask,zoom,!NEAREST_NEIGHBOR) ;mask is always zoomed by nearest neighbor
    img = anat + mask
endif else begin
    if scaling_type eq 0 then $
        max = max(zstat_slice,MIN=min) $
    else begin
        max = zstat_img_max
        min = zstat_img_min
    endelse
    if(-min gt max) then max = -min
    scl = (!LEN_COLORTAB2-1)/(max-min)
    case interp of
        !NEAREST_NEIGHBOR: begin
            zstat_slice = zoom_by_2n(zstat_slice,zoom,!NEAREST_NEIGHBOR)
            anat_slice = zoom_by_2n(anat_slice,zoom,!NEAREST_NEIGHBOR)
        end
        !BILINEAR: begin
            zstat_slice = zoom_by_2n(zstat_slice,zoom,!BILINEAR)
            anat_slice = zoom_by_2n(anat_slice,zoom,!BILINEAR)
        end
        else: begin
            print,'Invalid interp. Something is amiss.'
        end
    endcase

    ;mask_binary = abs(zstat_slice) gt 1
    ;START131209
    ;mask_binary = abs(zstat_slice) ge 1
    ;START130214
    if gt_ge eq 0 then mask_binary = abs(zstat_slice) gt thresh else mask_binary = abs(zstat_slice) ge thresh 

    mask = mask_binary*(scl*(zstat_slice - min) + !LEN_COLORTAB1)
    anat = scl1*(anat_slice-min1)*(1-mask_binary)
    img = anat + mask
endelse

img = img*(img gt 0)
img = img*(img lt !NUM_COLORS) + (img ge !NUM_COLORS)*(!NUM_COLORS-1)
if reslice_type ge 2 then img = reverse(img,2)
if orientation eq !NEURO then img = reverse(img,1)

if overlay_type eq 0 then begin
    load_colortable,fi,dsp,TABLE1=colortable1_regions,TABLE2=colortable2_regions
endif else begin
    load_colortable,fi,dsp,TABLE1=colortable1_zmap,TABLE2=colortable2_zmap
endelse
tv,img,0,/ORDER
if overlay_type eq 1 then highlight_region_new
if lc_showcolors eq 0 then showcolors,'',wd,dsp,fi
check_math_errors,!FALSE
end




;**********************************************
pro refresh_display_mask_new,fi,wd,dsp,help,stc
;**********************************************
common define_regions_comm
;print,'refresh_display_mask_new top' 
 
display_dimensions
dsp_sizex = zoom*xd
dsp_sizey = zoom*yd

if wd_draw ge 0 then widget_control,wd_drawbase,/DESTROY
wd_drawbase = widget_base(TITLE="Define Regions",/COLUMN,GROUP_LEADER=leader)
yscrollsize = 700
if(dsp_sizey gt yscrollsize) then begin
    if(dsp_sizex gt 800) then $
	xscrollsize = 800 $
    else $
	xscrollsize = dsp_sizex
    wd_draw = widget_draw(wd_drawbase,/BUTTON_EVENTS,/MOTION_EVENTS,COLORS=!LEN_COLORTAB1, $
        XSIZE=dsp_sizex,YSIZE=dsp_sizey,RETAIN=2,X_SCROLL_SIZE=xscrollsize,Y_SCROLL_SIZE=yscrollsize,/SCROLL, $
        KILL_NOTIFY=string('edit_mask_dead'))
endif else begin
    wd_draw = widget_draw(wd_drawbase,/BUTTON_EVENTS,/MOTION_EVENTS,COLORS=!LEN_COLORTAB1, $
        XSIZE=dsp_sizex,YSIZE=dsp_sizey,RETAIN=2,KILL_NOTIFY=string('edit_mask_dead'))
endelse

widget_control,wd_drawbase,/REALIZE
xmanager,'define_regions',wd_drawbase

;This prevents the working window from getting screwed up.
widget_control,GET_VALUE=i,wd_draw
mask_window = i
if mask_window gt 0 then wset,mask_window

if plane gt zd then plane = zd
display_mask_new,dsp,fi,wd
widget_control,wd_plnnum,SET_VALUE=string(plane,FORMAT='(i3)')
end

;***************************************************
pro define_regions,fi,wd,dsp,help,stc,pref,dsp_image
;***************************************************
common define_regions_comm

n_clicks = -1
current_reg = -1
orientation = dsp[fi.cw].orientation
plane = dsp[fi.cw].plane
draw_mode = !EDIT_MASK_NONE
draw_box = !DRAW_LOCATION
overlay_type = 0
scaling_type = 0
reslice_type = !TRANSVERSE
wd_sel_label_flag = 0
first_time = 1
respect = 0
thisslice = 0
bb = 0
bbx1 = 0.
bbx2 = 0.
bby1 = 0.
bby2 = 0.
bbz1 = 0.
bbz2 = 0.

;START1401214
thresh = .9 
gt_ge = 0

colortable1_zmap = !OVERLAY_SCALE 
colortable2_zmap = !DIFF_SCALE 
;colortable1_regions = !GRAY_SCALE
;colortable2_regions = !NO_SCALE
;START
colortable1_regions = !OVERLAY_SCALE 
colortable2_regions = !LINECOLORS
;fi.color_scale1[fi.current_colors] = colortable1_regions
;fi.color_scale2[fi.current_colors] = colortable2_regions

if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Select the thresholded image that will be overlaid on the ' $
    +'anatomical image. It can be corrected for multiple-comparisons or simply thresholded. It may also be another region file.'

if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr_zstat,idx_zstat,'statistical map',TITLE='Please select statistical map.') $
    eq !ERROR then return

if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Enter name of existing region file. Select "New" if none exists.'

repeat begin
    labels = ['New (nonzero values in statistical map)', $
              'New (No potential regions)', $
              'New (Every voxel a potential region)', $
              'On disk','Exit']
    get_filelist_labels,fi,len,tails,index
    if len ne 0 then labels = [tails,labels]
    idx_mask = get_button(labels,TITLE='Please select region file or initial mask.')
    if idx_mask eq len+4 then $
        return $
    else if idx_mask eq len+3 then begin
        repeat begin
            stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,/NOLOAD_COLOR,TITLE='Please select region file or initial mask.')
            if stat eq !OK then begin
                dsp_image = update_image(fi,dsp,wd,stc,pref)
                idx_mask = fi.n
                hdr_mask = *fi.hdr_ptr[idx_mask]
                scrap = 1
            endif else begin
                print,' *** Error while loading file. ***'
                scrap = get_button(['Try again.','Exit'],TITLE='Error loading region file or initial mask.')
                if scrap eq 1 then return
            endelse
        endrep until scrap eq 1
    endif else if idx_mask eq len+2 then begin
        idx_mask = -3
        hdr_mask = hdr_zstat
    endif else if idx_mask eq len+1 then begin
        idx_mask = -2
        hdr_mask = hdr_zstat
    endif else if idx_mask eq len then begin
        idx_mask = -1
        hdr_mask = hdr_zstat
    endif else begin
        hdr_mask = *fi.hdr_ptr[index[idx_mask]]
    endelse

    if idx_mask ge 0 then or_with_zmap = get_button(['yes','no'],TITLE='Do you wish to OR the region file with the zmap?')
    widget_control,/HOURGLASS
    zstat_img = fltarr(hdr_zstat.xdim,hdr_zstat.ydim,hdr_zstat.zdim)
    mask_img = fltarr(hdr_mask.xdim,hdr_mask.ydim,hdr_mask.zdim)
    if n_elements(zstat_img) ne n_elements(mask_img) then begin
        result=dialog_message('Mcomp file and region file are not in the same space. Abort!',/ERROR) 
        return
    endif


    ;for z=0,hdr_zstat.zdim-1 do begin
    ;    zstat = get_image(z,fi,stc,FILNUM=idx_zstat+1)
    ;    if(idx_mask ge 0) then begin
    ;        mask = get_image(z,fi,stc,FILNUM=idx_mask+1)
    ;        ;if or_with_zmap eq 0 then mask = mask or (abs(zstat) ne 0.)
    ;        if or_with_zmap eq 0 then mask = mask or (abs(zstat) gt .9)
    ;    endif else begin
    ;        if(idx_mask eq -1) then $
    ;            mask = abs(zstat) gt .9
    ;        if(idx_mask eq -2) then $
    ;            mask = fltarr(hdr_mask.xdim,hdr_mask.ydim)
    ;        if(idx_mask eq -3) then begin
    ;            mask = fltarr(hdr_mask.xdim,hdr_mask.ydim)
    ;            mask[*,*] = 1
    ;        endif
    ;    endelse
    ;    mask_img[*,*,z] = mask
    ;    zstat_img[*,*,z] = zstat
    ;endfor
    ;START140214
    for z=0,hdr_zstat.zdim-1 do begin
        zstat = get_image(z,fi,stc,FILNUM=idx_zstat+1)
        zstat_img[*,*,z] = zstat
    endfor
    get_mask_img



    zmax = max(zstat_img,MIN=zmin)
    zstat_img_max = zmax
    zstat_img_min = zmin
    
    ;Make sure mask_img is not an activation map.
    stop = !TRUE
    index = where(mask_img ne 0.,count)
    if count gt 0 then begin
        scrap_mask = abs(mask_img[index])
        if total(scrap_mask mod fix(scrap_mask)) > 0. then begin
            result=dialog_message(fi.tails[idx_mask]+' contains noninteger values.'+string(10B)+'Region files should ' $
                +'only contain integer values. Please select "OK" to load a different file.',/CANCEL,/ERROR)
            if result eq 'OK' then $
                stop = !FALSE $
            else begin
                result=dialog_message('Do you know what you are doing?',/QUESTION)
                if result eq 'No' then return
            endelse
        endif 
    endif
endrep until stop eq !TRUE

labels = ['On disk','Exit']
;if fi.space[idx_zstat] eq !SPACE_222 or fi.space[idx_zstat] eq !SPACE_333 then begin
if fi.space[idx_zstat] eq !SPACE_111 or fi.space[idx_zstat] eq !SPACE_222 or fi.space[idx_zstat] eq !SPACE_333 then begin
    get_atlas_labels,atlaslabels,atlasfiles,natlas,THIS_SPACE_ONLY=fi.space[idx_zstat]
    labels = [atlaslabels,labels]
endif else begin
    natlas = 0
endelse
get_filelist_labels,fi,ntails,tails,index
if ntails ne 0 then labels = [tails,labels]
len = natlas + ntails


repeat begin
    idx_anat = get_button(labels,TITLE='Please select anatomy image.')
endrep until labels[idx_anat] ne !BLANK
if idx_anat eq len+1 then $
    return $
else if idx_anat eq len then begin
    repeat begin
        stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,/NOLOAD_COLOR,TITLE='Please select anatomy image.')
        if stat eq !OK then begin
            dsp_image = update_image(fi,dsp,wd,stc,pref)
            idx_anat = fi.n
            scrap = 1
        endif else begin
            print,' *** Error while loading file. ***'
            scrap = get_button(['Try again.','Exit'],TITLE='Error loading anatomy image.')
            if scrap eq 1 then return
        endelse
    endrep until scrap eq 1
endif else if idx_anat ge ntails then begin
    idx_anat = idx_anat - ntails 
    stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,FILENAME=atlasfiles[idx_anat],/NOLOAD_COLOR, $
        TITLE='Please select anatomy image.')
    idx_anat = ntails 
endif
hdr_anat = *fi.hdr_ptr[idx_anat]

name = fi.list[idx_zstat] + '_regions'

widget_control,/HOURGLASS
anat_img = fltarr(hdr_anat.xdim,hdr_anat.ydim,hdr_anat.zdim)
if fi.space[idx_zstat] ne fi.space[idx_anat] then begin
    result=dialog_message('Mcomp file and anatomy file are not in the same space. Abort!',/ERROR)
    return
endif
for z=0,hdr_anat.zdim-1 do begin
    anat = get_image(z,fi,stc,FILNUM=idx_anat+1)
    anat_img[*,*,z] = anat
endfor
if help.enable eq !TRUE then begin
    x = 'Enter name of existing region file.'+'  Select "New" if none exists.'
    widget_control,help.id,SET_VALUE=x
endif

leader = widget_base(title='Define Regions',/COLUMN,GROUP_LEADER=!FMRI_LEADER)
disp_base1 = widget_base(leader,/ROW)
disp_base2 = widget_base(leader,/ROW)
disp_base3 = widget_base(leader,/ROW)
disp_base4 = widget_base(leader,/ROW)
disp_base5 = widget_base(leader,/ROW)
disp_base6 = widget_base(leader,/ROW)
menu_base1 = widget_base(disp_base1,/COLUMN,/EXCLUSIVE)
menu_base2 = widget_base(disp_base1,/COLUMN)
menu_base3 = widget_base(disp_base1,/COLUMN)
menu_base2b = widget_base(menu_base2,/ROW,/NONEXCLUSIVE)
menu_base2b0 = widget_base(menu_base2,/ROW,/NONEXCLUSIVE)
menu_base2b1 = widget_base(menu_base2,/ROW,/EXCLUSIVE)
menu_base2c = widget_base(menu_base2,/ROW)
menu_base2d = widget_base(menu_base2,/ROW)
menu_base3a = widget_base(menu_base3,/ROW,/NONEXCLUSIVE)
menu_base3b = widget_base(menu_base3,/ROW)
menu_base3c = widget_base(menu_base3,/ROW)
menu_base3d = widget_base(menu_base3,/ROW)
menu_base4 = widget_base(disp_base5,/ROW)

;START140214
menu_base4a = widget_base(disp_base5,/COLUMN)
menu_base4b = widget_base(disp_base5,/ROW)

menu_base5 = widget_base(disp_base5,/COLUMN)
menu_base6 = widget_base(disp_base5,/ROW)
menu_base7 = widget_base(disp_base5,/COLUMN)
wd_delete_vox = widget_button(menu_base1,value='Delete voxel/area')
wd_add_vox = widget_button(menu_base1,value='Add voxel/area to mask')
wd_unlabel = widget_button(menu_base1,value='Unlabel voxel/area')
wd_label_vox = widget_button(menu_base1,value='Label voxel/area of mask')
wd_label = widget_button(menu_base1,value='Label contiguous voxels of mask')
wd_respect = widget_button(menu_base2b,value='Respect numerical boundaries',SENSITIVE=0)
wd_thisslice = widget_button(menu_base2b0,value='This slice only',SENSITIVE=0)
wd_rzmap = widget_button(menu_base2b1,value='zmap',SENSITIVE=0)
wd_rmask = widget_button(menu_base2b1,value='mask',SENSITIVE=0)
wd_zminlab = widget_label(menu_base2c,VALUE='         min: ',SENSITIVE=0)
wd_zmaxlab = widget_label(menu_base2d,VALUE='         max: ',SENSITIVE=0)
wd_zmin = widget_text(menu_base2c,VALUE=strtrim(zmin,2),SENSITIVE=0,/EDITABLE,/ALL_EVENTS)
wd_zmax = widget_text(menu_base2d,VALUE=strtrim(zmax,2),SENSITIVE=0,/EDITABLE,/ALL_EVENTS)
wd_bb = widget_button(menu_base3a,value='Bounding box in atlas coor',SENSITIVE=0)
wd_bbx1l = widget_label(menu_base3b,VALUE='x',SENSITIVE=0)
wd_bbx1 = widget_text(menu_base3b,VALUE='0',SENSITIVE=0,/EDITABLE,/ALL_EVENTS,XSIZE=4)
wd_bbx2l = widget_label(menu_base3b,VALUE='x',SENSITIVE=0)
wd_bbx2 = widget_text(menu_base3b,VALUE='0',SENSITIVE=0,/EDITABLE,/ALL_EVENTS,XSIZE=4)
wd_bby1l = widget_label(menu_base3c,VALUE='y',SENSITIVE=0)
wd_bby1 = widget_text(menu_base3c,VALUE='0',SENSITIVE=0,/EDITABLE,/ALL_EVENTS,XSIZE=4)
wd_bby2l = widget_label(menu_base3c,VALUE='y',SENSITIVE=0)
wd_bby2 = widget_text(menu_base3c,VALUE='0',SENSITIVE=0,/EDITABLE,/ALL_EVENTS,XSIZE=4)
wd_bbz1l = widget_label(menu_base3d,VALUE='z',SENSITIVE=0)
wd_bbz1 = widget_text(menu_base3d,VALUE='0',SENSITIVE=0,/EDITABLE,/ALL_EVENTS,XSIZE=4)
wd_bbz2l = widget_label(menu_base3d,VALUE='z',SENSITIVE=0)
wd_bbz2 = widget_text(menu_base3d,VALUE='0',SENSITIVE=0,/EDITABLE,/ALL_EVENTS,XSIZE=4)
wd_sel_label = widget_button(disp_base2,value='Select current label')
wd_cur_label = cw_field(disp_base2,TITLE='Label (editable - type and hit return): ',VALUE='None',XSIZE=25,/RETURN_EVENTS)
wd_nextpln = widget_button(disp_base3,value='Next Plane')
wd_prevpln = widget_button(disp_base3,value='Prev Plane')
wd_plnnum = cw_field(disp_base3,TITLE='Slice: ',VALUE='1',/RETURN_EVENTS,XSIZE=3)
wd_regnum = cw_field(disp_base3,TITLE='Region: ',VALUE='None',/NOEDIT,XSIZE=40)

;wd_coord = cw_field(disp_base4,VALUE='(0,0)',TITLE=' ',/NOEDIT,XSIZE=62)
;START140214
wd_coord = cw_field(disp_base4,VALUE='(0,0)',TITLE=' ',/NOEDIT,XSIZE=75)

wd_overlay = cw_bgroup(menu_base4,['Regions','Z-map'],/EXCLUSIVE,SET_VALUE=0,/COLUMN,/NO_RELEASE)
wd_scaling = cw_bgroup(menu_base4,['Local','Global'],/EXCLUSIVE,SET_VALUE=0,/COLUMN,/NO_RELEASE,LABEL_TOP='Z-map scaling')

;START140214
wd_thresh = cw_field(menu_base4a,TITLE='Threshold: ',VALUE=trim(thresh),XSIZE=5,/RETURN_EVENTS)
wd_gt_ge = cw_bgroup(menu_base4a,['>','>='],/EXCLUSIVE,SET_VALUE=0,/COLUMN,/NO_RELEASE)

;wd_reslice = cw_bgroup(menu_base4,['Transverse','Sagittal','Coronal'],/EXCLUSIVE,SET_VALUE=0,/COLUMN,/NO_RELEASE)
;START140214
wd_reslice = cw_bgroup(menu_base4b,['Transverse','Sagittal','Coronal'],/EXCLUSIVE,SET_VALUE=0,/COLUMN,/NO_RELEASE)
wd_showcolors = cw_bgroup(menu_base4b,['Show color scale','Do not show'],/EXCLUSIVE,SET_VALUE=1,/COLUMN,/NO_RELEASE)


wd_zoomin = widget_button(menu_base5,value='Zoom in')
wd_zoomout = widget_button(menu_base5,value='Zoom out')
wd_refresh = widget_button(menu_base5,value='Refresh')

;START140214
;wd_showcolors = cw_bgroup(menu_base4,['Show color scale','Do not show'],/EXCLUSIVE,SET_VALUE=1,/COLUMN,/NO_RELEASE)

wd_exit = widget_button(menu_base7,value='Exit',FRAME=5)
wd_save = widget_button(disp_base6,value='Save',FRAME=5)
wd_savename = cw_field(disp_base6,TITLE='Filename: ',VALUE=name,XSIZE=75,/ALL)
lc_showcolors = 1 

;print,'wd_delete_vox=',wd_delete_vox
;print,'wd_add_vox=',wd_add_vox
;print,'wd_unlabel=',wd_unlabel
;print,'wd_label_vox=',wd_label_vox
;print,'wd_label=',wd_label
;print,'wd_sel_label=',wd_sel_label
;print,'wd_cur_label=',wd_cur_label
;print,'wd_nextpln=',wd_nextpln
;print,'wd_prevpln=',wd_prevpln
;print,'wd_plnnum=',wd_plnnum
;print,'wd_regnum=',wd_regnum
;print,'wd_coord=',wd_coord
;print,'wd_overlay=',wd_overlay
;print,'wd_reslice=',wd_reslice
;print,'wd_zoomin=',wd_zoomin
;print,'wd_zoomout=',wd_zoomout
;print,'wd_refresh=',wd_refresh
;print,'wd_exit=',wd_exit



xdim = hdr_anat.xdim
ydim = hdr_anat.ydim
zdim = hdr_anat.zdim
if xdim eq 64 and ydim eq 64 then $
    zoom = 4 $
else $
    zoom = 2

wd_drawbase = widget_base(TITLE="Define Regions",/COLUMN,GROUP_LEADER=leader)
wd_draw = widget_draw(wd_drawbase,/BUTTON_EVENTS,/MOTION_EVENTS,COLORS=!LEN_COLORTAB1, $
            XSIZE=xdim,YSIZE=ydim,RETAIN=2,KILL_NOTIFY=string('edit_mask_dead'))

ifh = hdr_mask.ifh
region_names = strarr(1000)
if(ptr_valid(ifh.region_names)) then begin
    nreg = n_elements(*ifh.region_names)
    region_names_tmp = *ifh.region_names
    region_names[0:nreg-1] = region_names_tmp[0:nreg-1]
endif else begin
    nreg = 0
endelse


widget_control,/HOURGLASS

xd = xdim
yd = ydim
zd = zdim

if hdr_anat.xdim ne hdr_zstat.xdim then begin
    print,'WARNING : Anatomy and statistical image have different dimensions.'
    stat = widget_message('Anatomy and statistical image have different dimensions. Perhaps the statistical image ' $
           +'needs to be transformed to atlas space.',/ERROR)
endif

if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='"Bilinear" is the normal interpolations mode.' $
    +'  Only select "Nearest neighbor" if you are working with very small images (say 10x10)'

interp = get_button(['Bilinear','Nearest neighbor'],TITLE='Interpolation method')
if interp eq 0 then $
    interp = !BILINEAR $
else $
    interp = !NEAREST_NEIGHBOR

if(help.enable eq !TRUE) then begin
    x = '"Delete voxels:" Voxels can be deleted from the file by first clicking "Delete voxels" and then either clicking on a discrete clump of voxel in the mask (which will remove the entire clump),' $
+' or by depressing the leftmost mouse button and dragging the cursor.' $
+' This will draw a box on the image.' $
+'  All voxels in the box will be deleted.'
    widget_control,help.id,SET_VALUE=x
    widget_control,help.id,/APPEND,SET_VALUE='"Delete voxel/area:" Selecting this button will either delete a single voxel if the leftmost mouse button is' $
+' clicked over a region or it will delete a group of voxels if a box is formed by dragging the cursor.'
    widget_control,help.id,/APPEND,SET_VALUE='"Add voxel/area" works the same way as "Delete voxel/area" except that selected voxels are added to the mask'
    widget_control,help.id,/APPEND,SET_VALUE='"Unlabel voxel/area:" This button will unlabel voxels under a box created by clicking the leftmost mouse button and possible dragging the cursor.'
    widget_control,help.id,/APPEND,SET_VALUE='"Label voxel/area:"  Click this button and then click on an individual voxel (and possibly drag to define a box to label a voxel or a region.'
    widget_control,help.id,/APPEND,SET_VALUE='"Label contiguous voxels:"  Select this button and then click on a contiguous region to label all voxels in that region with the current label.'
    widget_control,help.id,/APPEND,SET_VALUE='"Select current label:"  A button box appears asking for the name of the region.' +'  Select "New" if you want to define a new one.'+'  The name of the selected label will appear in the box below the button and will be assigned to voxels subsequently selected with either of the "Label" buttons.'
    widget_control,help.id,/APPEND,SET_VALUE='"Highlight region:"  To see what voxels are assigned to a given region name, select this option and specifiy the region name.'+'  All voxels in that region will be highlighted in green.'
    widget_control,help.id,/APPEND,SET_VALUE='"Refresh:" Redraw the screen.'
    widget_control,help.id,/APPEND,SET_VALUE='"Regions:" Overlay regions and potential regions on the image.'
    widget_control,help.id,/APPEND,SET_VALUE='"Z-Map:" Display z-statistics overlaid on the anatomy image.'
    widget_control,help.id,/APPEND,SET_VALUE='Placing the cursor over a region will display its name in the box labeled "Region".'
    widget_control,help.id,/APPEND,SET_VALUE='"Exit" will exit this program and save the region mask in the file list.' $
+'  This file can be saved as a 4dfp stack and retrieved for later editing or application.'
endif


;This was added to stop the orange screen, but it doesn't seem to work.
;index = where(zstat_img eq !UNSAMPLED_VOXEL,count)
;if count ne 0 then zstat_img[index] = 0

val = 2

widget_control,leader,/REALIZE
xmanager,'define_regions',leader

widget_control,GET_VALUE=i,wd_draw
widget_control,wd_rzmap,/SET_BUTTON
rzmap = 1
mask_window = i
if(mask_window gt 0) then wset,mask_window

;print,'define_regions here1 fi.current_colors=',fi.current_colors
;fi.color_scale1[fi.current_colors] = colortable1_regions
;fi.color_scale2[fi.current_colors] = colortable2_regions
refresh_display_mask_new,fi,wd,dsp,help,stc
widget_control,wd_plnnum,SET_VALUE=string(plane,FORMAT='(i2)')

end
