;Copyright 12/31/99 Washington University.  All Rights Reserved.
;checkmprage.pro  $Revision: 1.2 $

;*****************************
pro checkmprage,fi,wd,dsp,help
;*****************************

filnam_4dfp = dialog_pickfile(FILTER='*img',PATH=fi.path, $
       GET_PATH=wrtpath,TITLE='Select mprage file.')
dot = strpos(filnam_4dfp,'.')
if(dot gt 0) then $
    stem = strmid(filnam_4dfp,0,dot) $
else $
    stem = filnam_4dfp
path = strmid(stem,0,rstrpos(filnam_4dfp,'/'))
tail = strmid(stem,rstrpos(filnam_4dfp,'/')+1)

openw,lu_csh,'cover.csh',/GET_LUN
printf,lu_csh,FORMAT='("#!/bin/csh")'
printf,lu_csh,FORMAT='("cd ",a)',path
printf,lu_csh,FORMAT='("checkmprage ",a)',tail
close,lu_csh
free_lun,lu_csh
spawn,'csh cover.csh'
spawn,'rm cover.csh'

print,'DONE'
end

