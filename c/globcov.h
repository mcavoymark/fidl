/* Copyright 2/21/19 Washington University.  All Rights Reserved.
   globcov.h  $Revision: 1.1 $ */

#ifndef __GLOBCOV_H__
    #define __GLOBCOV_H__

    #include "files_struct.h"
    #include "dim_param2.h"
    #include "find_regions_by_file_cover.h"
    #include "get_atlas_param.h"

    #ifdef __cplusplus
        extern "C" {
    #endif

    #if 0
    int globcov_guts(Files_Struct *fi,Dim_Param2 *dp,int *valid_frms,double *globcov,int SunOS_Linux,int *scalingv,
        Regions_By_File *rbf,Files_Struct *xform_file,int *t4_identify,Atlas_Param *ap,double *weights,int *weightsi,int weightsc,
        int lccompressed,int *tdim);
    #endif
    //START190909
    int globcov_guts(Files_Struct *fi,Dim_Param2 *dp,int *valid_frms,double *globcov,int *scalingv,
        Regions_By_File *rbf,Files_Struct *xform_file,int *t4_identify,Atlas_Param *ap,double *weights,int *weightsi,int weightsc,
        int lccompressed,int *tdim);


    #ifdef __cplusplus
        }//extern
    #endif
#endif

