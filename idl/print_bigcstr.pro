;Copyright 12/03/09 Washington University.  All Rights Reserved.
;print_bigcstr.pro  $Revision: 1.2 $

;pro print_bigcstr,nsubject,bigcstr,lu
;printf,lu,'set TIME_COURSES = (-tc \'
;START210303
pro print_bigcstr,nsubject,bigcstr,lu,BASH=BASH
if not keyword_set(BASH) then printf,lu,'set TIME_COURSES = (-tc \' else printf,lu,'TIME_COURSES=(-tc \'


scrap=strsplit(bigcstr[0],/EXTRACT)
for i=1,nsubject-1 do scrap=[[scrap],[strsplit(bigcstr[i],/EXTRACT)]]
size_scrap = size(scrap)
tol=256
for i=0,size_scrap[1]-1 do begin
    scrapstr = strjoin(scrap[i,*],',',/SINGLE)
    lscrapstr = strlen(scrapstr)
    if lscrapstr lt tol then $
        printf,lu,'        '+scrapstr+' \' $
    else begin
        lmax=100
        k=lmax
        llscrapstr=lscrapstr
        repeat begin
            repeat begin
                k = k + 1
            endrep until strmid(scrapstr,k,1) eq ',' or k eq (llscrapstr-1)
            if k eq (llscrapstr-1) then endchar = ' \' else endchar = '\'
            printf,lu,'        '+strmid(scrapstr,0,k+1)+endchar
            scrapstr = strmid(scrapstr,k+1)
            llscrapstr=strlen(scrapstr)
            k = lmax < (llscrapstr-2)
        endrep until endchar eq ' \'
    endelse
endfor
printf,lu,'        '+')'
printf,lu,''
end
