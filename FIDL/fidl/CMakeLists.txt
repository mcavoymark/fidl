#cmake_minimum_required(VERSION 2.8)
cmake_minimum_required(VERSION 2.6)
#cmake_minimum_required(VERSION 3.1)
#
# Name of Project
#
PROJECT(fidl)

#START150423
##SET(LIBS0 ${LIBS})

#SET(CMAKE_CXX_FLAGS "-W -Wall")
#START150522
#SET(CMAKE_CXX_FLAGS "-W -Wall -O3")
#SET(CMAKE_CXX_FLAGS "-W -Wall -Wextra -O3")
#START190702
SET(CMAKE_CXX_FLAGS "-W -Wall -Wextra -O3 -std=c++0x")
#START191010
#SET(CMAKE_CXX_FLAGS "-W -Wall -Wextra -O3 -std=c++11")

#SET(CMAKE_C_FLAGS "-W -Wall -O3 -std=c99")
SET(CMAKE_C_FLAGS "-W -Wall -Wextra -O3 -std=c99")

SET(IGNORE_QT FALSE CACHE BOOL "don't try to use QT")

#START190805
SET(L0 "/home/usr/mcavoy/GITHUB/c")

#
# Use QT for xml/string
#
IF (NOT IGNORE_QT)
    FIND_PACKAGE(Qt4 4.8)
    IF (QT_FOUND)
        SET(QT_DONT_USE_QTGUI TRUE)
        SET(CIFTILIB_USE_QT TRUE)
        ADD_DEFINITIONS(-DCIFTILIB_USE_QT)
        INCLUDE(${QT_USE_FILE})
        SET(LIBS ${LIBS} ${QT_LIBRARIES})
    ENDIF (QT_FOUND)
ENDIF (NOT IGNORE_QT)

