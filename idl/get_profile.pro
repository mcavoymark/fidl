;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_profile.pro  $Revision: 12.90 $

;*****************
pro get_profile,ev
;*****************

; Get profile through an image and return vector of values and row.
; row: Row number of profile.

common profiles,x1,x2,y1,y2,x1m1,x2m1,y1m1,y2m1,row,col,profdat,profmask

common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg, $
       glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

;print,'get_profile top'

; Abort time profile.
if ev.press eq 2 then begin
    ;pr.mode = !DRAW_OFF
    ;pr.prof_on = !PROF_NO
    ;device,SET_GRAPHICS=3,/cursor_crosshair ;Resume normal graphics.
    ;widget_control,wd.draw[fi.cw],DRAW_MOTION_EVENTS=0
    ;widget_control,wd.draw[fi.cw],EVENT_PRO='stats_exec_event'

    abort_profile_mode,pr,wd,fi
    return
endif

case pr.mode of

!DRAW_OFF: begin
    widget_control,wd.draw[fi.cw],/DRAW_MOTION_EVENTS
    x = ev.x
    y = ev.y
    device,SET_GRAPHICS=6             ;Set XOR mode
    pln = dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + dsp[fi.cw].plane-1
    if help.enable eq !TRUE then begin
        widget_control,help.id,SET_VALUE='Left button: Select row, Middle button: Exit profile mode, Right button: ' $
            +'Plot profile. The column/row are displayed in the dialog window. The selected coordinates are converted ' $
            +'to image coordinates for computation of the profile. After it is plotted, the region represented by the ' $
            +'profile is outlined with a green line.'
    endif
    zoom = float(dsp[fi.cw].zoom)
    if pr.horzprof eq !TRUE then begin
        row = fix(( dsp[fi.cw].dsizey - y -1)/dsp[fi.cw].zoom)
        if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then begin
            y1 = dsp[fi.cw].dsizey - 1 - (dsp[fi.cw].zoom*(row - pr.height/2))
            y2 = y1 - dsp[fi.cw].zoom*pr.height
        endif else begin
            y1 = y - dsp[fi.cw].zoom*(pr.height-1)/2
            y2 = y + dsp[fi.cw].zoom*(pr.height-1)/2
        endelse
        widget_control,wd.error,SET_VALUE=string('Row: ',row)
        y1 = y - dsp[fi.cw].zoom*(pr.height-1)/2
        y2 = y1 + dsp[fi.cw].zoom*(pr.height-1)
        plots,[0, dsp[fi.cw].dsizey],[y1,y1],COLOR=!D.TABLE_SIZE-1,/DEV,/NOCLIP
        if pr.height gt 1 then plots,[0,dsp[fi.cw].dsizey],[y2,y2],COLOR=!D.TABLE_SIZE-1,/DEV,/NOCLIP
        y1m1 = y1
        y2m1 = y2
        ym1 = y
    endif else begin
        x1 = x - dsp[fi.cw].zoom*(pr.width-1)/2
        x2 = x1 + dsp[fi.cw].zoom*(pr.width-1)/2
        if(pr.width gt 1) then $
        col = fix(x1/(2*dsp[fi.cw].zoom)) + fix(x2/(2*dsp[fi.cw].zoom)) $
        else $
        col = fix(x1/dsp[fi.cw].zoom)
        widget_control,wd.error,SET_VALUE=string('Column: ',col)


        ;plots,[x1,x1],[0, dsp[fi.cw].dsizeX],COLOR=pr.color,/DEV,/NOCLIP
        ;if pr.width gt 1 then plots,[x2,x2],[0, dsp[fi.cw].dsizeX],COLOR=pr.color,/DEV,/NOCLIP

        plots,[x1,x1],[0,dsp[fi.cw].dsizeX],COLOR=!D.TABLE_SIZE-1,/DEV,/NOCLIP
        if pr.width gt 1 then plots,[x2,x2],[0,dsp[fi.cw].dsizeX],COLOR=!D.TABLE_SIZE-1,/DEV,/NOCLIP


        x1m1 = x1
        x2m1 = x2
        xm1 = x
    endelse
    if ev.press eq 4 then $
        pr.mode = !DRAW_DONE $
    else $
        pr.mode = !DRAWING
end

