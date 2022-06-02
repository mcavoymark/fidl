;Copyright 10/31/12 Washington University.  All Rights Reserved.
;expand_filenames.pro  $Revision: 1.4 $

;function expand_filenames,fi,filenames,labels
;START170712
function expand_filenames,fi,filenames,labels,label
if n_elements(label) eq 0 then label='Files'

out = filenames
repeat begin
    stat = 'again'
    scrap=strarr(n_elements(filenames))
    widget_control,/HOURGLASS
    for i=0,n_elements(filenames)-1 do begin
        ;spawn,'find '+filenames[i],result,/STDERR
        spawn,'find '+out[i],result,/STDERR
        scrap[i] = result
    endfor
    widget_control,/HOURGLASS
    idx=where(strpos(scrap,'No such file or directory') ne -1,cnt,COMPLEMENT=idx0)
    if cnt eq 0 then begin 
        ;out=filenames
        stat='ok'
    endif else begin
        if idx0[0] ne -1 then out[idx0]=scrap[idx0]

        ;scrap = get_str(cnt,labels[idx],out[idx],TITLE='Please try again.',LABEL='Files not found.',/ONE_COLUMN,/BELOW, $
        ;    SPECIAL='Browse - one at a time',/GO_BACK,/EXIT,FRONT=getenv('PWD')+'/')
        ;START170712
        scrap = get_str(cnt,labels[idx],out[idx],TITLE='Please try again.',LABEL=label+' not found.',/ONE_COLUMN,/BELOW, $
            SPECIAL='Browse - one at a time',/GO_BACK,/EXIT,FRONT=getenv('PWD')+'/')

        if scrap[0] eq 'GO_BACK' then return,'GO_BACK' else if scrap[0] eq 'EXIT' then return,'EXIT'
        if scrap[0] eq 'SPECIAL' then begin
            for i=0,cnt-1 do begin
                get_dialog_pickfile,'*img',fi.path,'Please select file for '+labels[idx[i]],file,nfile,path
                if file eq 'GOBACK' then return,'GO_BACK' else if file eq 'EXIT' then return,'EXIT'
                out[idx[i]] = file
            endfor
        endif else $
            out[idx]=scrap 
    endelse
endrep until stat ne 'again' 
return,out
end
