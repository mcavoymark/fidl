;Copyright 12/31/99 Washington University.  All Rights Reserved.
;define_contrast.pro  $Revision: 12.188 $
pro define_contrast_event,ev
    common contrast_comm,cstr,bases,wtab,g_ok
    if ev.id eq g_ok then begin
        widget_control,wtab,GET_VALUE=cstr
        widget_control,ev.top,/DESTROY
    endif
end

;pro create_contrast,glm,contrast_labels,c,model,cnorm,delay,stimlen,tot_eff,nc,lcfunc,hrf_param,funclen,hrf_model
;START151117
pro create_contrast,glm,contrast_labels,c,model,cnorm,delay,stimlen,tot_eff,nc,lcfunc,hrf_param,funclen,hrf_model,n_interest

    effect_label = *glm[model].effect_label
    effect_label = effect_label[0:tot_eff-1]
    effect_column = *glm[model].effect_column
    effect_column = effect_column[0:tot_eff-1]
    effect_length = *glm[model].effect_length
    effect_length = effect_length[0:tot_eff-1]
    if lcfunc[0] eq !BLOCK_DESIGN then $
        stat=dialog_message('This is a block design. This module is intended for event related designs only.',/INFORMATION) $
    else begin 
        setupglm = 1
        if hrf_model eq -1 then hrf_model=!BOYNTON

        ;default_contrasts,glm,contrast_labels,c,model,cnorm,delay,stimlen,tot_eff,nc,lcfunc,hrf_param, $
        ;            funclen,setupglm,hrf_model
        ;START151117
        default_contrasts,glm,contrast_labels,c,model,cnorm,delay,stimlen,tot_eff,nc,lcfunc,hrf_param,funclen,setupglm,hrf_model,$
            n_interest

        get_create_contrast,tot_eff,effect_label,nc,contrast_labels,c,cnorm,glm[model].N,effect_column,effect_length,dc,dcnorm
        if ptr_valid(glm[model].c) then ptr_free,glm[model].c
        if nc ne 0 then glm[model].c = ptr_new(c)
        if ptr_valid(glm[model].contrast_labels) then ptr_free,glm[model].contrast_labels
        if nc ne 0 then glm[model].contrast_labels = ptr_new(contrast_labels)
        if ptr_valid(glm[model].cnorm) then ptr_free,glm[model].cnorm
        if nc ne 0 then glm[model].cnorm = ptr_new(cnorm)
        glm[model].nc = nc
    endelse
end
pro export_contrast,fi,glm,help,contrast_labels,c,model,cnorm,delay,stimlen,nc
    common contrast_comm
    tot_eff = glm[model].tot_eff
    Mcol = glm[model].N
    if ptr_valid(glm[model].effect_label) then effect_label = *glm[model].effect_label
    if ptr_valid(glm[model].effect_length) then effect_length = *glm[model].effect_length
    all_eff = glm[model].all_eff
    if n_elements(contrast_labels) le 0 then contrast_labels = effect_label 
    contrast_labels = reform(contrast_labels,nc)
    cc = c[0:Mcol-1,*]
    clab = strarr(Mcol)
    j = 0
    effect_label = strtrim(effect_label,2)
    istr = strtrim(indgen(max(effect_length))+1,2)
    widget_control,/HOURGLASS
    for eff=0,all_eff-1 do begin
        for i=0,effect_length[eff]-1 do begin
            clab[j] = effect_label[eff]+'_'+istr[i]
            j = j + 1
        endfor
    endfor
    cstr = strarr(nc+1,Mcol+1)
    widget_control,/HOURGLASS
    for i=1,Mcol do begin
        for ic=0,nc-1 do $
            cstr[ic+1,i] = string(cc[i-1,ic],FORMAT='(f8.5)')
    endfor
    cstr[1:nc,0] = contrast_labels[0:nc-1]
    cstr[0,1:Mcol] = clab
    get_dialog_pickfile,'*.slk',getenv('PWD'),'Spreadsheet filename',filename,rtn_nfiles,rtn_path,FILE=fi.tails[fi.n]+'_contrasts.slk'
    if filename eq 'GOBACK' or filename eq 'EXIT' then return
    stat = write_sylk(filename,cstr)
