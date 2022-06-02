;Copyright 8/25/14 Washington University.  All Rights Reserved.
;fidl_threshclus.pro  $Revision: 1.8 $
pro fidl_threshclus,fi,wd,dsp,help,pref,dsp_image,stc,IDL=idl
efilestr=''
respectstr=''
goback0:
gg=get_glms(pref,fi,GET_THIS='imgs')
if gg.msg eq 'EXIT' then return
nF=0
for i=0,gg.total_nsubjects-1 do if gg.ifh[i].datades eq 'gausssianizedF' then nF=nF+1
goback1:
act_type = intarr(3)
act_type[0]=1
lizard=['all','pos','neg']
act_type_str = ' -all'
if nF ne gg.total_nsubjects then begin    
    rtn = select_files(['two sided (positives and negatives)','one sided (positives only)','one sided (negatives only)'], $
        TITLE='Please select',/CANCEL,/EXIT,/ONE_COLUMN,/GO_BACK)
    if rtn.files[0] eq 'GO_BACK' then goto,goback0 else if rtn.files[0] eq 'EXIT' then return
    act_type = rtn.list
    act_type_str = ' '+strjoin('-'+lizard[rtn.index],' ',/SINGLE)
    undefine,rtn
end
goback2:
threshold_extent = get_str(2,['threshold','min. # of face connected voxels'],['3','21'],TITLE='Please specify.',/GO_BACK,/EXIT,ADD=2)
if threshold_extent[0] eq 'EXIT' then return else if threshold_extent[0] eq 'GO_BACK' then begin
    if nF ne gg.total_nsubjects then goto,goback1 else begin
        undefine,gg
        goto,goback0
    endelse
endif
threshold_extent = trim(threshold_extent)
testr=' -threshold_extent'
for i=0,n_elements(threshold_extent)-1,2 do testr = testr + ' "' + strjoin(threshold_extent[i:i+1],' ',/SINGLE) + '"'
goback3:
outtypestr=''
rtn = get_bool_list(['map','region file'],TITLE='Please select output',/GO_BACK,/EXIT)
if rtn.list[0] eq -2 then return else if rtn.list[0] eq -1 then begin
    goto,goback2
endif
if rtn.list[0] eq 1 and rtn.list[1] eq 0 then outtypestr = ' -map_only' $
else if rtn.list[0] eq 0 and rtn.list[1] then outtypestr = ' -regions_only'
undefine,rtn
goback4:
regnamestr=''
lcpeak=0
lccom=0
if outtypestr ne ' -map_only' then begin
    scrap = get_button(['peak','center of mass','both','go back'],TITLE='Regions should be named')
    if scrap eq 3 then goto,goback3
    if scrap eq 0 then begin
        regnamestr = ' -peak'
        lcpeak=1
    endif else if scrap eq 1 then begin
        regnamestr = ' -center_of_mass'
        lccom=1
    endif else if scrap eq 2 then begin
        regnamestr = ' -peak -center_of_mass'
        lcpeak=1
        lccom=1
    endif
endif
if not keyword_set(IDL) then begin
    goback4b:
    spawn,!BINEXECUTE+'/fidl_timestr2',timestr
    csh = 'fidl_threshclus'+'_'+timestr+'.csh'
    scrap = get_str(1,'script',csh,WIDTH=100,TITLE='Please name',/GO_BACK,/EXIT)
    if scrap[0] eq 'EXIT' then return else if scrap[0] eq 'GO_BACK' then goto,goback4
    csh = fix_script_name(scrap[0])
    action = get_button(['execute','return','go back','exit'],TITLE='Please select',BASE_TITLE=csh[0])
    if action eq 3 then return else if action eq 2 then goto,goback4b
    openw,lu,csh[0],/GET_LUN
    top_of_script,lu
    for i=0,gg.total_nsubjects-1 do printf,lu,'nice +19 $BIN'+'/fidl_threshclus'+' -zmap '+gg.imgselect[i]+testr+act_type_str $
        +regnamestr+outtypestr+efilestr+respectstr
    close,lu
    free_lun,lu
    spawn,'chmod +x '+csh
    print,csh[0]+' has been written to disk'
    if action eq 0 then spawn_cover,csh[0],fi,wd,dsp,help,stc
