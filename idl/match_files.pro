;Copyright 3/15/01 Washington University.  All Rights Reserved.
;match_files.pro  $Revision: 1.11 $

;function match_files,files1,files2,rtnidx,neach,TITLE=title,GO_BACK=go_back,BASE_TITLE=base_title,MORE_THAN_ONE=more_than_one
;START30
function match_files,files1,files2,rtnidx,neach,TITLE=title,GO_BACK=go_back,BASE_TITLE=base_title,MORE_THAN_ONE=more_than_one, $
    SUMROWS=sumrows

if not keyword_set(BASE_TITLE) then base_title = ''
if not keyword_set(TITLE) then title = 'Please select file for '
files2_copy = files2 ;need to preserve files2 for anova
n_elements_files1 = n_elements(files1)
n_elements_files2 = n_elements(files2_copy)
origidx = indgen(n_elements_files2)

if not keyword_set(MORE_THAN_ONE) then begin
    if n_elements_files1 gt 1 and n_elements_files2 eq 1 then begin
        index = get_button(['yes','go back'],BASE_TITLE=files2_copy[0],TITLE='Use for all?') 
        if index eq 1 then return,'GO_BACK'
        files2_select = make_array(n_elements_files1,/STRING,VALUE=files2_copy[0])
    endif else begin
        files2_select = strarr(n_elements_files1)
        rtnidx = intarr(n_elements_files1)

        if not keyword_set(SUMROWS) then loop=n_elements_files1-1 else loop=sumrows-1
        for i=0,loop do begin

            if keyword_set(GO_BACK) then $
                index = get_button([files2_copy,'GO BACK'],TITLE=title+files1[i],BASE_TITLE=base_title) $
            else $
                index = get_button(files2_copy,TITLE=title+files1[i],BASE_TITLE=base_title)
            if index eq n_elements_files2 then return,'GO_BACK'
            files2_select[i] = files2_copy[index]
            rtnidx[i] = origidx[index]
            for j = index,n_elements_files2-2 do begin
                files2_copy[j] = files2_copy[j+1]
                origidx[j] = origidx[j+1]
            endfor
            n_elements_files2 = n_elements_files2 - 1
            if n_elements_files2 gt 0 then begin
                files2_copy = files2_copy[0:n_elements_files2-1]
                origidx = origidx[0:n_elements_files2-1]
            endif
        endfor
    endelse
endif else begin
    files2_select = ''
    neach = intarr(n_elements_files1)
    for i=0,n_elements_files1-1 do begin
        goback0:
        if keyword_set(GO_BACK) then begin 
            index = get_bool_list([files2_copy,'GO BACK'],TITLE=title+files1[i],BASE_TITLE=base_title)
            index = index.list
            if index[n_elements_files2] eq 1 then return,'GO_BACK'
        endif else begin
            index = get_bool_list(files2_copy,TITLE=title+files1[i],BASE_TITLE=base_title)
            index = index.list
        endelse
        index_select = where(index,count)
        index_notselect = where(index eq 0,count_notselect)
        if count eq 0 then begin
            stat=dialog_message('You failed to make a choice. Please try again.',/ERROR)
            goto,goback0
        endif
        files2_select = [files2_select,files2_copy[index_select]]
        neach[i] = count
        if count_notselect gt 1 then files2_copy = files2_copy[index_notselect[0:count_notselect-2]]
        n_elements_files2 = n_elements_files2 - count 
    endfor
    files2_select = files2_select[1:*]
endelse
return,files2_select
end
