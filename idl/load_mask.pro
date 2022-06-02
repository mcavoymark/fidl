;Copyright 3/11/05 Washington University.  All Rights Reserved.
;load_mask.pro  $Revision: 1.4 $
pro load_mask,fi,st,dsp,wd,stc,help,glm,pref
files = [!MASK_FILE_111,!MASK_FILE_222,!MASK_FILE_333,!MASK_FILE_111ASIGMA]
labels = ['Standard Avi 111 mask','Standard Avi 222 mask','Standard Avi 333 mask','Jenny Wu white matter 111 mask']
nlabels = n_elements(labels)
scrap = get_bool_list([labels, 'Exit'],TITLE='Please make selections.')
scrap = scrap.list
if scrap[nlabels] eq 1 then return
index = where(scrap,count)
if count eq 0 then return
for i=0,count-1 do begin
    stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,FILENAME=files[index[i]],/NOLOAD_COLOR)
    dsp_image = update_image(fi,dsp,wd,stc,pref)
end
end
