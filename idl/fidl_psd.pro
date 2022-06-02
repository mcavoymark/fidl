;Copyright 5/27/05 Washington University.  All Rights Reserved.
;fidl_psd.pro  $Revision: 1.3 $

;***********************************
pro fidl_psd,fi,wd,dsp,help,stc,pref
;***********************************
outstr=''
fstr=''
psdstr=''

goback0a:
;if dialog_message('Output is mean squared power of the one sided power spectral density in the specified frequency band in ' $
;    +'decibels.'+string(10B)+'Will also output the entire power spectrum.',/INFORMATION,/CANCEL) eq 'Cancel' then return
if dialog_message('Output is mean squared power of the one sided power spectral density in the specified frequency band.' $
    +string(10B)+'Will also output the entire power spectrum.',/INFORMATION,/CANCEL) eq 'Cancel' then return

goback0:
;if rtn.num_files eq 0 then return
;if get_bolds(fi,dsp,wd,glm,help,stc,idx) eq 0 then return
idx = get_bolds(fi,dsp,wd,glm,help,stc)
if idx lt 0 then return

goback1:
vals = get_str(1,'TR in sec',strtrim(pref.TR,2),/GO_BACK)
if vals[0] eq 'GO_BACK' then goto,goback0
TR = float(vals[0])

nrun = stc[idx].n
tdim_file = *stc[idx].tdim_file
filenames = *stc[idx].filnam
root = fi.list[idx]
scrap = strpos(root,'.conc')
if scrap ne -1 then root = strmid(root,0,scrap)
space = fi.space[idx]

goback2:
vals = get_str(nrun,'run'+strcompress(indgen(nrun)+1),make_array(nrun,/STRING,VALUE=strtrim(pref.paraskip,2)), $
    TITLE='Number of frames to skip.',WIDTH=25,/GO_BACK)
if vals[0] eq 'GO_BACK' then goto,goback1
init_skip = fix(vals)

N = max(tdim_file-init_skip)
f = float(indgen(N/2+1))/(float(N)*TR)
print,'f=',f

goback3:
nband = 10
vals = get_str(nband,'band'+strcompress(indgen(nband)+1),make_array(nband,/STRING,VALUE=''),TITLE='Please specify bands.', $
    WIDTH=25,/GO_BACK,LABEL='Commas and dashes accepted.'+string(10B)+'Ex1.  0.005-0.2    Total power from 0.005 to 0.2 Hz.'+ $
    string(10B)+'Ex2.  PSD    Entire spectrum.',LONGTEXT=strtrim(f,2),LONGLABEL='Frequencies (Hz)')
if vals[0] eq 'GO_BACK' then goto,goback2
index = where(vals ne '',nband)
if nband eq 0 then begin
    if dialog_message('You failed to specify any bands. Please try again.'+string(10B)+'Cancel to exit.',/ERROR,/CANCEL) $
        eq 'Cancel' then return else goto,goback3
endif

vals = vals[index]
goback3b:
scrap = get_mask(space,fi)
if scrap eq 'GO_BACK' then $
    goto,goback3 $
else if scrap eq '' then $ 
    maskstr = '' $
else $ 
    maskstr = ' -mask ' + scrap

goback4:
dummy = 1 + nband
scraplabels = strarr(dummy)
scrap = strarr(dummy)
scraplabels[0] = 'script'
scrap[0] = 'fidl_psd.csh'
psd = strcmp(strtrim(vals,2),'PSD',/FOLD_CASE)
index0 = where(psd eq 0,count0)
index1 = where(psd eq 1,count1)
if count0 ne 0 then begin
    ;scraplabels[index0+1] = 'mean square amlitude in dB ' + vals[index0] + ' Hz'
    ;for i=0,count0-1 do scrap[index0[i]+1] = root+'_msqamp'+strjoin(strsplit(vals[index[i]],',',/EXTRACT),'_',/SINGLE) $
    ;    +'HzdB.4dfp.img'
    scraplabels[index0+1] = 'mean square amlitude in ' + vals[index0] + ' Hz'
    for i=0,count0-1 do scrap[index0[i]+1] = root+'_msqamp'+strjoin(strsplit(vals[index[i]],',',/EXTRACT),'_',/SINGLE) $
        +'Hz.4dfp.img'
endif
if count1 ne 0 then begin
    scraplabels[index1+1] = 'power spectral density'
    ;scrap[index1+1] = root+'_msqamp_psddB.4dfp.img'
    scrap[index1+1] = root+'_msqamp_psd.4dfp.img'
    vals[index1] = 'PSD'
endif

scrap = get_str(dummy,scraplabels,scrap,WIDTH=100,TITLE='Please enter names.',/GO_BACK)
if scrap[0] eq 'GO_BACK' then goto,goback3b

csh = fix_script_name(scrap[0])



;outstr = ' -output'
;for i=1,nband do outstr = outstr + ' "' + scrap[i] + '"'
;fstr = ' -freq ' + strjoin(strcompress(vals),' ',/SINGLE)

if count0 ne 0 then begin
    outstr = ' -output'
    for i=0,count0-1 do outstr = outstr + ' "' + scrap[index0[i]+1] + '"'
    fstr = ' -freq ' + strjoin(strcompress(vals[index0]),' ',/SINGLE)
endif
if count1 ne 0 then psdstr = ' -psd ' + ' "' + scrap[index1[0]+1] + '"' 







action = get_button(['execute','return','GO BACK'],TITLE='Please select action.'+string(10B)+string(10B) $
    +'To run on linux, please select return. Login to a linux machine and enter the script on the command line.',BASE_TITLE=csh)
if action eq 2 then goto,goback4





openw,lu,csh,/GET_LUN
top_of_script,lu
print_files_to_csh,lu,nrun,filenames,'FILES','files'
printf,lu,'nice +19 $BIN'+'/fidl_psd $FILES'+fstr+outstr+psdstr+maskstr
close,lu
free_lun,lu
spawn,'chmod +x '+csh
if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc
print,'DONE'
print,''


end
