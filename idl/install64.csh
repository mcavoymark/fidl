#!/bin/csh
#Copyright 11/12/15 Washington University.  All Rights Reserved.
#install64.csh  $Revision: 1.11 $
umask

#if($#argv < 3) then                                   #if 3rd argument in present, don't make binaries
#    echo 'Usage: install.csh revision_number <install> NOBIN'
#    exit                                    #0 or 1=install
#endif
#START151116
#if($#argv < 1) then
#    echo 'Usage: csh install64.csh revision_number 
#    exit               
#endif


#set IDL1 = idl #idl_5.3
#set IDL2 = idl_6.3
#START151112
set IDL2 = idl_8.3

#set REV = $1
#START151116
set REV = 3.0 

set FIDL_DIR = fidl_${REV}
set REVNAME = rev${REV}

#START151116
#if($2 == 0) then
#    set INSTALL = 0
#else if($2 == 1) then
#    set INSTALL = 1
#else
#    echo "$2" is unknown
#    exit
#endif

set REVNAME = `date | gawk '{printf("%s%s%s",substr($6,3),substr($2,1,3),$3)}'`_${REVNAME}

#START151116
#if($#argv == 3) then
#    set NOBIN = 1
#else
#    set NOBIN = 0
#endif

echo argv = $#argv

#START151119
#echo NOBIN = $NOBIN
#echo INSTALL = $INSTALL

set FIDL_PATH = /home/usr/fidl/fidl_code 
set INSTALL_PATH = ${FIDL_PATH}/${FIDL_DIR}
set LIB_PATH = /home/usr/fidl/lib
set D = /home/usr/fidl/idl
set DMORE = $D/fidl/${FIDL_DIR}
set DL64 = $DMORE/bin_linux64
co Makefile

#make checkout
#START151119
make checkout64

set BINDIR_LINUX64 = ${HOME}/bin_linux64
cd $D
mkdir fidl
cd fidl
mkdir ${FIDL_DIR} 
mkdir ${FIDL_DIR}/bin_linux64
mkdir ${FIDL_DIR}/$IDL2
mkdir ${FIDL_DIR}/scripts
cd ..
cd clib 
set BLIST = (compute_glm2 \
             fidl_zstat \
             fidl_avg_zstat \
             fidl_bolds \
             fidl_tc_ss \
             #fidl_anova2 \
             fidl_anova3 \
             fidl_ttest \
             fidl_logreg_ss \
             fidl_f_to_z \
             fidl_t_to_z \
             fidl_r_to_z \
             convert_af3d_to_fidl \
             compute_region_stats \
             compute_residuals \
             compute_region_clusters \
             grow_regions \
             delete_regions \
             atlas_transform \
             #fix_grand_mean \
             white_noise \
             fidl_avg \
             fix_region_file \
             compute_pearson \
             #compute_lsq_tc \
             #compute_mag_norm \
             compute_boynton \
             compute_tc_max_deflect \
             compute_voxel_value \
             fidl_gauss \
             compute_tc_reg_stats \
             compute_chisquare \
             #add_planes \
             fidl_reg_ass \
             fidl_4dfp_to_text \
             fidl_text_to_4dfp \
             fidl_minmax \
             fidl_flipreg \
             fidl_flip \
             #fidl_threshold_cluster \
             fidl_cc \
             fidl_pc \
             fidl_lminusrdivlplusr \
             fidl_genconc \
             fidl_histogram \
             fidl_cov \
             fidl_trend \
             fidl_rename_contrasts \
             fidl_rename_effects \
             fidl_rename_paths \
             fidl_chisquare_to_z \
             fidl_ac \
             fidl_crosscorr \
             fidl_stack_crosscov \
             fidl_stack_crosscov_seed \
             fidl_glmsavecorr \
             fidl_corrvox \
             fidl_fano \
             fidl_extract_fzstat \
             fidl_collate \
             fidl_logic \
             fidl_annalisa_contrast \
             fidl_and \
             fidl_checkglm \
             fidl_pca \
             fidl_labelfile \
             fidl_p_to_z \
             fidl_glm_metrics \
             fidl_swap \
             fidl_lda3 \
             fidl_timestr \
             fidl_timestr2 \
             fidl_or \
             fidl_hemitxt \
             fidl_split \
             fidl_eog \
             fidl_mvpa4 \
             fidl_scratchdir \
             fidl_zeromean \
             fidl_combinations \
             fidl_copy_contrasts \
             fidl_mask \
             fidl_threshclus \
             fidl_motionreg \
             fidl_glmcond)

#cp -p ${BINDIR_LINUX64}/libFidl.a $DL64
#START151118
cp -p ${BINDIR_LINUX64}/libfidlc.so $DL64

cp -p ${BINDIR_LINUX64}/libfidl.so $DL64
cp -p ${BINDIR_LINUX64}/libfidlcifti.so $DL64
foreach i ($BLIST)
    cp -p ${BINDIR_LINUX64}/$i $DL64
end

