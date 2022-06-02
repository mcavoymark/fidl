/* Copyright 4/20/15 Washington University.  All Rights Reserved.
   AtoATAgsl_svd.c  $Revision: 1.4 $ */

//#ifndef __sun__
//    #include <stdint.h>
//#endif
//START150929
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

//START151005
#include <gsl/gsl_errno.h>
#include "gsl_svd.h"

int _AtoATAgsl_svd(int argc,char **argv){
    float *AT = (float*)argv[0];
    #ifdef __sun__
        int m = (int)argv[1];
        int n = (int)argv[2];
    #else
        int m = (intptr_t)argv[1];
        int n = (intptr_t)argv[2];
    #endif
    double *cond = (double*)argv[3];
    int *ndep = (int*)argv[4];

    //size_t i;
    //START151006
    int i,status;

    float *ATA;
    double *ATAd,*V,*S,*work;
    if(!(ATA=malloc(sizeof*ATA*n*n))) {
        printf("fidlError: Unable to malloc ATA n*n=%d\n",n*n);
        return 0;
        }
    if(!(ATAd=malloc(sizeof*ATAd*n*n))) {
        printf("fidlError: Unable to malloc ATAd n*n=%d\n",n*n);
        free(ATA);
        return 0;
        }
    if(!(V=malloc(sizeof*V*n*n))) {
        printf("fidlError: Unable to malloc V\n");
        free(ATA);
        free(ATAd);
        return 0;
        }
    if(!(S=malloc(sizeof*S*n))) {
        printf("fidlError: Unable to malloc S\n");
        free(ATA);
        free(ATAd);
        free(V);
        return 0;
        }
    if(!(work=malloc(sizeof*work*n))) {
        printf("fidlError: Unable to malloc work\n");
        free(ATA);
        free(ATAd);
        free(V);
        free(S);
        return 0;
        }
    gsl_matrix_float_view ATg = gsl_matrix_float_view_array(AT,n,m);
    gsl_matrix_float_view ATAg = gsl_matrix_float_view_array(ATA,n,n);

    //gsl_blas_sgemm(CblasNoTrans,CblasTrans,1.,&ATg.matrix,&ATg.matrix,0.,&ATAg.matrix);
    //START151005
    gsl_set_error_handler_off();
    if((status=gsl_blas_sgemm(CblasNoTrans,CblasTrans,1.,&ATg.matrix,&ATg.matrix,0.,&ATAg.matrix))){
        printf("fidlInfo: AtoATAgsl_svd status=%d %s\n",status,gsl_strerror(status));
        return status;
        }

    for(i=0;i<n*n;i++) ATAd[i] = (double)ATA[i];
    gsl_svd_golubreinsch(ATAd,n,n,0.,V,S,(double*)NULL,cond,ndep,work);
    free(work);free(S);free(V);free(ATAd);free(ATA);
    return 1;
    }
