;Copyright 12/31/99 Washington University.  All Rights Reserved.
;save_paradigm.pro  $Revision: 12.83 $

;************************
function save_paradigm,fi
;************************

hd = *fi.hdr_ptr(fi.n)
hdr = *fi.hdr_ptr(hd.mother)

paradigm = *fi.paradigm[fi.n]
paradigm_code =  encode_paradigm(paradigm,hdr.tdim)
print,'paradigm code: ',paradigm_code
len = strlen(paradigm_code)
if(len(0) eq 0) then begin
     print,'Paradigm code had length of zero.'
     return,!ERROR
endif

get_lun,lu
tail = fi.tails(hd.mother)
dot = rstrpos(tail,'.')
if(dot gt 0) then $
    stem = strmid(tail,0,dot) $
else $
    stem = tail
tail = strcompress(string(stem,'.4dfp.ifh'),/REMOVE_ALL)
filnam = fi.paths(hd.mother) + '/' + tail
openu,lu,filnam,/APPEND
printf,lu,'paradigm format := ',paradigm_code
close,lu
free_lun,lu
fi.paradigm_code(hd.mother) = paradigm_code

return,!OK
end
