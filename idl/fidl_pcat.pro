;Copyright 2/7/17 Washington University.  All Rights Reserved.
;fidl_pcat.pro  $Revision: 1.2 $
pro fidl_pcat,fi,dsp,wd,glm,help,stc,pref
directory=getenv('PWD')
spawn,!BINEXECUTE+'/fidl_timestr2',timestr
goback0:
;title='You first define the temporal components or features. Think of it in terms of a single factor with a certain number of ' $
;    +'levels.'+string(10B)+'Next a file is assigned to each feature.'+string(10B)+'Finally, you provide a mask that identifies ' $
title='You first define the temporal components or features.' $
    +string(10B)+'Next a file is assigned to each feature.'+string(10B)+'Finally, you provide a mask that identifies ' $
    +'the voxels.'+string(10B)+'Thus each voxel is an obsevation and the number of components equals the number of features.'
scrap = get_button(['Ok','Exit'],BASE_TITLE='Temporal principal component analysis',TITLE=title,/ALIGN_LEFT)
if scrap eq 1 then return
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE=title
goback1:
scrap = get_str(1,'number of features','2',WIDTH=40,TITLE='How many features?',/FRONT,/GO_BACK,/EXIT)
if scrap[0] eq 'GO_BACK' then goto,goback0 else if scrap[0] eq 'EXIT' then return
ntc = fix(scrap[0])
treatment_str = 'feature' + trim(indgen(ntc)+1)
levelnames=treatment_str
goback2:
scrap = get_str(ntc,treatment_str,levelnames,WIDTH=40,TITLE='Please name features',/FRONT,/GO_BACK)
if scrap[0] eq 'GO_BACK' then goto,goback1 else if scrap[0] eq 'EXIT' then return
levelnames=scrap
goback3:
widget_control,/HOURGLASS
gg=get_glms(pref,GET_THIS='imgs',FILTER='*.img',LEVELNAMES=levelnames,HOWMANY=1)
if gg.msg eq 'GO_BACK' then goto,goback2 else if gg.msg eq 'EXIT' then return

print,'gg.glm_space=',gg.glm_space
print,'gg.bold_space=',gg.bold_space

goback4:
mask=get_mask(gg.bold_space[0],fi)
if mask eq 'GO_BACK' then goto,goback3
goback5:
names=get_str(2,['script','out'],['fidl_pcat'+timestr[0]+'.csh','fidl_pcat'+timestr[0]+'.txt'],WIDTH=100,/ONE_COLUMN,/GO_BACK, $
    /EXIT,TITLE='Please enter names.',FRONT=directory+'/',/REPLACE)
if names[0] eq 'EXIT' then return else if names[0] eq 'GO_BACK' then goto,goback4 
names=fix_script_name(names)
script=names[0] & out=names[1]
action = get_button(['execute','return','go back','exit'],TITLE='Please select action.',BASE_TITLE=script)
if action eq 2 then goto,goback5 else if action eq 3 then return  
openw,lu,script,/GET_LUN
top_of_script,lu
print_files_to_csh,lu,ntc,gg.imgselect,'FILES','files',/NO_NEWLINE
printf,lu,'nice +19 $BIN/fidl_pcat $FILES -features '+strjoin(trim(levelnames),' ',/SINGLE)+' -mask '+mask+' -out "'+out+'"'
close,lu
free_lun,lu
spawn,'chmod +x '+script
if action eq 0 then begin
    spawn,script+'>'+script+'.log'+' &'
    scrap=dialog_message(script+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
