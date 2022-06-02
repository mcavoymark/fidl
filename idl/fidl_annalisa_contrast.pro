;Copyright 6/30/09 Washington University.  All Rights Reserved.
;fidl_annalisa_contrast.pro  $Revision: 1.8 $
pro fidl_annalisa_contrast_event,ev
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw
common fidl_annalisa_contrast_comm,wd_text,wd_reset,parameter_default,gtot_eff,gwstr,gwstr2,gwstr3,gwstr4, $
    wd_create,wd_exit,geffect_label,topbase,sumrows,index_conditions, $
    wd_text2,wd_text3,wd_text4,total_nsubjects,effectlabels, $
    ne_max,get_labels_struct,glmfiles,t4select,effect_length_ts,glm_list_str,wd_load,wd_save
index = where(wd_text eq ev.id,count)
if count ne 0 then begin
    dummy = wd_text[index]
    widget_control,dummy[0],GET_VALUE=scrap
    gwstr[index] = scrap
endif

index = where(wd_text2 eq ev.id,count)
if count ne 0 then begin
    dummy = wd_text2[index]
    widget_control,dummy[0],GET_VALUE=scrap
    gwstr2[index] = scrap
endif

index = where(wd_text3 eq ev.id,count)
if count ne 0 then begin
    dummy = wd_text3[index]
    widget_control,dummy[0],GET_VALUE=scrap
    gwstr3[index] = scrap
endif

index = where(wd_text4 eq ev.id,count)
if count ne 0 then begin
    dummy = wd_text4[index]
    widget_control,dummy[0],GET_VALUE=scrap
    gwstr4[index] = scrap
endif

if ev.id eq wd_reset then begin
    for i=0,gtot_eff-1 do begin
        widget_control,wd_text[i],SET_VALUE=parameter_default[i]
        widget_control,wd_text2[i],SET_VALUE=parameter_default[i]
        widget_control,wd_text3[i],SET_VALUE=parameter_default[i]
        widget_control,wd_text4[i],SET_VALUE=parameter_default[i]
    endfor
    gwstr[*]='0'
    gwstr2[*]='0'
    gwstr3[*]='0'
    gwstr4[*]='0'
endif
if ev.id eq wd_load then begin
    get_dialog_pickfile,'*.dat',fi.path,'Please select parameter file.',rtn_filenames,rtn_nfiles,rtn_path
    if rtn_filenames eq 'GOBACK' or rtn_filenames eq 'EXIT' then return
    cnt = readf_ascii_file(rtn_filenames)
    ;print,'cnt.NR=',cnt.NR,' cnt.NF=',cnt.NF,' cnt.nhdr=',cnt.nhdr,' cnt.NF_each_line=',cnt.NF_each_line
    ;print,'cnt.hdr=',cnt.hdr
    ;print,'cnt.data='
    ;print,cnt.data

    iL = where(cnt.hdr eq 'L',count)
    if count eq 0 then begin
        stat = dialog_message('L not present in '+rtn_filenames,/ERROR)
        goto,bottom
    endif
    iR = where(cnt.hdr eq 'R',count)
    if count eq 0 then begin
        stat = dialog_message('R not present in '+rtn_filenames,/ERROR)
        goto,bottom
    endif
    inum = where(cnt.hdr eq 'num',count)
    if count eq 0 then begin
        stat = dialog_message('num not present in '+rtn_filenames,/ERROR)
        goto,bottom
    endif
    iden = where(cnt.hdr eq 'den',count)
    if count eq 0 then begin
        stat = dialog_message('den not present in '+rtn_filenames,/ERROR)
        goto,bottom
    endif
    ;print,'iL=',iL,' iR=',iR,' inum=',inum,' iden=',iden 
    for i=0,cnt.NR-1 do begin
        ;print,'i=',i,' cnt.data[0,i]=',cnt.data[0,i]
        index = where(geffect_label eq cnt.data[0,i],count)
        if count ne 0 then begin
            ;print,'index=',index,' geffect_label=',geffect_label[index]
            gwstr[index] = cnt.data[1,i]    
            widget_control,wd_text[index[0]],SET_VALUE=cnt.data[1,i]
            gwstr2[index] = cnt.data[2,i]    
            widget_control,wd_text2[index[0]],SET_VALUE=cnt.data[2,i]
            gwstr3[index] = cnt.data[3,i]    
            widget_control,wd_text3[index[0]],SET_VALUE=cnt.data[3,i]
            gwstr4[index] = cnt.data[4,i]    
            widget_control,wd_text4[index[0]],SET_VALUE=cnt.data[4,i]
        endif
    endfor
