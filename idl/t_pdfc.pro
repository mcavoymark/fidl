;Copyright 12/31/99 Washington University.  All Rights Reserved.
;t_pdfc.pro  $Revision: 12.85 $

;function t_pdfc,rv,df
;length = long(n_elements(rv))
;rv1 = double(rv)
;integral = double(rv)
;df = double(df)
;passtype = bytarr(4)
;passtype(1) = 0
;passtype(2) = 1
;passtype(3) = 0
;stat=call_external(!SHARE_LIB,'_t_pdfc',integral,rv1,length,df,VALUE=passtype)
;return,integral
;end

function t_pdfc,rv,df
length = long(n_elements(rv))
ndf = long(n_elements(df))
dfstack = double(df)
if length gt 1 then begin
    if ndf eq 1 then $
        dfstack = make_array(length,/DOUBLE,VALUE=double(df)) $
    else if ndf lt length then begin
        stat=dialog_message('length='+strtrim(length,2)+' ndf='+strtrim(ndf,2)+' Must be equal!',/ERROR)
        return,!ERROR
   endif
endif 
rv1 = double(rv)
integral = dblarr(length)
status = lonarr(length)
passtype = bytarr(5)
passtype[2] = 1
stat=call_external(!SHARE_LIB,'_t_pdfc',integral,rv1,length,dfstack,status,VALUE=passtype)
return,integral
end
