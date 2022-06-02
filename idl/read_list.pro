;Copyright 9/21/2000 Washington University.  All Rights Reserved.
;read_list.pro  $Revision: 1.35 $

;function read_list,glm_list
;START170216
function read_list,glm_list,SKIP=skip

openr,lu,glm_list,/GET_LUN
scrap = strpos(glm_list,'/')
path = strmid(glm_list,scrap,strpos(glm_list,'/',/REVERSE_SEARCH)-scrap+1)
identify=''
files=''
t4=''
glm=''
conc=''
dtseries=''
dscalar=''
img=''
imgt=''
ev=''
txt=''
vals=''
ext=''
dtseriest=''
dscalart=''
nii=''
niit=''

;START160819
replace=''

;START210127
wmparc=''
wmparct=''

while not EOF(lu) do begin
    s = ''
    readf,lu,s
    s = strtrim(s,2)
    colon = strpos(s,":")+1
    pound = strmid(s,0,1)
    if s eq '' then begin
        ;do nothing
    endif else if pound[0] eq '#' then begin
        print,'read_list Ignoring '+s 
    endif else if colon gt 0 then begin
        s1 = strtrim(strsplit(s,':',/EXTRACT),2)
        if n_elements(s1) lt 2 then begin
            ;do nothing
        endif else if strpos(s1[0],"number_of_lists") ge 0 then begin 
            ;do nothing 
        endif else if strpos(s1[0],"number_of_files") ge 0 then begin 
            ;do nothing 
        endif else if(strpos(s1[0],"file") ge 0) then begin 
            s = s1[1]
            goto,spider
        endif else $ 
            goto,spider 
    endif else begin
        spider:
        scrap = strsplit(s,/EXTRACT)
        if n_elements(scrap) gt 1 then identify = [identify,scrap[1]]
        files = [files,scrap[0]]
    endelse
endwhile
close,lu
free_lun,lu
nfiles = n_elements(files)-1
if nfiles eq 0 then begin

    ;stat=get_button(['go back','exit'],BASE_TITLE=glm_list,TITLE='No files detected')
    ;if stat eq 1 then return,rtn={msg:'EXIT'} else return,rtn={msg:'GOBACK'}
    ;START170216
    if not keyword_set(SKIP) then begin
        stat=get_button(['go back','exit'],BASE_TITLE='ERROR',TITLE=glm_list+string(10B)+'No files detected') 
        if stat eq 1 then return,rtn={msg:'EXIT'} else return,rtn={msg:'GOBACK'}
    endif else begin
        stat=get_button(['skip this one and continue','go back','exit'],BASE_TITLE='ERROR',TITLE=glm_list+string(10B) $
            +'No files detected') 
        if stat eq 0 then return,rtn={msg:'SKIP'} else if stat eq 1 then return,rtn={msg:'EXIT'} else return,rtn={msg:'GOBACK'}
    endelse

endif
files=files[1:*]
gf=get_filetype(files)
if gf.msg ne 'OK' then return,rtn={msg:gf.msg}
idx = where(strpos(files,"/") eq -1,cnt)
if cnt ne 0 then files[idx] = path + files[idx]
idx = where(gf.filetype eq 't4',nt4)
if nt4 ne 0 then t4 = files[idx]
idx = where(gf.filetype eq 'glm',nglm)
if nglm ne 0 then begin
    glm = files[idx]
    if nt4 gt 0 and nglm ne nt4 then begin 
        stat=get_button(['ok','go back','exit'],BASE_TITLE='Warning',TITLE='You have '+trim(nglm)+' glms, but '+trim(nt4)+' t4s.' $
            +string(10B)+'Should be the same number of each.')
        if stat eq 2 then return,rtn={msg:'EXIT'} else if stat eq 1 then return,rtn={msg:'GOBACK'}
    endif
endif
idx = where(gf.filetype eq 'conc',nconc)
if nconc ne 0 then begin
    conc = files[idx]
    if nt4 gt 0 and nconc ne nt4 then begin 
        stat=get_button(['ok','go back','exit'],BASE_TITLE='Warning',TITLE='You have '+trim(nconc)+' concs, but '+trim(nt4) $
            +' t4s.'+string(10B)+'Should be the same number of each.')
        if stat eq 2 then return,rtn={msg:'EXIT'} else if stat eq 1 then return,rtn={msg:'GOBACK'}
    endif
