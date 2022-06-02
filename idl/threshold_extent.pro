;Copyright 12/31/99 Washington University.  All Rights Reserved.
;threshold_extent.pro  $Revision: 1.34 $
function threshold_extent,stat_type,fwhm_in,space,mask,ACT_TYPE=act_type
print,'threshold_extent : space=',space
superbird='These were updated 7/12/16. We recommend a threshold >= 3.00.'
addtitle = ''
fwhm = fwhm_in
if not keyword_set(ACT_TYPE) then act_type = !ALL_ACTIVATIONS
if stat_type ne !T_STAT and stat_type ne !F_STAT then begin
    print,'Unknown stat_type. Must be either a T-statistic or F-statistic.'
    return,rtn={threshold_extent_str:'ERROR'}
endif
if space eq !SPACE_DATA then begin

    ;print,'Unknown space. Image must be in 222 or 333 space.'
    ;START170323
    print,'Unknown space.'

    return,rtn={threshold_extent_str:'ERROR'}
endif
if n_elements(mask) eq 0 then begin
    mask = get_mask(space,fi)
    if mask eq 'GO_BACK' then return,rtn={threshold_extent_str:'GO_BACK'}
endif
if stat_type eq !T_STAT then begin
    if act_type eq !ALL_ACTIVATIONS then begin
        if space eq !SPACE_111 then begin
            if mask eq !MASK_FILE_111 then begin
                if fwhm eq 0 then begin
                    threshold=[2  ,2  ,2.25,2.25,2.5,2.5,2.75,3  ,3.25,3.25,3.5,3.75,4  ,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent=   [11 ,13 ,8   ,9   ,6  ,7  ,5   ,4  ,3   ,4   ,3  ,3   ,2  ,3  ,2   ,2  ,2   ,2  ,2   ,2  ]
                    pval=     [.05,.01,.05 ,.01 ,.05,.01,.01 ,.01,.05 ,.01 ,.01,.01 ,.05,.01,.01 ,.01,.01 ,.01,.01 ,.01]
                endif else if fwhm eq 1 then begin
                    threshold=[2  ,2  ,2.25,2.25,2.5,2.5,2.75,2.75,3  ,3  ,3.25,3.25,3.5,3.5,3.75,4  ,4.25,4.5,4.5,4.75,5  ,5.25,5.5]
                    extent=   [20 ,22 ,14  ,15  ,10 ,11 ,8   ,9   ,6  ,7  ,5   ,6   ,4  ,5  ,4   ,3  ,3   ,2  ,3  ,2   ,2  ,2   ,2  ]
                    pval=     [.05,.01,.05 ,.01 ,.05,.01,.05 ,.01 ,.05,.01,.05,.01 ,.05,.01 ,.01 ,.01,.01 ,.05,.01,.01 ,.01,.01 ,.01]
                endif else if fwhm eq 2 then begin
                    threshold=[3.50,3.50,3.75,3.75,4  ,4  ,4.25,4.25]
                    extent=   [15  ,18  ,12  ,14  ,9  ,11 ,7   ,9   ]
                    pval=     [.05 ,.01 ,.05 ,.01 ,.05,.01,.05 ,.01 ]
                endif else begin
                    stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                        +'level of smoothing. See McAvoy.',/INFORMATION)
                    return,rtn={threshold_extent_str:'ERROR'}
                endelse
           endif else if mask eq !MASK_FILE_111ASIGMA then begin
                if fwhm eq 0 then begin
                    threshold=[2  ,2  ,2.25,2.25,2.5,2.75,3  ,3.25,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5,5.5]
                    extent=   [11 ,12 ,7   ,8   ,6  ,5   ,4  ,3   ,4   ,3  ,3   ,2  ,2   ,2  ,2   ,2  ,2   ,1  ,2  ]
                    pval=     [.05,.01,.05 ,.01 ,.01,.01 ,.01,.05 ,.01 ,.01,.01 ,.01,.01 ,.01,.01 ,.01,.01 ,.05,.01]
                endif else if fwhm eq 1 then begin
                    threshold=[2  ,2  ,2.25,2.25,2.5,2.5,2.75,2.75,3  ,3.25,3.5,3.75,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5,5.5]
                    extent=   [18 ,21 ,12  ,14  ,9  ,10 ,7   ,8   ,6  ,5   ,4  ,3   ,4   ,3  ,3   ,2  ,2   ,2  ,2   ,1  ,2  ]
                    pval=     [.05,.01,.05 ,.01 ,.05,.01,.05 ,.01 ,.01,.01 ,.01,.05 ,.01 ,.01,.01 ,.01,.01 ,.01,.01 ,.05,.01]
                endif else if fwhm eq 2 then begin
                    threshold=[3.75,3.75,4  ,4  ,4.25,4.25]
                    extent=   [10  ,13  ,8  ,10 ,6   ,7   ]
                    pval=     [.05 ,.01 ,.05,.01,.05 ,.01 ]
                endif else begin
                    stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                        +'level of smoothing. See McAvoy.',/INFORMATION)
                    return,rtn={threshold_extent_str:'ERROR'}
                endelse
            endif else begin
                stat=dialog_message(mask+string(10B)+string(10B)+'No threshold extents computed for this mask.',/INFORMATION)
                return,rtn={threshold_extent_str:'ERROR'}
            endelse
        endif else if space eq !SPACE_222 then begin
            rtn=get_root([mask,!MASK_FILE_222])
            if rtn.file[0] eq rtn.file[1] then begin
                if fwhm eq 0 then begin
                    stat=get_button('ok',TITLE='No threshold extents computed for fwhm=0.'+string(10B)+'We will use the threshold ' $
                        +'extents for fwhm=1.')
                    fwhm = 1
                    goto,goback5
                endif else if fwhm eq 1 then begin
                    goback5:
                    threshold = [2  ,2.25,2.5,2.75,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [16 ,11  ,8  ,6   ,5  ,4   ,4  ,3   ,3  ,2   ,2  ,2   ,2  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else if fwhm eq 2 then begin

                    ;threshold = [2  ,2.25,2.5,2.75,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    ;extent =    [100,63  ,42 ,29  ,21 ,15  ,11 ,9   ,6  ,5   ,3  ,2   ,2  ,1   ,1  ]
                    ;pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                    ;START160616 nsim=100000
                    threshold = [2  ,2.25,2.5,2.75,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [100,62  ,42 ,29  ,21 ,15  ,11 ,9   ,6  ,5   ,3  ,2   ,2  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.01]
                    threshold = [threshold,2  ,2.25,2.5,2.75,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25]
                    extent =    [extent   ,120,75  ,50 ,35  ,25 ,19  ,14 ,11  ,8  ,6   ,5  ,3   ,3  ,2   ]
                    pval =      [pval     ,.01,.01 ,.01,.01 ,.01,.01 ,.01,.01 ,.01,.01 ,.01,.01 ,.01,.01 ]
                    addtitle = string(10B)+string(10B)+superbird+string(10B)

                endif else if fwhm eq 3 then begin

                    ;threshold = [2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.5,4.75,  5,5.25,5.5]
                    ;extent =    [ 168, 109,  75,  52,  37,  26,  18,  13,   8,  5,   3,  2,   1,  1]
                    ;pval =      [ .05, .05, .05, .05, .05, .05, .05, .05, .05,.05, .05,.05, .05,.05]
                    ;START160711 nsim=100000
                    threshold = [  2,2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.5,4.75,  5,5.25,5.5]
                    extent =    [278, 167, 108,  74,  52,  37,  26,  19,  13,   9,  6,   3,  2,   1,  1]
                    pval =      [.05, .05, .05, .05, .05, .05, .05, .05, .05, .05,.05, .05,.05, .05,.01]
                    threshold = [threshold,  2,2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.5,4.75,  5,5.25]
                    extent =    [extent   ,342, 206, 134,  92,  65,  47,  35,  25,  18,  13,  9,   6,  4,   2]
                    pval =      [pval     ,.01, .01, .01, .01, .01, .01, .01, .01, .01, .01,.01, .01,.01, .01]
                    addtitle = string(10B)+string(10B)+superbird+string(10B)

                endif else if fwhm eq 4 then begin
                    threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [99 ,69  ,48 ,33  ,22 ,13  ,7  ,3   ,1  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else if fwhm eq 5 then begin
                    threshold = [2.25,2.50,3.00,3.25,3.50,3.75,4.00,4.25,4.50,4.75,5.00,5.25,5.50]
                    extent =    [567 ,365 ,167 ,116 ,80  ,53  ,34  ,19  ,9   ,3   ,1   ,1   ,1   ]
                    pval =      [0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05]
                endif else if fwhm eq 6 then begin
                    threshold = [2.00,2.25,2.50,2.75,3.00,3.25,3.50,3.60,3.75,4.00]
                    extent =    [1427,868 ,557 ,371 ,251 ,170 ,114 ,96  ,73  ,44]
                    pval =      [0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05]
                endif else begin
                    stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                        +'level of smoothing. See McAvoy.',/INFORMATION)
                    return,rtn={threshold_extent_str:'ERROR'}
                endelse
            endif else begin
                stat=dialog_message(mask+string(10B)+string(10B)+'No threshold-extents computed for this mask.',/INFORMATION)
                return,rtn={threshold_extent_str:'ERROR'}
            endelse
        endif else if space eq !SPACE_333 then begin
            if mask eq !MASK_FILE_333 then begin
                if fwhm eq 0 then begin
                    stat=get_button('ok',TITLE='No threshold extents computed for fwhm=0.'+string(10B)+'We will use the threshold ' $
                        +'extents for fwhm=1.')
                    fwhm = 1
                    goto,goback10
                endif else if fwhm eq 1 then begin
                    goback10:
                    threshold = [2  ,2.25,2.5,2.75,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [14 ,10  ,7  ,6   ,5  ,4   ,3  ,3   ,2  ,2   ,2  ,2   ,1  ,1   ,1]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else if fwhm eq 2 then begin

                    ;threshold = [2.25,2.5,2.75,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    ;extent =    [53  ,35 ,24  ,17 ,12  ,9  ,7   ,5  ,3   ,2  ,2   ,1  ,1   ,1  ]
                    ;pval =      [.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.01]
                    ;threshold = [threshold,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25]
                    ;extent =    [extent   ,22 ,16  ,12 ,9   ,7  ,5   ,4  ,3   ,2  ,2   ]
                    ;pval =      [pval     ,.01,.01 ,.01,.01 ,.01,.01 ,.01,.01 ,.01,.01 ]
                    ;START160711 nsim=100000
                    threshold = [  2,2.25,2.5,2.75,  3,3.25,3.5,3.75,  4,4.25,4.5,4.75,  5,5.25,5.5]
                    extent =    [ 84,  52, 35,24  , 18,  13,  9,   7,  5,   4,  3,   2,  1,   1,1  ]
                    pval =      [.05, .05,.05,.05 ,.05, .05,.05, .05,.05, .05,.05, .05,.05, .05,.01]
                    threshold = [threshold,  2,2.25,2.5,2.75,  3,3.25,3.5,3.75,  4,4.25,4.5,4.75,  5,5.25]
                    extent =    [extent   ,103,  64, 43,  30, 22,  16, 12,   9,  7,   5,  4,   3,  2,   2]
                    pval =      [pval     ,.01, .01,.01, .01,.01, .01,.01, .01,.01, .01,.01, .01,.01, .01]
                    addtitle = string(10B)+string(10B)+superbird+string(10B)

                endif else if fwhm eq 3 then begin

                    ;threshold = [2.00,2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.50,4.75,5.00,5.25,5.50]
                    ;extent =    [224 ,136 ,88  ,60  ,42  ,29  ,20  ,14  ,9   ,6   ,3   ,2   ,1   ,1   ,1   ]
                    ;pval =      [0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05]
                    ;START160712 nsim=100000
                    threshold = [2.00,2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.50,4.75,5.00,5.25,5.50]
                    extent =    [ 222, 136,  89,  60,  42,  29,  21,  14,  10,   6,   4,   2,   1,   1,   1]
                    pval =      [0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.01,0.01]
                    threshold = [threshold,2.00,2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.50,4.75,5.00]
                    extent =    [extent   , 284, 173, 114,  78,  55,  39,  29,  21,  15,  10,   7,   4,   3]
                    pval =      [pval     ,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01]
                    addtitle = string(10B)+string(10B)+superbird+string(10B)

                endif else if fwhm eq 4 then begin
                    threshold = [3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [36 ,23  ,14 ,8   ,4  ,1   ,1  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else begin
                    stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                        +'level of smoothing. See McAvoy.',/INFORMATION)
                    return,rtn={threshold_extent_str:'ERROR'}
                endelse
            endif else begin
                stat=dialog_message(mask+string(10B)+string(10B)+'No threshold extents computed for this mask.',/INFORMATION)
                return,rtn={threshold_extent_str:'ERROR'}
            endelse
        endif else if space eq !SPACE_MNI222 then begin
            rtn=get_root([mask,!MASK_MNI222])
            if rtn.file[0] eq rtn.file[1] then begin
                if fwhm eq 0 or fwhm eq 1 then begin
                    stat=get_button('ok',TITLE='No threshold extents computed for fwhm=0. or 1.'+string(10B) $
                        +'We will use the threshold extents for fwhm=2.')
                    fwhm = 2
                    goto,goback15
                endif else if fwhm eq 2 then begin
                    goback15:

                    ;threshold = [2  ,2.25,2.5,2.75,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    ;extent =    [104,65  ,43 ,30  ,22 ,16  ,12 ,9   ,7  ,5   ,4  ,3   ,2  ,1   ,1  ]
                    ;pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                    ;START160712 nsim=100000
                    threshold = [  2,2.25,2.5,2.75,  3,3.25,3.5,3.75,  4,4.25,4.5,4.75,  5,5.25,5.5]
                    extent =    [105,  65, 43,  30, 22,  16, 12,   9,  7,   5,  4,   3,  2,   1,  1]
                    pval =      [.05, .05,.05, .05,.05, .05,.05, .05,.05, .05,.05, .05,.05, .05,.01]
                    threshold = [threshold,  2,2.25,2.5,2.75,  3,3.25,3.5,3.75,  4,4.25,4.5,4.75,  5,5.25]
                    extent =    [extent   ,126,  78, 52,  36, 27,  20, 15,  11,  9,   7,  5,   4,  3,   2]
                    pval =      [pval     ,.01, .01,.01, .01,.01, .01,.01, .01,.01, .01,.01, .01,.01, .01]
                    addtitle = string(10B)+string(10B)+superbird+string(10B)

                endif else if fwhm eq 3 then begin

                    ;threshold = [2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.5,4.75,  5,5.25,5.5]
                    ;extent =    [ 176, 114,  78,  54,  39,  28,  20,  14,  10,  6,   4,  2,   1,  1]
                    ;pval =      [ .05, .05, .05, .05, .05, .05, .05, .05, .05,.05, .05,.05, .05,.05]
                    ;START160712 nsim=100000
                    threshold = [2.00,2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.5,4.75,  5,5.25,5.5]
                    extent =    [ 293, 176, 115,  78,  55,  39,  28,  20,  14,  10,  6,   4,  2,   1,  1]
                    pval =      [ .05, .05, .05, .05, .05, .05, .05, .05, .05, .05,.05, .05,.05, .05,.01]
                    threshold = [threshold,2.00,2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.50,4.75,5.00,5.25]
                    extent =    [extent   , 359, 217, 141,  97,  69,  50,  37,  27,  19,  14,  10,   7,   4,   2]
                    pval =      [pval     , .01, .01, .01, .01, .01, .01, .01, .01, .01, .01, .01, .01, .01, .01]
                    addtitle = string(10B)+string(10B)+superbird+string(10B)

                ;START170324
                endif else if fwhm eq 4 then begin
                    threshold = [2.00,2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.5,4.75,  5,5.25,5.5]
                    extent =    [ 597, 355, 230, 155, 107,  75,  53,  37,  25,  16,  9,   4,  2,   1,  1]
                    pval =      [ .05, .05, .05, .05, .05, .05, .05, .05, .05, .05,.05, .05,.05, .05,.01]
                    threshold = [threshold,2.00,2.25,2.50,2.75,3.00,3.25,3.50,3.75,4.00,4.25,4.50,4.75,5.00,5.25]
                    extent =    [extent   , 749, 446, 288, 197, 137,  99,  71,  51,  36,  25,  16,  10,   5,   2]
                    pval =      [pval     , .01, .01, .01, .01, .01, .01, .01, .01, .01, .01, .01, .01, .01, .01]
                    addtitle = string(10B)+string(10B)+superbird+string(10B)

                endif else begin
                    stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                        +'level of smoothing. See McAvoy.',/INFORMATION)
                    return,rtn={threshold_extent_str:'ERROR'}
                endelse
            endif else begin
                stat=dialog_message(mask+string(10B)+string(10B)+'No threshold-extents computed for this mask.',/INFORMATION)
                return,rtn={threshold_extent_str:'ERROR'}
            endelse
        endif else begin
            stat=widget_message('Unknown space!',/ERROR)
            return,rtn={threshold_extent_str:'ERROR'}
        endelse
    endif else begin ;one sided
        if space eq !SPACE_111 then begin
            if mask eq !MASK_FILE_111 then begin
                if fwhm eq 0 then begin
                    threshold=[2  ,2  ,2.25,2.25,2.5,2.75,3  ,3.25,3.5,3.75,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5,5.5]
                    extent=   [8  ,9  ,6   ,7   ,5  ,4   ,4  ,3   ,3  ,2   ,3   ,2  ,2   ,2  ,2   ,2  ,2   ,1  ,2  ]
                    pval=     [.05,.01,.05 ,.01 ,.05,.05 ,.01,.01 ,.01,.05 ,.01 ,.01,.01 ,.01,.01 ,.01,.01 ,.05,.01]
                endif else if fwhm eq 1 then begin
                    threshold=[2  ,2  ,2.25,2.25,2.5,2.5,2.75,2.75,3  ,3  ,3.25,3.5,3.75,3.75,4  ,4.25,4.5,4.5,4.75,5  ,5.25,5.5,5.5]
                    extent=   [18 ,21 ,13  ,15  ,9  ,11 ,7   ,8   ,6  ,7  ,5   ,4  ,3   ,4   ,3  ,3   ,2  ,3  ,2   ,2  ,2   ,1  ,2  ]
                    pval=     [.05,.01,.05 ,.01 ,.05,.01,.05 ,.01 ,.05,.01,.01 ,.01,.05 ,.01 ,.01,.01 ,.05,.01,.01,.01 ,.01 ,.05,.01]
                endif else if fwhm eq 2 then begin
                    threshold=[3.50,3.50,3.75,3.75,4  ,4  ,4.25,4.25]
                    extent=   [14  ,17  ,11  ,13  ,8  ,10 ,6   ,8   ]
                    pval=     [.05 ,.01 ,.05 ,.01 ,.05,.01,.05 ,.01 ]
                endif else begin
                    stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                        +'level of smoothing. See McAvoy.',/INFORMATION)
                    return,rtn={threshold_extent_str:'ERROR'}
                endelse
            endif else if mask eq !MASK_FILE_111ASIGMA then begin
                if fwhm eq 0 then begin
                    threshold=[2  ,2  ,2.25,2.5,2.75,3  ,3  ,3.25,3.5,3.75,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5,5.5]
                    extent=   [7  ,8  ,6   ,5  ,4   ,3  ,4  ,3   ,3  ,2   ,3   ,2  ,2   ,2  ,2   ,2  ,2   ,1  ,2  ]
                    pval=     [.05,.01,.01 ,.01,.01 ,.05,.01,.01 ,.01,.05 ,.01,.01 ,.01,.01 ,.01,.01 ,.01,.05 ,.01]
                endif else if fwhm eq 1 then begin
                    threshold=[2  ,2  ,2.25,2.25,2.5,2.5,2.75,2.75,3  ,3  ,3.25,3.25,3.5,3.75,3.75,4  ,4.25,4.25,4.5,4.75,5  , $
                               5.25,5.5,5.5]
                    extent=   [17 ,19 ,12  ,13  ,9  ,10 ,7   ,8   ,5  ,6  ,4   ,5   ,4  ,3   ,4   ,3  ,2   ,3   ,2  ,2   ,2  , $
                               2   ,1  ,2  ]
                    pval=     [.05,.01,.05 ,.01 ,.05,.01,.05 ,.01 ,.05,.01,.05 ,.01 ,.01,.05 ,.01 ,.01,.05 ,.01 ,.01,.01 ,.01, $
                               .01 ,.05,.01]
                endif else if fwhm eq 2 then begin
                    threshold=[3.50,3.75,3.75,4  ,4  ,4.25,4.25]
                    extent=   [12  ,9   ,11  ,7  ,9  ,5   ,7   ]
                    pval=     [.05 ,.05 ,.01,.05,.01 ,.05 ,.01 ]
                endif else begin
                    stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                        +'level of smoothing. See McAvoy.',/INFORMATION)
                    return,rtn={threshold_extent_str:'ERROR'}
                endelse
            endif else begin
                stat=dialog_message(mask+string(10B)+string(10B)+'No threshold extents computed for this mask.',/INFORMATION)
                return,rtn={threshold_extent_str:'ERROR'}
            endelse
        endif else if space eq !SPACE_222 then begin
            if mask eq !MASK_FILE_222 then begin
                if fwhm eq 1 then begin
                    threshold = [2  ,2.25,2.5,2.75,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [15 ,10  ,8  ,6   ,5  ,4   ,3  ,3   ,2  ,2   ,2  ,2   ,2  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else if fwhm eq 2 then begin
                    threshold = [2  ,2.25,2.5,2.75,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [92 ,57  ,38 ,27  ,19 ,14  ,10 ,8   ,6  ,4   ,3  ,2   ,1  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else if fwhm eq 3 then begin
                    threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [46 ,33  ,23 ,16  ,11 ,7   ,4  ,2   ,1  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else if fwhm eq 4 then begin
                    threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [88 ,60  ,41 ,28  ,18 ,10  ,5  ,2   ,1  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else begin
                    stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                        +'level of smoothing. See McAvoy.',/INFORMATION)
                    return,rtn={threshold_extent_str:'ERROR'}
                endelse
            endif else begin
                stat=dialog_message(mask+string(10B)+string(10B)+'No threshold extents computed for this mask.',/INFORMATION)
                return,rtn={threshold_extent_str:'ERROR'}
            endelse
        endif else if !SPACE_333 then begin
            if mask eq !MASK_FILE_333 then begin
                if fwhm eq 1 then begin
                    threshold = [2  ,2.25,2.5,2.75,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [13 ,9   ,7  ,5   ,4  ,4   ,3  ,3   ,2  ,2   ,2  ,2   ,1  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else if fwhm eq 2 then begin
                    threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [15 ,11  ,8  ,6   ,4  ,3   ,2  ,2   ,1  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.01 ,.01]
                    threshold = [threshold,3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ]
                    extent =    [extent   ,20 ,15  ,11 ,8   ,6  ,5   ,3  ,2   ,2  ]
                    pval =      [pval     ,.01,.01 ,.01,.01 ,.01,.01 ,.01,.01 ,.01]
                endif else if fwhm eq 3 then begin
                    threshold = [3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [25  ,17 ,12  ,7  ,4   ,2  ,1   ,1  ,1   ,1  ]
                    pval =      [.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else if fwhm eq 4 then begin
                    threshold = [3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                    extent =    [29 ,18  ,10 ,5   ,2  ,1   ,1  ,1   ,1  ]
                    pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
                endif else begin
                    stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                        +'level of smoothing. See McAvoy.',/INFORMATION)
                    return,rtn={threshold_extent_str:'ERROR'}
                endelse
            endif else begin
                stat=dialog_message(mask+string(10B)+string(10B)+'No threshold extents computed for this mask.',/INFORMATION)
                return,rtn={threshold_extent_str:'ERROR'}
            endelse
        endif else begin
            stat=widget_message('Unknown space!',/ERROR)
            return,rtn={threshold_extent_str:'ERROR'}
        endelse
    endelse
endif else begin ;!F_STAT
    if space eq !SPACE_222 then begin
        if fwhm lt 2 then fwhm = 2
        if mask eq !MASK_FILE_222 then begin
            if fwhm eq 2 then begin
                threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                extent =    [45 ,34  ,24 ,18  ,12 ,8   ,5  ,3   ,2  ,1   ,1  ]
                pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
            endif else if fwhm eq 3 then begin
                threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                extent =    [70 ,51  ,36 ,26  ,17 ,11  ,6  ,3   ,2  ,1   ,1  ]
                pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
            endif else if fwhm eq 4 then begin
                threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
                extent =    [110,74  , 51,35  ,24 ,15  ,8  ,3   ,2  ,1   ,1  ]
                pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
            endif else begin
                stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                    +'level of smoothing. See McAvoy.',/INFORMATION)
                return,rtn={threshold_extent_str:'ERROR'}
            endelse
        endif else begin
            stat=dialog_message(mask+string(10B)+string(10B)+'No threshold extents computed for this mask.',/INFORMATION)
            return,rtn={threshold_extent_str:'ERROR'}
        endelse
    endif else if space eq !SPACE_333 then begin 
        if mask eq !MASK_FILE_333 then begin
            if fwhm eq 0 then begin
                threshold = [3.00,3.25,3.50,3.75,4.00,4.25,4.50,4.75,5.00,5.25,5.50]
                extent =    [3   ,3   ,3   ,2   ,2   ,2   ,2   ,2   ,1   ,1   ,1   ]
                pval =      [0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05]
            endif else if fwhm eq 1 then begin
                threshold = [3.00,3.25,3.50,3.75,4.00,4.25,4.50,4.75,5.00,5.25,5.50]
                extent =    [4   ,3   ,3   ,2   ,2   ,2   ,2   ,2   ,1   ,1   ,1   ]
                pval =      [0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05]
            endif else if fwhm eq 2 then begin
                threshold = [3.00,3.25,3.50,3.75,4.00,4.25,4.50,4.75,5.00,5.25,5.50]
                extent =    [13  ,9   ,7   ,5   ,4   ,3   ,2   ,2   ,1   ,1   ,1   ]
                pval =      [0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05]
            endif else if fwhm eq 3 then begin
                threshold = [3.00,3.25,3.50,3.75,4.00,4.25,4.50,4.75,5.00,5.25,5.50]
                extent =    [30  ,21  ,15  ,11  ,7   ,5   ,3   ,2   ,1   ,1   ,1   ]
                pval =      [0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05,0.05]
            endif else begin
                stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                    +'level of smoothing. See McAvoy.',/INFORMATION)
                return,rtn={threshold_extent_str:'ERROR'}
            endelse
            addtitle = string(10B)+string(10B)+'THESE ARE NEW. 2/1/07'+string(10B)
        endif else begin
            stat=dialog_message(mask+string(10B)+string(10B)+'No threshold-extents computed for this mask.',/INFORMATION)
            return,rtn={threshold_extent_str:'ERROR'}
        endelse
    endif else if space eq !SPACE_MNI222 then begin
        if fwhm lt 2 then fwhm = 2

        ;if mask eq !MASK_MNI222 then begin
        ;    if fwhm eq 2 then begin
        ;        threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
        ;        extent =    [45 ,34  ,24 ,18  ,12 ,8   ,5  ,3   ,2  ,1   ,1  ]
        ;        pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
        ;    endif else if fwhm eq 3 then begin
        ;        threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
        ;        extent =    [70 ,51  ,36 ,26  ,17 ,11  ,6  ,3   ,2  ,1   ,1  ]
        ;        pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
        ;    endif else if fwhm eq 4 then begin
        ;        threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
        ;        extent =    [110,74  , 51,35  ,24 ,15  ,8  ,3   ,2  ,1   ,1  ]
        ;        pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
        ;    endif else begin
        ;        stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
        ;            +'level of smoothing. See McAvoy.',/INFORMATION)
        ;        return,rtn={threshold_extent_str:'ERROR'}
        ;    endelse
        ;    addtitle = string(10B)+string(10B)+'These are for 711-2B, but conservative, so possibly reasonable for MNI which is ' $
        ;        +' slightly larger.'+string(10B)
        ;endif else begin
        ;    stat=dialog_message(mask+string(10B)+string(10B)+'No threshold extents computed for this mask.',/INFORMATION)
        ;    return,rtn={threshold_extent_str:'ERROR'}
        ;endelse
        ;START140807
        if mask ne !MASK_MNI222 then begin
            ifh = read_mri_ifh(mask)
            openr,lu,mask,/GET_LUN
            img = assoc(lu,fltarr(ifh.matrix_size_1,ifh.matrix_size_2,/NOZERO),0)
            nonzero=0
            for i=0,ifh.matrix_size_3-1 do nonzero = nonzero + total(img[i] ne 0.)
            print,'nonzero = ',nonzero,' !LENBRAIN_MNI222 = ',!LENBRAIN_MNI222
            close,lu
            free_lun,lu
            if nonzero gt (!LENBRAIN_MNI222+2500L) then begin
                stat=dialog_message(mask+string(10B)+string(10B)+'No threshold extents computed for this mask.',/INFORMATION)
                return,rtn={threshold_extent_str:'ERROR'}
            endif
        endif
        if fwhm eq 2 then begin
            threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
            extent =    [45 ,34  ,24 ,18  ,12 ,8   ,5  ,3   ,2  ,1   ,1  ]
            pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
        endif else if fwhm eq 3 then begin
            threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
            extent =    [70 ,51  ,36 ,26  ,17 ,11  ,6  ,3   ,2  ,1   ,1  ]
            pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
        endif else if fwhm eq 4 then begin
            threshold = [3  ,3.25,3.5,3.75,4  ,4.25,4.5,4.75,5  ,5.25,5.5]
            extent =    [110,74  , 51,35  ,24 ,15  ,8  ,3   ,2  ,1   ,1  ]
            pval =      [.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05,.05 ,.05]
        endif else begin
            stat=dialog_message('fwhm = '+trim(fwhm)+string(10B)+string(10B)+'No threshold extents computed for this ' $
                +'level of smoothing. See McAvoy.',/INFORMATION)
            return,rtn={threshold_extent_str:'ERROR'}
        endelse
        addtitle = string(10B)+string(10B)+'These are for 711-2B, but conservative, so possibly reasonable for MNI which is ' $
            +' slightly larger.'+string(10B)



    endif else begin 
        stat=dialog_message('No threshold extents computed for this space.',/INFORMATION)
        return,rtn={threshold_extent_str:'ERROR'}
    endelse
endelse
threshstr = strtrim(string(threshold,FORMAT='(f6.2)'),2)
extentstr = trim(extent)
pvalstr = strtrim(string(pval,FORMAT='(f6.2)'),2)
threshold_extent_str = 'z='+threshstr+',vox='+extentstr+'    p val='+pvalstr
scrap = select_files(threshold_extent_str,TITLE='Monte Carlo Correction for Multiple Comparisons' $
    +string(10B)+'Please select threshold,extent for FWHM = '+trim(fwhm)+addtitle,MIN_NUM_TO_SELECT=1,/EXIT,/GO_BACK)
if scrap.files[0] eq 'GO_BACK' then $
    return,rtn={threshold_extent_str:'GO_BACK'} $
else if scrap.files[0] eq 'EXIT' then $
    return,rtn={threshold_extent_str:'EXIT'}
threshstr = threshstr[scrap.index]
extentstr = extentstr[scrap.index]
pvalstr = strmid(pvalstr[scrap.index],1)
tepstr = 'z'+threshstr+'n'+extentstr+'p'+pvalstr
threshold_extent_str = ' -threshold_extent'+strjoin(' "'+threshstr+' '+extentstr+'"',/SINGLE)+' -pval'+strjoin(' '+pvalstr,/SINGLE) 
return,rtn={threshold:threshold[scrap.index],extent:extent[scrap.index],pval:pval[scrap.index],tepstr:tepstr,count:scrap.count, $
    threshold_extent_str:threshold_extent_str}
end
