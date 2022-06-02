;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_time_prof.pro  $Revision: 12.123 $

;*******************
pro get_time_prof,ev
;*******************

;print,'here get_time_prof top'

common profiles,x1,x2,y1,y2,x1m1,x2m1,y1m1,y2m1,row,col,profdat,profmask

common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg, $
       glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

common linear_fit,fit_name,slope,intcpt,trend

already_averaged = !FALSE

if(help.enable eq !TRUE) then begin
    value='Left button: Select row, Middle button: Exit profile mode, Right button: Plot profile.'$
    +'  After the profile is plotted, the pixels used in the calculation are outlined in green.'$
    +'  Since they are converted to image coordinates from the zoomed display image, they will be shifted slightly from those specified.'$
    +'  The mean value of the profile is subtracted from each point prior to plotting it.'
    widget_control,help.id,SET_VALUE=value
endif

;;;print,'get_time_prof, ev.press: ',ev.press
if(ev.press eq 4) then $
    pr.mode = !DRAW_DONE




;; Abort time profile.
;if(ev.press eq 2) then begin
;    ;pr.mode = !DRAW_OFF
;    ;pr.prof_on = !PROF_NO
;    ;device,SET_GRAPHICS=3,/cursor_crosshair ;Resume normal graphics.
;    ;;widget_control,wd.draw[fi.cw],DRAW_MOTION_EVENTS=0
;    ;widget_control,wd.draw[fi.cw],/DRAW_MOTION_EVENTS
;    ;widget_control,wd.draw[fi.cw],EVENT_PRO='stats_exec_event'
;    abort_profile_mode,pr,wd,fi
;    return
;endif
;zoom = fix(dsp[fi.cw].zoom)
;hdr = *fi.hdr_ptr(dsp[fi.cw].file_num)
;mom = hdr.mother
;hd = *fi.hdr_ptr(mom)
;if((hd.tdim lt 2) and (pr.prof_on ne !PROF_REG)) then begin
;    stat = dialog_message('Only one frame in series.',/ERROR)
;    ;pr.mode = !DRAW_OFF
;    ;widget_control,wd.draw[fi.cw],DRAW_MOTION_EVENTS=0
;    ;widget_control,wd.draw[fi.cw],EVENT_PRO='stats_exec_event'
;    abort_profile_mode,pr,wd,fi
;    return
;endif

if ev.press eq 2 then begin
    abort_profile_mode,pr,wd,fi
    return
endif
zoom = fix(dsp[fi.cw].zoom)
hdr = *fi.hdr_ptr(dsp[fi.cw].file_num)
mom = hdr.mother
hd = *fi.hdr_ptr[mom]
if hd.tdim lt 2 and pr.prof_on ne !PROF_REG then begin
    stat = dialog_message('Only one frame in series.',/ERROR)
    abort_profile_mode,pr,wd,fi
    return
endif





if(ptr_valid(fi.paradigm(hdr.mother))) then begin
    paradigm = *fi.paradigm(hdr.mother)
endif else begin
    paradigm = fltarr(hd.tdim)
    paradigm[*] = 1
    if(hd.tdim gt pr.frames_to_skip) then begin
        if(pr.frames_to_skip gt 0) then $
            paradigm[0:pr.frames_to_skip-1.] = 0
    endif
endelse
if(total(paradigm) eq 0.) then $
    paradigm[*] = 1.

if(pr.regional eq !TRUE) then begin
    height = 1
    width = 1
    zoom1 = fix(dsp[fi.cw].zoom*hdr.xdim/hd.xdim)
endif else begin
    case pr.time_type of
        !GANG_TPROF: begin
            height = fix(pr.gang_dim*pr.gang_subdim)
            width = height
        zoom1 = zoom
        end
        else: begin
            height = fix(pr.height)
            width = fix(pr.width)
        zoom1 = zoom
        end
    endcase
endelse
if(height gt hd.ydim) then $
    height = hd.ydim
if(width gt hd.xdim) then $
    width = fix(hd.xdim)

case pr.mode of

!DRAW_OFF: begin
    widget_control,wd.draw[fi.cw],/DRAW_MOTION_EVENTS
    xxx = intarr(16)
    if(!VERSION.OS_FAMILY eq 'Windows') then $
        device,set_graphics=6,/CURSOR_CROSSHAIR $
    else $
        device,set_graphics=6,cursor_image=xxx
    widget_control,wd.error,SET_VALUE=string('Left: Select row, Right: Plot profile, Center: exit.')
    y = ev.y
    x = ev.x
    row = fix((dsp[fi.cw].dsizey - y -1)/zoom)
    y1 = dsp[fi.cw].dsizey - 1 - (zoom*(row - height/2) - zoom/2)
    y2 = dsp[fi.cw].dsizey - 1 - (zoom*(row + height/2) + zoom/2)
    col = x/zoom
    x1 = zoom*(col - width/2) - zoom/2
    x2 = zoom*(col + width/2) + zoom/2
    plots,[x1,x1,x2,x2,x1],[y1,y2,y2,y1,y1],COLOR=dsp[fi.cw].green,/DEV,/NOCLIP
    x1m1 = x1
    x2m1 = x2
    y1m1 = y1
    y2m1 = y2
    pr.mode = !DRAWING
end