end
pro edit_contrast,glm,help,contrast_labels,c,model,cnorm,delay,stimlen,nc
    common contrast_comm
    ;print,'c'
    ;print,c
    ;print,'size(c)=',size(c)
    ;print,'cnorm'
    ;print,cnorm
    ;print,'size(cnorm)=',size(cnorm)
    tot_eff = glm[model].tot_eff
    Mcol = glm[model].N
    if ptr_valid(glm[model].effect_label) then effect_label = strtrim(*glm[model].effect_label,2)
    if ptr_valid(glm[model].effect_length) then effect_length = *glm[model].effect_length
    if ptr_valid(glm[model].effect_column) then effect_column = *glm[model].effect_column
    all_eff = glm[model].all_eff
    if n_elements(contrast_labels) le 0 then contrast_labels = effect_label 
    value = nc
    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Enter the number of constrasts you wish to specify.  ' $
        +'Be sure to enter a carriage return after you make an entry.'
    label = 'Number of contrasts'
    str = fix(get_str(1,label,value))
    nc1 = fix(str[0])
    if nc1 eq 0 then begin
        stat = dialog_message('No contrasts have been defined. Try "Default contrasts".',/INFORMATION)
        return
    endif
    if nc1 ne nc then begin
        if nc1 lt nc then begin
            cnorm = cnorm[*,0:nc1-1]
            contrast_labels = contrast_labels[0:nc1-1]
            c = c[*,0:nc1-1]
        endif else begin
            cc = fltarr(glm[model].N,nc1)
            ccnorm = fltarr(tot_eff,nc1)
            clabs = strarr(nc1)
            if nc gt 0 then begin
                ccnorm[*,0:nc-1] = cnorm
                clabs[0:nc-1] = contrast_labels[0:nc-1]
                cc[*,0:nc-1] = c[*,*]
            endif else begin
                if nc1 le tot_eff then clabs[0:nc1-1] = effect_label[0:nc1-1]
            endelse
            cnorm = ccnorm
            c = cc
            if strcompress(effect_label[tot_eff-1],/REMOVE_ALL) eq 'Control' then begin
                if nc lt tot_eff-1 then $
                    clabs[nc] = effect_label[nc] $
                else $
                    clabs[nc] = 'New'
            endif else begin
                if nc lt tot_eff then $
                    clabs[nc] = effect_label[nc] $
                else $
                    clabs[nc] = 'New'
            endelse
            contrast_labels = clabs
        endelse
        nc = nc1
    endif

    ;START141120
    ;Initialize contrasts.
    ;if total(c^2) eq 0. then begin
    ;    if glm[model].N gt nc then $
    ;        max = nc $
    ;    else $
    ;        max = glm[model].N
    ;    for i=0,max-1 do begin
    ;        c[i,i] = 1.
    ;        cnorm[i,i] = 1
    ;        if nctl eq 1 then begin
    ;            ;if strpos(effect_label[tot_eff-1],'_ctl') ge 0 then begin
    ;            if strpos(effect_label[tot_eff-1],'ctl') ge 0 then begin
    ;                c[tot_eff-1,i] = -1
    ;                cnorm[tot_eff-1,i] = 1
    ;                ;print,'c=',c[*,i]
    ;                ;print,'cnorm=',cnorm[*,i]
    ;            endif
    ;        endif else begin
    ;            for j=0,tot_eff-1 do begin
    ;                if strpos(effect_label[j],'_ctl') ge 0 then begin
    ;                    if strpos(effect_label[j],effect_label[i]) ge 0 then begin
    ;                        c[j,i] = -1
    ;                        cnorm[j,i] = 1
    ;                    endif
    ;                endif
    ;            endfor
    ;        endelse
    ;    endfor
    ;endif

    cc = c[0:Mcol-1,*]
    ccnorm = cnorm[0:tot_eff-1,*]
    clab = strarr(Mcol+1)
    clab[0] = 'Contrast Names'
    scrap = strcmp(effect_label,'Baseline')
    scraptotal = total(scrap)
    j = 1
    for eff=0,all_eff-1 do begin
        for i=0,effect_length[eff]-1 do begin
            if scraptotal eq 0 and effect_label[eff] eq 'Trend' then begin
                scrap_run_num = strtrim(i/2 + 1,2)
                if i mod 2 ne 0 then $
                    clab[j] = 'Trend run' + scrap_run_num $
                else $
                    clab[j] = 'Baseline run' + scrap_run_num
            endif else if effect_length[eff] eq 1 then $
                clab[j] = strcompress(string(effect_label[eff],FORMAT='(a)'),/REMOVE_ALL) $
            else $
                clab[j] = strcompress(string(effect_label[eff],i+1,FORMAT='(a,"_",i2)'),/REMOVE_ALL)
            j = j + 1
        endfor
    endfor
    rlab = strarr(nc)
    for i=0,nc-1 do rlab[i] = strcompress(string(i+1,FORMAT='("C",i2)'),/REMOVE_ALL)
    cstr = [transpose(contrast_labels),strtrim(cc,2)]
    cwidth = intarr(Mcol+1)
    cwidth[*] = max(strlen(clab))/3 * 35 
    cwidth[0] = max(strlen(contrast_labels[0:nc-1]))/3 * 35 
    cbase =  widget_base(/COLUMN,TITLE='Please enter desired contrast.',/MODAL,GROUP_LEADER=!FMRI_LEADER)
    wtab = widget_table(cbase,/EDITABLE,/RESIZEABLE_COLUMNS,VALUE=cstr,FORMAT='(a)',COLUMN_LABELS=clab,ROW_LABELS=rlab, $
        COLUMN_WIDTHS=cwidth,XSIZE=n_elements(clab),YSIZE=nc)
    g_ok = widget_button(cbase,VALUE='(Press return on the keyboard before hitting this button.) OK')
    geo = widget_info(cbase,/GEOMETRY)
    xdim = geo.scr_xsize + 2*geo.margin
    ydim = geo.scr_ysize + 2*geo.margin
    if xdim gt !DISPXMAX or ydim gt !DISPYMAX then begin
        widget_control,cbase,/DESTROY
        cbase =  widget_base(/COLUMN,TITLE='Please enter desired contrast.',/MODAL,GROUP_LEADER=!FMRI_LEADER)
        wtab = widget_table(cbase,/EDITABLE,/RESIZEABLE_COLUMNS,VALUE=cstr,FORMAT='(a)',COLUMN_LABELS=clab,ROW_LABELS=rlab, $
            COLUMN_WIDTHS=cwidth,SCR_XSIZE=xdim<!DISPXMAX,SCR_YSIZE=ydim<!DISPYMAX)
        g_ok = widget_button(cbase,VALUE='(Press return on the keyboard before hitting this button.) OK')
    endif
    widget_control,cbase,/REALIZE
    xmanager,'define_contrast',cbase
    c[0:Mcol-1,*] = float(cstr[1:Mcol,*])
    contrast_labels = reform(cstr[0,0:nc-1])
    cnorm[0:tot_eff-1,*] = ccnorm
