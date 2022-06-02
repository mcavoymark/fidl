;Copyright 11/6/99 Washington University.  All Rights Reserved.
;load_atlas.pro  $Revision: 1.6 $

;********************************************
pro load_atlas,fi,st,dsp,wd,stc,help,glm,pref
;********************************************

get_atlas_labels,labels,files,natlas
scrap = get_bool_list([labels, 'Exit'],TITLE='Please make selections.')
scrap = scrap.list
if scrap[natlas] eq 1 then return
index = where(scrap,count)
if count eq 0 then return
for i=0,count-1 do begin
    stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,FILENAME=files[index[i]],/NOLOAD_COLOR)
    dsp_image = update_image(fi,dsp,wd,stc,pref)
end

end
