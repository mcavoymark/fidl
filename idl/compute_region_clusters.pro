;Copyright 11/14/00 Washington University.  All Rights Reserved.
;compute_region_clusters.pro  $Revision: 1.20 $
pro compute_region_clusters,fi,wd,dsp,help,stc
cluster_file_str = ''

;START120731
goback0:

rtn = get_regions(fi,wd,dsp,help)
if rtn.msg eq 'EXIT' or rtn.msg eq 'GO_BACK' then return 
region_names = rtn.region_names
region_str = rtn.region_str
num_roi = n_elements(region_names)
nregfiles = rtn.nregfiles
region_file = rtn.region_file
;print,'rtn.reg_by_filei=',rtn.reg_by_filei
;print,'rtn.ireg_by_file=',rtn.ireg_by_file

spawn,'whoami',whoami
whoami = whoami[0]

scrap = get_str(1,'number of clusters',num_roi,WIDTH=50,TITLE='Please enter number of clusters.', $
    LABEL='It is ok to enter more than you need.')

;START120731
num_clusters1 = fix(scrap[0])

goback1:

;num_clusters = fix(scrap[0])
;if num_clusters eq 1 then set_unset=2 else set_unset=0
;num_clusters = num_clusters + 1
;START120731
if num_clusters1 eq 1 then set_unset=2 else set_unset=0
num_clusters = num_clusters1 + 1

labels = strarr(num_clusters,num_roi)
for m=0,num_roi-1 do begin
    for k=0,num_clusters-1 do begin
        if k eq 0 then $
            labels[k,m] = 'ignore' $
        else $
            labels[k,m] = strcompress(string(k),/REMOVE_ALL)
    endfor
endfor
repeat begin
    scrap = 1
    widget_control,/HOURGLASS
    rtn = get_choice_list_many(region_names,labels,TITLE='Please assign regions to clusters.',/COLUMN_LAYOUT,/SET_DIAGONAL, $
        SET_UNSET=set_unset)
    list = rtn.list
    max_list = max(list)
    if max_list eq 0 then begin
        stat=dialog_message('No regions assigned to clusters.',/ERROR)
        scrap = get_button(['Yes. Please let me try again.','No. I want out of this program.'], $
            TITLE='Do you wish to assign regions to clusters?')
        if scrap eq 1 then begin
            print,'DONE'
            return
        endif
    endif
endrep until scrap eq 1



;lcdontseparate = get_button(['yes','no','go back'],TITLE='Put each cluster in a separate file?')
;if lcdontseparate eq 2 then goto,goback1
;roistr = strarr(max_list)
;if lcdontseparate eq 1 then begin
;    scrap = strarr(max_list+1) 
;    cluster_names = strarr(max_list+1)
;endif else begin
;    scrap = strarr(max_list) 
;    cluster_names = strarr(2*max_list) 
;endelse
;k = 0
;for i=1,max_list do begin
;    region_numbers = where(list eq i,count) + 1
;    if count gt 0 then begin
;        cluster_names[k] = ''
;        scrap[k] = 'cluster' + strcompress(string(k+1),/REMOVE_ALL)
;        roistr[k] = strjoin(strtrim(region_numbers,2),'+',/SINGLE)
;        cluster_names[k] = strjoin(region_names[region_numbers-1],'_and_',/SINGLE)
;        k = k + 1
;    endif
;endfor
;index = where(strlen(cluster_names) gt 500,count)
;if count ne 0 then begin
;    dummy = strtrim(indgen(k)+1,2)
;    for i=0,count-1 do cluster_names[index] = 'cluster' + dummy[index]
;endif
;if nregfiles gt 1 then $
;    cluster_names[k] = 'cluster.4dfp.img' $
;else begin
;    rtn_gr = get_root(region_file,'.4dfp.img')
;    cluster_names[k] = rtn_gr.file[0] + '_cluster.4dfp.img'
;endelse
;if lcdontseparate eq 1 then begin
;    scrap[k] = 'cluster filename'
;endif else begin
;    scrap=[scrap,'cluster '+strtrim(indgen(max_list)+1,2)+' filename']
;    for i=0,max_list-1 do cluster_names[max_list+i] = 'roi' + cluster_names[i] + '.4dfp.img'
;endelse
;START120418
roistr = strarr(max_list)
cluster_names=''
spider=-1
goose=-1
k = 0
for i=1,max_list do begin
    region_numbers = where(list eq i,count) + 1
    if count gt 0 then begin
        goose = [goose,i]
        roistr[k] = strjoin(strtrim(region_numbers,2),'+',/SINGLE)
        cluster_names = [cluster_names,strjoin(region_names[region_numbers-1],'_and_',/SINGLE)]
        k = k + 1
    endif else $
        spider = [spider,i]
endfor
if n_elements(spider) gt 1 then begin
    title = 'No regions were assigned to clusters '+strjoin(strtrim(spider[1:*],2),', ',/SINGLE)
    scrap = get_button(['Ok, proceed. I know what I am doing.','Go back. Let me try again.','exit'],TITLE=title, $
        BASE_TITLE=whoami+', do you know what you are doing?',WIDTH=50)
    if scrap eq 2 then return else if scrap eq 1 then goto,goback1
