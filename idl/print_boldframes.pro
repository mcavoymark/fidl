;Copyright 5/1/03 Washington University.  All Rights Reserved.
;print_boldframes.pro  $Revision: 1.38 $
function print_boldframes,nframes,sumrows_time,index_time,trialframes,boldframes,bf_str,index_which_level,boldconditions,boldrun, $
    trialframes_in,filenames,treatment_str,tdim_sum_new,LOGREG=logreg,CC=cc,PAIRS=pairs,FANO=fano,MVPA=mvpa,APPEND=append
if not keyword_set(APPEND) then append=0
if not keyword_set(mvpa) then mvpa=0
if n_elements(index_which_level) eq 0 then index_which_level = intarr(n_elements(trialframes))
time = index_time + 1

;print,'nframes=',nframes
;print,'sumrows_time=',sumrows_time
;print,'size(sumrows_time,/N_DIMENSIONS)=',size(sumrows_time,/N_DIMENSIONS)
;print,'index_time=',index_time
;print,'size(index_time)=',size(index_time)
;print,'trialframes=',trialframes
;print,'boldframes=',boldframes
;print,'index_which_level=',index_which_level
;if n_elements(boldcondtions) gt 0 then print,boldconditions 
;print,'time=',time

startloop = 0
loop = n_elements(nframes)
nframes_temp = nframes

;min_tc = make_array(loop,/INT,VALUE=max(index_time))
;if size(index_time,/N_DIMENSIONS) le 2 then begin
;    for i=0,nframes[0]-1 do begin
;        minimum = min(index_time[i,0:sumrows_time[i]-1])
;        if minimum lt min_tc[0] then min_tc[0] = minimum
;    endfor
;endif else begin
;    for k=0,loop-1 do begin
;        for i=0,nframes[k]-1 do begin
;            minimum = min(index_time[k,i,0:sumrows_time[k,i]-1])
;            if minimum lt min_tc[k] then min_tc[k] = minimum
;        endfor
;    endfor
;endelse
;START151116
if n_elements(sumrows_time) ne 0 then begin
    min_tc = make_array(loop,/INT,VALUE=max(index_time))
    if size(index_time,/N_DIMENSIONS) le 2 then begin
        for i=0,nframes[0]-1 do begin
            minimum = min(index_time[i,0:sumrows_time[i]-1])
            if minimum lt min_tc[0] then min_tc[0] = minimum
        endfor
    endif else begin
        for k=0,loop-1 do begin
            for i=0,nframes[k]-1 do begin
                minimum = min(index_time[k,i,0:sumrows_time[k,i]-1])
                if minimum lt min_tc[k] then min_tc[k] = minimum
            endfor
        endfor
    endelse
endif else $
    min_tc = make_array(loop,/INT,VALUE=1)


if keyword_set(LOGREG) then $
    dummy = nframes $
else if keyword_set(CC) then begin
    if keyword_set(PAIRS) then $
        dummy = nframes $
    else begin
        loop = 1
        min_tc = min(min_tc)
        nframes_temp = max(time)
        dummy = max(time) - min_tc
    endelse
endif else if keyword_set(FANO) then begin
    dummy = nframes 
    loop = 1
endif else $
    dummy = make_array(loop,/INTEGER,VALUE=1)    
 

;print,'min_tc=',min_tc
;print,'nframes=',nframes
;print,'dummy=',dummy

index = -1
if not keyword_set(MVPA) then begin
    trialframes_in = trialframes
    count = 0
    s2 = -1
    for i=startloop,loop-1 do begin
        scrap = where(index_which_level eq i,scrapcount)
        if scrapcount ne 0 then begin
            trialframes[scrap] = (trialframes[scrap] - min_tc[i]) < nframes_temp[i]
            scrap2 = where(trialframes[scrap] ge dummy[i],scrapcount)
            if scrapcount ne 0 then begin
                index = [index,scrap[scrap2]]
                count = count + scrapcount
                s2 = [s2,scrap2]
            endif
        endif
    endfor
    index = index[1:*]
    index = index[sort(index)]
    s2 = s2[1:*]
    s2 = s2[sort(s2)]

    ;print,'index=',index
    ;print,'size(index)=',size(index)

endif else begin 
    count = n_elements(trialframes)
endelse

;print,'trialframes_in'
;print,trialframes_in
;print,'trialframes'
;print,trialframes


if keyword_set(PAIRS) then begin
    count = count - 1
    indexnew = -1
    countnew = 0
    i = 0
    repeat begin
        if s2[i] eq s2[i+1] then begin
            indexnew = [indexnew,index[i],index[i+1]]
            countnew = countnew + 2
            i = i + 2
        endif else begin
            i = i + 1
        endelse
    endrep until i ge count
    ;print,'i=',i,' count=',count
    ;print,'indexnew=',indexnew
    ;print,'countnew=',countnew
    index = indexnew[1:*]
    count = countnew
