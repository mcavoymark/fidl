;Copyright 12/31/99 Washington University.  All Rights Reserved.
;profile.pro  $Revision: 12.85 $

;************************************************************************
pro profile,prof,row,pr,dsp,fi,file,dummy,XMIN=xmin,XMAX=xmax,TITLE=xtitle,YLOG=ylog
;************************************************************************

pr.cur_window = pr.nwindows
pr.nwindows = pr.nwindows + 1
plot_title = string(pr.cur_window,FORMAT='("Profile #",i2)')
pr.base(pr.cur_window) = widget_base(TITLE=plot_title,/COLUMN)
pr.draw(pr.cur_window) = widget_draw(pr.base(pr.cur_window),xsize=400,ysize=500,COLORS=2,RETAIN=2,KILL_NOTIFY=string('prof_dead'))
widget_control,pr.base(pr.cur_window),/REALIZE
widget_control,GET_VALUE=i,pr.draw(pr.cur_window)
pr.id(pr.cur_window)=i
wset,pr.id(pr.cur_window)
xr=fltarr(2)
if(keyword_set(YLOG)) then ylog = !TRUE else ylog = !FALSE
if(keyword_set(XMIN)) then xr(0) = xmin else xr(0) = 0
if(keyword_set(XMAX)) then xr(1) = xmax else xr(1) = n_elements(prof) + xr(0)
if(keyword_set(TITLE)) then pr.plot_title = xtitle(0) else pr.plot_title=file
profdat = fltarr(n_elements(prof),20)
del = (xr(1) - xr(0))/float(n_elements(prof))
profdat(0,0) = xr(0)
for i=1,n_elements(prof)-1 do $
    profdat(i,0) = profdat(i-1) + del
profdat(*,1) = prof
yr=fltarr(2)
yr(0) = min(prof)
if(yr(0) gt 0) then yr(0) = 0
yr(1) = 1.25*max(prof)
pr.profcmd = strarr(10)
legcol = intarr(10)
subtitle = string(FORMAT='("Row/Column: ",i3)',row)
case pr.symtype of
    !LINE: pr.sym(0) = 0
    !SYMBOL: pr.sym(0) = !SQUARE
    !SYMLINE: pr.sym(0) = - !SQUARE 
    else: pr.sym(0) = 0 
endcase
xxx = fltarr(n_elements(prof))
if(dsp[fi.cw].orientation eq !NEURO) then  $
    xtitle = 'Display index in neurological coordinates' $
else $
    xtitle = 'Display index in radiological coordinates'
if(ylog eq !FALSE) then begin
    plot,profdat(*,0),prof,COLOR=dsp[fi.cw].white,XRANGE=xr,FONT=0,SYMSIZE=.4, $
    XSTYLE=1,SUBTITLE=subtitle,XTITLE=xtitle,PSYM=pr.sym(0), $
    YTITLE=string('Image value'),TITLE=pr.plot_title(0),YRANGE=yr, $
    BACKGROUND=dsp[fi.cw].black
    oplot,xxx,COLOR=dsp[fi.cw].white
endif else begin
    plot,profdat(*,0),prof,COLOR=dsp[fi.cw].white,XRANGE=xr,FONT=0,SYMSIZE=.4, $
    XSTYLE=1,SUBTITLE=subtitle,XTITLE=xtitle,PSYM=pr.sym(0), $
    YTITLE=string('Image value'),TITLE=pr.plot_title(0),/YLOG, $
    BACKGROUND=dsp[fi.cw].black
endelse

if(ylog eq !FALSE) then begin
    pr.profcmd(0) = strcompress(string('plot,prd.profdat(*,0),prd.profdat(*,1),BACKGROUND=dsp[fi.cw].black,COLOR=0,XRANGE=xr,FONT=0,XSTYLE=1,XTITLE=xtitle,YTITLE=string("Image value"),TITLE=pr.plot_title(0),YRANGE=yr,PSYM=pr.sym(0),SYMSIZE=.4,SUBTITLE=prd.subtitle,BACKGROUND=dsp[fi.cw].black'))
endif else begin
    pr.profcmd(0) = strcompress(string('plot,prd.profdat(*,0),prd.profdat(*,1),BACKGROUND=dsp[fi.cw].black,COLOR=0,XRANGE=xr,FONT=0,XSTYLE=1,XTITLE=string("Row/Column index"),YTITLE=string("Image value"),TITLE=pr.plot_title(0),PSYM=pr.sym(0),SYMSIZE=.4,SUBTITLE=prd.subtitle,/YLOG'))
endelse

pr.profcmd(1) = string('oplot,prof,COLOR=0')
pr.plotfiles = strarr(10)
pr.plotfiles(0) = strcompress(string(file,dsp[fi.cw].plane,dsp[fi.cw].frame, $
			FORMAT='(a,"_p",i,"_f",i)'),/REMOVE_ALL)
col  = dsp[fi.cw].white
legend,pr.plotfiles(0),PSYM=pr.sym(0),COLORS=col,TEXTCOLORS=col,BOX=0

pr.nprof = 1
xx = {name:'',profcmd:pr.profcmd,profdat:profdat,plotfiles:pr.plotfiles,subtitle:subtitle,nprof:pr.nprof,type:!NOT_GANGED}
pr.profcmds(pr.cur_window) = ptr_new(xx)

wset,dsp[fi.cw].image_index[fi.cw]

return
end
