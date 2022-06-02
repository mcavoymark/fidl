/* Copyright 3/24/15 Washington University.  All Rights Reserved.
   dmatrix_mult.h  $Revision: 1.2 $ */
#ifndef __DMATRIX_MULT_H__
    #define __DMATRIX_MULT_H__

#define TRANSPOSE_NONE 0
#define TRANSPOSE_FIRST 1
#define TRANSPOSE_SECOND 2

    #ifdef __cplusplus
        extern "C" {
    #endif

double **dmatrix_mult(double **A,double **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose);
double **dmatrix_mult_ff(float **A,float **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose);
double **dmatrix_mult_fd(float **A,double **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose);
double **dmatrix_mult_df(double **A,float **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose);
double **dmatrix_mult_allocate(int NrowA,int NcolA,int NrowB,int NcolB,int Transpose);
void dmatrix_mult_nomem(double **A,double **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose,double **C);
void dmatrix_mult_nomem_ff(float **A,float **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose,double **C);
void dmatrix_mult_nomem_fd(float **A,double **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose,double **C);
void dmatrix_mult_nomem_df(double **A,float **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose,double **C);

    #ifdef __cplusplus
        }//extern
    #endif
#endif

