;Copyright 12/31/99 Washington University.  All Rights Reserved.
;ganged_time_profile.pro  $Revision: 12.85 $

;************************************************************************
pro ganged_time_profile,prof,row,pr,dsp,fi,file,profdat,plot_titles,XMIN=xmin,XMAX=xmax,XTITLE=xtitle,TITLE=title
;************************************************************************

multi_save = !P.MULTI
!P.MULTI = [0,pr.gang_dim,pr.gang_dim]

pr.cur_window = pr.nwindows
pr.nwindows = pr.nwindows + 1
if(keyword_set(TITLE)) then $
    plot_title = title(0) $
else $
    plot_title = string(pr.cur_window,FORMAT='("Profile #",i2)')
pr.base(pr.cur_window) = widget_base(TITLE=plot_title,/COLUMN)
pr.draw(pr.cur_window) = widget_draw(pr.base(pr.cur_window),xsize=200*pr.gang_dim,ysize=150*pr.gang_dim,COLORS=2,RETAIN=2,KILL_NOTIFY=string('prof_dead'))
widget_control,pr.base(pr.cur_window),/REALIZE
widget_control,GET_VALUE=i,pr.draw(pr.cur_window)
pr.id(pr.cur_window)=i
wset,pr.id(pr.cur_window)
len = n_elements(prof(0,*))
xr=fltarr(2)
if(keyword_set(XMIN)) then xr(0) = xmin else xr(0) = 0
if(keyword_set(XMAX)) then xr(1) = xmax else xr(1) = len + xr(0)
if(keyword_set(XTITLE)) then pr.plot_title = xtitle(0) else pr.plot_title=file
profdat = fltarr(len,pr.gang_dim^2 + 5)
del = (xr(1) - xr(0))/float(len)
profdat(0,0) = xr(0)
for i=1,len-1 do $
    profdat(i,0) = profdat(i-1) + del
yr=fltarr(2)
legcol = intarr(10)
subtitle = string(FORMAT='("Row/Column: ",i3)',row)
!LINE: pr.sym = 0
profcmd = strarr(pr.gang_dim^2+5)
profcmd(0) = string(" !P.MULTI = [0,",pr.gang_dim,",",pr.gang_dim,"]")
for i=0,pr.gang_dim^2-1 do begin
    yr(0) = -max(prof(i,*))
    if(yr(0) gt 0) then yr(0) = 0
    yr(1) = 1.5*max(prof(i,*))
    profdat(*,i+1) = prof(i,*)
    if(total(profdat[*,i+1]) eq 0.) then begin
        profdat[*,i+1] = 1.e-20
        profdat[*,i+1] = 0.
    endif
    plot,profdat(*,0),profdat(*,i+1),BACKGROUND=0,COLOR=dsp[fi.cw].white,XRANGE=xr,FONT=0,SYMSIZE=.4, $
    XSTYLE=1,PSYM=pr.sym(0),YRANGE=yr,XTICKS=1,title=plot_titles[i]
    profcmd(i+1) = strcompress(string('plot,prd.profdat(*,0),prd.profdat(*,',i+1,'),BACKGROUND=dsp[fi.cw].white'))+',COLOR=0,XRANGE=xr,FONT=0,XSTYLE=1,YRANGE=yr,PSYM=pr.sym(0),SYMSIZE=.4'
    xxx = fltarr(len)
endfor
profcmd(pr.gang_dim^2+1) = '!P.MULTI = [0,0,0]'

plotfiles = strarr(pr.gang_dim^2 + 5)
plotfiles(0) = " "
col  = dsp[fi.cw].white
legend,plotfiles(0),PSYM=pr.sym(0),COLORS=col,TEXTCOLORS=col,BOX=0

pr.nprof = pr.gang_dim^2 + 2
xx = {name:'',profcmd:profcmd,profdat:profdat,plotfiles:plotfiles,subtitle:subtitle,nprof:pr.nprof,type:!GANG_TPROF}
pr.profcmds(pr.cur_window) = ptr_new(xx)
wset,dsp[fi.cw].image_index[fi.cw]

!P.MULTI = multi_save

return
end
