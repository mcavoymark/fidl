;Copyright 11/14/01 Washington University.  All Rights Reserved.
;af3d_loci.pro  $Revision: 1.11 $

;*********************
pro af3d_loci_event,ev
;*********************
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg,glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

common view_images_comm_shared,lc_refreshed,prf,win,lc_color_bar_once,wd_hidecolors,wd_showcolors_vert, $
    wd_showcolors_horiz,current_label,current_cells,display_mode,af3dh

common af3d_loci_comm,space,af3d_files,wd_filename,wd_msymbol,wd_plot,wd_exit,topbase,nshapes,ncolors,atlas_space 

;print,'here top af3d_loci_event'

index = where(wd_filename eq ev.id,count)
if count gt 0 then begin
    scrap = af3d_files[index] eq space
    win.af3d_checked[index] = abs(win.af3d_checked[index]-1)
    if scrap[0] then begin
        if read_af3d(fi,wd,dsp,help,atlas_space,af3dp1) ne !ERROR then begin
            filenames=''
            scrap = *af3dp1.files
            for i=0,af3dp1.nfiles-1 do begin
                result = strsplit(scrap[i],'/',/EXTRACT)
                filenames = filenames + result[n_elements(result)-1] + ', '
            endfor
            filenames = strmid(filenames,0,strlen(filenames)-2)
            widget_control,ev.id,SET_VALUE=filenames
            af3d_files[index]=filenames
            win.af3dp_new[index] = af3dp1
            *win.af3dp_new[index].z = *win.af3dp_new[index].z - 1
        endif else begin
            widget_control,ev.id,SET_BUTTON=0
            win.af3d_checked[index] = 0
        endelse
    endif
    ;print,'wd_filename win.af3d_checked=',win.af3d_checked
    lc_refreshed = !FALSE 
endif

index = where(wd_msymbol eq ev.id,count)
if count gt 0 then begin
    ;scrap = !PIXMAP_PATH+'/'+win.af3d_new[index].shape+'_'+win.af3d_new[index].color+'_'+win.af3d_new[index].fill	
    af3dptr = ptr_new(win.af3d_new[index])
    af3d_loci_symbol,af3dptr,topbase,af3dh,nshapes,ncolors,prf.points_size
    win.af3d_new[index] = *af3dptr
    ptr_free,af3dptr
    scrap_new = !PIXMAP_PATH+'/'+win.af3d_new[index].shape+'_'+win.af3d_new[index].color+'_'+win.af3d_new[index].fill	
    ;if scrap ne scrap_new then lc_refreshed = !FALSE
    lc_refreshed = !FALSE
    widget_control,ev.id,SET_VALUE=scrap_new,/BITMAP,SET_BUTTON=0
endif

if ev.id eq wd_plot then af3d_loci_plot
if ev.id eq wd_exit then widget_control,ev.top,/DESTROY

;print,'here bottom af3d_loci_event'
end

;*****************
pro af3d_loci_plot
;*****************
common stats_comm
common view_images_comm_shared

print,'af3d_loci_plot top'
print,'win.af3d_checked=',win.af3d_checked

index = where(win.af3d_checked,count)
if count ne 0 then begin
    for j=0,count-1 do begin
        print,'here2'
        af3d_x = *win.af3dp_new[index[j]].x
        print,'here3'
        af3d_y = *win.af3dp_new[index[j]].y
        af3d_z = *win.af3dp_new[index[j]].z
        ;print,'af3d_x=',af3d_x
        ;print,'af3d_y=',af3d_y
        ;print,'af3d_z=',af3d_z
        nindex = win.af3dp_new[index[j]].nindex
        ;delete cells start
        if win.ntrans le 0 then return
        translation_table = win.translation_table
        ncells = win.ntrans
        cell_list = lindgen(win.ntrans)
        ;delete cells end
        vp = intarr(3,nindex*ncells)
        nvp = 0
        for i=0,nindex-1 do begin
            for itr=0,ncells-1 do begin
                ic = win.translation_table[cell_list[itr]]
                ;print,'win.cell.sclice=',win.cell[ic].slice,' af3d_z=',af3d_z[i]
                if win.cell[ic].slice eq af3d_z[i] then begin
                    vp[*,nvp] = get_image_coords(af3d_x[i],af3d_y[i],af3d_z[i],win.cell[ic].x,win.cell[ic].y, $
                        win.cell[ic].view,win.atlas_space,win.cell[ic].xpad,win.cell[ic].ypad,prf.zoom, $
                        win.sprf[win.idxidx].sagittal_face_left,prf.orientation,win.cell[ic].xdim,win.cell[ic].ydim, $
                        win.ydim_draw,display_mode)
                    nvp = nvp + 1
                endif
            endfor
        endfor
        if nvp gt 0 then begin
            vp = vp[*,0:nvp-1]


            ;if ptr_valid(win.labels) then $
            ;    *win.af3dpts_new[index[j]] = vp $
            ;else $
            ;    win.af3dpts_new[index[j]] = ptr_new(vp)

            win.af3dpts_new[index[j]] = ptr_new(vp)


            win.naf3dpts_new[index[j]] = nvp
            print,index[j],' win.naf3dpts_new=',win.naf3dpts_new[index[j]]
        endif
    endfor
