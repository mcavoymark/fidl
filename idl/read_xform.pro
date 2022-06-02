;Copyright 10/5/01 Washington University.  All Rights Reserved.
;read_xform.pro  $Revision: 1.4 $
function read_xform,t4_file
temp_float = fltarr(16)
if call_external(!SHARE_LIB,'_read_xform',t4_file,temp_float,VALUE=[1,0]) eq 0L then $
    stat = dialog_message('Error reading '+t4_file,/ERROR) $
else $
    t4 = temp_float
return,t4
end
