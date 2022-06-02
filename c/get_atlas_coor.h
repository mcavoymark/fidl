/* Copyright 3/24/15 Washington University.  All Rights Reserved.
   get_atlas_coor.h  $Revision: 1.3 $*/
#ifndef __GET_ATLAS_COOR_H__
    #define __GET_ATLAS_COOR_H__
    
    #include "get_atlas_param.h"

    #ifdef __cplusplus
        extern "C" {
    #endif

    void print_atlas_coor(int index,Atlas_Param *ap);
    void get_atlas_coor(int n,double *col,double *row,double *slice,double zdim,double *center,double *mmppix,double *atlas_coor);

    //START190124
    void get_atlas_coorf(int n,double *col,double *row,double *slice,double zdim,double *center,double *mmppix,float *atlas_coor);

    //START190508
    void get_atlas_coorff(int n,double *col,double *row,double *slice,double zdim,float *center,float *mmppix,float *atlas_coor);

    void col_row_slice(int n,int *index,double *dcol,double *drow,double *dslice,Atlas_Param *ap);

    //START190128
    void col_row_slice2(int n,int *index,double *dcol,double *drow,double *dslice,int xdim,int ydim);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
