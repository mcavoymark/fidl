;Copyright 10/30/02 Washington University.  All Rights Reserved.
;get_factor_levels.pro  $Revision: 1.23 $

;function get_factor_levels,help,time,within_or_between,rtn,rtn_slk,DATATYPE=datatype,NFACTORSIS0=nfactorsis0
;START150504
function get_factor_levels,help,time,within_or_between,rtn,rtn_slk,addtitle,DATATYPE=datatype,NFACTORSIS0=nfactorsis0
if n_elements(addtitle) eq 0 then addtitle=''

if not keyword_set(DATATYPE) then datatype=0
if n_elements(rtn) then begin
    nfactors = rtn.nfactors
    if nfactors eq -1 then begin
        nfactors = 0
        goto,goback_gfl0b
    endif
    factor_names = rtn.factor_names
    ;help,rtn,/STRUCTURE
    nlevels = rtn.nlevels
    treatments = rtn.treatments
    level_names = rtn.level_names
    total_nlevels = total(nlevels)
    if nfactors eq 0 then goto,goback_gfl0a
endif else begin
    goback_gfl0b:
    nlevels = 0
    level_names = ''
    goback_gfl0a:
    factor_names = ''
    if help.enable eq !TRUE then begin
        if within_or_between eq 0 then begin
            if time lt 2 then begin
                scrap2 = 'You need to specify the number of within-subject factors. These factors are crossed. ' $
                    + 'If 0 is entered then a simple main effect for a single condition is computed.'
                widget_control,help.id,SET_VALUE=scrap2
            endif
        endif else begin
            scrap2 = 'You need to specify the number of between-subject factors. These factors are not crossed. ' $
                + 'If 0 is entered then a single group of subjects is analyzed.'
            widget_control,help.id,SET_VALUE=scrap2
        endelse
    endif
    if within_or_between eq 0 then begin
        if time lt 2 then begin
            title = 'Please enter the number of within-subject factors'
            if time eq 0 then begin
                if datatype eq 0 then begin
                    title = title + ' excluding time'
                    dummy = 'Enter 0 if time is the only within-subject factor. '
                endif else begin 
                    title = title + ' including time'
                    dummy = 'Enter 1 if time is the only within-subject factor. '
                endelse
            endif else begin
                dummy = 'Number of factors'
            endelse
            title = title + '.'
            nfactors = get_str(1,dummy,1,TITLE=title,WIDTH=30,/GO_BACK)
            if nfactors[0] eq 'GO_BACK' then return,rtn={nfactors:-1}
            nfactors = fix(nfactors[0])
        endif else begin
            nfactors = 0
        endelse
    endif else begin
        if keyword_set(NFACTORSIS0) then $ 
            nfactors = 0 $
        else begin
            if n_elements(rtn) eq 0 then begin
                if time lt 2 then nfactors = 0 else nfactors = 1
            endif

            ;KEEP just to remember that it can also be done this way
            ;nfactors = get_str(1,'Enter 0 to examine a single group of subjects. ',nfactors,/GO_BACK,/EXIT,/ALIGN_LEFT, $
            ;    TITLE='Please enter the number of between-subject factors.',LABEL='Between-subject factors can be assigned from ' $
            ;    +'a slk.'+string(10B)+'Use the column label for the factor name and the individual values for the level names.' $
            ;    +addtitle)
            ;START160506
            nfactors = get_str(1,'Enter 0 to examine a single group of subjects. ',nfactors,/GO_BACK,/EXIT,/ALIGN_LEFT, $
                TITLE='Please enter the number of between-subject factors.',LABEL='Additional between-subject factors can be ' $
                +'assigned from a slk.'+string(10B)+'You will be asked if you want to load a slk later.' $
                +addtitle)

            if nfactors[0] eq 'GO_BACK' then return,rtn={nfactors:-1} else if nfactors[0] eq 'EXIT' then return,rtn={nfactors:-2}
            nfactors = fix(nfactors[0])
        endelse
    endelse
    treatments = intarr(nfactors+1,/NOZERO)
    treatments[nfactors] = 1
    if nfactors gt 0 then begin

        ;if n_elements(labels_factor_names) lt nfactors then begin
        ;    goback_gfl0:
        ;    if within_or_between eq 0 then $
        ;        scrap = 'within' $
        ;    else $
        ;        scrap = 'between'
        ;    labels_factor_names = strarr(nfactors)
        ;    for i=0,nfactors-1 do labels_factor_names[i] = scrap + '-subject factor ' + strcompress(string(i+1),/REMOVE_ALL)
        ;endif
        ;START170609
        goback_gfl0:
        if within_or_between eq 0 then $
            scrap = 'within' $
        else $
            scrap = 'between'
        labels_factor_names = strarr(nfactors)
        for i=0,nfactors-1 do labels_factor_names[i] = scrap + '-subject factor ' + strcompress(string(i+1),/REMOVE_ALL)


        if factor_names[0] eq '' then begin
            factor_names = labels_factor_names
            factor_names = fix_script_name(factor_names,/SPACE_ONLY)
        endif 
        scrap = strarr(2)
        if within_or_between eq 0 then begin
            scrap[0] = 'cue'
            scrap[1] = 'visual_field'
        endif else begin
            scrap[0] = 'age'
            scrap[1] = 'gender'
        endelse
        if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='You may specify names of your own choosing for the ' $
            +'factors. For example, if two factors have been specified, factor 1 may be "'+scrap[0]+'" and factor 2 "' $
            +scrap[1]+'".'
        factor_names = get_str(nfactors,labels_factor_names,factor_names,WIDTH=50,TITLE='Please enter the factor names.',/GO_BACK)
        if factor_names[0] eq 'GO_BACK' then goto,goback_gfl0a 
        factor_names = fix_script_name(factor_names,/SPACE_ONLY)
        nlevels = intarr(nfactors,/NOZERO)
        nlevels[*] = 2 ; each factor must have at least 2 levels
        scrap1 = strarr(2,2)
        if within_or_between eq 0 then begin
            scrap1[0,0] = 'valid'
            scrap1[0,1] = 'invalid'
            scrap1[1,0] = 'right'
            scrap1[1,1] = 'left'
        endif else begin
            scrap1[0,0] = 'young'
            scrap1[0,1] = 'old'
            scrap1[1,0] = 'male'
            scrap1[1,1] = 'female'
        endelse
        goback_gfl1:
        if help.enable eq !TRUE then widget_control,help.id,SET_VALUE=string('Each factor must have at least two levels. For ' $
            +'example, you may have a factor "'+scrap[0]+'" at two levels, "'+scrap1[0,0]+'" and "'+scrap1[0,1]+'", and ' $
            +'another factor "'+scrap[1] +'" at two levels, "'+scrap1[1,0]+'" and "'+scrap1[1,1]+'".')
        nlevels = get_str(nfactors,factor_names,nlevels,WIDTH=40,TITLE='Please enter the number of levels of each factor.', $
            /GO_BACK)
        if nlevels[0] eq 'GO_BACK' then goto,goback_gfl0
        nlevels = fix(nlevels)

        index = where(nlevels lt 2,count)
        if count ne 0 then begin
            stat=dialog_message('All factors must have at least 2 levels.',/ERROR)
            goto,goback_gfl0
        endif 
    
        treatments[nfactors-1] = nlevels[nfactors-1]
        for i=nfactors-2,0,-1 do treatments[i] = treatments[i+1] * nlevels[i]
    
        total_nlevels = total(nlevels)
        lc_level_names = !TRUE
        if n_elements(rtn) then begin
            if rtn.nfactors gt 0 then begin
                if total(rtn.nlevels) eq total_nlevels then lc_level_names = !FALSE
            endif
        endif
        if lc_level_names eq !TRUE then begin
            level_names = strarr(total_nlevels)
            k = 0
            for i=0,nfactors-1 do begin
                for j=0,nlevels[i]-1 do begin
                    level_names[k] = 'level' + strtrim(j+1,2)
                    k = k + 1
                endfor
            endfor
        endif else begin
            level_names = rtn.level_names
        endelse
    endif