endif else begin
    gr = get_root(gg.imgselect,'.4dfp.img')
    nreg = 0L
    nvox = 0L
    for i=0,gg.total_nsubjects-1 do begin
        idx = gg.imgselecti[i] 

        ;START170327
        ;if idx eq -1 then begin

            stat = load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,/NOLOAD_COLOR,FILENAME=gg.imgselect[i]) 
            if stat eq !OK then begin
                idx = fi.n
                if !D.WINDOW eq -1 then dsp_image = update_image(fi,dsp,wd,stc,pref)
            endif else begin
                print,' *** Error while loading file. ***'
                scrap = get_button(['Skip this one','Exit'],TITLE='Error loading '+gg.imgselect[i])
                if scrap eq 1 then return
            endelse

        ;START170327
        ;endif

        if idx ne -1 then begin
            hdr = *fi.hdr_ptr[idx]
            zimage = fltarr(hdr.xdim,hdr.ydim,hdr.zdim,/NOZERO)
            actmask = fltarr(hdr.xdim,hdr.ydim,hdr.zdim)
            vol = long(hdr.xdim)*long(hdr.ydim)*long(hdr.zdim)
            reglist = fltarr(vol)
            regsize = fltarr(vol)
            widget_control,/HOURGLASS
            for z=0,hdr.zdim-1 do zimage[*,*,z] = get_image(z,fi,stc,FILNUM=idx+1)
            for j=0,2 do begin ;0:all 1:pos 2:neg
                if act_type[j] eq 1 then begin
                    for k=0,n_elements(threshold_extent)/2-1 do begin
                        zimage1 = zimage
                        actmask1 = actmask
                        if call_external(!SHARE_LIB,'_spatial_extent2',zimage1,actmask1,float(threshold_extent[k*2]),lizard[j], $
                            hdr.xdim,hdr.ydim,hdr.zdim,long(threshold_extent[k*2+1]),nreg,nvox,reglist,regsize, $
                            VALUE=[0,0,0,1,1,1,1,1,0,0,0,0]) eq 0L then return
                        root = gr.file[i]+'_z'+threshold_extent[k*2]+'n'+threshold_extent[k*2+1]+'_'+lizard[j]
                        if outtypestr ne ' -regions_only' then begin
                            ifh = hdr.ifh
                            put_image,zimage1,fi,wd,dsp,root,hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl, $
                                !FLOAT_ARRAY,hdr.mother,IFH=ifh
                            fi.color_scale1[fi.n] = !GRAY_SCALE
                            fi.color_scale2[fi.n] = !NO_SCALE
                            load_colortable,fi,dsp,TABLE1=fi.color_scale1[fi.n],TABLE2=fi.color_scale2[fi.n]
                        endif
                        if outtypestr ne ' -map_only' and nreg ne 0 then begin
                            coor = fltarr(nreg*3)
                            peakcoor = fltarr(nreg*3)
                            peakval = fltarr(nreg)
                            if call_external(!SHARE_LIB,'_center_of_mass2',zimage1,vol,nvox,nreg,reglist,regsize,coor,peakcoor, $
                                peakval,VALUE=[0,1,1,1,0,0,0,0,0]) eq 0L then return
                            regnames = strarr(nreg)
                            for l=0,nreg-1 do begin

                                ;regname=call_external(!SHARE_LIB,'_assign_region_names2',coor[l*3:l*3+2],peakcoor[l*3:l*3+2], $
                                ;    peakval[l],regsize[l],lccom,lcpeak,VALUE=[0,0,0,1,1,1],/S_VALUE)
                                ;START140908
                                regname=call_external(!SHARE_LIB,'_assign_region_names2',coor[l*3:l*3+2],peakcoor[l*3:l*3+2], $
                                    peakval[l],long(regsize[l]),lccom,lcpeak,VALUE=[0,0,0,1,1,1],/S_VALUE)

                                if regname eq 'NULL' then return
                                regnames[l] = regname
                                undefine,regname
                            endfor
                            ifh = hdr.ifh
                            if ptr_valid(ifh.region_names) then ptr_free,ifh.region_names
                            ifh.nreg = nreg
                            ifh.region_names = ptr_new(regnames)
                            put_image,actmask1,fi,wd,dsp,root+'_reg',hdr.xdim,hdr.ydim,hdr.zdim,1,!FLOAT,hdr.dxdy,hdr.dz,hdr.scl, $
                                !FLOAT_ARRAY,hdr.mother,IFH=ifh
                            fi.color_scale1[fi.n] = !GRAY_SCALE
                            fi.color_scale2[fi.n] = !NO_SCALE
                            load_colortable,fi,dsp,TABLE1=fi.color_scale1[fi.n],TABLE2=fi.color_scale2[fi.n]
                            undefine,peakval,peakcoor,coor
                        endif
                    endfor
                endif
            endfor
            undefine,regsize,reglist,vol,hdr
        endif
    endfor
endelse
undefine,gg,lizard
print,'DONE'
end
