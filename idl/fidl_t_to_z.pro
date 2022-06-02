;Copyright 10/15/07 Washington University.  All Rights Reserved.
;fidl_t_to_z.pro  $Revision: 1.5 $
pro fidl_t_to_z,fi,dsp,wd,stc,help,pref
directory=getenv('PWD')
img_text = get_button(['4dfp','Text','Exit'],TITLE='What type of data?')
if img_text eq 2 then return
if img_text eq 0 then begin
    goback0:
    if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx,'T statistic file',TITLE='Please select 4dfp.') eq !ERROR then return
    if(idx lt fi.nfiles) and (fi.whereisit[idx] eq !INTERNAL) then begin
        filename = get_filename_from_index(fi,idx)
        stat = write_4dfp(filename,fi,wd,dsp,help,stc,hdr.tdim,INDEX=idx)
    endif else begin 
        filename = fi.names[idx]
    endelse
    if hdr.ifh.dof_condition eq 0 then begin
        scrap = get_str(1,'Please enter the degrees of freedom','0',LABEL='The ifh lacks the degrees of freedom.',$
            TITLE='Degrees of freedom',/GO_BACK)
        if scrap[0] eq 'GO_BACK' then goto,goback0 
        df = fix(scrap[0])
        if df eq 0 then begin
            stat=dialog_message('Zero degrees of freedom. Program aborted!',/ERROR)
            return
        endif
        hdr.ifh.dof_condition = df
        write_mri_ifh,fi.paths[idx]+fi.tails[idx]+'.4dfp.ifh',hdr.ifh
        *fi.hdr_ptr[idx] = hdr
    endif
    csh = 'fidl_t_to_z.csh'
    openw,lu,csh,/GET_LUN
    top_of_script,lu
    printf,lu,'nice +19 $BIN'+'/fidl_t_to_z -t '+filename
    close,lu
    free_lun,lu
    spawn,'chmod +x '+csh
    spawn_cover,csh,fi,wd,dsp,help,stc
end else begin
    get_dialog_pickfile,'*.txt',directory,'Please select text file.',filename,rtn_nfiles,rtn_path
    if filename eq 'GOBACK' or filename eq 'EXIT' then return
    csh = 'fidl_t_to_z.csh'
    openw,lu,csh,/GET_LUN
    top_of_script,lu
    printf,lu,'nice +19 $BIN'+'/fidl_t_to_z -t '+filename
    close,lu
    free_lun,lu
    spawn,'chmod +x '+csh
    spawn,csh,result
    stat=dialog_message(result,/INFORMATION)
endelse
print,'DONE'
end
