;Copyright 12/31/99 Washington University.  All Rights Reserved.
;find_cell.pro  $Revision: 1.3 $

;*************************************************
function find_cell_label_num,icell,win,x,y,zoom_in
;*************************************************

if(ptr_valid(win.cell[icell].labels)) then $
    labels = *win.cell[icell].labels $
else $
    return,-1

lnum = -1
for ilab=0,win.cell[icell].nlab-1 do begin
    zoom = zoom_in
    x0 = win.cell[icell].x + labels[ilab].x
    y0 = win.cell[icell].y + labels[ilab].y
    if((x ge x0) and (x lt x0+labels[ilab].wdth)) then begin
        if((y ge y0) and (y lt y0+labels[ilab].hgth)) then $
            return,ilab
     endif
endfor

return,lnum
end


;**********************************************
function find_cell,win,x,y,zoom_in,display_mode
;**********************************************

fnd = {Find_cell}

fnd.itr = -1
fnd.lnum = -1
case display_mode of
    !OVERLAY_MODE: begin
        for itr=0,win.ntrans-1 do begin
            icell = win.translation_table[itr]
            if(win.cell[icell].view eq !VIEW_COLOR_SCALE) then $
                zoom = 1 $
            else  $
                zoom = zoom_in
            if(win.cell[icell].paint eq !TRUE) then begiN
                if((x ge win.cell[icell].x) and (x lt win.cell[icell].x+zoom*win.cell[icell].xdim)) then begin
                    if((y ge win.cell[icell].y) and (y lt win.cell[icell].y+zoom*win.cell[icell].ydim)) then begin
                        fnd.itr = itr
;;;                        fnd.lnum = find_label_num(icell,win,x,y,zoom_in)
                        fnd.lnum = -1
                        return,fnd
                    endif
                endif
             endif
        endfor
    end
    !ANAT_OVER_ACT_MODE: begin
       for itr=0,win.ntrans-1 do begin
            icell = win.translation_table[itr]
            if(win.cell[icell].view eq !VIEW_COLOR_SCALE) then $
                zoom = 1 $
            else  $
                zoom = zoom_in
            if(win.cell[icell].paint eq !TRUE) then begin
                if((x ge win.cell[icell].x) and (x lt win.cell[icell].x+zoom*win.cell[icell].xdim)) then begin
                    if((y ge win.cell[icell].y) and (y lt win.cell[icell].y+2*zoom*win.cell[icell].ydim)) then begin
                        fnd.itr = itr
                        return,fnd
                    endif
                endif
             endif
        endfor
    end
    else: print,'Invalid value of display_mode in reslice.'
endcase

return,fnd
end
