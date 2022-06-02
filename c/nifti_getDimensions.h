/* Copyright 7/21/15 Washington University.  All Rights Reserved.
   nifti_getDimensions.h  $Revision: 1.2 $*/
#ifndef __NIFTI_GETDIMENSIONS_H__
    #define __NIFTI_GETDIMENSIONS_H__
    #ifdef __cplusplus
        extern "C" {
    #endif
    int nifti_getDimensions(char *file,int64_t *xdim,int64_t *ydim,int64_t *zdim,int64_t *tdim);
    int _nifti_getDimensions(int argc,char **argv);
    #ifdef __cplusplus
        }//extern
    #endif
#endif

