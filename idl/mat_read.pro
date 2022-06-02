;Copyright 12/31/99 Washington University.  All Rights Reserved.
;mat_read.pro  $Revision: 12.83 $

;***********************
function mat_read,filnam
;***********************

; Read a matlab matrix.  Assume that there is only one matrix per file.

get_lun,lu
openr,lu,filnam,ERROR=status
if(status ne 0) then $
    return,-1
close,lu
free_lun,lu

names = strarr(100)
ptrs = ptrarr(100)

matrix = 0
matnam = ' '
type = ' '
nrow = long(0)
ncol = long(0)
ntags = 0
cmd = string('mat_read ',filnam)
spawn,cmd,UNIT=fd,PID=pid
readf,fd,matnam,type,nrow,ncol
repeat begin
;    print,matnam,' ',type,nrow,ncol
    names(ntags) = matnam
    if(type eq 'numeric') then begin
        if(nrow*ncol gt 0) then begin
            matrix = dblarr(nrow,ncol)
            readu,fd,matrix
	endif else begin
	    matrix = -1
	endelse
    endif else begin
        if(nrow*ncol gt 0) then begin
	    matrix = strarr(nrow)
	    str = ' '
            for i=0,nrow-1 do begin
                readf,fd,str
                matrix(i) = str
;                print,type,i,matrix(i)
	    endfor
	endif else begin
	    matrix = 'missing'
	endelse
    endelse
    ptrs(ntags) = ptr_new(matrix)
    ntags = ntags + 1
    readf,fd,matnam,type,nrow,ncol
endrep until(matnam eq 'null')
free_lun,fd
;cmd = string('kill -9 ',pid)
;spawn,cmd

case ntags of

    1: mat = matrix
    2: begin
        aa = *ptrs(0)
        mat = create_struct(names(0),aa)
    end
    2: begin
        aa = *ptrs(0)
        bb = *ptrs(1)
        mat = create_struct(names(0),aa,names(1),bb)
    end
    3: begin
        aa = *ptrs(0)
        bb = *ptrs(1)
        cc = *ptrs(2)
        mat = create_struct(names(0),aa, $
		names(1),bb, $
		names(2),cc)
    end
    4: begin
        aa = *ptrs(0)
        bb = *ptrs(1)
        cc = *ptrs(2)
        dd = *ptrs(3)
        mat = create_struct(names(0),aa, $
		names(1),bb, $
		names(2),cc, $
		names(3),dd)
    end
    5: begin
        aa = *ptrs(0)
        bb = *ptrs(1)
        cc = *ptrs(2)
        dd = *ptrs(3)
        ee = *ptrs(4)
        mat = create_struct(names(0),aa, $
		names(1),bb, $
		names(2),cc, $
		names(3),dd, $
		names(4),ee)
    end
    6: begin
        aa = *ptrs(0)
        bb = *ptrs(1)
        cc = *ptrs(2)
        dd = *ptrs(3)
        ee = *ptrs(4)
        ff = *ptrs(5)
        mat = create_struct(names(0),aa, $
		names(1),bb, $
		names(2),cc, $
		names(3),dd, $
		names(4),ee, $
		names(5),ff)
    end
    7: begin
        aa = *ptrs(0)
        bb = *ptrs(1)
        cc = *ptrs(2)
        dd = *ptrs(3)
        ee = *ptrs(4)
        ff = *ptrs(5)
        gg = *ptrs(6)
        mat = create_struct(names(0),aa, $
		names(1),bb, $
		names(2),cc, $
		names(3),dd, $
		names(4),ee, $
		names(5),ff, $
		names(6),gg)
    end
    14: begin
        aa = *ptrs(0)
        bb = *ptrs(1)
        cc = *ptrs(2)
        dd = *ptrs(3)
        ee = *ptrs(4)
        ff = *ptrs(5)
        gg = *ptrs(6)
        hh = *ptrs(7)
        ii = *ptrs(8)
        jj = *ptrs(9)
        kk = *ptrs(10)
        ll = *ptrs(11)
        mm = *ptrs(12)
        nn = *ptrs(13)
        oo = *ptrs(14)
        mat = create_struct(names(0),aa, $
		names(1),bb, $
		names(2),cc, $
		names(3),dd, $
		names(4),ee, $
		names(5),ff, $
		names(6),gg, $
		names(7),hh, $
		names(8),ii, $
		names(9),jj, $
		names(10),kk, $
		names(11),ll, $
		names(12),mm, $
		names(13),nn)
    end
    15: begin
        aa = *ptrs(0)
        bb = *ptrs(1)
        cc = *ptrs(2)
        dd = *ptrs(3)
        ee = *ptrs(4)
        ff = *ptrs(5)
        gg = *ptrs(6)
        hh = *ptrs(7)
        ii = *ptrs(8)
        jj = *ptrs(9)
        kk = *ptrs(10)
        ll = *ptrs(11)
        mm = *ptrs(12)
        nn = *ptrs(13)
        oo = *ptrs(14)
        pp = *ptrs(15)
        mat = create_struct(names(0),aa, $
		names(1),bb, $
		names(2),cc, $
		names(3),dd, $
		names(4),ee, $
		names(5),ff, $
		names(6),gg, $
		names(7),hh, $
		names(8),ii, $
		names(9),jj, $
		names(10),kk, $
		names(11),ll, $
		names(12),mm, $
		names(13),nn, $
		names(14),oo)
    end
    16: begin
        aa = *ptrs(0)
        bb = *ptrs(1)
        cc = *ptrs(2)
        dd = *ptrs(3)
        ee = *ptrs(4)
        ff = *ptrs(5)
        gg = *ptrs(6)
        hh = *ptrs(7)
        ii = *ptrs(8)
        jj = *ptrs(9)
        kk = *ptrs(10)
        ll = *ptrs(11)
        mm = *ptrs(12)
        nn = *ptrs(13)
        oo = *ptrs(14)
        pp = *ptrs(15)
        mat = create_struct(names(0),aa, $
		names(1),bb, $
		names(2),cc, $
		names(3),dd, $
		names(4),ee, $
		names(5),ff, $
		names(6),gg, $
		names(7),hh, $
		names(8),ii, $
		names(9),jj, $
		names(10),kk, $
		names(11),ll, $
		names(12),mm, $
		names(13),nn, $
		names(14),oo, $
		names(15),pp)
    end
    else: begin
        print,'Invalid number of tags'
	mat = 0
    end
endcase

return,mat
end
