;Copyright 12/31/99 Washington University.  All Rights Reserved.
;logical_and.pro  $Revision: 12.91 $
pro logical_and,fi,wd,dsp,help,stc
get_filelist_labels,fi,nlabels,labels,index,nlabels_glm,labels_glm,index_glm,nlabels_conc,labels_conc,index_conc
lclist = get_bool_list(labels,TITLE='Please select images to AND.')
lclist = lclist.list
index = where(lclist,count)
if count lt 2 then begin
   stat = dialog_message('Less than two files were specified. Please try again.',/ERROR)
   return
endif
if count gt 2 then begin
    name = 'Logical_and_'
    for i=0,count-1 do name = name + string(index[i],FORMAT='(i2,"_")')
endif else $
    name = fi.list[index[0]] + '_AND_' + fi.list[index[1]]
name = strcompress(name,/REMOVE_ALL)
str = get_str(1,'Name',name,WIDTH=100)
name = str[0]
widget_control,/HOURGLASS
hdr = *fi.hdr_ptr[index[0]]
xdim = hdr.xdim
ydim = hdr.ydim
zdim = hdr.zdim
image = fltarr(xdim,ydim,zdim)
ANDimage = fltarr(xdim,ydim,zdim)
ANDimage[*,*,*] = 1 
keep = -1 
for i=0,count-1 do begin
    hdr = *fi.hdr_ptr[index[i]]
    if hdr.xdim ne xdim or hdr.ydim ne ydim or hdr.zdim ne zdim then begin
        stat = dialog_message('All images must have the same dimensions. Abort!',/ERROR)
        return
    endif
    for z=0,hdr.zdim-1 do image[*,*,z] = get_image(z,fi,stc,FILNUM=index[i]+1)
    if ptr_valid(hdr.ifh.region_names) then $
        keep = i $
    else if hdr.array_type ne !LOGICAL_ARRAY then begin
        scrap = 0L
        for z=0,zdim-1 do $
            for y=0,ydim-1 do $
                for x=0,xdim-1 do $
                    if image[x,y,z] ne 0 and image[x,y,z] ne 1 then begin
                        ;print,'image=',image[x,y,z]
                        scrap = scrap + 1
                    endif
        if scrap eq 0 then $
            hdr.array_type = !LOGICAL_ARRAY $
        else begin
            stat = dialog_message('Images must be binary values. Abort!',/ERROR)
            return
        endelse
    endif
    for z=0,hdr.zdim-1 do ANDimage[*,*,z] = ANDimage[*,*,z] * image[*,*,z] 
endfor
if keep eq -1 then begin ;no region files
    keep = 0 
    hdr = *fi.hdr_ptr[index[keep]]
    ifh = hdr.ifh
endif else begin
    maximum = max(ANDimage,MIN=minimum)
    scrapindex = where(ANDimage ne 0.,count)
    if count gt 0 then begin
        scrap_mask = abs(ANDimage[scrapindex])
        if total(scrap_mask mod fix(scrap_mask)) > 0. then begin
            result=dialog_message('AND image contains noninteger values. Abort!',/ERROR)
            return
        endif
    endif
    if minimum lt 0 then begin
        result=dialog_message('AND image contains values less than zero. Abort!',/ERROR)
        return
    endif
    ;print,'minimum=',minimum,' maximum=',maximum
    if minimum lt 2 then minimum = 2
    regionANDimage = fltarr(xdim,ydim,zdim)
    j = 2
    for i=minimum,maximum do begin
        scrapindex = where(ANDimage eq i,count)
        if count gt 0 then begin
            regionANDimage[scrapindex] = j 
            j = j + 1
        endif
    endfor
    ANDimage = regionANDimage
    hdr = *fi.hdr_ptr[index[keep]]
    ifh = hdr.ifh
    nreg = j - 2
    ;print,'before ',*ifh.region_names
    assign_region_names,ifh,ANDimage,nreg
    ;print,*ifh.region_names
    print,''
    print,'Regions in ',name
    print,'------------------------------------------------------'
    scrap = *ifh.region_names
    for i=0,nreg-1 do print,scrap[i]
    print,'' 
endelse
name = strcompress(name,/REMOVE_ALL)

;paradigm = -2
;put_image,ANDimage,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!LOGICAL_ARRAY,hdr.mother, $
;    paradigm,IFH=ifh
;START150731
put_image,ANDimage,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!LOGICAL_ARRAY,hdr.mother,IFH=ifh

fi.color_scale1(fi.n) = !GRAY_SCALE
fi.color_scale2(fi.n) = !NO_SCALE
load_colortable,fi,dsp,TABLE1=!GRAY_SCALE,TABLE2=!NO_SCALE
end
