;Copyright 7/3/01 Washington University.  All Rights Reserved.
;generate_conc.pro  $Revision: 1.9 $

;***********************************
pro generate_conc,fi,dsp,wd,help,stc
;***********************************
seed_str = ''
directory = getenv('PWD')



;scrap = get_str(5,['x dimension','y dimension','number of slices','number of frames','number of runs'], $
;    ['64','64','16','128','12'],WIDTH=20,TITLE='Please enter desired parameters.')
;xdim_str = strtrim(scrap[0],2)
;ydim_str = strtrim(scrap[1],2)
;zdim_str = strtrim(scrap[2],2)
;tdim_str = strtrim(scrap[3],2)

scrap = get_str(7,['x dimension','y dimension','number of slices','number of frames','number of runs','xy voxel size (mm)', $
    'z voxel size (mm)'],['64','64','16','128','12','3.75','8.00'],WIDTH=20,TITLE='Please enter desired parameters.')
scrap = strtrim(scrap,2)
xdim_str = scrap[0]
ydim_str = scrap[1]
zdim_str = scrap[2]
tdim_str = scrap[3]
nruns = fix(scrap[4])
nruns_str = scrap[4]
xyvoxsize_str = scrap[5]
zvoxsize_str = scrap[6]

lc_purpose = get_button(['I wish to test an event file.','I wish to do a white noise simulation.'], $
    TITLE='Do you wish to generate a conc set to test an event file or do a white noise simulation?')

dummy = 2 + lc_purpose
names = strarr(dummy)
labels = strarr(dummy)
names[0] = directory+'/white_noise.csh'
labels[0] = 'script
names[1] = directory + '/white_noise_' + tdim_str
labels[1] = 'output root'
if lc_purpose eq 0 then $
    title ='Please enter desired filenames.' $
else begin
    names[2] = '0 0 0' 
    labels[2] = 'Starting seed (3 numbers)'
    title ='Please enter desired filenames and starting seed.'
endelse
names = get_str(dummy,labels,names,WIDTH=100,TITLE=title)
white_noise_csh = fix_script_name(names[0])
root_str = ' -filename ' + names[1]
if lc_purpose eq 1 then begin
    seed_str = ' -seed ' + names[2]
endif
if lc_purpose eq 0 then nruns_generate = 1 else nruns_generate = nruns
runs_str = ' -runs ' + strtrim(nruns_generate,2)
runs_conc_str = ' -runs_conc ' + strtrim(nruns,2)


widget_control,/HOURGLASS
openw,lu_csh,white_noise_csh,/GET_LUN
top_of_script,lu_csh
printf,lu_csh,'nice +19 $BIN/white_noise -dimensions '+xdim_str+' '+ydim_str+' '+zdim_str+' '+tdim_str+' -xyvoxsize '+xyvoxsize_str  $
    +' -zvoxsize '+zvoxsize_str+root_str+seed_str+runs_str+runs_conc_str
close,lu_csh
free_lun,lu_csh
spawn,'chmod +x '+white_noise_csh
action = get_button(['execute','return'],BASE_TITLE='Please select.',TITLE=white_noise_csh)
if action eq 0 then begin

    ;spawn,white_noise_csh+' &'
    ;START110912
    spawn,'csh '+white_noise_csh+' &'

    stats=dialog_message(white_noise_csh+' submitted as batch job.  The log file(s) will be e-mailed to you upon completion.', $
        /INFORMATION)
endif
print,'DONE'
end
