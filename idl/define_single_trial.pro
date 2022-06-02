;Copyright 12/31/99 Washington University.  All Rights Reserved.
;define_single_trial.pro  $Revision: 12.157 $
function define_single_trial,fi,pref,help,BATCH=batch,DEFAULT=default,EVENT_FILE=event_file,SUGGESTION=suggestion,HDR_ONLY=hdr_only
if keyword_set(DEFAULT) then lcdefault = !TRUE else lcdefault = !FALSE
if not keyword_set(HDR_ONLY) then hdr_only = 0 

if not keyword_set(EVENT_FILE) then begin
    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Defines a single trial paradigm based on a file ' $
        +'supplied by the user.  The first line of this file should contain the TR followed by a text string for each ' $
        +'effect.  The remaining lines should contain the time in seconds in the first column, and a code for the ' $
        +'stimuli/response in the second column.  These codes should be numbered from 0 to N-1 where N-1 is the number of ' $
        +'effects.  This file can be defined using Excel templates at http://imaging.wustl.edu/Ollinger/fidl/fmri_analysis.htm'
    if keyword_set(SUGGESTION) then $
        get_dialog_pickfile,'*fi??',fi.path[0],'Please select an event file.',filename,rtn_nfiles,rtn_path,FILE=suggestion, $
            /DONT_ASK $
    else $
        get_dialog_pickfile,'*fi??',fi.path[0],'Please select an event file.',filename,rtn_nfiles,rtn_path,/LINMOD
endif else begin
    filename = event_file
endelse

;START160524
sng1 = {Eventfile}

if filename eq 'GOBACK' or filename eq 'EXIT' then return,rtn={name:'ERROR'}
;START160524
;if filename eq 'GOBACK' or filename eq 'EXIT' then begin
;    sng1.name='ERROR'
;    return,sng1
;endif

if filename eq '' then $
    return,rtn={event_file:''} $
else if filename eq 'NONE' then $
    return,rtn={event_file:'NONE'}
print,'Reading '+filename
widget_control,/HOURGLASS
cnt = readf_ascii_file(filename,HDR_ONLY=hdr_only)
if cnt.name eq 'ERROR' then return,rtn={name:'ERROR'}

num_levels = n_elements(cnt.hdr) - 1
labels = fix_script_name(cnt.hdr[1:num_levels],/FORWARD_SLASH)
if hdr_only eq 1 then return,rtn={name:'',num_levels:num_levels,factor_labels:labels}

;print,'here100 size(cnt.data)=',size(cnt.data)
;cnt.data=reform(cnt.data,cnt.NF,cnt.NR,/OVERWRITE) ;NECESSARY if cnt.NR=1
;cnt.data=reform(cnt.data,2,1,/OVERWRITE) ;NECESSARY if cnt.NR=1
    ;Tried this back in readf_ascii_file, but dimension is truncated after being passed. 
;print,'here101 size(cnt.data)=',size(cnt.data)
data = cnt.data
data=reform(data,cnt.NF,cnt.NR,/OVERWRITE) ;NECESSARY if cnt.NR=1
    ;Tried this back in readf_ascii_file, but dimension is truncated after being passed. 
    ;Tried sending cnt.dat as ptr_new(cnt.data) but dimension still truncated.
    ;Also need to rename data = cnt.data for dimensions to take.
;print,'here102 size(data)=',size(data)
;data = *cnt.data
;print,'here100 size(data)=',size(data)

conditions = reform(fix(data[1,*]))
if cnt.NF gt 2 then $
    stimlen_vs_t = reform(float(data[2,*])) $ ;The float cast is totally necessary, otherwise stimlen_vs_t is a string.
else $
    stimlen_vs_t = fltarr(cnt.NR)

;START160524
if stregex(cnt.hdr[0],'[^0-9.]') ne -1 then begin
    spawn,'whoami',whoami
    whoamistr = 'Hey '+whoami[0]+'!'
    scrap = get_button('exit',TITLE='You need to specify a proper TR in '+string(10B)+filename,BASE_TITLE=whoamistr)
    ;return,rtn={name:'ERROR'}
    sng1.name='ERROR'
    return,sng1
