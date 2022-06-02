;Copyright 12/31/99 Washington University.  All Rights Reserved.
;logical_or.pro  $Revision: 12.91 $
pro logical_or,fi,wd,dsp,help,stc,pref
goback0:
scrap=get_glms(pref,fi,GET_THIS='imgs')
if scrap.msg eq 'GO_BACK' or scrap.msg eq 'EXIT' then return
files = scrap.imgselect
nfiles = scrap.total_nsubjects
goback1:
scrap = ['Script','Output file']
scraplabels = ['fidl_or.csh','mask.4dfp.img']
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter names.',/GO_BACK)
if names[0] eq 'GO_BACK' then goto,goback0
names = fix_script_name(names)
script = names[0]
out = names[1]
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=script)
if action eq 2 then goto,goback1
openw,lu,script,/GET_LUN
top_of_script,lu
print_files_to_csh,lu,nfiles,files,'FILES','files'
printf,lu,'nice +19 $BIN/fidl_or $FILES -out '+out
close,lu
free_lun,lu
spawn,'chmod +x '+script
if action eq 0 then spawn_cover,script,fi,wd,dsp,help,stc
print,'DONE'
end
