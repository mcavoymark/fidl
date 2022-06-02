;Copyright 10/14/09 Washington University.  All Rights Reserved.
;t_to_z.pro  $Revision: 1.2 $
pro t_to_z,fi,dsp,wd,stc,help,pref

goback0:
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr,idx,'T map') eq !ERROR then return
if(hdr.tdim gt 1) then begin
    str = get_str(1,"Number of frames to save",string(hdr.tdim))
    tdim = fix(str[0])
    z_image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,tdim)
endif else begin
    tdim = 1
    z_image = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
endelse

if hdr.ifh.dof_condition eq 0 then begin
    scrap = get_str(1,'Please enter the degrees of freedom','0',LABEL='The ifh lacks the degrees of freedom.',$
        TITLE='Degrees of freedom',/GO_BACK)
    if scrap[0] eq 'GO_BACK' then goto,goback0 
    df = fix(scrap[0])
    if df eq 0 then begin
        stat=dialog_message('Zero degrees of freedom. Program aborted!',/ERROR)
        return
    endif
endif else $
    df = hdr.ifh.dof_condition

passtype = bytarr(4)
passtype[2] = 1
timg = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
zimg = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
lenvol = long(hdr.xdim)*long(hdr.ydim)*long(hdr.zdim)
widget_control,/HOURGLASS
for t=0,tdim-1 do begin
    if(tdim gt 0) then widget_control,wd.error,SET_VALUE=strcompress(string(t+1,FORMAT='("Processing frame #",i4)'))
    for z=0,hdr.zdim-1 do timg[*,*,z] = get_image(z+t*hdr.zdim,fi,stc,FILNUM=idx+1)
    stat=call_external(!SHARE_LIB,'_t_to_z',timg,zimg,lenvol,double(df),VALUE=passtype)
    z_image[*,*,*,t] = zimg
endfor
widget_control,HOURGLASS=0
    
hdr.tdim = tdim

name = fi.tails[idx]
pos = strpos(fi.tails[idx],'tstat')
if pos eq -1 then $
    name = name + '_zstat' $
else $
    strput,name,'z',pos

paradigm= -1.

put_image,reform(z_image,hdr.xdim,hdr.ydim,hdr.zdim*tdim),fi,wd,dsp,name,hdr.xdim,hdr.ydim,hdr.zdim, $
    tdim,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl,!FLOAT_ARRAY,hdr.mother,paradigm,IFH=hdr.ifh

return
end
