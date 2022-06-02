;Copyright 6/20/06 Washington University.  All Rights Reserved.
;print_contrasts_to_csh.pro  $Revision: 1.10 $
pro print_contrasts_to_csh,lu_csh,ntreatments0,nfiles,start_j,contrast,ntests1
sc = size(contrast)
if sc[0] ne 0 then contrast1 = reform(contrast) else contrast1 = contrast
contrast_str = strtrim(contrast1,2)
if nfiles gt 1 then begin
    if n_elements(ntests1) eq 0 then ntests1 = ntreatments0
    size_contrast = size(contrast1)
    count_tests = 0
    for m=0,size_contrast[2]-1 do begin
        cstr = ''
        j = start_j
        for i=0,nfiles-1 do begin
            if contrast1[j,m] ne 0 then begin
                if i eq 0 then begin
                    cstr = cstr + contrast_str[j,m]
                    ;Done this way to avoid having a space placed before the first number.
                endif else begin
                    cstr = cstr + ',' + contrast_str[j,m]
                endelse
            endif
            if cstr ne '' and i eq nfiles-1 then count_tests = count_tests + 1
            j = j + 1
        endfor
        if cstr ne '' then begin
            if count_tests ne ntests1 then $
                printf,lu_csh,'        '+cstr+' \' $
            else begin
                printf,lu_csh,'        '+cstr+')'
                goto,jump
            endelse
        endif
    endfor
endif else begin 
    idx = where(contrast1 ne 0,cnt)
    printf,lu_csh,'        '+strjoin(contrast_str[idx],' ',/SINGLE)+')'
endelse
jump:
end
