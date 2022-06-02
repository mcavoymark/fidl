;Copyright 12/31/99 Washington University.  All Rights Reserved.
;export_dsgn_matrix.pro  $Revision: 12.99 $
pro export_dsgn_matrix,fi,dsp,wd,glm,help,stc
goback0:
lcload=0
if fi.nfiles eq 0 then begin 
    get_dialog_pickfile,fi.glm_filter,fi.path,'Please select GLMs.',rtn_filenames,rtn_nfiles,rtn_path,/MULTIPLE_FILES,/DONT_ASK
    if rtn_nfiles eq 0 then return 
    nfiles = rtn_nfiles
    lcload=1
    rtn1=get_root(rtn_filenames,'.glm')
    filename = rtn1.file
endif else begin
    nfiles=1
    filename = fi.list[fi.n]
    result = strpos(filename,'.glm',/REVERSE_SEARCH)
    if result lt 0 then result = strpos(filename,'glm',/REVERSE_SEARCH)
    if result gt 0 then filename = strmid(filename,0,result)
endelse   
filename = get_str(nfiles,make_array(nfiles,/STRING,VALUE='Output'),filename+'_dsgn_matrix.txt', $
    TITLE='Design matrix will be written to a text file.',/ONE_COLUMN,FRONT='vc',BACK='vc',/REPLACE,/GO_BACK)
if filename[0] eq 'GO_BACK' then goto,goback0
scrap = strarr(nfiles)
for l=0,nfiles-1 do begin
    if lcload eq 1 then begin
        rtn=load_linmod(fi,dsp,wd,glm,help,FILENAME=rtn_filenames[l],/GET_CNORM)
        A = rtn.A
        effect_label = *rtn.ifh.glm_effect_label
        effect_length = *rtn.ifh.glm_effect_length
        all_eff = rtn.ifh.glm_all_eff
    endif else begin
        hdr = *fi.hdr_ptr[fi.n]
        if hdr.array_type ne !LINEAR_MODEL then begin
            stat = dialog_message('Image not a glm.')
            return
        endif
        A = *glm[fi.n].A
        effect_label = *glm[fi.n].effect_label
        effect_length = *glm[fi.n].effect_length
        all_eff = glm[fi.n].all_eff
    endelse
    AT = transpose(A)
    xdim = n_elements(AT[*,0])
    ydim = n_elements(AT[0,*])
    effect_table = intarr(xdim)
    k = 0
    for i=0,all_eff-1 do begin
        for j=0,effect_length[i]-1 do begin
            effect_table[k] = i
            k = k + 1
        endfor
    endfor
    openw,lu,filename[l],/GET_LUN

    ;firstline = ''
    ;for x=0,xdim-1 do firstline=firstline+string(effect_label[effect_table[x]],FORMAT='(" ",a8,$)')
    ;firstline='%'+firstline
    ;printf,lu,firstline
    ;START141002
    printf,lu,'%'+strjoin(effect_label[effect_table[indgen(xdim)]],string(9B),/SINGLE)
 
    widget_control,/HOURGLASS

    ;for y=0,ydim-1 do begin
    ;    for x=0,xdim-1 do printf,lu,AT[x,y],FORMAT='(" ",f8.4,$)'
    ;    printf,lu,''
    ;endfor
    ;START141002
    for y=0,ydim-1 do printf,lu,strjoin(trim(AT[indgen(xdim),y]),string(9B),/SINGLE)
        

    close,lu
    free_lun,lu
    scrap[l] = 'Design matrix written to ' + filename[l]
    print,scrap[l]
endfor
stat = dialog_message(scrap,/INFORMATION)
print,'DONE'
end
