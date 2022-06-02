;Copyright 10/1/14 Washington University.  All Rights Reserved.
;fidl_motionreg.pro  $Revision: 1.10 $
pro fidl_motionreg,fi,dsp,wd,glm,help,stc,pref
directory = getenv('PWD')
goback0:

;scrap = get_button(['Continue','Exit'],BASE_TITLE='Motion scrubbing',TITLE='This program takes *.dat files from the see_movement ' $
;    +'script or bold files.'+string(10B)+string(10B)+'*.dat files from the see_movement script'+string(10B) $
;    +'----------------------------------------'+string(10B)+'The easiest way to do a large number of subjects is to create a list ' $
;    +'file with each subject listed as *.conc.'+string(10B)+'Then each conc lists the *.dat files.'+string(10B)+string(10B) $
;    +'bold files'+string(10B)+'----------'+string(10B)+'The easiest way is to load the same list you wish for glm computation.' $
;    +string(10B)+'The list may include concs, event files and t4s.'+string(10B)+'The list is updated to include the *.ext motion ' $
;    +'regressor files.'+string(10B)+string(10B)+'The output is a text file for each subject.'+string(10B)+'Movement regressors ' $
;    +"may include [R R' R2 R'2] where R = [X Y X pitch yaw roll]"+string(10B)+'Columns are run separated and zero mean.')
;START211010
scrap = get_button(['Continue','Exit'],BASE_TITLE='Motion scrubbing',TITLE='This program takes *.dat files from the see_movement ' $
    +'script or bold files.'+string(10B)+string(10B)+'*.dat files from the see_movement script'+string(10B) $
    +'----------------------------------------'+string(10B)+'The easiest way to do a large number of subjects is to create a list ' $
    +'file with each subject listed as *.conc.'+string(10B)+'Then each conc lists the *.dat files.'+string(10B)+string(10B) $
    +'bold files'+string(10B)+'----------'+string(10B)+'The easiest way is to load the same list you wish for glm computation.' $
    +string(10B)+'The list may include concs, event files and t4s.'+string(10B)+'The list is updated to include the *.ext motion ' $
    +'regressor files.'+string(10B)+string(10B)+'The output is a text file for each subject.'+string(10B)+'Movement regressors ' $
    +"may include [R R' R2 R'2] where R = [X Y X pitch yaw roll]"+string(10B)+'Columns are run separated and zero mean.'+string(10B)+string(10B) $
    +'ext files'+string(10B)+'---------'+string(10B)+'Load or put in a list.')



if scrap eq 1 then return
goback1:
gg=get_glms(pref,FILTER='*.dat')

;if gg.msg eq 'GO_BACK' then goto,goback0
;START211009
if gg.msg eq 'GO_BACK' then goto,goback0 else if gg.msg eq 'ERROR' then begin
    print,gg.msg
    lizard=get_button('exit',TITLE=gg.msg+' in get_glms', BASE_TITLE='Big problem')
    return
endif

goback2:
fd=get_str(1,'FD','0.2',TITLE='Please specify threshold',LABEL='Frame displacements greater than this threshold are scrubbed', $
    /GO_BACK,/EXIT)
if fd[0] eq 'EXIT' then return else if fd[0] eq 'GO_BACK' then goto,goback1

;START150420
goback3:

;sf=select_files(['motion regressors [X Y Z pitch yaw roll]','derivatives','squares','squared derivatives'],BASE_TITLE='Please ' $
;    +'select motion regressors',MIN_NUM_TO_SELECT=1,/GO_BACK,/ONE_COLUMN,TITLE='Each selection yields six regressors',/EXIT,/NO_ALL)
;START211008
sf=select_files(['motion regressors [X Y Z pitch yaw roll]','derivatives','squares','squared derivatives'],BASE_TITLE='Please ' $
    +'select motion regressors',MIN_NUM_TO_SELECT=-1,/GO_BACK,/ONE_COLUMN,TITLE='Each selection yields six regressors.'+string(10B) $
    +"Don't select any to just make scrub files.",/EXIT,/NO_ALL)


if sf.files[0] eq 'EXIT' then return else if sf.files[0] eq 'GO_BACK' then goto,goback3
moreg = [' -R',' -Rd',' -R2',' -Rd2']

;moregstr=strjoin(moreg[sf.index],/SINGLE)
;START211008
moregstr=''
if sf.count ne 0 then begin
    moregstr=strjoin(moreg[sf.index],/SINGLE)
endif

goback4:
label='Regressor is zero mean only over nonskipped frames.'+string(10B)+'Skipped frames are set to zero.'
if sf.list[1]+sf.list[3] ne 0 then label=label+string(10B)+'First frame is always skipped for derivatives and their squares.'
skipstr=''
skip=get_str(1,'Initial frames to skip',pref.paraskip,TITLE='Number of frames to skip.',WIDTH=25,/GO_BACK,FRONT=scrap,/EXIT, $
    LABEL=label)
if skip[0] eq 'EXIT' then return else if skip[0] eq 'GO_BACK' then goto,goback2
if fix(skip) gt 0 then skipstr=' -skip '+trim(skip[0])

