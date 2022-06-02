;Copyright 4/13/04 Washington University.  All Rights Reserved.
;set_batch.pro  $Revision: 1.4 $

;**********************************************
function set_batch,csh_file 
;**********************************************

openw,lu,'tmp.txt',/GET_LUN
cd,CURRENT=cdir
printf,lu,'csh ' + csh_file
close,lu
free_lun,lu
spawn,'chmod 0777 '+ 'tmp.txt'
spawn,'at -q b -m now -c -m < '+cdir+'/tmp.txt |& tee '+csh_file+'.txt',result
print,result
;stats = dialog_message('Analysis submitted as batch job using the script '+csh_file+'.'+string(10B)+'The log file(s) will ' $
;    +'be e-mailed to you upon completion.',/INFORMATION)

return,!OK
end
