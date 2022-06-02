;Copyright 12/31/99 Washington University.  All Rights Reserved.
;view_images_dead.pro  $Revision: 1.7 $

;************************
pro view_images_dead,draw
;************************
 
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg, $
       glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

return
if(ptr_valid(vw.win[vw.cw])) then begin
    win = *vw.win[vw.cw]
endif else begin
    return
endelse
for i=0,win.ncell do begin
    if(ptr_valid(win.cell[i].base_image)) then $
        ptr_free,win.cell[i].base_image
    if(ptr_valid(win.cell[i].overlay_image)) then $
        ptr_free,win.cell[i].overlay_image
endfor
ptr_free,vw.win[vw.cw]
if(vw.num_windows eq 1) then begin
    vw.num_windows = 0
    widget_control,vw.view_leader,/DESTROY
    return
endif

; This code clears out the window structures.
if(vw.cw lt vw.num_windows-1) then begin
    win = *vw.win[vw.num_windows-1]
    vw.win[vw.cw] = ptr_new(win)
    ptr_free,vw.win[vw.num_windows-1]
    vw.base[vw.cw] = vw.base[vw.num_windows-1]
    vw.id[vw.cw] = vw.id[vw.num_windows-1]
    vw.wdw[vw.cw] = vw.wdw[vw.num_windows-1]
    vw.base[vw.num_windows-1] = -1
    vw.id[vw.num_windows-1] = -1
    vw.wdw[vw.num_windows-1] = -1
endif else begin
    vw.base[vw.cw] = -1
    vw.id[vw.cw] = -1
    vw.wdw[vw.num_windows-1] = -1
endelse
vw.num_windows = vw.num_windows - 1
vw.cw = vw.num_windows - 1
win = *vw.win[vw.cw]
prf = win.prf[win.idxidx]
update_text,fi,help,vw,*vw.win[vw.cw]

vw.num_windows = vw.num_windows - 1
;;;if(vw.num_windows eq 1) then $
;;;    widget_control,vw.view_leader,/DESTROY
 
return
end

