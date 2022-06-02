;Copyright 12/31/99 Washington University.  All Rights Reserved.,MASK=mask
;threshold_display.pro  $Revision: 12.106 $

;pro get_newimg,image,newimg
;START170103
;pro get_newimg,image,newimg,ALLFRAMES=allframes
;START170104
pro get_newimg,newimg,ALLFRAMES=allframes,MASK=mask

    ;common threshold_display,wslider,wsliderh,waccept,image_index,wcancel,tgt_image,newmin,newmax,threshold,absimg, $
    ;    act_type,act_type_str,idx_tgt,wtext,wtexth,npos,wreverse,lc_reverse,hdr
    ;START170103
    ;common threshold_display,wslider,wsliderh,waccept,image_index,wcancel,tgt_image,newmin,newmax,threshold,absimg, $
    ;    act_type,act_type_str,idx_tgt,wtext,wtexth,npos,hdr,wtextn,wtexthn,wslidern,wsliderhn
    ;START170104
    common threshold_display,idx_tgt,newmax,newmin,image,wtext,wslider,wtexth,wsliderh,wtextn,wtexthn,wslidern,wsliderhn, $
        waccept,wcancel,wmask,minp,maxp,minn,maxn,minp0,maxp0,minn0,maxn0,hdr


    common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw


    ;if hdr.tdim eq 1 then begin
    ;    if npos gt 0 then begin
    ;        newimg = image*(abs(image) ge newmin and abs(image) le newmax)
    ;        if act_type eq !POSITIVE_ACTIVATIONS then $
    ;            newimg = newimg * (newimg ge 0.) $
    ;        else if act_type eq !NEGATIVE_ACTIVATIONS then $
    ;            newimg = newimg * (newimg le 0.)
    ;    endif else begin
    ;        if lc_reverse eq 0 then $
    ;            newimg = image*(image lt newmin) $
    ;        else $
    ;            newimg = image*(image gt newmin)
    ;    endelse
    ;endif else begin
    ;    newimg = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim)
    ;    jmg = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
    ;    widget_control,/HOURGLASS
    ;    for t=0,hdr.tdim-1 do begin
    ;        print,'hdr.tdim=',hdr.tdim,' t=',t
    ;        for z=0,hdr.zdim-1 do jmg[*,*,z] = get_image(hdr.zdim*t + z,fi,stc,FILNUM=idx_tgt+1)
    ;        if npos gt 0 then begin
    ;            kmg = jmg*(abs(jmg) ge newmin and abs(jmg) le newmax)
    ;            if act_type eq !POSITIVE_ACTIVATIONS then $
    ;                kmg = kmg * (kmg ge 0.) $
    ;            else if act_type eq !NEGATIVE_ACTIVATIONS then $
    ;                kmg = kmg * (kmg le 0.)
    ;        endif else begin
    ;            if lc_reverse eq 0 then $
    ;                kmg = jmg*(jmg lt newmin) $
    ;            else $
    ;                kmg = jmg*(jmg gt newmin)
    ;        endelse
    ;        newimg[*,*,*,t] = kmg
    ;    endfor
    ;endelse
    ;refresh_display,fi,dsp,stc,wd,pref,IMAGE=newimg
    ;START170104
    image[*,*]=get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1)+dsp[fi.cw].plane-1,fi,stc,FILNUM=idx_tgt+1)
    image=image*((image ge minp0 and image le maxp0)+(image ge maxn0 and image le minn0))
    refresh_display,fi,dsp,stc,wd,pref,IMAGE=image
    if keyword_set(ALLFRAMES) then begin
        newimg = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,hdr.tdim)
        jmg=fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
        widget_control,/HOURGLASS
        for t=0,hdr.tdim-1 do begin
            print,'hdr.tdim=',hdr.tdim,' t=',t
            for z=0,hdr.zdim-1 do jmg[*,*,z]=get_image(hdr.zdim*t+z,fi,stc,FILNUM=idx_tgt+1)
            newimg[*,*,*,t]=jmg*((jmg ge minp0 and jmg le maxp0)+(jmg ge maxn0 and jmg le minn0))
        endfor
    endif else if keyword_set(MASK) then begin
        newimg=fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
        for z=0,dsp[fi.cw].zdim-1 do newimg[*,*,z]=get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1)+z,fi,stc,FILNUM=idx_tgt+1)
        newimg=(newimg ge minp0 and newimg le maxp0)+(newimg ge maxn0 and newimg le minn0)
    endif

