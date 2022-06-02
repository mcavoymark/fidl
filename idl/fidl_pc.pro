;Copyright 12/22/05 Washington University.  All Rights Reserved.
;fidl_pc.pro  $Revision: 1.18 $
pro fidl_pc,fi,dsp,wd,glm,help,stc,pref
gauss_str = ''
mailstr = ''
directory = getenv('PWD')
cor_part = get_button(['correlation','partial correlation','exit'],TITLE='Analysis')
if cor_part eq 2 then return    
goback0:
scrap=get_glms(pref,/GET_CONC)
if scrap.msg ne 'OK' then return
load = scrap.load
glmfiles = scrap.glmfiles
nlists = scrap.nlists
total_nsubjects = scrap.total_nsubjects
t4select = scrap.t4select
glm_list_str = scrap.glm_list
concselect = scrap.concselect
evselect = scrap.evselect
lc_est_in_atlas = 0
lc_atlas = 0
if strmid(concselect[0],strlen(concselect[0])-3) eq 'img' then begin
    scrap = get_button(['proceed','exit','go back'],TITLE='Each file is treated as a separate subject.'+string(10B) $
        +'If you wish to analyze sets of bold runs, then load concs.',BASE_TITLE='Information')
    if scrap eq 1 then return else if scrap eq 2 then goto,goback0    
    lc_concs = 0
endif else begin
    lc_concs = 1
endelse
t4_identify_str = make_array(total_nsubjects,/STR,vALUE='')
space = intarr(total_nsubjects)
stc2 = replicate({Stitch},total_nsubjects)
ifh2 = replicate({InterFile_Header},total_nsubjects)
for m=0,total_nsubjects-1 do begin
    print,'Loading ',concselect[m]
    rload_conc=load_conc(fi,stc,dsp,help,wd,concselect[m],/DONT_PUT_IMAGE)
    if rload_conc.msg eq 'GOBACK' then goto,goback0 else if rload_conc.msg eq 'EXIT' then return
    stc2=rload_conc.stc1
    ifh2[m]=rload_conc.ifh
    space[m] = get_space(ifh2[m].matrix_size_1,ifh2[m].matrix_size_2,ifh2[m].matrix_size_3)
endfor
if total(space - space[0]) ne 0 then begin
    stat=dialog_message('Not all images are in the same space. Abort!',/ERROR)
    return
endif
max_tdim = min(stc2[*].tdim_all,index)
print,'max_tdim=',max_tdim
bold_space = space[0]

;START150130
;bold_ifh = ifh[0]

goback0a:
atlasspace = bold_space

;bold_size = long(bold_ifh.matrix_size_1)*long(bold_ifh.matrix_size_2)*long(bold_ifh.matrix_size_3)
;START150130
bold_size = long(ifh2[0].matrix_size_1)*long(ifh2[0].matrix_size_2)*long(ifh2[0].matrix_size_3)

if bold_space ne !SPACE_111 and bold_space ne !SPACE_222 and bold_space ne !SPACE_333 and bold_size ne 1 then begin
    bs = 1
    goback100a:
    rtn=select_space()
    atlasspace=rtn.space
    atlas=rtn.atlas
    atlas_str=rtn.atlas_str
    lc_atlas = 1
    if t4select[0] eq '' then begin
        goback1:

        ;t4select = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_711-2?_t4',/GO_BACk)
        ;if t4select[0] eq 'GO_BACk' then goto,goback100a
        ;START131015
        gf = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_711-2?_t4')
        if gf.msg eq 'GO_BACK' then goto,goback100a else if gf.msg eq 'EXIT' then return
        t4select = gf.files

        if t4select[0] ne 'NONE' then begin
            scrap = ''
            neach = intarr(total_nsubjects)
            for i=0,total_nsubjects-1 do begin
                rtn_gol = get_ordered_list(t4select,indgen(1000)+1,TITLE='Please select t4(s) for '+concselect[i])
                if rtn_gol.count ne 0 then begin
                    list = rtn_glm.labels_sorted
                    scrap = [scrap,list]
                    neach[i] = n_elements(list)
                endif
            endfor
        endif
        lc_est_in_atlas = 1
        t4select = scrap[1:*]
        if total(neach-1) ne 0 then begin
            tneach = total(neach)
            dummy = strarr(tneach)
            k = 0
            for i=0,total_nsubjects-1 do begin
                for j=0,neach[i]-1 do begin
                    dummy[k] = concselect[i] + string(10B) + t4select[k]
                    k = k + 1
                endfor
            endfor
            scrap = make_array(tneach,/STRING,VALUE='0')
            goback1b:
            scrap = get_str(tneach,dummy,scrap,WIDTH=50,TITLE='Please assign runs to t4s. First run is 1.', $
                LABEL='Elements can be separated by spaces, tabs, or commas. ex. 1-10,11-24',/GO_BACK,/ONE_COLUMN,/BELOW)
            if scrap[0] eq 'GO_BACK' then goto,goback1
            k = 0
            for i=0,total_nsubjects-1 do begin
                t4_identify = intarr(1000)
                for j=1,neach[i] do begin
                    segments = strsplit(scrap[k],'[ '+string(9B)+',]',/REGEX,/EXTRACT)
                    nseg = n_elements(segments)
                    for seg=0,nseg-1 do begin
                        limits = long(strsplit(segments[seg],'-',/EXTRACT)) - 1
                        if n_elements(limits) eq 1 then limits = [limits[0],limits[0]]
                        ;print,'limits=',limits,' j=',j
                        t4_identify[limits[0]:limits[1]] = j
                    endfor
                    k = k + 1
                endfor
                index = where(t4_identify,count)
                if count eq 0 then begin
                    stat=dialog_message('You have failed to assign runs from '+concselect[i]+string(10B)+'Please try again.')
                    goto,goback1b
                endif
                t4_identify = strtrim(t4_identify[index],2)
                t4_identify_str[i] = ' -t4'
                for j=0,count-1 do t4_identify_str[i] = t4_identify_str[i] + ' ' + t4_identify[j]
            endfor
        endif
    endif
