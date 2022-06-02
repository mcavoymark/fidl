;Copyright 2/17/11 Washington University.  All Rights Reserved.
;fidl_swap.pro  $Revision: 1.4 $
pro fidl_swap,fi,dsp,wd,help,pref
get_dialog_pickfile,'*.img',fi.path,'Please select files.',files,nfiles,path,/MULTIPLE_FILES
if nfiles eq 0 then return
csh = 'fidl_swap.csh' 
openw,lu,csh,/GET_LUN
top_of_script,lu
printf,lu,''
print_files_to_csh,lu,nfiles,files,'FILES','files'
printf,lu,'nice +19 $BIN/fidl_swap $FILES'
close,lu
free_lun,lu
spawn,'chmod +x '+csh
spawn,'source '+csh+' &'
stats = dialog_message('fidl_swap has been executed.',/INFORMATION)
print,'DONE'
end
