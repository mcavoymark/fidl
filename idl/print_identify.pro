;Copyright 2/8/07 Washington University.  All Rights Reserved.
;print_identify.pro  $Revision: 1.3 $

;********************************************
function print_identify,total_nsubjects,is,lu
;********************************************

searchstr = ''
for i=0,total_nsubjects-1 do searchstr = [searchstr,*is[i].s]
searchstr = searchstr[1:*] 
searchstr = searchstr[uniq(searchstr,sort(searchstr))]
nsearchstr = n_elements(searchstr)
istop = nsearchstr-1

printf,lu,'set RUNS = (-runs \'
for i=0,istop do begin
    goose = ''
    for j=0,total_nsubjects-1 do begin
        index = where(*is[j].s eq searchstr[i],count) 
        if count gt 1 then begin
            stat=dialog_message('count='+strtrim(count,2)+scrap(10)+'Must be 0 or 1.',/ERROR)
            return,rtn={nsearchstr:-1}
        endif
        if count eq 0 then $
            goose = [goose,'-1'] $
        else begin
            idx = *is[j].i
            nidx = *is[j].ni
            starti = *is[j].starti
            goose = [goose,strjoin(strtrim(idx[starti[index[0]]:starti[index[0]]+nidx[index[0]]-1]+1,2),'+',/SINGLE)]
        endelse
    endfor
    if i lt istop then ending = ' \' else ending = ')'
    printf,lu,'    '+strjoin(goose[1:*],',',/SINGLE)+ending
endfor
return,rtn={nsearchstr:nsearchstr,searchstr:searchstr}
end
