/* Copyright 2/20/18 Washington University.  All Rights Reserved.
   nifti_getmni.h  $Revision: 1.1 $*/

#ifndef __NIFTI_GETMNI_H__
    #define __NIFTI_GETMNI_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    int nifti_getmni(char *file,float *indices,float *coor);

    //START190124
    int nifti_getmnis(char *file,float *indices,float *coor,int nindices);
    

    int _nifti_getmni(int argc,char **argv);

    //START170619
    #ifdef __cplusplus
        }//extern
    #endif
#endif
