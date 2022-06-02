/* Copyright 7/16/14 Washington University.  All Rights Reserved.
   Acond_norm1.c  $Revision: 1.9 $ */

//#ifndef __sun__
//    #include <stdint.h>
//#endif
//START150929
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

//START150929
#include "cond_norm1.h"

int _Acond_norm1(int argc,char **argv){
    float *AT = (float*)argv[0];
    #ifdef __sun__
        int m = (int)argv[1];
        int n = (int)argv[2];
    #else
        int m = (intptr_t)argv[1];
        int n = (intptr_t)argv[2];
    #endif
    double *cond = (double*)argv[3];
    int i,status;
    float *ATA;
    double *ATAd,*Ainvd;
    if(!(ATA=malloc(sizeof*ATA*n*n))) {
        printf("fidlError: Unable to malloc ATA n*n=%d\n",n*n);
        return 0;
        }
    if(!(ATAd=malloc(sizeof*ATAd*n*n))) {
        printf("fidlError: Unable to malloc ATAd n*n=%d\n",n*n);
        free(ATA);
        return 0;
        }
    if(!(Ainvd=malloc(sizeof*Ainvd*n*n))) {
        printf("fidlError: Unable to malloc Ainvd\n\n");
        free(ATAd);
        free(ATA);
        return 0;
        }
    gsl_permutation *perm = gsl_permutation_alloc(n);
    gsl_matrix_float_view ATg = gsl_matrix_float_view_array(AT,n,m);
    gsl_matrix_float_view ATAg = gsl_matrix_float_view_array(ATA,n,n);
    gsl_blas_sgemm(CblasNoTrans,CblasTrans,1.,&ATg.matrix,&ATg.matrix,0.,&ATAg.matrix);
    for(i=0;i<n*n;i++) ATAd[i] = (double)ATA[i];

    //status=cond_norm1(ATAd,n,cond,Ainvd,perm)?0:1; 
    //START150929
    status=cond_norm1(ATAd,n,cond,Ainvd,perm); 

    gsl_permutation_free(perm);
    free(Ainvd);
    free(ATAd);
    free(ATA);
    fflush(stdout);
    return status;
    }
