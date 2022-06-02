;Copyright 3/10/05 Washington University.  All Rights Reserved.
;fidl_flipnew.pro  $Revision: 1.12 $
pro fidl_flipnew,fi,wd,dsp,help,pr,stc,pref
mailstr = ''
delstr = ''
region_str = ''
roi_str = ''
region_file = ''
lcglm = 0
useglm = intarr(2)
script = 'temp_fidl_flipnew.csh'
popglm_list = ''
region_space = -1 
indglm_list = ''
gauss_str = ''
mask_str = ''
atlas_str = ''
goback0:

;if dialog_message('This program takes a population mean and standard deviation image/GLMs along with an individual image/GLMs.' $
;    +string(10B)+'The z score is computed for the flipped and unflipped individual image.'+string(10B)+'The z score for a ' $
;    +'voxel on the left of the individual image is reported on the left in both the flipped and unflipped case.'+string(10B) $
;    +'This convention can be changed if you are uncomfortable with it.',/INFORMATION,/CANCEL) eq 'Cancel' then return
;START160614
if dialog_message('z-score flipper'+string(10B)+'Images are converted to Z-scores.'+string(10B) $
    +'This program takes a population mean and standard deviation image/GLMs along with an individual image/GLMs.' $
    +string(10B)+'The z score is computed for the flipped and unflipped individual image.'+string(10B)+'The z score for a ' $
    +'voxel on the left of the individual image is reported on the left in both the flipped and unflipped case.'+string(10B) $
    +'For unflipped, the individual image is not flipped, neither are the mean and sd.'+string(10B) $
    +'For flipped, the individual image is not flipped. The mean and sd are flipped.',/INFORMATION,/CANCEL) eq 'Cancel' then return

goback1:
analysis_type = get_button(['voxel by voxel','specfic regions','GO BACK'],TITLE='Please select analysis type.')
asksmooth = 1 - analysis_type
if analysis_type eq 2 then goto,goback0
if analysis_type eq 1 then begin
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
    ;    region_file = rtn.region_file
    ;endelse

    if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then goto,goback11b
    region_names = rtn.region_names
    region_str = rtn.region_str
    region_space = rtn.space
    region_file = rtn.region_file



    goback16b:
    rtn = select_files(region_names,TITLE='Please select regions you wish to analyze.',/GO_BACK,MIN_NUM_TO_SELECT=1)
    if rtn.files[0] eq 'GO_BACK' then goto,goback16a
    roi_str = ' -regions_of_interest ' + strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)
endif

;scrap=get_glms(popload,popfiles,popnlists,poptotal_nsubjects,popt4select,popglm_list,popconcselect,popevselect, $
;    BASE_TITLE='Please select population sample mean.',GET_THIS='GLMs/4dfps',MAGICWORD='population')
;if scrap eq 'GO_BACK' then goto,goback1 else if scrap eq 'EXIT' then return
;START70
scrap=get_glms(BASE_TITLE='Please select population sample mean.',GET_THIS='GLMs/4dfps',MAGICWORD='population')
if scrap.msg eq 'GO_BACK' then goto,goback1 else if scrap.msg eq 'EXIT' then return
popload = scrap.load
popfiles = scrap.glmfiles
popnlists = scrap.nlists
poptotal_nsubjects = scrap.total_nsubjects
popt4select = scrap.t4select
popglm_list = scrap.glm_list
popconcselect = scrap.concselect
popevselect = scrap.evselect


goback2:
if strmid(popfiles[0],strlen(popfiles[0])-4) ne '.glm' then begin
    if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr_sd,idx_sd,'sample standard deviation', $
        TITLE='Please select population sample standard deviation.') eq !ERROR then return
endif else begin
    ls={load:popload,glmfiles:popfiles,nlists:popnlists,total_nsubjects:poptotal_nsubjects,t4select:popt4select, $
        glm_list_str:popglm_list,basetitle:'Population',lczstat:0,analysis_type:analysis_type,tc_type:'',pop_ind:0, $
        script:script,mean:'',sd:'',atlasspace:region_space,region_file:region_file,roi_str:roi_str,asksmooth:asksmooth, $
        gauss_str:gauss_str,mask_str:mask_str,atlas_str:atlas_str}

    ;compute_avg_zstat,fi,dsp,wd,glm,help,stc,pref,ls
    ;START130118
    lizard=compute_avg_zstat(fi,dsp,wd,glm,help,stc,pref,ls)

    popmean = ls.mean
    popsd = ls.sd
    asksmooth = 0
    gauss_str = ls.gauss_str
    mask_str = ls.mask_str
    atlas_str = ls.atlas_str
    print,'popmean=',popmean
    print,'popsd=',popsd
    useglm[0] = 1
endelse

goback11b:

;scrap=get_glms(indload,indfiles,indnlists,indtotal_nsubjects,indt4select,indglm_list,indconcselect,indevselect, $
;    BASE_TITLE='Individual',GET_THIS='GLMs/4dfps',MAGICWORD='individual')
;if scrap eq 'GO_BACK' then goto,goback2 else if scrap eq 'EXIT' then return
;START70
scrap=get_glms(BASE_TITLE='Individual',GET_THIS='GLMs/4dfps',MAGICWORD='individual')
if scrap.msg eq 'GO_BACK' then goto,goback2 else if scrap.msg eq 'EXIT' then return
indload = scrap.load
indfiles = scrap.glmfiles
indnlists = scrap.nlists
indtotal_nsubjects = scrap.total_nsubjects
indt4select = scrap.t4select
indglm_list = scrap.glm_list
indconcselect = scrap.concselect
indevselect = scrap.evselect



