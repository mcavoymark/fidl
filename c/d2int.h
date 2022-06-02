/* Copyright 1/10/19 Washington University.  All Rights Reserved.
   d2int.h  $Revision: 1.1 $ */
#ifndef __D2INT_H__
    #define __D2INT_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

    int **d2int(int dim1,int dim2);
    void free_d2int(int **array);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
