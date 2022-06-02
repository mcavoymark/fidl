;Copyright 9/27/00 Washington University.  All Rights Reserved.
;write_pref.pro  $Revision: 12.91 $
pro write_pref,pref
home = getenv('HOME')
file = home + '/.fidl'
openw,lu,file,/GET_LUN
printf,lu,'wilc_sig',pref.wilc_sig,FORMAT='(a25,i)'
printf,lu,'wilc_prob',pref.wilc_prob,FORMAT='(a25,i)'
printf,lu,'wilc_Z',pref.wilc_Z,FORMAT='(a25,i)'
printf,lu,'glm_z',pref.glm_z,FORMAT='(a25,i)'
printf,lu,'glm_mag',pref.glm_mag,FORMAT='(a25,i)'
printf,lu,'glm_var',pref.glm_var,FORMAT='(a25,i)'
printf,lu,'glm_t',pref.glm_t,FORMAT='(a25,i)'
printf,lu,'diff_var',pref.diff_var,FORMAT='(a25,i)'
printf,lu,'diff_pct',pref.diff_pct,FORMAT='(a25,i)'
printf,lu,'diff_diff',pref.diff_diff,FORMAT='(a25,i)'
printf,lu,'diff_z',pref.diff_z,FORMAT='(a25,i)'
printf,lu,'slope',pref.slope,FORMAT='(a25,i)'
printf,lu,'intercept',pref.intercept,FORMAT='(a25,i)'
printf,lu,'paradigm_skip',pref.paraskip,FORMAT='(a25,i)'
printf,lu,'paradigm_init',pref.parainit,FORMAT='(a25,i)'
printf,lu,'paradigm_act',pref.paraact,FORMAT='(a25,i)'
printf,lu,'paradigm_ctl',pref.paractl,FORMAT='(a25,i)'
printf,lu,'HRF_period',pref.period,FORMAT='(a25,i)'
printf,lu,'orientation',pref.orientation,FORMAT='(a25,i)'
printf,lu,'orientation_hide_label',pref.orientation_hide_label,FORMAT='(a25,i)'
printf,lu,'printer_name',pref.printer_name,FORMAT='(a25,1x,a)'
printf,lu,'concpath',pref.concpath,FORMAT='(a25,1x,a)'
printf,lu,'fidlpath',pref.fidlpath,FORMAT='(a25,1x,a)'
if pref.chunk eq 1 then scrap='yes' else scrap='no'
printf,lu,'chunk',scrap,FORMAT='(a25,1x,a)'

;START170224
printf,lu,'cpu',pref.cpu,FORMAT='(a25,1x,a)'

rhost=*pref.rhost
rpath=*pref.rpath
idx=where(rhost ne '' and rpath ne '',cnt)
if cnt ne 0 then for i=0,cnt-1 do printf,lu,'Rpath '+rhost[i]+' '+rpath[i]
close,lu
free_lun,lu
end
