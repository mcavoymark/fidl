/* Copyright 11/19/18 Washington University.  All Rights Reserved.
   ellipsestack.h  $Revision: 1.1 $*/

#ifndef __ELLIPSESTACK_H__
    #define __ELLIPSESTACK_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    int ellipsestack(int N,int *x,int *y,int *z,float *stack,float *value,int xdim,int ydim,int zdim,float *xdia,float *ydia,
        float *zdia,float xvoxelsize,float yvoxelsize,float zvoxelsize);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