!DRAWING: begin
    x = ev.x
    y = ev.y
    row = ( dsp[fi.cw].dsizey - y -1)/zoom1
    y1 = dsp[fi.cw].dsizey - 1 - (zoom1*(row + height/2)) - zoom1
    y2 = dsp[fi.cw].dsizey - 1 - (zoom1*(row - height/2))
    col = x/zoom1
    x1 = zoom1*(col - width/2)
    x2 = zoom1*(col + width/2) + zoom1
    if(dsp[fi.cw].orientation eq !NEURO) then begin
        col = hdr.xdim - 1 - col
        xx = dsp[fi.cw].dsizex-ev.x-1
    endif else $
        xx = ev.x
    if(x1 eq x2) then x2 = x1 + 1
    if(y1 eq y2) then y2 = y1 + 1;
    if(y1 ne y1m1) or (y2 ne y2m1) or (x1 ne x1m1) or (x2 ne x2m1) then begin
        plots,[x1m1,x2m1,x2m1,x1m1,x1m1],[y1m1,y1m1,y2m1,y2m1,y1m1],COLOR=dsp[fi.cw].green,/DEV,/NOCLIP
        plots,[x1,x1,x2,x2,x1],[y1,y2,y2,y1,y1],COLOR=dsp[fi.cw].green,/DEV,/NOCLIP
    endif
    if(x1 ne x1m1) or (x2 ne x2m1) or (y1 ne y1m1) or (y2 ne y2m1) then begin
        col1 = col
        str = strcompress(string("(",col1,",",row,")"),/REMOVE_ALL)
        v = get_atlas_coords(xx,dsp[fi.cw].dsizey-ev.y-1,dsp[fi.cw].plane-1,0,0,fi.view[fi.n],fi.space[mom],0,0,dsp[fi.cw].zoom,!FALSE,!RADIOL,dsp[fi.cw].xdim,dsp[fi.cw].ydim) 
        str = str+' '+strcompress(string(v[0],v[1],v[2],FORMAT='(" Atlas: (",i4,",",i4,",",i4,")")'),/REMOVE_ALL)
        widget_control,wd.error,SET_VALUE=str
    endif
    x1m1 = x1
    x2m1 = x2
    y1m1 = y1
    y2m1 = y2
end

!DRAW_DONE: begin
   pr.mode = !DRAW_OFF
   xdim = dsp[fi.cw].xdim
   ydim = dsp[fi.cw].ydim
   zdim = dsp[fi.cw].zdim
   xydim = xdim*ydim
   xyzdim = xydim*zdim
   plane = dsp[fi.cw].plane
   if(pr.subtract_trend eq !TRUE) then  begin
       if(not ptr_valid(trend.slope_ptr(hdr.mother))) then $
           stat = widget_message('Linear trend has not been computed.  Select the Process|Fit Linear Trend option.')
    endif
    plots,[x1m1,x1m1],[y1m1,y2m1],COLOR=dsp[fi.cw].green,/DEV,/NOCLIP
    if(width gt 1) then $
        plots,[x2m1,x2m1],[y1m1,y2m1],COLOR=dsp[fi.cw].green,/DEV,/NOCLIP
    plots,[x1m1,x2m1],[y1m1,y1m1],COLOR= !d.table_size-1,/DEV,/NOCLIP
    if(height gt 1) then $
         plots,[x1m1,x2m1],[y2m1,y2m1],COLOR= !d.table_size-1,/DEV,/NOCLIP
    device,SET_GRAPHICS=3,/cursor_crosshair ;Resume normal graphics.
    x1 = zoom1*(col - width/2)
    x2 = zoom1*(col + width/2) + zoom1
    y1 = dsp[fi.cw].dsizey - 1 - (zoom1*(row + height/2)) - zoom1
    y2 = dsp[fi.cw].dsizey - 1 - (zoom1*(row - height/2))
    if(x1 eq x2) then x2 = x1 + 1
    if(y1 eq y2) then y2 = y1 + 1;
    if(pr.time_type ne !GANG_TPROF and pr.time_type ne !THRESH_TPROF) then $
        rect_plot,x1,x2,y1,y2,zoom1,COLOR=pr.color,ORIENTATION=dsp[fi.cw].orientation
    x1 = col - width/2
    x2 = col + width/2
    y1 = row - height/2
    y2 = row + height/2
    if(x1 lt 0) then x1 = 0
    if(x2 gt xdim-1) then x2 = xdim-1
    if(y1 lt 0) then y1 = 0
    if(y2 gt ydim-1) then y2 = ydim-1
    pr.coord(0) = row
    pr.coord(1) = col
    statimg = get_image(zdim*(dsp[fi.cw].frame-1) + plane-1,fi,stc,FILNUM=dsp[fi.cw].file_num+1)

;   Plot boxes around voxels or regions used in profiles.
    if(pr.regional eq !FALSE) then begin
        case pr.time_type of
        !THRESH_TPROF: begin
            prof = fltarr(hd.tdim)
            if(pr.time_thresh_type eq !PERCENT) then begin
                if(pr.time_thresh gt 0) then begin
                    max = max(statimg(x1:x2,y1:y2))
                    thresh = pr.time_thresh*max
                    mask = statimg gt thresh
                endif else begin
                    min = min(statimg(x1:x2,y1:y2))
                    thresh = pr.time_thresh*abs(min)
                    mask = statimg lt thresh
                endelse
            endif else begin  ; Threshold based on log significance prob
                if(pr.time_thresh gt 0) then $
                    mask = statimg gt pr.time_thresh $
                else $
                    mask = statimg lt pr.time_thresh
            endelse
