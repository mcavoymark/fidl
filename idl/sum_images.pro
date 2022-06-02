;Copyright 12/31/99 Washington University.  All Rights Reserved.
;sum_images.pro  $Revision: 12.95 $
pro sum_images,fi,wd,dsp,help,stc,pref
lcz = get_button(['Z-score mean','Simple mean','Sum'],TITLE='Select operation')
ntails=0
scrap=0
repeat begin
    get_filelist_labels,fi,ntails,tails,idx,ntails_glm,tails_glm,index_glm,ntails_conc,tails_conc,index_conc
    if ntails eq 0 then begin
        repeat begin
            stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,/NOLOAD_COLOR,/MULTIPLE_FILES)
            if stat eq !OK then begin
                scrap = 1
                if !D.WINDOW eq -1 then dsp_image = update_image(fi,dsp,wd,stc,pref)
            endif else if stat eq !ERROR then begin
                return
            endif else begin
                print,' *** Error while loading file. ***'
                scrap = get_button(['Try again.','Exit'],TITLE='Error loading '+image_type_str+'.')
                if scrap eq 1 then return
            endelse
        endrep until scrap eq 1
    endif
endrep until ntails ne 0
goback0:
if scrap eq 0 then begin
    rtn = select_files(tails,TITLE='Images to be summed.',/EXIT,/ONE_COLUMN)
    if rtn.files[0] eq 'EXIT' then return
    nfiles = rtn.count
    idx = rtn.index
endif else $
    nfiles = ntails
hdr = *fi.hdr_ptr[idx[0]]
name = fi.tails[idx[0]]
for i=1,nfiles-1 do begin
    hdr1 = *fi.hdr_ptr[idx[i]]
    if(hdr1.xdim ne hdr.xdim or hdr1.ydim ne hdr.ydim or hdr1.zdim ne hdr.zdim) then begin
        stat = dialog_message('All images must have the same dimensions.',/ERROR)
        return
    endif
endfor



if hdr.tdim gt 1 then $
    action = get_button(['Sum over files','Sum over frames','Sum over files and frames'],TITLE='Select action') $
else $
    action = 0
case action of
    0: begin
        if nfiles lt 2 then begin
            print,'Not enough files specified.'
            return
        endif else begin
            t0 = 0
            t1 = hdr.tdim - 1
            lc_frames = !FALSE
        endelse
    end
    1: begin
        if nfiles gt 1 then begin
            print,'Too many files specified.'
            return
        endif else begin
            t0 = 0
            t1 = hdr.tdim - 1
            lc_frames = !TRUE
        endelse
    end
    2: begin
        if nfiles lt 2 then begin
            print,'Not enough files specified.'
            return
        endif else begin
            t0 = 0
            t1 = hdr.tdim - 1
            lc_frames = !TRUE
        endelse
    end
    else: print,'Invalid value of action.'
endcase
case lcz of
    0: begin
        name = name + '_meanz'
        name = get_str(1,'Output',name)
       end
    1: begin 
        name = name + '_mean'
        name = get_str(1,'Output',name)
       end

    ;2: name = name + '+' + fi.tails(idx[1])
    ;START110920
    2: if nfiles eq 2 then name = name + '+' + fi.tails(idx[1]) else name='sum_'+name 

    else: print,'Invalid value of lcz in sum_images.'
endcase
if lc_frames eq !TRUE then begin
    if hdr.tdim gt 1 then begin
        lcfrm = get_boolean('Sum over frames?',['Yes','No'])
        if lcfrm eq !TRUE then begin
            str = get_str(2,['First frame','Last frame'],[1,hdr.tdim])
            t0 = long(str[0]) - 1
            t1 = long(str[1]) - 1
        endif
        nfiles = nfiles*(t1 - t0 + 1)
    endif
endif
first = !TRUE
widget_control,/HOURGLASS

;if lc_frames eq !TRUE then image=fltarr(hdr.xdim,hdr.ydim,hdr.zdim) else image=fltarr(hdr.xdim,hdr.ydim,hdr.zdim,t1-t0+1)
;for ifile=0,n_elements(lclist)-1 do begin
;    if(lclist[ifile] ne 0) then begin
;         hdr1 = *fi.hdr_ptr(ifile)
;         if(hdr1.xdim ne hdr.xdim or hdr1.ydim ne hdr.ydim or hdr1.zdim ne hdr.zdim) then begin
;             stat = widget_message('All images must have the same dimensions.')
;             return
;         endif
;         for t=t0,t1 do begin
;             for z=0,hdr.zdim-1 do $
;                 if(lc_frames eq !TRUE) then $
;                     image[*,*,z] = image[*,*,z] + get_image(z+hdr.zdim*t,fi,stc,FILNUM=ifile+1) $
;                 else $
;                     image[*,*,z,t] = image[*,*,z,t] + get_image(z+hdr.zdim*t,fi,stc,FILNUM=ifile+1)
;         endfor
;    endif
;endfor
;START110920
if lc_frames eq !TRUE then begin
    image=fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
    for i=0,nfiles-1 do begin
        hdr = *fi.hdr_ptr[idx[i]]
        for t=t0,t1 do for z=0,hdr.zdim-1 do image[*,*,z] = image[*,*,z] + get_image(z+hdr.zdim*t,fi,stc,FILNUM=idx[i]+1)
    endfor
endif else begin
    image=fltarr(hdr.xdim,hdr.ydim,hdr.zdim,t1-t0+1)
    for i=0,nfiles-1 do begin
        hdr = *fi.hdr_ptr[idx[i]]
        for t=t0,t1 do for z=0,hdr.zdim-1 do image[*,*,z,t] = image[*,*,z,t] + get_image(z+hdr.zdim*t,fi,stc,FILNUM=idx[i]+1)
    endfor
endelse


tdim = t1 - t0 + 1
if lc_frames eq !FALSE then image = reform(image,hdr.xdim,hdr.ydim,long(hdr.zdim)*long(tdim))
case lcz of
    0: image = image/sqrt(nfiles)
    1: image = image/nfiles
    2: ;do nothing
    else: print,'Invalid value of lcz in sum_images.'
endcase
put_image,image,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,tdim,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,fi.n,MODEL=fi.n,IFH=hdr.ifh
end
