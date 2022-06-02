;Copyright 7/19/02 Washington University.  All Rights Reserved.
;get_filename_from_index.pro  $Revision: 1.2 $

;****************************************
function get_filename_from_index,fi,index
;****************************************

filename = strcompress(fi.list[index],/REMOVE_ALL)
if strpos(fi.list[index],'.4dfp.img',/REVERSE_SEARCH) eq -1 then filename = filename + '.4dfp.img'
scrap = ''
for i=0,n_elements(filename)-1 do scrap = scrap + filename[i]
filename = fi.paths[index] + scrap

return,filename
end
