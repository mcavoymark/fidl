/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   f_to_z.c  $Revision: 1.30 $ */
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
#include <gsl/gsl_cdf.h>
#include "fidl.h"

#ifndef M_LN2
    #define M_LN2 0.693147180559945309417 
#endif
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
int _f_to_z(int argc,char **argv){
    int i,lenvol,*index;
    float *fimg,*zimg;
    double condition_dof,error_dof,*dfimg,*dzimg,*df1,*df2;
    fimg = (float *)argv[0];
    zimg = (float *)argv[1];
    condition_dof = *(double *)argv[3];
    error_dof = *(double *)argv[4];
    #ifdef __sun__
        lenvol = (int)argv[2];
    #else
        lenvol = (intptr_t)argv[2];
    #endif
    if(!(dfimg=malloc(sizeof*dfimg*lenvol))) {
        printf("Error: Unable to malloc dfimg\n");
        return 0;
        }
    if(!(dzimg=malloc(sizeof*dzimg*lenvol))) {
        printf("Error: Unable to malloc dzimg\n");
        return 0;
        }
    if(!(df1=malloc(sizeof*df1*lenvol))) {
        printf("Error: Unable to malloc df1\n");
        return 0;
        }
    if(!(df2=malloc(sizeof*df2*lenvol))) {
        printf("Error: Unable to malloc df2\n");
        return 0;
        }
    if(!(index=malloc(sizeof*index*lenvol))) {
        printf("Error: Unable to malloc index\n");
        return 0;
        }
    for(i=0;i<lenvol;i++) {
        df1[i] = condition_dof;
        df2[i] = error_dof;
        dfimg[i] = (double)fimg[i];
        }
    f_to_z(dfimg,dzimg,lenvol,df1,df2,index);
    for(i=0;i<lenvol;i++) zimg[i] = (float)dzimg[i];
    free(dfimg);
    free(dzimg);
    free(df1);
    free(df2);
    free(index);
    return 1;
    }
void f_to_z(double *f,double *z,int lenvol,double *df1,double *df2,int *index){
    int i,j,k,count;
    double log2pi,z0sq,df1div2,n,invdf1div2,df2div2,m,a,d2divd1,product,logp,td;
    log2pi = (double)M_LN2 + log((double)M_PI);
    for(count=j=0;j<lenvol;j++) {

        #if 0
        if(f[j] > 1.) {
            df1div2 = df1[j]/2.;
            n = 1. - df1[j]/2.;
            invdf1div2 = 2./df1[j];
            df2div2 = df2[j]/2.;
            m = (df1[j]+df2[j])/2.;
            a = df1[j]/df2[j];
            d2divd1 = df2[j]/df1[j];
            for(logp=0.,i=1;i<=20;i++) {
                for(product=1.,k=1;k<=i;k++) product *= (2*k-df1[j])/(2*k+df2[j]);
                logp += pow(-1.,(double)i)*pow(d2divd1,(double)i)*pow(f[j],(double)(-i))*product;
                }
            logp = df1div2*log(a) + lgamma(m)-lgamma(df2div2)-lgamma(df1div2) - (m-1.)*log(1.+a*f[j]) - n*log(f[j]) +
                log(invdf1div2*(1+logp));
          #ifdef __sun__
            if(IsNANorINF(logp)) {
          #else
            if(isnan(logp) || isinf(logp)) {
          #endif
                index[count++] = j;
                }
            else if(logp < -14.05) {
                z0sq = -2.*logp - log2pi;
                zmap[j] = sqrt(z0sq);
                for(i=0;i<3;i++) {
                    zmap[j] = sqrt(z0sq - 2*log(zmap[j]) + 2*log(1-pow(zmap[j],-2.)+3*pow(zmap[j],-4.)));
                    }
                }
            else {
                index[count++] = j;
                }
            }
        else {
            zmap[j] = (double)UNSAMPLED_VOXEL;
            }
        #endif
        //START160714
        if(f[j]==(double)UNSAMPLED_VOXEL||!isfinite(f[j])){
            z[j]=(double)UNSAMPLED_VOXEL;
            }
        else if(f[j] > 1.) {
            df1div2 = df1[j]/2.;
            n = 1. - df1[j]/2.;
            invdf1div2 = 2./df1[j];
            df2div2 = df2[j]/2.;
            m = (df1[j]+df2[j])/2.;
            a = df1[j]/df2[j];
            d2divd1 = df2[j]/df1[j];
            for(logp=0.,i=1;i<=20;i++) {
                for(product=1.,k=1;k<=i;k++) product *= (2*k-df1[j])/(2*k+df2[j]);
                logp += pow(-1.,(double)i)*pow(d2divd1,(double)i)*pow(f[j],(double)(-i))*product;
                }
            logp = df1div2*log(a) + lgamma(m)-lgamma(df2div2)-lgamma(df1div2) - (m-1.)*log(1.+a*f[j]) - n*log(f[j]) +
                log(invdf1div2*(1+logp));
            if(!isfinite(logp)){
                z[j]=(double)UNSAMPLED_VOXEL;
                }
            else if(logp < -14.05) {
                z0sq = -2.*logp - log2pi;
                z[j] = sqrt(z0sq);
                for(i=0;i<3;i++) {
                    z[j] = sqrt(z0sq - 2*log(z[j]) + 2*log(1-pow(z[j],-2.)+3*pow(z[j],-4.)));
                    }
                }
            else {
                index[count++] = j;
                }
            }
        else {
            z[j] = (double)UNSAMPLED_VOXEL;
            }


        }
    if(count) {
        for(j=0;j<count;j++) {
            if(f[index[j]]==(double)UNSAMPLED_VOXEL) {
                z[index[j]] = (double)UNSAMPLED_VOXEL;
                }
            else {
                td = gsl_cdf_fdist_Q(f[index[j]],df1[index[j]],df2[index[j]]);
                z[index[j]] = gsl_cdf_ugaussian_Qinv(td/2.);
                }
            }

        }
    }
