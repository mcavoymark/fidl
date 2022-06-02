;Copyright 10/16/15 Washington University.  All Rights Reserved.
;get_hdr_4dfp2.pro  $Revision: 1.4 $
function get_hdr_4dfp2,fi,st,dsp,wd,stc,help,glm,pref,title,GET_CONC=get_conc,FILTER=filter
no_all=1
labels = ['On disk','Exit']
get_filelist_labels,fi,ntails,tails,index,ntails_glm,tails_glm,index_glm,ntails_conc,tails_conc,index_conc
if keyword_set(GET_CONC) and n_elements(tails_conc) gt 0 then $  
    labels = [tails_conc,labels] $
else if n_elements(tails) gt 0 then begin 
    labels = [tails,labels]
    no_all=0
endif

;data_idx=[ ]
;START151105
;data_idx=!NULL
;START151120
data_idx=-1

sf=select_files(labels,TITLE=title,/EXIT,/ONE_COLUMN,NO_ALL=no_all)
if sf.files[0] eq 'EXIT' then return,rtn={msg:'EXIT'}

;START151120
idx=where(sf.index lt ntails,cnt)
if cnt ne 0 then data_idx=[data_idx,sf.index[idx]]


idx=where(sf.index eq ntails,cnt)
if cnt ne 0 then begin
    nfilesi0 = fi.nfiles
    widget_control,/HOURGLASS
    repeat begin
        if load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,FILTER=filter,/NOLOAD_COLOR,TITLE=title,/MULTIPLE_FILES) eq !OK then begin
            scrap = 1
            if !D.WINDOW eq -1 then dsp_image = update_image(fi,dsp,wd,stc,pref)
        endif else begin
            print,' *** Error loading file. ***'
            scrap = get_button(['Try again.','Exit'],TITLE='Error loading file.')
            if scrap eq 1 then return,!ERROR
        endelse
    endrep until scrap eq 1
    nspider = fi.nfiles - nfilesi0

    ;data_idx = indgen(nspider) + nfilesi0
    ;START151120
    data_idx=[data_idx,indgen(nspider)+nfilesi0]

endif

;START151120
;idx=where(sf.index lt ntails,cnt)
;if cnt ne 0 then data_idx=[data_idx,sf.index[idx]]
;return,rtn={msg:'OK',idx:data_idx}
;START151120
if n_elements(data_idx) gt 1 then rtn={msg:'OK',idx:data_idx[1:*]} else rtn={msg:'EXIT'}
return,rtn

end
