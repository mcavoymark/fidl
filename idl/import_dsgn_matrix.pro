;Copyright 12/31/99 Washington University.  All Rights Reserved.
;import_dsgn_matrix.pro  $Revision: 1.5 $

;************************************************
pro import_dsgn_matrix,fi,dsp,wd,glm,idx,help,stc
;************************************************

if(help.enable eq !TRUE) then begin
    widget_control,help.id,SET_VALUE=string('Select a text version of the design matrix.'+'  This should consist of a row of labels followed by a table of the same form as the design matrix.'+'  The table should be space-delimited (i.e., no tabs).'+'  This is the *.prn format in Excel.'+' You might have to edit the first row to make sure that the labels are separated by spaces.'+'  Excel sometimes runs the labels together, which will make this routine crash.')
endif
filename = dialog_pickfile(/READ,FILTER='*.prn',PATH=fi.path, $
                    GET_PATH=wrtpath,FILE=filename,TITLE='File containing design matrix.')
if(filename eq '') then begin
    stat=widget_message('*** Invalid file.  ***',/ERROR)
    return
endif
if(n_elements(filename) gt 1) then begin
    filename = filename(0) + filename(1)
endif

widget_control,/HOURGLASS

glmtxt = readf_ascii_file(filename)

xdim = glmtxt.NF
ydim = glmtxt.NR
eff_labs = glmtxt.hdr
A = transpose(float(glmtxt.data))

effect_label = strarr(xdim)
effect_length = intarr(xdim)
effect_column = intarr(xdim)
lcfunc = intarr(xdim)
all_eff = 0
x = 0
efflen = 0
efflabm1 = eff_labs[0]
effect_column[0] = 0
while(x lt xdim) do begin
    if(eff_labs[x] ne efflabm1) then begin
        all_eff = all_eff + 1
        effect_column[all_eff] = x
    endif
    effect_label[all_eff] = eff_labs[x]
    effect_length[all_eff] = effect_length[all_eff] + 1
    efflabm1 = eff_labs[x]
    x = x + 1
end
all_eff = all_eff + 1
diff_tot = glm[idx].all_eff - all_eff
tot_eff = glm[idx].tot_eff - diff_tot
n_interest = 0
for eff=0,tot_eff-1 do begin
    n_interest = n_interest + effect_length[eff]
    if(effect_length[eff] eq 1) then $
        lcfunc[eff] = !BOYNTON
endfor
effect_label = effect_label[0:all_eff-1]
effect_length = effect_length[0:all_eff-1]

ncol = 3
clab = strarr(ncol)
clab[0] = 'Stimulus delay'
clab[1] = 'Stimulus duration'
clab[2] = 'Regressor: 0=point,1=function)'
rlab = strarr(tot_eff+1)
for eff=0,tot_eff-1 do $
    rlab[eff] = effect_label[eff]

if(help.enable eq !TRUE) then begin
    widget_control,help.id,SET_VALUE=string('The entries in this table might be erroneous, since there is not enough information in the text design matrix to ensure their correctness.'+' The first two columns refer to parameters of the Boynton model: the delay and stimulus duration.'+'  These values are used in the "linear contrasts with delay" option.'+'  The third column designates whether the corresponding effect estimates the time-course (0) or the magnitude of an assumed function (1).')
endif
if(ptr_valid(glm[idx].delay)) then $
    delay = *glm[idx].delay $
else $
    delay = fltarr(tot_eff)
if(ptr_valid(glm[idx].stimlen)) then $
    stimlen = *glm[idx].stimlen $
else $
    stimlen = fltarr(tot_eff)
if(ptr_valid(glm[idx].lcfunc)) then $
    lcfunc = *glm[idx].lcfunc $
else $
    lcfunc = fltarr(tot_eff)
cstr = strarr(ncol,tot_eff)
cstr[0,*] = string(delay[0:tot_eff-1])
cstr[1,*] = string(stimlen[0:tot_eff-1])
cstr[2,*] = string(lcfunc[0:tot_eff-1])
tab = get_table(cstr,clab,rlab,TITLE='Update design info.',WIDTH=[4,4,6])
delay = float(tab[0,*])
stimlen = float(tab[1,*])
lcfunc = float(tab[2,*])

Ysim = fltarr(glm[idx].tdim,tot_eff)
glm[idx].nc = 0
ptr_free,glm[idx].Ysim
glm[idx].Ysim = ptr_new(Ysim)
ptr_free,glm[idx].delay
glm[idx].delay = ptr_new(delay)
ptr_free,glm[idx].stimlen
glm[idx].stimlen = ptr_new(stimlen)
ptr_free,glm[idx].c
ptr_free,glm[idx].cnorm
glm[idx].tot_eff = tot_eff
glm[idx].all_eff = all_eff
glm[idx].N = xdim
glm[idx].n_interest = n_interest
glm[idx].all_eff = all_eff
ptr_free,glm[idx].effect_length
glm[idx].effect_length = ptr_new(effect_length)
ptr_free,glm[idx].effect_column
glm[idx].effect_column = ptr_new(effect_column)
ptr_free,glm[idx].effect_label
glm[idx].effect_label = ptr_new(effect_label)
ptr_free,glm[idx].lcfunc
glm[idx].lcfunc = ptr_new(lcfunc)
ptr_free,glm[idx].A
glm[idx].A = ptr_new(A)
widget_control,HOURGLASS=0

x = *glm[idx].effect_label

return
end
