;Copyright 1/5/06 Washington University.  All Rights Reserved.
;get_printer_list.pro  $Revision: 1.2 $

;************************
function get_printer_list
;************************
return,printer_list = strsplit(!PRINTERS,',',/EXTRACT)
end
