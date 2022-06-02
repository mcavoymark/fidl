;Copyright 9/28/10 Washington University.  All Rights Reserved.
;fidl_labelfile.pro  $Revision: 1.5 $

pro fidl_labelfile,fi,wd,dsp,help,stc
nameregionsstr=''
spheresstr=''
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='A freesurfer label file is converted to a fidl region file.'

goback0:
get_dialog_pickfile,'*.label',fi.path[0],'Please select label file(s).',filename,rtn_nfiles,rtn_path,/MULTIPLE_FILES
if rtn_nfiles eq 0 then return

goback1:
space = get_button(['111','222','333','load ifh','go back'],TITLE='Please select atlas.')
if space eq 0 then $
    atlas_str = ' -atlas 111' $
else if space eq 1 then $
    atlas_str = ' -atlas 222' $
else if space eq 2 then $
    atlas_str = ' -atlas 333' $
else if space eq 3 then begin 
    get_dialog_pickfile,'*ifh',fi.path,'Please load ifh.',file,nfile,rtn_path
    if file eq 'GOBACK' then goto,goback1 else if file eq 'EXIT' then return
    atlas_str = ' -ifh ' + file
endif else $
    goto,goback0



;goback2:
;scraplabel = ['script','output','log']
;rtn_gr = get_root(filename,'.')
;root = rtn_gr.file
;scrap = ['fidl_labelfile.csh',root+'.4dfp.img',root+'.log']
;scrap = get_str(n_elements(scraplabel),scraplabel,scrap,WIDTH=100,TITLE='Please enter names.',/GO_BACK)
;if scrap[0] eq 'GO_BACK' then goto,goback1
;csh = fix_script_name(scrap[0])
;if scrap[1] eq '' then outstr = '' else outstr = ' -out "'+scrap[1]+'"' 
;if scrap[2] eq '' then logstr = '' else logstr = ' -log ' + scrap[2]
;action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=csh)
;if action eq 2 then goto,goback2
;openw,lu,csh,/GET_LUN
;top_of_script,lu
;printf,lu,'nice +19 $BIN'+'/fidl_labelfile -file '+filename+atlas_str+outstr+logstr
;close,lu
;free_lun,lu
;spawn,'chmod +x '+csh
;if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc
;print,'DONE'
;end
;START9
goback2:
rtn_gr = get_root(filename,'.')
root = rtn_gr.file
nroot = n_elements(root)
scraplabel = ['script',make_array(nroot,/STRING,VALUE='output')+' '+strtrim(indgen(nroot)+1,2)]
scrap = ['fidl_labelfile.csh',root+'.4dfp.img']
scrap = get_str(n_elements(scraplabel),scraplabel,scrap,WIDTH=100,TITLE='Please enter names.',/GO_BACK)
if scrap[0] eq 'GO_BACK' then goto,goback1
csh = fix_script_name(scrap[0])
outstr = ' -out "'+scrap[1:*]+'"'
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 2 then goto,goback2 
openw,lu,csh,/GET_LUN
top_of_script,lu
for i=0,nroot-1 do printf,lu,'nice +19 $BIN'+'/fidl_labelfile -file '+filename[i]+atlas_str+outstr[i]
close,lu
free_lun,lu
spawn,'chmod +x '+csh
if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc
print,'DONE'
end
