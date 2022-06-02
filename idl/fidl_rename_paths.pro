;Copyright 3/2/11 Washington University.  All Rights Reserved.
;fidl_rename_paths.pro $Revision: 1.5 $
pro fidl_rename_paths,fi,dsp,wd,glm,help,stc,pref
directory = getenv('PWD')

goback0:

;scrap=get_glms()
;START140305
scrap=get_glms(/NOTMORETHANONE)

if scrap.msg ne 'OK' then return
total_nsubjects = scrap.total_nsubjects
glmfiles = scrap.glmfiles
concselect = scrap.concselect
evselect = scrap.evselect

;print,'concselect=',concselect,'END'
;print,'evselect=',evselect,'END'

if concselect[0] eq '' or evselect[0] eq '' then begin
    conc = strarr(total_nsubjects)
    ev = strarr(total_nsubjects)
    for i=0,total_nsubjects-1 do begin
        ifh = read_mri_ifh(glmfiles[i])
        if ifh.glm_rev gt -17 then begin
            stat=dialog_message('Revision number for '+strmid(glmfiles[i],1)+' too old. Update file by resaving.',/ERROR)
            return 
        endif
        conc[i] = ifh.data_file 
        ev[i] = *ifh.glm_event_file 
        ;print,'*ifh.glm_event_file=',*ifh.glm_event_file
        ;print,'ifh.glm_event_file=',ifh.glm_event_file
    endfor
    if concselect[0] eq '' then concselect=conc
    if evselect[0] eq '' then evselect=ev
endif

goback1:
concselect = get_str(total_nsubjects,glmfiles,concselect,TITLE='Please check concs.',/ONE_COLUMN,/BELOW,/GO_BACK, $
    FRONT=directory+'/',/REPLACE)
if concselect[0] eq 'GO_BACK' then goto,goback0

goback2:
evselect = get_str(total_nsubjects,glmfiles,evselect,TITLE='Please check event files.',/ONE_COLUMN,/BELOW,/GO_BACK, $
    FRONT=directory+'/',/REPLACE)
if evselect[0] eq 'GO_BACK' then goto,goback1

goback3:
scrap = ['script']
scraplabels = ['fidl_rename_paths.csh']
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=50,TITLE='Please enter desired names.',/GO_BACK)
if names[0] eq 'GO_BACK' then goto,goback2
csh = fix_script_name(names[0])

openw,lu,csh,/GET_LUN
top_of_script,lu
for i=0,total_nsubjects-1 do begin
    ;printf,lu,'nice +19 $BIN/fidl_rename_paths -glm_files '+glmfiles[i]+' -conc '+concselect[i]+' -ev '+evselect[i]
    cmd = 'nice +19 $BIN/fidl_rename_paths -glm_files '+glmfiles[i]
    if concselect[i] ne '' then cmd = cmd + ' -conc '+concselect[i]
    if evselect[i] ne '' then cmd = cmd + ' -ev '+evselect[i]
    printf,lu,cmd
endfor
close,lu
free_lun,lu
spawn,'chmod +x '+csh

action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 2 then goto,goback3 
if action eq 0 then begin

    ;spawn,csh+'>'+csh+'.log &'
    ;START130307
    spawn,'csh '+csh+'>'+csh+'.log &'

    stats = dialog_message(csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
endif
print,'DONE'
end
