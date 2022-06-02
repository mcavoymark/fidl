;Copyright 12/01/30 Washington University.  All Rights Reserved.
;fidl_collatecol.pro $Revision: 1.2 $
pro fidl_collatecol,fi,dsp,wd,glm,help,stc,pref
scrap = get_button(['continue','exit'],BASE_TITLE='Information',TITLE='Load conc(s). Each conc contains a list of text files.' $
    +string(10B)+'Each text file contains a column(s) of numbers.'+string(10B)+'Output is a single text file for each conc ' $
    +'that includes all columns.')
if scrap eq 1 then return
goback0:
scrap=get_glms(GET_THIS='conc(s)',FILTER='*.conc')
if scrap.msg ne 'OK' then return
concselect = scrap.concselect
total_nsubjects = scrap.total_nsubjects
goback1:
scrap = ['script','out '+strtrim(indgen(total_nsubjects)+1,2)]
rtn_gr = get_root(concselect,'.conc')
scraplabels = ['fidl_collatecol.csh',rtn_gr.file+'.dat']
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter filenames.',/ONE_COLUMN,/GO_BACK,/REPLACE)
if names[0] eq 'GO_BACK' then goto,goback0 
names = fix_script_name(names)
csh = names[0]
out = names[1:*]
openw,lu,csh,/GET_LUN
top_of_script,lu
for i=0,total_nsubjects-1 do printf,lu,'nice +19 $BIN/fidl_collatecol -file '+concselect[i]+' -out '+out[i]
close,lu
free_lun,lu
spawn,'chmod +x '+csh
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 2 then goto,goback1
if action eq 0 then begin
    spawn,csh+'>'+csh+'.log &'
    stats = dialog_message(csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
