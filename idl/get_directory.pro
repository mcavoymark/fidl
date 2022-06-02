;Copyright 7/3/01 Washington University.  All Rights Reserved.
;get_directory.pro  $Revision: 1.5 $

pro get_directory,directory,offset_spawn
offset_spawn = 0
;use a harmless command because the $cwd could be real long
spawn,'hostname',hostname
if n_elements(hostname) gt 1 then offset_spawn = 1

;spawn,'echo $cwd',dir
;directory = ''
;for i=offset_spawn,n_elements(dir)-1 do directory = directory + dir[i]
directory = getenv('PWD')

if directory eq '' then begin
    directory = get_str(1,'working directory','/mydirectory',LABEL='Please provide a working directory.', $
        TITLE='Not able to determine the working directory.',WIDTH=100,/EXIT)
    directory = directory[0] 
endif
end
