;Copyright 12/31/99 Washington University.  All Rights Reserved.
;actmap.pro  $Revision: 12.83 $

;*****************
pro actmap,fi,help
;*****************

path = '/data/pethp1/img2/tec/bin'

if(help.enable eq !TRUE) then begin
    widget_control,help.id,SET_VALUE=string('Usage: /data/pethp1/img2/tec/bin/actmapf_4dfp [options] format 4dfp_name   Options: -a: Specify outfile name trailer (default = "actmap")  '+'-c: Scale output by specified factor. -u: Scale weights to unit variance. -z: Adjust weights to zero sum. -V: Use previous version (no ifh) 4dfp I/O.  '+' Example:  /data/pethp1/img2/tec/bin/actmapf_4dfp -zu "3x3(11+4x15-)" b1_rmsp_dbnd_xr3d_norm /data/pethp1/img2/tec/bin/actmapf_4dfp -aanatomy -c10 -u "++" b1_rmsp_dbnd_xr3d')
endif

hd = *fi.hdr_ptr(fi.n)
value = strarr(4)
label = strarr(4)
value(0) = fi.names(hd.mother)
value(1) = string("-uzc10")
value(2) = fi.paradigm_code(hd.mother)
label(0) = string("File name")
label(1) = string("Options")
label(2) = string("Stimulus time-course")
strs = get_str(3,label,value)
filename = strs(0)
options = strs(1)
code = strs(2)

cmd = string(path+'/actmapf_4dfp ',options,' ',code,' ',filename)
if(n_elements(cmd) gt 1) then $
    command = cmd(0) + cmd(1) $
else $
    command = cmd
print,command
spawn,command

return
end
