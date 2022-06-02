;Copyright 5/22/12 Washington University.  All Rights Reserved.
;fidl_scratchdir.pro  $Revision: 1.5 $

;function fidl_scratchdir,NOPATH=nopath
;    dummy = !BINEXECUTE+'/fidl_scratchdir'
;    print,dummy
;    spawn,dummy,result
;    if strpos(result[0],'fidlError') ne -1 then begin
;        stat=dialog_message(result,/ERROR)
;        return,'ERROR'
;    endif
;    if keyword_set(NOPATH) then begin
;        scrap = strpos(result[0],'SCRATCH')
;        result=strmid(result[0],scrap,strpos(result[0],'/',/REVERSE_SEARCH)-scrap+1)
;    endif
;    return,result[0]
;end
;START140715
;function fidl_scratchdir,NOPATH=nopath,LABEL=label
;    nopathstr = ''
;    labelstr = ''
;    if keyword_set(NOPATH) then nopathstr = ' -nopath'
;    if keyword_set(LABEL) then labelstr=' -label '+label
;    dummy = !BINEXECUTE+'/fidl_scratchdir'+nopathstr+labelstr
;    print,dummy
;    spawn,dummy,result
;    if strpos(result[0],'fidlError') ne -1 then begin
;        stat=dialog_message(result,/ERROR)
;        return,'ERROR'
;    endif
;    return,result[0]
;end
;START210128
function fidl_scratchdir,NOPATH=nopath,LABEL=label
    dummy = !BINEXECUTE+'/fidl_timestr3'
    spawn,dummy,superbird
    superbird = superbird[0] 
    if keyword_set(LABEL) then superbird=label+superbird else superbird='SCRATCH'+superbird
    if not keyword_set(NOPATH) then superbird=getenv('PWD')+'/'+superbird
    superbird=superbird+'/'
    dummy = 'mkdir -p '+superbird
    spawn,dummy,result
    return,superbird
end

;mcavoy@vglab% /home/usr/mcavoy/FIDL/build/fidl_timestr3
;_210128184415706986
;mcavoy@vglab% /home/usr/mcavoy/FIDL/build/fidl_scratchdir -nopath
;SCRATCH_210128:18:47:19:747456/
;mcavoy@vglab% /home/usr/mcavoy/FIDL/build/fidl_scratchdir
;/home/usr/mcavoy/SCRATCH_210128:18:48:19:591356/
;mcavoy@vglab% /home/usr/mcavoy/FIDL/build/fidl_scratchdir -label dog
;/home/usr/mcavoy/dog_210128:18:48:34:937940/
