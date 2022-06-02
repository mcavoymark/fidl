;Copyright 11/27/02 Washington University.  All Rights Reserved.
;extract_mag.pro  $Revision: 1.8 $
pro extract_mag,fi,dsp,wd,glm,help,stc,pref
delay_str = ''
magnorm_str = ''
stat = get_hdr_model(fi,glm,hdr,model)
if stat eq 'ERROR' then return
if(help.enable eq !TRUE) then begin
    widget_control,help.id,SET_VALUE='This version spawns a C program to extract magnitudes. ' $
    +'All magnitudes are saved to a single file. The file is automatically '$
    +'stored on disk, and the command can be used in a shell script. The C program name is "compute_zstat".'
endif
if strpos(fi.names[model],'.glm') eq -1 then begin
    glm_file = strcompress(fi.names[model],/REMOVE_ALL) + '.glm' 
endif else begin
    glm_file = fi.names[model]
endelse

if hdr.ifh.glm_rev gt -17 then begin
    stat = dialog_message('GLM file revision number too old. Please update by resaving.',/ERROR)
    return
endif

effect_length = *glm[model].effect_length
index = where(effect_length gt 1,count)
if count ne 0 then if get_delays(delay_str) eq 'GOBACK' then return

t4_str = ''
if fi.space[model] eq !SPACE_DATA then begin
    lcatlas = get_button(['yes','no'],TITLE='Do you want to transform to atlas space?')
    if lcatlas eq 0 then begin
        get_dialog_pickfile,'*_anat_ave_to_711-2B_t4',fi.path[0],'Please select transformation matrix.',filnam
        if filnam eq 'GOBACK' or filnam eq 'EXIT' then return
        t4_str = ' -xform_file '+ filnam
    endif
endif 

if(help.enable eq !TRUE) then begin
    widget_control,help.id,SET_VALUE=string('Some find it tedious to set up contrasts in each individual GLM file. ' $
    +'You can instead set up the desired contrasts in a single GLM file and use those contrasts from that file. Note: ' $
    +'each GLM must have the same number of estimates (ie conditions and runs).')
endif
lccontrast_glm = get_button(['yes','no'],TITLE='Do you want to specify a different glm file that contains the contrasts?')
if lccontrast_glm eq 0 then begin
    get_dialog_pickfile,'*glm',fi.path[0],'Please select GLM.',filnam
    if filnam eq 'GOBACK' then begin 
        ;do nothing
    endif else if filnam eq 'EXIT' then $
        return $
    else begin
        contrast_glm_str = ' -contrast_glm '+ filnam
        ;rtn = load_linmod(fi,dsp,wd,glm,help,stc,FILENAME=filnam,ifh,/GET_CNORM)
        rtn = load_linmod(fi,dsp,wd,glm,help,FILENAME=filnam,ifh,/GET_CNORM)
        glm_labels = rtn.contrast_labels
        cnorm = rtn.cnorm
    endelse
endif else begin
    contrast_glm_str = ''
    glm_labels = *glm[model].contrast_labels
    cnorm = *glm[model].cnorm
endelse

rtn = select_files(glm_labels,TITLE='Please select contrasts.',MIN_NUM_TO_SELECT=1,/EXIT)
if rtn.files[0] eq 'EXIT' then return 

cstr = string(strcompress(rtn.index+1,/REMOVE_ALL),FORMAT='(" -contrasts ",(100(a," "),:))')

lcmag = ' -mag ONLY'

neff_per_contrast = total(abs(cnorm[*,rtn.index]),1)
index = where(neff_per_contrast gt 1,count)
if count ne 0 then begin
    if dialog_message('Do you want the contrast weights normalized to have a magnitude of 1?',/QUESTION) eq 'Yes' then $
       magnorm_str = ' -magnorm' 
endif 

lcsd = ''

unscaled_mag_str = ''
scrap = get_button(['percent signal change','MR units'],TITLE='I wish the output to be in')
if scrap eq 1 then unscaled_mag_str = ' -unscaled_mag'

dummy = strmid(glm_file,0,strpos(glm_file,'.glm')) + '_mag'
if scrap eq 1 then dummy = dummy + '_unscaled'
dummy = dummy + '.4dfp.img'
dummy = get_str(1,'Output filename ',dummy)
one_file_str = ' -one_file ' + dummy

;command = string(!BIN,glm_file,cstr,lcmag,lcsd,delay_str,t4_str,contrast_glm_str,magnorm_str,unscaled_mag_str,one_file_str, $
;    FORMAT='("nice +19 ",a,"/fidl_zstat -glm_file ",a,a,a,a,a,a,a,a,a,a)')
;START161214
command = string(!BIN,glm_file,cstr,lcmag,lcsd,delay_str,t4_str,contrast_glm_str,magnorm_str,unscaled_mag_str,one_file_str, $
    FORMAT='("nice +19 ",a,"/fidl_zstat2 -glm_file ",a,a,a,a,a,a,a,a,a,a)')


spawn_cover,command,fi,wd,dsp,help,stc
print,'DONE'
print,''
end
