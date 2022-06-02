;Copyright 12/14/05 Washington University.  All Rights Reserved.
;fidl_lminusrdivlplusr.pro $Revision: 1.5 $

;*******************************************
pro fidl_lminusrdivlplusr,fi,wd,dsp,help,stc
;*******************************************
nindroots = 0
indrootsstr = ''
goback0:
directory = getenv('PWD')
subject_id = string(indgen(1000)+1)

goback1:
filterL = '*.4dfp.img'
Ls = get_list_of_files(filterL,directory,'Please select Ls.',subject_id,ORDERED_TITLE='Please select Ls.',/BELOW)
if Ls[0] eq '' then return
print,'before filterL=',filterL

;filterL = fix_script_name(get_root(filterL,'.4dfp.img'),/ALL)
;START64
rtn_gr = get_root(filterL,'.4dfp.img')
root = rtn_gr.file
filterL = fix_script_name(root,/ALL)

goback2:
filterR = '*.4dfp.img'
Rs = get_list_of_files(filterR,directory,'Please select Rs.',subject_id,ORDERED_TITLE='Please select Rs.',/BELOW)
if Rs[0] eq '' then return
print,'before filterR=',filterR


;filterR = fix_script_name(get_root(filterR,'.4dfp.img'),/ALL)
;START64
root = get_root(filterR,'.4dfp.img')
filterR = fix_script_name(root,/ALL)





nLs = n_elements(Ls)
nRs = n_elements(Rs)
if nLs ne nRs then begin
    stat=dialog_message('You have selected '+strtrim(nLs,2)+' and '+strtrim(nRs,2)+'.  Unequal. Abort!',/ERROR)
    return
endif
;print,'Ls'
;print,Ls
;print,'Rs'
;print,Rs
goback3:
ind_yesno = get_button(['yes','no','go back','exit'],TITLE='Output individual ratios?')
if ind_yesno eq 2 then goto,goback2 else if ind_yesno eq 3 then return



;if ind_yesno eq 0 then begin
;    indroots = get_root(Ls,'.4dfp.img') + '_vs_' + get_root(Rs,'.4dfp.img')
;    nindroots = nLs
;endif
;START64
if ind_yesno eq 0 then begin
    rtn_gr = get_root(Ls,'.4dfp.img')
    Lsroot = rtn_gr.file
    rtn_gr = get_root(Rs,'.4dfp.img')
    Rsroot = rtn_gr.file
    indroots = Lsroot + '_vs_' + Rsroot
    nindroots = nLs
endif





goback4:
dummy = 2 + nindroots
scrap = strarr(dummy)
scraplabels = strarr(dummy)
scrap[0] = 'script'
scrap[1] = 'output - average of the ratios'
scraplabels[0] = 'fidl_lvsr.csh'

;scraplabels[1] = 'LvsR.4dfp.img'
scraplabels[1] = filterL+'vs'+filterR+'.4dfp.img'

if nindroots gt 0 then begin
    junk = 'individual root ' + strtrim(indgen(nLs)+1,2)
    scrap[2:dummy-1] = junk
    scraplabels[2:dummy-1] = indroots 
endif
;for i=0,dummy-2 do print,scrap[i],' ',scraplabels[i]
;names = get_str(dummy,scrap,scraplabels,WIDTH=50,TITLE='Please enter names.',/GO_BACK,/FRONT)
names = get_str(dummy,scrap,scraplabels,WIDTH=50,TITLE='Please enter names.',/GO_BACK,FRONT=getenv('PWD'))
if names[0] eq 'GO_BACK' then goto,goback3
csh = fix_script_name(names[0])
outstr = ' -out "' + names[1] + '"'
if nindroots gt 0 then indrootsstr = ' -indroots '+strjoin(names[2:*],' ',/SINGLE)

openw,lu_csh,csh,/GET_LUN
top_of_script,lu_csh
print_files_to_csh,lu_csh,nLs,Ls,'L','l'
print_files_to_csh,lu_csh,nRs,Rs,'R','r'
printf,lu_csh,'nice +19 $BIN/fidl_lminusrdivlplusr $L $R'+outstr+indrootsstr
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