if strmid(indfiles[0],strlen(indfiles[0])-4) ne '.glm' then begin
    ;do nothing
endif else begin
    ls={load:indload,glmfiles:indfiles,nlists:indnlists,total_nsubjects:indtotal_nsubjects,t4select:indt4select, $
        glm_list_str:indglm_list,basetitle:'Individual',lczstat:0,analysis_type:0,tc_type:ls.tc_type,pop_ind:1, $
        script:ls.script,mean:'',sd:'',atlasspace:ls.atlasspace,region_file:'',roi_str:'',asksmooth:asksmooth,gauss_str:gauss_str, $
        mask_str:mask_str,atlas_str:atlas_str}

    ;compute_avg_zstat,fi,dsp,wd,glm,help,stc,pref,ls,/APPEND
    ;START130128
    lizard=compute_avg_zstat(fi,dsp,wd,glm,help,stc,pref,ls,/APPEND)

    indmean = ls.mean
    print,'indmean=',indmean
    useglm[1] = 1
endelse
lcglm = total(useglm)

filestr = ' -file ' + indmean 
if analysis_type eq 0 then begin
    meanstr = ' -mean ' + popmean 
    sdstr = ' -sd ' + popsd 
    file = indmean
    scrap = strpos(file,'/',/REVERSE_SEARCH)
    scrap = strmid(file,scrap+1,strpos(file,'.4dfp.img')-scrap-1)
    unflipstr = scrap + '_zscore.4dfp.img'
    flipstr = scrap + '_zscoreflip.4dfp.img'

    goback12:
    if lcglm lt 2 then begin
        scrap = get_mask(ls.atlasspace,fi)
        if scrap eq 'GO_BACK' then goto,goback11b
        if scrap ne '' then mask_str = ' -mask ' + scrap
    endif

    goback13a:
    scrap = get_str(3,['script','z score unflipped','z score flipped'],['fidl_flip.csh',unflipstr,flipstr],WIDTH=100, $
        TITLE='Please enter names. Leave blank any files you do not want.',/GO_BACK)
    if scrap[0] eq 'GO_BACK' then goto,goback12
    csh = fix_script_name(scrap[0])
    spawn,'mv -f '+script+' '+csh
    outstr = ''
    if scrap[1] ne '' then outstr = outstr + ' -unflip "'+scrap[1]+'"'
    if scrap[2] ne '' then outstr = outstr + ' -flip "'+scrap[2]+'"'

    if lcglm gt 0 then begin
        scrap = ''
        if useglm[0] eq 1 then scrap = [scrap,popmean,popsd]
        if useglm[1] eq 1 then scrap = [scrap,indmean]
        scrap = scrap[1:*]
        rtn = get_bool_list(scrap,TITLE='Please select files to delete.',/GO_BACK,BASE_TITLE='Intermediary files.')
        list = rtn.list
        index = where(list,count)
        if count ne 0 then begin
            scrap = scrap[index]
            pos = strpos(scrap,'.img')
            len = strlen(scrap)
            for i=0,count-1 do scrap = [scrap,strmid(scrap[i],0,len[i]-4)+'.ifh']
            delstr = 'rm -f '+strjoin(scrap,' ',/SINGLE)
        endif
    endif
endif else begin
    meanstr = ' -mean_and_sd ' + popmean 
    sdstr = ''
    goback13b:
    scrap = get_str(2,['script','output'],['fidl_flip.csh','fidl_flip.txt'],WIDTH=100,TITLE='Please enter names.',/GO_BACK)
    if scrap[0] eq 'GO_BACK' then goto,goback11b
    csh = fix_script_name(scrap[0])
    spawn,'mv -f '+script+' '+csh
    outstr = ' -out "'+scrap[1]+'"'
endelse

action = get_button(['execute','return','GO BACK'],TITLE='Please select action.'+string(10B)+string(10B) $
    +'To run on linux, please select return. Login to a linux machine and enter the script on the command line.',BASE_TITLE=csh)
if action eq 2 then begin
    if analysis_type eq 0 then goto,goback13a ;else goto,goback13b
endif

openw,lu,csh,/GET_LUN,/APPEND
printf,lu,''
printf,lu,'nice +19 $BIN'+'/fidl_flip'+meanstr+sdstr+filestr+mask_str+region_str+roi_str+outstr+mailstr
printf,lu,''
printf,lu,delstr
close,lu
free_lun,lu
spawn,'chmod +x '+csh

widget_control,/HOURGLASS
if action eq 0 then begin
    if lcglm eq 0 then begin
        if analysis_type eq 0 and lcglm eq 0 then $
            spawn_cover,csh,fi,wd,dsp,help,stc $
        else begin
            spawn,csh,result
            stat=dialog_message(result,/INFORMATION)
        endelse
    endif else $
        spawn,csh+'>'+csh+'.log &'
endif
print,'DONE'
end
