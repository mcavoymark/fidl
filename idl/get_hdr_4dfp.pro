;Copyright 8/21/02 Washington University.  All Rights Reserved.
;get_hdr_4dfp.pro  $Revision: 1.29 $
function get_hdr_4dfp,fi,st,dsp,wd,stc,help,glm,pref,hdr,idx,image_type_str,GETGLMS=getglms,GET_CONC=get_conc,TITLE=title, $
    DONT_PUT_IMAGE=dont_put_image,FILTER=filter
if not keyword_set(DONT_PUT_IMAGE) then dont_put_image = 0
labels = ['On disk','Exit']
get_filelist_labels,fi,ntails,tails,index,ntails_glm,tails_glm,index_glm,ntails_conc,tails_conc,index_conc
if not keyword_set(GETGLMS) then begin
    if keyword_set(GET_CONC) and n_elements(tails_conc) gt 0 then $  
        labels = [tails_conc,labels] $
    else if n_elements(tails) gt 0 then $
        labels = [tails,labels]
    idx = get_button(labels,TITLE='Please select '+image_type_str+'.',/ONE_COLUMN)
    if idx lt ntails then $
        idx = index[idx] $
    else if idx eq ntails then begin
        if not keyword_set(TITLE) then title = 'Change filter to load a conc.'
        repeat begin
            stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,/NOLOAD_COLOR,TITLE=title,DONT_PUT_IMAGE=dont_put_image)
            if stat eq !OK then begin
                idx = fi.n
                scrap = 1
                if !D.WINDOW eq -1 then dsp_image = update_image(fi,dsp,wd,stc,pref)
            endif else if stat eq !ERROR then begin
                return,!ERROR
            endif else begin
                print,' *** Error while loading file. ***'
                scrap = get_button(['Try again.','Exit'],TITLE='Error loading '+image_type_str+'.')
                if scrap eq 1 then return,!ERROR
            endelse
        endrep until scrap eq 1
    endif else begin 
        return,!ERROR
    endelse

    hdr = *fi.hdr_ptr[idx]
    return,!OK
endif else begin
    if getglms eq 1 then begin
        if ntails_glm ne 0 then labels = [tails_glm,labels]
        idx = get_button(labels,TITLE='Please select '+image_type_str+'.')
        if idx lt ntails_glm then $
            idx = index_glm[idx] $
        else if idx eq ntails_glm then begin

            ;glm_file = load_linmod(fi,dsp,wd,glm,help,ifh)
            ;if glm_file eq 'EXIT' or glm_file eq 'GO_BACK' then return,!ERROR
            ;START54
            rtn_glm = load_linmod(fi,dsp,wd,glm,help,ifh)
            glm_file = rtn_glm.filename
            if glm_file eq 'EXIT' or glm_file eq 'GO_BACK' then return,!ERROR


            idx = fi.n
        endif else $ 
            return,!ERROR
        hdr = *fi.hdr_ptr[idx]
        return,!OK
    endif else begin
        nfilenames = 0
        filenames = ''
        if ntails_glm ne 0 then begin
            labels = [tails_glm,labels[0]]
            if ntails_glm lt 2 then no_all=1 else no_all=0
            rtn = select_files(labels,TITLE='Please select GLMs',/CANCEL,/EXIT,/ONE_COLUMN,/GO_BACK,NO_ALL=no_all)
            ;if rtn.files[0] eq 'GO_BACK' or rtn.files[0] eq 'EXIT' then return,rtn={nfilenames:0}
            if rtn.files[0] eq 'GO_BACK' or rtn.files[0] eq 'EXIT' then return,rtn={msg:rtn.files[0]}
            index = where(rtn.index lt ntails_glm,count)
            if count ne 0 then filenames = [filenames,fi.names[rtn.index[index]]]
            nfilenames = nfilenames + count
            ;if rtn.list[ntails_glm] eq 0 then return,rtn={nfilenames:nfilenames,filenames:filenames[1:*]}
            if rtn.list[ntails_glm] eq 0 then return,rtn={msg:'OK',nfilenames:nfilenames,filenames:filenames[1:*]}
        endif
        scrap=get_glms()
        if scrap.msg eq 'GO_BACK' or scrap.msg eq 'EXIT' then begin
            ;return,rtn={msg:scrap.msg,nfilenames:nfilenames,filenames:filenames[1:*]}
            return,rtn={msg:scrap.msg}
        endif
        nfilenames = nfilenames + scrap.total_nsubjects
        filenames = [filenames,scrap.glmfiles]
        return,rtn={msg:scrap.msg,nfilenames:nfilenames,filenames:filenames[1:*],load:scrap.load,nlists:scrap.nlists, $
            t4select:scrap.t4select,glm_list_str:scrap.glm_list} 
    endelse
endelse
;print,'get_hdr_4dfp bottom'
end
