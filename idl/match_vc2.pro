;Copyright 9/01/10 Washington University.  All Rights Reserved.
;match_vc2.pro  $Revision: 1.5 $

;function match_vc2,roots1,roots2,files1,files2,searchindex
;maxsearchindex = max(searchindex)
;nsearchindex = n_elements(searchindex)
;searcharr = make_array(maxsearchindex+2,/STRING,VALUE='?')
;searcharr[maxsearchindex+1]='*'
;r2 = roots2
;f2 = files2
;n1 = n_elements(files1)
;n2 = n_elements(files2)
;notfoundf1 = ''
;foundf1 = ''
;foundf2 = ''
;for i=0,n1-1 do begin
;    for j=0,nsearchindex-1 do searcharr[searchindex[j]]=strmid(roots1[i],searchindex[j],1)
;    searchstr = strjoin(searcharr,'',/SINGLE)
;    print,'here100 searchstr=',searchstr
;    idx = where(strmatch(r2,searchstr) eq 1,count)
;    if count eq 0 then begin
;        notfoundf1 = [notfoundf1,files1[i]]
;    endif else begin
;        foundf1 = [foundf1,files1[i]]
;        foundf2 = [foundf2,f2[idx[0]]]
;        idx = where(strmatch(r2,searchstr) eq 0,count)
;        if count ne 0 then begin
;            r2 = r2[idx]
;            f2 = f2[idx]
;        endif
;    endelse
;endfor
;START2
function match_vc2,roots1,roots2,files1,files2,SEARCHINDEX=searchindex,SEARCHROOT=searchroot
if not keyword_set(SEARCHROOT) then searchroot = 'vc'
if keyword_set(SEARCHINDEX) then begin
    maxsearchindex = max(searchindex)
    nsearchindex = n_elements(searchindex)
    searcharr = make_array(maxsearchindex+2,/STRING,VALUE='?')
    searcharr[maxsearchindex+1]='*'
    f1 = files1
    r1 = roots1
endif else begin
    pos=strpos(roots1,searchroot)
    lsearchroot = strlen(searchroot)
    index=where(pos eq -1,count)
    if count ne 0 then begin
        stat=dialog_message_long('INFORMATION','The following files will be excluded because they lack the search root numbers.' $
            +string(10B)+string(10B)+strjoin(strtrim(files1[index],2),string(10B),/SINGLE))
        if stat eq 'EXIT' then return,rtn={msg:'EXIT'} else if stat eq 'GOBACK' then return,rtn={msg:'GO_BACK'}
        if count eq nfiles1 then begin
            stat=dialog_message('No search root found.',/ERROR)
            return,rtn={msg:'GO_BACK'}
         endif
    endif
    newindex=where(pos gt -1,newcount)
    f1 = files1[newindex]
    r1 = roots1[newindex]
    pos = pos[newindex]
    for i=0,newcount-1 do begin
        scrap=strsplit(strmid(r1[i],pos[i]+lsearchroot),'[^0-9]',/REGEX,/EXTRACT)
        r1[i] = scrap[0]
    endfor
endelse
r2 = roots2
f2 = files2
n1 = n_elements(f1)

;START12
;n2 = n_elements(f2)

notfoundf1 = ''
foundf1 = ''
foundf2 = ''
for i=0,n1-1 do begin
    if keyword_set(SEARCHINDEX) then begin 
        for j=0,nsearchindex-1 do searcharr[searchindex[j]]=strmid(r1[i],searchindex[j],1)
        searchstr = strjoin(searcharr,'',/SINGLE)
    endif else $
        searchstr = '*' + r1[i] + '*'

    ;print,'here100 searchstr=',searchstr
    idx = where(strmatch(r2,searchstr) eq 1,count)
    if count eq 0 then begin
        notfoundf1 = [notfoundf1,files1[i]]
    endif else begin
        foundf1 = [foundf1,f1[i]]
        foundf2 = [foundf2,f2[idx[0]]]
        idx = where(strmatch(r2,searchstr) eq 0,count)
        ;print,'count=',count
        if count ne 0 then begin
            r2 = r2[idx]
            f2 = f2[idx]

        ;endif
        ;START12
        endif else if count eq 0 then f2=''

    endelse
endfor




outofhere:
if n_elements(notfoundf1) gt 1 then begin
    str = 'The following concs did not have matching evs.'+string(10B)+strjoin(strtrim(notfoundf1[1:*],2),string(10B),/SINGLE)
endif

;if n2 gt 0 then begin
;START12
if f2[0] ne '' then begin

    if n_elements(str) then str=str+string(10B)+string(10B) else str = ''
    str = str + 'The following evs were not matched.'+string(10B)+strjoin(strtrim(f2,2),string(10B),/SINGLE)
endif
if n_elements(str) then begin 
    stat=dialog_message_long('INFORMATION',str)
    if stat eq 'EXIT' then return,rtn={msg:'EXIT'} else if stat eq 'GOBACK' then return,rtn={msg:'GO_BACK'}
endif
if n_elements(foundf1) eq 1 then return,rtn={msg:'ERROR'} 
return,rtn={msg:'OK',files1:foundf1[1:*],files2:foundf2[1:*]}
end
