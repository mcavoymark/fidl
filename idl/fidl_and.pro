;Copyright 9/4/09 Washington University.  All Rights Reserved.
;fidl_and.pro  $Revision: 1.4 $

pro fidl_and,fi,wd,dsp,help,stc
anat_ave_str = ''
lc_anat_ave = 0
sd_str = ''
mail_str = ''
directory=getenv('PWD')
goback1:

;files = get_files(TITLE='Please specify filter.',FILTER='*.4dfp.img',/EXIT);
;if files[0] eq 'EXIT' then return 
;START131015
gf = get_files(TITLE='Please specify filter.',FILTER='*.4dfp.img')
if gf.msg eq 'GO_BACK' or gf.msg eq 'EXIT' then return
files = gf.files


goback2:
rtn = select_files(files,TITLE='Please select 4dfps.',MIN_NUM_TO_SELECT=1,/GO_BACK)
if rtn.files[0] eq 'GO_BACK' then goto,goback1
files = rtn.files
nfiles = rtn.count
goback3:
scrap = ['Script','Output file'] 

;scraplabels = ['fidl_and.csh','fidl_and.4dfp.img'] 
;START111129
if nfiles gt 2 then $
    spider = 'fidl_and.4dfp.img' $
else begin
    rtn = get_root(files,'.4dfp.img')
    spider = rtn.file[0] + '_and_' + rtn.file[1] + '.4dfp.img'
endelse

;scraplabels = ['fidl_and.csh',spider] 
;START180706
spawn,!BINEXECUTE+'/fidl_timestr2',timestr
scraplabels = ['fidl_and_'+timestr[0]+'.csh',spider] 

names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter names.',/GO_BACK)
if names[0] eq 'GO_BACK' then goto,goback2
names = fix_script_name(names)
script = names[0]
out = names[1]
action = get_button(['execute','return','GO BACK'],TITLE='Please select action.',BASE_TITLE=script)
if action eq 2 then goto,goback3

openw,lu,script,/GET_LUN
top_of_script,lu
print_files_to_csh,lu,nfiles,files,'FILES','files'
printf,lu,'nice +19 $BIN/fidl_and $FILES -out '+out
close,lu
free_lun,lu
spawn,'chmod +x '+script

if action eq 0 then spawn_cover,script,fi,wd,dsp,help,stc
print,'DONE'
end
