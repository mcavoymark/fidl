;Copyright 4/23/07 Washington University.  All Rights Reserved.
;checkos.pro  $Revision: 1.6 $
function checkos
spawn,'uname -a',result


;result=strsplit(result[0],' ',/EXTRACT)
;bit32_64=32
;if result[0] eq 'SunOS' then $ 
;    SunOS_Linux=0 $
;else if result[0] eq 'Linux' then begin 
;    SunOS_Linux=1
;    if result[n_elements(result)-2] eq 'x86_64' then bit32_64=64
;endif else begin
;    SunOS_Linux=-1
;    str = 'Error: os = '+result[0]+'    Unknown operating system. Abort!'+string(10B) $
;        +'    This error will appear erroneously if you are out of disk space.'
;    print,str
;    stat=dialog_message(str,/ERROR)
;endelse
;START120720
print,result
result = strjoin(result,' ',/SINGLE)
bit32_64=32
if strpos(result,'SunOS') ne -1 then $
    SunOS_Linux=0 $
else if strpos(result,'Linux') ne -1 then begin
    SunOS_Linux=1
    if strpos(result,'x86_64') ne -1 then bit32_64=64
endif else begin
    SunOS_Linux=-1
    str = 'Error: os = '+result[0]+'    Unknown operating system. Abort!'+string(10B) $
        +'    This error will appear erroneously if you are out of disk space.'
    print,str
    stat=dialog_message(str,/ERROR)
endelse


return,rtn={SunOS_Linux:SunOS_Linux,bit32_64:bit32_64}
end