;           Zero mask voxels outside of box.
            if(x1 gt 0) then $
                mask(0:x1-1,*) = 0
            if(x2+1 le xdim-1) then $
                mask(x2+1:xdim-1,*) = 0
            if(y1 gt 0) then $
                mask(*,0:y1-1) = 0
            if(y2+1 lt xdim-1) then $
                mask(*,y2+1:ydim-1) = 0
            widget_control,wd.error,SET_VALUE=strcompress(string(total(mask)," voxels selected"),/REMOVE_ALL)
            print,strcompress(string(total(mask)," voxels selected"))
            if(total(mask) eq 0) then begin
                stat=widget_message('No voxels meet threshold criterion',/ERROR)
                pr.mode = !DRAW_OFF
                pr.time_type = pr.last_type
                device,SET_GRAPHICS=3,/cursor_crosshair ;Resume normal graphics.
                widget_control,wd.draw[fi.cw],DRAW_MOTION_EVENTS=0
                widget_control,wd.draw[fi.cw],EVENT_PRO='stats_exec_event'
                return
            endif
            indices_slc = where(mask,nind)
            indices_vol = indices_slc + xydim*(plane-1)
            for i=0,nind-1 do begin
                y = indices_slc[i]/xdim
                x = indices_slc[i] - long(y)*long(xdim)
                xx = x*zoom
                if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then begin
                    yy = dsp[fi.cw].dsizey - 1 - long(y+1)*long(zoom)
                    rect_plot,xx,xx+zoom,yy,yy+zoom,zoom,COLOR=pr.color, $
                    ORIENTATION=dsp[fi.cw].orientation
                endif else begin
                    yy = dsp[fi.cw].dsizey - 1 - long(y)*long(zoom)
                    side = zoom/2
                    rect_plot,xx-side,xx+side-1,yy-side,yy+side-1,zoom,COLOR=pr.color, $
                    ORIENTATION=dsp[fi.cw].orientation
                endelse
             endfor
          end
          !GANG_TPROF: begin
              mask = abs(statimg) gt 0
              x = fix(x1+x2)/2
              y = fix(y1+y2)/2
              if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then begin
                  xx1 = zoom*x
                  xx2 = zoom*(x + float(pr.gang_dim*pr.gang_subdim))
                  yy2 = y - 1
                  yy1 = y + float(pr.gang_dim*pr.gang_subdim) - 1
                  xx1 = zoom*(x - fix(pr.gang_dim*pr.gang_subdim)/2)
                  xx2 = zoom*(x + fix(pr.gang_dim*pr.gang_subdim)/2 + 1)
                  yy2 = y - fix(pr.gang_dim*pr.gang_subdim)/2
                  yy1 = y + fix(pr.gang_dim*pr.gang_subdim)/2 + 1
              endif else begin
                  xx1 = zoom*(x - float(pr.gang_dim*pr.gang_subdim)/2)
                  xx2 = zoom*(x + float(pr.gang_dim*pr.gang_subdim)/2)
                  yy2 = y - float(pr.gang_dim*pr.gang_subdim)/2
                  yy1 = y + float(pr.gang_dim*pr.gang_subdim)/2
              endelse
              yy2 = dsp[fi.cw].dsizey - 1 - long(yy2)*long(zoom)
              yy1 = dsp[fi.cw].dsizey - 1 - long(yy1)*long(zoom)
              rect_plot,xx1,xx2,yy1,yy2,zoom,COLOR=pr.color, $
                      ORIENTATION=dsp[fi.cw].orientation
              yx = yy1 + pr.gang_subdim*zoom
              for i=0,pr.gang_dim-2 do begin
                 plots,[xx1,xx2],[yx,yx],/DEV,/NOCLIP,COLOR=pr.color
                 yx = yx + pr.gang_subdim*zoom
              endfor
              xx = xx1 + pr.gang_subdim*zoom
              for i=0,pr.gang_dim-2 do begin
                 plots,[xx,xx],[yy1,yy2],/DEV,/NOCLIP,COLOR=pr.color
                 xx = xx + pr.gang_subdim*zoom
              endfor
          end
          !PLAIN_TPROF: begin
              prof = fltarr(hd.tdim)
              mask = fltarr(xdim,ydim)
              if((x2 ge x1) and (y2 ge y1)) then begin
                  mask(x1:x2,y1:y2) = 1
                  indices_slc = where(mask,nind)
                  indices_vol = indices_slc + xydim*(plane-1)
              endif else begin
                  x1 = 0
                  x2 = 0
                  y1 = 0
                  y2 = 0
                  widget_control,wd.error,SET_VALUE='Invalid mask in get_time_prof'
                  beep
              endelse
          end
         !EXT_THRESH_TPROF: begin
             prof = fltarr(hd.tdim)
             mask = fltarr(xdim,ydim)
             mask = profmask(*,*,plane-1)
             mask(0:x1-1,*) = 0
             mask(x2+1:xdim-1,*) = 0
             mask(*,0:y1-1) = 0
             mask(*,y2+1:ydim-1) = 0
             indices_slc = where(mask,nind) 
             indices_vol = indices_slc + xydim*(plane-1)
             for i=0,nind-1 do begin
                 y = indices_slc(i)/xdim
                 x = indices_slc(i) - long(y)*long(xdim)
                 xx = x*zoom
                 yy = dsp[fi.cw].dsizey - 1 - long(y)*long(zoom)
                 side = zoom/2
                 rect_plot,xx-side,xx+side-1,yy-side,yy+side-1,zoom,COLOR=pr.color, $
                     ORIENTATION=dsp[fi.cw].orientation
             endfor
         end
         !HISTO_TPROF:
         else: print,'Invalid value of pr.time_type.'
        endcase
    endif else begin
