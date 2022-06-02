;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_id.pro  $Revision: 12.88 $

;*****************************
function get_id,fi,dsp,cstm,pr
;*****************************

;Purpose: Get id of any currently displayed window.

;;;nwin = dsp[fi.cw].nwindows + cstm.nwindows + pr.nwindows + vw.num_windows + 1
nwin = dsp[fi.cw].nwindows + cstm.nwindows + pr.nwindows + 1
if(nwin le 0) then begin
    return,-1
endif
labels = strarr(nwin)
ids = intarr(nwin)
ids(0) = dsp[fi.cw].image_index[fi.cw]
labels(0) = string('Working window')
j = 1
for i=0,dsp[fi.cw].nwindows-1 do begin
    if(dsp[fi.cw].id(i) gt 0) then begin
	ids(j) = dsp[fi.cw].id(i)
        labels(j) = string(i,FORMAT='("Display all #",i3)')
        j = j + 1
    endif
end
for i=0,cstm.nwindows-1 do begin
    if(cstm.id(i) gt 0) then begin
	ids(j) = cstm.id(i)
        labels(j) = string(i,FORMAT='("Custom Display #",i3)')
        j = j + 1
    endif
end
for i=0,pr.nwindows-1 do begin
    if(pr.id(i) gt 0) then begin
	ids(j) = pr.id(i)
        labels(j)=string(i,FORMAT='("Profile #",i3)')
        j = j + 1
    endif
end
;;;for i=0,vw.num_windows-1 do begin
;;;    if(vw.id(i) gt 0) then begin
;;;	ids(j) = vw.wdw(i)
;;;        labels(j)=string(i,FORMAT='("View25d window #",i3)')
;;;        j = j + 1
;;;    endif
;;;endfor
if(j le 0) then begin
    return,-1
endif
lab1 = strarr(j)
lab1 = labels(0:j-1)
index = get_button(lab1)
id = ids(index)

return,id
end