endif
view_images_refresh,fi,dsp,vw.fnt

print,'af3d_loci_plot bottom'
end



;***************************
pro af3d_loci,atlas_space_in
;***************************
common stats_comm
common view_images_comm_shared
common af3d_loci_comm

atlas_space = atlas_space_in

;af3dh = { $
;    shape:['square','circle','diamond','trianglen','triangles','trianglew','trianglee','plus','x'], $
;    psym: [   8    ,   8    ,    8    ,     8     ,     8     ,     8     ,     8     ,  1   , 7 ], $
;    color:['white','yellow','red','green','blue','orange','magenta','cyan','brown','black'], $
;    fill:['unfilled.m.pm','filled.m.pm'] $
;    }

nshapes = n_elements(af3dh.shape)
ncolors = n_elements(af3dh.color)

space = '                                                                      '
topbase = widget_base(/COLUMN,TITLE='Plot Atlas Points')
labelbase = widget_label(topbase,VALUE='Filename')
prefbase = widget_base(topbase,/ROW)
colbase_symbol = widget_base(prefbase,/COLUMN,/EXCLUSIVE)
colbase = widget_base(prefbase,/COLUMN,/NONEXCLUSIVE)
rowbase = widget_base(topbase,/ROW)


af3d_files = strarr(!NAF3D_FILES)
wd_filename = lonarr(!NAF3D_FILES)
;win.af3d_checked = intarr(!NAF3D_FILES)
for i=0,!NAF3D_FILES-1 do begin
    af3d_files[i] = space 
    wd_filename[i] = widget_button(colbase,VALUE=af3d_files[i],SCR_YSIZE=25)
endfor

pixmap_file = strarr(!NAF3D_FILES)
pixmap_file[0] = 'square_magenta_filled.m.pm'
pixmap_file[1] = 'circle_cyan_unfilled.m.pm'
pixmap_file[2] = 'diamond_orange_filled.m.pm'
pixmap_file[3] = 'trianglen_brown_unfilled.m.pm'
pixmap_file[4] = 'triangles_red_filled.m.pm'
pixmap_file[5] = 'trianglew_green_unfilled.m.pm'
pixmap_file[6] = 'trianglee_blue_filled.m.pm'
pixmap_file[7] = 'square_yellow_unfilled.m.pm'
pixmap_file[8] = 'circle_black_filled.m.pm'
pixmap_file[9] = 'diamond_white_unfilled.m.pm'

wd_msymbol = lonarr(!NAF3D_FILES)
for i=0,!NAF3D_FILES-1 do $ 
    wd_msymbol[i] = widget_button(colbase_symbol,VALUE=!PIXMAP_PATH+'/'+pixmap_file[i],/BITMAP,SCR_YSIZE=25,/NO_RELEASE)

wd_plot = widget_button(rowbase,value='Plot')
wd_exit = widget_button(rowbase,value='Exit')

;;af3d = {Af3d_symbol_pref}
;;af3d = replicate(af3d,!NAF3D_FILES)
;af3d = replicate({Af3d_symbol_pref},!NAF3D_FILES)

for i=0,!NAF3D_FILES-1 do begin
    scrap = strsplit(pixmap_file[i],'_',/EXTRACT)

    ;af3d[i].shape = scrap[0]
    ;af3d[i].color = scrap[1]
    ;af3d[i].fill = scrap[2]

    win.af3d_new[i].shape = scrap[0]
    win.af3d_new[i].color = scrap[1]
    win.af3d_new[i].fill = scrap[2]

    index = where(win.af3d_new[i].shape eq af3dh.shape)
    win.af3d_new[i].shape_index = index[0]
    win.af3d_new[i].psym = af3dh.psym[index[0]]
    index = where(win.af3d_new[i].color eq af3dh.color)
    win.af3d_new[i].color_index = index[0]

    ;print,'here win.af3d_new[i].shape_index=',win.af3d_new[i].shape_index
    ;print,'here win.af3d_new[i].color_index=',win.af3d_new[i].color_index
    ;help,af3d[i],/STRUCTURE
endfor

widget_control,topbase,/REALIZE
xmanager,'af3d_loci',topbase
end
