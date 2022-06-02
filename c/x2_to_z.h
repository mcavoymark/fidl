/* Copyright 8/09/17 Washington University.  All Rights Reserved.
   x2_to_z.h  $Revision: 1.1 $ */
#ifndef __X2_TO_Z_H__
    #define __X2_TO_Z_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    void x2_to_z(double *x,double *z,int vol,double *df);

    //START180105
    void x2_to_z_asym(double *x,double *z,int vol,double *df);

    #ifdef __cplusplus
        }//extern
    #endif

#endif
