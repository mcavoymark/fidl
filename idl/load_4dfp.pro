;Copyright 12/31/99 Washington University.  All Rights Reserved.
;load_4dfp.pro  $Revision: 12.89 $
pro load_4dfp,fi,stc,dsp,st,help,wd,filename,roi
stat = assoc_4dfp(fi,stc,dsp,st,help,wd,filename,roi,ifh,type,num_type,img,mother,xdim,ydim,zdim,tdim,dxdy,dz,paradigm)
scl = 1.

;put_image,img,fi,wd,dsp,filename,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,mother,paradigm,IFH=ifh
;START150731
put_image,img,fi,wd,dsp,filename,xdim,ydim,zdim,tdim,num_type,dxdy,dz,scl,type,mother,IFH=ifh

if !BATCH eq !FALSE then begin
    fi.color_scale1[fi.n] = !GRAY_SCALE
    fi.color_scale2[fi.n] = !NO_SCALE
    load_colortable,fi,dsp,TABLE1=!GRAY_SCALE,TABLE2=!NO_SCALE
endif
return
end
