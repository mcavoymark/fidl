;Copyright 4/23/08 Washington University.  All Rights Reserved.
;extract_fzstat.pro  $Revision: 1.4 $
pro extract_fzstat,fi,dsp,wd,glm,help
    hdr = *fi.hdr_ptr[fi.n]
    glmidx = fi.n
    if not ptr_valid(glm[glmidx].fzstat) then begin
        stat=dialog_message('*** F statistic not specified.  ***',/ERROR)
        return
    endif
    fzstat = *glm[glmidx].fzstat
    F_names = *glm[glmidx].F_names2
    list = get_bool_list(F_names,TITLE='F statistics to be extracted.')
    for i=0,glm[glmidx].nF-1 do begin
        if list.list[i] gt 0 then begin
            name = strcompress(string(fi.tails[glmidx],F_names[i],FORMAT='(a,"_fzstat",a)'),/REMOVE_ALL)

            ;put_image,fzstat[*,*,*,i],fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY, $
            ;    glmidx,-1,MODEL=glmidx,IFH=hdr.ifh
            ;START150731
            put_image,fzstat[*,*,*,i],fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY, $
                glmidx,MODEL=glmidx,IFH=hdr.ifh

        endif
    endfor
    undefine,hdr,glmidx,fzstat,F_names,list

end