endif
if ev.id eq wd_save then begin
    index = where(float(gwstr) ne 0.,count)
    index2 = where(float(gwstr2) ne 0.,count2)
    index3 = where(float(gwstr3) ne 0.,count3)
    index4 = where(float(gwstr4) ne 0.,count4)
    if count+count2+count3+count4 ne 0. then begin 
        scrap = get_str(1,'parameter file','annacon.dat',WIDTH=50,TITLE='Please enter desired filename.',/GO_BACK)
        if scrap[0] eq 'GO_BACK' then goto,bottom
        paramname = fix_script_name(scrap[0])
        openw,lu,paramname,/GET_LUN
        printf,lu,'cond L R num den'
        index1234 = [index,index2,index3,index4]
        index1234 = index1234[uniq(index1234,sort(index1234))]
        s1 = strtrim(gwstr[index1234],2)
        s2 = strtrim(gwstr2[index1234],2)
        s3 = strtrim(gwstr3[index1234],2)
        s4 = strtrim(gwstr4[index1234],2)
        for i=0,n_elements(index1234)-1 do printf,lu,s1[i]+' '+s2[i]+' '+s3[i]+' '+s4[i]
        close,lu
        free_lun,lu
    endif          
endif

if ev.id eq wd_create then begin
    j=0
    treatment_str = ''
    dummy = ''
    wts_str = ''
    index = where(float(gwstr) ne 0.,count)
    if count ne 0 then begin
        sumrows[j]=count
        index_conditions[j,0:count-1]=index
        treatment_str = [treatment_str,'L']
        wts_str = wts_str + ' -L ' + strjoin(strtrim(gwstr[index],2),' ',/SINGLE)
        j=j+1
    endif
    index2 = where(float(gwstr2) ne 0.,count2)
    if count2 ne 0 then begin
        sumrows[j]=count2
        index_conditions[j,0:count2-1]=index2
        treatment_str = [treatment_str,'R']
        wts_str = wts_str + ' -R ' + strjoin(strtrim(gwstr2[index2],2),' ',/SINGLE)
        j=j+1
    endif
    ntc=j
    if count+count2 gt 0 then begin
        index12 = [index,index2]
        index12 = index12[uniq(index12,sort(index12))]
        index3 = where(float(gwstr3) ne 0.,count3)
        if count3 ne 0 then begin
            for i=0,count3-1 do begin
                idx = where(index3[i] eq index12,cnt)
                if cnt eq 0 then begin
                    stat = dialog_message('num: '+geffect_label[index3[i]]+' not present in either L or R.',/ERROR)
                    goto,bottom
                endif
            endfor
            wts_str = wts_str + ' -num ' + strjoin(strtrim(gwstr3[index3],2),' ',/SINGLE)
            index4 = where(float(gwstr4) ne 0.,count4)
            if count4 ne 0 then begin
                for i=0,count4-1 do begin
                    idx = where(index4[i] eq index12,cnt)
                    if cnt eq 0 then begin
                        stat = dialog_message('den: '+geffect_label[index4[i]]+' not present in either L or R.',/ERROR)
                        goto,bottom
                    endif
                endfor
                wts_str = wts_str + ' -den ' + strjoin(strtrim(gwstr4[index4],2),' ',/SINGLE)
            endif ;else begin
            ;    stat = dialog_message('den: Need to specify weights.',/ERROR)
            ;    goto,bottom
            ;endelse
        endif

        treatment_str = treatment_str[1:*]
        scrap = cft_and_scft(total_nsubjects,ntc,ne_max,sumrows,index_conditions,geffect_label,get_labels_struct,0, $
            glmfiles,treatment_str,0,dummy,effectlabels,/REPEATS)
        if scrap.sum_contrast_for_treatment[0,0,0] eq -2 then $
            goto,wdexit $
        else if scrap.sum_contrast_for_treatment[0,0,0] eq -1 then $
            goto,bottom
        sum_contrast_for_treatment = scrap.sum_contrast_for_treatment

        tc_length = intarr(ntc)
        scrap = intarr(total_nsubjects)
        for i=0,ntc-1 do begin
            for j=0,total_nsubjects-1 do scrap[j] = effect_length_ts[j,sum_contrast_for_treatment[j,i,0]-1]
            tc_length[i] = max(scrap)
        endfor

        sumrows_time_tc = make_array(ntc,max(tc_length),/INTEGER,VALUE=1)
        conditions_time_tc = intarr(ntc,max(tc_length),max(tc_length))
        for i=0,ntc-1 do conditions_time_tc[i,0:tc_length[i]-1,0] = indgen(tc_length[i]) + 1

        tc_frames_str = ' -tc_frames ' + strjoin(strtrim(tc_length,2),' ',/SINGLE)

        ncontrast=0
        if count3 gt 0 then begin
            ncontrast = count3+count4
            index_contrast = [index3,index4]
            sumrows_contrast = make_array(ncontrast,/INT,VALUE=1)
            label = geffect_label[index_contrast]
            scrap = cft_and_scft(total_nsubjects,ncontrast,ne_max,sumrows_contrast,index_contrast,geffect_label,get_labels_struct,$
                1,glmfiles,label,0,'Please select contrast.',effectlabels,/MATCH)
            if scrap.sum_contrast_for_treatment[0,0,0] eq -2 then $
                goto,wdexit $
            else if scrap.sum_contrast_for_treatment[0,0,0] eq -1 then $
                goto,bottom
            contrast = scrap.sum_contrast_for_treatment
        endif

        scrap = ['script','root']
        scraplabels = ['fidl_annalisa_contrast.csh','annacon']
        names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=50,TITLE='Please enter desired filenames.',/GO_BACK)
        if names[0] eq 'GO_BACK' then goto,bottom
        csh = fix_script_name(names[0])
        root_str = ' -root '+strtrim(names[1],2)

        ntreatments = [ntc,ncontrast]
        gauss_str = ''
        region_file = ''
        cols_or_rows_str = ''
        roi_str = ''
        avgstat_output_str = ''
        Nimage_name_str = ''
        Nimage_mask_str = ''
        delay_str = ''
        within_subject_sd_str = ''
        magnorm_str = ''
        tc_type = ''
        print_unscaled_mag_str = ''
        lcmag = ''
        directory_str = ''
        wfiles = ''
        lcsd = ''
        compute_avg_zstat_csh,csh,1,0,total_nsubjects,glmfiles,t4select,0,ntreatments,'','', $
            gauss_str,region_file,cols_or_rows_str,roi_str,avgstat_output_str,Nimage_name_str,Nimage_mask_str,delay_str, $
            within_subject_sd_str,magnorm_str, $
            tc_length,effect_length_ts,conditions_time_tc,sumrows_time_tc,sum_contrast_for_treatment, $
            tc_type,print_unscaled_mag_str,lcmag,glm_list_str,directory_str,contrast,wfiles,lcsd, $
            wts_str+root_str+tc_frames_str,1, $
            '',0,'fidl_annalisa_contrast'
        spawn,'chmod +x '+csh
        spawn,csh+'>'+csh+'.log &'
        stats = dialog_message(csh+string(10B)+string(10B)+'Script has been executed.',/INFORMATION)
        print,'DONE'
    endif
