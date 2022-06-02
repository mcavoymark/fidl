;Copyright 11/21/11 Washington University.  All Rights Reserved.
;fidl_split.pro  $Revision: 1.10 $
pro fidl_split,fi,dsp,wd,stc,help,pref 
spawn,!BINEXECUTE+'/fidl_timestr2',timestr
goback0:
gg=get_glms(pref,fi,BASE_TITLE='Files are split by hemisphere, anterior/posterior and superior/inferior.',GET_THIS='imgs')
if gg.msg ne 'OK' then return
goback1:

;gr=get_root(gg.imgselect,'.4dfp.img')
;START190108
gr=get_root(gg.imgselect,gg.ext)

;print,'here0 gr.file=',gr.file
;print,'here0 gr.path=',gr.path

if gg.identifyselect[0] ne '' then roots=gg.identifyselect+'_'+gr.file else roots=gr.file



;if gg.identifyselect[0] eq '' then begin 
;START190128
if gg.identifyselect[0] eq '' and n_elements(gr.path) gt 1 then begin 


    ;print,'gr.path=',gr.path
    ;print,'n_elements(gr.path)=',n_elements(gr.path)
    superbird=strsplit(gr.path[0],/EXTRACT,'/')
    ;print,'superbird=',superbird
    ;print,'n_elements(superbird)=',n_elements(superbird)
    elephantpool=strsplit(gr.path[1],/EXTRACT,'/')
    ;print,'elephantpool=',elephantpool
    ;print,'n_elements(elephantpool)=',n_elements(elephantpool)


    ;for i=0,n_elements(superbird)-1 do if superbird[i] ne elephantpool[i] then goto,eureka
    ;eureka:
    ;for j=0,n_elements(gr.path)-1 do begin
    ;    lizard=strsplit(gr.path[j],/EXTRACT,'/')
    ;    roots[j]=lizard[i]+'_'+roots[j]
    ;endfor
    ;START190108
    for i=0,n_elements(superbird)-1 do begin
        if superbird[i] ne elephantpool[i] then begin 
            for j=0,n_elements(gr.path)-1 do begin

                ;lizard=strsplit(gr.path[j],/EXTRACT,'/')
                ;roots[j]=lizard[i]+'_'+roots[j]
                ;START190128
                lizard=strsplit(gr.path[j],/EXTRACT,'/')
                if strpos(roots[j],lizard[i]) eq -1 then roots[j]=lizard[i]+'_'+roots[j]

            endfor
            break
        endif
    endfor

endif


scrap = ['script',make_array(gg.total_nsubjects,/STRING,VALUE='output root')]
scraplabels = ['fidl_split_'+timestr[0]+'.csh',roots]

;names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter.',/GO_BACK)
;START180921
names = get_str(n_elements(scrap),scrap,scraplabels,WIDTH=100,TITLE='Please enter.',/GO_BACK,/ONE_COLUMN)

if names[0] eq 'GO_BACK' then goto,goback0
names = fix_script_name(names)
script = names[0]
roots = names[1:*]
goback2:
lc2 = get_button(['Let me specify a universal name','Use the output root','Go back','Exit'], $
    TITLE='How do you want the regions to be named?',BASE_TITLE='Region names')
if lc2 eq 3 then return else if lc2 eq 2 then goto,goback1
goback3:
rroot=''
if lc2 eq 0 then begin
   if gg.glm_list[0] ne '' then begin
       gr1=get_root(gg.glm_list[0],'.')
       lizard=gr1.file
   endif else $
       lizard=gr.file[0]
   rroot = get_str(1,'Universal root',lizard,WIDTH=100,TITLE='Please enter.',/GO_BACK,/EXIT)
   if rroot[0] eq 'EXIT' then return else if rroot[0] eq 'GO_BACK' then goto,goback2
endif
goback4:
scrap = get_button(['Only the hemispherical region file','Everything','Go back','Exit'], $
    TITLE='Please select output',BASE_TITLE='Output options')
if scrap eq 3 then return else if scrap eq 2 then goto,goback3
if scrap eq 0 then outstr=' -LRregonly' else outstr=''
action = get_button(['Execute','Return','Go back','Exit'],TITLE='Please select',BASE_TITLE=script)
if action eq 3 then return else if action eq 2 then begin
    if rroot[0] eq '' then goto,goback2 else goto,goback3
endif

openw,lu,script,/GET_LUN
top_of_script,lu



;print_files_to_csh,lu,gg.total_nsubjects,gg.imgselect,'FILES','files'
;print_files_to_csh,lu,gg.total_nsubjects,roots,'ROOTS','roots'
;cmd = 'nice +19 $BIN/fidl_split $FILES $ROOTS'+outstr
;if rroot[0] ne '' then cmd = cmd + ' -rroot '+rroot[0]
;printf,lu,cmd
;START190125
for i=0,gg.total_nsubjects-1 do begin
    print_files_to_csh,lu,1,gg.imgselect[i],'FILES','files',/NO_NEWLINE
    print_files_to_csh,lu,1,roots[i],'ROOTS','roots',/NO_NEWLINE
    cmd = 'nice +19 $BIN/fidl_split $FILES $ROOTS'+outstr
    if rroot[0] ne '' then cmd = cmd + ' -rroot '+rroot[0]
    printf,lu,cmd
    printf,lu,''
endfor 

;set FILES = (-files /data/nil-bluearc/raichle/mcavoy/benphilip/HCP/segment/sub01A_aparc+aseg_brainmask.nii.gz)
;set ROOTS = (-roots sub01A_aparc+aseg_brainmask)
;nice +19 $BIN/fidl_split $FILES $ROOTS -LRregonly -rroot global





close,lu
free_lun,lu
spawn,'chmod +x '+script
if action eq 0 then spawn_cover,script,fi,wd,dsp,help,stc
print,'DONE'
end