;        Average over a region.
            prof = fltarr(hd.tdim)
            idx_mc = dsp[fi.cw].file_num
            image = fltarr(xdim,ydim,zdim)
            for z=0,zdim-1 do $
                image(*,*,z) = get_image(z,fi,stc,FILNUM=dsp[fi.cw].file_num+1)
            cursor_val =image[long(col+.5),long(row+.5),plane-1]
            indices_vol = where(image eq cursor_val,nind)
            indices_slc = where(statimg eq cursor_val,nind_slc)
            for i=0,nind_slc-1 do begin
                y = indices_slc(i)/xdim
                x = indices_slc(i) - long(y)*long(xdim)
                xx = x*zoom
                if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then begin
                    yy = dsp[fi.cw].dsizey - 1 - long(y+1)*long(zoom)
                    rect_plot,xx,xx+zoom,yy,yy+zoom,zoom,COLOR=pr.color, $
                    ORIENTATION=dsp[fi.cw].orientation
                endif else begin
                    yy = dsp[fi.cw].dsizey - 1 - long(y)*long(zoom)
                    side = zoom/2
                    rect_plot,xx-side,xx+side-1,yy-side,yy+side-1,zoom,COLOR=pr.color, $
                    ORIENTATION=dsp[fi.cw].orientation
                endelse
             endfor
    endelse


    case pr.time_type of
    !GANG_TPROF: begin
        prof = fltarr(pr.gang_dim^2,hd.tdim)
        gang_titles_made = !FALSE
        gang_titles = strarr(pr.gang_dim,pr.gang_dim)
    end
    !HISTO_TPROF: begin
        mask = fltarr(xdim,ydim)
        mask(x1:x2,y1:y2) = 1
        indices_slc = where(mask,nind)
        indices_vol = indices_slc + xydim
        prof = fltarr(long(x2-x1+1)*long(y2-y1+1),hd.tdim)
        n_histo = long(x2-x1+1)*long(y2-y1+1)*long(hd.tdim)
        n_pix = long(x2-x1+1)*long(y2-y1+1)
    end
    else: begin
            prof = fltarr(hd.tdim)
    end
    endcase

;   Get rid of discarded frames.
    lenprof = n_elements(prof)
    pmask = fltarr(hd.tdim)
    if(fi.paradigm_loaded(hdr.mother) eq !TRUE  and  $
        pr.time_type ne !HISTO_TPROF and $
        hd.tdim eq n_elements(paradigm)) then begin
        pmask(0:hd.tdim-1) = abs(paradigm[0:hd.tdim-1])
    endif

    if(((hd.tdim gt 1) or (pr.time_type eq !HISTO_TPROF)) and (pr.roiprof eq !FALSE)) then begin
      if(x1 lt 0) then x1 = 0
      if(x2 gt hdr.xdim-1) then x2 = hdr.xdim-1
      if(y1 lt 0) then y1 = 0
      if(y2 gt hdr.ydim-1) then y2 = hdr.ydim-1
      t1 = 0
      if(hd.array_type eq !STITCH_ARRAY) then begin
          t_to_file = *stc(mom).t_to_file
          num_file = stc(mom).n
      endif else begin
          t_to_file = fltarr(hd.tdim)
          num_file = 1
      endelse
      ifilem1 = -1
      for t=0,hd.tdim-1 do begin
        ifile = t_to_file[t]
        if(ifile ne ifilem1) then $
            t_file = 0
        ifilem1 = ifile
        if((pr.time_type ne !GANG_TPROF) and (pr.time_type ne !HISTO_TPROF)) then begin
            if((t_file lt pr.frames_to_skip) and (hd.tdim gt 32) and $
               (pr.scatter_plot eq !FALSE)) then begin
