;Copyright 12/29/00 Washington University.  All Rights Reserved.
;glm_metrics.pro  $Revision: 1.14 $
;******************************************
pro glm_metrics,glm,fi,dsp,wd,help,stc,pref
;******************************************

if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,glm_idx,'GLM',/GETGLMS) eq !ERROR then return

if(ptr_valid(glm[glm_idx].A)) then begin
    A = *glm[glm_idx].A
endif else begin
    print,'Design matrix is undefined.'
    return
endelse
    
nmain = glm[glm_idx].n_interest
widget_control,/HOURGLASS
AT = transpose(A)
widget_control,/HOURGLASS
ATA = transpose(A)#A
widget_control,/HOURGLASS
ATAm1 = float(pinv(ATA,0.))

sdbar = 0.
sd = fltarr(nmain)
for i=0,nmain-1 do begin
    sd[i] = sqrt(ATAm1[i,i])
    sdbar = sdbar + sd[i]
endfor
sdbar = sdbar/nmain
mse_sd = 0.
for i=0,nmain-1 do $
    mse_sd = mse_sd + (sd[i] - sdbar)^2
rmse_sd = sqrt(mse_sd)
ms_cc = 0.
for i=0,nmain-1 do begin
    for j=i+1,nmain-1 do begin
        ms_cc = ms_cc + ATAm1[i,j]^2/(ATAm1[i,i]*ATAm1[j,j])
    endfor
endfor
if nmain gt 1 then rms_cc = sqrt(2.*ms_cc/(nmain*(nmain-1)))

print,' '

;ADDED 070706
if strpos(fi.tails[glm_idx],'.glm') eq -1 then fi.tails[glm_idx]=fi.tails[glm_idx]+'.glm'

print,'File: ',fi.tails[glm_idx]
print,'Design metrics:'
print,'Mean (scaled) std dev: ',sdbar
if nmain gt 1 then begin
    print,'RMS deviation of scaled std dev: ',rmse_sd
    print,'RMS correlation coefficient: ',rms_cc
endif
print,''




;print,strcompress(string(cond(ATA,/DOUBLE),FORMAT='("Condition number:",g12.4)'))
;print,''
;xdim = n_elements(AT[*,0])
;ydim = n_elements(AT[0,*])
;count = intarr(xdim)
;for y=0,ydim-1 do for x=0,xdim-1 do if abs(AT[x,y]) gt 0 then count[x] = count[x] + 1
;effect_table = intarr(xdim)
;effect_label = *glm[glm_idx].effect_label
;effect_length = *glm[glm_idx].effect_length
;effect_column = *glm[glm_idx].effect_column
;k = 0
;for i=0,glm[glm_idx].all_eff-1 do begin
;    for j=0,effect_length[i]-1 do begin
;        effect_table[k] = i
;        k = k + 1
;    endfor
;endfor
;START110111
n_interest = glm[glm_idx].n_interest
t_valid = glm[glm_idx].t_valid
;constrain = 1./float(n_interest)
;print,'constrain=',constrain
;cnt = intarr(n_interest)
;for i=0,n_interest-1 do begin
;    index = where(A[*,i] ne 0.,count)
;    ;print,'i=',i,' count=',count,' t_valid=',t_valid
;    if count eq t_valid then begin
;        index1 = where(A[*,i] eq (1.-constrain),count1)
;        ;print,'    constrain count ',count1
;        if count1 ne 0 then count = count1
;    endif
;    cnt[i] = count
;endfor
;START110112
cnt = lonarr(n_interest)
stat=call_external(!SHARE_LIB,'_count_estimates',A,n_interest,t_valid,cnt,VALUE=[0,1,1,0])
if stat eq 0L then begin
    print,'Error in _count_estimates'
    return
endif





cnt = strtrim(cnt,2)
effect_label = *glm[glm_idx].effect_label
effect_length = *glm[glm_idx].effect_length

;START110113
;effect_column = *glm[glm_idx].effect_column
;effect_table = intarr(n_interest)
;k = 0
;for i=0,glm[glm_idx].tot_eff-1 do begin
;    for j=0,effect_length[i]-1 do begin
;        effect_table[k] = i
;        k = k + 1
;    endfor
;endfor

effect_label = strtrim(effect_label,2)
strlen_effect_label = strlen(effect_label)
maxlen = max(strlen_effect_label) > 6 
space = '                                                                      '
print,'effect'+strmid(space,0,maxlen-6)+'frame number of trials'
print,'------'+strmid(space,0,maxlen-6)+'----- ----------------'
k = 0
for i=0,glm[glm_idx].tot_eff-1 do begin
    for j=0,effect_length[i]-1 do begin
        frame_str = strtrim(j+1,2)

        ;print,effect_label[effect_table[x]],strmid(space,0,maxlen-strlen_effect_label[i]+2),frame_str, $
        ;    strmid(space,0,10-strlen(frame_str)),strtrim(count[x],2),FORMAT='(a,a,a,a,a)'
        ;START110111
        ;print,effect_label[effect_table[k]]+strmid(space,0,maxlen-strlen_effect_label[i]+2)+frame_str $
        ;    +strmid(space,0,10-strlen(frame_str))+cnt[k]
        ;START110113
        print,effect_label[i]+strmid(space,0,maxlen-strlen_effect_label[i]+2)+frame_str $
            +strmid(space,0,10-strlen(frame_str))+cnt[k]

        k = k + 1
    endfor
endfor

index = where(*glm[glm_idx].valid_frms eq 0,count)
if count ne 0 then begin
    print,''
    print,'number of invalid frames = ',count
    print,'---------------------------'
    print,'invalid frames = ',index+1
endif

print,''
print,'DONE'
end
