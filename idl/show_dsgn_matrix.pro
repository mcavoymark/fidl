;Copyright 12/31/99 Washington University.  All Rights Reserved.
;show_dsgn_matrix.pro  $Revision: 12.92 $

;START150331
;pro show_dsgn_matrix,fi,dsp,wd,glm,help,cstm
;    if fi.nfiles eq 0 then wd1_load_linmod,fi,dsp,wd,glm,help
;    if fi.nfiles eq 0 then return
;    hdr = *fi.hdr_ptr[fi.n]
;    if hdr.array_type eq !LINEAR_MODEL then begin
;        show_dsgn_matrix_guts,glm[fi.n],cstm,fi.tails[fi.n]
;    endif else begin
;        hd1 = *fi.hdr_ptr(hdr.model)
;        if ptr_valid(glm[hd1.model].A) then $
;            show_dsgn_matrix_guts,glm(hdr.model),cstm,fi.tails[fi.n] $
;        else $
;            stat = dialog_message('Linear model is not defined for this file.')
;    endelse
;end
pro show_dsgn_matrix_guts,glm,cstm,name
    image_index = !D.WINDOW
    if not ptr_valid(glm.A) then begin
        stat=dialog_message('*** Design matrix has not been defined.  ***')
        return
    endif
    widget_control,/HOURGLASS
    A = *glm.A
    nc = glm.nc
    if nc gt 0 then begin 
        c_in = *glm.c
        contrast_labels = *glm.contrast_labels
    endif
    effect_label = *glm.effect_label
    if nc eq 0 then nc = 1
    effect_length = *glm.effect_length
    all_eff = glm.all_eff
    label_size = 110
    N = n_elements(A(0,*))
    tcomp = n_elements(A(*,0))
    dw = 15
    ydim = 2*tcomp
    image = fltarr(dw*N,ydim)
    for j=0,N-1 do begin
        max = max(abs(A[*,j]))
        for i=0,2*tcomp-1,2 do if max ne 0. then image[j*dw:(j+1)*dw-1,i:i+1] = A(i/2,j)/max
    endfor
    max = max(image,MIN=min)
    image = (image-min)*(!LEN_COLORTAB1-1)/(max - min)
    c = fltarr(N,nc)
    if glm.nc ne 0 then for ic=0,nc-1 do c[*,ic] = c_in[*,ic]
    dc = fltarr(dw*N,nc)
    hc = 51
    hc1 = hc/2-5
    cimg = fltarr(dw*N,hc,nc)
    cmin = float(min(c(*,*)))
    cmax = float(max(c(*,*)))
    if(abs(cmin) gt abs(cmax)) then $
        scl = hc1/abs(cmin) $
    else $
        scl = hc1/abs(cmax)
    for ic=0,nc-1 do begin
        for i=0,N-1 do $
            dc(i*dw:(i+1)*dw-1,ic) = c(i,ic)
        for i=0,N-1 do begin
            if(c(i,ic) gt 0) then begin
                if(cmax gt 0) then $
                    h = hc/2 + fix(scl*c(i,ic)) $
    	        else $
                    h = hc/2
	        if(h-1 lt hc/2) then $
		    h = hc/2 + 1
    ;;;print,'B ',i*dw,(i+1)*dw-1,hc/2,h-1,ic
                cimg(i*dw:(i+1)*dw-1,hc/2:h-1,ic)  = 1
            endif else begin
                if(cmin lt 0.) then $
                    h = hc/2 + fix(scl*c(i,ic)) $
    	        else $
                    h = hc/2
    ;;;print,'A ',i*dw,(i+1)*dw-1,h,hc/2,ic
                cimg(i*dw:(i+1)*dw-1,h:hc/2,ic) = 1
            endelse
            cimg(*,hc/2,ic) = 1
        endfor
    endfor
    cimg = cimg*(!LEN_COLORTAB1-1)
    cstm.cur_window = cstm.nwindows
    cstm.nwindows = cstm.nwindows + 1
    title = string(cstm.cur_window,name,FORMAT='("Custom window ",i3,": ",a)')
    dsgn_base = widget_base(/COLUMN,TITLE=title)
    dsgn = widget_base(dsgn_base,/COLUMN)
    ;;;if(dw*N gt 900) then $
    ;;;    wlabels = widget_draw(dsgn,XSIZE=900,YSIZE=label_size,RETAIN=2) $
    ;;;else $
    ;;;    wlabels = widget_draw(dsgn,XSIZE=dw*N,YSIZE=label_size,RETAIN=2)
    wcontrast = lonarr(nc)
    xsz = dw*N
    ysz = ydim + nc*hc + label_size
    wdsgn = widget_draw(dsgn,XSIZE=xsz,YSIZE=ysz,RETAIN=2,X_SCROLL_SIZE=!DISPXMAX<xsz,Y_SCROLL_SIZE=!DISPYMAX<ysz, $
        KILL_NOTIFY='custom_dead')
    widget_control,dsgn_base,/REALIZE
    widget_control,GET_VALUE=dsgn_id,wdsgn
    cstm.id(cstm.cur_window) = dsgn_id
    cstm.draw(cstm.cur_window) = wdsgn
    if glm.nc ne 0 then begin
        for ic=0,nc-1 do begin
            widget_control,GET_VALUE=dsgn_window,wdsgn
            wset,dsgn_window
            ypos = ydim + (nc-ic-1)*hc
            tv,cimg(*,*,ic),0,ypos
            tag = strcompress(string(ic+1,FORMAT='("C",i2,":")'),/REMOVE_ALL) + ' ' + contrast_labels[ic]
            xyouts,dw*N,ypos,tag,/DEVICE,COLOR=!YELLOW,CHARSIZE=1.1,ALIGNMENT=1.0
        endfor
    endif
    widget_control,GET_VALUE=dsgn_window,wdsgn
    wset,dsgn_window
    tv,image,/ORDER
    spc = 0.5
    ypos = ydim + nc*hc
    cimg = fltarr(xsz)
    cimg[*] = (!LEN_COLORTAB1 - 1)
    tv,cimg,0,ypos
    ypos = ypos + 5
    for i=0,all_eff-1 do begin
        spc = spc + float(effect_length[i])/2.
        xyouts,spc*dw,ypos,effect_label[i],ORIENTATION=90,/DEVICE,COLOR=!YELLOW,CHARSIZE=1.1
        spc = spc + float(effect_length[i])/2.
    endfor
    wset,image_index
    widget_control,HOURGLASS=0
