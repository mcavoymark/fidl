;Copyright 12/31/99 Washington University.  All Rights Reserved.
;assign_region_names.pro  $Revision: 1.6 $
;********************************************************************
pro assign_region_names,ifh,region_img,nreg,REGION_NAMES=region_names 
;********************************************************************
if(keyword_set(REGION_NAMES)) then $
    str = region_names[0:nreg-1] $
else begin
    str = strarr(nreg) 
    for i=0,nreg-1 do str[i] = 'voxel_value' + strtrim(i+2,2)
endelse

;START120809
;if(not ptr_valid(ifh.region_names)) then ptr_free,ifh.region_names

for i=0,nreg-1 do begin
    scrap = strsplit(str[i],/EXTRACT)

    ;str[i] = scrap[0] + ' ' + strcompress(string(long(total(region_img eq (i+2)))),/REMOVE_ALL)
    ;START120809
    str[i] = scrap[0] + ' ' + strtrim(long(total(region_img eq (i+2))),2)

endfor

;START120809
if ptr_valid(ifh.region_names) then ptr_free,ifh.region_names
ifh.nreg = nreg
ifh.region_names = ptr_new(str)
end
