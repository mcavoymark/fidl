;Copyright 3/10/05 Washington University.  All Rights Reserved.
;fidl_flip.pro  $Revision: 1.11 $
pro fidl_flip,fi,wd,dsp,help,pr,stc,pref
maskstr = ''
region_str = ''
roi_str = ''

;if dialog_message('This program takes a population mean and standard deviation image along with an individual image.'+string(10B) $
;    +'The z score is computed for the flipped and unflipped individual image.'+string(10B)+'The z score for a voxel on the left ' $
;    +'of the individual image is reported on the left in both the flipped and unflipped case.'+string(10B) $
;    +'This convention can be changed if you are uncomfortable with it.',/INFORMATION,/CANCEL) eq 'Cancel' then return
;START160614
if dialog_message('z-score flipper'+string(10B)+'Images are converted to Z-scores.'+string(10B) $
    +'This program takes a population mean and standard deviation image along with an individual image.'+string(10B) $
    +'The z score is computed for the flipped and unflipped individual image.'+string(10B)+'The z score for a voxel on the left ' $
    +'of the individual image is reported on the left in both the flipped and unflipped case.'+string(10B) $
    +'For unflipped, the individual image is not flipped, neither are the mean and sd.'+string(10B) $
    +'For flipped, the individual image is not flipped. The mean and sd are flipped.',/INFORMATION,/CANCEL) eq 'Cancel' then return

goback11:
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr_mean,idx_mean,'sample mean', $
    TITLE='Please select sample mean.') eq !ERROR then return
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr_sd,idx_sd,'sample standard deviation', $
    TITLE='Please select sample standard deviationl.') eq !ERROR then return
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr_flip,idx_flip,'individual image', $
    TITLE='Please select individual image.') eq !ERROR then return

meanstr = ' -mean ' + fi.names[idx_mean]
sdstr = ' -sd ' + fi.names[idx_sd]
filestr = ' -file ' + fi.names[idx_flip]
goback11b:
analysis_type = get_button(['voxel by voxel','specfic regions','GO BACK'],TITLE='Please select analysis type.')
if analysis_type eq 2 then goto,goback11
if analysis_type eq 0 then begin
    file = fi.names[idx_flip]
    scrap = strpos(file,'/',/REVERSE_SEARCH)
    scrap = strmid(file,scrap+1,strpos(file,'.4dfp.img')-scrap-1)
    unflipstr = scrap + '_zscore.4dfp.img'
    flipstr = scrap + '_zscoreflip.4dfp.img'
    goback12:
    scrap = get_mask(fi.space[idx_flip],fi)
    if scrap eq 'GO_BACK' then $
        goto,goback11b $
    else if scrap eq '' then begin
        ;do nothing
    endif else $
        maskstr = ' -mask ' + scrap

    goback13a:
    scrap = get_str(3,['script','z score unflipped','z score flipped'],['fidl_flip.csh',unflipstr,flipstr],WIDTH=100, $
        TITLE='Please enter names. Leave blank any files you do not want.',/GO_BACK)
    if scrap[0] eq 'GO_BACK' then goto,goback12
    csh = fix_script_name(scrap[0])
    outstr = ''
    if scrap[1] ne '' then outstr = outstr + ' -unflip "'+scrap[1]+'"'
    if scrap[2] ne '' then outstr = outstr + ' -flip "'+scrap[2]+'"'
    mailstr = ''
endif else begin
    goback16a:
    rtn = get_regions(fi,wd,dsp,help)



    ;if rtn.goback eq !TRUE then $
    ;    goto,goback11b $
    ;else if rtn.error_flag eq !TRUE then $
    ;    return $
    ;else begin
    ;    region_names = rtn.region_names
    ;    region_str = rtn.region_str
    ;    region_space = rtn.space
    ;endelse

    if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then goto,goback11b
    region_names = rtn.region_names
    region_str = rtn.region_str
    region_space = rtn.space



    if fi.space[idx_mean] ne region_space or fi.space[idx_sd] ne region_space or fi.space[idx_flip] ne region_space then begin
        stat = dialog_message("All files must be in the same space. Abort!",/ERROR)
        return
    endif
    goback16b:
    rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.',/GO_BACK,MIN_NUM_TO_SELECT=1)
    if rtn.files[0] eq 'GO_BACK' then goto,goback16a
    roi_str = ' -regions_of_interest ' + strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)

    goback13b:
    scrap = get_str(2,['script','output'],['fidl_flip.csh','fidl_flip.txt'],WIDTH=100,TITLE='Please enter names.',/GO_BACK)
    if scrap[0] eq 'GO_BACK' then goto,goback16b
    csh = fix_script_name(scrap[0])
    outstr = ' -out "'+scrap[1]+'"'
endelse

action = get_button(['execute','return','GO BACK'],TITLE='Please select action.'+string(10B)+string(10B) $
    +'To run on linux, please select return. Login to a linux machine and enter the script on the command line.',BASE_TITLE=csh)
if action eq 2 then begin
    if analysis_type eq 0 then goto,goback13a else goto,goback13b
endif
if action eq 0 then mailstr = '' else mailstr = ' |& mail `whoami`'

openw,lu,csh,/GET_LUN
top_of_script,lu
printf,lu,'nice +19 $BIN'+'/fidl_flip'+meanstr+sdstr+filestr+maskstr+region_str+roi_str+outstr+mailstr
close,lu
free_lun,lu
spawn,'chmod +x '+csh

;if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc
widget_control,/HOURGLASS
if action eq 0 then begin
    if analysis_type eq 0 then $
        spawn_cover,csh,fi,wd,dsp,help,stc $
    else begin
        spawn,csh,result
        stat=dialog_message(result,/INFORMATION)
    endelse
endif

print,'DONE'
print,''
end
