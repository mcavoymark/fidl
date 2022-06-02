;Copyright 12/31/99 Washington University.  All Rights Reserved.
;print_prof.pro  $Revision: 12.87 $

;****************************
pro print_prof,pr,fi,dsp,pref
;****************************


hd = *fi.hdr_ptr(fi.n)
hdr = *fi.hdr_ptr(hd.mother)

;wset,pr.id(pr.cur_window)
prd = *pr.profcmds(pr.cur_window)
set_plot,'ps'
;    device,YSIZE=16,YOFFSET=2,FILENAME=fi.printfile,/LANDSCAPE $
if(hdr.tdim gt 1) then $
    device,FILENAME=fi.printfile,/LANDSCAPE $
else $
    device,YSIZE=20,YOFFSET=4,FILENAME=fi.printfile
plot_file = strcompress(string(getenv("HOME"),"/idl.ps"),/REMOVE_ALL)
yr = fltarr(2)
xr = fltarr(2)
xr(0) = prd.profdat(0,0)
xr(1) = prd.profdat(n_elements(prd.profdat(*,0))-1,0)
if(prd.type eq !NOT_GANGED) then begin
    yr(0) = min(prd.profdat(*,1:prd.nprof))
    if(yr(0) gt 0) then yr(0) = 0
    yr(1) = 1.25*max(prd.profdat(*,1:prd.nprof))
endif
prof = fltarr(n_elements(prd.profdat(*,0)))
for i=0,prd.nprof do begin
    if((prd.type eq !GANG_TPROF) and $
       (i gt 0) and (i lt prd.nprof-1)) then begin
        yr(0) = min(prd.profdat(*,i))
        if(yr(0) gt 0) then yr(0) = 0
        yr(1) = 1.25*max(prd.profdat(*,i))
;print,i,yr,prd.nprof
;print,prd.profdat(*,i)
;print,' '
    endif
    case i of
        1: symbol = !SQUARE
        2: symbol = !TRIANGLE
        3: symbol = !PLUS
        4: symbol = !STAR
        else: symbol = !DOT
    endcase
    stat = execute(prd.profcmd(i))
endfor
legend,prd.plotfiles(0:prd.nprof-1),PSYM=sym,BOX=0
device,/CLOSE

;cmd = string('lpr -P',pref.printer_name,' ',fi.printfile)
cmd = get_printer_cmd(pref.printer_name,fi.printfile) 

cmd = get_str(1,'Print command: ',cmd,TITLE='Check printer command')
spawn,cmd
set_plot,'X'

return
end
