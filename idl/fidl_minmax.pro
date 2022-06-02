;Copyright 1/10/05 Washington University.  All Rights Reserved.
;fidl_minmax.pro  $Revision: 1.3 $
pro fidl_minmax,fi,wd,dsp,help,stc
minstr = ''
maxstr = ''
goback1:

;files = get_files(TITLE='Please specify the proper filter to load 4dfps for min and max',FILTER='*.4dfp.img',/EXIT);
;if files[0] eq 'EXIT' then return 
;goback2:
;rtn = select_files(files,TITLE='Please select 4dfps for min and max.',MIN_NUM_TO_SELECT=2,/GO_BACK)
;START131015
gf = get_files(TITLE='Please specify the proper filter to load 4dfps for min and max',FILTER='*.4dfp.img');
if gf.msg eq 'GO_BACK' or gf.msg eq 'EXIT' then return 
goback2:
rtn = select_files(gf.files,TITLE='Please select 4dfps for min and max.',MIN_NUM_TO_SELECT=2,/GO_BACK)

if rtn.files[0] eq 'GO_BACK' then goto,goback1
files = rtn.files
total_nsubjects = rtn.count
goback3:
get_directory,directory
dummy = 3 
scrap = strarr(dummy)
scraplabels = strarr(dummy)
scrap[0] = 'Min and max script' 
scraplabels[0] = directory+'/fidl_minmax.csh'
scrap[1] = 'min name' 
scraplabels[1] = directory+'/min.4dfp.img'
scrap[2] = 'max name' 
scraplabels[2] = directory+'/max.4dfp.img'
names = get_str(dummy,scrap,scraplabels,WIDTH=100,TITLE='Please enter desired filenames. Leave blank any files ' $
    +'not of interest.',/GO_BACK)
if names[0] eq 'GO_BACK' then $
    goto,goback2 $
else if names[0] eq '' then $
    csh = 'fidl_minmax.csh' $
else $
    csh = fix_script_name(names[0])
if names[1] ne '' then minstr = ' -min "' + names[1] + '"'
if names[2] ne '' then maxstr = ' -max "' + names[2] + '"'
action = get_button(['execute','return','GO BACK'],TITLE='Please select action.'+string(10B)+string(10B) $
    +'To run on linux, please select return. Login to a linux machine and enter the script on the command line.',BASE_TITLE=csh)
if action eq 0 then $
    mailstr = '' $
else if action eq 1 then $
    mailstr =  ' |& mail `whoami`' $
else $
    goto,goback3
openw,lu,csh,/GET_LUN
top_of_script,lu
print_files_to_csh,lu,total_nsubjects,files,'FILES','files'
printf,lu,'nice +19 $BIN/fidl_minmax $FILES'+minstr+maxstr+mailstr
close,lu
free_lun,lu
spawn,'chmod +x '+csh
if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc
print,'DONE'
end
