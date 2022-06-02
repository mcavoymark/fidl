;Copyright 7/12/17 Washington University.  All Rights Reserved.
;getproc.pro  $Revision: 1.1 $
function getproc,pref,total_nsubjects
    spawn,'cat /proc/cpuinfo | grep processor | wc -l',processor
    print,'processor=',processor
    if pref.cpu ne '' then superbird=100/pref.cpu else superbird=2
    proc=(processor/superbird)>1
    print,'superbird=',superbird
    proc=proc[0]<total_nsubjects
    proceach=ceil(float(total_nsubjects)/proc)
    print,'proc='+trim(proc)+' proceach='+trim(proceach)+' glms will be assigned to each processor'
    wallace0=trim(indgen(proc)+1)
    idx=where(strlen(wallace0) eq 1,cnt)
    if cnt ne 0 then wallace0[idx]='0'+wallace0[idx]
    return,rtn={proc:proc,proceach:proceach,wallace0:wallace0}
end
