/* Copyright 7/14/17 Washington University.  All Rights Reserved.
   d2float.h  $Revision: 1.1 $ */
#ifndef __D2FLOAT_H__
    #define __D2FLOAT_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    float **d2float(int dim1,int dim2);
    void free_d2float(float **array);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
