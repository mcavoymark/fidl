;Copyright 12/31/99 Washington University.  All Rights Reserved.
;load_raw.pro  $Revision: 12.83 $

;*************************************************************************************
function load_raw,fi,st,dsp,wd,stc,img_type,help,filnam,xdim,ydim,zdim,tdim,num_type,dxdy,dz, $
    scl,type,paradigm,img
;*************************************************************************************

; Load unformatted data.

if(help.enable eq !TRUE) then begin
    widget_control,help.id,SET_VALUE=string('Loads a single XYZ frame of data using associated variables.')
endif
file = dialog_pickfile(/read,FILTER='*',PATH=fi.path, $
                            GET_PATH=path,/NOCONFIRM)
if(file eq '') then return, !ERROR
fi.path = path
if(n_elements(file) gt 1) then $
    filnam = file(0)+file(1) $
else $
    filnam = file

;   Get number format.
labels = strarr(5)
labels(0) = string('Byte')
labels(1) = string('Short integer')
labels(2) = string('Floating point')
labels(3) = string('Double precision')
labels(4) = string('Complex')
index = get_button(labels)
case index of
    0: num_type = !BYTE
    1: num_type = !SHORT
    2: num_type = !FLOAT
    3: num_type = !DOUBLE
    4: num_type = !COMPLEX
endcase

;Get file size, prompt for x, y dimensions, calcuate z and prompt.
cmd = string('ls -l '+filnam+' | gawk '+string(39B)+'{print $5}'+string(39B)) 
spawn,cmd,csize
size = long(csize)

value = strarr(4)
labels = strarr(4)

if csize[0] eq 917504 then begin
    value(0) = string('256')
    value(1) = string('7')
    value(2) = string('128')
end else if csize[0] eq 262144 then begin
    value(0) = string('64')
    value(1) = string('64')
    value(2) = string('16')
end else if csize[0] eq 33554432 then begin
    value(0) = string('64')
    value(1) = string('64')
    value(2) = string('16')
end else begin
    value(0) = string('64')
    value(1) = string('64')
    value(2) = string('17')
end
value(3) = string('0')

labels(0) = string("X dimension: ")
labels(1) = string("Y dimension: ")
labels(2) = string("Z dimension: ")
labels(3) = string("Offset (bytes): ") ; Offset into file in bytes.
dims = get_str(4,labels,value)
xdim = long(dims(0))
ydim = long(dims(1))
zdim = long(dims(2))
offset = long(dims(3))
labels(0) = string("Time dimension: ")
labels(1) = string('XY pixel size')
labels(2) = string('Plane thickness')
value(0) = size/(xdim*ydim*zdim*num_type)
case num_type of
    !FLOAT: value[0] = size/(xdim*ydim*zdim*4)
    !SHORT: value[0] = size/(xdim*ydim*zdim*2)
    !BYTE:  value[0] = size/(xdim*ydim*zdim*1)
    !DOUBLE: value[0] = size/(xdim*ydim*zdim*8)
    !COMPLEX: value[0] = size/(xdim*ydim*zdim*8)
endcase

value(1) = string('1')
value(2) = string('1')
dims = get_str(3,labels,value)
tdim = fix(dims(0))
dxdy = float(dims(1))
dz   = float(dims(2))
type = !FLOAT_ARRAY
fi.n = fi.nfiles
get_lun,lun
fi.lun(fi.n) = lun
openr,lun,filnam
case num_type of
    !FLOAT: img = assoc(lun,fltarr(xdim,ydim,/NOZERO),offset)
    !SHORT: img = assoc(lun,intarr(xdim,ydim,/NOZERO),offset)
    !BYTE:  img = assoc(lun,bytarr(xdim,ydim,/NOZERO),offset)
    !COMPLEX: img = assoc(lun,complexarr(xdim,ydim,/NOZERO),offset)
    !DOUBLE: img = assoc(lun,dblarr(xdim,ydim,/NOZERO),offset)
endcase

paradigm = fltarr(tdim)

return, !OK
end
