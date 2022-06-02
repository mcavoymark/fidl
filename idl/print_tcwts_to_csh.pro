;Copyright 12/21/12 Washington University.  All Rights Reserved.
;print_tcwts_to_csh.pro  $Revision: 1.3 $
pro print_tcwts_to_csh,lu_csh,ntreatments0,nframes_in,nfiles,startj,conditions_time,sumrows_time, $
    sum_contrast_for_treatment,effect_length_ts,ntests,behav_data1,indvar
if n_elements(nframes_in) eq 1 then begin
    nframes = intarr(ntreatments0)
    nframes[*] = nframes_in 
endif else $
    nframes = nframes_in
if n_elements(ntests) eq 0 then ntests = ntreatments0

;print,'ntreatments0=',ntreatments0
;print,'conditions_time=',conditions_time
;print,'sumrows_time=',sumrows_time
;print,'size(conditions_time)=',size(conditions_time)
;print,'size(sumrows_time)=',size(sumrows_time)
;print,'nframes=',nframes

count_tests = 0
for m=0,ntreatments0-1 do begin
    for k=0,nframes[m]-1 do begin
        if nframes[m] eq 1 then p = m else p = k ;THIS MAY NOT WORK FOR compute_avg_zstat.pro
        ;print,'here0 m=',m,' k=',k,' p=',p

        ndst = size(sumrows_time,/N_DIMENSIONS)
        if n_elements(sumrows_time) eq 1 then $
            scrap2 = sumrows_time[0] $
        else if ndst eq 1 then $
            scrap2 = sumrows_time[p] $
        else $
            scrap2 = sumrows_time[m,k]
        sct = size(conditions_time)
        ;print,'sct=',sct
        ;print,'p=',p
        ;print,'scrap2=',scrap2

        if n_elements(conditions_time) eq 1 then $
            scrap = conditions_time[0] $
        else if sct[0] eq 1 then $
            scrap = conditions_time[p] $
        else if sct[0] eq 2 then begin
            if sct[1] eq 1 then begin
                if scrap2 eq 1 then scrap = conditions_time[0] else scrap = conditions_time[*]
            endif else $
                scrap = conditions_time[p,*]
        endif else $
            scrap = conditions_time[m,k,*]

        j = startj
        tcstr = ''
        for i=0,nfiles-1 do begin

            dummy=''
            scrapchar=''
            index = where(sum_contrast_for_treatment[j,m,*] ne 0,count)
            for q=0,count-1 do begin
                if n_elements(effect_length_ts[j,*]) gt 0 then $
                    goose = scrap2 < effect_length_ts[j,sum_contrast_for_treatment[q]-1] $
                else $
                    goose = scrap2 

                ;dummy = dummy + scrapchar + strjoin(make_array(goose,/STRING,VALUE=behav_data1[j,indvar[m,q]]),'+')
                ;START130523
                dummy = dummy + scrapchar + strjoin(make_array(goose,/STRING,VALUE=trim(float(behav_data1[j,indvar[m,q]]))),'+')

                scrapchar = '+'
            endfor
            if dummy eq '' then dummy='0'
            tcstr = tcstr + dummy

            if i ne nfiles-1 then tcstr = tcstr + ','
            if i eq nfiles-1 and k eq nframes[m]-1 then count_tests = count_tests + 1
            if strlen(tcstr) gt 100 and i ne nfiles-1 then begin
                printf,lu_csh,'        '+tcstr+'\'
                tcstr = ''
            endif
            j = j + 1
        endfor
        if tcstr ne '' then begin
            ;print,'tcstr=',tcstr,'END'
            ;print,'count_tests=',count_tests,' ntests=',ntests
            if count_tests ne ntests then $
                printf,lu_csh,'        '+tcstr+' \' $
            else begin
                printf,lu_csh,'        '+tcstr+')'
                goto,jump
            endelse
        endif
    endfor
endfor
jump:
;print,'print_tc_to_csh bottom'
end
