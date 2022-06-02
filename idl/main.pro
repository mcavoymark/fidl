;Copyright 12/31/99 Washington University.  All Rights Reserved.
;main.pro  $Revision: 12.64 $

pro main

defsysv,'!FIDL_REV',getenv('FIDL_REV')
@ECATdef.h
@stats.h
fontnum =  long(getenv('FIDL_FONTNUM'))
;if n_elements(fontnum) gt 0 then begin
if fontnum gt 0 then begin
    print,'processing fontnum ',fontnum
    case fontnum of
    1: font = '0-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1'
    2: font='-adobe-helvetica-medium-r-normal--14-140-75-75-p-77-iso8859-1'
    3: font = '-adobe-helvetica-bold-r-normal--14-140-75-75-p-77-iso8859-1'
    4: font = '-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso8859-1'
    else: print,'Using default font'
    endcase
    if n_elements(font) gt 0 then widget_control,DEFAULT_FONT=font
endif

;dispxmax = long(getenv('FIDL_SCREENX'))
;if dispxmax eq 0 then dispxmax = 1024 
;defsysv,'!DISPXMAX',dispxmax

;dispymax = long(getenv('FIDL_SCREENY'))
;if dispymax eq 0 then dispymax = 800 
;defsysv,'!DISPYMAX',dispymax

;print,'dispxmax=',dispxmax,' !DISPXMAX=',!DISPXMAX
;print,'dispymax=',dispymax,' !DISPYMAX=',!DISPYMAX


stats_exec
return
end
