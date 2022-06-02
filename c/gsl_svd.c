/* Copyright 8/27/10 Washington University.  All Rights Reserved.
   gsl_svd.c  $Revision: 1.15 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include <float.h>
#include "gsl_svd.h"
int _gsl_svd(int argc,char **argv){
    double *A,*Ainv,*V,*S,*work;
    int i;
    float *Ain = (float*)argv[0];
    #ifdef __sun__
        int m = (int)argv[1];
        int n = (int)argv[2];
    #else
        int m = (intptr_t)argv[1];
        int n = (intptr_t)argv[2];
    #endif
    double *cond = (double*)argv[3];
    int *ndep = (int*)argv[4];
    if(!(A=malloc(sizeof*A*m*n))) {
        printf("Error: Unable to malloc A\n");
        return 0;
        }
    if(!(Ainv=malloc(sizeof*Ainv*m*n))) {
        printf("Error: Unable to malloc Ainv\n");
        return 0;
        }
    for(i=0;i<(m*n);i++) A[i] = (double)Ain[i];
    if(!(V=malloc(sizeof*V*n*n))) {
        printf("Error: Unable to malloc V\n");
        return 0;
        }
    if(!(S=malloc(sizeof*S*n))) {
        printf("Error: Unable to malloc S\n");
        return 0;
        }
    if(!(work=malloc(sizeof*work*n))) {
        printf("Error: Unable to malloc work\n");
        return 0;
        }
    gsl_svd_golubreinsch(A,m,n,0.,V,S,Ainv,cond,ndep,work);
    for(i=0;i<(m*n);i++) Ain[i] = (float)Ainv[i];
    free(work);free(S);free(V);free(Ainv);free(A);
    return 1;
    }
double gsl_svd_golubreinsch(double *A,int m,int n,double tol,double *V,double *S,double *Ainv,double *cond_norm2,int *ndep,
    double *work){
    int i,j,k;
    gsl_matrix_view gA = gsl_matrix_view_array(A,m,n);
    gsl_matrix_view gV = gsl_matrix_view_array(V,n,n);
    gsl_vector_view gS = gsl_vector_view_array(S,n);
    gsl_vector_view gwork = gsl_vector_view_array(work,n);
    gsl_linalg_SV_decomp(&gA.matrix,&gV.matrix,&gS.vector,&gwork.vector);
    if(tol<=0.) tol = (double)(m>n?m:n)*S[0]*(double)DBL_EPSILON;
    /*printf("gsl_svd_golubreinsch S[0]=%g tol=%g\n",S[0],tol);*/
    *cond_norm2 = S[0]/S[n-1];
    for(*ndep=0,i=1;i<n;i++) if((S[i]/S[0])<tol) (*ndep)++;
    if(Ainv) {
        gsl_matrix_view gAinv = gsl_matrix_view_array(Ainv,m,n);
        for(i=0;i<n;i++) S[i] = S[i]<tol ? 0. : 1./S[i];
        for(k=i=0;i<n;i++) for(j=0;j<n;j++,k++) V[k] *= S[j];
        gsl_blas_dgemm(CblasNoTrans,CblasTrans,1.0,&gV.matrix,&gA.matrix,0.0,&gAinv.matrix);
        }
    return tol; 
    }
double gsl_svd_jacobi(double *A,int m,int n,double tol,double *V,double *S,double *Ainv,double *cond_norm2,int *ndep){
    int i,j,k;
    gsl_matrix_view gA = gsl_matrix_view_array(A,m,n);
    gsl_matrix_view gV = gsl_matrix_view_array(V,n,n);
    gsl_vector_view gS = gsl_vector_view_array(S,n);
    gsl_linalg_SV_decomp_jacobi(&gA.matrix,&gV.matrix,&gS.vector);
    if(tol<=0.) tol = (double)(m>n?m:n)*S[0]*(double)DBL_EPSILON;
    *cond_norm2 = S[0]/S[n-1];
    for(*ndep=0,i=1;i<n;i++) if((S[i]/S[0])<tol) (*ndep)++;
    if(Ainv) {
        gsl_matrix_view gAinv = gsl_matrix_view_array(Ainv,m,n);
        for(i=0;i<n;i++) S[i] = S[i]<tol ? 0. : 1./S[i];
        for(k=i=0;i<n;i++) for(j=0;j<n;j++,k++) V[k] *= S[j];
        gsl_blas_dgemm(CblasNoTrans,CblasTrans,1.0,&gV.matrix,&gA.matrix,0.0,&gAinv.matrix);
        }
    return tol;
    }

//START170228
double gsl_svd_modgolubreinsch(double *A,int m,int n,double tol,double *V,double *S,double *Ainv,double *cond_norm2,int *ndep,
    double *work,double *X){
    int i,j,k;
    gsl_matrix_view gA = gsl_matrix_view_array(A,m,n);
    gsl_matrix_view gV = gsl_matrix_view_array(V,n,n);
    gsl_vector_view gS = gsl_vector_view_array(S,n);
    gsl_vector_view gwork = gsl_vector_view_array(work,n);
    gsl_matrix_view gX = gsl_matrix_view_array(X,n,n);
    gsl_linalg_SV_decomp_mod(&gA.matrix,&gX.matrix,&gV.matrix,&gS.vector,&gwork.vector);
    if(tol<=0.) tol = (double)(m>n?m:n)*S[0]*(double)DBL_EPSILON;
    *cond_norm2 = S[0]/S[n-1];
    for(*ndep=0,i=1;i<n;i++) if((S[i]/S[0])<tol) (*ndep)++;
    if(Ainv) {
        gsl_matrix_view gAinv = gsl_matrix_view_array(Ainv,m,n);
        for(i=0;i<n;i++) S[i] = S[i]<tol ? 0. : 1./S[i];
        for(k=i=0;i<n;i++) for(j=0;j<n;j++,k++) V[k] *= S[j];
        gsl_blas_dgemm(CblasNoTrans,CblasTrans,1.0,&gV.matrix,&gA.matrix,0.0,&gAinv.matrix);
        }
    return tol;
    }