;spawn,!BINEXECUTE+'/fidl_timestr2',timestr
;START211009
spawn,!BINEXECUTE+'/fidl_timestr3',timestr

;csh = 'fidl_motionreg_'+timestr[0]+'.csh'
;START211010
csh = 'fidl_motionreg'+timestr[0]+'.sh'


;gr_conc = get_root(gg.concselect,'.conc')
;out = gr_conc.file + '_motion.ext'
;START211009
if gg.concselect ne '' then begin
    gr_conc = get_root(gg.concselect,'.conc')
    out = gr_conc.file + '_motion.ext'
endif


scrap0 = 'script'
scrap1 = csh

;print,'gg.nlists=',gg.nlists
;print,'n_elements(gg.nlists)=',n_elements(gg.nlists)
;print,'gg.glm_list=',gg.glm_list
;print,'n_elements(gg.glm_list)=',n_elements(gg.glm_list)

goback5:


;names = get_str(n_elements(scrap1)+gg.total_nsubjects,[scrap0,'subject '+trim(indgen(gg.total_nsubjects)+1)],[scrap1,out],WIDTH=100, $
;    TITLE='Please name output files. Keep the *.ext extension.',/ONE_COLUMN,/GO_BACK,FRONT=directory+'/',/REPLACE,/EXIT)
;START211010
if gg.total_nsubjects ne 0 then begin
    if gg.glm_list[0] ne '' then begin
        gr_list = get_root(gg.glm_list,'.')
        scrap0 = [scrap0,'list'] 
        scrap1 = [scrap1,gr_list.file[0]+'.list']
    endif
    names = get_str(n_elements(scrap1)+gg.total_nsubjects,[scrap0,'subject '+trim(indgen(gg.total_nsubjects)+1)],[scrap1,out],WIDTH=100, $
        TITLE='Please name output files. Keep the *.ext extension.',/ONE_COLUMN,/GO_BACK,FRONT=directory+'/',/REPLACE,/EXIT)
endif else begin
    names = get_str(n_elements(scrap1),scrap0,scrap1,WIDTH=100, $
        TITLE='Please name output files. Keep the *.ext extension.',/ONE_COLUMN,/GO_BACK,FRONT=directory+'/',/REPLACE,/EXIT)
endelse


if names[0] eq 'EXIT' then return else if names[0] eq 'GO_BACK' then goto,goback4
names = fix_script_name(names) 
script = names[0]

;if n_elements(gg.glm_list) eq 0 then $
;    out = names[1:*] $
;else begin 
;    out = names[2:*] 
;    dummy = names[2:*]
;    idx = where(strpos(dummy,'/') eq -1,cnt)
;    if cnt ne 0 then dummy[idx] = directory+'/'+dummy[idx]
;    openw,lu,names[1],/GET_LUN
;    for i=0,n_elements(out)-1 do begin
;        printf,lu,gg.concselect[i]
;        if gg.t4select[0] ne '' then printf,lu,gg.t4select[i]
;        if gg.evselect[0] ne '' then printf,lu,gg.evselect[i]
;        printf,lu,dummy[i]
;    endfor
;    close,lu
;    free_lun,lu
;endelse
;START211010
if gg.total_nsubjects ne 0 then begin
    if n_elements(gg.glm_list) eq 0 then $
        out = names[1:*] $
    else begin
        out = names[2:*]
        dummy = names[2:*]
        idx = where(strpos(dummy,'/') eq -1,cnt)
        if cnt ne 0 then dummy[idx] = directory+'/'+dummy[idx]
        openw,lu,names[1],/GET_LUN
        for i=0,n_elements(out)-1 do begin
            printf,lu,gg.concselect[i]
            if gg.t4select[0] ne '' then printf,lu,gg.t4select[i]
            if gg.evselect[0] ne '' then printf,lu,gg.evselect[i]
            printf,lu,dummy[i]
        endfor
        close,lu
        free_lun,lu
    endelse
endif



action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=script)
if action eq 2 then goto,goback5
openw,lu,script,/GET_LUN

;top_of_script,lu
;for i=0,gg.total_nsubjects-1 do printf,lu,'nice +19 $BIN/fidl_motionreg -dat '+gg.concselect[i]+' -out '+out[i]+' -FD '+fd[0] $
;    +skipstr+moregstr 
;START211010
printf,lu,'#!/bin/bash'+string(10B)+'BIN='+!BINLINUX64+string(10B)
if gg.total_nsubjects ne 0 then begin
    for i=0,gg.total_nsubjects-1 do printf,lu,'$BIN/fidl_motionreg -dat '+gg.concselect[i]+' -out '+out[i]+' -FD '+fd[0] $
        +skipstr+moregstr 
endif else begin
    for i=0,n_elements(gg.extselect)-1 do printf,lu,'$BIN/fidl_motionreg -dat '+gg.extselect[i]+' -FD '+fd[0] $
        +skipstr+moregstr 
endelse


close,lu
free_lun,lu
spawn,'chmod +x '+script
if action eq 0 then begin
    if action eq 0 then scrap='>'+script+'.log' else scrap=''
    spawn,script+scrap+' &'
    stats = dialog_message(script+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
