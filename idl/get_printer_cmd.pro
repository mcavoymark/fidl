;Copyright 1/5/06 Washington University.  All Rights Reserved.
;get_printer_cmd.pro  $Revision: 1.2 $

;*********************************************
function get_printer_cmd,printer_name,filename
;*********************************************
return,cmd='lpr -P "'+printer_name+'" '+filename
end
