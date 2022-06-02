;Copyright 9/19/12 Washington University.  All Rights Reserved.
;fidl_randskew.pro  $Revision: 1.2 $
pro fidl_randskew,fi,dsp,wd,help,stc
goback0:
spawn,'fidl_timestr',timestr
csh = 'fidl_randskew_'+timestr[0]+'.csh'
scrap = ['0','0','1','randskew','0','1','0 0 0','0',csh]
goback1:
scrap = get_str(n_elements(scrap),['alpha (shape parameter)','location','scale','output root','first output file no.', $
    'no. of images','seed (3 numbers)','gaussian smoothing (fwhm in voxels)','script'],scrap,WIDTH=100, $
    TITLE='Please enter.',LABEL='alpha=0 location=0 scale=1 produces standard normal deviates',/EXIT,SPECIAL='Reset')
if scrap[0] eq 'EXIT' then return else if scrap[0] eq 'SPECIAL' then goto,goback0
scrap = strtrim(scrap,2)
paramstr = ' -alpha '+scrap[0]+' -location '+scrap[1]+' -scale '+scrap[2]+' -root '+scrap[3]+' -start '+scrap[4]+' -images ' $
    +scrap[5]+' -seed '+scrap[6]+' -gauss_smoth '+scrap[7] 
csh = fix_script_name(scrap[n_elements(scrap)-1])
goback2:
repeat begin
    scrap1  = get_button(['Standard Avi 111 mask','Standard Avi 222 mask','Standard Avi 333 mask','On disk','Go back','Exit'], $
        BASE_TITLE='Please select mask',TITLE='A mask is required as it provides image dimensions.')
    if scrap1 eq 0 then $
        mask = !MASK_FILE_111 $
    else if scrap1 eq 1 then $
        mask = !MASK_FILE_222 $
    else if scrap1 eq 2 then $
        mask = !MASK_FILE_333 $
    else if scrap1 eq 3 then $ 
        get_dialog_pickfile,'*img',fi.path,'Please select mask',mask $
    else if scrap1 eq 4 then $ 
        goto,goback1 $
    else if scrap1 eq 5 then $ 
        return
endrep until mask ne ''
openw,lu,csh,/GET_LUN
top_of_script,lu
printf,lu,'nice +19 $BIN/fidl_randskew '+paramstr+' -mask '+mask 
close,lu
free_lun,lu
spawn,'chmod +x '+csh
action = get_button(['Execute','Return','Go back'],BASE_TITLE='Please select.',TITLE=csh)
if action eq 2 then goto,goback2
if action eq 0 then begin
    spawn,'csh '+csh+' &'
    stat=dialog_message(csh+' has been executed.',/INFORMATION)
endif
print,'DONE'
end
