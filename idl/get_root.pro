;Copyright 8/19/05 Washington University.  All Rights Reserved.
;get_root.pro  $Revision: 1.8 $
function get_root,filein,ext,KEEPPATH=keeppath
nfile = n_elements(filein)
path = strarr(nfile)
file = strarr(nfile)
if not keyword_set(KEEPPATH) then begin 
    scrap = strpos(filein,'/',/REVERSE_SEARCH)
    scrap1 = scrap + 1
    index = where(scrap eq -1,count)
    if count ne 0 then path[index] = '.'
    index = where(scrap ne -1,count)
    if count ne 0 then for i=0,count-1 do path[index[i]] = strmid(filein[index[i]],0,scrap[index[i]])
endif else $
    scrap1 = intarr(nfile)

;lizard=n_elements(ext)
;if lizard gt 0 then begin
;    if lizard eq 1 then $
;        junk = strpos(filein,ext,/REVERSE_SEARCH) $
;    else if lizard eq nfile then begin 
;        junk = intarr(nfile)
;        for i=0,nfile-1 do junk[i] = strpos(filein[i],ext[i],/REVERSE_SEARCH)  
;    endif
;    for i=0,nfile-1 do file[i] = strmid(filein[i],scrap1[i],junk[i]-scrap1[i])
;endif else begin 
;    for i=0,nfile-1 do file[i] = strmid(filein[i],scrap1[i])
;endelse
;START190924
lizard=n_elements(ext)
if lizard gt 0 then begin
    junk=intarr(nfile)
    idx=where(ext eq '.4dfp.img' or ext eq '.nii.gz' or ext eq '.nii',cnt)
    if cnt eq lizard then begin
        junk0 = strpos(filein,'.4dfp.img',/REVERSE_SEARCH)
        idx=where(junk0 gt -1,cnt)
        if cnt ne 0 then junk=junk0[idx] 
        junk0 = strpos(filein,'.nii.gz',/REVERSE_SEARCH)
        idx=where(junk0 gt -1,cnt)
        if cnt ne 0 then junk=junk0[idx] 
        junk0 = strpos(filein,'.nii',/REVERSE_SEARCH)
        idx=where(junk0 gt -1,cnt)
        if cnt ne 0 then junk=junk0[idx] 
    endif else begin
        if lizard eq 1 then $
            junk = strpos(filein,ext,/REVERSE_SEARCH) $
        else if lizard eq nfile then begin 
            for i=0,nfile-1 do junk[i] = strpos(filein[i],ext[i],/REVERSE_SEARCH)  
        endif
    endelse
    for i=0,nfile-1 do file[i] = strmid(filein[i],scrap1[i],junk[i]-scrap1[i])
endif else begin 
    for i=0,nfile-1 do file[i] = strmid(filein[i],scrap1[i])
endelse


return,rtn={file:file,path:path}
end
