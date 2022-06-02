;Copyright 5/5/04 Washington University.  All Rights Reserved.
;spawn_cover.pro  $Revision: 1.8 $
pro spawn_cover,command,fi,wd,dsp,help,stc
if n_elements(command) eq 1 then begin
    if strmid(command,strlen(command)-4) eq '.csh' then command = 'csh ' + command 
endif
print,'********************** SCRIPT **********************'
print,command
print,'****************************************************'

;START150126
;print,''

widget_control,/HOURGLASS
spawn,command,result
stat = spawn_check_and_load(result,fi,wd,dsp,help,nfiles_loaded,files_loaded,stc)
if stat.msg eq 'OK' then begin
    if nfiles_loaded ne 0 then begin

        ;scrap = ''
        ;for i=0,nfiles_loaded-1 do scrap = scrap + files_loaded[i]+' has been written to disk.' + string(10B)
        ;print,scrap
        ;stat = dialog_message(scrap,/INFORMATION)
        ;START150126
        ;scrap = files_loaded+string(10B)
        ;print,scrap
        lizard=trim(nfiles_loaded)+' files have been written to disk'
        print,lizard
        ;stat = dialog_message_long(lizard,scrap)
        stat = dialog_message_long(lizard,files_loaded)
        ;undefine,stat,lizard,scrap
        undefine,stat,lizard,files_loaded,nfiles_loaded

    endif
endif
end