endif

TR = double(cnt.hdr[0])

;print,'define_single_trial cnt.hdr[0]=',cnt.hdr[0]
;print,'define_single_trial TR=',TR
;print,"define_single_trial stregex(cnt.hdr[0],'[^0-9.]')=",stregex(cnt.hdr[0],'[^0-9.]')

stimlenframes_vs_t = stimlen_vs_t/TR
nbehavcol = 0
behav_vs_t = !FALSE
if cnt.NF gt 3 then begin
    behav_vs_t = strupcase(data[3:*,*])
    j = 0
    for i=3,cnt.NF-1 do begin
        index = where(behav_vs_t[j,*] eq '' or behav_vs_t[j,*] eq 'NA',count)
        if count gt 0 then behav_vs_t[j,index] = !UNSAMPLED_VOXEL
        j = j + 1
    endfor
    behav_vs_t = float(behav_vs_t)
    nbehavcol = cnt.NF - 3
endif

; Compute number of levels per factor as maximum over each condition.
; This assumes that factors are labeled sequentially from zero.
; Assume that the header is a list of one-word descriptions of the levels
; of each factor, starting with the first factor, first level, and ending
; with the last factor, last level.  Words are delimited by single spaces.
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Number of frames to skip defines the number of frames ' $
    +'at the beginning of each run that are to be skipped.  Period of HRF in frames defines how the length of the assumed ' $
    +'hemodynamic response function.  Duration of stimulus is an estimate of the duration of the applied stimulus (or the ' $
    +'duration of neuronal firing).  This is used to compute the assumed hemodynamic response function.'



;times = reform(double(data[0,0:cnt.NR-1]))
;frames_float =  times/TR
;frames = round(frames_float)
;frames_floor = floor(frames_float)
;fraction = frames_float - frames_floor
;frames_halfTR = frames_floor 
;frames_quarterTR = frames_floor
;offsets_halfTR = intarr(cnt.NR)
;for i=0,cnt.NR-1 do begin
;    if fraction[i] lt .25 then begin 
;        ;do nothing
;    endif else if fraction[i] lt .75 then begin 
;        offsets_halfTR[i] = 1
;        frames_halfTR[i] = frames_halfTR[i] + 1
;    endif else $
;        frames_halfTR[i] = frames_halfTR[i] + 1
;endfor
;offsets_quarterTR = intarr(cnt.NR)
;for i=0,cnt.NR-1 do begin
;    if fraction[i] lt .125 then begin 
;        ;do nothing
;    endif else if fraction[i] lt .375 then begin 
;        offsets_quarterTR[i] = 3 
;        frames_quarterTR[i] = frames_quarterTR[i] + 1
;    endif else if fraction[i] lt .625 then begin 
;        offsets_quarterTR[i] = 2 
;        frames_quarterTR[i] = frames_quarterTR[i] + 1
;    endif else if fraction[i] lt .875 then begin 
;        offsets_quarterTR[i] = 1 
;        frames_quarterTR[i] = frames_quarterTR[i] + 1
;    endif else $
;        frames_quarterTR[i] = frames_quarterTR[i] + 1
;endfor
;effect_shift_TR = fltarr(num_levels) ;just keep this at zero for now 030725
;frames = [[frames],[frames_halfTR],[frames_quarterTR]]
;offsets = [[intarr(cnt.NR)],[offsets_halfTR],[offsets_quarterTR]]
;START141124
times = fltarr(cnt.NR)
fraction = fltarr(cnt.NR)
frames = lonarr(cnt.NR)
frames_halfTR = lonarr(cnt.NR)
frames_quarterTR = lonarr(cnt.NR)
offsets_halfTR = lonarr(cnt.NR)
offsets_quarterTR = lonarr(cnt.NR)
stat=call_external(!SHARE_LIB,'_timesframesoffsets',filename,long(cnt.NR),times,fraction,frames,frames_halfTR,frames_quarterTR, $
    offsets_halfTR,offsets_quarterTR,VALUE=[1,1,0,0,0,0,0,0,0])
