;Copyright 1/18/12 Washington University.  All Rights Reserved.
;fidl_levene.pro  $Revision: 1.5 $
pro fidl_levene,glm,dsp,fi,pref,wd,help,stc
directory = getenv('PWD')
goback0:
scrap=get_glms(pref,/GET_CONC)
if scrap.msg eq 'GO_BACK' then return else if scrap.msg eq 'EXIT' then return
total_nsubjects = scrap.total_nsubjects
t4select = scrap.t4select
concselect = scrap.concselect
evselect = scrap.evselect
txtselect = scrap.txtselect

;KEEP
;if txtselect[0] eq '' then begin
;    scrap=get_button(['ok','go back'],BASE_TITLE='Information',TITLE='All frames will be used.'+string(10B)+'To exclude frames ' $
;        +'use a text file that denotes the valid frames.'+string(10B)+'1 valid, 0 ignore. Single column. Extant matches ' $
;        +'the length of the conc. One for each conc.')
;    if scrap eq 1 then goto,goback0
;    txtselect = make_array(total_nsubjects,/STRING,VALUE='')
;endif else $
;    txtselect = '-validframes '+txtselect
scrap=get_button(['ok','exit'],BASE_TITLE='Information',TITLE='All frames will be used.'+string(10B)+'To exclude frames see McAvoy.')
if scrap eq 1 then return

stc2 = replicate({Stitch},total_nsubjects)
widget_control,/HOURGLASS
for m=0,total_nsubjects-1 do begin
    print,'Loading ',concselect[m]
    rload_conc=load_conc(fi,stc,dsp,help,wd,concselect[m],/DONT_PUT_IMAGE)
    if rload_conc.msg eq 'GOBACK' then goto,goback0 else if rload_conc.msg eq 'EXIT' then return
    stc2[m] = rload_conc.stc1
endfor
nrun = stc2[*].n
idx = where(nrun lt 2,cnt)
if cnt ne 0 then begin
    scrap=get_button(['ok','go back'],BASE_TITLE='Information',TITLE='The following concs will not be analyzed because they ' $
        +'contain less than 2 runs.'+string(10B)+string(10B)+strjoin(concselect[idx],string(10B),/SINGLE))
    if scrap eq 1 then goto,goback0
    if cnt eq total_nsubjects then return
    idx = where(nrun ge 2,total_nsubjects)
    concselect = concselect[idx]
    stc2 = stc[idx]
endif 
goback1:
if strmid(concselect[0],strlen(concselect[0])-3) eq 'img' then lc_concs = 0 else lc_concs = 1
if lc_concs eq 1 then ext='.' else ext='.4dfp.img'
rtn_gr = get_root(concselect,ext,/KEEPPATH)
scrap = ['script','sub '+strtrim(indgen(total_nsubjects)+1,2)]
scraplabels = ['fidl_levene.csh',rtn_gr.file+'_levene.4dfp.img']
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter filenames.',/ONE_COLUMN,/GO_BACK, $
    FRONT=directory+'/',/REPLACE)
if names[0] eq 'GO_BACK' then goto,goback0 
names = fix_script_name(names)
csh = names[0]
out = ' -out ' + names[1:*]
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 2 then goto,goback1
openw,lu,csh,/GET_LUN
top_of_script,lu
for i=0,total_nsubjects-1 do begin
    print_files_to_csh,lu,stc2[i].n,*stc2[i].filnam,'FILES','files',/NO_NEWLINE
    printf,lu,'nice +19 $BIN/fidl_levene $FILES'+out[i]
    printf,lu,'#'+concselect[i]
endfor
close,lu
free_lun,lu
print,'Script written to ',csh
spawn,'chmod +x '+ csh
if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc
print,'DONE'
end
