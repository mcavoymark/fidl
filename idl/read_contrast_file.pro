;Copyright 9/14/04 Washington University.  All Rights Reserved.
;read_contrast_file.pro  $Revision: 1.6 $
function read_contrast_file,constr,wstr,float_cdata
stat=get_button(['ok','go back','exit'],BASE_TITLE='Information', $
    TITLE='FIDL assumes that each list is properly ordered.'+string(10B)+'Thus the ' $
    +'first file in each list should be the first matched pair.'+string(10B)+'The second file in each list ' $
    +'should be the second matched pair and so on.')
if stat eq 2 then return,c={name:'EXIT'} else if stat eq 1 then return,c={name:'GO_BACK'} 
get_directory,directory
get_dialog_pickfile,'*.txt',directory,'Please select contrast file.',cfile,n_cfile,rtn_path
if cfile eq 'GOBACK' or cfile eq 'EXIT' then return,c={name:'GO_BACK'}
widget_control,/HOURGLASS
c = readf_ascii_file(cfile)
;print,'c=',c
;print,'c.name=',c.name
;print,'c.NR=',c.NR
;print,'c.NF=',c.NF
;print,'c.hdr=',c.hdr
;print,'c.data=',c.data
;print,'size(c.data)=',size(c.data)
;print,'abs(total(float(c.data),2))=',abs(total(float(c.data),2))
task_label = c.hdr
float_cdata = float(c.data)
scrap = abs(total(float_cdata,2))
scrapstr = strtrim(scrap,2)
for i=0,c.NF-1 do begin
    if scrap[i] gt .01 then begin
        stat = get_button(['continue','exit'],TITLE='Contrast '+task_label[i]+' sums to '+scrapstr[i]+' (unsigned).'+string(10B) $
            +'A valid contrast sums to zero.',BASE_TITLE='Warning')
        if stat eq 1 then return,c={name:'GO_BACK'}
    endif
endfor
constr = strarr(c.NF) 
wstr = strarr(c.NF) 
for i=0,c.NF-1 do begin
    index = where(float_cdata[i,*] ne 0.,count)
    constr[i] = strjoin(strtrim(index+1,2),'+',/SINGLE)
    wstr[i] = strjoin(strtrim(c.data[i,index],2),',',/SINGLE)
endfor
return,c
end
