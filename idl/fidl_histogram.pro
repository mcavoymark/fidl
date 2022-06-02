;Copyright 2/3/06 Washington University.  All Rights Reserved.
;fidl_histogram.pro  $Revision: 1.10 $
pro fidl_histogram,fi,wd,dsp,help,stc,pref
maskstr = ''
liststr = ''
dir = getenv('PWD')

;KEEP THIS
;goback0:
;load = get_button(['list','individual files','exit'],TITLE='Which do you wish to load?')
;if load eq 2 then return
;goback0a:
;if load eq 0 then begin
;    get_dialog_pickfile,'.list',directory,'Please select list.',liststr,nlists,rtn_path
;    if liststr[0] eq 'GO_BACK' then goto,goback0 else if list_str[0] eq 'EXIT' then return
;    if read_list(liststr,nlists,nfiles,files,nt4s,t4s,nconcs,concs) eq !ERROR then return
;endif else begin 
;    files = get_files(filter,/GO_BACK)
;    if files[0] eq 'GO_BACK' then goto,goback0
;endelse
;filesin = files
;goback0b:
;rtn = select_files(filesin,TITLE='Select files.',/GO_BACK,/ONE_COLUMN)
;if rtn.files[0] eq 'GO_BACK' then goto,goback0a
;nfiles = rtn.count
;files = rtn.files
;goback0c:
;histype = get_button(['bins','sliding window','go back'],TITLE='Compute histogram from')
;if histype eq 2 then goto,goback0b

goback0:
data_idx = get_bolds(fi,dsp,wd,glm,help,stc)
if data_idx lt 0 then return
files = *stc[data_idx].filnam
nfiles = stc[data_idx].n
goback0c:
histype = get_button(['bins','sliding window','go back'],TITLE='Compute histogram from')
if histype eq 2 then goto,goback0





goback1:
if histype eq 0 then begin
    bin = get_str(1,'number of bins','12',WIDTH=50,TITLE='Please enter number of bins.', $
        LABEL='It is ok to enter more than you need.',SPECIAL='Load bins from text file.',/GO_BACK)
    if bin[0] eq 'GO_BACK' then goto,goback0c
    if bin[0] eq 'SPECIAL' then begin
        goback1a:
        get_dialog_pickfile,'*.dat',dir,'Please load bin file.',filename,nregfiles,rtn_path
        if filename eq '' or filename eq 'GOBACK' then goto,goback1 else if filename eq 'EXIT' then return
        scrap = readf_ascii_file(filename,/NOHDR)
        nbins = scrap.NR
        binstr = ' -bins "' + strjoin(scrap.data,'" "',/SINGLE) + '"'
    endif else begin
        nbins = fix(bin[0])
        goback2:
        scrap = 'bin' + strtrim(indgen(nbins)+1,2)
        scrap2 = make_array(nbins,/STRING,VALUE='')
        scrap = get_str(nbins,scrap,scrap2,WIDTH=50,TITLE='Bin sizes',LABEL='Ex. [-3.1416,-2.6180)'+string(10B) $
            +'Blanks are ignored.',SPECIAL='Uniformly between min and max.',/GO_BACK)
        if scrap[0] eq 'GO_BACK' then goto,goback1
        if scrap[0] eq 'SPECIAL' then begin 
            binstr = ' -nbins ' + bin[0] 
        endif else begin
            scrap = strcompress(scrap,/REMOVE_ALL)
            index = where(scrap ne '',count)
            if count eq 0 then begin
                junk = get_button(['try again','exit'],TITLE='You failed to enter any bins.')
                if junk eq 0 then goto,goback2 else return
            endif
            binstr = ' -bins "' + strjoin(scrap[index],'" "',/SINGLE) + '"'
        endelse
    endelse
endif else begin
    scrap = get_str(4,['window size','increment','start','end'],['0.0872','0.0174','0.0','6.28'],WIDTH=50, $
        TITLE='Window size and increment.',LABEL='Window size and increment.',/GO_BACK)
    if scrap[0] eq 'GO_BACK' then goto,goback0c
    scrap = strtrim(scrap,2)
    binstr = ' -window '+scrap[0]+' -increment '+scrap[1]+' -start '+scrap[2]+' -end '+scrap[3]
endelse

space = intarr(nfiles)
for i=0,nfiles-1 do begin
    ifh = read_mri_ifh(files[i])
    space[i] = get_space(ifh.matrix_size_1,ifh.matrix_size_2,ifh.matrix_size_3)
endfor
if total(space - space[0]) ne 0 then begin
    stat=dialog_message('Not all files are in the same space. Abort!',/ERROR)
    return
endif
goback3:
scrap = get_mask(space[0],fi)
if scrap eq 'GO_BACK' then begin
    if bin[0] eq 'SPECIAL' then goto,goback1a else goto,goback2
endif
if scrap ne '' then maskstr = ' -mask ' + scrap






;KEEP THIS
;goback4:
;dummy = 2 + load
;scrap = ['script','output']
;scraplabels = ['fidl_histogram.csh','histogram.txt']
;if load eq 1 then begin
;    scrap = [scrap,'list']
;    scraplabels = [scraplabels,'histogram.list']
;endif
;names = get_str(dummy,scrap,scraplabels,WIDTH=50,TITLE='Please enter names.',/GO_BACK)
;if names[0] eq 'GO_BACK' then goto,goback3
;csh = fix_script_name(names[0])
;outstr = ' -out "' + names[1] + '"'
;if load eq 1 then begin
;    liststr = names[2]
;    write_list,liststr,nfiles,files
;endif
;liststr = ' -list "'+liststr+'"'



goback31:
bootstrapstr = get_str(1,'number of bootstraps','1000',WIDTH=50,TITLE='Bootstrap statistics', $
    LABEL='Enter 0 to do not compute bootstrap statistics.',/GO_BACK)
if bootstrapstr[0] eq 'GO_BACK' then goto,goback3
bootstrapstr = ' - bootstrap '+ strtrim(bootstrapstr[0],2)



goback4:
names = get_str(2,['script','output'],['fidl_histogram.csh','histogram.txt'],WIDTH=50,TITLE='Please enter names.',/GO_BACK)
if names[0] eq 'GO_BACK' then goto,goback31
csh = fix_script_name(names[0])
outstr = ' -out "' + names[1] + '"'
action = get_button(['execute','return','go back'],TITLE='Please select action.'+string(10B)+string(10B) $
    +'To run on linux, please select return. Login to a linux machine and enter the script on the command line.',BASE_TITLE=csh)
if action eq 2 then goto,goback4
openw,lu,csh,/GET_LUN
top_of_script,lu
print_files_to_csh,lu,nfiles,files,'FILES','files'
printf,lu,'nice +19 $BIN/fidl_histogram $FILES '+binstr+maskstr+outstr+liststr+bootstrapstr
close,lu
free_lun,lu
spawn,'chmod +x '+csh
if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc
print,'DONE'
end
