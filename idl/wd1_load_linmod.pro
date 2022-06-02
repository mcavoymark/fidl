;Copyright 4/23/01 Washington University.  All Rights Reserved.
;wd1_load_linmod.pro  $Revision: 1.11 $
pro wd1_load_linmod,fi,dsp,wd,glm,help,MULTIPLE_FILES=multiple_files
if not keyword_set(MULTIPLE_FILES) then multiple_files=0
get_dialog_pickfile,fi.glm_filter,fi.path,'Please select GLMs.',rtn_filenames,rtn_nfiles,rtn_path,MULTIPLE_FILES=multiple_files, $
    /DONT_ASK
if rtn_filenames[0] eq 'GOBACK' or rtn_filenames[0] eq 'EXIT' or rtn_filenames[0] eq '' then return
fi.path = rtn_path
for i=0,rtn_nfiles-1 do scrap = load_linmod(fi,dsp,wd,glm,help,FILENAME=rtn_filenames[i])
end
