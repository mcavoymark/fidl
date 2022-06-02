;Copyright 9/12/02 Washington University.  All Rights Reserved.  
;compute_tc_reg_stats.pro  $Revision: 1.18 $
pro compute_tc_reg_stats,fi,wd,dsp,help,stc,pref
t4_str = ''
formstr = ''
directory=getenv('PWD')
goback1:
widget_control,/HOURGLASS
scrap=get_glms(pref,/GET_CONC)
if scrap.msg eq 'GO_BACK' or scrap.msg eq 'EXIT' then return
load = scrap.load
nlists = scrap.nlists
total_nsubjects = scrap.total_nsubjects
t4select = scrap.t4select
concselect = scrap.concselect
t4_str=make_array(total_nsubjects,/STRING,VALUE='')
out_str=make_array(total_nsubjects,/STRING,VALUE='')
space = intarr(total_nsubjects)
stc2 = replicate({Stitch},total_nsubjects)
ifh2 = replicate({InterFile_Header},total_nsubjects)
widget_control,/HOURGLASS
for m=0,total_nsubjects-1 do begin
    print,'Loading ',concselect[m]
    rlc=load_conc(fi,stc,dsp,help,wd,concselect[m],/DONT_PUT_IMAGE)
    if rlc.msg ne 'OK' then return
    stc2[m] = rlc.stc1 
    ifh2[m] = rlc.ifh
    space[m] = get_space(rlc.ifh.matrix_size_1,rlc.ifh.matrix_size_2,rlc.ifh.matrix_size_3)
endfor
index_lc_t4=where(space ne !SPACE_111 and space ne !SPACE_222 and space ne !SPACE_333,count_lc_t4)
if count_lc_t4 gt 0 and count_lc_t4 ne total_nsubjects then begin
    scrap=get_button(['yes','no','not sure','go back'],BASE_TITLE='Images are not all of the same dimension', $
        TITLE='Do you know what you are doing?')
    if scrap eq 1 then $
        return $
    else if scrap eq 2 then begin
        scrap=get_button(['ok','go back','exit'],BASE_TITLE='hehehe',TITLE='We will just have see what happens.')
        if scrap eq 1 then goto,goback1 else if scrap eq 2 then return
    endif else if scrap eq 3 then goto,goback1
endif

if count_lc_t4 gt 0 then begin
    goback4:
    analtype = get_button(['Atlas space analysis','Data space analysis','Go back'], $
        TITLE='Do you wish to do an atlas space or a data space analysis?')
    if analtype eq 2 then goto,goback1
    if analtype eq 0 then begin
        if t4select[0] eq '' then begin
            goback4a:

            ;t4files = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_711-2?_t4')
            ;if n_elements(t4files) eq 1 and total_nsubjects gt 1 then $
            ;    t4select = make_array(total_nsubjects,/STRING,VALUE=t4files[0]) $
            ;else $
            ;    t4select[index_lc_t4] = match_files(concselect[index_lc_t4],t4files,TITLE='Please select t4 file for ')
            ;START131015
            gf = get_files(TITLE='Please enter atlas filter.',FILTER='*anat_ave_to_711-2?_t4')
            if gf.msg eq 'GO_BACK' then goto,goback4 else if gf.msg eq 'EXIT' then return 
            t4files = gf.files
            if n_elements(t4files) eq 1 and total_nsubjects gt 1 then $
                t4select = make_array(total_nsubjects,/STRING,VALUE=t4files[0]) $
            else if n_elements(t4files) eq 1 and total_nsubjects eq 1 and gf.msg eq 'DONTCHECK' then $
                t4select = t4files $
            else $
                t4select[index_lc_t4] = match_files(concselect[index_lc_t4],t4files,TITLE='Please select t4 file for ')



        endif else $
            t4select = get_str(total_nsubjects,concselect,t4select,TITLE='Please check t4s.',/ONE_COLUMN,/BELOW)
        index=where(t4select ne '',count)
        if count ne 0 then t4_str[index] = ' -xform_file ' + t4select[index]
        rtn=select_space()
        space[index_lc_t4]=rtn.space
    endif