endif
if ev.id eq wd_exit then begin
    wdexit:
    widget_control,ev.top,/DESTROY
endif
bottom:
end

;*********************************
pro fidl_annalisa_contrast_widgets
;*********************************
common fidl_annalisa_contrast_comm
lettersize=19
scr_ysize = 40
y_row1 = gtot_eff*scr_ysize < 500
y_row2d = !DISPYMAX - 200 - y_row1
gwstr[*] = ''
x = max(strlen(geffect_label))*lettersize

x1 = max(strlen(geffect_label))*10

topbase = widget_base(/COLUMN,TITLE='Annalisa',/MODAL,GROUP_LEADER=!FMRI_LEADER)
row0 = widget_base(topbase,/ROW)
row00 = widget_base(row0,/COLUMN)
row01 = widget_base(row0,/COLUMN)
row02 = widget_base(row0,/COLUMN)
row03 = widget_base(row0,/COLUMN)
row04 = widget_base(row0,/COLUMN)
row1 = widget_base(topbase,/ROW,Y_SCROLL_SIZE=!DISPYMAX)
wl_base = widget_base(row1,/COLUMN)
text_base = widget_base(row1,/COLUMN)
text_base2 = widget_base(row1,/COLUMN)
text_base3 = widget_base(row1,/COLUMN)
text_base4 = widget_base(row1,/COLUMN)
row2a1 = widget_base(topbase,/ROW)
row2b = widget_base(topbase,/ROW)
row2b1 = widget_base(topbase,/ROW)


