;Copyright 3/15/01 Washington University.  All Rights Reserved.
;select_files.pro  $Revision: 1.26 $

;function select_files,files,TITLE=title,GO_BACK=go_back,EXIT=exit,MIN_NUM_TO_SELECT=min_num_to_select,CANCEL=cancel, $
;    ONE_COLUMN=one_column,NO_ALL=no_all,BASE_TITLE=base_title,SPECIAL=special,GROUP_LEADER=group_leader,S2=s2
;START150504
function select_files,files,TITLE=title,GO_BACK=go_back,EXIT=exit,MIN_NUM_TO_SELECT=min_num_to_select,CANCEL=cancel, $
    ONE_COLUMN=one_column,NO_ALL=no_all,BASE_TITLE=base_title,SPECIAL=special,GROUP_LEADER=group_leader,S2=s2,ALIGN_LEFT=align_left

if not keyword_set(BASE_TITLE) then base_title = 0 
if not keyword_set(TITLE) then title = 'Please select files.'
if not keyword_set(SPECIAL) then special=0
if not keyword_set(MIN_NUM_TO_SELECT) then $
    min_num_to_select = 1 $
else if min_num_to_select eq -1 then $
    min_num_to_select = 0
if min_num_to_select eq 1 then $
    errmsg = 'You failed to make a selection.' $
else $
    errmsg = 'You must make at least '+strcompress(string(min_num_to_select),/REMOVE_ALL)+' selections.'
errmsg = errmsg + ' Please try again.'
if keyword_set(CANCEL) then errmsg = errmsg + string(10B) + 'Hit cancel to exit.'
nfiles = n_elements(files)
if nfiles gt 1 and not keyword_set(NO_ALL) then $
    choices = ['ALL',files] $
else $
    choices = files
if not keyword_set(ONE_COLUMN) then one_column = 0 
if not keyword_set(GROUP_LEADER) then group_leader=0
if not keyword_set(S2) then s2=0

;START150504
if not keyword_set(ALIGN_LEFT) then align_left=0

repeat begin

    ;if keyword_set(GO_BACK) then begin
    ;    if keyword_set(EXIT) then begin
    ;        rtn = get_bool_list(choices,TITLE=title,/GO_BACK,/EXIT,ONE_COLUMN=one_column,BASE_TITLE=base_title,SPECIAL=special,$
    ;            GROUP_LEADER=group_leader,S2=s2)
    ;   endif else begin
    ;        rtn = get_bool_list(choices,TITLE=title,/GO_BACK,ONE_COLUMN=one_column,BASE_TITLE=base_title,SPECIAL=special,$
    ;            GROUP_LEADER=group_leader,S2=s2)
    ;    endelse
    ;endif else if keyword_set(EXIT) then begin
    ;    rtn = get_bool_list(choices,TITLE=title,/EXIT,ONE_COLUMN=one_column,BASE_TITLE=base_title,SPECIAL=special,$
    ;        GROUP_LEADER=group_leader,S2=s2)
    ;endif else begin
    ;    rtn = get_bool_list(choices,TITLE=title,ONE_COLUMN=one_column,BASE_TITLE=base_title,SPECIAL=special,$
    ;        GROUP_LEADER=group_leader,S2=s2)
    ;endelse
    ;START150504
    if keyword_set(GO_BACK) then begin
        if keyword_set(EXIT) then begin
            rtn = get_bool_list(choices,TITLE=title,/GO_BACK,/EXIT,ONE_COLUMN=one_column,BASE_TITLE=base_title,SPECIAL=special,$
                GROUP_LEADER=group_leader,S2=s2,ALIGN_LEFT=align_left)
       endif else begin
            rtn = get_bool_list(choices,TITLE=title,/GO_BACK,ONE_COLUMN=one_column,BASE_TITLE=base_title,SPECIAL=special,$
                GROUP_LEADER=group_leader,S2=s2,ALIGN_LEFT=align_left)
        endelse
    endif else if keyword_set(EXIT) then begin
        rtn = get_bool_list(choices,TITLE=title,/EXIT,ONE_COLUMN=one_column,BASE_TITLE=base_title,SPECIAL=special,$
            GROUP_LEADER=group_leader,S2=s2,ALIGN_LEFT=align_left)
    endif else begin
        rtn = get_bool_list(choices,TITLE=title,ONE_COLUMN=one_column,BASE_TITLE=base_title,SPECIAL=special,$
            GROUP_LEADER=group_leader,S2=s2,ALIGN_LEFT=align_left)
    endelse


    list=rtn.list
    rtnspecial=rtn.special
    rtns2=rtn.s2
    if list[0] eq -1 then begin
        rtn = {files:'GO_BACK'}
        return,rtn
    endif else if list[0] eq -2 then begin
        rtn = {files:'EXIT',count:0}
        return,rtn
    endif else if nfiles gt 1 and not keyword_set(NO_ALL) then begin
        if list[0] eq 1 then begin
            list[*] = 1
            list[0] = 0
        endif
    endif
    index = where(list eq 1,count)
    result = 'ignore'
    if count lt min_num_to_select then begin
        if keyword_set(CANCEL) then $
            result = dialog_message(errmsg,/CANCEL) $
        else $    
            result = dialog_message(errmsg)
    endif
endrep until count ge min_num_to_select or result eq 'Cancel'
if nfiles gt 1 and not keyword_set(NO_ALL) then index = index - 1
if nfiles gt 1 and not keyword_set(NO_ALL) then $
    list = list[1:nfiles] $
else $
    list = list[0:nfiles-1]
if count ne 0 then rtnfiles = files[index] else rtnfiles = 'OK'
rtn = {files:rtnfiles,count:count,index:index,list:list,special:rtnspecial,s2:rtns2}
return,rtn
end
