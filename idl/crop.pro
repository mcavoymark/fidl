;Copyright 12/31/99 Washington University.  All Rights Reserved.
;crop.pro  $Revision: 12.83 $

;****************
pro crop,cstm,dsp
;****************

value = strarr(4)
label = strarr(4)
value(0) = string(0)
value(1) = string(dsp[fi.cw].xdim-1)
value(2) = string(0)
value(3) = string(dsp[fi.cw].ydim-1)
label(0) = string("Minimum X Coordinate")
label(1) = string("Maximum X Coordinate")
label(2) = string("Minimum Y Coordinate")
label(3) = string("Maximum Y Coordinate")
dims = get_str(4,label,value)
cstm.crop(0) = fix(dims(0))
cstm.crop(1) = fix(dims(1))
cstm.crop(2) = fix(dims(2))
cstm.crop(3) = fix(dims(3))

end
