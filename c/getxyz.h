/* Copyright 1/24/19 Washington University.  All Rights Reserved.
   getxyz.h  $Revision: 1.1 $*/

#ifndef __GETXYZ_H__
    #define __GETXYZ_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    //void getxyz(int n,double *col,double *row,double *slice,int cf_flip,int ydim,float *xyz);
    //START190125
    void getxyz(int n,double *col,double *row,double *slice,int cf_flip,int ydim,float *xyz,char *atlas);

    #ifdef __cplusplus
        }//extern
    #endif

#endif
