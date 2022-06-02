;Copyright 12/31/99 Washington University.  All Rights Reserved.
;transform_f_to_z.pro  $Revision: 1.9 $

;pro transform_f_to_z,fi,dsp,wd,stc,help
;idx_img = get_button(fi.tails[0:fi.nfiles-1],TITLE='Image to be transformed.')
;hdr = *fi.hdr_ptr[idx_img]
;z_image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
;filename = fi.paths[idx_img]+fi.tails[idx_img]+'.4dfp.img'
;ifh = read_mri_ifh(filename)
;if (ifh.dof_condition eq 0) or (ifh.dof_error eq 0) then begin
;    stat=widget_message('Zero degrees of freedom. Program aborted!')
;    return
;endif
;command = string(filename,FORMAT='("transform_f_to_z -fstat_file ",a)')
;print,command
;widget_control,/HOURGLASS
;spawn,command,result
;nfiles = n_elements(result)
;for i=0,nfiles-1 do begin
;    if strpos(result[i],'Error') ge 0 then begin
;        stat=widget_message(result[i])
;        if(strpos(result[i],'Error: Could not open') ge 0) then $
;            stat=widget_message('4dfp stack needs to be written out to disk before calling binary.')
;        return
;    endif
;endfor
;for i=0,nfiles-1 do begin
;    strings = str_sep(result[i],' ')
;    tmp = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,FILENAME=strings[n_elements(strings)-1],MODEL=model)
;endfor
;widget_control,HOURGLASS=0
;return
;end

pro transform_f_to_z,fi,wd,dsp,help,pref,dsp_image,stc 
goback0:

;if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx,'image') eq !ERROR then return
;START111109
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx,'F stat',TITLE='Please select 4dfp.') eq !ERROR then return
if(idx lt fi.nfiles) and (fi.whereisit[idx] eq !INTERNAL) then begin
    filename = get_filename_from_index(fi,idx)
    stat = write_4dfp(filename,fi,wd,dsp,help,stc,hdr.tdim,INDEX=idx)
endif



;if hdr.ifh.dof_condition eq 0 then begin
;    stat=dialog_message('The numerator degrees of freedom needs to specified as "degrees of freedom condition" in the ifh.',/ERROR)
;    return
;endif
;if hdr.ifh.dof_error eq 0 then begin
;    stat=dialog_message('The denominator degrees of freedom needs to specified as "degrees of freedom error" in the ifh.',/ERROR)
;    return
;endif
;filename = fi.tails[idx] + '.4dfp.img'
;START111109
scrap = get_str(2,['numerator degrees of freedom','denominator degrees of freedom'], $
    strtrim([hdr.ifh.dof_condition,hdr.ifh.dof_error],2),LABEL='Please check degrees of freedom.',TITLE=fi.tails[idx],/GO_BACK)
if scrap[0] eq 'GO_BACK' then goto,goback0
hdr.ifh.dof_condition = float(scrap[0])
hdr.ifh.dof_error = float(scrap[1])
write_mri_ifh,fi.paths[idx]+fi.tails[idx]+'.4dfp.ifh',hdr.ifh
filename = fi.paths[idx]+fi.tails[idx]+'.4dfp.img'
*fi.hdr_ptr[idx] = hdr



csh = 'fidl_f_to_z.csh'
openw,lu,csh,/GET_LUN
top_of_script,lu
printf,lu,'nice +19 $BIN'+'/fidl_f_to_z -fstat '+filename
close,lu
free_lun,lu
spawn,'chmod +x '+csh
spawn_cover,csh,fi,wd,dsp,help,stc
print,'DONE'
print,''
end
