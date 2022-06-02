;Copyright 12/31/99 Washington University.  All Rights Reserved.
;save_conc.pro  $Revision: 12.89 $
pro save_conc,fi,stc
hdr = *fi.hdr_ptr(fi.n)
if hdr.array_type ne !STITCH_ARRAY then begin
    stats = dialog_message('Invalid data type - must be concatenated file set',/ERROR)
    return
endif
file = fi.list[fi.n]
if strpos(file,'.conc') eq -1 then file = file + '.conc'
get_dialog_pickfile,'*.conc',fi.path,'Please select or enter file to write.',filename,rtn_nfiles,rtn_path,FILE=file
if filename eq 'GOBACK' or filename eq 'EXIT' then return
write_conc,stc,fi,filename
end