end
pro import_contrasts,glm,help,contrast_labels,c,model,cnorm,delay,stimlen,tot_eff,nc,lcfunc,hrf_param,funclen
    get_directory,path
    get_dialog_pickfile,'*.slk',path,'Please load a slk file.',filename,rtn_nfiles,rtn_path,/DONT_ASK
    if filename eq 'GOBACK' or filename eq 'EXIT' or filename eq '' then return
    widget_control,/HOURGLASS
    cstr = read_sylk_str(filename,/ARRAY)
    nc = n_elements(cstr[*,0]) - 1
    contrast_labels = cstr[1:nc,0]
    c = transpose(float(cstr[1:nc,1:glm[model].N]))
    if ptr_valid(glm[model].effect_length) then $
        effect_length = *glm[model].effect_length $
    else begin
        stat=dialog_message('GLM too old. Please resave to update.',/ERROR)
        return
    endelse
    cnorm = fltarr(tot_eff,nc)
    for i=0,nc-1 do begin
        k=0
        for j=0,tot_eff-1 do begin
            scrap = where(c[k:k+effect_length[j]-1,i] ne 0.,count)
            if count ne 0 then cnorm[j,i]=1.
            k = k + effect_length[j]    
        endfor
    endfor
    stat=dialog_message('Contrasts have been successfully imported.'+string(10B)+string(10B) $
        +'You will need to save this GLM for the contrasts to be retained.',/INFORMATION)
    print,'DONE'
end
pro delete_contrast,glm,help,contrast_labels,c,nc,cnorm
    if nc eq 0 then begin
        stat = dialog_message('No contrasts have been defined. Try "Default contrasts".',/INFORMATION)
        return
    endif
    rtn = select_files(contrast_labels,TITLE='Please select contrasts to delete.',MIN_NUM_TO_SELECT=-1)
    if rtn.count eq 0 then return
    index = where(rtn.list eq 0,count)
    if count ne 0 then begin 
        if count lt nc then begin
            c = c[*,index]
            cnorm = cnorm[*,index] 
            contrast_labels = contrast_labels[index]
            nc = count
        endif
    endif else begin
        c[*,*] = 0
        cnorm[*,*] = 0
        nc = 0
        contrat_labels = 0
    endelse    
