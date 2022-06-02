;Copyright 12/31/99 Washington University.  All Rights Reserved.
;glm_4dfp.pro  $Revision: 1.7 $

;**********************************
pro glm_4dfp,fi,dsp,wd,glm,help,stc
;**********************************

;Get image header.
hdr = *fi.hdr_ptr(fi.n)
hd = *fi.hdr_ptr(hdr.mother)

if(hdr.model eq fi.n) then $
    model = hdr.model $ ; Use current file if it is a model.
else $
    model = hd.model  ; Otherwise use model attached to data file.
hdr = *fi.hdr_ptr(model)

if(model lt 0) then begin
    stat = widget_message('Model estimates are not defined.',/ERROR)
    return
endif

if(hdr.array_type ne !LINEAR_MODEL) then begin
    stat = widget_message('Designated file is not a glm file.')
    return
endif

if(help.enable eq !TRUE) then begin
    widget_control,help.id,SET_VALUE=string('This routine extracts time-courses as specified by the contrasts.' $
    +'  If a contrast has a positive sign for a specific effect, i.e., the Boynton model has a positive coefficient,' $
    +' the time-course of that effect is added.  If the contrast has a negative sign for an effect,' $
    +' the time-course for that effect is subtracted.  The sum is unweighted, i.e.,' $
    +' time-courses are simply summed or differenced.'+string([10B,10B]) $
    +'  The result is identical to that obtained with the IDL version, but the results are automatically stored on disk,' $
    +' and the command can be used in a shell script.  The C program name is +string(34B)+compute_zstat+string(34B)')
endif

glm_file = fi.names[model]
if strpos(glm_file,'.glm') eq -1 then $
    glm_file = strcompress(glm_file,/REMOVE_ALL) + '.glm'

print,fi.names[model]
get_lun,lu
openr,lu,glm_file
s = string("         ")
readf,lu,s,FORMAT='(a)'
if(strpos(s,"INTERFILE") ge 0) then begin
    lc_interfile = !TRUE
    get_mri_ifh,lu,ifh
    rev = ifh.glm_rev
endif else begin
    point_lun,lu,0
    rev = 0
    readu,lu,rev
endelse
close,lu
free_lun,lu

help,rev
if(rev gt -17) then begin
    stat=widget_message('glm file revision number too old. glms can be updated by resaving.')
    return
endif

glm_labels = *glm[model].contrast_labels
contrast = get_button(glm_labels,TITLE='Contrast numbers')
cstr = string(contrast+1,FORMAT='(a," ",i2)')

openw,lu_csh,'glm_4dfp.csh',/GET_LUN
printf,lu_csh,FORMAT='("#!/bin/csh")'
printf,lu_csh,''
printf,lu_csh,FORMAT='("if($#argv != 2) then")'
printf,lu_csh,FORMAT='("    echo ",a)',string([34B,32B,34B])
printf,lu_csh,FORMAT='("    echo Invalid number of arguments.")'
printf,lu_csh,FORMAT='("    echo Usage: glm_4dfp.csh filename contrast")'
printf,lu_csh,FORMAT='("    exit")'
printf,lu_csh,FORMAT='("endif")'
printf,lu_csh,''
printf,lu_csh,FORMAT='("compute_zstat -glm_file $1 -extract_timecourse $2 -extract_timecourse_tags LABELS -one_file -dat -dof")'
close,lu_csh
free_lun,lu_csh
spawn,'chmod +x glm_4dfp.csh'

command = string(glm_file,cstr,FORMAT='("glm_4dfp.csh ",a,a)')
print,''
print,'SCRIPT TO EXTRACT TIMECOURSE.'
print,'-----------------------------'
print,command,string(10B) ;13B doesn't skip a line
widget_control,/HOURGLASS
print,'OUTPUT OF glm_4dfp.csh SCRIPT'
print,'------------------------------'
spawn,command
print,string(10B),'DONE' ;13B doesn't skip a line
end
