;Copyright 9/21/00 Washington University.  All Rights Reserved.
;convert_af3d_to_fidl.pro  $Revision: 1.21 $
pro convert_af3d_to_fidl,fi,wd,dsp,help,stc
nameregionsstr=''
spheresstr=''
logstr = ''
regnamelabel = ''
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='An af3d file or a simple text file of atlas coordinates is ' $
    +'converted to the fidl region format.'
goback0:
get_dialog_pickfile,'*',fi.path[0],'Please select af3d or text file.',filename,rtn_nfiles,rtn_path
if filename eq 'GOBACK' or filename eq 'EXIT' then return
goback1:
strs = get_str(2,['Threshold: ','Diameter in mm: '],['3','10'],TITLE='Spheres of specified diameter will be drawn at each point.', $
    LABEL='If your file contains only points, then ignore the threshold setting.',/GO_BACK)
if strs[0] eq 'GO_BACK' then goto,goback0
strs = strtrim(strs,2)
goback2:
space = get_button(['111','222','333','load ifh','go back'],TITLE='What type of fidl region file do you wish to create?')
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
    goto,goback1

goback20:
nameregions = get_button(['file coordinates','center of mass','go back'],TITLE='Name regions')
if nameregions eq 2 then goto,goback2
if nameregions eq 1 then nameregionsstr=' -center_of_mass'

goback25:
spheres = get_button(['Old way - rounds to integers right away','New way - rounds to integers at the very end','go back'], $
    TITLE='How do you want the spheres drawn?',BASE_TITLE='Please select')
if spheres eq 2 then goto,goback20
if spheres eq 1 then spheresstr=' -new'

goback3:
outtype = get_button(['Single file','Separate file for each point','go back'],TITLE='What type of output?')
if outtype eq 2 then goto,goback25 
goback4:
if outtype eq 0 then begin
    outtypestr = ''

    ;START110713
    ;dummy = 3

    scraplabel = ['script','output','log']
    rtn_gr = get_root(filename,'.')
    root = rtn_gr.file
    scrap = ['fidl_af3d.csh',root+'_D'+strs[1]+'mm_t'+strs[0]+'_reg.4dfp.img','fidl_af3d.csh.log']
endif else begin
    outtypestr = ' -separate'

    ;START110713
    ;dummy = 2

    scraplabel = ['script','log']
    rtn_gr = get_root(filename,'.')
    root = rtn_gr.file
    scrap = ['fidl_af3d.csh',root+'.log']
endelse

;scrap = get_str(dummy,scraplabel,scrap,WIDTH=100,TITLE='Please enter names.',LABEL='Leaving the log blank prevents its output.', $
;    /GO_BACK)
;START110713
scraplabel = [scraplabel,'append this label to region names']
scrap = [scrap,'']
scrap = get_str(n_elements(scraplabel),scraplabel,scrap,WIDTH=100,TITLE='Please enter names.',/GO_BACK, $
    LABEL='Leaving the log blank prevents its output.')


if scrap[0] eq 'GO_BACK' then goto,goback3
csh = fix_script_name(scrap[0])
j = 1
if outtype eq 0 then begin
    outstr = ' -out "'+scrap[1]+'"' 
    j = j + 1
endif else $
    outstr = ''

;if scrap[j] eq '' then logstr = '' else logstr = ' -log ' + scrap[j]
;START110713
if scrap[j] ne '' then logstr = ' -log ' + scrap[j]
j = j+1
if scrap[j] ne '' then regnamelabel = ' -regnamelabel '+scrap[j] 

action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 2 then goto,goback4 
openw,lu,csh,/GET_LUN
top_of_script,lu

;printf,lu,'nice +19 $BIN'+'/convert_af3d_to_fidl -af3d_file '+filename+' -threshold '+strs[0]+' -diameter_in_mm '+strs[1] $
;    +atlas_str+outstr+outtypestr+logstr+nameregionsstr
;START110713
printf,lu,'nice +19 $BIN'+'/convert_af3d_to_fidl -af3d_file '+filename+' -threshold '+strs[0]+' -diameter_in_mm '+strs[1] $
    +atlas_str+outstr+outtypestr+logstr+nameregionsstr+spheresstr+regnamelabel

close,lu
free_lun,lu
spawn,'chmod +x '+csh
if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc
print,'DONE'
end
