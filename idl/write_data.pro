;Copyright 12/31/99 Washington University.  All Rights Reserved.
;write_data.pro  $Revision: 12.107 $
function write_data,fi,st,dsp,wd,stc,img_type,help,cstm,pr,SELECTED_FILES=selected_files
if fi.nfiles ge !NUM_FILES then begin
    st.error_msg=string('Too many images loaded.')
    st.error = !TRUE
    stat = widget_message(st.error_msg,/ERROR)
    return,-1
endif
case img_type of
!GIF: begin
    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Writes either the main window (the image contained in ' $
        +'the main window) or the currently selected custom window to a GIF file.'
    id = get_id(fi,dsp,cstm,pr)
    if(id lt 0) then begin
       stat=dialog_message('*** No valid windows.  ***',/ERROR)
        return, -1
    endif
    wset,id
    gif_file = fi.list[fi.n]
    scrap = strpos(gif_file,'.4dfp.img',/REVERSE_SEARCH)
    if scrap ne -1 then gif_file = strmid(gif_file,0,scrap)
    gif_file = gif_file + '.gif'
    get_dialog_pickfile,'*.gif',fi.path,'Please specify file.',gif_file,rtn_nfiles,wrtpath,FILE=gif_file,/DONT_ASK
    if gif_file eq '' or gif_file eq 'GOBACK' or gif_file eq 'EXIT' then return,!ERROR
    if(ptr_valid(fi.color_lct1[fi.n])) then begin
        lct = *fi.color_lct1[fi.n]
        write_gif,gif_file(0),tvrd(),lct[*,0],lct[*,1],lct[*,2]
    endif else begin
        write_gif,gif_file(0),tvrd()
    endelse
    wset,dsp[fi.cw].image_index[fi.cw]
    widget_control,wd.error,SET_VALUE='Written to '+gif_file
    error = !FALSE
    fi.whereisit[fi.n] = !ON_DISK
end
!TIFF: begin
    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Writes either the main window (the image contained ' $
        +'in the main window) or the currently selected custom window to a GIF file.'
    id = get_id(fi,dsp,cstm,pr)
    if(id lt 0) then begin
       stat=widget_message('*** No valid windows.  ***',/ERROR)
        return, -1
    endif
    wset,id
    tif_file = strcompress(string(id,FORMAT='("custom_",i,".tif")'),/REMOVE_ALL)
    tif_file = fi.list[fi.n]
    scrap = strpos(tif_file,'.4dfp.img',/REVERSE_SEARCH)
    if scrap ne -1 then tif_file = strmid(tif_file,0,scrap)
    tif_file = tif_file + '.tif'
    get_dialog_pickfile,'*.tif',fi.path,'Please specify file.',tif_file,rtn_nfiles,wrtpath,FILE=tif_file,/DONT_ASK
    if tif_file eq '' or tif_file eq 'GOBACK' or tif_file eq 'EXIT' then return,!ERROR
    img = tvrd()
    img[0:32,0:9] = 0
    img = reverse(img,2)
    img = reverse(img,1)
    if(ptr_valid(fi.color_lct1[fi.n])) then begin
        lct = *fi.color_lct1[fi.n]
        write_tiff,tif_file(0),img,RED=lct[*,0],GREEN=lct[*,1],BLUE=lct[*,2],COMPRESSION=1
    endif else begin
        write_tiff,tif_file(0),img
    endelse
    wset,dsp[fi.cw].image_index[fi.cw]
    widget_control,wd.error,SET_VALUE='Written to '+tif_file
    error = !FALSE

    fi.whereisit[fi.n] = !ON_DISK
end

!MRI_4DFP: begin
    if help.enable eq !TRUE then widget_control,help.id,SET_VALUE='Writes the currently selected file to a 4DFP format file. ' $
        +'This function is only valid for processed data. It will not save input data. File names are constructed by ' $
        +'appending .img and .rec to the displayed name.'
    if n_elements(fi.selected_files eq 1) then selected_files = fi.n else selected_files = fi.selected_files
    for ifile=0,fi.nselected-1 do begin

        ;START160415
        goback0:

        idx = selected_files[ifile]
        hdr = *fi.hdr_ptr[idx]
        if hdr.array_type eq !LINEAR_MODEL then begin
            stat = widget_message('Use the "GLM|Save Linear Model" button to save the results of the linear model.')
            return,-1
        endif
        filnam_4dfp = strcompress(fi.list[idx],/REMOVE_ALL)
        if strpos(fi.list[idx],'.4dfp.img',/REVERSE_SEARCH) eq -1 then filnam_4dfp = filnam_4dfp + '.4dfp.img' 
        scrap = ''
        for i=0,n_elements(filnam_4dfp)-1 do scrap = scrap + filnam_4dfp[i]
        filnam_4dfp = scrap
        get_dialog_pickfile,'*.img',getenv('PWD'),'Please specify file.',filnam_4dfp,rtn_nfiles,rtn_path,FILE=filnam_4dfp
        if filnam_4dfp eq 'GOBACK' or filnam_4dfp eq 'EXIT' then return,!ERROR
        fi.write_path = rtn_path
        fi.paths[idx] = rtn_path
        fi.names[idx] = filnam_4dfp
        dot = rstrpos(filnam_4dfp,'.')
        if dot gt 0 then $
            stem = strmid(filnam_4dfp,0,dot) $
        else $
            stem = filnam_4dfp
        dot = rstrpos(stem,'.')
        if dot gt 0 then $
            stemstem = strmid(stem,0,dot) $
        else $
            stemstem = stem
        if stemstem eq fi.names[idx] and fi.whereisit[idx] eq !ON_DISK then return,-1 else fi.whereisit[idx] = !ON_DISK
        if !VERSION.OS_FAMILY eq 'Windows' then begin
            if strpos(filnam_4dfp,'\',/REVERSE_SEARCH) lt 0 then filnam_4dfp = strcompress(wrtpath+filnam_4dfp,/REMOVE_ALL)
        endif else begin
            if strpos(filnam_4dfp,'/',/REVERSE_SEARCH) lt 0 then filnam_4dfp = strcompress(wrtpath+filnam_4dfp,/REMOVE_ALL)
        endelse
        if hdr.tdim gt 1 then begin
            str = get_str(1,"Number of frames to save",string(hdr.tdim))
            tdim = fix(str[0])
        endif else begin
            tdim = 1
        endelse
        error = write_4dfp(filnam_4dfp,fi,wd,dsp,help,stc,tdim,INDEX=idx)

        ;START160415
        if error eq 'EXIT' then return,error else if error eq 'GO_BACK' then goto,goback0

    endfor
end
else: begin
    stat=widget_message('Unknown file type.',/ERROR)
        st.error_msg = string('Unknown file type.')
        st.error = !TRUE
end
endcase
return,error
end
