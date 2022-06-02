;Copyright 02/07/05 Washington University.  All Rights Reserved.
;fidl_flipreg.pro  $Revision: 1.6 $
pro fidl_flipreg,fi,wd,dsp,help,stc
goback9:
rtn = get_regions(fi,wd,dsp,help)
if rtn.msg eq 'EXIT' then return else if rtn.msg eq 'GO_BACK' then return 
region_names = rtn.region_names
region_str = rtn.region_str
nregfiles = rtn.nregfiles
region_file = rtn.region_file[0]
num_roi = n_elements(region_names)
labels = strarr(2,num_roi)
labels[0,*] = 'ignore'
labels[1,*] = 'flip'
goback10:
repeat begin
    scrap = 1
    widget_control,/HOURGLASS
    rtn = get_choice_list_many(region_names,labels,TITLE='Please select regions to flip.',/COLUMN_LAYOUT,/SET_UNSET,/GO_BACK)
    if rtn.special eq 'GO_BACK' then goto,goback9
    list = rtn.list
    max_list = max(list)
    if max_list eq 0 then begin
        stat=dialog_message('No regions selected.',/ERROR)
        scrap = get_button(['Yes. Please let me try again.','Go back','No. I want out of this program.'], $
            TITLE='Do you wish to flip regions?')
        if scrap eq 1 then begin
            goto,goback9
        endif
        if scrap eq 2 then begin
            print,'DONE'
            return
        endif
    endif
endrep until scrap eq 1
scrap = strtrim(where(list gt 0,count)+1,2)
roi_str = ' -regions_of_interest ' 
for i=0,count-1 do roi_str = roi_str + ' ' + scrap[i]
regnames = region_names


;if strpos(region_names[0],'roi_') eq 0 then begin
;    indexneg=where(strmatch(region_names,'roi_-*') eq 1,countneg)
;    indexpos=where(strmatch(region_names,'roi_+*') eq 1,countpos)
;    if countneg ne 0 then begin
;        scrap = regnames[indexneg]
;        strput,scrap,'+',4
;        regnames[indexneg] = scrap
;    endif
;    if countpos ne 0 then begin
;        scrap = regnames[indexpos]
;        strput,scrap,'-',4
;        regnames[indexpos] = scrap
;    endif
;endif
;START141125
if strpos(region_names[0],'roi_') eq 0 then begin
    indexneg=where(strmatch(region_names,'roi_-*') eq 1,countneg)
    indexpos=where(strmatch(region_names,'roi_+*') eq 1,countpos)
    if countneg ne 0 then begin
        scrap = regnames[indexneg]
        strput,scrap,'+',4
        regnames[indexneg] = scrap
    endif
    if countpos ne 0 then begin
        scrap = regnames[indexpos]
        strput,scrap,'-',4
        regnames[indexpos] = scrap
    endif
endif else begin
    indexneg=where(strmatch(region_names,'-*') eq 1,countneg)
    indexpos=where(strmatch(region_names,'+*') eq 1,countpos)
    if countneg ne 0 then begin
        scrap = regnames[indexneg]
        strput,scrap,'+',0
        regnames[indexneg] = scrap
    endif
    if countpos ne 0 then begin
        scrap = regnames[indexpos]
        strput,scrap,'-',0
        regnames[indexpos] = scrap
    endif
endelse 







goback11:
regnames = get_str(num_roi,region_names,regnames,WIDTH=50,TITLE='Please enter flipped region names.',/GO_BACK,/ONE_COLUMN)
if regnames[0] eq 'GO_BACK' then goto,goback10
regnamestr = ' -regions'
for i=0,num_roi-1 do regnamestr = regnamestr + ' ' + string(34B) + regnames[i] + string(34B)





;if nregfiles eq 1 then begin 
;    scrap = strpos(region_file,'/',/REVERSE_SEARCH)
;    outstr=strmid(region_file,scrap+1,strpos(region_file,'.4dfp.img')-scrap-1)+'_flip.4dfp.img'
;endif else $
;    outstr='regflip.4dfp.img'
;goback12:
;scrap = get_str(2,['script','output filename'],['fidl_flipreg.csh',outstr],WIDTH=100,TITLE='Please enter names.',/GO_BACK)
;if scrap[0] eq 'GO_BACK' then goto,goback11
;csh = fix_script_name(scrap[0])
;outstr=' -out_file "'+scrap[1]+'"'

if nregfiles eq 1 then begin
    scrap = strpos(region_file,'/',/REVERSE_SEARCH)
    scrap = strmid(region_file,scrap+1,strpos(region_file,'.4dfp.img')-scrap-1)+'_flip'
    outstr = scrap + '.4dfp.img'

    ;START141201
    ;tdstr = scrap + '.TD'

endif else begin
    outstr = 'regflip.4dfp.img'

    ;START141201
    ;tdstr = 'flip.TD'

endelse
goback12:

;scrap = get_str(3,['script','region file','TD file'],['fidl_flipreg.csh',outstr,tdstr],WIDTH=100,TITLE='Please enter names.', $
;    /GO_BACK)
;START141201
scrap = get_str(2,['script','region file'],['fidl_flipreg.csh',outstr],WIDTH=100,TITLE='Please enter names.',/GO_BACK)

if scrap[0] eq 'GO_BACK' then goto,goback11
csh = fix_script_name(scrap[0])
outstr=' -out_file "'+scrap[1]+'"'

;START141201
;tdstr=' -td_file "'+scrap[2]+'"'


;action = get_button(['execute','return','GO BACK'],TITLE='Please select action.'+string(10B)+string(10B) $
;    +'To run on linux, please select return. Login to a linux machine and enter the script on the command line.',BASE_TITLE=csh)
;if action eq 2 then goto,goback12
;START141125
action = get_button(['execute','return','GO BACK'],TITLE='Please select action.',BASE_TITLE=csh)
if action eq 2 then goto,goback12


openw,lu,csh,/GET_LUN
top_of_script,lu

;printf,lu,'nice +19 $BIN'+'/fidl_flipreg'+region_str+roi_str+outstr+tdstr+regnamestr
;START141201
printf,lu,'nice +19 $BIN'+'/fidl_flipreg'+region_str+roi_str+outstr+regnamestr

close,lu
free_lun,lu
spawn,'chmod +x '+csh
if action eq 0 then spawn_cover,csh,fi,wd,dsp,help,stc
print,'DONE'
end
