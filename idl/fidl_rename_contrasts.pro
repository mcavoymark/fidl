;Copyright 6/7/06 Washington University.  All Rights Reserved.
;fidl_rename_contrasts.pro  $Revision: 1.12 $
pro fidl_rename_contrasts,fi,dsp,wd,glm,help,stc,pref
goback7:

;scrap=get_glms()
;START140320
scrap=get_glms(/NOTMORETHANONE)

if scrap.msg ne 'OK' then return
load = scrap.load
glmfiles = scrap.glmfiles
nlists = scrap.nlists
total_nsubjects = scrap.total_nsubjects
t4select = scrap.t4select
glm_list_str = scrap.glm_list
get_labels_struct = replicate({Get_labels_struct},total_nsubjects)
for i=0,total_nsubjects-1 do begin
    get_labels_struct[i] = get_labels_from_glm(fi,dsp,wd,glm,help,pref,ifh,glmfiles[i],/CONTRASTS_ONLY)
    if get_labels_struct[i].ifh.glm_rev gt -17 then begin
        stat=dialog_message('Revision number for '+strmid(glmfiles[i],1)+' too old. Update file by resaving.',/ERROR)
        return
    endif
endfor
nc_max = max(get_labels_struct[*].ifh.glm_nc,nc_max_glm_index,MIN=nc_min)

goback8:
contrast_labels = *get_labels_struct[nc_max_glm_index].scraplabels
scrap = get_str(nc_max,contrast_labels,contrast_labels,TITLE='Please rename contrasts.',/GO_BACK,FRONT='1',/ONE_COLUMN)
if scrap[0] eq 'GO_BACK' then goto,goback7
index = where(strcmp(contrast_labels,scrap) eq 0,nchange)
if nchange eq 0 then return
;print,'nchange=',nchange
;print,'index=',index
newnamestr = ' -c_names "' + strjoin(strtrim(scrap[index],2),'" "',/SINGLE) + '"'
;print,'newnamestr=',newnamestr

sumrows = make_array(nchange,/INT,VALUE=1)
scrap = cft_and_scft(total_nsubjects,nchange,nc_max,sumrows,index,contrast_labels,get_labels_struct,1, $
    glmfiles,contrast_labels,0,'Please select contrast.')
if scrap.sum_contrast_for_treatment[0,0] eq -2 then $
    return $
else if scrap.sum_contrast_for_treatment[0,0] eq -1 then $
    goto,goback8
contrast = scrap.sum_contrast_for_treatment
;print,'contrast=',contrast
;print,'size(contrast)=',size(contrast)

cstr = ' -contrasts'
scrap = strtrim(contrast,2)
for i=0,nchange-1 do cstr = cstr + ' ' + strjoin(scrap[*,i],',',/SINGLE) 
;print,'cstr=',cstr
    
goback9:
names = get_str(1,'script','fidl_rename_contrasts.csh',TITLE='Please name.',/GO_BACK)
if scrap[0] eq 'GO_BACK' then goto,goback8
csh = fix_script_name(names[0])

openw,lu,csh,/GET_LUN
top_of_script,lu
print_files_to_csh,lu,total_nsubjects,glmfiles,'GLM_FILES','glm_files'
printf,lu,'nice +19 $BIN/fidl_rename_contrasts $GLM_FILES'+cstr+newnamestr
close,lu
free_lun,lu
spawn,'chmod +x '+csh

;action = get_button(['execute with log','return','go back'],TITLE='Please select action.'+string(10B)+string(10B) $
;    +'To run on linux, please select return. Login to a linux machine and enter the script on the command line.',BASE_TITLE=csh)
action = get_button(['execute with log','return','go back'],TITLE='Please select action.',BASE_TITLE=csh)

if action eq 2 then goto,goback9
if action eq 0 then begin
    spawn,csh+'>'+csh+'.log'+' &'
    stats = dialog_message(csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'

end
