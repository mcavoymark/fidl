;Copyright 10/31/01 Washington University.  All Rights Reserved.
;compute_pearson.pro  $Revision: 1.19 $
pro compute_pearson,fi,wd,dsp,help,pr,stc,pref
threshold_extent_str = ''
skip = ''
output_str = ''
region_str = ''
roi_str = ''
directory=getenv('PWD')
ibold = get_bolds(fi,dsp,wd,glm,help,stc)
if ibold lt 0 then return
nbold = stc[ibold].n
fbold = *stc[ibold].filnam
goback3:
text_or_4dfp = get_button(['Text file.','4dfp stack.'],TITLE='Please select target.')
if text_or_4dfp eq 0 then begin
    get_dialog_pickfile,'*.txt',directory,'Please select target.',target_filename,rtn_nfiles,rtn_path
    if target_filename eq 'GOBACK' or target_filename eq 'EXIT' then return
    ntarget = 1
    ftarget = target_filename
    target_filter = '.'
endif else begin
    itarget = get_bolds(fi,dsp,wd,glm,help,stc)
    if itarget lt 0 then return
    ntarget = stc[itarget].n
    ftarget = *stc[itarget].filnam
    target_filter = '.4dfp.img'
endelse

scrap = nbold 
if text_or_4dfp eq 1 then scrap = scrap + ntarget
atlas = intarr(scrap)
tdim = intarr(scrap)

hdr_ptr = *stc[0].hdr_ptr
for i=0,nbold-1 do begin
    hdr = *hdr_ptr[i]
    atlas[i] = get_space(hdr.xdim,hdr.ydim,hdr.zdim)
    tdim[i] = hdr.tdim
endfor
if text_or_4dfp eq 1 then begin
    hdr_ptr = *stc[1].hdr_ptr
    for j=0,ntarget-1 do begin
        hdr = *hdr_ptr[j]
        atlas[i] = get_space(hdr.xdim,hdr.ydim,hdr.zdim)
        tdim[i] = hdr.tdim
        i=i+1
    endfor
endif
if total(abs(atlas - atlas[0])) ne 0 then begin
    stat = dialog_message('Images are not all in the same space. Abort!',/ERROR)
    return
endif
if total(abs(tdim - tdim[0])) ne 0 then begin
    stat = dialog_message('Time dimension not equal across images.'+string(10B)+'Do you know what you are doing?',/QUESTION)
    if stat eq 'No' then return
endif

min_tdim = min(tdim)
if min_tdim gt 1 then begin
    goback4:
    skip = get_str(1,'Enter zero to skip none.',string(min_tdim-1<pref.paraskip),WIDTH=50, $
        TITLE='Please enter the number of initial frames to skip.')
    if fix(skip[0]) gt 0 then skip = ' -skip ' + strtrim(skip[0],2)
endif

if min_tdim eq 1 then begin
    goback16a:
    rtn = get_regions(fi,wd,dsp,help)
    if rtn.msg eq 'EXIT' then $
        return $
    else if rtn.msg eq 'GO_BACK' then begin
        if min_tdim gt 1 then goto,goback4 else goto,goback3
    endif
    region_names = rtn.region_names
    region_str = rtn.region_str
    ;region_file = rtn.region_file
    region_space = rtn.space
    if atlas[0] ne region_space then begin
        stat = dialog_message('4dfps and region file are not in the same space. Abort!',/ERROR)
        return
    endif
    goback16b:
    rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.',/GO_BACK,MIN_NUM_TO_SELECT=1)
    if rtn.files[0] eq 'GO_BACK' then goto,goback16a
    roi_str = ' -regions_of_interest ' + strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)
endif else begin
    goback5:
    option_str = ['correlation coefficient','Uncorrected T-map','Uncorrected Z-map','Monte Carlo Corrected T-map', $
        'Monte Carlo Corrected Z-map','Fisher z']

    ;option_list = get_bool_list(option_str,TITLE='Please select output options.')
    ;START1
    rtn = get_bool_list(option_str,TITLE='Please select output options.')
    option_list = rtn.list

    total_option_list = total(option_list)
    if total_option_list eq 0 then option_list[0] = 1
    output_str = ' -output'
    if option_list[0] eq 1 then output_str = output_str + ' corrcoeff'
    if option_list[1] eq 1 then output_str = output_str + ' T_uncorrected'
    if option_list[2] eq 1 then output_str = output_str + ' Z_uncorrected'
    if option_list[3] eq 1 then output_str = output_str + ' T_monte_carlo' 
    if option_list[4] eq 1 then output_str = output_str + ' Z_monte_carlo' 
    if option_list[5] eq 1 then output_str = output_str + ' fisherz' 
    if option_list[3]+option_list[4] gt 0 then begin
        scrap = get_str(1,'FWHM in voxels ','2',WIDTH='30',TITLE='How much have your images been smoothed?',/GO_BACK)
        if scrap[0] eq 'GO_BACK' then goto,goback5
        rtn = threshold_extent(!T_STAT,scrap[0],atlas[0])
    endif
