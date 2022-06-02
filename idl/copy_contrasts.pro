;Copyright 12/31/99 Washington University.  All Rights Reserved.
;copy_contrasts.pro  $Revision: 1.10 $
pro copy_contrasts,fi,dsp,wd,glm,help,stc,pref
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,glm_idx,'file to copy from',/GETGLMS) ne !OK then return
c_in = *glm[glm_idx].c
cnorm_in = *glm[glm_idx].cnorm
nc_in = glm[glm_idx].nc
contrast_labels_in = *glm[glm_idx].contrast_labels
ncol = glm[glm_idx].N
tot_eff = glm[glm_idx].tot_eff
get_dialog_pickfile,fi.glm_filter,fi.path,'Please select one or more files to copy to.',file_names,nfiles,rtn_path, $
    /MULTIPLE_FILES,/DONT_ASK
if nfiles eq 0 then return
scrapall = ''
for ifile=0,nfiles-1 do begin
    print,'Processing ',file_names[ifile]
    rtn = load_linmod(fi,dsp,wd,glm,help,FILENAME=file_names[ifile],/DONT_PUT_IMAGE)
    doit = 1
    if glm[fi.n].tot_eff ne tot_eff then begin
        title = file_names[ifile]+string(10B)+string(10B)+'Mismatch between number of event types in design matrix.' $
            +string(10B)+'Read '+strtrim(glm[fi.n].tot_eff,2)+'.  Expecting '+strtrim(tot_eff,2)+'.'   
        print,title
        stat = get_button(['I know what I am doing. Copy it in.','Skip this one and continue.','Exit.'],BASE_TITLE='WARNING', $
            TITLE=title)
        if stat eq 2 then return else if stat eq 1 then doit=0
    endif
    if doit eq 1 then begin
        ptr_free,glm[fi.n].c
        ptr_free,glm[fi.n].cnorm
        ptr_free,glm[fi.n].contrast_labels
        c = fltarr(glm[fi.n].N,nc_in)
        if glm[fi.n].N ge ncol then c[0:ncol-1,*] = c_in else c = c_in[0:glm[fi.n].N-1,*]
        glm[fi.n].c = ptr_new(c)
        cnorm = fltarr(glm[fi.n].tot_eff,nc_in)
        if glm[fi.n].tot_eff ge tot_eff then cnorm[0:tot_eff-1,*] = cnorm_in else cnorm = cnorm_in[0:glm[fi.n].tot_eff-1,*]
        glm[fi.nfiles-1].cnorm = ptr_new(cnorm_in)
        glm[fi.nfiles-1].contrast_labels = ptr_new(contrast_labels_in)
        glm[fi.nfiles-1].nc = nc_in
        dummy = save_linmod(fi,dsp,wd,glm,help,stc,pref,fi.n,fi.n,FILENAME='tmp.glm',/NO_LOAD,CLOSELU=rtn.lu)

        ;cmd = '/usr/bin/mv tmp.glm ' + file_names[ifile]
        ;START120426
        cmd = 'mv tmp.glm ' + file_names[ifile]

        status = ''
        spawn,cmd,status
        if status ne '' then begin
            scrap = 'Error updating ' + file_names[ifile]
            print,scrap
            scrapall = scrapall + scrap + string(10B)
        endif
    endif


endfor
if scrapall ne '' then stat = dialog_message(scrapall,/ERROR)


end