IF (NOT QT_FOUND)
    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
    FIND_PACKAGE(libxml++ REQUIRED)
    INCLUDE_DIRECTORIES(${libxml++_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${libxml++_LIBRARIES})
    ADD_DEFINITIONS(-DCIFTILIB_USE_XMLPP)
ENDIF (NOT QT_FOUND)

IF (NOT QT_FOUND)
    FIND_PACKAGE(Boost REQUIRED COMPONENTS filesystem system)
ELSE (NOT QT_FOUND)
    FIND_PACKAGE(Boost REQUIRED)
ENDIF (NOT QT_FOUND)
SET(LIBS ${LIBS} ${Boost_LIBRARIES})
IF (Boost_VERSION LESS 104400)
    #absolute() was added in 1.44.0, with filesystem v3
    ADD_DEFINITIONS(-DCIFTILIB_BOOST_NO_FSV3)
ENDIF (Boost_VERSION LESS 104400)
IF (Boost_VERSION LESS 104800)
    #canonical() was added in 1.48.0
    ADD_DEFINITIONS(-DCIFTILIB_BOOST_NO_CANONICAL)
ENDIF (Boost_VERSION LESS 104800)
IF (Boost_VERSION LESS 105600)
    #try_lexical_cast was added in 1.56.0
    ADD_DEFINITIONS(-DCIFTILIB_BOOST_NO_TRY_LEXICAL)
ENDIF (Boost_VERSION LESS 105600)

#
# Need XML from Qt
#
#SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
#INCLUDE(${QT_USE_FILE})


#START200825
FIND_PACKAGE(ZLIB)
SET(LIBS ${LIBS} ${ZLIB_LIBRARIES})


#FIND_LIBRARY(CIFTI NAMES Cifti libCifti PATHS /home/hannah/mcavoy/FIDL/cifti/build/Cifti)
#FIND_LIBRARY(NIFTI NAMES Nifti libNifti PATHS /home/hannah/mcavoy/FIDL/cifti/build/Nifti)
#FIND_LIBRARY(COMMON NAMES Common libCommon PATHS /home/hannah/mcavoy/FIDL/cifti/build/Common)
#FIND_LIBRARY(GSL NAMES gsl libgsl PATHS /home/hannah/mcavoy/FIDL/gsl1.16/lib)
#FIND_LIBRARY(GSLCBLAS NAMES gslcblas libgslcblas PATHS /home/hannah/mcavoy/FIDL/gsl1.16/lib)
#START150424
add_library(Cifti STATIC IMPORTED)
set_property(TARGET Cifti PROPERTY IMPORTED_LOCATION /home/usr/mcavoy/FIDL/cifti/build/Cifti/libCifti.a)
add_library(Nifti STATIC IMPORTED)
set_property(TARGET Nifti PROPERTY IMPORTED_LOCATION /home/usr/mcavoy/FIDL/cifti/build/Nifti/libNifti.a)
add_library(Common STATIC IMPORTED)
set_property(TARGET Common PROPERTY IMPORTED_LOCATION /home/usr/mcavoy/FIDL/cifti/build/Common/libCommon.a)
add_library(Gsl STATIC IMPORTED)
set_property(TARGET Gsl PROPERTY IMPORTED_LOCATION /home/usr/mcavoy/FIDL/gsl1.16/lib/libgsl.a)
add_library(Gslcblas STATIC IMPORTED)
set_property(TARGET Gslcblas PROPERTY IMPORTED_LOCATION /home/usr/mcavoy/FIDL/gsl1.16/lib/libgslcblas.a)
 
#START151113
add_library(cifti SHARED IMPORTED)
set_property(TARGET cifti PROPERTY IMPORTED_LOCATION /home/usr/mcavoy/FIDL/cifti/build/Cifti/libcifti.so)
add_library(nifti SHARED IMPORTED)
set_property(TARGET nifti PROPERTY IMPORTED_LOCATION /home/usr/mcavoy/FIDL/cifti/build/Nifti/libnifti.so)
add_library(common SHARED IMPORTED)
set_property(TARGET common PROPERTY IMPORTED_LOCATION /home/usr/mcavoy/FIDL/cifti/build/Common/libcommon.so)

#START151113
add_library(gsl SHARED IMPORTED)
set_property(TARGET gsl PROPERTY IMPORTED_LOCATION /home/usr/mcavoy/FIDL/gsl1.16/lib/libgsl.so)
add_library(gslcblas SHARED IMPORTED)
set_property(TARGET gslcblas PROPERTY IMPORTED_LOCATION /home/usr/mcavoy/FIDL/gsl1.16/lib/libgslcblas.so)

ADD_LIBRARY(Fidl
${L0}/interfile.c
${L0}/utilities.c
${L0}/twoA_or_twoB.c
${L0}/get_atlas_param.c
${L0}/get_atlas.c
${L0}/d2charvar.c
${L0}/files_struct.c
${L0}/shouldiswap.c
${L0}/machine_epsilon.c
${L0}/checkOS.c
${L0}/make_timestr2.c
${L0}/read_xform.c
${L0}/subs_util.c
${L0}/minmax.c
${L0}/d2intvar.c
${L0}/strutil.c)

ADD_EXECUTABLE(fidl_sleep ${L0}/fidl_sleep.c)
TARGET_LINK_LIBRARIES(fidl_sleep m Fidl)

ADD_EXECUTABLE(fidl_power120 ${L0}/fidl_power120.c)
TARGET_LINK_LIBRARIES(fidl_power120 fidlc)

ADD_EXECUTABLE(fidl_chloe ${L0}/fidl_chloe.c)
TARGET_LINK_LIBRARIES(fidl_chloe m Fidl)

ADD_EXECUTABLE(fidl_zstat2
${L0}/get_grand_mean_struct.c
${L0}/d2double.c
${L0}/d2float.c
${L0}/subs_glm.c
${L0}/t4_atlas.c
${L0}/gauss_smoth2.c

#${L0}/tags_tc_glmstack.c
${L0}/tags_tc_glmstack.cxx

${L0}/boynton_contrast.c
${L0}/boynton_model.c
${L0}/t_to_z.c
${L0}/make_scratchdir.c
${L0}/filetype.c
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/write1.c
${L0}/nifti_gethdr.cxx
${L0}/get_atlas_param.c
${L0}/read_tc_string_new.c
${L0}/header.cxx

#${L0}/region2.cxx
${L0}/region4.cxx

${L0}/get_limits.c
${L0}/mask.cxx

#${L0}/find_regions_by_file_cover.c
${L0}/find_regions_by_file_cover3.cxx

${L0}/get_indbase.c
${L0}/check_dimensions.cxx
${L0}/cifti_getDimensions.cxx
${L0}/stack.cxx
${L0}/nifti_flipslice.cxx

#${L0}/lut.cxx
${L0}/lut2.cxx

${L0}/fidl_zstat2.cxx)
TARGET_LINK_LIBRARIES(fidl_zstat2 Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_bolds
${L0}/d2double.c
${L0}/d2float.c
${L0}/d2int.c
${L0}/t4_atlas.c
${L0}/gauss_smoth2.c
${L0}/read_frames_file.c
${L0}/make_scratchdir.c
${L0}/filetype.c
${L0}/dim_param2.cxx
${L0}/read_tc_string_new.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/write1.c
${L0}/header.cxx
${L0}/subs_glm.c
${L0}/mask.cxx

#${L0}/find_regions_by_file_cover.c
${L0}/find_regions_by_file_cover3.cxx

##${L0}/make_timestr2.c
${L0}/timestr.cxx

${L0}/map_disk.c
${L0}/stack.cxx

#${L0}/region2.cxx
#${L0}/region3.cxx
${L0}/region4.cxx

${L0}/check_dimensions.cxx
${L0}/ptr.cxx

#${L0}/lut.cxx
${L0}/lut2.cxx

${L0}/lookuptable.cxx
${L0}/fidl_bolds.cxx)
TARGET_LINK_LIBRARIES(fidl_bolds Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_lut
${L0}/lut.cxx
${L0}/fidl_lut.cxx)
#TARGET_LINK_LIBRARIES(fidl_lut Fidl)

ADD_EXECUTABLE(fidl_lut_cluster

#${L0}/lut.cxx
${L0}/lut2.cxx

${L0}/cifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/filetype.c
${L0}/header.cxx
${L0}/stack.cxx
${L0}/subs_nifti.cxx
${L0}/fidl_lut_cluster.cxx)
TARGET_LINK_LIBRARIES(fidl_lut_cluster Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_lut_cluster_PET
${L0}/lut.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/filetype.c
${L0}/header.cxx
${L0}/stack.cxx
${L0}/subs_nifti.cxx
${L0}/region2.cxx
${L0}/fidl_lut_cluster_PET.cxx)
TARGET_LINK_LIBRARIES(fidl_lut_cluster_PET Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_lut_cluster_PET2
${L0}/lut.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/filetype.c
${L0}/header.cxx
${L0}/stack.cxx
${L0}/subs_nifti.cxx
${L0}/region2.cxx
${L0}/fidl_lut_cluster_PET2.cxx)
TARGET_LINK_LIBRARIES(fidl_lut_cluster_PET2 Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_PETcsv_stack
${L0}/subs_nifti.cxx
${L0}/lut.cxx
${L0}/fidl_PETcsv_stack.cxx)
TARGET_LINK_LIBRARIES(fidl_PETcsv_stack Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_wmh
${L0}/header.cxx
${L0}/stack.cxx
${L0}/mask.cxx
${L0}/subs_glm.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/filetype.c
${L0}/subs_nifti.cxx
${L0}/d2float.c
${L0}/d2double.c
${L0}/fidl_wmh.cxx)
TARGET_LINK_LIBRARIES(fidl_wmh Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_PETcsv_stack2
${L0}/subs_nifti.cxx
${L0}/lut.cxx
${L0}/fidl_PETcsv_stack2.cxx)
TARGET_LINK_LIBRARIES(fidl_PETcsv_stack2 Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_txt_wmparc
${L0}/cifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/filetype.c
${L0}/subs_nifti.cxx
${L0}/lut2.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/fidl_txt_wmparc.cxx)
TARGET_LINK_LIBRARIES(fidl_txt_wmparc Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_separate_lut
${L0}/cifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/filetype.c
${L0}/subs_nifti.cxx
${L0}/lut2.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/fidl_separate_lut.cxx)
TARGET_LINK_LIBRARIES(fidl_separate_lut Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_sumconcpair
${L0}/d2double.c
${L0}/t4_atlas.c
${L0}/dim_param2.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/write1.c
${L0}/fidl_sumconcpair.c)
TARGET_LINK_LIBRARIES(fidl_sumconcpair Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_pcat
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/dim_param2.c
${L0}/d2double.c
${L0}/read_frames_file.c
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/gsl_svd.c
${L0}/fidl_pcat.c)
TARGET_LINK_LIBRARIES(fidl_pcat Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(compute_glm2
${L0}/d2double.c
${L0}/dim_param2.cxx
${L0}/subs_glm.c
${L0}/gsl_svd.c
${L0}/cond_norm1.c
${L0}/nrmlrv.c
${L0}/write_glm.c
${L0}/t4_atlas.c
${L0}/gauss_smoth2.c
${L0}/f_to_z.c
${L0}/cifti_getDimensions.cxx
${L0}/subs_cifti.cxx
${L0}/nifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/subs_nifti.cxx
${L0}/nifti_flipslice.cxx
${L0}/dim_param2.cxx
${L0}/d2float.c
${L0}/filetype.c
${L0}/d2int.c
${L0}/header.cxx
${L0}/find_regions_by_file_cover.c
${L0}/check_dimensions.cxx
${L0}/region2.cxx
${L0}/mask.cxx
${L0}/stack.cxx
${L0}/lut.cxx
${L0}/linmod6.cxx
${L0}/compute_glm2.c)
#${L0}/compute_glm2.cxx)
TARGET_LINK_LIBRARIES(compute_glm2 Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(compute_glm27
${L0}/d2double.c
${L0}/dim_param2.cxx
${L0}/subs_glm.c
${L0}/gsl_svd.c
${L0}/cond_norm1.c
${L0}/nrmlrv.c
${L0}/write_glm.c
${L0}/t4_atlas.c
${L0}/gauss_smoth2.c
${L0}/f_to_z.c
${L0}/cifti_getDimensions.cxx
${L0}/subs_cifti.cxx
${L0}/nifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/subs_nifti.cxx
${L0}/nifti_flipslice.cxx
${L0}/dim_param2.cxx
${L0}/d2float.c
${L0}/filetype.c
${L0}/d2int.c
${L0}/header.cxx

#${L0}/find_regions_by_file_cover.c
#${L0}/find_regions_by_file_cover2.c
${L0}/find_regions_by_file_cover3.cxx

${L0}/check_dimensions.cxx

#${L0}/region2.cxx
#${L0}/region3.cxx
${L0}/region4.cxx

${L0}/mask.cxx
${L0}/stack.cxx

#${L0}/lut.cxx
${L0}/lut2.cxx

${L0}/timestr.cxx

#${L0}/linmod6.cxx
${L0}/linmod7.cxx

${L0}/compute_glm2.c)
#${L0}/compute_glm2.cxx)
TARGET_LINK_LIBRARIES(compute_glm27 Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})


#ADD_EXECUTABLE(fidl_anova4
#${L0}/d2double.c
#${L0}/get_grand_mean_struct.c
#${L0}/box_correction.c
#${L0}/read_driver2.c
#${L0}/gsl_svd.c
#${L0}/cond_norm1.c
#${L0}/dmatrix_mult.c
#${L0}/t4_atlas.c
#${L0}/nrutil.c
#${L0}/get_atlas_coor.c
#${L0}/f_to_z.c
#${L0}/inverse.c
#${L0}/monte_carlo_mult_comp.c
#${L0}/spatial_extent2.c
#${L0}/center_of_mass2.c
#${L0}/assign_region_names2.c
#${L0}/subs_cifti.cxx
#${L0}/subs_nifti.cxx
#${L0}/nifti_gethdr.cxx
#${L0}/write1.c
#${L0}/filetype.c
#${L0}/read_tc_string_new.c
#${L0}/subs_glm.c
#${L0}/subs_mask.c
#${L0}/map_disk.c
#${L0}/d2float.c
#${L0}/d2int.c
#${L0}/find_regions_by_file_cover.c
#${L0}/cifti_getDimensions.cxx
#${L0}/header.cxx
#${L0}/check_dimensions.cxx
#${L0}/lut.cxx
#${L0}/lookuptable.cxx
#${L0}/fidl_anova4.c)
#TARGET_LINK_LIBRARIES(fidl_anova4 Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_anova5
${L0}/d2double.c
${L0}/get_grand_mean_struct.c
${L0}/box_correction.c
${L0}/read_driver2.c
${L0}/gsl_svd.c
${L0}/cond_norm1.c
${L0}/dmatrix_mult.c
${L0}/t4_atlas.c
${L0}/nrutil.c
${L0}/get_atlas_coor.c
${L0}/f_to_z.c
${L0}/inverse.c
${L0}/monte_carlo_mult_comp.c
${L0}/spatial_extent2.c
${L0}/center_of_mass2.c
${L0}/assign_region_names2.c
${L0}/subs_cifti.cxx
${L0}/subs_nifti.cxx
${L0}/nifti_gethdr.cxx
${L0}/write1.c
${L0}/filetype.c
${L0}/read_tc_string_new.c
${L0}/subs_glm.c
${L0}/subs_mask.c
${L0}/map_disk.c
${L0}/d2float.c
${L0}/d2int.c
${L0}/find_regions_by_file_cover.c
${L0}/cifti_getDimensions.cxx
${L0}/header.cxx
${L0}/check_dimensions.cxx
${L0}/lut2.cxx
${L0}/lookuptable.cxx
${L0}/fidl_anova5.c)
TARGET_LINK_LIBRARIES(fidl_anova5 Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_anova6
${L0}/d2double.c
${L0}/get_grand_mean_struct.c
${L0}/box_correction.c
${L0}/read_driver2.c
${L0}/gsl_svd.c
${L0}/cond_norm1.c
${L0}/dmatrix_mult.c
${L0}/t4_atlas.c
${L0}/nrutil.c
${L0}/get_atlas_coor.c
${L0}/f_to_z.c
${L0}/inverse.c
${L0}/monte_carlo_mult_comp.c
${L0}/spatial_extent2.c
${L0}/center_of_mass2.c
${L0}/assign_region_names2.c
${L0}/subs_cifti.cxx
${L0}/subs_nifti.cxx
${L0}/nifti_gethdr.cxx
${L0}/write1.c
${L0}/filetype.c
${L0}/read_tc_string_new.c
${L0}/subs_glm.c
${L0}/mask.cxx
${L0}/stack.cxx
${L0}/header.cxx
${L0}/nifti_flipslice.cxx
${L0}/map_disk.c
${L0}/d2float.c
${L0}/d2int.c
${L0}/find_regions_by_file_cover3.cxx
${L0}/cifti_getDimensions.cxx
${L0}/header.cxx
${L0}/check_dimensions.cxx
${L0}/lut2.cxx
${L0}/lookuptable.cxx
${L0}/fidl_anova6.cxx)
TARGET_LINK_LIBRARIES(fidl_anova6 Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})


