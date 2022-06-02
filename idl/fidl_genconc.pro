;Copyright 12/29/05 Washington University.  All Rights Reserved.
;fidl_genconc.pro  $Revision: 1.10 $
function fidl_genconc,fi,dsp,wd,glm,help,stc,pref,glmfile,concfile
directory = getenv('PWD')
betastr = ''
if n_elements(glmfile) eq 0 then from_linmod = 0 else from_linmod = 1 
if from_linmod eq 0 then begin
    goback0:
    if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,glm_idx,'GLM',/GET_GLMS) ne !OK then return,'GO_BACK'
    glmfile = fi.names[glm_idx]
    stat=load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,/NOLOAD_COLOR,FILENAME=hdr.ifh.data_file)
    goback1:
    if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,conc_idx,'conc',/GET_CONC,FILTER='*.conc',TITLE='Please load conc') ne !OK $
        then return,'GO_BACK'
    concfile = fi.names[conc_idx]
    goback2:
    scrap = get_button(['beta file','design matrix','go back','exit'],TITLE='Beta file'+string(10B)+'---------'+string(10B) $
        +'Each line is an event type and its timecourse.'+string(10B)+'Event type must match that in the glm.'+string(10B) $
        +string(10B)+'Design matrix'+string(10B)+'-------------'+string(10B) $
        +'Sum over event types frame by frame in the design matrix.'+string(10B) $
        +'In the glm, all event types should be assumed responses.'+string(10B),BASE_TITLE='Please select')
    if scrap eq 2 then goto,goback1 else if scrap eq 3 then return,'GO_BACK'
    if scrap eq 0 then begin
        get_dialog_pickfile,'*.dat',directory,'Please select beta file.',beta_file
        if beta_file eq 'GOBACK' then goto,goback2 else if beta_file eq 'EXIT' then return,'GO_BACK'
        rtn_gr = get_root(beta_file,strmid(beta_file,strpos(beta_file,'.',/REVERSE_SEARCH)))
        root = rtn_gr.file 
        betastr = ' -beta ' + beta_file
    endif else begin 
        rtn_gr = get_root(glmfile,'.glm')
        root = rtn_gr.file 
    endelse
    out = directory + '/' + root +'.conc'
    goback3:
    names = get_str(2,['script','conc'],['fidl_genconc.csh',out],WIDTH=50,TITLE='Please enter names.',/GO_BACK)
    if names[0] eq 'GO_BACK' then goto,goback2
    csh = fix_script_name(names[0])
    out = names[1]
    action = get_button(['execute','return','go back'],TITLE='Please select action.'+string(10B) $
        +string(10B)+'To run on linux, please select return. Login to a linux machine and enter the script on the command line.', $
        BASE_TITLE=csh)
    if action eq 2 then goto,goback3
endif else begin

    ;root = get_root(concfile,'.')
    ;START64
    rtn_gr = get_root(concfile,'.')
    root = rtn_gr.file 

    out = directory + '/' + root +'.conc'
    names = get_str(1,'conc',out,WIDTH=50,TITLE='Please enter names.',/GO_BACK)
    if names[0] eq 'GO_BACK' then return,'GO_BACK'
    csh = 'fidl_genconc.csh'
    out = names[0]
    action = 0
endelse
glmstr = ' -glm ' + glmfile
concstr = ' -conc ' + concfile
outstr = ' -out "' + out + '"'

openw,lu,csh,/GET_LUN
top_of_script,lu
printf,lu,'nice +19 $BIN/fidl_genconc'+glmstr+concstr+betastr+outstr
close,lu
free_lun,lu
spawn,'chmod +x '+csh
if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc
;if from_linmod eq 1 then begin
;    cmd = 'rm -f scratch.glm fidl_genconc.csh'
;    spawn,cmd,result
;endif
print,'DONE'
return,'OK'
end
