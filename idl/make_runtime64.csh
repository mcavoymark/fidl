#!/bin/csh

# Purpose: Generate .sav files required by IDL runtime license

if($#argv < 1) then
    echo Usage: make_runtime.csh output_path
    exit(-1)
endif

set DIR = $1

###echo defsysv,"'"\!FIDL_REV"'",0 > tmp1.h
echo "@stats.h" > tmp1.h
#echo "@ECATdef.h" >> tmp1.h
echo ".run main.pro" >> tmp1.h
#echo ".run stats" >> tmp1.h
echo ".run stats_exec" >> tmp1.h
echo ".run get_time_prof" >> tmp1.h
echo ".run get_profile" >> tmp1.h
echo ".run write_4dfp" >> tmp1.h
echo ".run load_4dfp" >> tmp1.h
#echo ".run load_stitch_names" >> tmp1.h
echo ".run load_data" >> tmp1.h
#echo ".run get_cov" >> tmp1.h
echo ".run xmanager" >> tmp1.h
echo ".run norm" >> tmp1.h
#echo ".run bonferroni" >> tmp1.h
echo ".run mult_comp" >> tmp1.h
#echo ".run mult_comp_spm" >> tmp1.h
echo ".run load_colortable" >> tmp1.h
echo ".run view_images" >> tmp1.h
#echo ".run path_distance" >> tmp1.h

#START151217
echo ".run strsplit" >> tmp1.h
#START151224
echo ".run mult_comp_monte_carlo" >> tmp1.h

set dead = `ls *dead.pro`
if($#dead < 11) then
    echo ".run $dead" >> tmp1.h
else
    echo ".run $dead[1-10]" >> tmp1.h
    echo ".run $dead[11-$#dead]" >> tmp1.h
endif

echo "resolve_all_jmo" >> tmp1.h
#START151217
#echo "resolve_all" >> tmp1.h
#echo "resolve_all,RESOLVE_FUNCTION='strsplit'" >> tmp1.h

echo "exit" >> tmp1.h
$IDL_DIR\/bin/idl tmp1.h |& tee tmp_run.txt

echo "@stats.h" > tmp.h
#echo "@ECATdef.h" >> tmp.h
#echo ".run load_stitch_names.pro" >> tmp.h
tr "[A-Z]" "[a-z]" < tmp_run.txt | gawk '{if($2=="compiled") printf(".run %spro\n",$4)}' >> tmp.h
echo "exit" >> tmp.h
$IDL_DIR\/bin/idl tmp.h >& tmp_sav.txt

echo 'echo @stats.h > tmp.h' > tmp1.csh
#echo 'echo @ECATdef.h >> tmp.h' >> tmp1.csh
echo 'echo .run stats >> tmp.h' >> tmp1.csh
echo 'echo .run write_4dfp >> tmp.h' >> tmp1.csh
echo 'echo .run load_4dfp >> tmp.h' >> tmp1.csh
echo 'echo .run xmanager >> tmp.h' >> tmp1.csh
echo " " >> tmp1.csh
tr "[A-Z]" "[a-z]" < tmp_run.txt | gawk 'BEGIN{printf("foreach file (")} {if($2 == "compiled") printf(" %spro",$4)}END{printf(")\n")}' >> tmp1.csh
echo '    if($file !~ *event.pro) then' >> tmp1.csh
echo '        echo .run $file >> tmp.h' >> tmp1.csh
echo '    endif' >> tmp1.csh
echo 'end' >> tmp1.csh

; Create save commands for each file.
tr "[A-Z]" "[a-z]" < tmp_sav.txt | gawk 'BEGIN{printf("foreach file (")} {if($2 == "compiled") printf(" %spro",$4)}END{printf(")\n")}' >> tmp1.csh

#echo "    make_save $DIR"' $file:r >> tmp.h' >> tmp1.csh
#START151111
echo "    /home/usr/fidl/bin_linux64/make_save $DIR"' $file:r >> tmp.h' >> tmp1.csh

echo 'end' >> tmp1.csh

# Now add auxiliary main routines that use the run-time licenses.
#echo "echo .run scripted_dsgn_comp >> tmp.h" >> tmp1.csh
#echo "    make_save $DIR"' main scripted_dsgn_comp >> tmp.h' >> tmp1.csh

echo 'echo exit >> tmp.h' >> tmp1.csh

echo '$IDL_DIR\/bin/idl tmp.h' >> tmp1.csh

csh tmp1.csh

# Verify that all compiled files have the same creation date.
ls -l $DIR | gawk '{if(NR==2) time=$8;if(time != $8) printf("***WARNING*** File may not have been compiled: %s\n",$9)}'