end
pro threshold_display_event,ev
    common threshold_display
    common stats_comm

    ;print,'here0 ev.id=',ev.id

    case ev.id of

        ;wtext: begin
        ;    widget_control,wtext,GET_VALUE=newmin
        ;    newmin = float(newmin)
        ;    newmin = newmin[0]
        ;    widget_control,wslider,SET_VALUE=newmin
        ;    get_newimg,tgt_image[*,*,dsp[fi.cw].plane-1],newimg
        ;end
        ;wtexth: begin
        ;    widget_control,wtexth,GET_VALUE=newmax
        ;    newmax = float(newmax)
        ;    newmax = newmax[0]
        ;    widget_control,wsliderh,SET_VALUE=newmax
        ;    get_newimg,tgt_image[*,*,dsp[fi.cw].plane-1],newimg
        ;end
        ;wslider: begin
        ;    widget_control,wslider,GET_VALUE=newmin
        ;    widget_control,wtext,SET_VALUE=string(newmin)
        ;    get_newimg,tgt_image[*,*,dsp[fi.cw].plane-1],newimg
        ;end
        ;wsliderh: begin
        ;    widget_control,wsliderh,GET_VALUE=newmax
        ;    widget_control,wtexth,SET_VALUE=string(newmax)
        ;    get_newimg,tgt_image[*,*,dsp[fi.cw].plane-1],newimg
        ;end
        ;START170104
        wtext: begin
            widget_control,wtext,GET_VALUE=x
            minp0=float(x)
            minp0=minp0[0]
            minp0=minp0>minp
            ;widget_control,wtext,SET_VALUE=string(minp0)
            widget_control,wtext,SET_VALUE=trim(minp0)
            widget_control,wslider,SET_VALUE=minp0
            get_newimg
        end
        wtexth: begin
            widget_control,wtexth,GET_VALUE=x
            maxp0=float(x)
            maxp0=maxp0[0]
            maxp0=maxp0<maxp
            ;widget_control,wtexth,SET_VALUE=string(maxp0)
            widget_control,wtexth,SET_VALUE=trim(maxp0)
            widget_control,wsliderh,SET_VALUE=maxp0
            get_newimg
        end
        wslider: begin
            widget_control,wslider,GET_VALUE=minp0
            ;widget_control,wtext,SET_VALUE=string(minp0)
            widget_control,wtext,SET_VALUE=trim(minp0)
            get_newimg
        end
        wsliderh: begin
            widget_control,wsliderh,GET_VALUE=maxp0
            ;widget_control,wtexth,SET_VALUE=string(maxp0)
            widget_control,wtexth,SET_VALUE=trim(maxp0)
            get_newimg
        end
        wtextn: begin
            widget_control,wtextn,GET_VALUE=x
            maxn0=float(x)
            maxn0=maxn0[0]
            maxn0=maxn0<maxn
            ;widget_control,wtextn,SET_VALUE=string(maxn0)
            widget_control,wtextn,SET_VALUE=trim(maxn0)
            widget_control,wslidern,SET_VALUE=maxn0
            get_newimg
        end
        wtexthn: begin
            widget_control,wtexthn,GET_VALUE=x
            minn0=float(x)
            minn0=minn0[0]
            minn0=minn0>minn
            ;widget_control,wtexthn,SET_VALUE=string(minn0)
            widget_control,wtexthn,SET_VALUE=trim(minn0)
            widget_control,wsliderhn,SET_VALUE=minn0
            get_newimg
        end
        wslidern: begin
            widget_control,wslidern,GET_VALUE=maxn0
            ;widget_control,wtextn,SET_VALUE=string(maxn0)
            widget_control,wtextn,SET_VALUE=trim(maxn0)
            get_newimg
        end
        wsliderhn: begin
            widget_control,wsliderhn,GET_VALUE=minn0
            ;widget_control,wtexthn,SET_VALUE=string(minn0)
            widget_control,wtexthn,SET_VALUE=trim(minn0)
            get_newimg
        end

        waccept: begin

            ;hdr = *fi.hdr_ptr[idx_tgt]
            ;name=strcompress(string(fi.tails[idx_tgt],newmin,newmax,act_type_str,FORMAT='(a,"_threshl",f7.2,"h",f7.2,a)'),/REMOVE_ALL)
            ;widget_control,/HOURGLASS
            ;get_newimg,tgt_image,newimg
            ;START170104
            widget_control,/HOURGLASS
            get_newimg,newimg,/ALLFRAMES
            name=fi.tails[idx_tgt]
            if newmax gt 0. then name=name+'_pl'+trim(minp0)+'ph'+trim(maxp0) else name=name+'_'
            if newmin lt 0. then name=name+'nh'+trim(minn0)+'nl'+trim(maxn0)
            put_image,newimg,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother, $
                IFH=hdr.ifh

            ;START170223
            dsp_image=update_image(fi,dsp,wd,stc,pref)

        end
        wcancel: begin
            widget_control,ev.top,/DESTROY
        end

        ;START170104
        wmask:begin
            widget_control,/HOURGLASS
            get_newimg,newimg,/MASK
            name=fi.tails[idx_tgt]
            if newmax gt 0. then name=name+'_pl'+trim(minp0)+'ph'+trim(maxp0) else name=name+'_'
            if newmin lt 0. then name=name+'nh'+trim(minn0)+'nl'+trim(maxn0)
            name=name+'_mask'
            put_image,newimg,fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother, $
                IFH=hdr.ifh

            ;START170223
            dsp_image=update_image(fi,dsp,wd,stc,pref)

        end

        ;START170103
        ;wreverse: begin
        ;    lc_reverse = ev.value
        ;    get_newimg,tgt_image[*,*,dsp[fi.cw].plane-1],newimg
        ;end

        else: begin
            print,'hereA'
        end
    endcase
    check_math_errors,!FALSE
