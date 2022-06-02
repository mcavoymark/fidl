;Copyright 12/31/99 Washington University.  All Rights Reserved.
;writeu_swap.pro  $Revision: 1.9 $

pro writeu_swap,lu,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,SWAPBYTES=swapbytes

np = n_params() - 1
if np gt 20 then begin
    x = 'Too many parameters passed to writeu_swap.'
    print,x
    stat=dialog_message(x,/ERROR)
    return
endif
if !VERSION.OS_FAMILY eq 'Windows' then swapbytes=1 else if not keyword_set(SWAPBYTES) then swapbytes=0
if swapbytes eq 1 then begin
    for iarg=1,np do begin
        case iarg of
            1: b = a1
            2: b = a2 
            3: b = a3
            4: b = a4
            5: b = a5
            6: b = a6
            7: b = a7
            8: b = a8
            9: b = a9
            10: b = a10
            11: b = a11
            12: b = a12
            13: b = a13
            14: b = a14
            15: b = a15
            16: b = a16
            17: b = a17
            18: b = a18
            19: b = a19
            20: b = a20
        endcase
    endfor
    sz = size(b)
    if sz[n_elements(sz)-2] ne 7 then b = swap_endian(b)
    writeu,lu,b
endif else begin
    for i=1,np do begin
        case i of
            1:writeu,lu,a1
            2:writeu,lu,a2
            3:writeu,lu,a3
            4:writeu,lu,a4
            5:writeu,lu,a5
            6:writeu,lu,a6
            7:writeu,lu,a7
            8:writeu,lu,a8
            9:writeu,lu,a9
            10:writeu,lu,a10
            11:writeu,lu,a11
            12:writeu,lu,a12
            13:writeu,lu,a13
            14:writeu,lu,a14
            15:writeu,lu,a15
            16:writeu,lu,a16
            17:writeu,lu,a17
            18:writeu,lu,a18
            19:writeu,lu,a19
            20:writeu,lu,a20
        endcase
    endfor
endelse
end