;               Zero first four frames of each run.
                prof[t] = 0.
                pmask[t] = 0.
            endif else begin
                prof[t] = get_regional_avg(indices_vol+xyzdim*t,fi,stc,FILNUM=mom+1)
                pmask[t] = 1.
                already_averaged = !TRUE
            endelse
        endif

        case pr.time_type of
        !THRESH_TPROF: begin
           if(t eq 0) then begin
                val=string(nind,FORMAT='("Number of voxels: ",i3)')
                widget_control,wd.error,SET_VALUE=val
           endif
        end
        !PLAIN_TPROF:
        !EXT_THRESH_TPROF:
        !HISTO_TPROF: begin
            image = get_image(t*zdim+plane-1,fi,stc,FILNUM=mom+1)
            if(paradigm(t) ne 0 or hdr.tdim eq 1) then begin
                prof[*,t1] = image[indices_slc]
                t1 = t1 + 1
            endif
        end
        !GANG_TPROF: begin
            image = get_image(t*zdim+plane-1,fi,stc,FILNUM=mom+1)
            x = (x1 + x2)/2
            y = (y1 + y2)/2
            gang_title = strcompress(string(x,y,plane-1,FORMAT='("(",i3,",",i3,",",i3,") ",$)'),/REMOVE_ALL)
            if(x+(pr.gang_dim*pr.gang_subdim)/2 gt hdr.xdim-1) then $
                x = hdr.xdim -1 - (pr.gang_dim*pr.gang_subdim)/2
            if(x-(pr.gang_dim*pr.gang_subdim)/2 lt 0) then $
                x = (pr.gang_dim*pr.gang_subdim)/2
            if(y+(pr.gang_dim*pr.gang_subdim)/2 gt hdr.ydim-1) then $
                y = hdr.ydim -1 - (pr.gang_dim*pr.gang_subdim)/2
            if(y-(pr.gang_dim*pr.gang_subdim)/2 lt 0) then $
                y = (pr.gang_dim*pr.gang_subdim)/2
            y0 = y - (pr.gang_dim*pr.gang_subdim)/2
            k = 0
            for j=0,pr.gang_dim-1 do begin
                x0 = x - fix(pr.gang_dim*pr.gang_subdim)/2
                for ii=0,pr.gang_dim-1 do begin
                    if(gang_titles_made eq !FALSE) then begin
                        if(dsp[fi.cw].orientation eq !NEURO) then $
                            gang_titles[pr.gang_dim-1-ii,j] = strcompress(string(x0,y0, $
                                          FORMAT='("(",i3,",",i3,") ",$)'),/REMOVE_ALL) $
                        else $
                            gang_titles[k] = string(x0,y0,FORMAT='("(",i3,",",i3,") ",$)')
                        k = k + 1
                    endif
                    if(dsp[fi.cw].orientation eq !NEURO) then $
                        iii = pr.gang_dim - 1 - ii $
                    else $
                        iii = ii
                    val = total(image(x0:x0+pr.gang_subdim-1,y0:y0+pr.gang_subdim-1))/pr.gang_subdim^2
                    prof(iii+pr.gang_dim*j,t) = val
                    x0 = x0 + pr.gang_subdim
                endfor
                y0 = y0 + pr.gang_subdim
            endfor
            gang_titles = reform(gang_titles,pr.gang_dim^2)
            gang_titles_made = !TRUE
        end
        else: print,'Invalid value of pr.time_type
        endcase
        t_file = t_file + 1
    endfor
    ones = fltarr(lenprof)
    ones(*) = 1
    ones = ones*pmask

    if(pr.time_type eq !HISTO_TPROF) then begin
        prof1 = fltarr(nind,t1)
        if(t1 gt 1) then begin
            for i=0,nind-1 do begin
                m = total(prof(i,0:t1-1))/float(t1)
                prof1(i,0:t1-1) = prof(i,0:t1-1) - m
            endfor
            prof = reform(prof1,nind*long(t1))
        endif else begin
            prof = reform(prof,nind)
        endelse
    endif

        ; Smooth profile.
    if(pr.krnl_width gt 0) then begin
        if(pr.time_type eq !GANG_TPROF) then begin
            profile = fltarr(pr.gang_dim^2,hd.tdim)
            for z=0,pr.gang_dim^2-1 do $
                if(pr.krnl_type eq !BOXCAR) then $
                        profile(z,*) = smooth(prof(z,*),pr.krnl_width) $
                else $
                        profile(z,*) = gauss_smoth(prof(z,*),pr.krnl_width)
        endif else begin
            if(pr.krnl_type eq !BOXCAR) then $
                profile = smooth(prof,pr.krnl_width) $
            else $
                profile = gauss_smoth(prof,pr.krnl_width)
            endelse
    endif else begin
        profile = prof
    endelse