endif
scrap = 'cluster'+strtrim(goose[1:*],2)
cluster_names = cluster_names[1:*]
index = where(strlen(cluster_names) gt 500,count)
if count ne 0 then begin
    dummy = strtrim(indgen(k)+1,2)
    for i=0,count-1 do cluster_names[index] = scrap[index] 
endif

;START120731
;if nregfiles gt 1 then $
;    cluster_names = [cluster_names,'cluster.4dfp.img'] $
;else begin
;    rtn_gr = get_root(region_file,'.4dfp.img')
;    cluster_names = [cluster_names,rtn_gr.file[0] + '_cluster.4dfp.img']
;endelse
;lcdontseparate = get_button(['yes','no','go back'],TITLE='Put each cluster in a separate file?')
;if lcdontseparate eq 2 then goto,goback1
;if lcdontseparate eq 1 then begin
;    scrap = [scrap,'cluster filename']
;endif else begin
;    scrap=[scrap,'cluster '+strtrim(indgen(max_list)+1,2)+' filename']
;    for i=0,max_list-1 do cluster_names = [cluster_names,'roi' + cluster_names[i] + '.4dfp.img']
;endelse
;scrap = get_str(n_elements(scrap)+1,['script',scrap],['fidl_cluster.csh',cluster_names],WIDTH=50,TITLE='Please enter desired names.')
;START120731
lcdontseparate = get_button(['yes','no','go back'],TITLE='Put each cluster in a separate file?')
if lcdontseparate eq 2 then goto,goback1
if lcdontseparate eq 1 then begin
    if nregfiles gt 1 then $
        cluster_names = [cluster_names,'cluster.4dfp.img'] $
    else begin
        rtn_gr = get_root(region_file,'.4dfp.img')
        cluster_names = [cluster_names,rtn_gr.file[0] + '_cluster.4dfp.img']
    endelse
    scrap = [scrap,'cluster filename']
endif else begin
    scrap=[scrap,'cluster '+strtrim(indgen(max_list)+1,2)+' filename']
    goose=make_array(max_list,/STRING,VALUE='')
    idx = where(strpos(cluster_names,'roi') eq -1,cnt)
    if cnt ne 0 then goose[idx]='roi'
    cluster_names = [cluster_names,goose + cluster_names + '.4dfp.img']
endelse
scrap = get_str(n_elements(scrap)+1,['script',scrap],['fidl_cluster.csh',cluster_names],WIDTH=50, $
    TITLE='Please enter desired names.',/ONE_COLUMN)


csh = fix_script_name(scrap[0])
openw,lu,csh,/GET_LUN
top_of_script,lu
if lcdontseparate eq 1 then begin
    cluster_names_str = ' -clusters'
    for i=1,k do cluster_names_str = cluster_names_str + ' "' + scrap[i] + '"'
    if rstrpos(scrap[k+1],'.4dfp.img') eq -1 then scrap[k+1] = scrap[k+1] + '.4dfp.img'
    cluster_file_str = ' -cluster_file "' + scrap[k+1] + '"'
    cmd = '/compute_region_clusters'+region_str+' -regions_of_interest '+strjoin(roistr,' ',/SINGLE)+cluster_file_str $
        +cluster_names_str
    printf,lu,'nice +19 $BIN' + cmd
endif else begin

    ;kk = k + 1
    ;for i=0,max_list-1 do begin
    ;    if rstrpos(scrap[kk],'.4dfp.img') eq -1 then scrap[kk] = scrap[kk] + '.4dfp.img'
    ;    cmd = '/compute_region_clusters'+region_str+' -regions_of_interest '+roistr[i]+' -cluster_file "' + scrap[kk] + '"' $
    ;        +' -clusters "'+scrap[i+1]+'"'
    ;    printf,lu,'nice +19 $BIN' + cmd
    ;    kk = kk + 1
    ;endfor
    ;START120731
    ;kk = 1
    ;for i=0,max_list-1 do begin
    ;    if rstrpos(scrap[kk],'.4dfp.img') eq -1 then scrap[kk] = scrap[kk] + '.4dfp.img'
    ;    cmd = '/compute_region_clusters'+region_str+' -regions_of_interest '+roistr[i]+' -cluster_file "' + scrap[kk] + '"' $
    ;        +' -clusters "'+scrap[i+1]+'"'
    ;    printf,lu,'nice +19 $BIN' + cmd
    ;    kk = kk + 1
    ;endfor
    ;START120731
    ;print,'max_list=',max_list
    filename = scrap[max_list+1:*]
    idx = where(strpos(filename,'.4dfp.img',9,/REVERSE_OFFSET) eq -1,cnt)
    if cnt ne 0 then filename[idx] = filename[idx] + '.4dfp.img'
    for i=0,max_list-1 do begin
        cmd = '/compute_region_clusters'+region_str+' -regions_of_interest '+roistr[i]+' -cluster_file "' + filename[i] + '"' $
            +' -clusters "'+scrap[i+1]+'"'
        printf,lu,'nice +19 $BIN' + cmd
    endfor

endelse
close,lu
free_lun,lu
spawn_cover,csh,fi,wd,dsp,help,stc
print,'DONE'
end
