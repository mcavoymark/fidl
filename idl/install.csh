#!/bin/csh
#Copyright 12/31/99 Washington University.  All Rights Reserved.
#install.csh  $Revision: 12.296 $

umask

if($#argv < 3) then                                   #if 3rd argument in present, don't make binaries
    echo 'Usage: install.csh revision_number <install> NOBIN'
    exit                                    #0 or 1=install
endif
set IDL1 = idl #idl_5.3

set IDL2 = idl_6.3
#START151111
#set IDL2 = idl_8.3

set REV = $1
set FIDL_DIR = fidl_${REV}
set REVNAME = rev${REV}
if($2 == 0) then
    set INSTALL = 0
else if($2 == 1) then
    set INSTALL = 1
else
    echo "$2" is unknown
    exit
endif

set REVNAME = `date | gawk '{printf("%s%s%s",substr($6,3),substr($2,1,3),$3)}'`_${REVNAME}

if($#argv == 3) then
    set NOBIN = 1
else
    set NOBIN = 0
endif
echo argv = $#argv
echo NOBIN = $NOBIN
echo INSTALL = $INSTALL

#set LOCAL = /usr/local

#set FIDL_PATH = /usr/local/bin/fidl_code
set FIDL_PATH = /home/usr/fidl/fidl_code 

set INSTALL_PATH = ${FIDL_PATH}/${FIDL_DIR}
set LIB_PATH = /home/usr/fidl/lib
set D = /home/usr/fidl/idl
set DMORE = $D/fidl/${FIDL_DIR}
set DB = $DMORE/bin
set DL = $DMORE/bin_linux
set DL64 = $DMORE/bin_linux64
co Makefile
make checkout64

#START151119
#co -r1.316 define_single_dsgn_matrix_new.pro 

#START151111
#set BINDIR = ${BIN}
#set BINDIR_LINUX = ${HOME}/bin_linux

set BINDIR_LINUX64 = ${HOME}/bin_linux64

cd $D
mkdir fidl
cd fidl
mkdir ${FIDL_DIR} 
mkdir ${FIDL_DIR}/bin
mkdir ${FIDL_DIR}/bin_linux
mkdir ${FIDL_DIR}/bin_linux64
mkdir ${FIDL_DIR}/$IDL1
mkdir ${FIDL_DIR}/$IDL2
mkdir ${FIDL_DIR}/scripts

cd ..
cd clib 
#/bin/rm *.o
#/bin/rm *.os
#opt
#co Makefile
#make checkout
#make lib
#make slib
set BLIST = (compute_glm2 \
             fidl_zstat \
             fidl_zstat2 \
             #compute_avg_zstat \
             #fidl_avg_zstat \
             fidl_avg_zstat2 \
             fidl_bolds \
             fidl_tc_ss \
             #fidl_anova \
             #fidl_anova_new \
             #fidl_anova2 \
             #fidl_anova3 \
             fidl_anova4 \
             fidl_ttest \
             fidl_logreg_ss \
             #compute_mult_comp \
             #compute_mult_comp_spm \
             #compute_mult_comp_unified \
             #compute_bonferroni \
             #transform_f_to_z \
             #transform_t_field_to_zmap \
             fidl_f_to_z \
             fidl_t_to_z \
             fidl_r_to_z \
             convert_af3d_to_fidl \
             compute_region_stats \
             #compute_residuals \
             fidl_residuals \
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
             fidl_pcat \
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
             fidl_glmcond \
             fidl_assemble \
             fidl_chunkstack \
             fidl_flipglm \
             fidl_fdr)
if($NOBIN == 0) then
    foreach i ($BLIST)
        make $i
    end
endif
#cp -p  libfidl.a $DB
#cp -p fidl.so $DB

#START151111
#cp -p ${BINDIR}/libfidl.a $DB
#cp -p ${BINDIR}/fidl.so $DB
#foreach i ($BLIST)
#    cp -p  ${BINDIR}/$i $DB
#end
##cp -p ~/randy/* $DB
##cp -p ~/gsl/lib_hannah/*so* $DB
#cp -p ~/gsl/lib_hannah/libgsl.so.0.12.0 $DB
#cp -p ~/gsl/lib_hannah/libgslcblas.so.0.0.0 $DB
#cp -p ${BINDIR_LINUX}/libfidl.a $DL
#cp -p ${BINDIR_LINUX}/fidl.so $DL
#foreach i ($BLIST)
#    cp -p ${BINDIR_LINUX}/$i $DL
#end
##cp -p ~/gsl/lib_botticelli/*so* $DL
#cp -p ~/gsl/lib_botticelli/libgsl.so.0.12.0 $DL
#cp -p ~/gsl/lib_botticelli/libgslcblas.so.0.0.0 $DL

#cp -p ${BINDIR_LINUX64}/libfidl.a $DL64
cp -p ${BINDIR_LINUX64}/fidl.so $DL64
#START150413
cp -p ${BINDIR_LINUX64}/libFidl.a $DL64
cp -p ${BINDIR_LINUX64}/libfidl.so $DL64
cp -p ${BINDIR_LINUX64}/libfidlcifti.so $DL64

#START160722
cp -p ${BINDIR_LINUX64}/libfidlc.so $DL64

foreach i ($BLIST)
    cp -p ${BINDIR_LINUX64}/$i $DL64
end

#cp -p ~/gsl/lib_xena/libgsl.so.0.12.0 $DL64
#cp -p ~/gsl/lib_xena/libgslcblas.so.0.0.0 $DL64
#START150413
cp -p ~/gsl/lib_linux5/libgsl.so.0.17.0 $DL64
cp -p ~/gsl/lib_linux5/libgslcblas.so.0.0.0 $DL64

#START160803
cp -p /home/hannah/mcavoy/FIDL/cifti/build/Cifti/libcifti.so $DL64
cp -p /home/hannah/mcavoy/FIDL/cifti/build/Nifti/libnifti.so $DL64
cp -p /home/hannah/mcavoy/FIDL/cifti/build/Common/libcommon.so $DL64


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

#START160222
#setenv IDL_DIR /usr/local/pkg/idl_5.3
#set DI = $DMORE/$IDL1
#csh $D/make_runtime.csh $DI
#cp idl_startup.pro $DI
#cp make_fidl.csh $DI
#cp linecolors.dat $DI
#cp fidl_rt.txt $DI
#cp fidl_license.txt $DI
#cp make_fidl_rt.csh $DI
#chmod 664 $DI/idl_startup.pro
#chmod 664 $DI/make_fidl.csh
#chmod 664 $DI/linecolors.dat
#chmod 664 $DI/fidl_rt.txt
#chmod 664 $DI/fidl_license.txt
#chmod 664 $DI/make_fidl_rt.csh

#setenv LM_LICENSE_FILE /usr/local/pkg/idl_6.3/license.dat

setenv IDL_DIR /usr/local/pkg/idl_6.3
#START151111
#setenv IDL_DIR /opt/idl8.3/idl83

#setenv IDL_PATH +/home/usr/fidl/idl/cti_rev7:`pwd`:+$IDL_DIR\/lib
set DI2 = $DMORE/$IDL2
csh $D/make_runtime.csh $DI2
cp linecolors.dat $DI2
chmod 664 $DI2/linecolors.dat

#START160616
cp make_fidl.csh $DI2
chmod 664 $DI2/make_fidl.csh
cp make_fidl_rt.csh $DI2
chmod 664 $DI2/make_fidl_rt.csh

#START170221
cp make_fidl_rt_export.csh $DI2
chmod 664 $DI2/make_fidl_rt_export.csh

#START160720
cp fidl_rt.txt $DI2
chmod 664 $DI2/fidl_rt.txt
cp fidl_license.txt $DI2
chmod 664 $DI2/fidl_license.txt


set FIDL_TAR = fidl_release_${REVNAME}.tar
echo $FIDL_TAR
cd fidl

tar cf $D/archive/tar/$FIDL_TAR ${FIDL_DIR} 
compress $D/archive/tar/$FIDL_TAR
#START151111
#tar cf $D/archive/tar64/$FIDL_TAR ${FIDL_DIR} 
#gzip $D/archive/tar64/$FIDL_TAR

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

tar cf archive/tar/$SRC_TAR archive/idl archive/C
compress archive/tar/$SRC_TAR
#START151111
#tar cf archive/tar64/$SRC_TAR archive/idl archive/C
#gzip archive/tar64/$SRC_TAR

echo ${INSTALL_PATH}
mkdir ${INSTALL_PATH}
cd fidl/${FIDL_DIR}
cp -r scripts ${INSTALL_PATH}
cp -r bin ${INSTALL_PATH}
cp -r bin_linux ${INSTALL_PATH}
cp -r bin_linux64 ${INSTALL_PATH}

#START160222
#/bin/rm -f $DI/idl_startup.pro \
#           $DI/make_fidl.csh
#           $DI/fidl_rt.txt \
#           $DI/fidl_license.txt \
#           $DI/make_fidl_rt.csh #\

cp -r $IDL1 ${INSTALL_PATH}
cp -r $IDL2 ${INSTALL_PATH}

#crle -c ${INSTALL_PATH}/bin/config -u -l ${INSTALL_PATH}/bin
#crle -c ${INSTALL_PATH}/bin_linux/config -u -l ${INSTALL_PATH}/bin_linux
#crle -c ${INSTALL_PATH}/bin_linux64/config -u -l ${INSTALL_PATH}/bin_linux64

csh $D/make_fidl.csh $INSTALL_PATH $FIDL_PATH $REV

#csh $D/make_fidl_rt_NIL.csh $LIB_PATH $INSTALL_PATH $D

#make_fidl_rt.csh LIB_PATH INSTALL_PATH FIDL_RT_PATH IDL_DIR LICENSE_FILE
#START150413
set LICENSE_FILE = /usr/local/pkg/idl_5.3/license.dat
#csh $D/make_fidl_rt.csh $LIB_PATH $INSTALL_PATH $D $IDL_DIR $LICENSE_FILE
#START160720
set VM_DIR = /home/usr/fidl/vm/idl71linux
csh $D/make_fidl_rt.csh $LIB_PATH $INSTALL_PATH $D $IDL_DIR $VM_DIR $LICENSE_FILE



#set LICENSE_FILE = /usr/local/pkg/idl_5.3/license.dat
set RT_DIR = /usr/local/pkg/idl_5.3
set VM_DIR_SUN = /home/usr/fidl/vm/idl71solaris2
set VM_DIR_LINUX = /home/usr/fidl/vm/idl71linux 
#csh $D/make_fidl_rt_NILPSYCH.csh $LIB_PATH $INSTALL_PATH $D $LICENSE_FILE $RT_DIR $VM_DIR_SUN $VM_DIR_LINUX 

if($INSTALL == 1) then
    set versions = `/bin/ls ${FIDL_PATH} | grep fidl`
    #/bin/rm ${LOCAL}/bin/fidl
    #/bin/rm ${LOCAL}/bin/fidl_rt
    #/bin/rm ${LOCAL}/bin/fidl_rt_old
    #/bin/rm ${LOCAL}/bin/fidl_rt_oldold
    #if($#argv != 2) then
    #    foreach i ($BLIST)
    #        /bin/rm ${LOCAL}/bin/$i
    #    end
    #endif
    #/bin/rm ${LOCAL}/bin/anat222
    #/bin/rm ${LOCAL}/bin/checkbold
    #/bin/rm ${LOCAL}/bin/checkmprage
    #/bin/rm ${LOCAL}/bin/checkmprage111
    #/bin/rm ${LOCAL}/bin/fixanat
    #/bin/rm ${LOCAL}/bin/fixanat_4dfp
    #/bin/rm ${LOCAL}/bin/hor2cor
    #/bin/rm ${LOCAL}/bin/lineval
    #/bin/rm ${LOCAL}/bin/repack
    #/bin/rm ${LOCAL}/bin/stackfix_4dfp
    #/bin/rm ${LOCAL}/bin/transpose
    #/bin/rm ${LOCAL}/bin/view_4dfp
    #ln -s ${INSTALL_PATH}/scripts/fidl ${LOCAL}/bin/fidl
    #ln -s ${INSTALL_PATH}/scripts/fidl_rt ${LOCAL}/bin/fidl_rt
    #set N = `expr $#versions - 1`
    #ln -s ${FIDL_PATH}/$versions[$N]/scripts/fidl_rt ${LOCAL}/bin/fidl_rt_old
    #set N = `expr $#versions - 2`
    #ln -s ${FIDL_PATH}/$versions[$N]/scripts/fidl_rt ${LOCAL}/bin/fidl_rt_oldold
    #foreach i ($BLIST)
    #    ln -s ${INSTALL_PATH}/bin/$i ${LOCAL}/bin/$i
    #end 
    #ln -s ${INSTALL_PATH}/bin/anat222 ${LOCAL}/bin/anat222
    #ln -s ${INSTALL_PATH}/bin/checkbold ${LOCAL}/bin/checkbold
    #ln -s ${INSTALL_PATH}/bin/checkmprage ${LOCAL}/bin/checkmprage
    #ln -s ${INSTALL_PATH}/bin/checkmprage111 ${LOCAL}/bin/checkmprage111
    #ln -s ${INSTALL_PATH}/bin/fixanat ${LOCAL}/bin/fixanat
    #ln -s ${INSTALL_PATH}/bin/fixanat_4dfp ${LOCAL}/bin/fixanat_4dfp
    #ln -s ${INSTALL_PATH}/bin/hor2cor ${LOCAL}/bin/hor2cor
    #ln -s ${INSTALL_PATH}/bin/lineval ${LOCAL}/bin/lineval
    #ln -s ${INSTALL_PATH}/bin/repack ${LOCAL}/bin/repack
    #ln -s ${INSTALL_PATH}/bin/stackfix_4dfp ${LOCAL}/bin/stackfix_4dfp
    #ln -s ${INSTALL_PATH}/bin/transpose ${LOCAL}/bin/transpose
    #ln -s ${INSTALL_PATH}/bin/view_4dfp ${LOCAL}/bin/view_4dfp

    #START151111
    ln -s ${INSTALL_PATH}/bin/libgsl.so.0.12.0 ${INSTALL_PATH}/bin/libgsl.so
    ln -s ${INSTALL_PATH}/bin/libgsl.so.0.12.0 ${INSTALL_PATH}/bin/libgsl.so.0
    ln -s ${INSTALL_PATH}/bin/libgslcblas.so.0.0.0 ${INSTALL_PATH}/bin/libgslcblas.so
    ln -s ${INSTALL_PATH}/bin/libgslcblas.so.0.0.0 ${INSTALL_PATH}/bin/libgslcblas.so.0
    ln -s ${INSTALL_PATH}/bin_linux/libgsl.so.0.12.0 ${INSTALL_PATH}/bin_linux/libgsl.so
    ln -s ${INSTALL_PATH}/bin_linux/libgsl.so.0.12.0 ${INSTALL_PATH}/bin_linux/libgsl.so.0
    ln -s ${INSTALL_PATH}/bin_linux/libgslcblas.so.0.0.0 ${INSTALL_PATH}/bin_linux/libgslcblas.so
    ln -s ${INSTALL_PATH}/bin_linux/libgslcblas.so.0.0.0 ${INSTALL_PATH}/bin_linux/libgslcblas.so.0
    ln -s ${INSTALL_PATH}/bin_linux64/libgsl.so.0.17.0 ${INSTALL_PATH}/bin_linux64/libgsl.so
    ln -s ${INSTALL_PATH}/bin_linux64/libgsl.so.0.17.0 ${INSTALL_PATH}/bin_linux64/libgsl.so.0
    ln -s ${INSTALL_PATH}/bin_linux64/libgslcblas.so.0.0.0 ${INSTALL_PATH}/bin_linux64/libgslcblas.so
    ln -s ${INSTALL_PATH}/bin_linux64/libgslcblas.so.0.0.0 ${INSTALL_PATH}/bin_linux64/libgslcblas.so.0

    ln -s /usr/local/lib/libgcc_s.so.1 ${INSTALL_PATH}/bin/libgcc_s.so.1
endif
cd ..
/bin/rm -rf $DMORE
