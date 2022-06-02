;Copyright 2/17/14 Washington University.  All Rights Reserved.
;fidl_txt2conc_subcol.pro  $Revision: 1.3 $
pro fidl_txt2conc_subcol,fi,dsp,wd,help,pref
if dialog_message('This program puts values from a text file into an image.'+string(10B)+'Each column of the text file is a run. ' $
    +'Columns are subtracted in pairs. col1-1col2 col3-col4.'+string(10B)+'Each difference is output to a 4dfp.',/INFORMATION, $
    /CANCEL) eq 'Cancel' then return 
directory = getenv('PWD')
goback0:
get_dialog_pickfile,'*.txt',directory,'Please select text file(s).',rtn_filenames,rtn_nfiles,rtn_path,/MULTIPLE_FILES
if rtn_files eq 0 then return
goback3:
labels = ['script','root '+trim(indgen(rtn_nfiles)+1)]
spawn,!BINEXECUTE+'/fidl_timestr',timestr
rtn=get_root(rtn_filenames,'.',/KEEPPATH)
names = ['fidl_txt2conc_subcol_'+timestr[0]+'.csh',rtn.file]
names = get_str(n_elements(labels),labels,names,WIDTH=100,TITLE='Please enter desired names.',/REPLACE,/GO_BACK,/ONE_COLUMN,/EXIT, $
    FRONT=directory+'/')
if names[0] eq 'EXIT' then return else if names[0] eq 'GO_BACK' then goto,goback0
csh = fix_script_name(names[0])
openw,lu,csh,/GET_LUN
top_of_script,lu
for i=0,rtn_nfiles-1 do printf,lu,'nice +19 $BIN/fidl_txt2conc_subcol -file '+rtn_filenames[i]+' -root '+names[i+1]
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