if stat eq 0L then begin
    print,'Error in _timesframesoffsets'
    return,rtn={name:'ERROR'} 
endif
effect_shift_TR = fltarr(num_levels)
frames = [[frames],[frames_halfTR],[frames_quarterTR]]
offsets = [[lonarr(cnt.NR)],[offsets_halfTR],[offsets_quarterTR]]



index_present = intarr(num_levels)
count_present = 0
for i=0,num_levels-1 do begin
    index = where(conditions eq i,count)
    if count eq 0 then $
        print,'No frames of data for condition '+strtrim(i,2) $
    else begin
        index_present[count_present] = i
        count_present = count_present + 1
    endelse
endfor
index_present = index_present[0:count_present-1]

if filename eq 'timecourse_tmp.fidl' then $
    lcsim = !TRUE $
else $
    lcsim = !FALSE

if max(conditions) ge num_levels then begin
    print,filename
    print,'    max(conditions)=',max(conditions),' num_levels=',num_levels 
    spawn,'whoami',whoami
    dummy = 'Hey '+whoami+'!'+string(10B)+string(10B)+filename+string(10B)+string(10B)+strtrim(num_levels,2) $ 
        +' conditions are listed following the TR, but condition numbers go as high as '+strtrim(max(conditions),2) $
        +'.'+string(10B)+'It seems that the largest number in the second column should be '+strtrim(num_levels-1,2)+'.'
    ;print,dummy
    stat=dialog_message(dummy,/ERROR)
    return,rtn={name:'ERROR'}
endif

stimlen = fltarr(num_levels)
if n_elements(stimlen_vs_t) gt 1 then begin
    for i=0,cnt.NR-1 do if conditions[i] ge 0 then stimlen[conditions[i]] = stimlen_vs_t[i]
endif
if total(stimlen) lt 0.0001 then stimlen[*] = pref.glm_stim_duration

;print,'here0 define_single_trial cnt.NR=',cnt.NR,' num_levels=',num_levels
;print,'here0 define_single_trial frames'
;print,frames
;print,'here0 define_single_trial fraction'
;print,fraction
;print,'here0 define_single_trial size(offsets)=',size(offsets)
;print,'here0 define_single_trial offsets=',offsets
;print,'here0 define_single_trial num_offsets_eff=',num_offsets_eff
;print,'here0 define_single_trial effect_shift_TR=',effect_shift_TR
;print,'here0 define_single_trial stimlen_vs_t=',stimlen_vs_t
;print,'here0 define_single_trial stimlen=',stimlen


;frames_block  = reform(fix(cnt.data[0,0:cnt.NR-1]),cnt.NR)
frames_block  = reform(fix(data[0,0:cnt.NR-1]),cnt.NR)

;START160524
;sng1 = {Eventfile}

sng1.name = ''
sng1.init_ctl = pref.parainit
sng1.period = pref.period
sng1.factor_labels = ptr_new(labels)
sng1.conditions = ptr_new(conditions) 
sng1.num_trials = cnt.NR
sng1.num_levels = num_levels
sng1.frames = ptr_new(frames)
sng1.TR = TR
sng1.offsets = ptr_new(offsets)
sng1.stimlen = ptr_new(stimlen)
sng1.stimlen_vs_t = ptr_new(stimlen_vs_t)
sng1.stimlenframes_vs_t = ptr_new(stimlenframes_vs_t)
sng1.event_file = filename
sng1.effect_shift_TR = ptr_new(effect_shift_TR)
sng1.nbehavcol = nbehavcol
sng1.behav_vs_t = ptr_new(behav_vs_t)
sng1.index_present = ptr_new(index_present)
sng1.count_present = count_present
sng1.times = ptr_new(times)
sng1.frames_block = ptr_new(frames_block)
sng1.fraction = ptr_new(fraction)

;START141125
;sng1.frames_floor = ptr_new(frames_floor)

;print,'define_single_trial bottom'
return,sng1
end