#START150424
ADD_LIBRARY(fidlcifti2 SHARED
${L0}/subs_cifti.cxx)
FIND_LIBRARY(CIFTISO NAMES cifti libcifti PATHS /home/usr/mcavoy/FIDL/cifti/build/Cifti)
FIND_LIBRARY(NIFTISO NAMES nifti libnifti PATHS /home/usr/mcavoy/FIDL/cifti/build/Nifti)
FIND_LIBRARY(COMMONSO NAMES common libcommon PATHS /home/usr/mcavoy/FIDL/cifti/build/Common)
TARGET_LINK_LIBRARIES(fidlcifti2 ${CIFTISO} ${NIFTISO} ${COMMONSO} ${LIBS})

ADD_EXECUTABLE(fidl_ttest
${L0}/read_ttest_driver.c
${L0}/t_to_z.c
${L0}/get_atlas_coor.c
${L0}/monte_carlo_mult_comp.c
${L0}/spatial_extent2.c
${L0}/center_of_mass2.c
${L0}/assign_region_names2.c
${L0}/subs_cifti.cxx
${L0}/subs_nifti.cxx
${L0}/write1.c
${L0}/dim_param2.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/filetype.c
${L0}/d2double.c
${L0}/map_disk.c
${L0}/d2float.c
${L0}/d2int.c
${L0}/header.cxx
${L0}/nifti_gethdr.cxx
${L0}/subs_glm.c
${L0}/mask.cxx
${L0}/stack.cxx
${L0}/nifti_flipslice.cxx
${L0}/fidl_ttest.cxx)
TARGET_LINK_LIBRARIES(fidl_ttest Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

#ADD_EXECUTABLE(fidl_avg_zstat2
#${L0}/get_grand_mean_struct.c
#${L0}/d2double.c
#${L0}/subs_glm.c
#${L0}/tags_tc_glmstack.c
#${L0}/boynton_contrast.c
#${L0}/boynton_model.c
#${L0}/t_to_z.c
#${L0}/t4_atlas.c
#${L0}/gauss_smoth2.c
#${L0}/count_tab.c
#${L0}/read_data.c
#${L0}/write1.c
#${L0}/subs_nifti.cxx
#${L0}/subs_cifti.cxx
#${L0}/filetype.c
#${L0}/read_tc_string_new.c
#${L0}/d2float.c
#${L0}/mask.cxx
#${L0}/find_regions_by_file_cover.c
#${L0}/get_indbase.c
#${L0}/region2.cxx
#${L0}/header.cxx
#${L0}/check_dimensions.cxx
#${L0}/nifti_gethdr.cxx
#${L0}/map_disk.c
#${L0}/stack.cxx
#${L0}/lut.cxx
#${L0}/cifti_getDimensions.cxx
#${L0}/nifti_flipslice.cxx
#${L0}/lut.cxx
#${L0}/fidl_avg_zstat2.cxx)
#TARGET_LINK_LIBRARIES(fidl_avg_zstat2 Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_avg_zstat3
${L0}/get_grand_mean_struct.c
${L0}/d2double.c
${L0}/subs_glm.c

#${L0}/tags_tc_glmstack.c
${L0}/tags_tc_glmstack.cxx

${L0}/boynton_contrast.c
${L0}/boynton_model.c
${L0}/t_to_z.c
${L0}/t4_atlas.c
${L0}/gauss_smoth2.c
${L0}/count_tab.c
${L0}/read_data.c
${L0}/write1.c
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/filetype.c
${L0}/read_tc_string_new.c
${L0}/d2float.c
${L0}/mask.cxx

#${L0}/find_regions_by_file_cover.c
${L0}/find_regions_by_file_cover3.cxx

${L0}/get_indbase.c

#${L0}/region2.cxx
#${L0}/region3.cxx
${L0}/region4.cxx

${L0}/header.cxx
${L0}/check_dimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/map_disk.c
${L0}/stack.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx

#${L0}/lut.cxx
${L0}/lut2.cxx

${L0}/lookuptable.cxx
${L0}/ptrw.cxx
${L0}/fidl_avg_zstat3.cxx)
TARGET_LINK_LIBRARIES(fidl_avg_zstat3 Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_residuals
${L0}/get_grand_mean_struct.c
${L0}/d2double.c
${L0}/t4_atlas.c
${L0}/gauss_smoth2.c
${L0}/nrutil.c
${L0}/tred2.c
${L0}/eigsrt.c
${L0}/tqli.c
${L0}/pythag.c
${L0}/pca.c
${L0}/read_frames_file.c
${L0}/make_scratchdir.c
${L0}/mask.cxx
${L0}/header.cxx
${L0}/stack.cxx

#${L0}/region2.cxx
#${L0}/find_regions_by_file_cover.c
#${L0}/lut.cxx
${L0}/region3.cxx
${L0}/find_regions_by_file_cover3.cxx
${L0}/lut2.cxx

${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/dim_param2.cxx
${L0}/read_tc_string_new.c
${L0}/write1.c
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/filetype.c
${L0}/map_disk.c
${L0}/subs_glm.c
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/d2float.c
${L0}/d2int.c
${L0}/check_dimensions.cxx
${L0}/get_grand_mean_struct.c
${L0}/utilities2.c
${L0}/fidl_residuals.cxx)
TARGET_LINK_LIBRARIES(fidl_residuals Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_mvpa4
${L0}/subs_mask.c
${L0}/dim_param2.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/get_atlas_coor.c
${L0}/svm.cpp
${L0}/cond_norm1.c
${L0}/gsl_svd.c
${L0}/d2double.c
${L0}/read_driver2.c
${L0}/fidl_mvpa4.c)
TARGET_LINK_LIBRARIES(fidl_mvpa4 Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})


