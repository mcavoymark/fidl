;Copyright 12/05/11 Washington University.  All Rights Reserved.
;difference_contrast.pro  $Revision: 1.4 $

;pro difference_contrast,help,contrast_labels,nc,c,cnorm,N,tot_eff
;START121217
pro difference_contrast,help,contrast_labels,nc,c,cnorm,N,tot_eff,effect_label

if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Difference contrasts are contrasts constructed by subtracting ' $
    +'two previously defined contrasts.'
if nc eq 0 then begin
    stat = dialog_message('No contrasts have been defined. Try "Default contrasts".',/INFORMATION)
    return
endif
idx1 = get_button(contrast_labels,TITLE='First contrast in difference (a in (a-b))')
idx2 = get_button(contrast_labels,TITLE='Second contrast in difference (b in (a-b))')
cc = fltarr(N,nc+1)
ccnorm = fltarr(tot_eff,nc+1)
labels = strarr(nc+1)
cc[*,0:nc-1] = c
ccnorm[*,0:nc-1] = cnorm
labels[0:nc-1] = contrast_labels
total1 = abs(total(cc[*,idx1]))
total2 = abs(total(cc[*,idx2]))
if abs(total1-total2) lt .1 then begin
    scalar1 = 1.
    scalar2 = 1.
endif else if total1 gt total2 then begin
    scalar1 = 1.
    scalar2 = total1/total2
endif else begin
    scalar1 = total2/total1
    scalar2 = 1.
endelse
cc[*,nc] = scalar1*cc[*,idx1] - scalar2*cc[*,idx2]
scrap = total(cc[*,nc])
if abs(scrap) gt 0.01 then begin
    stat = dialog_message('Contrast sums to '+strtrim(scrap,2)+string(10B)+'Contrast does not sum to zero. ' $
        +'It is not a real contrast. Abort!',/ERROR)
    return
endif
ccnorm[*,nc] = scalar1*ccnorm[*,idx1] - scalar2*ccnorm[*,idx2]

scrap = contrast_labels[idx2]
lizard1=strpos(scrap,'-')
lizard2=strpos(scrap,'+')
idx = where(scrap eq effect_label,cnt)
if cnt ne 0 and (lizard1[0] ne -1 or lizard2[0] ne -1) then begin
    labels[nc] = contrast_labels[idx1] + '_-_' + contrast_labels[idx2] 
endif else begin
    plus = strsplit(scrap,'+')
    minus = strsplit(scrap,'-')
    for i=0,n_elements(plus)-1 do if plus[i] ne 0 then strput,scrap,'-',plus[i]-1
    for i=0,n_elements(minus)-1 do if minus[i] ne 0 then strput,scrap,'+',minus[i]-1
    labels[nc] = contrast_labels[idx1] + '-' + scrap
endelse

c = cc
cnorm = ccnorm
contrast_labels = labels
nc = nc + 1
end
