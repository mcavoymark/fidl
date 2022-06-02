;Copyright 6/25/02 Washington University.  All Rights Reserved.
;assoc_4dfp.pro  $Revision: 1.10 $
function assoc_4dfp,fi,stc,dsp,st,help,wd,filename,ifh,type,num_type,img,mother,xdim,ydim,zdim,tdim,dxdy,dz,paradigm
ifh = read_mri_ifh(filename)
if n_tags(ifh) eq 0 then return,!ERROR 
xdim = ifh.matrix_size_1
ydim = ifh.matrix_size_2
zdim = ifh.matrix_size_3
tdim = ifh.matrix_size_4
type = !ASSOC_ARRAY
fi.n = fi.nfiles
openr,lun,filename[0],/GET_LUN,ERROR=error
if error ne 0 then begin
    print,!ERROR_STATE.MSG
    stat = dialog_message(!ERROR_STATE.MSG,/ERROR)
    return,!ERROR
endif else begin
    fi.lun[fi.n] = lun
    fs = fstat(lun)
    if fs.size eq 0 then begin
        help,fs,/STRUCTURE
        stat = dialog_message(fs.name+' does not appear to have any data in it!',/ERROR);
        return,!ERROR
    endif
    element_size = fs.size/(long(xdim)*long(ydim)*long(zdim)*long(tdim))
    offset = 0
    case element_size of
        2: begin
            num_type = !SHORT
            img = assoc(lun,intarr(xdim,ydim,/NOZERO),offset)
        end
        4: begin
            num_type = !FLOAT
            img = assoc(lun,fltarr(xdim,ydim,/NOZERO),offset)
        end
        8: begin
            num_type = !DOUBLE
            img = assoc(lun,dblarr(xdim,ydim,/NOZERO),offset)
        end
        else: begin
            stat = dialog_message('Unknown number format in assoc_4dfp.',/ERROR)
            print,'element_size=',element_size,' fs.size=',fs.size,' long(xdim)*long(ydim)*long(zdim)*long(tdim)=', $
                long(xdim)*long(ydim)*long(zdim)*long(tdim)
            return,!ERROR
        end
    endcase
    mother = fi.nfiles
endelse

return,!OK
end
