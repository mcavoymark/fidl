;Copyright 12/31/99 Washington University.  All Rights Reserved.
;readu_swap.pro  $Revision: 12.88 $

pro readu_swap,lu,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,SWAPBYTES=swapbytes

np = n_params() - 1
for i=1,np do begin
    case i of
        1:readu,lu,a1 
        2:readu,lu,a2 
        3:readu,lu,a3
        4:readu,lu,a4
        5:readu,lu,a5
        6:readu,lu,a6
        7:readu,lu,a7
        8:readu,lu,a8
        9:readu,lu,a9
        10:readu,lu,a10
        11:readu,lu,a11
        12:readu,lu,a12
        13:readu,lu,a13
        14:readu,lu,a14
        15:readu,lu,a15
        16:readu,lu,a16
        17:readu,lu,a17
        18:readu,lu,a18
        19:readu,lu,a19
        20:readu,lu,a20
    endcase
endfor

if !VERSION.OS_FAMILY eq 'Windows' then $
    swapbytes=1 $
else if not keyword_set(SWAPBYTES) then $
    swapbytes=0
if swapbytes eq 1 then begin
    for iarg=1,np do begin
        case iarg of
        1: begin
               sz = size(a1)
               if sz[n_elements(sz)-2] ne 7 then a1 = swap_endian(a1)
           end
        2: begin
               sz = size(a2)
               if sz[n_elements(sz)-2] ne 7 then a2 = swap_endian(a2)
           end
        3: begin
               sz = size(a3)
               if sz[n_elements(sz)-2] ne 7 then a3 = swap_endian(a3)
           end
        4: begin
               sz = size(a4)
               if sz[n_elements(sz)-2] ne 7 then a4 = swap_endian(a4)
           end
        5: begin
               sz = size(a5)
               if sz[n_elements(sz)-2] ne 7 then a5 = swap_endian(a5)
           end
        6: begin
               sz = size(a6)
               if sz[n_elements(sz)-2] ne 7 then a6 = swap_endian(a6)
           end
        7: begin
               sz = size(a7)
               if sz[n_elements(sz)-2] ne 7 then a7 = swap_endian(a7)
           end
        8: begin
               sz = size(a8)
               if sz[n_elements(sz)-2] ne 7 then a8 = swap_endian(a8)
           end
        9: begin
               sz = size(a9)
               if sz[n_elements(sz)-2] ne 7 then a9 = swap_endian(a9)
           end
        10: begin
               sz = size(a10)
               if sz[n_elements(sz)-2] ne 7 then a10 = swap_endian(a10)
           end
        11: begin
               sz = size(a11)
               if sz[n_elements(sz)-2] ne 7 then a11 = swap_endian(a11)
           end
        12: begin
               sz = size(a12)
               if sz[n_elements(sz)-2] ne 7 then a12 = swap_endian(a12)
           end
        13: begin
               sz = size(a13)
               if sz[n_elements(sz)-2] ne 7 then a13 = swap_endian(a13)
           end
        14: begin
               sz = size(a14)
               if sz[n_elements(sz)-2] ne 7 then a14 = swap_endian(a14)
           end
        15: begin
               sz = size(a15)
               if sz[n_elements(sz)-2] ne 7 then a15 = swap_endian(a15)
           end
        16: begin
               sz = size(a16)
               if sz[n_elements(sz)-2] ne 7 then a16 = swap_endian(a16)
           end
        17: begin
               sz = size(a17)
               if sz[n_elements(sz)-2] ne 7 then a17 = swap_endian(a17)
           end
        18: begin
               sz = size(a18)
               if sz[n_elements(sz)-2] ne 7 then a18 = swap_endian(a18)
           end
        19: begin
               sz = size(a19)
               if sz[n_elements(sz)-2] ne 7 then a19 = swap_endian(a19)
           end
        20: begin
               sz = size(a20)
               if sz[n_elements(sz)-2] ne 7 then a20 = swap_endian(a20)
           end
        else: begin
               print,'Too many parameters passed to readu_swap.'
               return
           end
         endcase
     endfor
endif
end
