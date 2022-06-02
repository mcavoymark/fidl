/* Copyright 8/8/14 Washington University.  All Rights Reserved.
   Agsl_svd.c  $Revision: 1.5 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include <gsl/gsl_blas.h>

//#include "fidl.h"
//START151005
#include "gsl_svd.h"

int _Agsl_svd(int argc,char **argv){
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

    //size_t i,j,k,j1;
    //START151001
    int i,j;
    size_t j1,k1;

    double *Ad,*V,*S,*work;
    if(!(Ad=malloc(sizeof*Ad*m*n))) {
        printf("fidlError: Unable to malloc Ad m*n=%d\n",m*n);
        return 0;
        }
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

    //for(k=i=0;i<n;i++) for(j1=i,j=0;j<m;j++,j1+=n,k++) Ad[j1] = (double)AT[k];
    //START151005
    for(k1=i=0;i<n;i++)for(j1=i,j=0;j<m;j++,j1+=n,k1++)Ad[j1]=(double)AT[k1];

    #if 0
    printf("_Agsl_svd Ad\n");
    for(k=i=0;i<m;i++) {
        printf("i=%d ",i);
        for(j=0;j<n;j++,k++) printf("%.2f ",Ad[k]);
        printf("\n");
        } 
    printf("\n");
    #endif

    gsl_svd_golubreinsch(Ad,m,n,0.,V,S,(double*)NULL,cond,ndep,work);
    free(work);free(S);free(V);free(Ad);
    return 1;
    }
