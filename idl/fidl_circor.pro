;Copyright 2/24/06 Washington University.  All Rights Reserved.
;fidl_circor.pro $Revision: 1.3 $

;**************************************
pro fidl_circor,fi,wd,dsp,help,stc,pref
;**************************************
spawn,'whoami',whoami
if whoami ne 'mcavoy' then begin
    stat=dialog_message('Procedure aborted due to error.'+string(10B)+'Null pointer to vector X.',/ERROR)
    return
endif
nindroots = 0
indrootsstr = ''
goback0:
directory = getenv('PWD')
subject_id = string(indgen(1000)+1)
goback1:
Ls = get_list_of_files('*.4dfp.img',directory,'Please select phase 1.',subject_id)
if Ls[0] eq '' then return
goback2:
Rs = get_list_of_files('*.4dfp.img',directory,'Please select phase 2.',subject_id)
if Rs[0] eq '' then return
nLs = n_elements(Ls)
nRs = n_elements(Rs)
if nLs ne nRs then begin
    stat=dialog_message('You have selected '+strtrim(nLs,2)+' and '+strtrim(nRs,2)+'.  Unequal. Abort!',/ERROR)
    return
endif
print,'Ls'
print,Ls
print,'Rs'
print,Rs
goback3:
ind_yesno = get_button(['yes','no','go back','exit'],TITLE='Output individual ratios?')
if ind_yesno eq 2 then goto,goback2 else if ind_yesno eq 3 then return
if ind_yesno eq 0 then begin
    goose = strpos(Ls,'/',/REVERSE_SEARCH)+1
    indroots = strarr(nLs)
    for i=0,nLs-1 do indroots[i] = strmid(Ls[i],goose[i])
    goose = strpos(indroots,'_')
    for i=0,nLs-1 do indroots[i] = strmid(indroots[i],0,goose[i])
    ;print,'indroots=',indroots
    nindroots = nLs
endif

goback4:
dummy = 2 + nindroots
scrap = strarr(dummy)
scraplabels = strarr(dummy)
scrap[0] = 'script'
scrap[1] = 'output - average of the ratios'
scraplabels[0] = 'fidl_circor.csh'
scraplabels[1] = 'LvsR.4dfp.img'
if nindroots gt 0 then begin
    junk = 'individual root ' + strtrim(indgen(nLs),2)
    scrap[2:dummy-1] = junk
    scraplabels[2:dummy-1] = indroots 
endif
for i=0,dummy-2 do print,scrap[i],' ',scraplabels[i]
names = get_str(dummy,scrap,scraplabels,WIDTH=50,TITLE='Please enter names.',/GO_BACK,/FRONT)
if names[0] eq 'GO_BACK' then goto,goback3
csh = fix_script_name(names[0])
outstr = ' -out "' + names[1] + '"'
if nindroots gt 0 then indrootsstr = ' -indroots '+strjoin(names[2:*],' ',/SINGLE)

openw,lu_csh,csh,/GET_LUN
top_of_script,lu_csh
print_files_to_csh,lu_csh,nLs,Ls,'L','l'
print_files_to_csh,lu_csh,nRs,Rs,'R','r'
printf,lu_csh,'nice +19 $BIN/fidl_circor $L $R'+indrootsstr
close,lu_csh
free_lun,lu_csh
spawn,'chmod +x '+csh,result

action = get_button(['execute','return','go back'],TITLE='Please select action.'+string(10B) $
    +string(10B)+'To run on linux, please select return. Login to a linux machine and enter the script on the command line.', $
    BASE_TITLE=csh)
if action eq 2 then goto,goback4
if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc

print,'DONE'
end