!DRAWING: begin
    x = ev.x
    y = ev.y
    if(pr.horzprof eq !TRUE) then begin
        row = fix(( dsp[fi.cw].dsizey - y -1)/dsp[fi.cw].zoom)
        if(y ne ym1) then begin
            row = fix(( dsp[fi.cw].dsizey - y -1)/dsp[fi.cw].zoom)
            if(pr.height gt 1) then begin
                if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then begin
                    y1 = dsp[fi.cw].dsizey - 1 - (dsp[fi.cw].zoom*(row - pr.height/2))
                    y2 = y1 - dsp[fi.cw].zoom*pr.height
                endif else begin
                    y1 = dsp[fi.cw].dsizey - 1 - (dsp[fi.cw].zoom*(row - pr.height/2) - dsp[fi.cw].zoom/2)
                    y2 = dsp[fi.cw].dsizey - 1 - (dsp[fi.cw].zoom*(row + pr.height/2) + dsp[fi.cw].zoom/2)
                endelse
            endif else begin
                if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then $
                    y1 = dsp[fi.cw].dsizey - 1 - (dsp[fi.cw].zoom*row) $
                else $
                    y1 = dsp[fi.cw].dsizey - 1 - (dsp[fi.cw].zoom*(row - pr.height/2))
            endelse
            widget_control,wd.error,SET_VALUE=string('Row: ',row)
            plots,[0, dsp[fi.cw].dsizeX],[y1m1,y1m1],COLOR= !D.TABLE_SIZE-1,/DEV,/NOCLIP
            if(pr.height gt 1) then $
                plots,[0, dsp[fi.cw].dsizeX],[y2m1,y2m1],COLOR= !D.TABLE_SIZE-1,/DEV,/NOCLIP
            plots,[0, dsp[fi.cw].dsizeX],[y1,y1],COLOR= !D.TABLE_SIZE-1,/DEV,/NOCLIP
            if(pr.height gt 1) then $
                plots,[0, dsp[fi.cw].dsizeX],[y2,y2],COLOR=!D.TABLE_SIZE-1,/DEV,/NOCLIP
            y1m1 = y1
            y2m1 = y2
            ym1 = y
        endif
    endif else begin
        if(x ne xm1) then begin
            col = fix(x/dsp[fi.cw].zoom)
            if(dsp[fi.cw].orientation eq !NEURO) then begin
                col1 = dsp[fi.cw].xdim - 1 - col
            endif else begin
                col1 = col
            endelse
            widget_control,wd.error,SET_VALUE=string('Column: ',col1)
            if(pr.width gt 1) then begin
                if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then begin
                    x1 = dsp[fi.cw].zoom*(col - pr.width/2)
                    x2 = dsp[fi.cw].zoom*(col + pr.width/2) + dsp[fi.cw].zoom
                endif else begin
                    x1 = dsp[fi.cw].zoom*(col - pr.width/2) - dsp[fi.cw].zoom/2
                    x2 = dsp[fi.cw].zoom*(col + pr.width/2) + dsp[fi.cw].zoom/2
                endelse
            endif else begin
                if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then $
                     x1 = dsp[fi.cw].zoom*col +dsp[fi.cw].zoom/2 $
                else $
                     x1 = dsp[fi.cw].zoom*col
            endelse


            ;plots,[x1m1,x1m1],[0, dsp[fi.cw].dsizeX],COLOR=pr.color,/DEV,/NOCLIP
            ;if(pr.width gt 1) then $
            ;    plots,[x2m1,x2m1],[0, dsp[fi.cw].dsizeX],COLOR=pr.color,/DEV,/NOCLIP
            ;plots,[x1,x1],[0, dsp[fi.cw].dsizeX],COLOR=pr.color,/DEV,/NOCLIP
            ;if(pr.width gt 1) then $
            ;    plots,[x2,x2],[0, dsp[fi.cw].dsizeX],COLOR=pr.color,/DEV,/NOCLIP

            plots,[x1m1,x1m1],[0,dsp[fi.cw].dsizeX],COLOR=!D.TABLE_SIZE-1,/DEV,/NOCLIP
            if pr.width gt 1 then plots,[x2m1,x2m1],[0,dsp[fi.cw].dsizeX],COLOR=!D.TABLE_SIZE-1,/DEV,/NOCLIP
            plots,[x1,x1],[0,dsp[fi.cw].dsizeX],COLOR=!D.TABLE_SIZE-1,/DEV,/NOCLIP
            if pr.width gt 1 then plots,[x2,x2],[0,dsp[fi.cw].dsizeX],COLOR=!D.TABLE_SIZE-1,/DEV,/NOCLIP


            x1m1 = x1
            x2m1 = x2
            xm1 = x
        endif
    endelse
    if(ev.press eq 4) then $
        pr.mode = !DRAW_DONE
end

