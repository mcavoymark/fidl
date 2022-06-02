;Copyright 12/14/01 Washington University.  All Rights Reserved.
;assign_cft_and_scft_new.pro  $Revision: 1.20 $
function assign_cft_and_scft_new,sumrows,search_str,sum_contrast_for_treatment,cft0,cft1,time, $
    scrap,scraplabels,scraplabels_index,choice,match,notpresent,RTN=rtn,GLM_NAME=glm_name
if not keyword_set(GLM_NAME) then glm_name = ''
if time eq 0 then str = 'Condition' else str = 'Contrast'
dummy = sumrows
scrapflag = 0
search_str = strcompress(search_str,/REMOVE_ALL)
scraplabels = strcompress(scraplabels,/REMOVE_ALL)

;print,'search_str=',search_str,'END
;print,'scraplabels=',scraplabels,'END
;print,'scrap=',scrap

msg = ['Is this condition present?','Are these conditions present?']
start = 0
if n_elements(choice) eq 0 then choice = 0 
if n_elements(match) eq 0 then match = 0 

while scrapflag lt dummy do begin
    find_one_flag = !FALSE 
    not_found = intarr(scrap)
    for n=start,scrap-1 do begin
        if match eq 0 then $
            j = where(strcmp(scraplabels,search_str[n]),count) $
        else $
            j = where(strmatch(scraplabels,search_str[n]),count)
        ;print,'scraplabels=',scraplabels
        ;print,'here n=',n,' search_str=',search_str[n],' count=',count,' j=',j
        if count gt 1 then begin
            stat=dialog_message('Error: '+search_str[n]+' found '+strtrim(string(count),2)+' times in '+glm_name $
                +string(10B)+'The first occurence will be used.',/ERROR,/CANCEL)
            if stat eq 'Cancel' then return,-1
        endif
        if count ge 1 then begin
            j = j[0]
            sum_contrast_for_treatment[cft0,cft1,scrapflag] = scraplabels_index[j] + 1
            scrapflag = scrapflag + 1
            find_one_flag = !TRUE
            if j lt n_elements(scraplabels)-1 then begin
                scrap_n = n_elements(scraplabels)
                scraplabels[j:scrap_n-2] = scraplabels[j+1:scrap_n-1]
                scraplabels = scraplabels[0:scrap_n-2]
                scraplabels_index[j:scrap_n-2] = scraplabels_index[j+1:scrap_n-1]
                scraplabels_index = scraplabels_index[0:scrap_n-2]
            endif
        endif else $
            not_found[n] = 1
    endfor

    if scrapflag lt dummy then begin
        index = where(not_found,count)
        scrapstr = strjoin(search_str[index],string(10B),/SINGLE)
        if sumrows eq 1 then scrap1=1 else scrap1=0


        ;if sumrows gt 1 or notpresent eq 1 then begin
        ;    if choice ne 2 then begin
        ;        choice = get_button(['Yes','No','Please do not ask me again.'+string(10B) $
        ;             +'I have used consistent names and all missing conditions are not present.','My GLMs are a mess. ' $
        ;             +'Please let me do this manually.'],TITLE=scrapstr+string(10B)+'not found.'+string(10B)+string(10B) $
        ;             +msg[(count-1) < 1],BASE_TITLE=glm_name,/WIDTH)
        ;    endif
        ;    if choice eq 1 or choice eq 2 then dummy = dummy - count else if choice ne 0 then return,-1
        ;endif else begin
        ;    if choice ne 2 then begin
        ;        choice = get_button(['Yes','No','My GLMs are a mess. Please let me do this manually.'],TITLE=scrapstr $
        ;             +string(10B)+'not found.'+string(10B)+string(10B)+msg[(count-1) < 1],BASE_TITLE=glm_name,/WIDTH)
        ;    endif
        ;    if choice eq 1 then begin
        ;        stat=dialog_message('Conditions not present will result in cells with no data. This is no good. Abort!',/ERROR)
        ;        return,-2
        ;    endif else if choice ne 0 then $
        ;        return,-1
        ;endelse
        ;START181108
        if choice ne 2 then begin
            choice = get_button(['Yes','No','Please do not ask me again.'+string(10B) $
                 +'I have used consistent names and all missing conditions are not present.','My GLMs are a mess. ' $
                 +'Please let me do this manually.'],TITLE=scrapstr+string(10B)+'not found.'+string(10B)+string(10B) $
                 +msg[(count-1) < 1],BASE_TITLE=glm_name,/WIDTH)
        endif
        if choice eq 1 or choice eq 2 then dummy = dummy - count else if choice ne 0 then return,-1


        if choice eq 0 then begin
            title = 'Please select an alternate name. ' + str + ' not found. '
            scrapstr = strarr(scrap)
            for n=0,scrap-1 do scrapstr[n] = search_str[n]
            scrapstr = match_files(scrapstr,scraplabels,TITLE=title,BASE_TITLE=glm_name,SUMROWS=scrap1)
            scrapcount = n_elements(scrapstr)
            search_str[scrap:scrap+scrapcount-1] = scrapstr
            start = scrap
            scrap = scrap + scrapcount
        endif
    endif
endwhile
return,scrap
end
