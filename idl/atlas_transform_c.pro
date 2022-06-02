;Copyright 6/21/01 Washington University.  All Rights Reserved.
;atlas_transform_c.pro  $Revision: 1.16 $
pro atlas_transform_c,fi,dsp,wd,stc,help
directory = getenv('PWD')
concstr = ''
data_idx = get_bolds(fi,dsp,wd,glm,help,stc)
if data_idx lt 0 then return
nrun = stc[data_idx].n
files = *stc[data_idx].filnam
goback4a:
t4filestr=''
get_dialog_pickfile,'*anat_ave_to_*_t4',fi.path[0],'Please select transformation matrix. Enter NONE if none.',t4_file, $
    rtn_nfiles,rtn_path
if t4_file eq 'GOBACK' or t4_file eq 'EXIT' then return
if t4_file eq 'NONE' then $
    print,'No t4 file entered, assuming the identity matrix.' $
else begin 
    if get_t4_type(t4_file) eq 2. then return
    t4filestr = ' -xform_file '+t4_file
endelse
rtn=select_space()
space=rtn.space
atlas=rtn.atlas
atlas_str=rtn.atlas_str
if get_atlas_param(atlas,mmppix,center,ap_xdim,ap_ydim,ap_zdim,area,vol,ap_voxel_size,ap_str) eq !ERROR then return
if nrun eq 1 then $
    dummy = 2 $
else begin
    dummy = fi.tails[data_idx]
    scrap = strpos(dummy,'.conc',/REVERSE_SEARCH)
    if scrap ne -1 then dummy = strmid(dummy,0,scrap)
    concstr = dummy + ap_str + '.conc'
    dummy = 3
endelse
scrap = strarr(dummy)
scraplabels = strarr(dummy)
scrap[0] = 'script'
scrap[1] = 'put atlas files here'
scraplabels[0] = 'atlas.csh'
scraplabels[1] = directory 
if dummy eq 3 then begin
    scrap[2] = 'output conc'
    scraplabels[2] = concstr 
endif
goback0:
names = get_str(dummy,scrap,scraplabels,WIDTH=50,TITLE='Please enter names.',/GO_BACK)
if names[0] eq 'GO_BACK' then goto,goback4a
csh = fix_script_name(names[0])
if strmid(names[1],strlen(names[1])-1) ne '/' then outdir = names[1] + '/'
if dummy eq 3 then concstr = ' -conc_name ' + fix_script_name(names[2])
csh = names[0] 
openw,lu,csh,/GET_LUN
top_of_script,lu
printf,lu,''
print_files_to_csh,lu,nrun,files,'BOLD_FILES','bold_files'
printf,lu,'nice +19 $BIN/atlas_transform '+'$BOLD_FILES -directory '+outdir+t4filestr+atlas_str+concstr
close,lu
free_lun,lu
spawn,'chmod +x '+csh
action = get_button(['execute','return','GO BACK'],TITLE='Please select action.',BASE_TITLE=fidl_anova_csh)
if action eq 2 then goto,goback0
if action eq 0 then begin
    spawn,csh+ '>'+csh+'.log &'
    stats = dialog_message(csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
