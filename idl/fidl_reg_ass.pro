;Copyright 11/4/03 Washington University.  All Rights Reserved.  
;fidl_reg_ass.pro  $Revision: 1.5 $

;******************************
pro fidl_reg_ass,fi,wd,dsp,help
;******************************

goback0:
rtn = get_regions(fi,wd,dsp,help)



;if rtn.goback eq !TRUE then $
;    return $
;else if rtn.error_flag eq !TRUE then $
;    return $
;else begin
;    region_names = rtn.region_names
;    region_str = rtn.region_str
;endelse

if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then return 
region_names = rtn.region_names
region_str = rtn.region_str




plot_reg_ass,region_names,REGION_STR=region_str

print,'DONE'
end