end
pro default_contrasts,glm,contrast_labels,c,model,cnorm,delay,stimlen,tot_eff,nc,lcfunc,hrf_param,funclen,setupglm,hrf_model, $
    n_interest
    if ptr_valid(glm[model].effect_label) then effect_label = *glm[model].effect_label else effect_label = strarr(tot_eff)
    if ptr_valid(glm[model].effect_length) then effect_length = *glm[model].effect_length else effect_length = funclen
    if n_interest gt tot_eff then begin 
        if setupglm eq !FALSE then begin
            scrap = strarr(nc*2)
            scraplabels = strarr(nc*2)
            j = 0
            for i=0,nc*2-1,2 do begin
                scrap[i] = stimlen[j]
    
                ;scraplabels[i] = effect_labels[j] + ' duration'
                ;START140320
                scraplabels[i] = effect_label[j] + ' duration'
    
                j = j + 1
            endfor
            j = 0
            for i=1,nc*2-1,2 do begin
                scrap[i] = delay[j]
    
                ;scraplabels[i] = effect_labels[j] + ' delay'
                ;START140320
                scraplabels[i] = effect_label[j] + ' duration'
    
                j = j + 1
            endfor
            scrap = float(get_str(nc,scraplabels,scrap,TITLE='Please specify durations and delays.'))
            j = 0
            for i=0,nc*2-1,2 do begin
                stimlen[j] = scrap[i]
                j = j + 1
            endfor
            j = 0
            for i=1,nc*2-1,2 do begin
                delay[j] = scrap[i]
                j = j + 1
            endfor
        endif
        if ptr_valid(glm[model].effect_TR) then $
            effect_TR = *glm[model].effect_TR $
        else begin
            effect_TR = fltarr(tot_eff)
            effect_TR[*] = glm[model].TR
        endelse
        if ptr_valid(glm[model].effect_shift_TR) then effect_shift_TR=*glm[model].effect_shift_TR else effect_shift_TR=fltarr(tot_eff)
    endif
    N=glm[model].N
    rtn=default_contrasts_noglm(effect_label,effect_length,tot_eff,N,effect_TR,effect_shift_TR,stimlen,delay,lcfunc,hrf_param, $
        n_interest,hrf_model)
    nc=rtn.nc
    c=rtn.c
    cnorm=rtn.cnorm
    contrast_labels=rtn.contrast_labels
end

;**************************************************************************
pro modify_contrast,glm,help,contrast_labels,c,model,cnorm,delay,stimlen,nc
;**************************************************************************
common contrast_comm
if ptr_valid(glm[model].effect_length) then effect_length = *glm[model].effect_length
if ptr_valid(glm[model].effect_column) then effect_column = *glm[model].effect_column
labels = strarr(nc)
scrapstr = strarr(nc)
cindex = intarr(nc)
j = 0
for i=0,nc-1 do begin
    index = where(cnorm[*,i] ne 0.,count)
    if count eq 1 then begin
        labels[j] = contrast_labels[j]
        eci = effect_column[index]
        eli = effect_length[index]
        scrap = strtrim(c[eci[0]:eci[0]+eli[0]-1,i],2)
        scrapstr[j] = scrap[0]
        scrap2 = effect_length[index]
        for k=1,scrap2[0]-1 do scrapstr[j] = scrapstr[j] + ' ' + scrap[k]
        cindex[j] = i
        j = j + 1
    endif
