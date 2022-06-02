;Copyright 5/4/07 Washington University.  All Rights Reserved.
;fidl_lda.pro  $Revision: 1.10 $
pro fidl_lda,fi,dsp,wd,glm,help,stc,pref
directory = getenv('PWD')
goback5:




;scrap=get_glms(GET_THIS='imgs')
;if scrap.msg ne 'OK' then return
;load = scrap.load
;glmfiles = scrap.glmfiles
;nlists = scrap.nlists
;total_nsubjects = scrap.total_nsubjects
;t4select = scrap.t4select
;glm_list_str = scrap.glm_list
;concselect = scrap.concselect
;event_files = scrap.evselect
;goback8:
;glm_space = intarr(total_nsubjects)
;vol = lonarr(total_nsubjects)
;widget_control,/HOURGLASS
;for i=0,total_nsubjects-1 do begin
;    ifh = read_mri_ifh(glmfiles[i])
;    glm_space[i] = get_space(ifh.matrix_size_1,ifh.matrix_size_2,ifh.matrix_size_3)
;    vol[i] = ifh.matrix_size_1*ifh.matrix_size_2*ifh.matrix_size_3*ifh.matrix_size_4
;endfor
;START110711
scrap=get_glms(GET_THIS='imgs')
if scrap.msg ne 'OK' then return
total_nsubjects = scrap.total_nsubjects
glmfiles = scrap.concselect
glm_space = scrap.bold_space
ifh = scrap.ifh
vol = lonarr(total_nsubjects)
widget_control,/HOURGLASS
for i=0,total_nsubjects-1 do vol[i] = ifh[i].matrix_size_1*ifh[i].matrix_size_2*ifh[i].matrix_size_3*ifh[i].matrix_size_4




if total(vol-vol[0]) ne 0 then begin
    scrap = get_button(['Try again','Exit'],BASE_TITLE='ERROR',WIDTH=200,TITLE='All imgs must have the same dimensions.')
    if scrap eq 0 then goto,goback5
    return
endif
goback9:
mask_str = ''
diameter_str = ''
scrap = get_button(['regional','searchlight','go back','exit'],BASE_TITLE='Analysis type.',WIDTH=55,TITLE='Please select.')

;if scrap eq 3 then return else if scrap eq 2 then goto,goback8
;START110701
if scrap eq 3 then return else if scrap eq 2 then goto,goback5

if scrap eq 1 then begin
    goback91:
    mask  = get_mask(glm_space[0],fi)
    if mask eq 'GO_BACK' then goto,goback9
    if mask ne '' then mask_str = ' -mask ' + mask
    scrap = get_str(1,'diameter in mm','12',WIDTH=40,TITLE='Searchlight sphere.',/ONE_COLUMN,/GO_BACK)
    if scrap[0] eq 'GO_BACK' then goto,goback91
    diameter = strtrim(scrap[0],2)
    diameter_str = ' -diameter '+diameter
    diameterf = float(diameter)

    ;nvox=call_external(!SHARE_LIB,'_sphere_nvox',ifh.matrix_size_1*ifh.matrix_size_2*ifh.matrix_size_3,ifh.matrix_size_1, $
    ;    ifh.matrix_size_2,ifh.matrix_size_3,diameterf,ifh.scale_1,VALUE=[1,1,1,1,0,0])
    ;START110711
    nvox=call_external(!SHARE_LIB,'_sphere_nvox',ifh[0].matrix_size_1*ifh[0].matrix_size_2*ifh[0].matrix_size_3, $
        ifh[0].matrix_size_1,ifh[0].matrix_size_2,ifh[0].matrix_size_3,diameterf,ifh[0].scale_1,VALUE=[1,1,1,1,0,0])

    print,'nvox=',nvox
    if nvox eq 0 then begin
        stat=dialog_message("Error in _sphere_nvox.",/ERROR)
        return
    endif
endif else begin
    goback10:
    rtn = get_regions(fi,wd,dsp,help)
    if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then goto,goback5
    region_names = rtn.region_names
    region_files = rtn.region_file
    nregion_files = rtn.nregfiles
    region_space = rtn.space
    if glm_space[0] ne region_space then begin
        scrap = get_button(['Try again','Exit'],BASE_TITLE='ERROR',WIDTH=200,TITLE='Region file and imgs are not in the same space.')
        if scrap eq 0 then goto,goback10
        return
    endif
    goback15:
    rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.',/GO_BACK)
    if rtn.files[0] eq 'GO_BACK' then goto,goback10
    roi_str = ' -regions_of_interest ' + strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)
endelse



;dummy=1
;scrap = strarr(dummy)
;scraplabels = strarr(dummy)
;scrap[0] = 'script'
;scraplabels[0] = 'fidl_lda.csh'
;names = get_str(dummy,scrap,scraplabels,WIDTH=100,TITLE='Please enter filenames.',/ONE_COLUMN,/GO_BACK,FRONT=directory+'/',/REPLACE)
;if names[0] eq 'GO_BACK' then goto,goback15 
;script = fix_script_name(names[0])
;START110204
scrap = ['script','output']
if mask_str eq '' then $ 
    outfile = 'fidl_lda.txt' $
else $ 

    ;outfile = 'fidl_lda_'+strtrim(total_nsubjects,2)+'classes_'+diameter+'mmsphere.4dfp.img' 
    ;START110418
    outfile = 'fidl_lda_'+strtrim(total_nsubjects,2)+'classes_'+diameter+'mmsphere'+strtrim(nvox,2)+'vox.4dfp.img' 

scraplabels = ['fidl_lda.csh',outfile]
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter filenames.',/ONE_COLUMN,/GO_BACK, $
    FRONT=directory+'/',/REPLACE)
if names[0] eq 'GO_BACK' then goto,goback15
names = fix_script_name(names)
script = names[0]
outstr = ''
if names[1] ne '' then outstr = ' -out '+names[1]



openw,lu,script,/GET_LUN
top_of_script,lu
print_files_to_csh,lu,total_nsubjects,glmfiles,'FILES','files',/NO_NEWLINE
if mask_str eq '' then begin
    print_files_to_csh,lu,nregion_files,region_files,'REGION_FILE','region_file',/NO_NEWLINE
    printf,lu,'set ROI = ('+strtrim(roi_str[0],2)+')'

    ;printf,lu,'nice +19 $BIN/fidl_lda2 $FILES $REGION_FILE $ROI'+outstr
    ;START110726
    printf,lu,'nice +19 $BIN/fidl_lda3 $FILES $REGION_FILE $ROI'+outstr

endif else $ 

    ;printf,lu,'nice +19 $BIN/fidl_lda2 $FILES'+mask_str+diameter_str+outstr
    ;START110726
    printf,lu,'nice +19 $BIN/fidl_lda3 $FILES'+mask_str+diameter_str+outstr

close,lu
free_lun,lu
spawn,'chmod +x '+script

action = get_button(['execute','return'],TITLE='Please select action.',BASE_TITLE=script)
if action eq 0 then begin
    spawn,script+' &'
    stats = dialog_message(script+string(10B)+string(10B)+'LDA script submitted as batch job.',/INFORMATION)
endif

print,'DONE'
end