endif

;START160819
idx = where(gf.filetype eq 'replace',nreplace)
if nreplace ne 0 then begin
    replace = files[idx]
    if nconc gt 0 and nconc ne nreplace then begin
        stat=get_button(['ok','go back','exit'],BASE_TITLE='Warning',TITLE='You have '+trim(nconc)+' concs, but '+trim(nreplace) $
            +' replacement concs.'+string(10B)+'Should be the same number of each.')
        if stat eq 2 then return,rtn={msg:'EXIT'} else if stat eq 1 then return,rtn={msg:'GOBACK'}
    endif
endif


idx = where(gf.filetype eq 'dtseries',ndtseries)
if ndtseries ne 0 then begin
    dtseries = files[idx]
    dtseriest = gf.filetype[idx]
endif
idx = where(gf.filetype eq 'dscalar',ndscalar)
if ndscalar ne 0 then begin
    dtscalar = files[idx]
    dtscalart = gf.filetype[idx]
endif
idx = where(gf.filetype eq 'nii',nnii)
if nnii ne 0 then begin
    nii = files[idx]
    niit = gf.filetype[idx]
endif

;START210127
idx = where(gf.filetype eq 'wmparc',nwmparc)
if nwmparc ne 0 then begin
    wmparc = files[idx]
    wmparct = gf.filetype[idx]
endif


;idx = where(gf.filetype eq 'img',nimg)
;if nimg ne 0 then begin
;    img = files[idx]
;    imgt = gf.filetype[idx]
;    if nconc gt 0 and nconc ne nimg then begin 
;        lizard='Fidl puts '
;        for i=0,nimg/nconc-1 do begin
;            lizard = lizard+'img'+trim(nconc*i+1)+' with conc1, img'+trim(nconc*i+2)+' with conc2,..., ' $
;                +'img'+trim(nconc*i+nconc)+' with conc'+trim(nconc)
;            if i lt nimg/nconc-1 then lizard = lizard+','+string(10B)
;        endfor
;        stat=get_button(['ok','go back','exit'],BASE_TITLE='Information',TITLE='You have '+trim(nconc)+' concs, but '+trim(nimg) $
;            +' imgs.'+string(10B)+lizard)
;        if stat eq 2 then return,rtn={msg:'EXIT'} else if stat eq 1 then return,rtn={msg:'GOBACK'}
;    endif
;endif
;START190131
idx = where(gf.filetype eq 'img',nimg)
if nimg ne 0 then begin
    img = files[idx]
    imgt = gf.filetype[idx]
endif
if nimg+nnii ne 0 then begin
    if nconc gt 0 and nconc ne nimg+nnii then begin
        lizard='Fidl puts '
        for i=0,(nimg+nnii)/nconc-1 do begin
            lizard = lizard+'img'+trim(nconc*i+1)+' with conc1, img'+trim(nconc*i+2)+' with conc2,..., ' $
                +'img'+trim(nconc*i+nconc)+' with conc'+trim(nconc)
            if i lt (nimg+nnii)/nconc-1 then lizard = lizard+','+string(10B)
        endfor
        stat=get_button(['ok','go back','exit'],BASE_TITLE='Information',TITLE='You have '+trim(nconc)+' concs, but ' $
            +trim(nimg+nnii)+' imgs.'+string(10B)+lizard)
        if stat eq 2 then return,rtn={msg:'EXIT'} else if stat eq 1 then return,rtn={msg:'GOBACK'}
    endif
endif




idx = where(gf.filetype eq 'ev',nev)
if nev ne 0 then begin
    ev = files[idx]

;STARTHERE

    ;if nev gt 0 and nev ne nconc and nglm eq 0 then begin 
    ;    stat=get_button(['ok','go back','exit'],BASE_TITLE='Warning',TITLE='You have '+trim(nev)+' event files, but '+trim(nconc) $
    ;        +' concs.'+string(10B)+'Should be the same number of each.') 
    ;    if stat eq 2 then return,rtn={msg:'EXIT'} else if stat eq 1 then return,rtn={msg:'GOBACK'}
    ;endif
    ;START170216
    if nev gt 0 and nev ne nconc and nglm eq 0 then begin 
        stat=get_button(['ok','go back','exit'],BASE_TITLE='Warning',TITLE=glm_list+string(10B)+'has '+trim(nev)+' event files, ' $
            +'but '+trim(nconc)+' concs.') 
        if stat eq 2 then return,rtn={msg:'EXIT'} else if stat eq 1 then return,rtn={msg:'GOBACK'}
    endif

