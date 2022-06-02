;Copyright 3/27/13 Washington University.  All Rights Reserved.
;fidl_combinations.pro  $Revision: 1.6 $
function fidl_combinations,tot_eff,effect_label,scratchdir

    ;spawn,!BINEXECUTE+'/fidl_timestr',timestr
    ;START150610
    spawn,!BINEXECUTE+'/fidl_timestr2',timestr

    cmd = !BINEXECUTE+'/fidl_combinations -setsize '+strtrim(tot_eff,2)+' -out '+scratchdir+'fidl_combinations'+timestr+'.txt'
    print,cmd
    widget_control,/HOURGLASS
    spawn,cmd,result
    print,result
    idx = where(strpos(result,'fidlError') ne -1,cnt)
    if cnt ne 0 then begin
        stat=dialog_message_long('ERROR',strjoin(result[idx],string(10B),/SINGLE))
        return,rtn={msg:'ERROR'}
    endif
    strs = strsplit(result[0],/EXTRACT)
    strs = strs[n_elements(strs)-1]
    rtn2=readf_ascii_file(strs,/NOHDR)
    F_names = strarr(rtn2.NR)
    data = fix(rtn2.data)
    for j=0,rtn2.NR-1 do F_names[j] = strjoin(effect_label[data[0:rtn2.NF_each_line[j]-1,j]],'&',/SINGLE)
    nF = rtn2.NR

    ;START140122
    ;spawn,'rm -f '+strs

    return,rtn={msg:'OK',nF:nF,F_names:F_names}
end
