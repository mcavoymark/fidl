;Copyright 3/8/01 Washington University.  All Rights Reserved.
;fix_script_name.pro  $Revision: 1.10 $
function fix_script_name,name_in,SPACE_ONLY=space_only,FORWARD_SLASH=forward_slash,ALL=all
name = strcompress(strtrim(name_in,2))
if keyword_set(SPACE_ONLY) then begin 
    loop = 1 
    index = [0,1]
endif else if keyword_set(FORWARD_SLASH) then begin 
    loop = 4
    index = [0,1,2,3,4]
endif else if keyword_set(ALL) then begin 
    loop = 6
    index = [0,1,2,3,4,5,6]
endif else begin 
    loop = 5
    index = [0,1,2,3,5,6]
endelse
scraplabel = ['"',' ',')','(','/','*','!']
replace    = ['' ,'_','_','_','_','' ,'1']
for j=0,n_elements(name)-1 do begin
    for i=0,loop do begin
        name[j] = strjoin(strsplit(name[j],scraplabel[index[i]],/EXTRACT),replace[i])
    endfor
endfor
name = strtrim(name,2)
return,name
end
