;Copyright 12/31/99 Washington University.  All Rights Reserved.
;get_font_list.pro  $Revision: 1.10 $
pro get_font_list,fnt,font_sizes,font_names,nfonts
; Return a list of adobe fonts satisfying the restrictiuons of the fnt structure.


font_filter = '\-adobe\-' + fnt.family

if((fnt.bold eq !TRUE) and (fnt.family ne 'symbol')) then $
    font_filter = font_filter + '\-bold\-' $
else $
    font_filter = font_filter + '\-medium\-' 

if(fnt.italic eq !FALSE) then begin
    font_filter = font_filter + 'r'
endif else begin
    if(fnt.family eq 'times') then begin
        font_filter = font_filter + 'i'
    endif else if(fnt.family eq 'symbol') then begin
        font_filter = font_filter + 'r'
    endif else begin
        font_filter = font_filter + 'o'
    endelse
endelse

font_filter = font_filter + '\-normal'

cmd0 = 'xlsfonts | grep ' + "'" + font_filter + "'"
cmd = cmd0 + "| gawk " + "'" + '{FS=' + '"'+ '-' + '"' + '; if(($10 == 75)&&($9 >= 100)) print $0}' + "'"
spawn,cmd,font_names,COUNT=nfonts

print,cmd
print,'font_names'
print,font_names

idx = where(strmatch(font_names,'*iso8859*'),nfonts)

print,'nfonts=',nfonts
print,'idx=',idx

if nfonts eq 0 then begin
    print,'font_names'
    print,font_names
    spawn,'xlsfonts',xlsfonts
    print,'xlsfonts'
    print,xlsfonts
    stat=dialog_message('No font_names *iso8859* found in get_font_list',/ERROR)
    return
endif
font_names = font_names[idx]
font_sizes = strarr(nfonts)
for i=0,nfonts-1 do begin

    ;str = str_sep(font_names[i],'-')
    ;font_sizes[i] = str[8]
    ;START120912
    str = strsplit(font_names[i],'-',/EXTRACT)
    font_sizes[i] = str[6]

    ;print,font_names[i]
    ;for j=0,n_elements(str)-1 do print,'    j=',j,' str[j]=',str[j]
endfor
end
