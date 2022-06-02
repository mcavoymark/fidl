;Copyright 7/12/01 Washington University.  All Rights Reserved.
;fix_grand_mean.pro  $Revision: 1.8 $
pro fix_grand_mean,fi,wd,dsp,help,stc


;goback0:
;files = get_files(FILTER='*.glm',/EXIT)
;if files[0] eq 'EXIT' then return 
;goback1:
;rtn = select_files(files,TITLE='Please select GLMs.',MIN_NUM_TO_SELECT=1,/GO_BACK)
;if rtn.files[0] eq 'GO_BACK' then goto,goback0
;files = rtn.files
;nfiles = rtn.count
;START131015
goback0:
gg=get_glms(pref)
if gg.msg eq 'GO_BACK' or g.msg eq 'EXIT' then return
files = gg.glmfiles
nfiles = gg.total_nsubjects


names = get_str(nfiles+1,['script',files],['fix_mean.csh',files],WIDTH=60,TITLE='Please enter desired filenames.', $
    LABEL='You may retain the same names if you so desire.',/GO_BACK)

;if names[0] eq 'GO_BACK' then goto,goback1
;START131015
if names[0] eq 'GO_BACK' then goto,goback0

fix_mean_csh = fix_script_name(names[0])
get_directory,directory
openw,lu_csh,directory+'/'+fix_mean_csh,/GET_LUN

;printf,lu_csh,FORMAT='("#!/bin/csh")'
;printf,lu_csh,FORMAT='("unlimit")'
;printf,lu_csh,''
;START131015
top_of_script,lu_csh

if nfiles eq 1 then begin
    printf,lu_csh,'set GLM_FILES = (-glm_files ',files[0],')',FORMAT='(a,a,a)'
endif else begin
    printf,lu_csh,'set GLM_FILES = (-glm_files  \'
    for j=0,nfiles-2 do printf,lu_csh,'        ',files[j],' \',FORMAT='(a,a,a)'
    printf,lu_csh,'        ',files[j],')',FORMAT='(a,a,a)'
    printf,lu_csh,''
endelse
printf,lu_csh,''
if nfiles eq 1 then begin
    printf,lu_csh,'set NEW_GLM_NAMES = (-new_glm_names ',names[1],')',FORMAT='(a,a,a)'
endif else begin
    printf,lu_csh,'set NEW_GLM_NAMES = (-new_glm_names  \'
    for j=1,nfiles-1 do printf,lu_csh,'        ',names[j],' \',FORMAT='(a,a,a)'
    printf,lu_csh,'        ',names[j],')',FORMAT='(a,a,a)'
    printf,lu_csh,''
endelse
printf,lu_csh,''
printf,lu_csh,FORMAT='("fix_grand_mean $GLM_FILES $NEW_GLM_NAMES |& mail `whoami`")'
close,lu_csh
free_lun,lu_csh
spawn,'chmod +x '+fix_mean_csh
action = get_button(['execute','return'],TITLE='Select Action for '+fix_mean_csh)
if action eq 0 then begin
    spawn,fix_mean_csh+' &'
    stats = dialog_message(fix_mean_csh+' submitted as batch job.')
endif
print,'DONE'
end
