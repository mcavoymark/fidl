;Copyright 12/31/99 Washington University.  All Rights Reserved.
;encode_paradigm.pro  $Revision: 12.84 $

;*******************************************
function encode_paradigm,paradigm_in,tdim_in
;*******************************************

; Encode paradigm into ASCII string as defined by Avi.

if((total(paradigm_in^2) eq 0.) or (paradigm_in[0] lt -1)) then $
    return,'0x'
if(n_elements(paradigm_in) lt tdim_in+1) then $
    tdim = n_elements(paradigm_in)-1 $
else $
    tdim = tdim_in
paradigm = fltarr(tdim+1)
paradigm(0:tdim-1) = paradigm_in(0:tdim-1)
paradigm(tdim) = 5000
skip0 = 0
rest1 = 0
rest = 0
act = 0
skip = 0
skip1 = 0
skip2 = 0
rpt = 0
t = 0
while(paradigm(t) eq 0) do begin
    skip0 = skip0 + 1
    t = t + 1
end
while(paradigm(t) eq -1) do begin
    rest1 = rest1 + 1
    t = t + 1
end
while(paradigm(t) eq 0) do begin
    skip = skip + 1
    t = t + 1
end
if(skip eq 0) then $
    code = string(skip0,rest1,FORMAT='(i3,"x",i3,"-")') $
else $
    code = string(skip0,rest1,skip,FORMAT='(i3,"x",i3,"-",i3,"x")')

len = intarr(tdim)
type = intarr(tdim)
run = 0
while(t lt tdim) do begin
    type[run] = paradigm[t]
    len[run] = 1
    t = t + 1
    while(paradigm(t) eq type[run]) do begin
        len[run] = len[run] + 1
        t = t + 1
    end
    run = run + 1
end

typecode = ['-','x','+']
t = 0
while(t lt run) do begin
    rpt_len = 2
    rpt = 1
    while(rpt_len lt run-t) do begin
        t1 = t + rpt_len - 1
        t2 = t + 2*rpt_len - 1
        if(t2 gt run-1) then $
            t2 = run - 1
        if(total(len[t:t1] eq len[t1+1:t2]) eq rpt_len) then begin
            if(total(type[t:t1] eq type[t1+1:t2]) eq rpt_len) then begin
;               Repeated segment.
                rpt = rpt + 1
                t = t + rpt_len
                rpt_len_save = rpt_len
            endif else begin
                rpt_len = rpt_len + 1
            endelse
        endif else begin
             if(rpt gt 1) then begin
		code = string(code,rpt,FORMAT='(a,i3,"(")')
                for t1=t,t+rpt_len-1 do $
		    code = string(code,len[t1],typecode[type[t1]+1], $
							FORMAT='(a,i3,a)')
	        code = string(code,FORMAT='(a,")")')
                t = t + rpt_len_save
                rpt_len = run + 1
             endif else begin
                 rpt_len = rpt_len + 1
             endelse
        endelse
    end
    if(rpt eq 1) then begin
        code = string(code,len[t],typecode[type[t]+1], $
							FORMAT='(a,i3,a)')
        t = t + 1
    endif
end

code = strcompress(code,/REMOVE_ALL)

return,code
end
