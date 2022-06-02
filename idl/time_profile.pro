;Copyright 12/31/99 Washington University.  All Rights Reserved.
;time_profile.pro  $Revision: 12.87 $

;************************************************************************************************************
pro time_profile,prof,row,pr,dsp,fi,file,profdat,XMIN=xmin,XMAX=xmax,TITLE=xtitle,Y_TITLE=y_title,XVALS=xvals,SQUARE=square,NO_LEGEND=no_legend
;************************************************************************************************************

;print,'here time_profile top'

pr.cur_window = pr.nwindows
pr.nwindows = pr.nwindows + 1
plot_title = string(pr.cur_window,FORMAT='("Profile #",i2)')
pr.base(pr.cur_window) = widget_base(TITLE=plot_title,/COLUMN)
if(keyword_set(SQUARE)) then $
    pr.draw(pr.cur_window) = widget_draw(pr.base(pr.cur_window),xsize=500,ysize=500, $
            COLORS=2,RETAIN=2,KILL_NOTIFY=string('prof_dead')) $
else $
    pr.draw(pr.cur_window) = widget_draw(pr.base(pr.cur_window),xsize=600,ysize=300, $
            COLORS=2,RETAIN=2,KILL_NOTIFY=string('prof_dead'))
widget_control,pr.base(pr.cur_window),/REALIZE
widget_control,GET_VALUE=i,pr.draw(pr.cur_window)
pr.id(pr.cur_window)=i
wset,pr.id(pr.cur_window)
xr=fltarr(2)
if(keyword_set(Y_TITLE)) then y_title = y_title else y_title = ' '
if(keyword_set(XMIN)) then xr(0) = xmin else xr(0) = 0
if(keyword_set(XMAX)) then xr(1) = xmax else xr(1) = n_elements(prof) + xr(0)
if(keyword_set(TITLE)) then pr.plot_title = xtitle(0) else pr.plot_title=file

case pr.symtype of
    !LINE: pr.sym(0) = 0
    !SYMBOL: pr.sym(0) = !SQUARE
    !SYMLINE: pr.sym(0) = - !SQUARE
    else: pr.sym(0) = 0
endcase

profdat = fltarr(n_elements(prof),20)

if(keyword_set(XVALS)) then begin
    profdat[*,0] = xvals
    xr[0] = min(xvals)
    xr[1] = max(xvals)
    pr.sym(0) = !SQUARE
endif else begin
    del = (xr(1) - xr(0))/float(n_elements(prof))
    profdat(0,0) = xr(0)
    for i=1,n_elements(prof)-1 do $
        profdat(i,0) = profdat(i-1) + del
endelse

profdat(*,1) = prof
yr=fltarr(2)
max = max(prof(0:n_elements(prof)-1),MIN=min)
if(max gt 0) then begin
    if(abs(min) gt abs(max)) then $
	max = abs(min) 
endif
yr(0) = -max
if(yr(0) gt 0) then yr(0) = 0
yr(1) = max
pr.profcmd = strarr(10)
legcol = intarr(10)
subtitle = string(FORMAT='("Row/Column: ",i3)',row)
plot,profdat(*,0),prof,BACKGROUND=dsp[fi.cw].black,COLOR=dsp[fi.cw].white,XRANGE=xr,FONT=0,SYMSIZE=.4, $
    XSTYLE=1,SUBTITLE=subtitle,XTITLE=string('Frame'),PSYM=pr.sym(0), $
    YTITLE=y_title,TITLE=pr.plot_title(0),YRANGE=yr
xxx = fltarr(n_elements(prof))
oplot,xxx,COLOR=dsp[fi.cw].white

pr.profcmd(0) = strcompress(string('plot,prd.profdat(*,0),prd.profdat(*,1),BACKGROUND=dsp[fi.cw].black,COLOR=0,XRANGE=xr,FONT=0,XSTYLE=1,XTITLE=string("Frame"),YTITLE=string("Percent change"),TITLE=pr.plot_title(0),YRANGE=yr,PSYM=pr.sym(0),SYMSIZE=.4'))

pr.profcmd(1) = string('oplot,prof,COLOR=0')
pr.plotfiles = strarr(10)
pr.plotfiles(0) = strcompress(string(file,dsp[fi.cw].plane,dsp[fi.cw].frame, $
			FORMAT='(a,"_p",i,"_f",i)'),/REMOVE_ALL)
col  = dsp[fi.cw].white
if(not keyword_set(NO_LEGEND)) then $
    legend,pr.plotfiles(0),PSYM=pr.sym(0),COLORS=col,TEXTCOLORS=col,BOX=0

pr.nprof = 1
xx = {name:'',profcmd:pr.profcmd,profdat:profdat,plotfiles:pr.plotfiles,subtitle:subtitle,nprof:pr.nprof,type:!NOT_GANGED}
pr.profcmds(pr.cur_window) = ptr_new(xx)
wset,dsp[fi.cw].image_index[fi.cw]

;print,'here time_profile bottom'

return
end
