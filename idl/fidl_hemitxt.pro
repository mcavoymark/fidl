;Copyright 4/20/11 Washington University.  All Rights Reserved.
;fidl_hemitxt.pro  $Revision: 1.4 $
pro fidl_hemitxt,fi,wd,dsp,help,stc,pref
directory = getenv('PWD')
if dialog_message('This program takes a mask, a left seed map and right seed map.'+string(10B)+'Two text files are output: ' $
    +'left hemisphere voxels and right hemisphere voxels.'+string(10B)+'Each text file has two columns: 1, left seed map ' $
    +'voxel values; 2, right seed map voxel values.',/INFORMATION,/CANCEL) eq 'Cancel' then return
goback0:
get_dialog_pickfile,'*.4dfp.img',directory,'Please select left seed map.',Lseedmap
if Lseedmap eq 'GOBACK' or Lseedmap eq 'EXIT' then return 
goback1:
get_dialog_pickfile,'*.4dfp.img',directory,'Please select right seed map.',Rseedmap
if Rseedmap eq 'GOBACK' then goto,goback0 else if Rseedmap eq 'EXIT' then return 
goback2:
get_dialog_pickfile,'*.4dfp.img',directory,'Please select mask.',mask
if mask eq 'GOBACK' then goto,goback1 else if mask eq 'EXIT' then return 
goback3:
scrap = ['Script','Output - left hemisphere voxels','Output - right hemisphere voxels']
scraplabels = ['fidl_hemitxt.csh','Lhemivox.txt','Rhemivox.txt']
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter names.',/GO_BACK)
if names[0] eq 'GO_BACK' then goto,goback2
names = fix_script_name(names)
script = names[0]
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=script)
if action eq 2 then goto,goback3
openw,lu,script,/GET_LUN
top_of_script,lu
printf,lu,'nice +19 $BIN/fidl_hemitxt -Lseedmap '+Lseedmap+' -Rseedmap '+Rseedmap+' -mask '+mask+' -Lhemivox '+names[1] $
    +' -Rhemivox '+names[2]
close,lu
free_lun,lu
spawn,'chmod +x '+script
if action eq 0 then begin
    spawn,'source fidl_timestr',timestr
    spawn,'source '+script+'>'+script+'_'+timestr+'.log &' 
    stat = get_button('ok',TITLE='Script has been executed.',BASE_TITLE=script)
endif
print,'DONE'
end
