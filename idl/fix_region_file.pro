;Copyright 7/26/01 Washington University.  All Rights Reserved.
;fix_region_file.pro  $Revision: 1.11 $
pro fix_region_file,fi,wd,dsp,help,stc,pref
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='This program corrects FIDL region files that have the ' $
    +'first region defined as 1 instead of 2.'

;START121030
scrap = get_button(['ok','exit'],TITLE='This program corrects files that have the first region ' $
    +'defined as 1 instead of 2.'+string(10B)+'It will also create ifh tags for the regions.')
if scrap eq 1 then return
goback0:
idx = get_bolds(fi,dsp,wd,glm,help,stc,FILTER='*.4dfp.img',TITLE='Please select files.',/MULTIPLE_FILES)
if idx[0] eq -1 then return
nfiles = n_elements(idx)
scrap1=''
scrap2=''
idx1=-1
for i=0,nfiles-1 do begin
    hdr = *fi.hdr_ptr[idx[i]]
    ifh = hdr.ifh
    if ifh.nreg eq 0 then begin
        scrap1 = [scrap1,fi.tails[idx[i]]]
        scrap2 = [scrap2,'reg']
        idx1 = [idx1,idx[i]]
    endif
endfor
goback1:

regnamesnew = make_array(nfiles,/STRING,VALUE='')
;KEEP
;if n_elements(scrap1) gt 1 then begin
;    scrap1 = scrap1[1:*]
;    scrap2 = scrap2[1:*]
;    idx1 = idx1[1:*]
;    scrap = get_str(n_elements(scrap1),scrap1,scrap2,WIDTH=60,LABEL='Please enter region names.',/GO_BACK)
;    if scrap[0] eq 'GO_BACK' then goto,goback0
;    regnamesnew[idx1]=' -regname '+scrap
;endif

goback2:
files = fi.names[idx]
rtn_gr = get_root(files)
scrap = rtn_gr.file 
names = get_str(nfiles+1,['script',scrap],['fix_region_file.csh',scrap],WIDTH=60,TITLE='Please enter desired filenames.', $
    LABEL='You may retain the same names if you so desire.',/GO_BACK)
if names[0] eq 'GO_BACK' then begin
    if n_elements(scrap1) gt 1 then goto,goback1 else goto,goback0
endif
fix_region_file_csh = fix_script_name(names[0])
names = names[1:*]
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=fix_region_file_csh)
if action eq 2 then goto,goback2
openw,lu,fix_region_file_csh,/GET_LUN
top_of_script,lu
for i=0,nfiles-1 do printf,lu,'$BIN/fix_region_file -files '+fi.names[idx[i]]+' -new_filenames '+names[i]+regnamesnew[i]
close,lu
free_lun,lu
spawn,'chmod +x '+fix_region_file_csh
if action eq 0 then spawn_cover,fix_region_file_csh,fi,wd,dsp,help,stc
print, 'DONE'
end

;goback1:
;title = 'Please select region file.'
;if help.enable eq !TRUE then widget_control,help.id,SET_VALUE=title
;if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr_region,idx_region,'region file',TITLE=title) eq !ERROR then return
;files = fi.names[idx_region]
;nfiles = n_elements(fi.names[idx_region])
;goback2:
;rtn_gr = get_root(files)
;scrap = rtn_gr.file 
;names = get_str(nfiles+1,['script',scrap],['fix_region_file.csh',scrap],WIDTH=60,TITLE='Please enter desired filenames.', $
;    LABEL='You may retain the same names if you so desire.',/GO_BACK)
;if names[0] eq 'GO_BACK' then goto,goback1
;fix_region_file_csh = fix_script_name(names[0])
;names = names[1:*]
;action = get_button(['execute','return','GO BACK'],TITLE='Please select action.'+string(10B)+string(10B)+'To run on linux, ' $
;    +'please select return. Login to a linux machine and enter the script on the command line.',BASE_TITLE=fix_region_file_csh)
;if action eq 2 then goto,goback2
;mail_str = ''
;openw,lu_csh,fix_region_file_csh,/GET_LUN
;top_of_script,lu_csh
;print_files_to_csh,lu_csh,nfiles,files,'FILES','files'
;print_files_to_csh,lu_csh,nfiles,files,'FILES','files'
;print_files_to_csh,lu_csh,nfiles,names,'NEW_FILENAMES','new_filenames'
;printf,lu_csh,'$BIN/fix_region_file $FILES $NEW_FILENAMES '+mail_str
;close,lu_csh
;free_lun,lu_csh
;spawn,'chmod +x '+fix_region_file_csh
;if action eq 0 then spawn_cover,fix_region_file_csh,fi,wd,dsp,help,stc
;print, 'DONE'
;print,''
;end
