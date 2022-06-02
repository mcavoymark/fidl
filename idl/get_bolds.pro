;Copyright 2/19/02 Washington University.  All Rights Reserved.
;get_bolds.pro  $Revision: 1.20 $
function get_bolds,fi,dsp,wd,glm,help,stc,FILNUM=filnum,FILTER=filter,TITLE=title,MULTIPLE_FILES=multiple_files
if not keyword_set(FILTER) then filter = '*.conc'
if not keyword_set(TITLE) then title = 'Please select data to be processed.'
if not keyword_set(MULTIPLE_FILES) then multiple_files=0 
if keyword_set(FILNUM) then $
    data_idx = filnum - 1 $
else begin
    n = 0
    labels = ['On disk','Exit']
    get_filelist_labels,fi,nlabels_4dfp,labels_4dfp,index_4dfp,nlabels_glm,labels_glm,index_glm,nlabels_conc,labels_conc,index_conc
    if filter ne '*.glm' then begin
        if nlabels_conc ne 0 then begin
            labels = [labels_conc,labels]
            n = n + nlabels_conc
        endif
    endif else begin
        if nlabels_glm ne 0 then begin
            labels = [labels_glm,labels]
            n = n + nlabels_glm
        endif
    endelse
    idx = get_button(labels,TITLE=title)
    if idx eq n+1 then begin 
        return,-1
    endif else if idx eq n then begin 
        widget_control,/HOURGLASS
        nfilesi0 = fi.nfiles
        if filter ne '*.glm' then begin
            if load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,FILTER=filter,TITLE='Please select file. 4DFP or conc. ' $
                +'Just change the filter.',/NOLOAD_COLOR,MULTIPLE_FILES=multiple_files) ne !OK then begin
                stat = dialog_message('We were unable to load the file!',/ERROR)
                return,-1
            endif
        endif else begin
            rtn = load_linmod(fi,dsp,wd,glm,help,ifh)
            if rtn.filename eq 'EXIT' or rtn.filename eq 'GO_BACK' then return,-1
        endelse
        nspider = fi.nfiles - nfilesi0
        data_idx = indgen(nspider) + nfilesi0 
    endif else begin 
        if filter ne '*.glm' then data_idx = index_conc[idx] else data_idx = index_glm[idx]
    endelse
endelse
return,data_idx
end
