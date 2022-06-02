; $Id: resolve_all_jmo.pro,v 12.82 1999/12/16 20:48:59 jmo Exp $
;
; Copyright (c) 1995, Research Systems, Inc.  All rights reserved.
;	Unauthorized reproduction prohibited.
;+
; NAME:
;	RESOLVE_ALL
;
; PURPOSE:
;	Resolve (by compiling) all procedures and functions.
;	This is useful when preparing .sav files containing all the IDL
;	routines required for an application.
; CATEGORY:
;	Programming.
; CALLING SEQUENCE:
;	RESOLVE_ALL
; INPUTS:
;	None.
; KEYWORD PARAMETERS:
;	QUIET = if set, produce no messages.
; OUTPUTS:
;	No explicit outputs.
; COMMON BLOCKS:
;	None.
; SIDE EFFECTS:
; RESTRICTIONS:
;	Will not resolve procedures or functions that are called via
;	CALL_PROCEDURE, CALL_FUNCTION, or EXECUTE.  Only explicit calls
;	are resolved.
;
;	If an unresolved procedure or function is not in the IDL 
;	search path, an error occurs, and no additional routines
;	are resolved.
;
; PROCEDURE:
;	This routine iteratively determines the names of unresolved calls
;	to user-written or library procedures and functions, and then
;	compiles them.  The process stops when there are no unresolved
;	routines.
; EXAMPLE:
;	RESOLVE_ALL.
; MODIFICATION HISTORY:
; 	Written by:	Your name here, Date.
;	DMS, RSI, January, 1995.
;-

PRO resolve_all_jmo, QUIET = quiet

if n_elements(quiet) ne 0 then begin
    quiet_save= !quiet
    !quiet = quiet
endif else quiet = 0

repeat begin
    cnt = 0
    a = ROUTINE_NAMES(/PROC, /UNRESOLVED)
    len = n_elements(a)
    i = 0
    repeat begin
        if(strpos(strupcase(a(i)),'BYTORDER') ge 0) then begin
	    print,'Omitted ',a(i)
            if(len gt 1) then b = strarr(len-1) else b = ''
            if(i gt 0) then b(0:i-1) = a(0:i-1)
            if(i le len-2) then b(i:len-2) = a(i+1:len-1)
	    a = b
	    len = len - 1
        endif
        i = i + 1
    endrep until i ge len-1
    if strlen(a(0)) gt 0 then begin
	cnt = cnt + n_elements(a)
	if quiet eq 0 then print,'Resolving procedures: ', a
	resolve_routine, a
    endif
    a = ROUTINE_NAMES(/FUNC, /UNRESOLVED)
    len = n_elements(a)
    i = 0
    repeat begin
        if(strpos(strupcase(a(i)),'BYTORDER') ge 0) then begin
	    print,'Omitted ',a(i)
            if(len gt 1) then b = strarr(len-1) else b = ''
            if(i gt 0) then b(0:i-1) = a(0:i-1)
            if(i ge len-2) then b(i:len-2) = a(i+1:len-1)
	    a = b
	    len = len - 1
        endif
        i = i + 1
    endrep until i ge len-1
    if strlen(a(0)) gt 0 then begin
	cnt = cnt + n_elements(a)
	if quiet eq 0 then print,'Resolving functions: ', a
	resolve_routine, a, /IS_FUNCTION
    endif
endrep until cnt eq 0

if n_elements(quiet_save) ne 0 then !quiet = quiet_save
end
