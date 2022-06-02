;Copyright 6/14/16 Washington University.  All Rights Reserved.
;fidl_flipglm.pro  $Revision: 1.2 $
pro fidl_flipglm,fi,help,pref
directory=getenv('PWD')
lizard=get_button(['continue','exit'],BASE_TITLE='GLM and image flipper',TITLE='What was on the right is now on the left.')
if lizard eq 1 then return
goback0:
gg=get_glms(pref,fi,help,GET_THIS='GLMs/4dfps/*nii/*nii.gz')
if gg.msg eq 'EXIT' then return
if gg.glmfiles[0] ne '' then begin
    files = gg.glmfiles
    space = gg.glm_space
    lc_glm=0
endif else begin
    files = gg.imgselect
    space = gg.bold_space
    lc_glm=1
endelse
idx=where(space eq !SPACE_DATA,cnt,COMPLEMENT=idx0,NCOMPLEMENT=cnt0)
if cnt ne 0 then begin
    if cnt0 eq 0 then spider='exit' else spider=['continue','go back','exit']
    lizard=get_button(spider,BASE_TITLE='Unknown atlas',TITLE=strjoin(files[idx],string(10B),/SINGLE)+string(10B) $
        +'These files will not be flipped.')
    if cnt0 eq 0 then return
    if lizard eq 1 then goto,goback0 else if lizard eq 2 then return
endif
goback1:
spawn,!BINEXECUTE+'/fidl_timestr2',timestr
scrap = strarr(cnt0+1)
scraplabels = strarr(cnt0+1)
scrap[0]='script'
scraplabels[0]='fidl_flipglm_'+timestr[0]+'.csh'
if lc_glm eq 0 then ext='.' else ext='.4dfp.img'
rtn_gr=get_root(files[idx0],ext,/KEEPPATH)
scraplabels[1:cnt0] = rtn_gr.file + '_FLIP.glm'
wallace0=''
if cnt0 gt 1 then begin
    wallace0=trim(indgen(cnt0)+1)
    if cnt0 gt 9 then begin
        idx=where(strlen(wallace0) eq 1,cnt)
        if cnt ne 0 then wallace0[idx]='0'+wallace0[idx]
    endif
endif
scrap[1:cnt0]='GLM'+wallace0
names = get_str(cnt0+1,scrap,scraplabels,WIDTH=100,TITLE='Please enter filenames.',/ONE_COLUMN,/GO_BACK,FRONT=directory+'/', $
    /REPLACE,/EXIT)
if names[0] eq 'EXIT' then return else if names[0] eq 'GO_BACK' then goto,goback0 
csh=fix_script_name(names[0])
action=get_button(['execute','return','go back','exit'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 3 then return else if action eq 2 then goto,goback1
openw,lu,csh,/GET_LUN
top_of_script,lu
for i=0,cnt0-1 do printf,lu,'nice +19 $BIN'+'/fidl_flipglm -glm '+files[idx0[i]]+' -out '+names[i+1] 
close,lu
free_lun,lu
spawn,'chmod +x '+csh
if action eq 0 then begin
    spawn,csh+'>'+csh+'.log'+' &'
    lizard=get_button('ok',BASE_TITLE=csh,TITLE='Script has been executed.')
endif
print,'DONE'
end
