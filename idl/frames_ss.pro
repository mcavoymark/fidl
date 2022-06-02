;Copyright 03/20/03 Washington University.  All Rights Reserved.
;frames_ss.pro  $Revision: 1.15 $
function frames_ss,sng,nrun,tdim_sum,lcround_tc_index
if n_elements(lcround_tc_index) eq 0 then begin
    lcround_tc_index = intarr(sng.num_levels)
    print,'frames_ss  Using frames for full TR placing'
endif
tdim_sum_new = tdim_sum[1:*]
times = *sng.times
frames = *sng.frames

;print,'here100 times=',times
;print,'here100 frames=',frames
;print,'here100 sng.num_trials=',sng.num_trials
;print,'here100 tdim_sum_new=',tdim_sum_new

conditions = *sng.conditions
boldframes = intarr(sng.num_trials)
trialframes = intarr(sng.num_trials)
trialframes_overlap = intarr(sng.num_trials)
conditions_max = make_array(sng.num_levels,/INTEGER,VALUE=tdim_sum_new[nrun-1])

;START120110
run = intarr(sng.num_trials)

j = 0
for i=0,sng.num_trials-2 do begin
    frame0 = frames[i,lcround_tc_index[conditions[i]]]
    frame1 = frames[i+1,lcround_tc_index[conditions[i]]]

    ;if j lt nrun-1 then while times[i] ge float(tdim_sum_new[j])*sng.TR do j = j + 1
    ;START190211
    if j lt nrun-1 then while frame0 ge tdim_sum_new[j] do j = j + 1

    boldframes[i] = frame0
    trialframes[i] = (frame1 < tdim_sum_new[j]) - frame0
    trialframes_overlap[i] = tdim_sum_new[j] - frame0

    ;START120110
    run[i] = j+1

    ;print,'i=',i,' frame0=',frame0,' frame1=',frame1,' j=',j

    ;START110328
    if frame1 lt tdim_sum_new[j] then conditions_max[conditions[i]] = conditions_max[conditions[i]] < trialframes[i]

endfor

i = sng.num_trials-1
frame0 = frames[i,lcround_tc_index[conditions[i]]]


;while frame0 gt tdim_sum_new[j] do j = j + 1
;START190211
;if j lt nrun-1 then while times[i] ge float(tdim_sum_new[j])*sng.TR do j = j + 1
;if j lt nrun-1 then begin
;    print,'hereA double(times[i])=',double(times[i]),' tdim_sum_new[j]*sng.TR=',tdim_sum_new[j]*sng.TR
;    ;while times[i] ge float(tdim_sum_new[j])*sng.TR do begin
;    while double(times[i]) ge tdim_sum_new[j]*sng.TR do begin
;        print,'hereB'
;        j = j + 1
;    endwhile
;endif
if j lt nrun-1 then while frame0 ge tdim_sum_new[j] do j = j + 1


boldframes[i] = frame0
trialframes[i] = tdim_sum_new[j] - frame0
trialframes_overlap[i] = trialframes[i]

;START120110
run[i] = j+1

;START110328
for i=0,sng.num_levels-1 do begin
    index = where(conditions eq i,count)
    if count ne 0 then conditions_max[i] = max(trialframes[index]) 
endfor

;return,rtn={boldframes:boldframes,trialframes:trialframes,trialframes_overlap:trialframes_overlap,conditions_max:conditions_max}
;START120110
return,rtn={boldframes:boldframes,trialframes:trialframes,trialframes_overlap:trialframes_overlap,conditions_max:conditions_max, $
    run:run}

end
