;Copyright 12/08/05 Washington University.  All Rights Reserved.
;analysis_type1.pro  $Revision: 1.6 $

;**********************************************************************************************
function analysis_type1,fi,dsp,wd,help,space,goback,looptitle,pre_roi_str,NAME=name,NOWTS=nowts
;**********************************************************************************************
wfiles = ''
if goback eq 0 then begin
    lc_t4 = 0
    goback0:
    rtn = get_regions(fi,wd,dsp,help,name)



    ;if rtn.goback eq !TRUE then $
    ;    return,rtn={goback:1} $
    ;else if rtn.error_flag eq !TRUE then $
    ;    return,rtn={goback:-1} $
    ;else begin
    ;    region_names_in = rtn.region_names
    ;    region_str = rtn.region_str
    ;    region_space = rtn.space
    ;    region_file = rtn.region_file
    ;    harolds_num = rtn.harolds_num
    ;endelse

    if rtn.msg eq 'EXIT' then return,rtn={goback:-1} else if rtn.msg eq 'GO_BACK' then return,rtn={goback:1} 
    region_names_in = rtn.region_names
    region_str = rtn.region_str
    region_space = rtn.space
    region_file = rtn.region_file
    harolds_num = rtn.harolds_num



    if space ne region_space then begin
        if space eq !SPACE_DATA and (region_space eq !SPACE_111 or region_space eq !SPACE_222 or region_space eq !SPACE_333) $
        then begin
            space = region_space
            lc_t4 = 1
        endif else begin
            stat=dialog_message('Space incompatibility! Abort! Abort! Abort!',/ERROR)
            rtn={goback:-1}
        endelse
    endif
    loop = n_elements(looptitle)
    if loop eq 0 then begin
        loop = 1
        looptitle = 'Please select regions.'
        pre_roi_str = ' -regions_of_interest '
    endif
    if n_elements(pre_roi_str) eq 0 then pre_roi_str = ' -regions_of_interest '
    nreg = intarr(loop)
    roi_str = '' 
    region_names = ''
    for i=0,loop-1 do begin
        rtn = select_files(region_names_in,TITLE=looptitle[i],/GO_BACK)
        if rtn.files[0] eq 'GO_BACK' then goto,goback0
        nreg[i] = rtn.count
        region_names = [region_names,region_names_in[rtn.index]]
        roi_str = [roi_str,pre_roi_str[i]+strjoin(strtrim(rtn.index+1,2),' ',/SINGLE)]
    endfor      
    region_names = region_names[1:*]
    roi_str = roi_str[1:*]

endif
if not keyword_set(NOWTS) then begin
    regavg_type = get_button(['arithmetic mean','weighted mean'],TITLE='Regional averages should be the')
    if regavg_type eq 1 then wfiles = get_list_of_files('*.img',directory,'Please select weight files.',string(indgen(1000)+1))
endif
return,rtn={goback:0,nreg:nreg,region_names:region_names,region_str:region_str,roi_str:roi_str,lc_t4:lc_t4,wfiles:wfiles, $
    harolds_num:harolds_num,region_file:region_file}
end
