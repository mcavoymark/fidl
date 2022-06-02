;Copyright 2/8/07 Washington University.  All Rights Reserved.
;get_identify.pro  $Revision: 1.2 $

;************************************
function get_identify,nfiles,identify
;************************************
flag = make_array(nfiles,/INT,VALUE=-1)
idx = 0
identify_n = 0
identify_i = -1
identify_ni = -1
identify_s = ''
repeat begin
    searchstr = identify[idx[0]]
    index = where(identify eq searchstr,count)
    if count eq 0 then begin
        stat=dialog_message('count should be greater than zero, get_identify',/ERROR)
        return,!ERROR
    endif 
    flag[index] = identify_n
    identify_i = [identify_i,index]
    identify_ni = [identify_ni,count]
    identify_s = [identify_s,searchstr]
    identify_n = identify_n + 1
    idx = where(flag eq -1,count)
endrep until count eq 0
identify_i = identify_i[1:*]
identify_ni = identify_ni[1:*]
identify_s = identify_s[1:*]

starti = intarr(identify_n)
for i=1,identify_n-1 do starti[i] = starti[i-1] + identify_ni[i-1]

is = {Identify_struct}
is.n = identify_n
is.i = ptr_new(identify_i)
is.ni = ptr_new(identify_ni)
is.s = ptr_new(identify_s)
is.starti = ptr_new(starti)
return,is
end
