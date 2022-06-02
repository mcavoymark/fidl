;Copyright 2/27/02 Washington University.  All Rights Reserved.
;delete_regions.pro  $Revision: 1.19 $

;*****************************************
pro delete_regions,fi,wd,dsp,help,pref,stc
;*****************************************
title = 'Please select region file.'
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE=title
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr_region,idx_region,'region file',TITLE=title) eq !ERROR then return

widget_control,/HOURGLASS
region_img = fltarr(hdr_region.xdim,hdr_region.ydim,hdr_region.zdim)
widget_control,/HOURGLASS
for z=0,hdr_region.zdim-1 do begin
    region = get_image(z,fi,stc,FILNUM=idx_region+1)
    region_img[*,*,z] = region
endfor
region_names = *hdr_region.ifh.region_names
num_roi = hdr_region.ifh.nreg 
num_clusters = 2

scrap = intarr(num_roi)
for i=0,num_roi-1 do scrap[i] = total(region_img eq (i+2))
max_scrap = max(scrap)

goback2:



;thresh = get_str(1,'Mininum number of voxels','1',WIDTH=50,TITLE='Cluster threshold', $
;    LABEL='Regions with fewer than this many voxels will be marked for deletion.'+string(10B)+'The largest region has ' $
;    +strtrim(max_scrap,2)+' voxels.')
;threshstr = '_n'+strtrim(thresh[0],2)
;thresh = fix(thresh[0])
;default = intarr(num_roi)
;START110915
if max_scrap eq 0 then begin
    stat = get_button('exit',BASE_TITLE=fi.tail,TITLE='All regions have zero voxels.')
    return
endif else begin
    thresh = get_str(1,'Mininum number of voxels','1',WIDTH=50,TITLE='Cluster threshold', $
        LABEL='Regions with fewer than this many voxels will be marked for deletion.'+string(10B)+'The largest region has ' $
        +strtrim(max_scrap,2)+' voxels.')
    threshstr = '_n'+strtrim(thresh[0],2)
    thresh = fix(thresh[0])
    default = intarr(num_roi)
endelse





widget_control,/HOURGLASS
scrapstr = strtrim(scrap,2)
widget_control,/HOURGLASS
for i=0,num_roi-1 do begin
    if scrap[i] lt thresh then default[i] = 1
    region_names[i] = region_names[i] + ' ' + scrapstr[i]
endfor

widget_control,/HOURGLASS
labels = strarr(num_clusters,num_roi)
widget_control,/HOURGLASS
labels[0,*] = 'ignore'
labels[1,*] = 'delete'

if thresh gt 1 then junk='voxels' else junk='voxel'
title = 'Regions with fewer than '+strtrim(thresh,2)+' '+junk+' have been marked for deletion.'
repeat begin
    scrap = 'No'
    widget_control,/HOURGLASS

    ;rtn = get_choice_list_many(region_names,labels,TOP_TITLE='Please select regions to delete.',DEFAULT=default, $
    ;    /COLUMN_LAYOUT,TITLE=title,/GO_BACK)
    ;if rtn.special eq 'GO_BACK' then goto,goback2
    ;START110915
    rtn = get_choice_list_many(region_names,labels,TOP_TITLE='Please select regions to delete.',DEFAULT=default, $
        /COLUMN_LAYOUT,TITLE=title,/GO_BACK,/EXIT)
    if rtn.special eq 'GO_BACK' then goto,goback2 else if rtn.special eq 'EXIT' then return

    list = rtn.list
    max_list = max(list)
    if max_list eq 0 then begin
        scrap = dialog_message('No regions deleted. Do you wish to try again?',/QUESTION)
        if scrap eq 'No' then begin
            print,'DONE'
            return
        endif
    endif
endrep until scrap eq 'No' 
scrap = strtrim(where(list gt 0)+1,2)
roi_str = ' -delete'
for i=0,total(list)-1 do roi_str = roi_str + ' ' + scrap[i]
if strpos(fi.names[idx_region],'.4dfp.img',/REVERSE_SEARCH) eq -1 then fi.names[idx_region] = fi.names[idx_region] + '.4dfp.img'
region_str = ' -region_file ' + fi.names[idx_region]
scrap = fi.tails[idx_region]+threshstr+'.4dfp.img'

;scrap = get_str(1,'Output filename',scrap,WIDTH=100,TITLE='Please enter desired filename.')
;START110916
scrap = get_str(2,['output','script'],[scrap,'delete_regions.csh'],WIDTH=100,TITLE='Please enter.')
csh = fix_script_name(scrap[1])

scrap = scrap[0]
if strpos(scrap,'/') ne -1 then begin
    if strmid(scrap,0,1) ne '/' then scrap = '/' + scrap
endif
output_filename_str = ' -output_file "' + scrap + '"'

;cmd = !BINEXECUTE + '/delete_regions' + region_str + roi_str + output_filename_str
;START110916
cmd = 'nice +19 $BIN/delete_regions' + region_str + roi_str + output_filename_str
openw,lu,csh,/GET_LUN
top_of_script,lu
printf,lu,''
printf,lu,cmd
close,lu
free_lun,lu
spawn,'chmod +x '+csh

;spawn_cover,cmd,fi,wd,dsp,help,stc
;START110916
spawn_cover,csh,fi,wd,dsp,help,stc

print,'DONE'
end
