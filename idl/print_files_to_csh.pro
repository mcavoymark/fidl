;Copyright 7/30/01 Washington University.  All Rights Reserved.
;print_files_to_csh.pro  $Revision: 1.9 $

;pro print_files_to_csh,lu_csh,nfiles,files,set_str,option_str,NO_NEWLINE=no_newline,QUOTES=quotes
;if not keyword_set(QUOTES) then qstr='' else qstr='"'
;if nfiles eq 1 then begin
;    printf,lu_csh,'set '+set_str+' = (-'+option_str+' '+qstr+files[0]+qstr+')'
;endif else begin
;    printf,lu_csh,'set '+set_str+' = (-'+option_str+' \'
;    for j=0,nfiles-2 do printf,lu_csh,'        '+qstr+files[j]+qstr+' \'
;    printf,lu_csh,'        '+qstr+files[j]+qstr+')'
;endelse
;if not keyword_set(NO_NEWLINE) then printf,lu_csh,''
;end
;START210204
pro print_files_to_csh,lu_csh,nfiles,files,set_str,option_str,NO_NEWLINE=no_newline,QUOTES=quotes,BASH=bash
    if not keyword_set(BASH) then begin
        if not keyword_set(QUOTES) then qstr='' else qstr='"'
        if nfiles eq 1 then begin
            printf,lu_csh,'set '+set_str+' = (-'+option_str+' '+qstr+files[0]+qstr+')'
        endif else begin
            printf,lu_csh,'set '+set_str+' = (-'+option_str+' \'
            for j=0,nfiles-2 do printf,lu_csh,'        '+qstr+files[j]+qstr+' \'
            printf,lu_csh,'        '+qstr+files[j]+qstr+')'
        endelse
        if not keyword_set(NO_NEWLINE) then printf,lu_csh,''
    endif else begin
        if not keyword_set(QUOTES) then qstr='' else qstr='"'
        if nfiles eq 1 then begin
            printf,lu_csh,set_str+'=(-'+option_str+' '+qstr+files[0]+qstr+')'
        endif else begin
            printf,lu_csh,set_str+'=(-'+option_str+' \'
            for j=0,nfiles-2 do printf,lu_csh,'        '+qstr+files[j]+qstr+' \'
            printf,lu_csh,'        '+qstr+files[j]+qstr+')'
        endelse
        if not keyword_set(NO_NEWLINE) then printf,lu_csh,''
    endelse
end