endelse







goose = nbold 
if nbold gt 1 and ntarget eq 1 and text_or_4dfp eq 0 then begin
    spawn,'wc -l '+ftarget[0],nlines
    if nlines[0] eq stc[0].tdim_all then goose = 1
endif

dummy = 2 + goose 
labels = strarr(dummy)
names = strarr(dummy)
labels[0] = 'script'
names[0] = directory+'/compute_pearson.csh'
labels[1] = 'output storage path'
names[1] = directory
i = 2



if min_tdim eq 1 then begin
    labels[i] = 'output'
    names[i] = 'compute_pearson.txt'
endif else begin
    target_names = get_root(ftarget,target_filter)
    target_names = target_names.file
    bold_filter = '.4dfp.img'
    if goose eq 1 then begin
        if strmid(stc[0].name,strlen(stc[0].name)-3) ne 'img' then bold_filter = '.' 
        scrap = stc[0].name
    endif else begin 
        scrap = fbold
        if ntarget eq 1 then target_names = make_array(nbold,/STRING,VALUE=target_names)
    endelse
    bold_names = get_root(scrap,bold_filter)
    bold_names = bold_names.file
    combined_names = bold_names + '_and_' + target_names
    labels[i:i+goose-1] = 'output root'
    names[i:i+goose-1] = combined_names
endelse


goback19:
names = get_str(dummy,labels,names,WIDTH=100,TITLE='Please enter desired names.')


pearson_csh = fix_script_name(names[0])
directory = names[1]
if directory ne '' then begin
    if strmid(directory,0,1) ne '/' then directory = '/' + directory
    if strmid(directory,strlen(directory)-1,1) ne '/' then directory = directory + '/'
    directory_str = ' -directory ' + directory
endif

second = strpos(names[2:dummy-1],'.4dfp.img')
index = where(second ne -1,count)
if count ne 0 then names[index+2] = strmid(names[index+2],0,second)


action = get_button(['execute with log','execute with email','return','GO BACK'],TITLE='Please select action.'+string(10B) $
    +string(10B)+'To run on linux, please select return. Login to a linux machine and enter the script on the command line.', $
    BASE_TITLE=pearson_csh)
if action eq 3 then $
    goto,goback19 $
else if action eq 1 then $
    mailstr = ' |& mail `whoami`' $
else $
    mailstr = ''






openw,lu,pearson_csh,/GET_LUN
top_of_script,lu


print_files_to_csh,lu,nbold,fbold,'BOLD_FILES','bold_files'
print_files_to_csh,lu,ntarget,ftarget,'TARGET_FILES','target_files'






if min_tdim eq 1 then begin
    printf,lu,'nice +19 $BIN/fidl_corrvox $BOLD_FILES $TARGET_FILES '+skip+directory_str+output_str+threshold_extent_str $
        +mailstr+region_str+roi_str+' -output "'+names[i]+'"'
endif else begin
    print_files_to_csh,lu,goose,names[i:i+goose-1],'ROOT','root'
    printf,lu,'nice +19 $BIN/compute_pearson $BOLD_FILES $TARGET_FILES $ROOT'+skip+directory_str+output_str+threshold_extent_str $
        +mailstr+region_str+roi_str
endelse



close,lu
free_lun,lu
spawn,'chmod +x '+pearson_csh



;action = get_button(['execute','return'],TITLE='Please select action.',BASE_TITLE=pearson_csh)
;if action eq 0 then begin
;    spawn,pearson_csh+' &'
;    stats = dialog_message(pearson_csh+' submitted as batch job.  The log file(s) will be e-mailed to ' $
;        +'you upon completion.',/INFORMATION)
;endif

if action lt 2 then begin
    if action eq 0 then scrap='>'+pearson_csh+'.log' else scrap=''
    spawn,pearson_csh+scrap+' &'
    stats = dialog_message(pearson_csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif


print,'DONE'
end