;   Now mean-correct the profiles if required.
    case pr.time_type of
    !GANG_TPROF: begin
        if(pr.true_scl eq !FALSE) then begin
            zero_mean = fltarr(pr.gang_dim^2,hd.tdim)
            for z=0,pr.gang_dim^2-1 do begin
                    mean = total(profile(z,*)*pmask)/total(ones)
                    zero_mean(z,*) = (profile(z,*) - mean)*pmask
                for i=0,hd.tdim-1 do $
                    zero_mean(z,i) = 100.*zero_mean(z,i)/mean
            endfor
        endif else begin
            zero_mean = profile
        endelse
     end
     else: begin
         den = total(pmask)
         if(den eq 0) then begin
             pmean = total(profile)/total(profile gt 0)
         endif else begin
             pmean = total(profile*pmask)/total(pmask)
         endelse
         mean = pmean
         if((pr.true_scl eq !FALSE)) then begin
             zero_mean = 100.*(profile - pmean)*pmask/pmean
         endif else begin
             if already_averaged eq !FALSE then $
                 zero_mean = profile/float(nind) $
             else $
                 zero_mean = profile
         endelse
     end
     endcase

    ;   Now draw the profiles.
	name = strcompress(string(fi.tails(mom),"_(",col,",",row,")"),/REMOVE_ALL)
        ;print,'pr.prof_on=',pr.prof_on,'  !PROF_NO=',!PROF_NO
	if(pr.prof_on eq !PROF_NO) then begin;
        ;go_here = 1
        ;if go_here eq 1 then begin
            ;print,'here2'
	    case pr.time_type of
	    !GANG_TPROF: begin
		ganged_time_profile,zero_mean,row,pr,dsp,fi,name,profdat,gang_titles,TITLE=gang_title
		pr.prof_on = !PROF_NO
	    end
	    !HISTO_TPROF: begin
		nsig = 10
		pts_per_sig = 8
		nh = nsig*pts_per_sig
		smean = total(profile)
		N = n_elements(profile)
		sig = sqrt((N*total(profile^2) - smean^2)/N/(N-1))
		binsize = sig/pts_per_sig
		smean = float(smean/N)
		profile(*) = profile(*) - smean
		min = -nsig*sig/2
		max =  nsig*sig/2
		histo = histogram(profile,OMIN=hmin,OMAX=hmax,MIN=min, $
		    MAX=max,BINSIZE=binsize)
		title = 'Histogram of time profile'
		label = string(nind*t1)
		dum = 0
		sym = pr.symtype
		pr.symtype = !SYMBOL
		profile,float(histo),dum,pr,dsp,fi,label,profdat, $
		    TITLE=title,XMIN=hmin/sig,XMAX=hmax/sig
		pr.symtype = sym
		lenh = n_elements(histo)
		x = fltarr(lenh)
		x(0) = min + binsize/(2.*sig)
		for i=1,lenh-1 do $
		x(i) = x(i-1) + binsize/sig
		params = fltarr(6)
		fit = gaussfit1(x,histo(0:lenh-1),params)
		mean = params(1)
		label = strcompress(string(params(0),mean,params(2), $
		FORMAT='(f10.0,"exp(-.5(x - ",f8.3,")**2/2*",f8.3,"**2)")'))
		text = strcompress(string(params(0),mean,params(2),smean,sig, $
		FORMAT='("Amplitude: ",f10.0,", Mean: ",f8.3,", std dev: ",f8.3," Sample mean: ",f8.3," Sample std. dev: ",f8.3)'))
			widget_control,wd.error,SET_VALUE=text
		print,text
            ;print,'hereO1'
            overplot,float(fit),row,label,profdat,pr,dsp,fi,XMIN=hmin/sig,XMAX=hmax/sig
        end
        else: begin ; Plot the profiles for all other types.
            if(pr.scatter_plot eq !TRUE) then begin
                if(ptr_valid(hd.ifh.behavior_names)) then begin
                    behavior_names = *hd.ifh.behavior_names
                endif else begin
                    stats = dialog_message('No behavior names specified in the 4dfp file header.')
                    return
                endelse
                ibehav = get_button(behavior_names,TITLE='Select behavioral variable')
                nbehav = n_elements(behavior_names)
                xvals = fltarr(hd.tdim)
                for t=0,hd.tdim-1 do begin
                    img = get_image(t*zdim,fi,stc,FILNUM=mom+1)
                    xvals[t] = img[ibehav]
                    if(t eq 0) then begin
                         print,nind,FORMAT='("Number of voxels: ",i3)'
                    endif
                endfor
		tdim = hd.tdim-2*nbehav
		prof = prof[0:tdim-1]
		xvals = xvals[0:tdim-1]

                ;print,'hereA'
		time_profile,prof,row,pr,dsp,fi,name,profdat,Y_TITLE=' ',XMIN=1,XVALS=xvals,/NO_LEGEND,/SQUARE

		hdm = *fi.hdr_ptr(hd.model)
                slp = get_regional_avg(indices_vol+(tdim+ibehav*2  )*xyzdim,fi,stc,FILNUM=mom+1)
                int = get_regional_avg(indices_vol+(tdim+ibehav*2+1)*xyzdim,fi,stc,FILNUM=mom+1)
		oprof = prof
		oxvals = xvals
		xmin = min(xvals)
		xmax = max(xvals)
		del = (xmax-xmin)/(tdim-1)
		for i=0,tdim-1 do begin
		    oxvals[i] = xmin + i*del
		    oprof[i] = slp*oxvals[i] + int
		endfor
                ;print,'hereO2'
		overplot,oprof,row,name,profdat,pr,dsp,fi,XVALS=oxvals,/NO_LEGEND
		pr.prof_on = !PROF_NO
                time_overplot = !PLOT_NOTHING
                pr.mode = !DRAW_OFF
                device,SET_GRAPHICS=3,/cursor_crosshair ;Resume normal graphics.
                widget_control,wd.draw[fi.cw],DRAW_MOTION_EVENTS=0
                widget_control,wd.draw[fi.cw],EVENT_PRO='stats_exec_event'
            endif else begin
                if(n_elements(name) eq 2) then begin
                    nam = name(0) + name(1)
                    name = nam
                endif

                ;print,'hereB'
                time_profile,zero_mean,row,pr,dsp,fi,name,profdat,Y_TITLE=' ',XMIN=1

                time_overplot = pr.time_overplot
            endelse
            case pr.time_overplot of
            !PLOT_PARADIGM: begin
                if(fi.paradigm_loaded(mom) eq !TRUE) then begin
                    max = max(zero_mean)
                    min = min(zero_mean)
                    if(abs(max) gt abs(min)) then $
                    max = abs(max) $
                    else $
                    max = -min
                    prof = max*paradigm(0:hd.tdim-1)
                    name = ''
                    ;print,'hereO3'
                    overplot,prof,row,name,profdat,pr,dsp,fi,LINESTYLE=!DOTTED,XMIN=1
                endif
            end
            !PLOT_GLM: begin
                hdm = *fi.hdr_ptr(hd.model)
                if(hdm.array_type eq !LINEAR_MODEL) then begin
                    prof = fltarr(hd.tdim)
                    widget_control,/HOURGLASS
                    prof = get_glm(profile,glm,indices_vol,hd.model)
                    widget_control,HOURGLASS=0
                    if(pr.true_scl eq !FALSE) then $
                        prof = 100.*pmask*(prof - pmean)/pmean
                    name = ''
                    ;print,'hereO4'
                    overplot,prof,row,name,profdat,pr,dsp,fi,XMIN=1
                endif else begin
                    widget_control,wd.error,SET_VALUE='Invalid pointer to GLM'
                    beep
                endelse
              end
              !PLOT_RESIDUALS: begin
                  if(ptr_valid(glm(hd.model).A)) then begin
                      widget_control,/HOURGLASS
                      prof = fltarr(hd.tdim)
                      prof = get_glm(profile,glm,idx,hd.model)
                      prof = prof/float(nind)
                      prof = 100.*pmask*(profile - prof)/pmean
                      name = ''
                     ;print,'hereO5'
                     overplot,prof,row,name,profdat,pr,dsp,fi,XMIN=1
                     ss = total(prof^2)/n_elements(prof)
                     val = string(ss,FORMAT='("Summed squares: ",f10.5)')
                     widget_control,wd.error,SET_VALUE=val
                     widget_control,HOURGLASS=0
                  endif
              end
              !PLOT_PCA: begin
                  if(hd.tdim lt 20) then begin
                      corr = fltarr(hd.tdim,hd.tdim)
                      variance = *glm[hd.model].sd
                      ATAm1 = *glm[hd.model].ATAm1
                      for i=0,hd.tdim-1 do begin
                          for j=0,hd.tdim-1 do $
                              corr[i,j] = zero_mean[i]*zero_mean[j] + variance[i,j]*ATAm1[i,j]
                      endfor
                      svdc,corr,W,U,V
                      idx = sort(w)
                      eigsort = fltarr(hd.tdim,hd.tdim)
                      for i=0,hd.tdim-1 do begin
                          j = idx[hd.tdim-i-1]
                          eigsort[i,*] = w[j]*U[j,*]
                          print,i,total(zero_mean*U[j,*])
                      endfor
