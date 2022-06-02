;Copyright 7/24/01 Washington University.  All Rights Reserved.
;fidl_avg.pro  $Revision: 1.9 $
pro fidl_avg,fi,wd,dsp,help,stc,pref
goback1:
gg=get_glms(pref,fi,help,GET_THIS='imgs')
if gg.msg ne 'OK' then return 
files = gg.imgselect
total_nsubjects = gg.total_nsubjects
gr=get_root(files[0],'.4dfp.img')
root = gr.file
goback3:
rtn = select_files(['Mean','SEM','Sum','Percentage of voxels in maximum overlap','Anatomy average'],TITLE='Please select.', $
    MIN_NUM_TO_SELECT=1,/GO_BACK)
if rtn.files[0] eq 'GO_BACK' then goto,goback1
spider1=['Mean','SEM','Sum','Maximum overlap','Anat ave']
spider2=root[0]+['_mean.4dfp.img','_sem.4dfp.img','_sum.4dfp.img','_maxoverlap.4dfp.img','_anatave.4dfp.img']
spider3=['-avg "','-sem "','-sum "','-maxoverlap "','-anatave "']
spider1 = spider1[rtn.index]
spider2 = spider2[rtn.index]
spider3 = spider3[rtn.index]
dummy = 1+rtn.count
goback4:
scrap = ['Script',spider1] 

;scraplabels = ['fidl_avg.csh',spider2] 
;START170127
spawn,!BINEXECUTE+'/fidl_timestr2',timestr
scraplabels = ['fidl_avg_'+timestr[0]+'.csh',spider2] 

names = get_str(dummy,scrap,scraplabels,WIDTH=100,TITLE='Please enter desired filenames.',/GO_BACK)
if names[0] eq 'GO_BACK' then goto,goback3
avg_4dfp_csh = fix_script_name(names[0])
goose = strjoin(spider3+strtrim(names[1:*],2),'" ',/SINGLE)+'"'
action = get_button(['execute','return','GO BACK'],TITLE='Please select action.',BASE_TITLE=avg_4dfp_csh)
if action eq 2 then goto,goback4
openw,lu_csh,avg_4dfp_csh,/GET_LUN
top_of_script,lu_csh
print_files_to_csh,lu_csh,total_nsubjects,files,'FILES','files'
printf,lu_csh,'nice +19 $BIN/fidl_avg $FILES '+goose
close,lu_csh
free_lun,lu_csh
spawn,'chmod +x '+avg_4dfp_csh
if action eq 0 then spawn_cover,avg_4dfp_csh,fi,wd,dsp,help,stc
print,'DONE'
end
