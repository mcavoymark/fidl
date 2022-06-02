;Copyright 12/21/00 Washington University.  All Rights Reserved.
;readf_ascii_file.pro  $Revision: 1.24 $
function readf_ascii_file,filename,HDR_ONLY=hdr_only,NOHDR=nohdr
nhdr = 0
spawn,'wc -l '+filename,nlines
nlines = nlines[0]
if nlines eq '' then begin
    stat=dialog_message('Cannot open '+filename,/ERROR)
    return,contents = {name:'ERROR'}
endif
openr,lu,filename,/GET_LUN
hdr = ''
if not keyword_set(NOHDR) then begin
    header = ''
    readf,lu,header

    ;print,'!PATH=',!PATH

    hdr = strsplit(header,/EXTRACT)
    nhdr = n_elements(hdr)
    if keyword_set(HDR_ONLY) then begin
        close,lu
        free_lun,lu
        ;return,contents = {name:'',hdr:hdr}
        return,contents = {name:'',hdr:hdr,nhdr:nhdr}
    endif
    ;nhdr = n_elements(hdr)
endif
sdata = ''
lines_with_data = -1
i = 0
while not EOF(lu) do begin
    readf,lu,sdata
    scrap = strtrim(sdata,2)
    if scrap ne '' and scrap ne string(26B) then lines_with_data=[lines_with_data,1] else lines_with_data=[lines_with_data,0]
    i = i + 1
endwhile
lines_with_data = lines_with_data[1:*]
NR = total(lines_with_data)
;print,'NR=',NR
if NR eq 0 then begin
    spawn,'echo $LOGNAME',logname
    stat=dialog_message('Listen '+logname+'.'+string(10B)+string(10B)+filename+string(10B)+string(10B)+'Has nothing in it.  ' $
        +'How can you make a design matrix with an event file that has nothing?'+string(10B)+ $
        'Getting real mad. Fatal error. Abort!',/ERROR)
    return,contents={name:'ERROR'}
endif
NF_each_line = intarr(NR)
point_lun,lu,0
header = ''
if not keyword_set(NOHDR) then readf,lu,header 

;i = 0
;j = 0
;START140709
i = 0L
j = 0L

while not EOF(lu) do begin
    readf,lu,sdata
    if lines_with_data[j] eq 1 then begin
        sdata = strtrim(sdata,2)
        NF_each_line[i] = n_elements(strsplit(sdata,/EXTRACT))
        i = i + 1
    endif
    j = j + 1
endwhile
;print,'NF_each_line=',NF_each_line
NF = max(NF_each_line)
data = strarr(NF,NR)
point_lun,lu,0
header = ''
if not keyword_set(NOHDR) then readf,lu,header 

;i = 0
;j = 0
;START140709
i = 0L
j = 0L

while not EOF(lu) do begin
    readf,lu,sdata
    if lines_with_data[j] eq 1 then begin
        sdata = strtrim(sdata,2)
        data[0:NF_each_line[i]-1,i] = strsplit(sdata,/EXTRACT)
        i = i + 1
    endif
    j = j + 1
endwhile
close,lu
free_lun,lu
return,contents = {name:'',NR:NR,NF:NF,hdr:hdr,data:data,nhdr:nhdr,NF_each_line:NF_each_line}
end
