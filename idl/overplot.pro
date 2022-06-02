;Copyright 3/20/01 Washington University.  All Rights Reserved.
;overplot.pro  $Revision: 12.90 $

;*****************************************************************************
pro overplot,prof,row,label,profdat,pr,dsp,fi,XMIN=xmin,XMAX=xmax,YLOG=ylog, $
    LINESTYLE=linestyle,XVALS=xvals,NO_LEGEND=no_legend
;*****************************************************************************

; Plot profile over existing profile created by "profile".

;print,'overplot top'

wset,pr.id(pr.cur_window)

prd = *pr.profcmds(pr.cur_window)

xr = fltarr(2)
if(keyword_set(YLOG)) then ylog = !TRUE else ylog = !FALSE
if(keyword_set(XMIN)) then xmin = xmin else xmin = 0
if(keyword_set(XMAX)) then xmax = xmax else xmax = n_elements(prof) + xmin
if(keyword_set(LINESTYLE)) then linetype = linestyle else linetype = 0
x = fltarr(n_elements(prof))
for i=0,n_elements(prof)-1 do $
    x(i) = xmin + float(i)*(xmax-xmin)/n_elements(prof)

case prd.nprof of
    1: pr.sym(prd.nprof) = !TRIANGLE
    2: pr.sym(prd.nprof) = !PLUS
    3: pr.sym(prd.nprof) = !STAR
    4: pr.sym(prd.nprof) = !DOT
    5: pr.sym(prd.nprof) = !TRIANGLE
    6: pr.sym(prd.nprof) = !PLUS
    7: pr.sym(prd.nprof) = !STAR
    8: pr.sym(prd.nprof) = !DOT
    else: begin
    print,'*** Too many profiles.  ***'
    wset,dsp[fi.cw].image_index[fi.cw]
    pr.prof_on = !PROF_NO
    return
    end
endcase
case pr.symtype of
    !LINE: pr.sym(prd.nprof) = 0
    !SYMLINE: pr.sym(prd.nprof) = -pr.sym(prd.nprof)
    else: pr.sym(prd.nprof) = pr.sym(prd.nprof)
endcase

if(keyword_set(XVALS)) then $
    xv = xvals $
else $
    xv = prd.profdat[*,0]

if(n_elements(prof) gt n_elements(prd.profdat(*,prd.nprof))) then begin
    off = (n_elements(prof) - n_elements(prd.profdat(*,prd.nprof)))/2
endif else begin
    off = (n_elements(prd.profdat(*,prd.nprof)) - n_elements(prof))/2
endelse

if(keyword_set(XVALS)) then begin
    prd.nprof = prd.nprof + 1
    prd.profdat(*,prd.nprof) = xvals(off:n_elements(prd.profdat(*,prd.nprof))+off-1)
endif

prd.nprof = prd.nprof + 1
if(n_elements(prof) gt n_elements(prd.profdat(*,prd.nprof))) then begin
    prd.profdat(*,prd.nprof) = prof(off:n_elements(prd.profdat(*,prd.nprof))+off-1)
endif else begin
    prd.profdat(off:n_elements(prof)+off-1,prd.nprof) = prof
endelse

color = !LEN_COLORTAB+prd.nprof-1
if(color gt !D.N_COLORS-1) then color = !LEN_COLORTAB+prd.nprof-5
oplot,xv,prd.profdat(*,prd.nprof),PSYM=pr.sym(prd.nprof-1),COLOR=color, $
            SYMSIZE=.4,LINESTYLE=linetype
prd.profcmd(prd.nprof) = string('oplot,prd.profdat[*,0],prd.profdat(*,i),COLOR=0,PSYM=',pr.sym(prd.nprof-1),',SYMSIZE=.4')
col = intarr(prd.nprof)
col(*) = !WHITE
if(label ne '') then begin
    prd.plotfiles(prd.nprof-1) = strcompress(string(label,dsp[fi.cw].plane,dsp[fi.cw].frame, $
        FORMAT='(a,"_p",i,"_f",i)'),/REMOVE_ALL)
    if(not keyword_set(NO_LEGEND)) then begin 
        legend,prd.plotfiles(0:prd.nprof-1),PSYM=pr.sym(0:prd.nprof-1),COLORS=col,TEXTCOLORS=col,BOX=0
    endif
endif
wset,dsp[fi.cw].image_index[fi.cw]

xx = {name:'',profcmd:prd.profcmd,profdat:prd.profdat,plotfiles:prd.plotfiles,subtitle:prd.subtitle,nprof:prd.nprof,type:!NOT_GANGED}
ptr_free,pr.profcmds(pr.cur_window)
pr.profcmds(pr.cur_window) = ptr_new(xx)

;print,'overplot bottom'
end
