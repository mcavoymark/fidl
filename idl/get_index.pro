;Copyright 12/15/04 Washington University.  All Rights Reserved.
;get_index.pro  $Revision: 1.6 $
function get_index,ntreatments,nlevels,index_conditions,sumrows,nc,sng
index = -1
index_which_level = -1
index_whichc = -1
ic = -1
ic_by_levels = intarr(nlevels,nc)
nic_by_levels = intarr(nlevels)
k = 0
l = 0
for i=0,ntreatments-1 do begin
    for j=0,sumrows[i]-1 do begin
        scrap = where(*sng.conditions eq index_conditions[i,j],count)
        index = [index,scrap]
        index_which_level = [index_which_level,make_array(count,/INT,VALUE=k)]
        index_whichc = [index_whichc,make_array(count,/INT,VALUE=l)]
        ic = [ic,index_conditions[i,j]]
        ic_by_levels[k,nic_by_levels[k]] = index_conditions[i,j]
        nic_by_levels[k] = nic_by_levels[k] + 1
        l = l + 1
    endfor
    if i eq nlevels-1 then k = 0 else k = k + 1
endfor
index = index[1:*]
index_which_level = index_which_level[1:*]
index_whichc = index_whichc[1:*]
sort_index = sort(index)
index = index[sort_index]
index_which_level = index_which_level[sort_index]
index_whichc = index_whichc[sort_index]
boldframes = *sng.frames
boldframes = boldframes[index] 
boldconditions = *sng.conditions
boldconditions = boldconditions[index]
ic = ic[1:*]

;print,'START get_index'
;print,'ic=',ic
;print,'ic_by_levels=',ic_by_levels
;print,'nic_by_levels=',nic_by_levels
;print,'index=',index
;print,'index_which_level=',index_which_level
;print,'index_whichc=',index_whichc
;print,'boldframes=',boldframes
;print,'boldconditions=',boldconditions
;print,'END get_index'

return,rtn={index:index,index_which_level:index_which_level,index_whichc:index_whichc,ic:ic,ic_by_levels:ic_by_levels, $
    nic_by_levels:nic_by_levels,boldframes:boldframes,boldconditions:boldconditions}
end