ADD_EXECUTABLE(fidl_gauss 
${L0}/d2double.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/dim_param2.c
${L0}/subs_mask.c
${L0}/gauss_smoth2.c
${L0}/minmax.c
${L0}/fidl_gauss.c)
TARGET_LINK_LIBRARIES(fidl_gauss Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_sim 
${L0}/fidl_sim.c
${L0}/spatial_extent2.c
${L0}/gauss_smoth2.c
${L0}/t_to_z.c
${L0}/f_to_z.c
${L0}/subs_mask.c
${L0}/subs_util.c)
TARGET_LINK_LIBRARIES(fidl_sim m Fidl Gsl Gslcblas)

ADD_EXECUTABLE(fidl_glmcond
${L0}/fidl_glmcond.c
${L0}/gsl_svd.c
${L0}/cond_norm1.c)
TARGET_LINK_LIBRARIES(fidl_glmcond m Fidl Gsl Gslcblas)

ADD_EXECUTABLE(fidl_timestr2
${L0}/fidl_timestr2.c)
TARGET_LINK_LIBRARIES(fidl_timestr2 Fidl)

ADD_EXECUTABLE(fidl_timestr3
${L0}/timestr.cxx
${L0}/fidl_timestr3.cxx)
TARGET_LINK_LIBRARIES(fidl_timestr3)

