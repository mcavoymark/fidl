;Copyright 7/22/09 Washington University.  All Rights Reserved.
;fidl_rename_effects.pro $Revision: 1.12 $
pro fidl_rename_effects,fi,dsp,wd,glm,help,stc,pref
spawn,!BINEXECUTE+'/fidl_timestr2',timestr
goback7:
scrap=get_glms(/NOTMORETHANONE)
if scrap.msg ne 'OK' then return
load = scrap.load
glmfiles = scrap.glmfiles
nlists = scrap.nlists
total_nsubjects = scrap.total_nsubjects
t4select = scrap.t4select
glm_list_str = scrap.glm_list

;if glm_list_str ne '' then glm_list_str = ' -glm_list_file '+glm_list_str
;START170531
if glm_list_str[0] ne '' then glm_list_str = ' -glm_list_file '+strjoin(glm_list_str,' ',/SINGLE)

get_labels_struct = replicate({Get_labels_struct},total_nsubjects)
for i=0,total_nsubjects-1 do begin
    get_labels_struct[i] = get_labels_from_glm(fi,dsp,wd,glm,help,pref,ifh,glmfiles[i],/CONTRASTS_ONLY)
    if get_labels_struct[i].ifh.glm_rev gt -17 then begin
        stat=dialog_message('Revision number for '+strmid(glmfiles[i],1)+' too old. Update file by resaving.',/ERROR)
        return
    endif
endfor
ne_max = max(get_labels_struct[*].ifh.glm_all_eff,ne_max_glm_index,MIN=ne_min)
effect_label = *get_labels_struct[ne_max_glm_index].ifh.glm_effect_label
for i=0,total_nsubjects-1 do begin
    if i ne ne_max_glm_index then begin
        scrap=*get_labels_struct[i].ifh.glm_effect_label
        for j=0,get_labels_struct[i].ifh.glm_all_eff-1 do begin
            if total(strcmp(effect_label,scrap[j])) eq 0 then effect_label = [effect_label,scrap[j]]
        endfor
    endif
endfor
effectlabels = replicate(ptr_new(),total_nsubjects)
for m=0,total_nsubjects-1 do effectlabels[m] = get_labels_struct[m].ifh.glm_effect_label
goback8:
scrap = get_str(ne_max,effect_label,effect_label,TITLE='Please rename effects.',/GO_BACK,FRONT='2_',BACK='_2',/ONE_COLUMN, $
    /REPLACE,WIDTH=100)
if scrap[0] eq 'GO_BACK' then goto,goback7
index = where(scrap ne '',ncontrast)
if ncontrast eq 0 then return
sumrows = make_array(ncontrast,/INT,VALUE=1)
label = effect_label[index]
effect_labels = fix_script_name(scrap[index])
scrap = cft_and_scft(total_nsubjects,ncontrast,ne_max,sumrows,index,effect_label,get_labels_struct,$
    1,glmfiles,label,0,'Please select effect.',effectlabels,/MATCH,/NOTPRESENT)
if scrap.sum_contrast_for_treatment[0,0,0] eq -2 then $
    return $
else if scrap.sum_contrast_for_treatment[0,0,0] eq -1 then $
    goto,goback8
sum_contrast_for_treatment = scrap.sum_contrast_for_treatment
goback10:
scrap = ['script']

;scraplabels = ['fidl_rename_effects.csh']
;START150108
scraplabels = ['fidl_rename_effects'+timestr[0]+'.csh']

names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=50,TITLE='Please enter desired names.',/GO_BACK)
if names[0] eq 'GO_BACK' then goto,goback8
csh = fix_script_name(names[0])
openw,lu,csh,/GET_LUN
top_of_script,lu
for i=0,total_nsubjects-1 do begin
    index = where(sum_contrast_for_treatment[i,*,0] gt 0,count)
    if count ne 0 then begin
        contrast_str = ' -contrasts ' + strjoin(strtrim(indgen(count)+1,2),' ',/SINGLE)
        effect_label_str = ' -effect_labels "' + strjoin(effect_labels[index],'" "',/SINGLE) + '"'
        printf,lu,'nice +19 $BIN/fidl_rename_effects -glm_files '+glmfiles[i]+contrast_str+effect_label_str $
            +' -log '+csh+'.log'
    endif
endfor
close,lu
free_lun,lu
spawn,'chmod +x '+csh
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 2 then goto,goback10 
if action eq 0 then begin

    ;spawn,'source '+csh+'>'+csh+'.log &'
    ;START131009
    spawn,'source '+csh+' &'

    stats = dialog_message(csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
