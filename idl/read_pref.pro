;Copyright 9/27/00 Washington University.  All Rights Reserved.
;read_pref.pro  $Revision: 12.96 $
pro read_pref,pref
home = getenv('HOME')
file = home + '/.fidl'
find = findfile(file)
if find[0] eq '' then return
sigeff = 0
openr,lu,file,/GET_LUN
rhost=''
rpath=''
line=''
repeat begin
    readf,lu,line
    str=strsplit(line,/EXTRACT,COUNT=cnt)
    if cnt ge 2 then begin
        case str[0] of
            'paradigm_skip':pref.paraskip=fix(str[1])
            'paradigm_init':pref.parainit=fix(str[1])
            'slope':pref.slope=fix(str[1])
            'intercept':pref.intercept=fix(str[1])
            'glm_z':pref.glm_z=fix(str[1])
            'glm_mag':pref.glm_mag=fix(str[1])
            'glm_var':pref.glm_var=fix(str[1])
            'glm_t':pref.glm_t=fix(str[1])
            'HRF_period':pref.period=fix(str[1])
            'orientation':pref.orientation=fix(str[1])
            'orientation_hide_label':pref.orientation_hide_label=fix(str[1])
            'printer_name':pref.printer_name=str[1]
            'concpath':pref.concpath=str[1]
            'fidlpath':pref.fidlpath=str[1]
            'chunk':if strupcase(str[1]) eq 'YES' then pref.chunk=1 else pref.chunk=0 

            ;START170220
            'cpu':pref.cpu=str[1]

            'Rpath':begin
                if cnt ge 3 then begin
                    rhost=[rhost,str[1]]
                    rpath=[rpath,str[2]]
                endif
            end 
            else: ;do nothing 
        endcase
    endif
endrep until(eof(lu))
close,lu
free_lun,lu
if n_elements(rhost) eq 1 then begin
    rhost='default'
    rpath='/usr/local/pkg/R/bin'
endif else begin
    rhost=rhost[1:*]
    rpath=rpath[1:*]
endelse
pref.rhost=ptr_new(rhost)
pref.rpath=ptr_new(rpath)
undefine,str,cnt,line
end
