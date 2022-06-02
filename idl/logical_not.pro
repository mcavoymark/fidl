;Copyright 12/31/99 Washington University.  All Rights Reserved.
;logical_not.pro  $Revision: 1.6 $
pro logical_not,fi,wd,dsp,help,stc,pref
ntails=0
repeat begin
    get_filelist_labels,fi,ntails,tails,index,ntails_glm,tails_glm,index_glm,ntails_conc,tails_conc,index_conc
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
rtn = select_files(tails,TITLE='Logical image(s) to be inverted.',/EXIT,/ONE_COLUMN)
if rtn.files[0] eq 'EXIT' then return
name = 'Logical_not_' + tails[rtn.index]
str = get_str(rtn.count,'Name '+strtrim(indgen(rtn.count)+1,2),name,/GO_BACK,/EXIT,TITLE='Leave blank any you do not want.')
if str[0] eq 'EXIT' then return else if str[0] eq 'GO_BACK' then goto,goback0 
for i=0,rtn.count-1 do begin
    if str[i] ne '' then begin
        hdr = *fi.hdr_ptr[rtn.index[i]]
        widget_control,/HOURGLASS
        image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
        for z=0,hdr.zdim-1 do image[*,*,z] = get_image(z,fi,stc,FILNUM=rtn.index[i]+1)
        image = 1. - image
        put_image,image,fi,wd,dsp,strtrim(str[i],2),hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!LOGICAL_ARRAY, $
            hdr.mother,IFH=hdr.ifh
        fi.color_scale1[fi.n] = !GRAY_SCALE
        fi.color_scale2[fi.n] = !NO_SCALE
        ;load_colortable,fi,dsp,TABLE1=!GRAY_SCALE,TABLE2=!NO_SCALE
    endif
endfor
end
