;Copyright 7/19/02 Washington University.  All Rights Reserved.
;compute_voxel_value.pro  $Revision: 1.10 $
pro compute_voxel_value_event,ev
    common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw
    common compute_voxel_value_comm,wd_button,wd_button_str,filename,wd_text,wd_ok,wd_thresh,wd_mode,modes,wd_exit,wd_filter, $
        filter_out,wd_echo
    path = fi.path
    case ev.id of
        wd_button[0]: begin
            labels = strarr(fi.nfiles+1)
            if fi.nfiles gt 0 then labels[0:fi.nfiles-1] = fi.tails[0:fi.nfiles-1]
            labels[fi.nfiles] = 'On disk'
            idx_zstat = get_button([labels,'Cancel'],TITLE=wd_button_str[0])
            if idx_zstat eq fi.nfiles+1 then begin 
                ;do nothing 
            endif else begin
                if idx_zstat eq fi.nfiles then begin
                    repeat begin
                        if load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,/NOLOAD_COLOR) eq !OK then begin
                            idx_zstat = fi.n
                            scrap = 1
                            if !D.WINDOW eq -1 then begin
                                dsp_image = get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + dsp[fi.cw].plane-1,fi,stc)
                                refresh_display,fi,dsp,stc,wd,pref,IMAGE=dsp_image
                            endif
                        endif else begin
                            print,' *** Error while loading file. ***'
                            scrap = get_button(['Try again.','Exit'],TITLE='Error loading file.')
                        endelse
                    endrep until scrap eq 1
                endif
                filename[0] = get_filename_from_index(fi,idx_zstat)
                widget_control,wd_text[0],SET_VALUE=filename[0]
            endelse
        end
        wd_button[1]: begin
            widget_control,wd_filter,GET_VALUE=filter
            filter = filter[0]
            if filter eq '' then filter = '*.dat'
            get_dialog_pickfile,filter,path,wd_button_str[1],rtn_filenames,rtn_nfiles,rtn_path,/DONT_ASK
            if rtn_filenames ne '' and rtn_filenames ne 'GOBACK' and rtn_filenames ne 'EXIT' then begin 
                filename[1] = rtn_filenames
                widget_control,wd_text[1],SET_VALUE=filename[1]
                if filename[2] eq '' then begin
                    if strpos('.txt',filename[1],/REVERSE_SEARCH) eq -1 then filter_out = '.txt' else filter_out = '.out'
                    filename[2] = filename[1] + filter_out 
                    filter_out = '*' + filter_out
                    widget_control,wd_text[2],SET_VALUE=filename[2] 
                endif
            endif
        end
        wd_button[2]: begin
            get_dialog_pickfile,filter_out,path,wd_button_str[2],rtn_filenames,rtn_nfiles,rtn_path,/DONT_ASK
            if rtn_filenames ne '' and rtn_filenames ne 'GOBACK' and rtn_filenames ne 'EXIT' then begin
                filename[2] = rtn_filenames
                widget_control,wd_text[2],SET_VALUE=filename[2] 
            endif
        end
        wd_text[0]: begin
            widget_control,wd_text[0],GET_VALUE=scrap
            filename[0] = scrap
            if load_data(fi,st,dsp,wd,stc,!MRI_4DFP,help,glm,FILENAME=filename[0],/NOLOAD_COLOR) eq !OK then begin
                if !D.WINDOW eq -1 then begin
                    dsp_image = get_image(dsp[fi.cw].zdim*(dsp[fi.cw].frame-1) + dsp[fi.cw].plane-1,fi,stc)
                    refresh_display,fi,dsp,stc,wd,pref,IMAGE=dsp_image
                endif
            endif 
        end
        wd_ok: begin
            for i=0,2 do begin
                widget_control,wd_text[i],GET_VALUE=scrap
                filename[i] = scrap
            endfor
            filename = strtrim(filename,2)
            index = where(filename[0:1] eq '',count)
            if count ne 0 then begin
                scrap = ''
                for i=0,count-1 do scrap = scrap + wd_button_str[index[i]] + string(10B)
                stat = widget_message(scrap,/INFORMATION)
            endif else begin
                widget_control,wd_thresh,GET_VALUE=thresh_str
                widget_control,wd_mode,GET_VALUE=scrap
                mode_str = modes[scrap]
                str = !BINEXECUTE+'/compute_voxel_value'
                str = str + ' -image_files ' + filename[0]
                str = str + ' -point_files ' + filename[1]
                if filename[2] ne '' then str = str + ' -output_names ' + filename[2]
                str = str + ' -threshold ' + thresh_str
                str = str + mode_str 
                print,'******************'
                print,'HERE IS THE SCRIPT'
                print,'******************'
                print,str
                print,''
                spawn,str,result
                widget_control,wd_echo,SET_VALUE=result
            endelse
        end
        wd_exit: begin
            widget_control,ev.top,/DESTROY
        end
        else: ;do nothing
    endcase
end
pro compute_voxel_value
    common stats_comm
    common compute_voxel_value_comm
    topbase = widget_base(/COLUMN,TITLE='Voxel Values')
    wd_label = widget_label(topbase,VALUE='Please press a button or edit a field.',/ALIGN_CENTER)
    row1 = widget_base(topbase,/ROW)
    row2 = widget_base(topbase,/ROW)
    row3 = widget_base(topbase,/ROW)
    row4 = widget_base(topbase,/ROW)
    row5 = widget_base(topbase,/ROW)
    wd_button = lonarr(3)
    wd_button[0] = widget_button(row1,VALUE='Load image')
    wd_button[1] = widget_button(row2,VALUE='Load points')
    wd_button[2] = widget_button(row3,VALUE='Load output')
    default_str = 'This field is editable.'
    wd_text = lonarr(3)
    wd_text[0] = widget_text(row1,VALUE=default_str,/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=600)
    wd_text[1] = widget_text(row2,VALUE=default_str,/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=600)
    wd_filter = cw_field(row2,TITLE="Filter:",VALUE='*.dat',XSIZE=5)
    wd_thresh = cw_field(row2,TITLE="Threshold:",VALUE='0',XSIZE=5)
    wd_text[2] = widget_text(row3,VALUE=default_str,/EDITABLE,SCR_YSIZE=40,SCR_XSIZE=600)
    wd_mode = cw_bgroup(row3,['Append','Overwrite'],/RETURN_INDEX,SET_VALUE=0,FRAME=2,/EXCLUSIVE,ROW=1,YSIZE=30)
    wd_ok = widget_button(row4,VALUE='OK')
    wd_exit = widget_button(row4,VALUE='Exit')
    wd_echo = widget_text(row5,YSIZE=20,XSIZE=100,/SCROLL)
    wd_button_str = ['Please select image.','Please select points file (af3d or text).','Please select output_file.']
    filename = strarr(3)
    modes = [' -append',' -overwrite']
    filter_out = '*.txt'
    widget_control,topbase,/REALIZE
    xmanager,'compute_voxel_value',topbase
end
