/* Copyright 3/24/17 Washington University.  All Rights Reserved.
   nifti_flipslice.h  $Revision: 1.2 $*/
#ifndef __NIFTI_FLIPSLICE_H__
    #define __NIFTI_FLIPSLICE_H__
    #ifdef __cplusplus
        extern "C" {
    #endif
    int nifti_flipslice(int64_t xdim,int64_t ydim,int cf_flip,float* stack);

    //START190926
    int nifti_flipsliceus(int64_t xdim,int64_t ydim,int cf_flip,unsigned short* stack);

    //START201008
    int nifti_flipslicesi(int64_t xdim,int64_t ydim,int cf_flip,int* stack);

    int _nifti_flipslice(int argc,char **argv); 
    #ifdef __cplusplus
        }//extern
    #endif
#endif
