;Copyright 4/3/08 Washington University.  All Rights Reserved.
;fidl_logic.pro  $Revision: 1.8 $
pro fidl_logic,fi,dsp,wd,stc,help,pref
outvalstrAND=''
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr1,idx1,'Map 1',TITLE='Please select 4dfp.') eq !ERROR then return
if(idx1 lt fi.nfiles) and (fi.whereisit[idx1] eq !INTERNAL) then begin
    filename1 = get_filename_from_index(fi,idx1)
    stat = write_4dfp(filname1,fi,wd,dsp,help,stc,hdr1.tdim,INDEX=idx1)
endif else begin
    f1 = fi.names[idx1]
    filename1 = fi.tails[idx1]
endelse

;print,'here0 fi.names[idx1]=',fi.names[idx1]
;print,'here0 fi.list[idx1]=',fi.list[idx1]
;print,'here0 fi.tails[idx1]=',fi.tails[idx1]

goback1:
if get_hdr_4dfp(fi,st,dsp,wd,stc,help,glm,pref,hdr2,idx2,'Map 2',TITLE='Please select 4dfp.') eq !ERROR then return
if(idx2 lt fi.nfiles) and (fi.whereisit[idx2] eq !INTERNAL) then begin
    filename2 = get_filename_from_index(fi,idx2)
    stat = write_4dfp(filname2,fi,wd,dsp,help,stc,hdr2.tdim,INDEX=idx2)
endif else begin
    f2 = fi.names[idx2]
    filename2 = fi.tails[idx2]
endelse

;print,'here0 fi.names[idx2]=',fi.names[idx2]
;print,'here0 fi.list[idx2]=',fi.list[idx2]
;print,'here0 fi.tails[idx2]=',fi.tails[idx2]

goback5:
scrap = ['script','map1 AND NOTmap2','NOTmap1 AND map2','map1 AND map2']
scraplabels = ['fidl_logic.csh',filename1+'_AND_NOT'+filename2+'.4dfp.img','NOT'+filename1+'_AND_'+filename2+'.4dfp.img', $
    filename1+'_AND_'+filename2+'.4dfp.img']
wallace=''
repeat begin
    scraplabels=get_str(n_elements(scrap),scrap,scraplabels,WIDTH=50,TITLE='Please enter desired filenames.', $
        LABEL='Leave blank any you do not want.'+wallace,/GO_BACK,FRONT='1_',BACK='_1')
    if scraplabels[0] eq 'GO_BACK' then goto,goback1
    darby=strpos(scraplabels,'.4dfp.img')
    lizard=strlen(scraplabels)

    ;print,'here0 lizard=',lizard

    ;for i=1,n_elements(scraplabels)-1 do begin 
    ;    if darby[i] eq -1 then scraplabels[i]=scraplabels[i]+'.4dfp.img' $
    ;    else if ((lizard[i]-9) ne darby[i]) then scraplabels[i]=scraplabels[i]+'.4dfp.img'
    ;endfor
    ;START140731
    idx = where(lizard[1:*] gt 0,cnt)
    if cnt ne 0 then begin 
        for i=0,cnt-1 do begin 
            i1 = idx[i]+1
            if darby[i1] eq -1 then scraplabels[i1]=scraplabels[i1]+'.4dfp.img' $
            else if ((lizard[i1]-9) ne darby[i1]) then scraplabels[i1]=scraplabels[i1]+'.4dfp.img'
        endfor
    endif


    idx=where(strlen(scraplabels) gt 255,cnt) 
    ;print,'idx=',idx
    if cnt ne 0 then wallace=string(10B)+strjoin(scrap[idx],', ',/SINGLE)+' cannot exceed 255 characters.' 
endrep until cnt eq 0
names = fix_script_name(scraplabels)
scrap = ''
if names[1] ne '' then scrap = scrap + ' -1ANDNOT2 '+names[1]
if names[2] ne '' then scrap = scrap + ' -NOT1AND2 '+names[2] 
if names[3] ne '' then begin
    scrap = scrap + ' -1AND2 '+names[3]
    scrap0 = get_button([filename1,filename2,'1','Sum','Go back','Exit'],BASE_TITLE='Output values for AND', $
        TITLE='Output values for AND')
    if scrap0 eq 5 then return else if scrap0 eq 4 then goto,goback1
    if scrap0 eq 0 then outvalstrAND=' -outvalAND map1' $
    else if scrap0 eq 1 then outvalstrAND=' -outvalAND map2' $
    else if scrap0 eq 2 then outvalstrAND=' -outvalAND 1' $
    else outvalstrAND=' -outvalAND sum'
endif
if scrap eq '' then begin
    scrap = get_button(['go back','exit'],TITLE='Everthing was blank.')
    if scrap eq 0 then goto,goback5 else return 
endif
csh = names[0] 
openw,lu,csh,/GET_LUN
top_of_script,lu
printf,lu,'nice +19 $BIN/fidl_logic -file1 '+f1+' -file2 '+f2+scrap+outvalstrAND
close,lu
free_lun,lu
spawn,'chmod +x '+csh
spawn_cover,csh,fi,wd,dsp,help,stc
end
