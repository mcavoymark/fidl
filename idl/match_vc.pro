;Copyright 6/25/08 Washington University.  All Rights Reserved.
;match_vc.pro  $Revision: 1.7 $
function match_vc,ngroups,files,nfiles_per_group,roots,SEARCHINDEX=searchindex,SEARCHROOT=searchroot
if not keyword_set(SEARCHINDEX) then searchindex = -1
if not keyword_set(SEARCHROOT) then searchroot = 'vc'
files1=files
nfiles1=n_elements(files1)
roots1=roots
if nfiles1 ne n_elements(roots1) then begin
    stat=dialog_message('The number of files does not match the number of roots.',/ERROR)
    return,rtn={msg:'GO_BACK'}
endif
nfiles1_per_group=nfiles_per_group
if searchindex[0] eq -1 then begin 
    pos=strpos(roots1,searchroot)
    lsearchroot = strlen(searchroot)
    index=where(pos eq -1,count)
    if count ne 0 then begin
        stat=dialog_message_long('INFORMATION','The following files will be excluded because they lack the search root numbers.' $
            +string(10B)+string(10B)+strjoin(strtrim(files1[index],2),string(10B),/SINGLE))
        if stat eq 'EXIT' then return,rtn={msg:'EXIT'} else if stat eq 'GOBACK' then return,rtn={msg:'GO_BACK'} 
        if count eq nfiles1 then begin
            stat=dialog_message('No search root found.',/ERROR)
            return,rtn={msg:'GO_BACK'}
         endif 
        junk1=intarr(ngroups)
        junk1[0]=nfiles1_per_group[0]
        for i=1,ngroups-1 do junk1[i]=junk1[i-1]+nfiles1_per_group[i]
        for j=0,count-1 do begin
            i=0
            flag=0
            repeat begin
                if index[j] lt junk1[i] then begin
                    nfiles1_per_group[i]=nfiles1_per_group[i]-1
                    flag=1
                endif
                i=i+1
            endrep until flag eq 1
        endfor
    endif
    newindex=where(pos gt -1,newcount)
    files1 = files1[newindex]
    roots1 = roots1[newindex]
    pos = pos[newindex]
    for i=0,newcount-1 do begin
        scrap=strsplit(strmid(roots1[i],pos[i]+lsearchroot),'[^0-9]',/REGEX,/EXTRACT)
        roots1[i] = scrap[0]
    endfor
endif else begin
    newindex = indgen(nfiles1)
    maxsearchindex = max(searchindex)
    nsearchindex = n_elements(searchindex)
    searcharr = make_array(maxsearchindex+2,/STRING,VALUE='?') 
    searcharr[maxsearchindex+1]='*'
endelse
nsubject_per_group = intarr(ngroups)
newjunk = ''
nsubject = 0
subject_names = ''
nfiles_per_subject=-1
no_match = ''
nno_match = 0
index=-1
jj=0
widget_control,/HOURGLASS
for i=0,ngroups-1 do begin
    junkwpath=files1[jj:jj+nfiles1_per_group[i]-1]
    junkwopath=roots1[jj:jj+nfiles1_per_group[i]-1]
    junkindex=newindex[jj:jj+nfiles1_per_group[i]-1]
    scrap=0
    repeat begin
        if searchindex[0] eq -1 then begin 
            searchstr = junkwopath[0]
        endif else begin
            for j=0,nsearchindex-1 do searcharr[searchindex[j]]=strmid(junkwopath[0],searchindex[j],1) 
            searchstr = strjoin(searcharr,'',/SINGLE)
        endelse
        subject_names = [subject_names,searchstr]
        idx = where(strmatch(junkwopath,searchstr) eq 1,count)
        if count eq 0 then begin
            no_match = [no_match,junkwpath[0]]
            junkwopath = junkwopath[1:*]
            junkwpath = junkwpath[1:*]
            junkindex = junkindex[1:*]
            count=1
            nno_match = nno_match + 1
        endif else begin
            scrap = scrap + count 
            nfiles_per_subject = [nfiles_per_subject,count]
            newjunk = [newjunk,junkwpath[idx]]
            index = [index,junkindex[idx]]
            notidx = where(strmatch(junkwopath,searchstr) eq 0,count)
            if count ne 0 then begin
                junkwopath = junkwopath[notidx]
                junkwpath = junkwpath[notidx]
                junkindex = junkindex[notidx]
            endif
            nsubject = nsubject + 1
            nsubject_per_group[i] = nsubject_per_group[i] + 1
        endelse
    endrep until count eq 0
    nfiles1_per_group[i]=scrap
    jj = jj + nfiles_per_group[i]
endfor
if nno_match ne 0 then begin
    no_match = no_match[1:*]
    stat=dialog_message_long('INFORMATION','No matches were found for the following files.' $
        +string(10B)+string(10B)+strjoin(strtrim(no_match,2),string(10B),/SINGLE))
    if stat eq 'EXIT' then return,rtn={msg:'EXIT'} else if stat eq 'GOBACK' then return,rtn={msg:'GO_BACK'} 
    if nno_match eq newcount then return,rtn={msg:'GO_BACK'}
endif
files1 = newjunk[1:*]
nfiles1 = n_elements(files1)
nfiles_per_subject = nfiles_per_subject[1:*]
subject_names = subject_names[1:*]
index = index[1:*]
return,rtn={msg:'OK',files:files1,nfiles_per_group:nfiles1_per_group,nfiles_per_subject:nfiles_per_subject,nfiles:nfiles1, $
    subject_names:subject_names,nsubject:nsubject,index:index,nsubject_per_group:nsubject_per_group}
end
