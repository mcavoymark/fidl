;Copyright 3/5/01 Washington University.  All Rights Reserved.
;grow_regions.pro  $Revision: 1.8 $
pro grow_regions,fi,wd,dsp,help
offset_spawn = 0
Nimage_str = ''
AND_str = ''
region_files_str = ''
subject_contributions_str = ''
spawn,'hostname',hostname ;use a harmless command because the $cwd could be real long
if n_elements(hostname) gt 1 then offset_spawn = 1
goback0a:
type = get_button(['222_t88','333_t88','EXIT'],TITLE='What type of analysis do you wish to do?')
if type eq 0 then $
    mask_str = ' -mask ' + !MASK_FILE_222 $
else if type eq 1 then $
    mask_str = ' -mask ' + !MASK_FILE_333 $
else $
    return
goback0:

;mcomp_files = get_files(TITLE='Please specify the proper filter to load images corrected for multiple comparisons or region files.', $
;    FILTER='*.4dfp.img',OFFSET_SPAWN=offset_spawn,/GO_BACK);
;if mcomp_files[0] eq 'GO_BACK' then goto,goback0a
;goback1:
;rtn = select_files(mcomp_files,TITLE='Please select images corrected for multiple comparisons or region files.', $
;    MIN_NUM_TO_SELECT=1,/GO_BACK)
;if rtn.files[0] eq 'GO_BACK' then goto,goback0
;selected_mcomp_files = rtn.files
;total_nsubjects = rtn.count 
;START131015
gf=get_files(TITLE='Please specify the proper filter to load images corrected for multiple comparisons or region files.', $
    FILTER='*.4dfp.img')
if gf.msg eq 'GO_BACK' then goto,goback0a else if gf.msg eq 'EXIT' then return
if n_elements(gf.files) eq 1 and gf.msg eq 'DONTCHECK' then begin
    selected_mcomp_files = gf.files
    total_nsubjects = n_elements(gf.files)
endif else begin 
    goback1:
    rtn = select_files(gf.files,TITLE='Please select images corrected for multiple comparisons or region files.', $
        MIN_NUM_TO_SELECT=1,/GO_BACK)
    if rtn.files[0] eq 'GO_BACK' then goto,goback0
    selected_mcomp_files = rtn.files
    total_nsubjects = rtn.count 
endelse    


;goback2:
;af3d_files = get_files(TITLE='Please enter the default filter to load af3d files.',FILTER='*.3s*',OFFSET_SPAWN=offset_spawn,/GO_BACK);
;if af3d_files[0] eq 'GO_BACK' then goto,goback1
;goback3:
;selected_af3d_files = match_files(selected_mcomp_files,af3d_files,TITLE='Please select af3d file for ',/GO_BACK)
;if selected_af3d_files[0] eq 'GO_BACK' then goto,goback2
;START131015
goback2:
gf = get_files(TITLE='Please enter the default filter to load af3d files.',FILTER='*.3s*');
if gf.msg eq 'GO_BACK' then goto,goback1 else if gf.msg eq 'EXIT' then return
if n_elements(gf.files) eq 1 and gf.msg eq 'DONTCHECK' then begin
    af3d_files = gf.files
endif else begin 
    goback3:
    selected_af3d_files = match_files(selected_mcomp_files,af3d_files,TITLE='Please select af3d file for ',/GO_BACK)
    if selected_af3d_files[0] eq 'GO_BACK' then goto,goback2
endelse


goback4:
threshold_str = get_str(1,'Threshold ',3,TITLE='Please enter a threshold to apply to the af3d files.',WIDTH=40,/GO_BACK)
if threshold_str[0] eq 'GO_BACK' then goto,goback3
threshold_str = ' -threshold ' + strcompress(threshold_str[0],/REMOVE_ALL)
goback4a:
limit_str = get_str(1,'Diameter in mm ','200',/GO_BACK,TITLE='Please specify size of limiting sphere for region growth.', $
    LABEL='Enter 200 or larger for no limit.',WIDTH=50)