end
pro threshold_display
    common threshold_display
    common stats_comm
    image_index = !D.WINDOW
    if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx_tgt,'image to threshold') eq !ERROR then return
    dsp[fi.cw].file_num = idx_tgt
    widget_control,/HOURGLASS

    ;if help.enable eq !TRUE then begin
    ;    x = 'This program thresholds and image at a value set interactively by the user.  The threshold is set to positive and ' $
    ;    +'negative values in the image simulataneously (i.e., mask = abs(image) gt threshold)'
    ;    widget_control,help.id,SET_VALUE=x
    ;endif
    ;tgt_image = fltarr(dsp[fi.cw].xdim,dsp[fi.cw].ydim,dsp[fi.cw].zdim)
    ;for z=0,dsp[fi.cw].zdim-1 do tgt_image[*,*,z] = get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + z,fi,stc,FILNUM=idx_tgt+1)
    ;msk_image = tgt_image
    ;absimg = abs(msk_image)
    ;npos = total(msk_image gt !UNSAMPLED_VOXEL)
    ;if npos gt 0 then begin
    ;    max = max(msk_image,MIN=newmin)
    ;    if newmin lt 0 then begin
    ;        type = get_button(['All activations','Positive activations','Negative activations'], $
    ;               TITLE=string('Which are you interested in?'))
    ;        case type of
    ;            0: begin
    ;                act_type = !ALL_ACTIVATIONS
    ;                act_type_str = 'all'
    ;               end
    ;            1: begin
    ;                act_type = !POSITIVE_ACTIVATIONS
    ;                act_type_str = 'pos'
    ;               end
    ;            2: begin
    ;                act_type = !NEGATIVE_ACTIVATIONS
    ;                act_type_str = 'neg'
    ;               end
    ;            else: begin
    ;                print,'Invalid index'
    ;                return
    ;            end
    ;        endcase
    ;        if act_type eq !ALL_ACTIVATIONS or act_type eq !NEGATIVE_ACTIVATIONS then begin
    ;            if abs(newmin) gt abs(max) then max = abs(newmin)
    ;        endif
    ;        mask = absimg gt 0
    ;        newmin = min(absimg - mask*max) + max
    ;    endif else begin
    ;        act_type = !ALL_ACTIVATIONS
    ;        act_type_str = '_all'
    ;    endelse
    ;    value = '0.00'
    ;endif else begin
    ;    index = where(msk_image lt 0,count) 
    ;    if count eq 0 then begin
    ;        stat = dialog_message('We have a problem. Abort!',/ERROR)
    ;        return
    ;    endif
    ;    max = max(msk_image[index],MIN=newmin)
    ;    act_type = !ALL_ACTIVATIONS
    ;    act_type_str = ''
    ;    value = strtrim(max,2)
    ;endelse
    ;newmax = max
    ;intbase = widget_base(/COLUMN,TITLE='Threshold',/MODAL,GROUP_LEADER=!FMRI_LEADER)
    ;wlabel = widget_text(intbase,VALUE='Lower')
    ;wtext = widget_text(intbase,VALUE=value,/EDITABLE,/ALL_EVENTS)
    ;slider = widget_base(intbase,/ROW)
    ;wslider = cw_fslider(slider,DRAG=1,MINIMUM=newmin,MAXIMUM=newmax,XSIZE=256,FORMAT='(f8.2)',/SUPPRESS_VALUE)
    ;wlabelh = widget_text(intbase,VALUE='Higher')
    ;wtexth = widget_text(intbase,VALUE=strtrim(newmax,2),/EDITABLE,/ALL_EVENTS)
    ;sliderh = widget_base(intbase,/ROW)
    ;wsliderh = cw_fslider(sliderh,DRAG=1,MINIMUM=newmin,MAXIMUM=newmax,XSIZE=256,VALUE=newmax,FORMAT='(f8.2)',/SUPPRESS_VALUE)
    ;START170103
    tgt_image = fltarr(dsp[fi.cw].xdim,dsp[fi.cw].ydim,dsp[fi.cw].zdim)
    for z=0,dsp[fi.cw].zdim-1 do tgt_image[*,*,z] = get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + z,fi,stc,FILNUM=idx_tgt+1)
    newmax=max(tgt_image,MIN=newmin)
    intbase = widget_base(/COLUMN,TITLE='Threshold',GROUP_LEADER=!FMRI_LEADER)

    ;START170309
    wtext=0 & wtexth=0 & wtextn=0 & wtexthn=0
    wslider=0 & wsliderh=0 & wslidern=0 & wsliderhn=0

    minp=0.
    maxp=newmax 
    if newmax gt 0. then begin
        wlabel = widget_text(intbase,VALUE='Lower positive')
        ;wtext = widget_text(intbase,VALUE=trim(minp),/EDITABLE,/ALL_EVENTS)
        wtext = widget_text(intbase,VALUE=trim(minp),/EDITABLE)
        slider = widget_base(intbase,/ROW)
        ;wslider = cw_fslider(slider,DRAG=1,MINIMUM=minp,MAXIMUM=maxp,XSIZE=256,VALUE=minp,FORMAT='(f8.2)',/SUPPRESS_VALUE)
        wslider = cw_fslider(slider,DRAG=1,MINIMUM=minp,MAXIMUM=maxp,XSIZE=256,VALUE=minp,/SUPPRESS_VALUE)
        wlabelh = widget_text(intbase,VALUE='Higher positive')
        ;wtexth = widget_text(intbase,VALUE=trim(maxp),/EDITABLE,/ALL_EVENTS)
        wtexth = widget_text(intbase,VALUE=trim(maxp),/EDITABLE)
        sliderh = widget_base(intbase,/ROW)
        ;wsliderh = cw_fslider(sliderh,DRAG=1,MINIMUM=minp,MAXIMUM=maxp,XSIZE=256,VALUE=maxp,FORMAT='(f8.2)',/SUPPRESS_VALUE)
        wsliderh = cw_fslider(sliderh,DRAG=1,MINIMUM=minp,MAXIMUM=maxp,XSIZE=256,VALUE=maxp,/SUPPRESS_VALUE)
    endif
    minn=newmin
    maxn=0.
    if newmin lt 0. then begin
        wlabeln = widget_text(intbase,VALUE='Higher negative')
        ;wtextn = widget_text(intbase,VALUE=trim(maxn),/EDITABLE,/ALL_EVENTS)
        wtextn = widget_text(intbase,VALUE=trim(maxn),/EDITABLE)
        slider = widget_base(intbase,/ROW)
        ;wslidern = cw_fslider(slider,DRAG=1,MINIMUM=minn,MAXIMUM=maxn,XSIZE=256,VALUE=maxn,FORMAT='(f8.2)',/SUPPRESS_VALUE)
        wslidern = cw_fslider(slider,DRAG=1,MINIMUM=minn,MAXIMUM=maxn,XSIZE=256,VALUE=maxn,/SUPPRESS_VALUE)
        wlabelhn = widget_text(intbase,VALUE='Lower negative')
        ;wtexthn = widget_text(intbase,VALUE=trim(minn),/EDITABLE,/ALL_EVENTS)
        wtexthn = widget_text(intbase,VALUE=trim(minn),/EDITABLE)
        sliderh = widget_base(intbase,/ROW)
        ;wsliderhn = cw_fslider(sliderh,DRAG=1,MINIMUM=minn,MAXIMUM=maxn,XSIZE=256,VALUE=minn,FORMAT='(f8.2)',/SUPPRESS_VALUE)
        wsliderhn = cw_fslider(sliderh,DRAG=1,MINIMUM=minn,MAXIMUM=maxn,XSIZE=256,VALUE=minn,/SUPPRESS_VALUE)
    endif
    minp0=minp
    maxp0=maxp
    minn0=minn
    maxn0=maxn

    ;lc_reverse = 0
    ;wreverse = 0
    ;START170104
    image=fltarr(dsp[fi.cw].xdim,dsp[fi.cw].ydim)

    ;START170103
    ;if npos eq 0 then begin
    ;    rbut = widget_base(intbase,/ROW)
    ;    wreverse = cw_bgroup(rbut,['more neg than thresh','less neg than thresh'],/EXCLUSIVE,SET_VALUE=lc_reverse,/COLUMN,/NO_RELEASE)
    ;endif

    buttons = widget_base(intbase,/ROW)
    waccept = widget_button(buttons,VALUE='Accept')

    ;wcancel = widget_button(buttons,VALUE='Cancel')
    ;START170104
    wcancel=widget_button(buttons,VALUE='Exit')
    wmask=widget_button(buttons,VALUE='Create mask')

    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='A desired threshold may be entered directly by either double ' $
        +'clicking or clicking and dragging the displayed threshold. From the keyboard, type the desired number and hit enter/return.'
    widget_control,intbase,/REALIZE
    xmanager,'threshold_display',intbase
    wset,image_index
end