endif
boldconditionsrtn = -1 
if count ne 0 then begin
    if not keyword_set(MVPA) then begin
        trialframes = trialframes[index]

        ;print,'after trialframes'
        ;print,trialframes
        ;print,'index=',index

        boldframes = boldframes[index]
        boldruni = boldrun[index]-1
        index_which_level = index_which_level[index]
        trialframes_in = trialframes_in[index]
        if n_elements(boldconditions) ne 0 then boldconditions_str = trim(boldconditions[index])
    endif else begin 
        boldconditions_str = trim(boldconditions)
        boldruni = boldrun-1
    endelse
    openw,lu_csh,bf_str,/GET_LUN,APPEND=append
    if not keyword_set(CC) then begin
        if n_elements(boldconditions) eq 0 then goose = make_array(count,/STRING,VALUE='') else goose = boldconditions_str + ', '
        if size(index_time,/N_DIMENSIONS) le 2 then begin
            for m=0,count-1 do begin
                str = goose[m] 
                for k=0,trialframes[m]-1 do begin
                    str = str + ' ' + strcompress(boldframes[m]+time[k,0],/REMOVE_ALL)
                    for r=1,sumrows_time[k]-1 do str = str+'+'+strcompress(boldframes[m]+time[k,r],/REMOVE_ALL)
                endfor
                printf,lu_csh,str
            endfor
        endif else begin

            ;print,'trialframes=',trialframes
            ;print,'size(trialframes)=',size(trialframes)
            ;print,'trialframes_in=',trialframes_in
            ;print,'size(trialframes_in)=',size(trialframes_in)
            ;print,'boldframes=',boldframes
            ;print,'size(boldframes)=',size(boldframes)
            ;print,'boldruni=',boldruni
            ;print,'size(boldruni)=',size(boldruni)
            ;print,'time=',time
            ;print,'size(time)=',size(time)
            ;print,'tdim_sum_new=',tdim_sum_new
            ;print,'size(tdim_sum_new)=',size(tdim_sum_new)
            ;print,'index_which_level=',index_which_level
            ;print,'size(index_which_level)=',size(index_which_level)

            if not keyword_set(MVPA) then begin

                ;for m=0,count-1 do begin
                ;    str = ''
                ;    for k=0,trialframes[m]-1 do begin
                ;        ;print,'m=',m,' index_which_level[m]=',index_which_level[m],' k=',k,' time[index_which_level[m],k,0]=', $
                ;        ;    time[index_which_level[m],k,0],' sumrows_time[index_which_level[m],k]=', $
                ;        ;    sumrows_time[index_which_level[m],k],' trialframes_in[m]=',trialframes_in[m],' boldframes[m]=', $
                ;        ;    boldframes[m]
                ;        penguin = boldframes[m]+time[index_which_level[m],k,0]
                ;        if penguin le tdim_sum_new[boldruni[m]] then begin
                ;            str = str + ' ' + trim(penguin+append) 
                ;            for r=1,(sumrows_time[index_which_level[m],k]<trialframes_in[m])-1 do begin 
                ;                penguin = boldframes[m]+time[index_which_level[m],k,r]
                ;                if penguin le tdim_sum_new[boldruni[m]] then str=str+'+'+trim(penguin+append)
                ;            endfor
                ;        endif
                ;    endfor
                ;    if str ne '' then begin
                ;        str = goose[m] + str 
                ;        printf,lu_csh,str
                ;    endif
                ;endfor
                ;START151116
                if n_elements(sumrows_time) ne 0 then begin
                    for m=0,count-1 do begin
                        str = ''
                        for k=0,trialframes[m]-1 do begin
                            ;print,'m=',m,' index_which_level[m]=',index_which_level[m],' k=',k,' time[index_which_level[m],k,0]=', $
                            ;    time[index_which_level[m],k,0],' sumrows_time[index_which_level[m],k]=', $
                            ;    sumrows_time[index_which_level[m],k],' trialframes_in[m]=',trialframes_in[m],' boldframes[m]=', $
                            ;    boldframes[m]
                            penguin = boldframes[m]+time[index_which_level[m],k,0]
                            if penguin le tdim_sum_new[boldruni[m]] then begin
                                str = str + ' ' + trim(penguin+append)
                                for r=1,(sumrows_time[index_which_level[m],k]<trialframes_in[m])-1 do begin
                                    penguin = boldframes[m]+time[index_which_level[m],k,r]
                                    if penguin le tdim_sum_new[boldruni[m]] then str=str+'+'+trim(penguin+append)
                                endfor
                            endif
                        endfor
                        if str ne '' then begin
                            str = goose[m] + str
                            printf,lu_csh,str
                        endif
                    endfor
                endif else begin
                    for m=0,count-1 do begin
                        str = ''
                        for k=0,trialframes[m]-1 do begin
                            ;print,'m=',m,' index_which_level[m]=',index_which_level[m],' k=',k,' time[index_which_level[m],k,0]=', $
                            ;    time[index_which_level[m],k,0],' sumrows_time[index_which_level[m],k]=', $
                            ;    sumrows_time[index_which_level[m],k],' trialframes_in[m]=',trialframes_in[m],' boldframes[m]=', $
                            ;    boldframes[m]
                            penguin = boldframes[m]+time[index_which_level[m],k,0]
                            if penguin le tdim_sum_new[boldruni[m]] then begin
                                str = str + ' ' + trim(penguin+append)
                                for r=1,trialframes_in[m]-1 do begin
                                    penguin = boldframes[m]+time[index_which_level[m],k,r]
                                    if penguin le tdim_sum_new[boldruni[m]] then str=str+'+'+trim(penguin+append)
                                endfor
                            endif
                        endfor
                        if str ne '' then begin
                            str = goose[m] + str
                            printf,lu_csh,str
                        endif
                    endfor
                endelse


            endif else begin
                mvpa_str = trim(mvpa)
                session_str = mvpa_str[0] 
                if n_elements(mvpa) eq 1 then subject_str='1' else subject_str=mvpa_str[1]
                if n_elements(mvpa) le 2 then boldrun_str = trim(boldrun) else boldrun_str = trim(boldrun+mvpa[2])
                if not keyword_set(APPEND) then printf,lu_csh,'subject class run session 4dfp condition frames'
                for m=0,count-1 do begin
                    str = subject_str + ' ' + treatment_str[index_which_level[m]] + ' ' + boldrun_str[m] + ' ' + session_str
                    if n_elements(filenames) ne 0 then str = str+' '+filenames[m]
                    str = str + ' ' + boldconditions_str[m]
                    for k=0,trialframes[m]-1 do begin
                        penguin = boldframes[m]+time[index_which_level[m],k,0]
                        if penguin le tdim_sum_new[boldruni[m]] then begin
                            str = str + ' ' + strcompress(penguin,/REMOVE_ALL)
                            for r=1,(sumrows_time[index_which_level[m],k]<trialframes_in[m])-1 do begin
                                penguin = boldframes[m]+time[index_which_level[m],k,r]
                                if penguin le tdim_sum_new[boldruni[m]] then str=str+'+'+strcompress(penguin,/REMOVE_ALL)
                            endfor
                        endif
                    endfor
                    printf,lu_csh,str
                endfor
            endelse
        endelse
    endif else begin
        ;print,'trialframes=',trialframes
        ;print,'boldframes=',boldframes
        ;print,'nframes=',nframes
        ;print,'count=',count
        index_which_level = index_which_level[index]
        if not keyword_set(PAIRS) then begin
            for m=0,count-1 do begin
                str = ''
                for k=0,nframes[0]-1 do begin
                    str = str + ' ' + strcompress(boldframes[m]+time[0,k,0],/REMOVE_ALL)
                    for r=1,sumrows_time[0,k]-1 do str = str+'+'+strcompress(boldframes[m]+time[0,k,r],/REMOVE_ALL)
                endfor
                for k=0,nframes[1]-1 do begin
                    str = str + ' ' + strcompress(boldframes[m]+time[1,k,0],/REMOVE_ALL)
                    for r=1,sumrows_time[1,k]-1 do str = str+'+'+strcompress(boldframes[m]+time[1,k,r],/REMOVE_ALL)
                endfor
                printf,lu_csh,str
            endfor
        endif else begin
            for m=0,count-2,2 do begin
                str = ''
                for k=0,nframes[0]-1 do begin
                    str = str + ' ' + strcompress(boldframes[m]+time[0,k,0],/REMOVE_ALL)
                    for r=1,sumrows_time[0,k]-1 do str = str+'+'+strcompress(boldframes[m]+time[0,k,r],/REMOVE_ALL)
                endfor
                for k=0,nframes[1]-1 do begin
                    str = str + ' ' + strcompress(boldframes[m+1]+time[1,k,0],/REMOVE_ALL)
                    for r=1,sumrows_time[1,k]-1 do str = str+'+'+strcompress(boldframes[m+1]+time[1,k,r],/REMOVE_ALL)
                endfor
                printf,lu_csh,str
            endfor
        endelse
    endelse
    close,lu_csh
    free_lun,lu_csh
endif
return,rtn={index:index,count:count}
end
