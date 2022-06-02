;Copyright 7/26/01 Washington University.  All Rights Reserved.
;fidl_fix_region_file.pro  $Revision: 1.11 $
pro fidl_fix_region_file,fi,help,pref
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='This program corrects FIDL region files that have the ' $
    +'first region defined as 1 instead of 2.'
scrap = get_button(['ok','exit'],TITLE='This program corrects files that have the first region ' $
    +'defined as 1 instead of 2.'+string(10B)+'It will also create ifh tags for the regions.')
if scrap eq 1 then return
goback0:
gg=get_glms(pref,fi,help,GET_THIS='imgs')
if gg.msg ne 'OK' then return
gr=get_root(gg.imgselect)
goback1:
gr=get_root(gg.imgselect)
regname=get_str(1,'region name',gr.file[0],WIDTH=60,LABEL='Please enter region name',/GO_BACK)
if regname[0] eq 'GO_BACK' then goto,goback0
goback2:
root=strarr(gg.total_nsubjects)
out=strarr(gg.total_nsubjects)
for i=0,gg.total_nsubjects-1 do begin
    scrap=strsplit(gg.imgselect[i],'/',/EXTRACT)
    root[i]=scrap[6]
    out[i]=root[i]+'_'+gr.file[i]
endfor 
names=get_str(1+gg.total_nsubjects,['script',root],['fidl_fix_region_file.csh',out],WIDTH=60, $
    TITLE='Please enter desired filenames.',LABEL='You may retain the same names if you so desire.',/GO_BACK,/REPLACE,/ONE_COLUMN)
if names[0] eq 'GO_BACK' then goto,goback1 
script=fix_script_name(names[0])
openw,lu,script,/GET_LUN
top_of_script,lu
for i=0,gg.total_nsubjects-1 do printf,lu,'$BIN/fidl_fix_region_file -file '+gg.imgselect[i]+' -out '+names[i+1] $
    +' -regname '+regname[0]
close,lu
free_lun,lu
print,'Script written to '+script
spawn,'chmod +x '+script
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=fix_region_file_csh)
if action eq 2 then goto,goback2
if action eq 0 then begin
    spawn,script+'>'+script+'.log'+' &'
    scrap=get_button('Ok',BASE_TITLE=script,TITLE='Script has been executed.')
endif
print,'DONE'
end