ADD_EXECUTABLE(fidl_scratchdir
${L0}/fidl_scratchdir.c
${L0}/make_scratchdir.c
${L0}/make_timestr2.c)

ADD_EXECUTABLE(compute_tc_reg_stats
${L0}/compute_tc_reg_stats.c
${L0}/t4_atlas.c
${L0}/dim_param2.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx)
TARGET_LINK_LIBRARIES(compute_tc_reg_stats Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_cov

#${L0}/globcov.c 
${L0}/globcov2.cxx

${L0}/trend.c
${L0}/t4_atlas.c
${L0}/count_tab.c
${L0}/read_data.c
${L0}/dim_param2.cxx

#${L0}/region.cxx
#${L0}/region2.cxx
${L0}/region3.cxx

${L0}/map_disk.c

#${L0}/find_regions_by_file_cover.c
#${L0}/find_regions_by_file_cover2.c
${L0}/find_regions_by_file_cover3.cxx

${L0}/d2double.c
${L0}/d2float.c
${L0}/d2int.c
${L0}/filetype.c
${L0}/header.cxx
${L0}/stack.cxx
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/check_dimensions.cxx

#${L0}/lut.cxx
${L0}/lut2.cxx

${L0}/fidl_cov.cxx)
TARGET_LINK_LIBRARIES(fidl_cov Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_crop
${L0}/dim_param2.cxx
${L0}/d2float.c
${L0}/filetype.c
${L0}/d2int.c
${L0}/header.cxx
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/stack.cxx
${L0}/subs_nifti.cxx
${L0}/nifti_flipslice.cxx
${L0}/fidl_crop.cxx)
TARGET_LINK_LIBRARIES(fidl_crop Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(compute_boynton
${L0}/compute_boynton.c 
${L0}/boynton_model.c)
TARGET_LINK_LIBRARIES(compute_boynton m Fidl)

ADD_EXECUTABLE(compute_region_clusters
${L0}/compute_region_clusters.c
${L0}/assign_region_names2.c
${L0}/read_tc_string_new.c)
TARGET_LINK_LIBRARIES(compute_region_clusters m Fidl Gsl)

ADD_EXECUTABLE(compute_voxel_value
${L0}/compute_voxel_value.c 
${L0}/read_af3d.c)
TARGET_LINK_LIBRARIES(compute_voxel_value m Fidl)

ADD_EXECUTABLE(fidl_flipreg
${L0}/fidl_flipreg.c
${L0}/assign_region_names2.c
${L0}/get_atlas_coor.c)
TARGET_LINK_LIBRARIES(fidl_flipreg m Fidl Gsl Gslcblas)

ADD_EXECUTABLE(atlas_transform
${L0}/atlas_transform.c
${L0}/t4_atlas.c)
TARGET_LINK_LIBRARIES(atlas_transform m Fidl Gsl Gslcblas)

ADD_EXECUTABLE(compute_region_stats
${L0}/compute_region_stats.c
${L0}/center_of_mass2.c
${L0}/get_atlas_coor.c)
TARGET_LINK_LIBRARIES(compute_region_stats m Fidl Gsl Gslcblas)

ADD_EXECUTABLE(fidl_region_stats
${L0}/center_of_mass2.c
${L0}/get_atlas_coor.c
${L0}/header.cxx
${L0}/stack.cxx
${L0}/nifti_gethdr.cxx
${L0}/subs_nifti.cxx
${L0}/filetype.c
${L0}/nifti_flipslice.cxx
${L0}/write1.c
${L0}/subs_cifti.cxx
${L0}/check_dimensions.cxx
${L0}/region2.cxx
${L0}/lut.cxx
${L0}/find_regions_by_file_cover.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/fidl_region_stats.cxx)
TARGET_LINK_LIBRARIES(fidl_region_stats m Fidl Cifti Nifti Common Gsl ${LIBS})

ADD_EXECUTABLE(fidl_motionreg
${L0}/d2double.c
${L0}/read_data.c
${L0}/count_tab.c
${L0}/gsl_svd.c
${L0}/cond_norm1.c
${L0}/fidl_motionreg.c)
TARGET_LINK_LIBRARIES(fidl_motionreg m Fidl Gsl Gslcblas)

ADD_EXECUTABLE(fidl_split
${L0}/dim_param2.cxx
${L0}/filetype.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/header.cxx
${L0}/subs_glm.c
${L0}/subs_nifti.cxx
${L0}/nifti_gethdr.cxx
${L0}/d2float.c
${L0}/d2double.c
${L0}/get_atlas_coor.c
${L0}/stack.cxx
${L0}/nifti_flipslice.cxx
${L0}/d2int.c
${L0}/getxyz.c
${L0}/nifti_getmni.cxx
${L0}/fidl_split.cxx)
TARGET_LINK_LIBRARIES(fidl_split m Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_split2

#START210517
#${L0}/files_struct.c

${L0}/coor.cxx
${L0}/filetype.c

#START210427
#${L0}/utilities.c

${L0}/subs_util.c
${L0}/mask.cxx
${L0}/get_atlas_coor.c
${L0}/get_atlas.c
${L0}/getxyz.c
${L0}/nifti_getmni.cxx
${L0}/stack.cxx
${L0}/header.cxx
${L0}/subs_glm.c
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/d2float.c
${L0}/d2double.c
${L0}/lut.cxx
${L0}/fidl_split2.cxx)
TARGET_LINK_LIBRARIES(fidl_split2 Fidl Cifti Nifti Common ${LIBS})


ADD_EXECUTABLE(fidl_wmparc
${L0}/filetype.c
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/lut.cxx
${L0}/fidl_wmparc.cxx)
TARGET_LINK_LIBRARIES(fidl_wmparc Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_wmparc2
${L0}/filetype.c
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/lut2.cxx
${L0}/fidl_wmparc2.cxx)
TARGET_LINK_LIBRARIES(fidl_wmparc2 Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_wmparc_checkval
${L0}/filetype.c
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/lut.cxx
${L0}/fidl_wmparc_checkval.cxx)
TARGET_LINK_LIBRARIES(fidl_wmparc_checkval Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_extractlabels
${L0}/filetype.c
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/fidl_extractlabels.cxx)
TARGET_LINK_LIBRARIES(fidl_extractlabels Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_dice
${L0}/filetype.c
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/lut.cxx
${L0}/fidl_dice.cxx)
TARGET_LINK_LIBRARIES(fidl_dice Fidl Cifti Nifti Common ${LIBS})


ADD_EXECUTABLE(fidl_vstat
#${L0}/fidl_vstat.cxx)
/home/usr/mcavoy/GITHUB/tom/fidl_vstat.cxx)

ADD_EXECUTABLE(fidl_masknorm
${L0}/filetype.c
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/dim_param2.cxx
${L0}/d2float.c
${L0}/d2int.c
${L0}/d2double.c
${L0}/mask.cxx
${L0}/subs_glm.c
${L0}/get_grand_mean_struct.c
${L0}/check_dimensions.cxx
${L0}/movement.cxx
${L0}/fidl_masknorm.cxx)
TARGET_LINK_LIBRARIES(fidl_masknorm Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_lFCD
${L0}/filetype.c
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/dim_param2.cxx
${L0}/d2float.c
${L0}/d2int.c
${L0}/d2double.c
${L0}/mask.cxx
${L0}/subs_glm.c
${L0}/spatialextent.cxx
${L0}/movement.cxx
${L0}/lfcd.cxx
${L0}/fidl_lFCD.cxx)
TARGET_LINK_LIBRARIES(fidl_lFCD Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_lfcd2
${L0}/filetype.c
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/dim_param2.cxx
${L0}/d2float.c
${L0}/d2int.c
${L0}/d2double.c
${L0}/mask.cxx
${L0}/subs_glm.c
${L0}/spatialextent2.cxx
${L0}/movement.cxx
${L0}/lfcd2.cxx
${L0}/fidl_lfcd2.cxx)
TARGET_LINK_LIBRARIES(fidl_lfcd2 Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_lfcd3
${L0}/filetype.c
${L0}/nifti_gethdr.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/dim_param2.cxx
${L0}/d2float.c
${L0}/d2int.c
${L0}/d2double.c
${L0}/mask.cxx
${L0}/subs_glm.c
${L0}/spatialextent2.cxx
${L0}/movement.cxx
${L0}/lfcd3.cxx
${L0}/fidl_lfcd2.cxx)
TARGET_LINK_LIBRARIES(fidl_lfcd3 Fidl Cifti Nifti Common ${LIBS})


ADD_EXECUTABLE(fidl_threshclus
${L0}/fidl_threshclus.c
${L0}/spatial_extent2.c
${L0}/center_of_mass2.c
${L0}/assign_region_names2.c
${L0}/get_atlas_coor.c)
TARGET_LINK_LIBRARIES(fidl_threshclus m Fidl)

ADD_EXECUTABLE(fidl_combinations
${L0}/fidl_combinations.c)
TARGET_LINK_LIBRARIES(fidl_combinations m Fidl Gsl)

ADD_EXECUTABLE(fidl_p_to_z
${L0}/fidl_p_to_z.c)
TARGET_LINK_LIBRARIES(fidl_p_to_z m Gsl)

ADD_EXECUTABLE(fidl_and
${L0}/fidl_and.c
${L0}/dim_param2.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx)
TARGET_LINK_LIBRARIES(fidl_and Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_mask
${L0}/filetype.c
${L0}/dim_param2.c
${L0}/d2float.c
${L0}/d2double.c
${L0}/subs_glm.c
${L0}/subs_nifti.cxx
${L0}/nifti_gethdr.cxx
${L0}/header.cxx
${L0}/mask.cxx
${L0}/nifti_flipslice.cxx
${L0}/stack.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/subs_cifti.cxx
${L0}/write1.c
${L0}/spatial_extent2.c
${L0}/extract_regions.c
${L0}/fidl_mask.cxx)
TARGET_LINK_LIBRARIES(fidl_mask Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_avg
${L0}/fidl_avg.c
${L0}/dim_param2.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx)
TARGET_LINK_LIBRARIES(fidl_avg Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_logic
${L0}/fidl_logic.c)
TARGET_LINK_LIBRARIES(fidl_logic m Fidl)

ADD_EXECUTABLE(make_save
${L0}/make_save.c)
TARGET_LINK_LIBRARIES(make_save ${LIBS})

ADD_EXECUTABLE(fidl_tc_ss
${L0}/d2double.c
${L0}/d2float.c
${L0}/read_frames_file.c
${L0}/dim_param2.cxx
${L0}/get_atlas_coor.c
${L0}/t4_atlas.c
${L0}/read_tc_string_new.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/mask.cxx
${L0}/stack.cxx
${L0}/header.cxx
${L0}/subs_glm.c
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/check_dimensions.cxx
${L0}/region4.cxx
${L0}/filetype.c
${L0}/d2int.c
${L0}/find_regions_by_file_cover3.cxx
${L0}/map_disk.c
${L0}/write1.c
${L0}/ptr.cxx
${L0}/lut2.cxx
${L0}/timestr.cxx
${L0}/ptrw.cxx
${L0}/fidl_tc_ss.cxx)
TARGET_LINK_LIBRARIES(fidl_tc_ss Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_tc_ss_cull
${L0}/cifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/filetype.c
${L0}/region2.cxx
${L0}/lut.cxx
${L0}/fidl_tc_ss_cull.cxx)
TARGET_LINK_LIBRARIES(fidl_tc_ss_cull Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_block
${L0}/d2double.c
${L0}/fidl_block.cxx)
TARGET_LINK_LIBRARIES(fidl_block Fidl ${LIBS})

ADD_EXECUTABLE(fidl_scrub_frames
${L0}/fidl_scrub_frames.cxx)
TARGET_LINK_LIBRARIES(fidl_scrub_frames Fidl ${LIBS})


ADD_EXECUTABLE(fidl_rms
${L0}/dim_param2.cxx
${L0}/d2double.c
${L0}/d2float.c
${L0}/d2int.c
${L0}/nifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/mask.cxx
${L0}/stack.cxx
${L0}/header.cxx
${L0}/subs_glm.c
${L0}/filetype.c
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/cifti_getDimensions.cxx
#${L0}/map_disk.c
${L0}/write1.c
#${L0}/ptr.cxx
${L0}/movement.cxx
${L0}/fidl_rms.cxx)
TARGET_LINK_LIBRARIES(fidl_rms Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_subtract
${L0}/d2double.c
${L0}/d2float.c
#${L0}/d2int.c
${L0}/nifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_flipslice.cxx
${L0}/mask.cxx
${L0}/stack.cxx
${L0}/header.cxx
${L0}/subs_glm.c
${L0}/filetype.c
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/cifti_getDimensions.cxx
${L0}/write1.c
${L0}/fidl_subtract.cxx)
TARGET_LINK_LIBRARIES(fidl_subtract Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_hdr
${L0}/cifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/header.cxx
${L0}/filetype.c
${L0}/stack.cxx
${L0}/nifti_flipslice.cxx
${L0}/fidl_hdr.cxx)
TARGET_LINK_LIBRARIES(fidl_hdr Fidl Cifti Nifti Common ${LIBS})


ADD_EXECUTABLE(fidl_assemble
${L0}/dim_param2.c
${L0}/write1.c
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/fidl_assemble.c)
TARGET_LINK_LIBRARIES(fidl_assemble Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_chunkstack
${L0}/d2double.c
${L0}/dim_param2.c
${L0}/write1.c
${L0}/subs_nifti.cxx
${L0}/subs_cifti.cxx
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/filetype.c
${L0}/get_atlas_coor.c
${L0}/monte_carlo_mult_comp.c
${L0}/spatial_extent2.c
${L0}/center_of_mass2.c
${L0}/assign_region_names2.c
${L0}/fidl_chunkstack.c)
TARGET_LINK_LIBRARIES(fidl_chunkstack Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_checkglm
${L0}/gsl_svd.c
${L0}/cond_norm1.c
${L0}/fidl_checkglm.c)
TARGET_LINK_LIBRARIES(fidl_checkglm m Fidl Gsl Gslcblas)

ADD_EXECUTABLE(fidl_flipglm
${L0}/subs_mask.c
${L0}/get_atlas_param.c
${L0}/get_atlas_coor.c
${L0}/atlas_to_index.c
${L0}/write_glm.c
${L0}/fidl_flipglm.c)
TARGET_LINK_LIBRARIES(fidl_flipglm m Fidl)

ADD_EXECUTABLE(fidl_fdr
${L0}/read_data.c
${L0}/count_tab.c
${L0}/dim_param2.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/spatial_extent2.c
${L0}/subs_cifti.cxx
${L0}/subs_nifti.cxx
${L0}/write1.c
${L0}/fdr_guts.c
${L0}/center_of_mass2.c
${L0}/assign_region_names2.c
${L0}/get_atlas_coor.c
${L0}/fidl_fdr.c)
TARGET_LINK_LIBRARIES(fidl_fdr Fidl Cifti Nifti Common Gsl ${LIBS})

ADD_EXECUTABLE(fidl_rename_effects
${L0}/d2double.c
${L0}/read_tc_string_new.c
${L0}/write_glm.c
${L0}/fidl_rename_effects.c)
TARGET_LINK_LIBRARIES(fidl_rename_effects m Fidl)

ADD_EXECUTABLE(fidl_conc
${L0}/fidl_conc.c)
TARGET_LINK_LIBRARIES(fidl_conc m Fidl)

ADD_EXECUTABLE(fidl_logreg
${L0}/read_data.c
${L0}/x2_to_z.c
${L0}/d2double.c
${L0}/count_tab.c
${L0}/find_regions_by_file_cover.c
${L0}/header.cxx
${L0}/subs_glm.c
${L0}/subs_nifti.cxx
${L0}/filetype.c
${L0}/nifti_gethdr.cxx
${L0}/d2float.c
${L0}/map_disk.c
${L0}/read_tc_string_new.c
${L0}/read_frames_file.c
${L0}/dim_param2.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/mask.cxx
${L0}/write1.c
${L0}/subs_cifti.cxx
${L0}/get_limits.c
${L0}/cond_norm1.c
${L0}/gsl_svd.c
${L0}/check_dimensions.cxx

#${L0}/region.cxx
${L0}/region2.cxx

${L0}/logreg.c
${L0}/fidl_logreg.cxx)
TARGET_LINK_LIBRARIES(fidl_logreg m Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_logreg_montecarlo
${L0}/count_tab.c
${L0}/d2double.c
${L0}/d2float.c
${L0}/subs_glm.c
${L0}/nifti_gethdr.cxx
${L0}/filetype.c
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/mask.cxx
${L0}/read_data.c
${L0}/logreg.c
${L0}/map_disk.c
${L0}/dim_param2.c
${L0}/write1.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/subs_cifti.cxx
${L0}/x2_to_z.c
${L0}/spatial_extent2.c
${L0}/fidl_logreg_montecarlo.cxx)
TARGET_LINK_LIBRARIES(fidl_logreg_montecarlo m Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_logreg_fdr
${L0}/count_tab.c
${L0}/d2double.c
${L0}/d2float.c
${L0}/subs_glm.c
${L0}/nifti_gethdr.cxx
${L0}/filetype.c
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/mask.cxx
${L0}/read_data.c
${L0}/logreg.c
${L0}/map_disk.c
${L0}/dim_param2.c
${L0}/write1.c
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/subs_cifti.cxx
${L0}/x2_to_z.c
${L0}/stack.cxx
${L0}/nifti_flipslice.cxx
${L0}/nifti_getmni.cxx
${L0}/spatial_extent2.c
${L0}/fidl_logreg_fdr.cxx)
TARGET_LINK_LIBRARIES(fidl_logreg_fdr m Fidl Cifti Nifti Common Gsl ${LIBS})

ADD_EXECUTABLE(fidl_logreg_collinear
${L0}/count_tab.c
${L0}/d2double.c
${L0}/read_data.c
${L0}/cond_norm1.c
${L0}/gsl_svd.c
${L0}/fidl_logreg_collinear.cxx)
TARGET_LINK_LIBRARIES(fidl_logreg_collinear m Fidl Cifti Nifti Common Gsl Gslcblas ${LIBS})

ADD_EXECUTABLE(fidl_lesion
${L0}/cifti_getDimensions.cxx
${L0}/nifti_getDimensions.cxx
${L0}/d2double.c
${L0}/d2float.c
${L0}/subs_glm.c
${L0}/nifti_flipslice.cxx
${L0}/nifti_gethdr.cxx
${L0}/filetype.c
${L0}/subs_nifti.cxx
${L0}/header.cxx
${L0}/stack.cxx
${L0}/mask.cxx
${L0}/dim_param2.c
${L0}/spatial_extent2.c
${L0}/fidl_lesion.cxx)
TARGET_LINK_LIBRARIES(fidl_lesion Fidl Cifti Nifti Common ${LIBS})

ADD_EXECUTABLE(fidl_fix_region_file
${L0}/fidl_fix_region_file.c)
TARGET_LINK_LIBRARIES(fidl_fix_region_file m Fidl)

ADD_EXECUTABLE(fidl_chisquare_to_z
${L0}/x2_to_z.c
${L0}/fidl_chisquare_to_z.c)
TARGET_LINK_LIBRARIES(fidl_chisquare_to_z m Fidl)

ADD_EXECUTABLE(fidl_copy_contrasts
${L0}/subs_glm.c
${L0}/d2float.c
${L0}/d2double.c
${L0}/write_glm.c
${L0}/fidl_copy_contrasts.c)
TARGET_LINK_LIBRARIES(fidl_copy_contrasts m Fidl)

ADD_EXECUTABLE(fidl_Events82
${L0}/read_data.c
${L0}/d2double.c
${L0}/count_tab.c
${L0}/fidl_Events82.c)
TARGET_LINK_LIBRARIES(fidl_Events82 m Fidl)

ADD_EXECUTABLE(fidl_slicetimertxt
${L0}/bids.cxx
${L0}/fidl_slicetimertxt.cxx)
TARGET_LINK_LIBRARIES(fidl_slicetimertxt m Fidl)

ADD_LIBRARY(fidlcifti SHARED
${L0}/cifti_getDimensions.cxx
${L0}/nifti_gethdr.cxx
${L0}/nifti_getslice.cxx
${L0}/nifti_flipslice.cxx
${L0}/nifti_getmni.cxx
${L0}/nifti_getmni2.cxx
${L0}/nifti_getorient.cxx
${L0}/cifti_getColumn.cxx)
TARGET_LINK_LIBRARIES(fidlcifti cifti nifti common ${LIBS})

#ADD_EXECUTABLE(mpm_str_path
#/home/hannah/mcavoy/GITHUB/c/mpm_str_path.cxx)
#TARGET_LINK_LIBRARIES(mpm_str_path Fidl)
#ADD_EXECUTABLE(mpm_str_root
#/home/hannah/mcavoy/GITHUB/c/mpm_str_root.cxx)
#TARGET_LINK_LIBRARIES(mpm_str_root Fidl)

INCLUDE_DIRECTORIES(
/home/usr/mcavoy/FIDL/cifti/CiftiLib-master/Cifti
/home/usr/mcavoy/FIDL/cifti/CiftiLib-master/Nifti
/home/usr/mcavoy/FIDL/cifti/CiftiLib-master/Common
/home/usr/mcavoy/FIDL/gsl1.16/include
${CMAKE_SOURCE_DIR}/fidl
)
