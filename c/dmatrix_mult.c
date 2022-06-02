/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   dmatrix_mult.c  $Revision: 12.85 $ */

/*********************************************************
Purpose: Double precision multiply and NxK matrix by a KxM matrix, i.e.
        Result = AB where A is NxK and B is KxM when Transpose=0, and
        Result = transpose(A)B where A is NxK and B is KxM when Transpose=1

Convention: Matrices are assumed to follow the numerical recipes
            conventions for indices, i.e., starting at one.
**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if 0
#include <fidl.h>
#include <nrutil.h>
#endif
/*START150324*/
#include "nrutil.h"
#include "dmatrix_mult.h"

double **dmatrix_mult(double **A,double **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose)
{
    double  **C;
    if(C=dmatrix_mult_allocate(NrowA,NcolA,NrowB,NcolB,Transpose)) {
        dmatrix_mult_nomem(A,B,NrowA,NcolA,NrowB,NcolB,Transpose,C);
        }
    return C;
}
double **dmatrix_mult_ff(float **A,float **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose)
{
    double  **C;
    if(C=dmatrix_mult_allocate(NrowA,NcolA,NrowB,NcolB,Transpose)) {
        dmatrix_mult_nomem_ff(A,B,NrowA,NcolA,NrowB,NcolB,Transpose,C);
        }
    return C;
}
double **dmatrix_mult_fd(float **A,double **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose)
{
    double  **C;
    if(C=dmatrix_mult_allocate(NrowA,NcolA,NrowB,NcolB,Transpose)) {
        dmatrix_mult_nomem_fd(A,B,NrowA,NcolA,NrowB,NcolB,Transpose,C);
        }
    return C;
}
double **dmatrix_mult_df(double **A,float **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose)
{
    double  **C;
    if(C=dmatrix_mult_allocate(NrowA,NcolA,NrowB,NcolB,Transpose)) {
        dmatrix_mult_nomem_df(A,B,NrowA,NcolA,NrowB,NcolB,Transpose,C);
        }
    return C;
}
double **dmatrix_mult_allocate(int NrowA,int NcolA,int NrowB,int NcolB,int Transpose)
{
    double  **C=NULL;
    int dim;
    switch(Transpose) {
        case TRANSPOSE_NONE:
            dim = NcolB;
            break;
        case TRANSPOSE_FIRST:
            dim = NcolB;
            break;
        case TRANSPOSE_SECOND:
            dim = NrowB;
            break;
        default:
            printf("Error: Invalid transpose parameter in dmatrix_mult.\n");
        }
    C = dmatrix(1,NrowA,1,dim);
    return C;
}
void dmatrix_mult_nomem(double **A,double **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose,double **C)
{
int     i,j,k;
switch(Transpose) {
    case TRANSPOSE_NONE:
        for(i=1;i<=NrowA;i++) {
            for(j=1;j<=NcolB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NcolA;k++) C[i][j] += A[i][k]*B[k][j];
                }
            }
        break;
    case TRANSPOSE_FIRST:
        for(i=1;i<=NcolA;i++) {
            for(j=1;j<=NcolB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NrowA;k++) C[i][j] += A[k][i]*B[k][j];
                }
            }
        break;
    case TRANSPOSE_SECOND:
        for(i=1;i<=NrowA;i++) {
            for(j=1;j<=NrowB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NcolB;k++) C[i][j] += A[i][k]*B[j][k];
                }
            }
        break;
    default:
        fprintf(stderr,"Invalid transpose parameter in dmatrix_mult.\n");
    }
}

void dmatrix_mult_nomem_ff(float **A,float **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose,double **C)
{
int     i,j,k;
switch(Transpose) {
    case TRANSPOSE_NONE:
        for(i=1;i<=NrowA;i++) {
            for(j=1;j<=NcolB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NcolA;k++) C[i][j] += (double)A[i][k]*(double)B[k][j];
                }
            }
        break;
    case TRANSPOSE_FIRST:
        for(i=1;i<=NcolA;i++) {
            for(j=1;j<=NcolB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NrowA;k++) C[i][j] += (double)A[k][i]*(double)B[k][j];
                }
            }
        break;
    case TRANSPOSE_SECOND:
        for(i=1;i<=NrowA;i++) {
            for(j=1;j<=NrowB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NcolB;k++) C[i][j] += (double)A[i][k]*(double)B[j][k];
                }
            }
        break;
    default:
        fprintf(stderr,"Invalid transpose parameter in dmatrix_mult.\n");
    }
}

void dmatrix_mult_nomem_fd(float **A,double **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose,double **C)
{
int     i,j,k;
switch(Transpose) {
    case TRANSPOSE_NONE:
        for(i=1;i<=NrowA;i++) {
            for(j=1;j<=NcolB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NcolA;k++) C[i][j] += (double)A[i][k]*B[k][j];
                }
            }
        break;
    case TRANSPOSE_FIRST:
        for(i=1;i<=NcolA;i++) {
            for(j=1;j<=NcolB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NrowA;k++) C[i][j] += (double)A[k][i]*B[k][j];
                }
            }
        break;
    case TRANSPOSE_SECOND:
        for(i=1;i<=NrowA;i++) {
            for(j=1;j<=NrowB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NcolB;k++) C[i][j] += (double)A[i][k]*B[j][k];
                }
            }
        break;
    default:
        fprintf(stderr,"Invalid transpose parameter in dmatrix_mult.\n");
    }
}

void dmatrix_mult_nomem_df(double **A,float **B,int NrowA,int NcolA,int NrowB,int NcolB,int Transpose,double **C)
{
int     i,j,k;
switch(Transpose) {
    case TRANSPOSE_NONE:
        for(i=1;i<=NrowA;i++) {
            for(j=1;j<=NcolB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NcolA;k++) C[i][j] += A[i][k]*(double)B[k][j];
                }
            }
        break;
    case TRANSPOSE_FIRST:
        for(i=1;i<=NcolA;i++) {
            for(j=1;j<=NcolB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NrowA;k++) C[i][j] += A[k][i]*(double)B[k][j];
                }
            }
        break;
    case TRANSPOSE_SECOND:
        for(i=1;i<=NrowA;i++) {
            for(j=1;j<=NrowB;j++) {
                C[i][j] = 0.;
                for(k=1;k<=NcolB;k++) C[i][j] += A[i][k]*(double)B[j][k];
                }
            }
        break;
    default:
        fprintf(stderr,"Invalid transpose parameter in dmatrix_mult.\n");
    }
}
