/* Copyright 12/27/10 Washington University.  All Rights Reserved.
   r_to_z.c  $Revision: 1.7 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef __sun__
    #include <nan.h>
#endif
#ifndef __sun__
    #include <stdint.h>
#endif
#include "fidl.h"
int _r_to_z(int argc,char **argv){
    int i,*idx;
    double *r,*t,*z,*df;
    float *rimg = (float *)argv[0];
    float *zimg = (float *)argv[1];
    #ifdef __sun__
        int vol = (int)argv[2];
        int nsub = (int)argv[3];
    #else
        int vol = (intptr_t)argv[2];
        int nsub = (intptr_t)argv[3];
    #endif
    if(!(r=malloc(sizeof*r*vol))) {
        printf("fidlError: Unable to malloc r\n");
        return 0;
        }
    if(!(t=malloc(sizeof*t*vol))) {
        printf("fidlError: Unable to malloc t\n");
        return 0;
        }
    if(!(z=malloc(sizeof*z*vol))) {
        printf("fidlError: Unable to malloc z\n");
        return 0;
        }
    if(!(df=malloc(sizeof*df*vol))) {
        printf("fidlError: Unable to malloc df\n");
        return 0;
        }
    if(!(idx=malloc(sizeof*idx*vol))) {
        printf("fidlError: Unable to malloc idx\n");
        return 0;
        }
    for(i=0;i<vol;i++) {
        r[i] = (double)rimg[i];
        df[i] = (double)(nsub-2);
        }
    r_to_z(r,t,z,df,vol,idx);
    for(i=0;i<vol;i++) zimg[i] = (float)z[i];
    free(idx);
    free(df);
    free(z);
    free(t);
    free(r);
    return 1;
    }
void r_to_z(double *r,double *t,double *z,double *df,int vol,int *idx){ /*correlation df = n-2 */
    int i;
    for(i=0;i<vol;i++) t[i] = r[i]*sqrt(df[i])/sqrt(1.-r[i]*r[i]);
    t_to_z(t,z,vol,df,idx);
    }
