;Copyright 8/22/02 Washington University.  All Rights Reserved.
;fidl_gauss.pro  $Revision: 1.4 $
pro fidl_gauss,fi,wd,dsp,help,stc,pref
rtn_get_glms=get_glms(pref,fi,help,GET_THIS='imgs/concs')
if rtn_get_glms.msg ne 'OK' then return
goback1:
dims = get_str(1,'FWHM in voxels: ','2',/EXIT)
if dims[0] eq 'EXIT' then return
spawn,!BINEXECUTE+'/fidl_timestr2',timestr
script = 'fidl_gauss'+timestr[0]+'.csh'
openw,lu,script,/GET_LUN
top_of_script,lu
if rtn_get_glms.concselect[0] ne '' then begin
    for i=0,rtn_get_glms.total_nsubjects-1 do printf,lu,'nice +19 $BIN/fidl_gauss -file '+rtn_get_glms.concselect[i] $
        +' -gauss_smoth '+trim(dims[0])
endif else begin
    for i=0,rtn_get_glms.total_nsubjects-1 do printf,lu,'nice +19 $BIN/fidl_gauss -file '+rtn_get_glms.imgselect[i] $
        +' -gauss_smoth '+trim(dims[0])
endelse
close,lu
free_lun,lu
spawn,'chmod +x '+script,result
action = get_button(['execute','return','go back'],TITLE='Please select action.',BASE_TITLE=script)
if action eq 2 then goto,goback1
if action eq 0 then spawn_cover,script,fi,wd,dsp,help,stc
print,'DONE'
end
