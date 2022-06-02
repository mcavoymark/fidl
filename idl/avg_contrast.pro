;Copyright 12/05/11 Washington University.  All Rights Reserved.
;avg_contrast.pro  $Revision: 1.2 $
pro avg_contrast,help,contrast_labels,nc,c,cnorm,N,tot_eff
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Average contrasts are contrasts constructed by averaging two or ' $
   +'more previously defined contrasts.'
if nc eq 0 then begin
    stat = dialog_message('No contrasts have been defined. Try "Default contrasts".',/INFORMATION)
    return
endif
labels = strarr(nc)
nlab = 0
for i=0,nc-1 do if i lt tot_eff then labels[i] = contrast_labels[i] else labels[i] = string(i+1,FORMAT='("c",i1)')
if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Specify contrasts to be summed then click OK.'
rtn = get_bool_list(contrast_labels[0:nc-1],TITLE='Specify contrasts to average')
list = rtn.list
index = where(list eq 1,count)
if count eq 0 then return
cc = fltarr(N,nc+1)
ccnorm = fltarr(tot_eff,nc+1)
cc[*,0:nc-1] = c
ccnorm[*,0:nc-1] = cnorm[*,0:nc-1]
llabels = strarr(nc+1)
llabels[0:nc-1] = contrast_labels
scalar = 1./float(count)
cc[*,nc] = total(scalar*cc[*,index],2)
ccnorm[*,nc] = total(scalar*ccnorm[*,index],2)
scrap = strarr(count)
for i=0,count-1 do if stregex(strmid(llabels[index[i]],0,1),'[0-9]+',/BOOLEAN) then scrap[i]='_'
llabels[nc] = strjoin(strtrim(string(1./float(count),FORMAT='(g0)'),2)+scrap+llabels[index],'+',/SINGLE)
c = cc
cnorm = ccnorm
contrast_labels = strcompress(llabels,/REMOVE_ALL)
nc = nc + 1
end
