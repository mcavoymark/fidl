;Copyright 07/09/14 Washington University.  All Rights Reserved.
;fidl_fdr.pro  $Revision: 1.4 $
pro fidl_fdr,fi,wd,dsp,help,pref,dsp_image,stc
scrap = get_button(['Continue','Exit'],BASE_TITLE='False discovery rate',TITLE='This program corrects for the false discovery rate ' $
    +'with a cluster based method (Chumbley and Friston Neuroimage 2009).'+string(10B)+'You provide the threshold and q.')
if scrap eq 1 then return

;mailstr = ' |& mail mcavoy@npg.wustl.edu'

goback0:
gg = get_glms(pref,fi,GET_THIS='imgs')
if gg.msg ne 'OK' then return
datades = gg.ifh[*].datades
idxF = where(datades eq 'gaussianizedF',cntF)
idxT = where(datades eq 'gaussianizedT',cntT)

print,'idxF=',idxF
print,'idxT=',idxT
print,'gg.total_nsubjects=',gg.total_nsubjects

;START160721
datadesstr=make_array(gg.total_nsubjects,/STRING,VALUE='')

if cntF+cntT ne gg.total_nsubjects then begin
    idxnot = where(gg.ifh[*].datades ne 'gaussianizedF' and gg.ifh[*].datades ne 'gaussianizedT',cntnot)
    gr=get_root(gg.imgselect[idxnot])
    lizard=strjoin(gr.file,string(10B),/SINGLE)
    scrap = get_button(['Yes','No'],BASE_TITLE='Unknown statistical image',TITLE=lizard+string(10B)+string(10B) $
       +'Are these gaussianized T statistics?')
    if scrap eq 0 then datades[idxnot] = 'gaussianizedT' else return

    ;START160715
    idxT = where(datades eq 'gaussianizedT',cntT)
    
    ;START160721
    if cntT ne 0 then datadesstr[idxT]=' -gaussT'

endif
;-gaussF



goback1:
scrap=get_str(2,['threshold','q'],['3.0','0.05'],/GO_BACK,TITLE='FDR parameters',ADD=2,LABEL='Friston and Chumbley recommend a ' $
    +'threshold of at least 2.5')
if scrap[0] eq 'GO_BACK' then goto,goback0
threshstr = ' -thresh '+strjoin(trim(scrap[indgen(n_elements(scrap)/2)*2]),' ',/SINGLE)
qstr = ' -q '+strjoin(trim(scrap[indgen(n_elements(scrap)/2)*2+1]),' ',/SINGLE)

print,'threshstr=',threshstr
print,'qstr=',qstr

;START160718
if total(gg.bold_space-gg.bold_space[0]) gt 0 then begin
    print,'gg.bold_space'
    print,gg.bold_space
    print,'Warning: Not all images are in the same space.'
endif


;START160715
masks=make_array(gg.total_nsubjects,/STRING,VALUE='')
idxm=where(gg.ifh[*].mask eq '',cntm)
if cntm ne 0 then begin
    masks=gg.ifh[*].mask
    spaces=gg.bold_space[uniq(gg.bold_space,sort(gg.bold_space))]
    goback2:
    for i=0,n_elements(spaces)-1 do begin
        spacestr=get_space_str(spaces[i])
        idx=where(gg.bold_space eq spaces[0],cnt)
        print,'The following images are '+spacestr 
        print,gg.imgselect[idx]
        mask=get_mask(spaces[i],fi,BASE_TITLE='Please select mask for '+spacestr+' images.')
        if mask eq 'GO_BACK' then goto,goback1
        masks[idx]=mask
    endfor
    masks=' -mask '+masks
endif

fwhms=make_array(gg.total_nsubjects,/STRING,VALUE='')
idxf=where(gg.ifh[*].fwhm eq '',cntfwhm,COMPLEMENT=idxf0)
if cntfwhm ne 0 then begin
    goback3:
    str=get_str_bool(1,'How much have your images been smoothed?',['voxels','mm'],'',0) 
    if str[0] eq 'GOBACK' then begin
        if cntm ne 0 then goto,goback2 else goto,goback1
    endif
    if str[0] eq 'EXIT' then return
    if fix(str[1]) eq 1 then fwhms=float(str[0])/gg.ifh[*].scale_1
    if idxf0[0] ne -1 then fwhms[idxf0]=gg.ifh[idxf0].fwhm
    fwhms=' -fwhm '+fwhms