#cp -p ~/gsl/lib_linux5/libgsl.so.0.17.0 $DL64
#cp -p ~/gsl/lib_linux5/libgslcblas.so.0.0.0 $DL64
#START151116
cp -p /home/hannah/mcavoy/FIDL/cifti/build/Cifti/libcifti.so $DL64
cp -p /home/hannah/mcavoy/FIDL/cifti/build/Nifti/libnifti.so $DL64
cp -p /home/hannah/mcavoy/FIDL/cifti/build/Common/libcommon.so $DL64
cp -p /home/hannah/mcavoy/FIDL/gsl1.16/lib/libgsl.so $DL64
cp -p /home/hannah/mcavoy/FIDL/gsl1.16/lib/libgslcblas.so $DL64

#STARTHERE

cd ..
echo "pro main" > main.pro
echo defsysv,"'"\!FIDL_REV"'",${REV} >> main.pro
echo @stats.h >> main.pro
echo "fontnum = " long"("getenv"('"FIDL_FONTNUM"'))" >> main.pro
echo "if(n_elements(fontnum) gt 0)" then begin >> main.pro
echo     "case fontnum of" >> main.pro
echo         1: font = "'"0-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1"'" >> main.pro
echo         2: font="'"-adobe-helvetica-medium-r-normal--14-140-75-75-p-77-iso8859-1"'" >> main.pro
echo         3: font = "'"-adobe-helvetica-bold-r-normal--14-140-75-75-p-77-iso8859-1"'" >> main.pro
echo         4: font = "'"-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso8859-1"'" >> main.pro
echo         else: print,"'"Using default font."'" >> main.pro
echo     endcase >> main.pro
echo  -n   "if(n_elements(font) gt 0) then " >> main.pro
echo     "    widget_control,DEFAULT_FONT=font" >> main.pro
echo endif >> main.pro
echo stats_exec >> main.pro
echo return >> main.pro
echo end >> main.pro

setenv IDL_DIR /opt/idl8.3/idl83
set DI2 = $DMORE/$IDL2
csh $D/make_runtime64.csh $DI2
cp linecolors.dat $DI2
chmod 664 $DI2/linecolors.dat
set FIDL_TAR = fidl_release_${REVNAME}.tar
echo $FIDL_TAR
cd fidl
tar cf $D/archive/tar64/$FIDL_TAR ${FIDL_DIR} 
gzip $D/archive/tar64/$FIDL_TAR

cd ..
/bin/rm -f archive/idl/*
/bin/rm -f archive/C/*
cp *.pro archive/idl
cp *.h archive/idl
cp *.csh archive/idl
cp *.txt archive/idl
cp *.dat archive/idl
cp Makefile archive/idl
cp clib/*.c archive/C
cp clib/*.h archive/C
cp clib/Makefile archive/C
set SRC_TAR = ./fidl_archive_${REVNAME}.tar

#tar cf archive/tar/$SRC_TAR archive/idl archive/C
#compress archive/tar/$SRC_TAR
#START151111
tar cf archive/tar64/$SRC_TAR archive/idl archive/C
gzip archive/tar64/$SRC_TAR

echo ${INSTALL_PATH}
mkdir ${INSTALL_PATH}
cd fidl/${FIDL_DIR}
cp -r scripts ${INSTALL_PATH}

#START151116
#cp -r bin ${INSTALL_PATH}
#cp -r bin_linux ${INSTALL_PATH}

cp -r bin_linux64 ${INSTALL_PATH}

#/bin/rm -f $DI/idl_startup.pro \
#           $DI/make_fidl.csh
#           $DI/fidl_rt.txt \
#           $DI/fidl_license.txt \
#           $DI/make_fidl_rt.csh
#START151119
#/bin/rm -f $DI2/idl_startup.pro \
#           $DI2/make_fidl.csh
#           $DI2/fidl_rt.txt \
#           $DI2/fidl_license.txt \
#           $DI2/make_fidl_rt.csh
#START151119
#/bin/rm -f $DI2/idl_startup.pro \
#           $DI2/make_fidl.csh
#           $DI2/fidl_license.txt \
#           $DI2/make_fidl_rt64.csh
#START151216
/bin/rm -f $DI2/idl_startup.pro $DI2/make_fidl.csh $DI2/make_fidl_rt64.csh

#START151116
#cp -r $IDL1 ${INSTALL_PATH}

cp -r $IDL2 ${INSTALL_PATH}

#crle -c ${INSTALL_PATH}/bin/config -u -l ${INSTALL_PATH}/bin
#crle -c ${INSTALL_PATH}/bin_linux/config -u -l ${INSTALL_PATH}/bin_linux
#crle -c ${INSTALL_PATH}/bin_linux64/config -u -l ${INSTALL_PATH}/bin_linux64

csh $D/make_fidl.csh $INSTALL_PATH $FIDL_PATH $REV

#set LICENSE_FILE = /usr/local/pkg/idl_5.3/license.dat
#csh $D/make_fidl_rt.csh $LIB_PATH $INSTALL_PATH $D $IDL_DIR $LICENSE_FILE
#START151116
set IDL_DIR1 = /home/usr/fidl/vm/idl83
#csh $D/make_fidl_rt64.csh $LIB_PATH $INSTALL_PATH $D $IDL_DIR1 $LICENSE_FILE
#START151119
csh $D/make_fidl_rt64.csh $LIB_PATH $INSTALL_PATH $D $IDL_DIR1

ln -s /usr/local/lib/libgcc_s.so.1 ${INSTALL_PATH}/bin/libgcc_s.so.1
cd ..
/bin/rm -rf $DMORE