;;;                   time_profile,eigsort[0,*],row,pr,dsp,fi,name,profdat,Y_TITLE='Eigenvector'
                      ;print,'hereO6'
                      overplot,eigsort[0,*],row,name,profdat,pr,dsp,fi,XMIN=1
;;;                   for i=1,hd.tdim-1 do $
;;;                       overplot,eigsort[i,*],row,name,profdat,pr,dsp,fi,XMIN=1
                  endif
              end
              !PLOT_HRF: begin
                if(ptr_valid(glm(hd.model).A)) then begin
                    stimlen = *glm(hd.model).stimlen
                    delay = *glm(hd.model).delay
                    lcfunc = *glm(hd.model).lcfunc
                    if(ptr_valid(glm(hd.model).b)) then begin
                        b = *glm(hd.model).b
                    endif else begin
                        print,"GLM file doesn't contain data."
                        return
                    endelse
                    TR = glm[hd.model].TR
                    ;period = glm(hd.model).period
                    effect_label = *glm[hd.model].effect_label
                    effect_column = *glm[hd.model].effect_column
                    effect_length = *glm[hd.model].effect_length
                    effect_TR = *glm[hd.model].effect_TR
                    effect_shift_TR = *glm[hd.model].effect_shift_TR
                    lcheck = !TRUE
                    lglm = !TRUE
                endif else begin
                    ;print,'here0b'
                    label = strarr(4)
                    value = strarr(4)
                    label[0] = string("Duration in seconds")
                    label[1] = string("Delay in seconds")
                    label[2] = string("Period in frames")
                    label[3] = string("TR in seconds")
                    value[0] = '2.36'
                    value[1] = '2'
                    value[2] = '8'
                    value[3] = '2.36'
                    str = get_str(4,label,value,TITLE='Load .glm file and use link model to avoid this.')
                    stimlen = fltarr(hd.uval+1)
                    stimlen[hd.uval] = float(str[0])
                    delay = fltarr(hd.uval+1)
                    delay[hd.uval] = float(str[1])
                    period = long(str[2])
                    TR_mod = float(str[3])
                    lcheck = !FALSE
                    lglm = !FALSE
                endelse
                lcgamma_deriv = !FALSE
                if(lglm eq !TRUE) then begin
                    if(total(lcfunc eq !GAMMA_DERIV) ne 0) then $
                        lcgamma_deriv = !TRUE
                endif
                if lcgamma_deriv eq !FALSE then begin
                    eff = hdr.uval
                    stimlen_mod = stimlen[eff]
                    delay_mod = delay[eff]
                    if ptr_valid(glm[hd.model].A) then begin
                        TR_mod = effect_TR[eff]
                        effect_shift_TR = effect_shift_TR[eff]
                        period = effect_length[eff]
                    endif else $
                        effect_shift_TR = 0. 
                    if lcheck eq !TRUE then begin
                        label = strarr(2)
                        value = strarr(2)
                        hrf_params = fltarr(2)
                        hrf_params[0] = !HRF_DELTA
                        hrf_params[1] = !HRF_TAU
                        label[0] = string(eff+1,FORMAT='("Duration for effect #",i1," in seconds")')
                        label[1] = strcompress(string(!HRF_DELTA,FORMAT='("Time-shift from default (",f4.2," seconds)")'))
                        value[0] = string(stimlen[eff])
                        value[1] = string(delay[eff])
                        str = get_str_bool(2,label,['Boynton','SPM "Cannonical"'],value,!FALSE,BOOL_TITLE='HRF model:')
                        stimlen_mod = float(str[0])
                        delay_mod = float(str[1])
                        if(fix(str[2]) eq 0) then $
                            hrf_model = !BOYNTON $
                        else $
                            hrf_model = !SPM_CANNONICAL
                    endif
                    hrf = get_hrf(hrf_model,period,TR_mod,effect_shift_TR,delay_mod,stimlen_mod,hrf_params)
                    m1 = total(zero_mean)/period
                    mean = total(hrf)/period
                    hrf = hrf - mean
                    hrf = hrf*max(zero_mean - m1)/max(hrf) + m1
                    name = ''
                    overplot,hrf,row,name,profdat,pr,dsp,fi,XMIN=1
                 endif else begin
                    ;Plot gamma derivative model.
                    eff = get_button(effect_label,TITLE='Select effect being plotted.')
                    z = plane - 1

                    ;alpha = total(mask*get_assoc(b[*,*,effect_column[eff]*hdr.zdim+z]))/float(nind)
                    ;beta  = total(mask*get_assoc(b[*,*,(effect_column[eff]+1)*hdr.zdim+z]))/float(nind)
                    ;START5
                    alpha = total(mask*get_assoc(b[*,*,effect_column[eff]*hdr.zdim+z],hdr.ifh.bigendian))/float(nind)
                    beta  = total(mask*get_assoc(b[*,*,(effect_column[eff]+1)*hdr.zdim+z],hdr.ifh.bigendian))/float(nind)

                    hrf = fltarr(hd.tdim)
                    period = effect_length[eff]
                    for i=0,period-1 do begin
                        time = i*TR
                        gamma = alpha*(time^!GAMMA_ORDER)*exp(-time)/gamma(!GAMMA_ORDER)
                        dgamma = beta *(time^(!GAMMA_ORDER-1))*(!GAMMA_ORDER-time)*exp(-time)/gamma(!GAMMA_ORDER)
                        hrf[i] = alpha*(time^!GAMMA_ORDER)*exp(-time)/gamma(!GAMMA_ORDER) + $
                                 beta *(time^(!GAMMA_ORDER-1))*(!GAMMA_ORDER-time)*exp(-time)/gamma(!GAMMA_ORDER)
                    endfor
                    m1 = total(zero_mean)/period
                    mean = total(hrf)/period
                    hrf = hrf - mean
                    hrf = hrf*max(zero_mean - m1)/max(hrf) + m1
                    name = ''
                    overplot,hrf,row,name,profdat,pr,dsp,fi,XMIN=1
                 endelse
              end
              !PLOT_NOTHING: begin
              end
              else: print,'Invalid value of pr.time_overplot.'
            endcase
            end
        endcase
            ;print,'here3'
            pr.prof_on = !PROF_T
        endif else begin
        if(n_elements(name) eq 2) then begin
        nam = name(0) + name(1)
            name = nam
        endif
        ;print,'hereO9'
        overplot,zero_mean,row,name,profdat,pr,dsp,fi,XMIN=1
        ;print,'here4'
        pr.prof_on = !PROF_T
    endelse
  endif else begin

    ;Print out value for the region defined by the user.
    if(pr.subtract_trend eq !TRUE) then  begin
        image = get_image((dsp[fi.cw].frame-1)*zdim+plane-1,fi,stc,/SUBTRACT_TREND,FILNUM=mom+1)
    endif else begin
        if(fi.atlas_pix[0,fi.n] eq !MMPPIX_X_222) and (hd.xdim eq 64) then $
            image = get_image((dsp[fi.cw].frame-1)*zdim+plane-1,fi,stc) $
        else $
            image = get_image((dsp[fi.cw].frame-1)*zdim+plane-1,fi,stc,FILNUM=mom+1)
    endelse

    if(pr.prof_proc eq !PROF_PROC_NOTHING) then begin
        if(pr.time_type eq !THRESH_TPROF) then begin
            image = mask*image
            ;val = total(image(x1:x2,y1:y2))/float(nind)
        endif else begin
            nind = width*height
            ;val = total(image(x1:x2,y1:y2))/nind
        endelse
        rtn = mean_and_sd(image(x1:x2,y1:y2))
        nind = rtn.count
        val = rtn.mean
        sd = rtn.sd
    endif else begin
    if(pr.subtract_trend eq !TRUE) then  $
            image1 = get_image((dsp[fi.cw].frame-1)*zdim+plane-1,fi,stc,/SUBTRACT_TREND,FILNUM=fi.n+1) $
        else $
            image1 = get_image((dsp[fi.cw].frame-1)*zdim+plane-1,fi,stc,FILNUM=fi.n+1)
        val = prof_reg_proc(pr.prof_proc,image,image1,mask,x1,x2,y1,y2,dsp,nind)
        sd = !UNSAMPLED_VOXEL
    endelse
    ;new= 'Region centered at '+ $
    ;     strcompress(string(col,(y1+y2)/2,plane,FORMAT='("(",i3,",",i3,",",i3,")")'),/REMOVE_ALL) + $
    ;     ' '+strcompress(string(val,FORMAT='("Value:",f11.6)'),/REMOVE_ALL)+' #vox:'+strcompress(string(nind))
    new = 'Region centered at '+ strcompress(string(col,(y1+y2)/2,plane,FORMAT='("(",i3,",",i3,",",i3,")")'),/REMOVE_ALL)
    if val eq !UNSAMPLED_VOXEL then begin
        new = new +' All voxels unsampled.'
    endif else begin
        new = new +' '+strcompress(string(val,FORMAT='("mean:",f11.6)'),/REMOVE_ALL)
        if sd ne !UNSAMPLED_VOXEL then new = new +' '+strcompress(string(sd,FORMAT='("sd:",f11.6)'),/REMOVE_ALL)
        new = new +' #vox:'+strcompress(string(nind))
    endelse
    widget_control,wd.error,SET_VALUE=new
    print,new
    pr.prof_on = !PROF_REG
  endelse

  if(pr.time_type eq !HISTO_TPROF) then begin 
    ;print,'here5'
    pr.prof_on = !PROF_NO
  endif
  pr.x = x1
  pr.y = y1
  pr.mode = !DRAW_OFF
  pr.time_type = pr.last_type
  widget_control,wd.draw[fi.cw],DRAW_MOTION_EVENTS=0
  widget_control,wd.draw[fi.cw],EVENT_PRO='stats_exec_event'
end


endcase
 
;print,'here get_time_prof bottom'

end
