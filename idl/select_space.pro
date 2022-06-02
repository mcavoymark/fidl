;Copyright 8/22/03 Washington University.  All Rights Reserved.
;select_space.pro  $Revision: 1.6 $

;function select_space
;space = get_button(['111','222','333'],TITLE='Please select space.')
;case space of
;    0: begin
;        space = !SPACE_111
;        atlas = 111
;    end
;    1: begin
;        space = !SPACE_222
;        atlas = 222
;    end
;    2: begin
;        space = !SPACE_333
;        atlas = 333
;    end
;endcase
;atlas_str = ' -atlas ' + strtrim(atlas,2)
;return,rtn={space:space,atlas:atlas,atlas_str:atlas_str}
;end
;START151015
function select_space,space
if n_elements(space) eq 0 then begin
    space = get_button(['111','222','333'],TITLE='Please select space.')
    case space of
        0:space=!SPACE_111
        1:space=!SPACE_222
        2:space=!SPACE_333
    endcase
endif
case space of
    !SPACE_111:atlas=111
    !SPACE_222:atlas=222
    !SPACE_333:atlas=333
endcase
atlas_str = ' -atlas ' + strtrim(atlas,2)
return,rtn={space:space,atlas:atlas,atlas_str:atlas_str}
end
