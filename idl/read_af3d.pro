;Copyright 4/25/01 Washington University.  All Rights Reserved.
;read_af3d.pro  $Revision: 1.13 $
function read_af3d,fi,wd,dsp,help,atlas_space,af3dp
title = 'Please select af3d or text file(s).'
type = 'af3d'
if type eq 'af3d' then begin 
    case atlas_space of
       !SPACE_111: atlas = 111
       !SPACE_222: atlas = 222
       !SPACE_333: atlas = 333
       else: begin
            stat=dialog_message('Need to be in either 111, 222 or 333.',/ERROR)
            return,!ERROR
       endelse
    endcase
endif    
get_dialog_pickfile,'*.dat',fi.path,title,region_file,nregfiles,rtn_path,/MULTIPLE_FILES
if nregfiles eq 0 then return,!ERROR
if type eq 'af3d' then begin 
    thresholds = get_str(nregfiles,region_file,strtrim(fltarr(nregfiles),2),WIDTH=50, $
        TITLE='Please enter thresholds to apply to each file. '+string(13B) $
        +'If the files contain only coordinates, then press OK.')
endif else $
    thresholds = make_array(nregfiles,/STRING,VALUE='0')

nregnames = intarr(nregfiles)
for i=0,nregfiles-1 do begin

    spawn,'more '+ region_file[i] + ' | wc -l',scrap
    ;spawn,'more '+ region_file[i] + ' | wc -l',scrap,/SH
    ;spawn,'/bin/more '+ region_file[i] + ' | wc -l',scrap

    nregnames[i] = fix(scrap[0])
endfor
scrap = total(nregnames)
coordinates = fltarr(scrap,4)
x = intarr(scrap)
y = intarr(scrap)
z = intarr(scrap)
passtype = bytarr(11)
passtype[0] = 1
passtype[1] = 1
passtype[2] = 1
passtype[10] = 1
nindex = 0L
for i=0,nregfiles-1 do begin
    scrap_coor_x = fltarr(nregnames[i])
    scrap_coor_y = fltarr(nregnames[i])
    scrap_coor_z = fltarr(nregnames[i])
    scrap_coor_value = fltarr(nregnames[i])
    scrap_x = intarr(nregnames[i])
    scrap_y = intarr(nregnames[i])
    scrap_z = intarr(nregnames[i])
    scrap_nindex = call_external(!SHARE_LIB,'_read_af3d',region_file[i],atlas,thresholds[i],scrap_coor_x, $
        scrap_coor_y,scrap_coor_z,scrap_coor_value,scrap_x,scrap_y,scrap_z,type,VALUE=passtype,/I_VALUE)
    coordinates[nindex:nindex+scrap_nindex-1,0] = scrap_coor_x[0:scrap_nindex-1]
    coordinates[nindex:nindex+scrap_nindex-1,1] = scrap_coor_y[0:scrap_nindex-1]
    coordinates[nindex:nindex+scrap_nindex-1,2] = scrap_coor_z[0:scrap_nindex-1]
    coordinates[nindex:nindex+scrap_nindex-1,3] = scrap_coor_value[0:scrap_nindex-1]
    x[nindex:nindex+scrap_nindex-1] = scrap_x[0:scrap_nindex-1]
    y[nindex:nindex+scrap_nindex-1] = scrap_y[0:scrap_nindex-1]
    z[nindex:nindex+scrap_nindex-1] = scrap_z[0:scrap_nindex-1]
    nindex = nindex + scrap_nindex
endfor
coordinates = coordinates[0:nindex-1,*]
x = x[0:nindex-1]
y = y[0:nindex-1]
z = z[0:nindex-1]





af3dp = {Af3d_points}
af3dp.nfiles = nregfiles
af3dp.files = ptr_new(region_file)
af3dp.coordinates = ptr_new(coordinates)
af3dp.x = ptr_new(x)
af3dp.y = ptr_new(y)
af3dp.z = ptr_new(z) 
af3dp.nindex = nindex

return,!OK
end
