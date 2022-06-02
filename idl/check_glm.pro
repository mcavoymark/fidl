;Copyright 12/31/99 Washington University.  All Rights Reserved.
;check_glm.pro  $Revision: 1.11 $

;***********************************
;pro check_glm,fi,dsp,wd,glm,help,stc
pro check_glm,fi,dsp,wd,glm,help
;***********************************

;if fi.nfiles eq 0 then wd1_load_linmod,fi,dsp,wd,glm,help,stc
if fi.nfiles eq 0 then wd1_load_linmod,fi,dsp,wd,glm,help

hdr = *fi.hdr_ptr[fi.n]
hd = *fi.hdr_ptr[hdr.mother]
if hdr.model eq fi.n then $
    model = hdr.model $ ; Use current file if it is a model.
else $
    model = hd.model  ; Otherwise use model attached to data file.
if model lt 0 then begin
    stat = dialog_message('Model estimates are not defined.',/ERROR)
    return
endif
hdr = *fi.hdr_ptr[model]
if hdr.array_type ne !LINEAR_MODEL then begin
    stat = dialog_message('Designated file is not a glm file.')
    return
endif
if hdr.ifh.glm_rev gt -17 then begin
    stat=dialog_message('glm file revision number too old. glms can be updated by resaving.',/ERROR)
    return
endif
glm_file = fi.names[model]
if strpos(glm_file,'.glm') eq -1 then glm_file = strcompress(glm_file,/REMOVE_ALL) + '.glm'

openw,lu_csh,'check_glm.csh',/GET_LUN
printf,lu_csh,FORMAT='("#!/bin/csh")'
printf,lu_csh,''
printf,lu_csh,FORMAT='("if($#argv != 1) then")'
printf,lu_csh,FORMAT='("    echo ",a)',string([34B,32B,34B])
printf,lu_csh,FORMAT='("    echo Invalid number of arguments.")'
printf,lu_csh,FORMAT='("    echo Usage: check_glm.csh filename")'
printf,lu_csh,FORMAT='("    exit")'
printf,lu_csh,FORMAT='("endif")'
printf,lu_csh,''
printf,lu_csh,FORMAT='("compute_zstat -glm_file $1 -echo")'
close,lu_csh
free_lun,lu_csh
spawn,'chmod +x check_glm.csh'

command = string(glm_file,FORMAT='("check_glm.csh ",a)')
print,''
print,'SCRIPT TO CHECK GLM.'
print,'--------------------'
print,command,string(10B) ;13B doesn't skip a line
widget_control,/HOURGLASS
print,'OUTPUT OF check_glm.csh SCRIPT'
print,'------------------------------'
spawn,command
print,string(10B),'DONE' ;13B doesn't skip a line
end
