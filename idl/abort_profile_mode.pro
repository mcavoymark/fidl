;Copyright 6/29/01 Washington University.  All Rights Reserved.
;abort_profile_mode.pro  $Revision: 1.2 $

;******************************
pro abort_profile_mode,pr,wd,fi
;******************************

pr.mode = !DRAW_OFF
pr.prof_on = !PROF_NO
device,SET_GRAPHICS=3,/cursor_crosshair ;Resume normal graphics.
;widget_control,wd.draw[fi.cw],DRAW_MOTION_EVENTS=0
widget_control,wd.draw[fi.cw],/DRAW_MOTION_EVENTS
widget_control,wd.draw[fi.cw],EVENT_PRO='stats_exec_event'

end