endfor
labels = labels[0:j-1]
scrapstr = scrapstr[0:j-1]
scrapstr = scrapstr + '                                                                                                 '
cindex = cindex[0:j-1]
el = effect_length[cindex]
repeat begin
    stat = 'DONE'
    scrap = get_str(j,labels,scrapstr,TITLE='Please paste contrasts.',/ONE_COLUMN,/BELOW)
    scrap2 = intarr(j)
    for i=0,j-1 do scrap2[i] = n_elements(strsplit(scrap[i],/EXTRACT))
    index = where((scrap2-el) ne 0,count)
    if count gt 0 then begin
        str = labels[index[0]] 
        for i=1,count-1 do str = str + ', ' + labels[index[i]]
        stat = dialog_message(str+string(10B)+'The number of weights is incorrect. Please try again.',/ERROR,/CANCEL)
        if stat eq 'Cancel' then return
    endif else begin        
        count = 0
        index = intarr(j)
        k = 0
        igoose = where(scrap2 gt 1,cgoose)
        if cgoose ne 0 then begin
            for i=0,cgoose-1 do begin
                goose = float(strsplit(scrap[igoose[i]],/EXTRACT))
                if abs(total(goose)) gt 0.01 then begin
                    count = count + 1
                    index[k] = i
                    k = k + 1
                endif
            endfor
        endif
        if k gt 0 then index = index[0:k-1]
        if count gt 0 then begin
            str = labels[index[0]]
            for i=1,count-1 do str = str + ', ' + labels[index[i]]
            stat = get_button(['Ok, I know what I am doing.','Let me try again.','exit'],TITLE='Contrast does not sum to zero.', $
                BASE_TITLE=str) 
            if stat eq 0 then stat='DONE' else if stat eq 2 then return
        endif else begin
            count = 0
            index = intarr(j)
            k = 0
            for i=0,j-1 do begin
                scrap2 = float(strsplit(scrap[i],/EXTRACT))
                if abs(total(scrap2^2.)-1.) gt 0.01 then begin
                    count = count + 1 
                    index[k] = i
                    k = k + 1
                endif
            endfor
            if k gt 0 then index = index[0:k-1]
            if count gt 0 then begin
                str = labels[index[0]] 
                for i=1,count-1 do str = str + ', ' + labels[index[i]]
                stat = get_button(['Ok, I know what I am doing.','Let me try again.','exit'], $
                    TITLE='Contrast does not have a magnitude of 1.',BASE_TITLE=str) 
                if stat eq 0 then stat='DONE' else if stat eq 2 then return
            endif
        endelse
    endelse
endrep until stat eq 'DONE'
for i=0,j-1 do begin
    scrap2 = float(strsplit(scrap[i],/EXTRACT))
    index = where(cnorm[*,cindex[i]] ne 0.,count) 
    eci = effect_column[index]
    eli = effect_length[index]
    c[eci[0]:eci[0]+eli[0]-1,cindex[i]] = scrap2 
endfor
end
;**********************************************************************************************
pro define_contrast,glm,model,fi,help,dsp,wd,stc,pref,hrf_model,SETUPGLM=setupglm,NOMENU=nomenu
;**********************************************************************************************
common contrast_comm
if not keyword_set(SETUPGLM) then setupglm = 0 
if n_elements(hrf_model) eq 0 then hrf_model = -1 
if keyword_set(NOMENU) then $
    lcmenu = 0 $
else begin
    if hrf_model eq -1 then lcmenu = 1 else lcmenu = 0