endif
idx = where(gf.filetype eq 'txt',ntxt)
if ntxt ne 0 then begin
    txt = files[idx]
    if ntxt gt 0 and ntxt ne nconc then begin 
        stat=get_button(['ok','go back','exit'],BASE_TITLE='Warning',TITLE='You have '+trim(ntxt)+' scrub files, but ' $
            +trim(nconc)+' concs.'+string(10B)+'Should be the same number of each.') 
        if stat eq 2 then return,rtn={msg:'EXIT'} else if stat eq 1 then return,rtn={msg:'GOBACK'}
    endif
endif
idx = where(gf.filetype eq 'vals',nvals)
if nvals ne 0 then begin
    vals = files[idx]
    if nvals gt 0 and nconc gt 0 and nvals ne nconc then begin
        stat=get_button(['ok','go back','exit'],BASE_TITLE='Warning',TITLE='You have '+trim(nvals)+' vals files, but ' $
            +trim(nconc)+' concs.'+string(10B)+'Should be the same number of each.')
        if stat eq 2 then return,rtn={msg:'EXIT'} else if stat eq 1 then return,rtn={msg:'GOBACK'}
    endif
endif
idx = where(gf.filetype eq 'ext',next)
if next ne 0 then begin
    ext = files[idx]
    if next gt 0 and nconc gt 0 and next ne nconc then begin
        stat=get_button(['ok','go back','exit'],BASE_TITLE='Warning',TITLE='You have '+trim(next)+' ext files, but '+trim(nconc) $
            +' concs.'+string(10B)+'Should be the same number of each.')
        if stat eq 2 then return,rtn={msg:'EXIT'} else if stat eq 1 then return,rtn={msg:'GOBACK'}
    endif
endif
nidentify = n_elements(identify)-1
if nidentify gt 0 then identify=identify[1:*] 

;return,rtn={msg:'OK',nt4:nt4,t4:t4,nglm:nglm,glm:glm,nconc:nconc,conc:conc,nimg:nimg,img:img,nev:nev,ev:ev, $
;    ntxt:ntxt,txt:txt,nidentify:nidentify,identify:identify,nvals:nvals,vals:vals,next:next,ext:ext,ndtseries:ndtseries, $
;    dtseries:dtseries,ndscalar:ndscalar,dscalar:dscalar,nnii:nnii,nii:nii,dtseriest:dtseriest, $
;    dscalart:dscalart,niit:niit,imgt:imgt,lcnii:gf.lcnii}
;START160819
;return,rtn={msg:'OK',nt4:nt4,t4:t4,nglm:nglm,glm:glm,nconc:nconc,conc:conc,nimg:nimg,img:img,nev:nev,ev:ev, $
;    ntxt:ntxt,txt:txt,nidentify:nidentify,identify:identify,nvals:nvals,vals:vals,next:next,ext:ext,ndtseries:ndtseries, $
;    dtseries:dtseries,ndscalar:ndscalar,dscalar:dscalar,nnii:nnii,nii:nii,dtseriest:dtseriest, $
;    dscalart:dscalart,niit:niit,imgt:imgt,lcnii:gf.lcnii,nreplace:nreplace,replace:replace}
;START210127
return,rtn={msg:'OK',nt4:nt4,t4:t4,nglm:nglm,glm:glm,nconc:nconc,conc:conc,nimg:nimg,img:img,nev:nev,ev:ev, $
    ntxt:ntxt,txt:txt,nidentify:nidentify,identify:identify,nvals:nvals,vals:vals,next:next,ext:ext,ndtseries:ndtseries, $
    dtseries:dtseries,ndscalar:ndscalar,dscalar:dscalar,nnii:nnii,nii:nii,dtseriest:dtseriest, $
    dscalart:dscalart,niit:niit,imgt:imgt,lcnii:gf.lcnii,nreplace:nreplace,replace:replace,nwmparc:nwmparc,wmparc:wmparc,wmparct:wmparct}

end
