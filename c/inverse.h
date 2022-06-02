/* Copyright 1/25/21 Washington University.  All Rights Reserved.
   inverse.h  $Revision: 1.1 $ 
   From: Numerical recipes in C*/

#ifndef __INVERSE_H__
    #define __INVERSE_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    int inverse(double **A,int N,double **Ainv);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