!DRAW_DONE: begin
    pr.mode = !DRAW_OFF
    if pr.horzprof eq !TRUE then begin
        plots,[0,dsp[fi.cw].dsizeX],[y1,y1],COLOR= !D.TABLE_SIZE-1,/DEV,/NOCLIP
        if(pr.height gt 1) then $
            plots,[0, dsp[fi.cw].dsizeX],[y2,y2],COLOR= !D.TABLE_SIZE-1,/DEV,/NOCLIP
        device,SET_GRAPHICS=3   ;Resume normal graphics.
        if(pr.height gt 1) then begin
        row1 = row - pr.height/2
        row2 = row + pr.height/2
        if(row1 lt 0) then row1 = 0
        if(row1 gt dsp[fi.cw].ydim-1) then row1 = dsp[fi.cw].ydim-1
        if(row2 lt 0) then row2 = 0
        if(row2 gt dsp[fi.cw].ydim-1) then row2 = dsp[fi.cw].ydim-1
            prof = total(dsp_image(*,row1:row2),2)/(row2 - row1 + 1)
        endif else begin
        if(row gt dsp[fi.cw].ydim-1) then row = dsp[fi.cw].ydim-1
            prof = dsp_image(*,row)
        endelse
        if(dsp[fi.cw].orientation eq !NEURO) then $
            prof = reverse(prof)
        if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then begin
            y1 = dsp[fi.cw].dsizeX - 1 - (dsp[fi.cw].zoom*(row - pr.height/2))
            y2 = y1 - dsp[fi.cw].zoom*pr.height
         endif else begin
            y1 = dsp[fi.cw].dsizeY - 1 - (dsp[fi.cw].zoom*(row - pr.height/2) - dsp[fi.cw].zoom/2)
            y2 = dsp[fi.cw].dsizeY - 1 - (dsp[fi.cw].zoom*(row + pr.height/2) + dsp[fi.cw].zoom/2)
        endelse
        rect_plot,0,dsp[fi.cw].zoom*dsp[fi.cw].xdim-1,y1,y2,dsp[fi.cw].zoom,COLOR=pr.color, $
                        ORIENTATION=dsp[fi.cw].orientation
        pr.coord(0) = row
        coord = row
    endif else begin
        plots,[x1m1,x1m1],[0,dsp[fi.cw].dsizeX],COLOR=pr.color,/DEV,/NOCLIP
        if pr.width gt 1 then plots,[x2m1,x2m1],[0,dsp[fi.cw].dsizeX],COLOR=pr.color,/DEV,/NOCLIP
        device,SET_GRAPHICS=3 ;Resume normal graphics.
        if dsp[fi.cw].orientation eq !NEURO then begin
            if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then begin
                x1 = dsp[fi.cw].dsizeX - 1 - (dsp[fi.cw].zoom*(col + pr.width/2) + dsp[fi.cw].zoom)
                x2 = dsp[fi.cw].dsizeX - 1 - (dsp[fi.cw].zoom*(col - pr.width/2))
            endif else begin
                x1 = dsp[fi.cw].dsizeX - 1 - (dsp[fi.cw].zoom*(col + pr.width/2) + dsp[fi.cw].zoom/2)
                x2 = dsp[fi.cw].dsizeX - 1 - (dsp[fi.cw].zoom*(col - pr.width/2) - dsp[fi.cw].zoom/2)
            endelse
            col = fix(dsp[fi.cw].xdim - 1 - col)
        endif else begin
            if(dsp[fi.cw].zoomtype eq !NEAREST_NEIGHBOR) then begin
                x1 = dsp[fi.cw].zoom*(col - pr.width/2)
                x2 = dsp[fi.cw].zoom*(col + pr.width/2) + dsp[fi.cw].zoom
            endif else begin
                x1 = dsp[fi.cw].zoom*(col - pr.width/2) - dsp[fi.cw].zoom/2
                x2 = dsp[fi.cw].zoom*(col + pr.width/2) + dsp[fi.cw].zoom/2
            endelse
        endelse
        if pr.width gt 1 then begin
            col1 = col - pr.width/2
            col2 = col + pr.width/2
            if(col1 lt 0) then col1 = 0
            if(col1 gt dsp[fi.cw].ydim-1) then col1 = dsp[fi.cw].ydim-1
            if(col2 lt 0) then col2 = 0
            if(col2 gt dsp[fi.cw].ydim-1) then col2 = dsp[fi.cw].ydim-1
            prof = total(dsp_image(col1:col2,*),1)/(col2 - col1 + 1)
        endif else begin
            if(col lt 0) then col = 0
            if(col gt dsp[fi.cw].xdim-1) then col = dsp[fi.cw].xdim-1
            prof = dsp_image(col,*)
        endelse
        rect_plot,x1,x2, dsp[fi.cw].dsizeX-dsp[fi.cw].zoom*dsp[fi.cw].xdim, dsp[fi.cw].dsizeX,dsp[fi.cw].zoom, $
                COLOR=pr.color,ORIENTATION=dsp[fi.cw].orientation
        pr.coord[1] = col
        coord = col
    endelse
    pr.mode = !DRAW_OFF
    widget_control,wd.draw[fi.cw],DRAW_MOTION_EVENTS=0
    widget_control,wd.draw[fi.cw],EVENT_PRO='stats_exec_event'

    ;Smooth profile.
    if pr.krnl_width gt 0 then begin
        if(pr.krnl_type eq !BOXCAR) then $
            profile = smooth(prof,pr.krnl_width) $
        else $
            profile = gauss_smoth(prof,pr.krnl_width)
    endif else begin
        profile = prof
    endelse
    if pr.oprof then begin
        ;name = strcompress(string(fi.tail,"_r",row),/REMOVE_ALL)
        name = strtrim(fi.tail,2) + '_r' + strtrim(row,2)
        overplot,prof,row,name,profdat,pr,dsp,fi
    endif else begin
        profile,profile,row,pr,dsp,fi,fi.tail,profdat
    endelse
end

else: print,'Invalid value of pr.mode'

endcase

;print,'get_profile bottom'
end
