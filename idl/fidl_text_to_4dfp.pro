;Copyright 6/25/10 Washington University.  All Rights Reserved.
;fidl_text_to_4dfp.pro  $Revision: 1.4 $
pro fidl_text_to_4dfp,fi,dsp,wd,help,pref
if dialog_message('This program puts values from a text file into an image.'+string(10B)+'Each column of the text file is a ' $
    +'region.'+string(10B)+'Obviously, you need to provide a region file.',/INFORMATION,/CANCEL) eq 'Cancel' then return 
directory = getenv('PWD')
goback0:
get_dialog_pickfile,'*.txt',directory,'Please select text file.',rtn_filenames,rtn_nfiles,rtn_path,/MULTIPLE_FILES
if rtn_nfiles eq 0 then return
goback1:
widget_control,/HOURGLASS
rtn_getreg = get_regions(fi,wd,dsp,help)
if rtn_getreg.msg eq 'EXIT' then return else if rtn_getreg.msg eq 'GO_BACK' then goto,goback0
nregfiles = rtn_getreg.nregfiles
region_file = rtn_getreg.region_file
goback2:
rtn = select_files(rtn_getreg.region_names,TITLE='Please select regions you wish to analyze.',/GO_BACK)
if rtn.files[0] eq 'GO_BACK' then goto,goback1
roistr = ' -regions_of_interest '+strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)
goback3:
labels = ['script','output '+strtrim(indgen(rtn_nfiles)+1,2)]
rtn=get_root(rtn_filenames,'.')
names = ['fidl_text_to_4dfp.csh',rtn.file+'.4dfp.img']
names = get_str(n_elements(labels),labels,names,WIDTH=100,TITLE='Please enter desired names.',/REPLACE)
csh = fix_script_name(names[0])
openw,lu,csh,/GET_LUN
top_of_script,lu
print_files_to_csh,lu,nregfiles,region_file,'REGION_FILE','region_file'
printf,lu,'set ROI = ('+strtrim(roistr,2)+')'
for i=0,rtn_nfiles-1 do begin
    printf,lu,'nice +19 $BIN/fidl_text_to_4dfp -file '+rtn_filenames[i]+' $REGION_FILE $ROI -out '+names[i+1]
endfor
close,lu
free_lun,lu
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 2 then goto,goback3
spawn,'chmod +x '+csh
if action eq 0 then begin
    spawn,csh+'>'+csh+'.log &'
    stat = dialog_message(csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