endelse
if nfactors gt 0 then begin
    labels_level_names = strarr(total_nlevels)
    k = 0
    for i=0,nfactors-1 do begin
        for j=0,nlevels[i]-1 do begin
            labels_level_names[k] = factor_names[i] + '.' + 'level' + strtrim(j+1,2)
            k = k + 1
        endfor
    endfor
    if help.enable eq !TRUE then begin
        widget_control,help.id,SET_VALUE='You may specify level names for each of the factor levels. ' $
        +'For example, you may have a factor '+'"'+scrap[0]+'" at two levels, "'+scrap1[0,0]+'" and "' $
        +scrap1[0,1]+'", and another factor "'+scrap[1]+'" at two levels, '+'"'+scrap1[1,0]+'" and "'+scrap1[1,1]+'".'
    endif
    if within_or_between eq 0 and n_elements(rtn_slk) then begin
        if treatments[0] gt rtn_slk.ncolumn_labels then begin
            stat = get_button(['go back','exit'],BASE_TITLE='Error',TITLE='Your slk file has '+strtrim(rtn_slk.ncolumn_labels,2) $
                +' conditions, but you have specified '+strtrim(treatments[0],2)+'.')
            if stat eq 0 then goto,goback_gfl0b else return,rtn={nfactors:-2}
        endif
    endif
    level_names = get_str(total_nlevels,labels_level_names,level_names,WIDTH=40, $
        TITLE='Please enter the level names for each factor.',/GO_BACK)
    if level_names[0] eq 'GO_BACK' then goto,goback_gfl1
    level_names = fix_script_name(level_names,/SPACE_ONLY)
endif 
return,rtn={nfactors:nfactors,factor_names:factor_names,nlevels:nlevels,treatments:treatments,level_names:level_names}
end
