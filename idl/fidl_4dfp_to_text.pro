;Copyright 2/18/04 Washington University.  All Rights Reserved.
;fidl_4dfp_to_text.pro  $Revision: 1.5 $
pro fidl_4dfp_to_text,fi,dsp,wd,help,stc,pref
if get_button(['Ok','Exit'],BASE_TITLE='4dfp to test',TITLE='This program outputs outputs image ' $
    +'voxels to a text file.'+string(10B)+'Voxels are selected from a mask.'+string(10B)+'You may use the statistical image ' $
    +'as the mask.') eq 1 then return
idx_data = get_bolds(fi,dsp,wd,glm,help,stc,FILTER='*.4dfp.img',TITLE='Please load statistical image(s).',/MULTIPLE_FILES)
idx_mask = get_bolds(fi,dsp,wd,glm,help,stc,FILTER='*.4dfp.img',TITLE='Please load mask.')
goback0:
names = get_str(2,['script','output'],['fidl_4dfp_to_text.csh',fi.list[idx_data[0]]+'_voxval.txt'],WIDTH=100, $
    TITLE='Please enter.')
csh = fix_script_name(names[0])
openw,lu,csh,/GET_LUN
top_of_script,lu
print_files_to_csh,lu,n_elements(idx_data),fi.names[idx_data],'FILES','files'
;printf,lu,'nice +19 $BIN/fidl_4dfp_to_text -files '+fi.names[idx_data]+' -mask '+fi.names[idx_mask]+' -output '+names[1]
printf,lu,'nice +19 $BIN/fidl_4dfp_to_text $FILES -mask '+fi.names[idx_mask]+' -output '+names[1]
close,lu
free_lun,lu
spawn,'chmod +x '+csh
action = get_button(['execute','return','GO BACK'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 2 then goto,goback0
if action eq 0 then begin
    spawn,csh+' &'
    stats = dialog_message('Anova script submitted as batch job.'+string(10B) $
        +'The log file will be e-mailed to you upon completion.',/INFORMATION)
endif
print,'DONE'
end