end
;pro show_dsgn_matrix,fi,dsp,wd,glm,help,cstm
;    if fi.nfiles eq 0 then wd1_load_linmod,fi,dsp,wd,glm,help
;    if fi.nfiles eq 0 then return
;    hdr = *fi.hdr_ptr[fi.n]
;    if hdr.array_type eq !LINEAR_MODEL then begin
;        show_dsgn_matrix_guts,glm[fi.n],cstm,fi.tails[fi.n]
;    endif else begin
;        hd1 = *fi.hdr_ptr[hdr.model]
;        if ptr_valid(glm[hd1.model].A) then $
;            show_dsgn_matrix_guts,glm[hdr.model],cstm,fi.tails[fi.n] $
;        else $
;            stat = dialog_message('Linear model is not defined for this file.')
;    endelse
;end
pro show_dsgn_matrix,fi,dsp,wd,glm,help,cstm
    if fi.nfiles eq 0 then wd1_load_linmod,fi,dsp,wd,glm,help
    if fi.nfiles eq 0 then return
    hdr = *fi.hdr_ptr[fi.n]
    if hdr.array_type eq !LINEAR_MODEL then begin
        glm0=glm[fi.n]
        tail=fi.tails[fi.n]
    endif else begin
        hd1 = *fi.hdr_ptr[hdr.model]
        if ptr_valid(glm[hd1.model].A) then begin 
            glm0=glm[hdr.model]
            tail=fi.tails[hdr.model]
        endif else begin
            stat = dialog_message('Linear model is not defined for this file.')
            return
        endelse
    endelse
    show_dsgn_matrix_guts,glm0,cstm,tail
end

