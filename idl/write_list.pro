;Copyright 5/2/02 Washington University.  All Rights Reserved.
;write_list.pro  $Revision: 1.9 $
pro write_list,filename,nfiles_per_list,files
    nlists = n_elements(nfiles_per_list)
    if nlists eq 2 then if nfiles_per_list[1] eq 0 then nlists = 1
    openw,lu,filename,/GET_LUN
    
    ;START140415
    ;printf,lu,'number_of_lists:'+strtrim(nlists,2)
    
    k = 0
    for i=0,nlists-1 do begin
        
        ;START140415
        ;printf,lu,'number_of_files:'+strtrim(nfiles_per_list[i],2)
    
        for j=0,nfiles_per_list[i]-1 do begin
            printf,lu,files[k]
            k = k + 1
        endfor
    endfor
    close,lu
    free_lun,lu
end
