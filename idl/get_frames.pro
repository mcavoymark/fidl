;Copyright 10/15/01 Washington University.  All Rights Reserved.
;get_frames.pro  $Revision: 1.10 $

;*******************************************************************************************
function get_frames,tdim,nrun,tdim_file,len_hrf,valid_frms,KEEP=keep,LIMITS_ONLY=limits_only
;*******************************************************************************************
valid_trial_frms = ''

if keyword_set(KEEP) then begin
    if not keyword_set(LIMITS_ONLY) then begin
        str = get_str_bool(1,'Frames to keep (e.g. 10-20,64,592-605 numbered from 1)',['Each run','Entire data-set'], $
            '',1,BOOL_TITLE='Frames specified relative to:',/TEXT_BOX_BELOW)
        case fix(str[1]) of
            0: omit_reference = !RUN
            1: omit_reference = !DATASET
            else: print,'Invalid value for str[1]'
        endcase
    endif else begin 
        str = get_str(1,'Keep these frames for experimental conditions','                ', $
            TITLE='Frames to keep (e.g. 10-20,64,592-605 numbered from 1)')
        omit_reference = !DATASET 
    endelse
    scrap = 'Keeping'
endif else begin

    ;str = get_str_bool(1,'Frames to omit (e.g. 10-20,64,592-605 numbered from 1)',['Each trial','Each run','Entire data-set'], $
    ;    '',1,BOOL_TITLE='Frames specified relative to:',/TEXT_BOX_BELOW)
    ;START160128
    ;goback2:
    ;str = get_str_bool(1,'Frames to omit (e.g. 10-20,64,592-605 numbered from 1)',['Each trial','Each run','Entire data-set'], $
    ;    '',1,BOOL_TITLE='Frames specified relative to:',/TEXT_BOX_BELOW,SPECIAL='Load text file')
    ;if str[0] eq 'SPECIAL' then begin
    ;    get_dialog_pickfile,'*.txt',getenv('PWD'),'Please select file that lists frames to omit.',ofile,nofile,rtn_path
    ;    if nofile eq 0 then goto,goback2
    ;    raf=readf_ascii_file(ofile[0],/NOHDR)
    ;    str[0]=raf.data
    ;endif
    ;START160129
    values='' & bool_dflt=1
    goback2:
    str = get_str_bool(1,'Frames to omit (e.g. 10-20,64,592-605 numbered from 1)',['Each trial','Each run','Entire data-set'], $
        values,bool_dflt,BOOL_TITLE='Frames specified relative to:',/TEXT_BOX_BELOW,SPECIAL='Load text file')
    if str[0] eq 'SPECIAL' then begin
        get_dialog_pickfile,'*.txt',getenv('PWD'),'Please select file that lists frames to omit.',ofile,nofile,rtn_path
        if nofile eq 0 then goto,goback2
        raf=readf_ascii_file(ofile[0],/NOHDR)
        values=raf.data & bool_dflt=fix(str[1])
        undefine,raf
        goto,goback2
    endif



    case fix(str[1]) of
        0: omit_reference = !TRIAL
        1: omit_reference = !RUN
        2: omit_reference = !DATASET
        else: print,'Invalid value for str[1]'
    endcase
    scrap = 'Omitting'
endelse

segments = strsplit(str[0],'[ '+string(9B)+',]',/REGEX,/EXTRACT)
nseg = n_elements(segments)
limits_all = -1L
for seg=0,nseg-1 do begin
    limits = long(strsplit(segments[seg],'-',/EXTRACT)) - 1
    if n_elements(limits) eq 1 then begin
        limits = long(strsplit(segments[seg],/EXTRACT)) - 1
        limits = [limits[0],limits[0]]
    endif
    if limits[0] lt 0 then limits[0] = 0
    if limits[1] gt tdim-1 then limits[1] = tdim-1
    if limits[1] lt 0 then omit_reference = 0
    str=strcompress(string(scrap,limits+1,FORMAT='(a," frames: ",i4," to ",i4)'))
    print,str

    limits_all = [limits_all,limits[0],limits[1]]

    case omit_reference of
        0: ;Do nothing.
        !DATASET: valid_frms[limits[0]:limits[1]] = 0
        !RUN: begin
            t0 = 0
            for file=0,nrun-1 do begin
                for t=limits[0],limits[1] do valid_frms[t0+t] = 0
                t0 = t0 + tdim_file[file]
            endfor
        end
        !TRIAL: begin
            lenh = max(len_hrf)
            valid_trial_frms = intarr(lenh)
            valid_trial_frms[*] = 1
            if limits[1] ge lenh then limits[1] = lenh - 1
            valid_trial_frms[limits[0]:limits[1]] = 0
        end
        else: print,'Invalid value of omit_reference
    endcase
endfor
limits_all = limits_all[1:*]

;return,rtn={omit_reference:omit_reference,valid_trial_frms:valid_trial_frms}
return,rtn={omit_reference:omit_reference,valid_trial_frms:valid_trial_frms,nseg:nseg,limits_all:limits_all}
end