endif


;START160815
goback4:
outtypestr=''
rtn = get_bool_list(['map','region file'],TITLE='Please select output',/GO_BACK,/EXIT)
if rtn.list[0] eq -2 then return else if rtn.list[0] eq -1 then begin
    if cntfwhm ne 0 then goto,goback3 else if cntm ne 0 then goto,goback2 else goto,goback1 
endif
if rtn.list[0] eq 1 and rtn.list[1] eq 0 then outtypestr = ' -map_only' $
else if rtn.list[0] eq 0 and rtn.list[1] then outtypestr = ' -regions_only'
undefine,rtn
goback5:
regnamestr=''
if outtypestr ne ' -map_only' then begin
    scrap = get_button(['peak','center of mass','both','go back','exit'],TITLE='Regions should be named')
    if scrap eq 4 then return else if scrap eq 3 then goto,goback4 
    if scrap eq 0 then begin
        regnamestr = ' -peak'
    endif else if scrap eq 1 then begin
        regnamestr = ' -center_of_mass'
    endif else if scrap eq 2 then begin
        regnamestr = ' -peak -center_of_mass'
    endif
endif


if cntF ne 0 then begin
    lizard = make_array(gg.total_nsubjects,/INTEGER,VALUE=-1)
    j=0
    for i=0,cntF-1 do begin  
        if lizard[idxF[i]] eq -1 then begin
            idx11 = indgen(cntF-i)+i
            idx1 = where(gg.ifh[idxF[i]].df1 eq gg.ifh[idxF[idx11]].df1,cnt1)
            idx2 = where(gg.ifh[idxF[i]].df2 eq gg.ifh[idxF[idx11[idx1]]].df2,cnt2)
            lizard[idxF[idx11[idx1[idx2]]]] = j
            j=j+1
        endif
    endfor
    cshF = strarr(j)
    for i=0,j-1 do begin
        idx = where(lizard eq i,cnt)
        simstr = ' -df1 '+trim(gg.ifh[idx[0]].df1)+' -df2 '+trim(gg.ifh[idx[0]].df2)+' -fwhm '+trim(gg.ifh[idx[0]].fwhm) $
            +' -mask '+gg.ifh[idx[0]].mask+outtypestr+regnamestr
        filestr=' -files '+strjoin(gg.imgselect[idx],' ',/SINGLE)
        spawn,!BINEXECUTE+'/fidl_timestr2',timestr
        cshF[i] = 'fidl_fdr_'+timestr[0]+'.csh'
        openw,lu,cshF[i],/GET_LUN
        top_of_script,lu
        ;printf,lu,'nice +19 $BIN/fidl_fsim -nsim 1000'+simstr+threshstr+mailstr
        printf,lu,'nice +19 $BIN/fidl_fsim'+simstr+threshstr
        printf,lu,'nice +19 $BIN/fidl_fdr'+filestr+threshstr+qstr
        close,lu
        free_lun,lu
    endfor
endif
if cntT ne 0 then begin
    spawn,!BINEXECUTE+'/fidl_timestr2',timestr
    csh = 'fidl_fdr'+'_'+timestr+'.csh'
    goback6:
    scrap = get_str(1,'script',csh,WIDTH=100,TITLE='Please name',/GO_BACK,/EXIT)
    if scrap[0] eq 'EXIT' then return else if scrap[0] eq 'GO_BACK' then goto,goback5
    csh = fix_script_name(scrap[0])
    action = get_button(['execute','return','go back','exit'],TITLE='Please select',BASE_TITLE=csh[0])
    if action eq 3 then return else if action eq 2 then goto,goback6
    openw,lu,csh[0],/GET_LUN
    top_of_script,lu
    for i=0,cntT-1 do printf,lu,'nice +19 $BIN/fidl_fdr -file '+gg.imgselect[idxT[i]]+threshstr+qstr+masks[idxT[i]] $
        +fwhms[idxT[i]]+datadesstr[idxT[i]]+' -montecarlopath '+!MONTECARLO_PATH+outtypestr+regnamestr
    close,lu
    free_lun,lu
    spawn,'chmod +x '+csh
    print,csh[0]+' has been written to disk'
    if action eq 0 then spawn_cover,csh[0],fi,wd,dsp,help,stc
endif
print,'DONE'
end