endelse
nglm = 1
n_interest = -1
action2 = -1
lcstop = 0 
base = 0
labels = strarr(15)
labels[0] = 'Default contrasts'
labels[1] = 'Create and delete contrasts'
labels[2] = 'Delete contrasts'
labels[3] = 'Edit contrasts'
labels[4] = 'Form sum'
labels[5] = 'Form difference'
labels[6] = 'Form average'
labels[7] = 'Export contrasts to a spreadsheet (in sylk format)'
labels[8] = 'Import contrasts from a spreadsheet (in sylk format)'
labels[9] = 'Modify contrasts'
labels[10] = 'Rename contrasts'
labels[11] = 'Copy contrasts (IDL)'
labels[12] = 'Copy contrasts (C)'
labels[13] = 'Annalisa contrasts'
labels[14] = 'Exit'
widget_control,/HOURGLASS
repeat begin
    if lcmenu eq 1 then begin 
        if action2 eq -1 then $
            action = get_button(labels,TITLE='Please select action.') $
        else begin
            action = action2
            action2 = -1
        endelse
    endif else begin
        action = 0
        lcstop = 1
    endelse
    if (action lt 10 or setupglm eq 1) and n_interest eq -1 then begin
        if setupglm eq 0 then begin 
            if fi.nfiles eq 0 then wd1_load_linmod,fi,dsp,wd,glm,help
            if fi.nfiles gt 0 then begin
                dsp_image = update_image(fi,dsp,wd,stc,pref)
                hdr = *fi.hdr_ptr[fi.n]
                model = hdr.model
            endif else $
                return   
        endif
        n_interest = glm[model].n_interest
        lcfunc = *glm[model].lcfunc
        funclen = glm[model].funclen
        if ptr_valid(glm[model].contrast_labels) then $
            contrast_labels = strtrim(*glm[model].contrast_labels,2) $
        else $
            contrast_labels = ''
        tot_eff = glm[model].tot_eff
        hrf_param = fltarr(2,tot_eff)
        hrf_param[0,*] = !HRF_DELTA
        hrf_param[1,*] = !HRF_TAU
        if ptr_valid(glm[model].stimlen) then stimlen = *glm[model].stimlen else stimlen = fltarr(tot_eff)
        if ptr_valid(glm[model].delay) then delay = *glm[model].delay else delay = fltarr(tot_eff)

        print,'define_contrast glm[model].nc=',glm[model].nc

        if ptr_valid(glm[model].c) and glm[model].nc ne 0 then begin
            c = *glm[model].c
            sz = size(c)
            if sz[0] eq 1 then begin
                c = reform(c,n_elements(c),1)
                nc = 1
            endif else begin
                if(sz(0) gt 1) then $
                    nc = sz(2) $
                else $
                    nc = 0
            endelse
        endif else begin
            nc = 0
        endelse
        if ptr_valid(glm[model].cnorm) and glm[model].nc ne 0 then cnorm = *glm[model].cnorm
        if nc eq 0 then begin
            if action gt 0 then begin
                action2 = action
                action = 0
            endif
        endif
    endif

    print,'define_contrast nc=',nc

    case action of
        0:default_contrasts,glm,contrast_labels,c,model,cnorm,delay,stimlen,tot_eff,nc,lcfunc,hrf_param, $
              funclen,setupglm,hrf_model,n_interest
        1:create_contrast,glm,contrast_labels,c,model,cnorm,delay,stimlen,tot_eff,nc,lcfunc,hrf_param,funclen,hrf_model,n_interest
        2:delete_contrast,glm,help,contrast_labels,c,nc,cnorm
        3:edit_contrast,glm,help,contrast_labels,c,model,cnorm,delay,stimlen,nc
        4:sum_contrast,help,contrast_labels,nc,c,cnorm,glm[model].N,tot_eff
        5:difference_contrast,help,contrast_labels,nc,c,cnorm,glm[model].N,tot_eff,*glm[model].effect_label
        6:avg_contrast,help,contrast_labels,nc,c,cnorm,glm[model].N,tot_eff 
        7:export_contrast,fi,glm,help,contrast_labels,c,model,cnorm,delay,stimlen,nc
        8:import_contrasts,glm,help,contrast_labels,c,model,cnorm,delay,stimlen,tot_eff,nc,lcfunc,hrf_param,funclen
        9:modify_contrast,glm,help,contrast_labels,c,model,cnorm,delay,stimlen,nc
        10:fidl_rename_contrasts,fi,dsp,wd,glm,help,stc,pref
        11:copy_contrasts,fi,dsp,wd,glm,help,stc,pref
        12:fidl_copy_contrasts,fi,dsp,wd,glm,help,stc
        13:fidl_annalisa_contrast
        14:lcstop = !TRUE
        else: print,'Invalid choice.'
    endcase
endrep until lcstop eq !TRUE
if n_interest gt -1 then begin 
    if ptr_valid(glm[model].c) then ptr_free,glm[model].c
    if nc ne 0 then glm[model].c = ptr_new(c)
    if ptr_valid(glm[model].contrast_labels) then ptr_free,glm[model].contrast_labels
    if nc ne 0 then glm[model].contrast_labels = ptr_new(contrast_labels)

    ;if ptr_valid(glm[model].stimlen) then ptr_free,glm[model].stimlen
    ;if nc ne 0 then glm[model].stimlen = ptr_new(stimlen)
    ;if ptr_valid(glm[model].delay) then ptr_free,glm[model].delay
    ;if nc ne 0 then glm[model].delay = ptr_new(delay)
    ;START141120
    if ptr_valid(glm[model].stimlen) then ptr_free,glm[model].stimlen
    glm[model].stimlen = ptr_new(stimlen)
    if ptr_valid(glm[model].delay) then ptr_free,glm[model].delay
    glm[model].delay = ptr_new(delay)

    if ptr_valid(glm[model].cnorm) then ptr_free,glm[model].cnorm
    if nc ne 0 then glm[model].cnorm = ptr_new(cnorm)
    glm[model].nc = nc
    ;print,'c=',c
    ;print,'contrast_labels=',contrast_labels
    ;print,'stimlen=',stimlen
    ;print,'delay=',delay
    ;print,'cnorm=',cnorm
    ;print,'nc=',nc
    ;print,'*glm[model].delay=',*glm[model].delay
endif
;print,'define_contrast bottom'
end
