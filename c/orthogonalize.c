/* Copyright 10/4/10 Washington University. All Rights Reserved.
   orthogonalize.c  $Revision: 1.6 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include <gsl/gsl_linalg.h>
#include "fidl.h"
int _orthogonalize(int argc,char **argv){
    int o,i,j,k;
    double *Q,*R,*tau,*hipass;
    #ifdef __sun__
        int m = (int)argv[0];
        int n = (int)argv[1];
    #else
        int m = (intptr_t)argv[0];
        int n = (intptr_t)argv[1];
    #endif
    double *hipassT = (double*)argv[2];
    if(!(hipass=malloc(sizeof*hipass*m*n))) {
        printf("Error: Unable to malloc hipass\n");
        return 0;
        }
    gsl_matrix_view ghipassT = gsl_matrix_view_array(hipassT,n,m);
    gsl_matrix_view ghipass = gsl_matrix_view_array(hipass,m,n);
    gsl_matrix_transpose_memcpy(&ghipass.matrix,&ghipassT.matrix);

    if(!(Q=malloc(sizeof*Q*m*m))) {
        printf("Error: Unable to malloc Q\n");
        return 0;
        }
    if(!(R=malloc(sizeof*R*m*n))) {
        printf("Error: Unable to malloc R\n");
        return 0;
        }
    o = m<n?m:n;
    if(!(tau=malloc(sizeof*tau*o))) {
        printf("Error: Unable to malloc tau\n");
        return 0;
        }

    printf("here0 m=%d n=%d o=%d\n",m,n,o); fflush(stdout);

    #if 1
    /*for(k=i=0;i<m;i++) {*/
    for(k=i=0;i<1;i++) {
        for(j=0;j<n;j++,k++) printf("%f ",hipass[k]);
        printf("\n");
        /*exit(-1);*/
        }
    #endif

    orthogonalize(m,m,hipass,tau,Q,R);

    printf("here1\n"); fflush(stdout);

    /*for(k=ii=i=0;i<m;i++,ii+=m) for(jj=ii,j=0;j<n;j++,k++,jj++) hipass[k] = Q[jj];*/
    for(k=i=0;i<m;i++) {
        for(j=0;j<m;j++,k++) {
            hipass[k] = Q[i*m+j];
            if(i==0) printf("%f ",Q[i*m+j]);
            }
        if(i==0) printf("\n");
        } 
    gsl_matrix_transpose_memcpy(&ghipassT.matrix,&ghipass.matrix);

    printf("here2\n"); fflush(stdout);

    free(tau); free(R); free(Q);

    printf("here3\n"); fflush(stdout);

    return 1;
    }
void orthogonalize(int m,int n,double *hipass,double *tau,double *Q,double *R){
    gsl_matrix_view ghipass = gsl_matrix_view_array(hipass,m,n);
    gsl_vector_view gtau = gsl_vector_view_array(tau,m<n?m:n);
    gsl_matrix_view gQ = gsl_matrix_view_array(Q,m,m);
    gsl_matrix_view gR = gsl_matrix_view_array(R,m,n);

    printf("here100\n"); fflush(stdout);

    gsl_linalg_QR_decomp(&ghipass.matrix,&gtau.vector);

    printf("here101\n"); fflush(stdout);

    gsl_linalg_QR_unpack(&ghipass.matrix,&gtau.vector,&gQ.matrix,&gR.matrix);

    printf("here102\n"); fflush(stdout);
    } 