if limit_str[0] eq 'GO_BACK' then  goto,goback4
limit_str = ' -diameter_in_mm ' + strcompress(limit_str[0],/REMOVE_ALL)
goback5:
if total_nsubjects eq 1 then j = 1 else j = 4
dummy = j+total_nsubjects 
scrap = strarr(dummy)
scraplabels = strarr(dummy)
scrap[0] = 'Grow regions script'
scraplabels[0] = 'grow_regions.csh'
if total_nsubjects gt 1 then begin
    scrap[1] = 'AND image'
    scrap[2] = 'Number of subjects at each voxel'
    scrap[3] = 'Subjects that contributed an activated voxel'
    scraplabels[1] = 'AND.4dfp.img'
    scraplabels[2] = 'Nimage.4dfp.img'
    scraplabels[3] = 'Nimage.txt'
endif
for i=0,total_nsubjects-1 do begin
    if total_nsubjects gt 1 then junk = 'Subject '+strcompress(string(i+1),/REMOVE_ALL) else junk = ''
    scrap[i+j] = junk +' region file'
    start = strpos(selected_mcomp_files[i],"/",/REVERSE_SEARCH)+1
    scraplabels[i+j] = strmid(selected_mcomp_files[i],start,strpos(selected_mcomp_files[i],".4dfp.img")-start) + "_regions.4dfp.img"
endfor
names = get_str(dummy,scrap,scraplabels,WIDTH=50,TITLE='Please enter desired filenames. Leave blank any files ' $
    +'not of interest.',/GO_BACK)
if names[0] eq 'GO_BACK' then $
    goto,goback4a $
else if names[0] eq '' then $
    grow_regions_csh = 'grow_regions.csh' $
else begin
    grow_regions_csh = fix_script_name(names[0])
endelse
if total_nsubjects gt 1 then begin
    if names[1] ne '' then AND_str = ' -AND "' + names[1] + '"'
    if names[2] ne '' then Nimage_str = ' -Nimage "' + names[2] + '"'
    if names[3] ne '' then subject_contributions_str = ' -subject_contributions "' + names[3] + '"'
endif
regions_flag = intarr(total_nsubjects) 
for i=0,total_nsubjects-1 do if names[i+j] ne '' then regions_flag[i] = 1  
action = get_button(['execute '+grow_regions_csh+' script','return '+grow_regions_csh+' script','GO BACK'], $
    TITLE='Please select action.')
if action eq 2 then goto,goback5
openw,lu_csh,grow_regions_csh,/GET_LUN

;printf,lu_csh,FORMAT='("#!/bin/csh")'
;printf,lu_csh,FORMAT='("unlimit")'
;printf,lu_csh,''
;START131015
top_of_script,lu_csh

print_files_to_csh,lu_csh,total_nsubjects,selected_mcomp_files,'MCOMP_FILES','mcomp_files'
print_files_to_csh,lu_csh,total_nsubjects,selected_af3d_files,'AF3D_FILES','af3d_files'
if total(regions_flag) ge 1 then begin
    if total_nsubjects ge 2 then begin
        printf,lu_csh,'set REGION_FILENAMES = (-region_filenames  \'
        for i=0,total_nsubjects-2 do $
            if regions_flag[i] eq 1 then printf,lu_csh,'        ',names[i+j],' \',FORMAT='(a,a,a)'
        if regions_flag[i] eq 1 then printf,lu_csh,'        ',names[i+j],')',FORMAT='(a,a,a)'
    endif else begin
        printf,lu_csh,'set REGION_FILENAMES = (-region_filenames '+names[j]+')'
    endelse
    printf,lu_csh,''
    region_files_str = ' $REGION_FILENAMES'
endif

;printf,lu_csh,FORMAT='("nice +19 ",a,"/grow_regions $MCOMP_FILES $AF3D_FILES",a,a,a,a,a,a,a," |& mail `whoami`")', $
;    !BIN,region_files_str,threshold_str,AND_str,Nimage_str,subject_contributions_str,mask_str,limit_str
;START131015
printf,lu_csh,'nice +19 $BIN/grow_regions $MCOMP_FILES $AF3D_FILES'+region_files_str+threshold_str+AND_str+Nimage_str $
    +subject_contributions_str+mask_str+limit_str

close,lu_csh
free_lun,lu_csh
spawn,'chmod +x '+grow_regions_csh
if action eq 0 then begin
    spawn,grow_regions_csh+' &'

    ;stats = widget_message(grow_regions_csh+' submitted as batch job.  The log file(s) will be e-mailed to you upon completion.')
    ;START131015
    stats = dialog_message(grow_regions_csh+' submitted as batch job.',/INFORMATION)

endif
print,'DONE'
end