wd_cond = widget_label(row00,VALUE='cond',SCR_XSIZE=x1,/ALIGN_LEFT)
wd_lhemi = widget_label(row01,VALUE='L',SCR_XSIZE=100,/ALIGN_LEFT)
wd_rhemi = widget_label(row02,VALUE='R',SCR_XSIZE=100,/ALIGN_LEFT)
wd_idxnum = widget_label(row03,VALUE='num',SCR_XSIZE=100,/ALIGN_LEFT)
wd_idxden = widget_label(row04,VALUE='den',SCR_XSIZE=100,/ALIGN_LEFT)

wd_wl = lonarr(gtot_eff)
for i=0,gtot_eff-1 do wd_wl[i] = widget_label(wl_base,VALUE=geffect_label[i],SCR_YSIZE=40,/ALIGN_LEFT)

wd_text = lonarr(gtot_eff)
for i=0,gtot_eff-1 do wd_text[i] = widget_text(text_base,VALUE=parameter_default[i],/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=100, $
    /ALL_EVENTS)
wd_text2 = lonarr(gtot_eff)
for i=0,gtot_eff-1 do wd_text2[i] = widget_text(text_base2,VALUE=parameter_default[i],/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=100, $
    /ALL_EVENTS)
wd_text3 = lonarr(gtot_eff)
for i=0,gtot_eff-1 do wd_text3[i] = widget_text(text_base3,VALUE=parameter_default[i],/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=100, $
    /ALL_EVENTS)
wd_text4 = lonarr(gtot_eff)
for i=0,gtot_eff-1 do wd_text4[i] = widget_text(text_base4,VALUE=parameter_default[i],/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=100, $
    /ALL_EVENTS)

wd_reset = widget_button(row2b,VALUE='Reset',FRAME=2)
wd_create = widget_button(row2b,VALUE='Create',FRAME=2)
wd_exit = widget_button(row2b,VALUE='Exit',FRAME=2)

;START51
wd_load = widget_button(row2b,VALUE='Load',FRAME=2)
wd_save = widget_button(row2b,VALUE='Save',FRAME=2)

widget_control,topbase,/REALIZE
xmanager,'fidl_annalisa_contrast',topbase
end

;*************************
pro fidl_annalisa_contrast
;*************************
common fidl_annalisa_contrast_comm

scrap=get_glms()
if scrap.msg ne 'OK' then return
load = scrap.load
glmfiles = scrap.glmfiles
nlists = scrap.nlists
total_nsubjects = scrap.total_nsubjects
t4select = scrap.t4select
glm_list_str = scrap.glm_list
if glm_list_str ne '' then glm_list_str = ' -glm_list_file '+glm_list_str
get_labels_struct = replicate({Get_labels_struct},total_nsubjects)
for i=0,total_nsubjects-1 do begin
    get_labels_struct[i] = get_labels_from_glm(fi,dsp,wd,glm,help,pref,ifh,glmfiles[i],/CONTRASTS_ONLY)
    if get_labels_struct[i].ifh.glm_rev gt -17 then begin
        stat=dialog_message('Revision number for '+strmid(glmfiles[i],1)+' too old. Update file by resaving.',/ERROR)
        return
    endif
endfor
ne_max = max(get_labels_struct[*].ifh.glm_tot_eff,ne_max_glm_index,MIN=ne_min)
effect_label = *get_labels_struct[ne_max_glm_index].ifh.glm_effect_label
effect_length_ts = intarr(total_nsubjects,ne_max)
for i=0,total_nsubjects-1 do begin
    effect_length = *get_labels_struct[i].ifh.glm_effect_length
    effect_length_ts[i,0:get_labels_struct[i].ifh.glm_tot_eff-1] = effect_length[0:get_labels_struct[i].ifh.glm_tot_eff-1]
endfor
effectlabels = replicate(ptr_new(),total_nsubjects)
for m=0,total_nsubjects-1 do effectlabels[m] = get_labels_struct[m].ifh.glm_effect_label

gwstr = strarr(ne_max)
gwstr2 = strarr(ne_max)
gwstr3 = strarr(ne_max)
gwstr4 = strarr(ne_max)
parameter_default = make_array(ne_max,/STRING,VALUE='0')
sumrows = intarr(2)
index_conditions = fltarr(2,ne_max)

gtot_eff = ne_max 
geffect_label = effect_label 
fidl_annalisa_contrast_widgets
end