endif

output=1
if total(space - space[0]) ne 0 then begin
    stat=dialog_message('Not all images are in the same space. Abort!',/ERROR)
    return
endif

rtn = get_regions(fi,wd,dsp,help)

if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then goto,goback4
if rtn.msg eq 'EXIT' then $
    return $
else if rtn.msg eq 'GO_BACK' then begin
    if count_lc_t4 gt 0 then goto,goback4 else goto,goback1
endif

region_names = rtn.region_names
region_space = rtn.space
region_files = rtn.region_file
nregion_files = rtn.nregfiles

goback98:
rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.')
roi_str = ' -regions_of_interest ' + strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)

if space[0] ne region_space then begin
    stat=dialog_message('Space incompatibility! Abort! Abort! Abort!',/ERROR)
    return
endif



;scrap = get_button(['standard','mean only, runs in columns','ugly-lots of decimal places','GO BACK'],TITLE='What type of output?')
;if scrap eq 3 then goto,goback98 
;if scrap eq 1 then $
;    formstr = ' -mean_only_runs_in_columns' $
;else if scrap eq 2 then $
;    formstr = ' -ugly'

scrap = get_button(['standard','mean only, runs in columns','ugly-lots of decimal places','mean only, each regional tc a column', $
    'GO BACK'],TITLE='What type of output?')
if scrap eq 4 then goto,goback98
if scrap eq 1 then $
    formstr = ' -mean_only_runs_in_columns' $
else if scrap eq 2 then $
    formstr = ' -ugly' $
else if scrap eq 3 then $
    formstr = ' -mean_only_each_regtc_a_col'



goback5:
scrap = 'script'
scraplabels = 'compute_tc_reg_stats.csh'
if strmid(concselect[0],strlen(concselect[0])-3) eq 'img' then lc_concs = 0 else lc_concs = 1
if lc_concs eq 1 then ext='.' else ext='.4dfp.img'
rtn_gr = get_root(concselect,ext)
junk = 'output ' + strtrim(indgen(total_nsubjects)+1,2)
scrap = [scrap,junk]
junk = rtn_gr.file + '_fidl_tc_ss.txt'
scraplabels = [scraplabels,junk]
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=50,TITLE='Please enter desired filenames.',/GO_BACK)
if names[0] eq 'GO_BACK' then goto,goback4
script = directory+'/'+fix_script_name(names[0])
out_str = ' -output "' + names[1:total_nsubjects] + '"'

action = get_button(['execute','return','GO BACK'],TITLE='Please select action.',BASE_TITLE=script)
if action eq 2 then goto,goback5

openw,lu,script,/GET_LUN
top_of_script,lu
region_str = ' $REGION_FILE'
print_files_to_csh,lu,nregion_files,region_files,'REGION_FILE','region_file'
for m=0,total_nsubjects-1 do begin
    print,'NEXTNEXTNEXTNEXTNEXT ',concselect[m],' NEXTNEXTNEXTNEXTNEXT'
    tdim_sum = *stc2[m].tdim_sum
    nrun = stc2[m].n
    filenames = *stc2[m].filnam
    print_files_to_csh,lu,nrun,filenames,'TC_FILES','tc_files',/NO_NEWLINE
    str = 'nice +19 $BIN/compute_tc_reg_stats $TC_FILES'+region_str+roi_str+t4_str[m]+out_str[m]+formstr+' -conc_file "' $
        +concselect[m]+'"'
    print,str
    printf,lu,str
    printf,lu,''
endfor
close,lu
free_lun,lu
spawn,'chmod +x '+script
widget_control,/HOURGLASS
if action eq 0 then begin
    scrap='>'+script+'.log'
    spawn,script+' &'
    stats = dialog_message(script+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif

print,'DONE'
end
