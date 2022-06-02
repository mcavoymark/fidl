;Copyright 9/30/02 Washington University.  All Rights Reserved.
;get_atlas_param.pro  $Revision: 1.4 $
function get_atlas_param,atlas,mmppix,center,xdim,ydim,zdim,area,vol,voxel_size,str

    ;print,'get_atlas_param top atlas=',atlas

    case atlas of
        111: str = '_111_t88'
        222: str = '_222_t88'
        333: str = '_333_t88'
        else: begin
            print,'Unknown atlas.'
            return,!ERROR
        endelse
    endcase
    param = fltarr(14)

    ;passtype = bytarr(2)
    ;passtype[0] = 1
    ;passtype[1] = 0
    ;widget_control,/HOURGLASS
    ;stat = call_external(!SHARE_LIB,'_get_atlas_param',atlas,param,VALUE=passtype,/I_VALUE)
    ;widget_control,HOURGLASS=0
    ;START140410
    widget_control,/HOURGLASS


    ;START150212
    stat = call_external(!SHARE_LIB,'_get_atlas_param',trim(atlas),param,VALUE=[1,0])
    
    mmppix = param[0:2]
    center = param[3:5]
    xdim = param[6]
    ydim = param[7]
    zdim = param[8]
    area = param[9]
    vol = param[10]
    voxel_size = param[11:13]
    return,stat
end
