;Copyright 5/31/05 Washington University.  All Rights Reserved.
;get_mask.pro  $Revision: 1.14 $

;function get_mask,space,fi,mfile,BASE_TITLE=base_title
;START170714
function get_mask,space,fi,mfile,filter,BASE_TITLE=base_title
if not keyword_set(filter) then filter='*img'

if not keyword_set(BASE_TITLE) then base_title = 'Please select mask.'


;if space eq !SPACE_111 then begin
;    goback1:
;    masktype = get_button(['Jenny Wu white matter','Standard Avi','Load','None','GO BACK'],BASE_TITLE=base_title)
;    if masktype eq 4 then $
;        return,'GO_BACK' $
;    else if masktype eq 3 then $
;        return,''  $
;    else if masktype eq 2 then begin
;        get_dialog_pickfile,'*img',fi.path,'Please select mask',maskfile,nmaskfile,rtn_path
;        if maskfile eq 'GOBACK' then goto,goback1
;        return,maskfile
;    endif else if masktype eq 1 then $
;        return,!MASK_FILE_111 $
;    else $
;        return,!MASK_FILE_111ASIGMA
;endif else if space eq !SPACE_222 or space eq !SPACE_333 then begin
;    goback2:
;    masktype = get_button(['Standard Avi','Load','None','GO BACK'],BASE_TITLE=base_title, $
;        TITLE='Hint. Standard Avi is a good choice.')
;    if masktype eq 3 then $
;        return,'GO_BACK' $
;    else if masktype eq 2 then $
;        return,''  $
;    else if masktype eq 1 then begin
;        get_dialog_pickfile,'*img',fi.path,'Please select mask',maskfile,nmaskfile,rtn_path
;        if maskfile eq 'GOBACK' then goto,goback2
;        return,maskfile
;    endif else if masktype eq 0 then begin
;        if space eq !SPACE_111 then $
;            return,!MASK_FILE_111 $
;        else if space eq !SPACE_222 then $
;            return,!MASK_FILE_222 $
;        else if space eq !SPACE_333 then $
;            return,!MASK_FILE_333
;    endif
;endif else if space eq !SPACE_MNI222 then begin 
;    goback3:
;    masktype = get_button(['MNI 222','Load','None','Go back'],BASE_TITLE=base_title, $
;        TITLE='Hint. MNI 222 is a good choice.')
;    if masktype eq 3 then $
;        return,'GO_BACK' $
;    else if masktype eq 2 then $
;        return,''  $
;    else if masktype eq 1 then begin
;        get_dialog_pickfile,'*img',fi.path,'Please select mask',maskfile,nmaskfile,rtn_path
;        if maskfile eq 'GOBACK' then goto,goback3
;        return,maskfile
;    endif else $
;        return,!MASK_MNI222
;endif else begin
;    goback4:
;    wallace=['Load','None','Go back']
;    ne_mfile=n_elements(mfile)
;    if ne_mfile ne 0 then wallace=[mfile,wallace]
;    masktype=get_button(wallace,TITLE=base_title)
;    if masktype eq 2+ne_mfile then $
;        return,'GO_BACK' $
;    else if masktype eq 1+ne_mfile then $
;        return,''  $
;    else if masktype eq ne_mfile then begin
;        get_dialog_pickfile,'*img',fi.path,'Please select mask',maskfile,nmaskfile,rtn_path
;        if maskfile eq 'GOBACK' then goto,goback4
;        return,maskfile
;    endif else begin
;        return,mfile[masktype]
;    endelse
;endelse
;START170524
ne_mfile=n_elements(mfile)

;START170712
if ne_mfile ne 0 then begin
    if mfile[0] eq '' then ne_mfile=0
endif

if space eq !SPACE_111 and ne_mfile eq 0 then begin
    goback1:
    masktype = get_button(['Jenny Wu white matter','Standard Avi','Load','None','GO BACK'],BASE_TITLE=base_title)
    if masktype eq 4 then $
        return,'GO_BACK' $
    else if masktype eq 3 then $
        return,''  $
    else if masktype eq 2 then begin
        get_dialog_pickfile,'*img',fi.path,'Please select mask',maskfile,nmaskfile,rtn_path
        if maskfile eq 'GOBACK' then goto,goback1
        return,maskfile
    endif else if masktype eq 1 then $
        return,!MASK_FILE_111 $
    else $
        return,!MASK_FILE_111ASIGMA
endif else if(space eq !SPACE_222 or space eq !SPACE_333) and ne_mfile eq 0 then begin
    goback2:
    masktype = get_button(['Standard Avi','Load','None','GO BACK'],BASE_TITLE=base_title, $
        TITLE='Hint. Standard Avi is a good choice.')
    if masktype eq 3 then $
        return,'GO_BACK' $
    else if masktype eq 2 then $
        return,''  $
    else if masktype eq 1 then begin
        get_dialog_pickfile,'*img',fi.path,'Please select mask',maskfile,nmaskfile,rtn_path
        if maskfile eq 'GOBACK' then goto,goback2
        return,maskfile
    endif else if masktype eq 0 then begin
        if space eq !SPACE_111 then $
            return,!MASK_FILE_111 $
        else if space eq !SPACE_222 then $
            return,!MASK_FILE_222 $
        else if space eq !SPACE_333 then $
            return,!MASK_FILE_333
    endif
endif else if space eq !SPACE_MNI222 and ne_mfile eq 0 then begin
    goback3:
    masktype = get_button(['MNI 222','Load','None','Go back'],BASE_TITLE=base_title, $
        TITLE='Hint. MNI 222 is a good choice.')
    if masktype eq 3 then $
        return,'GO_BACK' $
    else if masktype eq 2 then $
        return,''  $
    else if masktype eq 1 then begin
        get_dialog_pickfile,'*img',fi.path,'Please select mask',maskfile,nmaskfile,rtn_path
        if maskfile eq 'GOBACK' then goto,goback3
        return,maskfile
    endif else $
        return,!MASK_MNI222
endif else begin
    goback4:
    wallace=['Load','None','Go back']
    if ne_mfile ne 0 then wallace=[mfile,wallace]
    masktype=get_button(wallace,TITLE=base_title)
    if masktype eq 2+ne_mfile then $
        return,'GO_BACK' $
    else if masktype eq 1+ne_mfile then $
        return,''  $
    else if masktype eq ne_mfile then begin

        ;get_dialog_pickfile,'*img',fi.path,'Please select mask',maskfile,nmaskfile,rtn_path
        ;START170714
        get_dialog_pickfile,filter,fi.path,'Please select mask',maskfile,nmaskfile,rtn_path

        if maskfile eq 'GOBACK' then goto,goback4
        return,maskfile
    endif else begin
        return,mfile[masktype]
    endelse
endelse


return,''
end
