;Copyright 1/6/12 Washington University.  All Rights Reserved.
;get_index2.pro  $Revision: 1.2 $
function get_index2,ntreatments,index_conditions,sumrows,sng,tf_use,run
conditions = *sng.conditions
frames = *sng.frames
index_which_level = -1
boldconditions = -1
boldframes = -1
boldtf = -1
boldrun = -1
for h=0,sng.num_trials-1 do begin
    for i=0,ntreatments-1 do begin
        for j=0,sumrows[i]-1 do begin
            if conditions[h] eq index_conditions[i,j] then begin
                index_which_level = [index_which_level,i]
                boldconditions = [boldconditions,conditions[h]]
                boldframes = [boldframes,frames[h]]
                boldtf = [boldtf,tf_use[h]]
                boldrun = [boldrun,run[h]]
            endif
        endfor
    endfor
endfor 
index_which_level = index_which_level[1:*]
boldconditions = boldconditions[1:*]
boldframes = boldframes[1:*]
boldtf = boldtf[1:*]
boldrun = boldrun[1:*]
return,rtn={index_which_level:index_which_level,boldconditions:boldconditions,boldframes:boldframes,boldtf:boldtf,boldrun:boldrun}
end