endif else begin
    lc_est_in_atlas = 1
    bs = 0
endelse

goback2:
scrap = get_str(1,'Enter NONE if none.','1-4,129-132',TITLE='Exclude frames',LABEL='Your files have a maximum of ' $
    +strtrim(max_tdim,2)+' frames.'+string(10B)+'First frame is 1.'+string(10B)+'Ex. 1-4,129-132',/GO_BACK,WIDTH=50)
if scrap[0] eq 'GO_BACK' then goto,goback0a
if scrap[0] ne 'NONE' then begin
    segments = strsplit(scrap[0],'[ '+string(9B)+',]',/REGEX,/EXTRACT)
    nseg = n_elements(segments)
    exclude_frames = -1
    for seg=0,nseg-1 do begin
        limits = long(strsplit(segments[seg],'-',/EXTRACT))
        if n_elements(limits) eq 1 then limits = [limits[0],limits[0]]
        subtract = limits[1] - limits[0]
        exclude_frames = [exclude_frames,indgen(subtract+1) + limits[0]]
    endfor
    exclude_frames = exclude_frames[1:*]
    exclude_frames_str = ' -exclude_frames '+strjoin(strtrim(exclude_frames,2),' ',/SINGLE)
    print,'exclude_frames_str=',exclude_frames_str
endif

vox_reg = get_button(['regions with voxels','regions with regions','go back'],TITLE='Correlate')
if vox_reg eq 2 then goto,goback2

goback3:
looptitle = 'Please select seeds.'
pre_roi_str = ' -seeds '
if vox_reg eq 0 then begin
    if atlasspace eq !SPACE_333 then scrap = '1' else scrap = '2'
    title = 'Smooth with a 3D Gaussian filter.' 
    label = 'Enter 0 for no smoothing.'
    scrap = get_str(1,'FWHM in voxels ',scrap,/GO_BACK,TITLE=title,LABEL=label,WIDTH=50)
    if scrap[0] eq 'GO_BACK' then goto,goback2
    fwhm = double(scrap[0])
    gauss_str = ' -gauss_smoth '+ strtrim(fwhm,2)
    mask_str = ' -mask ' + get_mask(atlasspace,fi)
endif else begin 
    looptitle = [looptitle,'Please select correlates.']
    pre_roi_str = [pre_roi_str,' -correg ']
endelse
goback3a:
rtn = analysis_type1(fi,dsp,wd,help,atlasspace,0,looptitle,pre_roi_str,/NOWTS)
if rtn.goback eq -1 then $
    return $
else if rtn.goback eq 1 then $ 
    if bold_size ne 1 then goto,goback3 else goto,goback2
nreg = rtn.nreg
region_names = rtn.region_names
region_str = rtn.region_str
roi_str = rtn.roi_str

if cor_part eq 1 then begin
    goback4:
    get_dialog_pickfile,'*img',directory,'Please select covariate files.',covfiles,ncovfiles,rtn_path,/MULTIPLE_FILES
    if ncovfiles eq 0 then goto,goback3a 
    print,'covfiles=',covfiles
endif

goback5:
names = get_str(1,'script','fidl_pc.csh',WIDTH=50,TITLE='Please enter desired filenames.',/GO_BACK)
if names[0] eq 'GO_BACK' then begin
    if cor_part eq 1 then goto,goback4 else goto,goback3a
endif 
csh = fix_script_name(names[0])

action = get_button(['execute','return','go back'],TITLE='Please select action.'+string(10B) $
    +string(10B)+'To run on linux, please select return. Login to a linux machine and enter the script on the command line.', $
    BASE_TITLE=csh)
if action eq 2 then goto,goback5

openw,lu_csh,csh,/GET_LUN
top_of_script,lu_csh
print_files_to_csh,lu_csh,ncovfiles,covfiles,'COV_FILES','cov_files'
for i=0,total_nsubjects-1 do begin
    printf,lu_csh,'nice +19 $BIN/fidl_pc -tc_files '+concselect[i]+' $COV_FILES'+exclude_frames_str+mask_str+region_str+roi_str $
        +gauss_str+' -clean_up'+mailstr
    printf,lu_csh,''
endfor
close,lu_csh
free_lun,lu_csh
spawn,'chmod +x '+csh

end
