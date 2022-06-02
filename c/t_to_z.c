/* Copyright 12/5/02 Washington University.  All Rights Reserved.
   t_to_z.c  $Revision: 1.15 $ */
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
int _t_to_z(int argc,char **argv){
    int i,lenvol,*t_to_zi;
    float *timg,*zimg;
    double *dtimg,*dzimg,df_scalar,*df;
    timg = (float *)argv[0];
    zimg = (float *)argv[1];
    df_scalar = *(double *)argv[3];
    #ifdef __sun__
        lenvol = (int)argv[2];
    #else
        lenvol = (intptr_t)argv[2];
    #endif
    if(!(dtimg=malloc(sizeof*dtimg*lenvol))) {
        printf("Error: Unable to malloc dtimg\n");
        return 0;
        }
    if(!(dzimg=malloc(sizeof*dzimg*lenvol))) {
        printf("Error: Unable to malloc dzimg\n");
        return 0;
        }
    if(!(df=malloc(sizeof*df*lenvol))) {
        printf("Error: Unable to malloc df\n");
        return 0;
        }
    if(!(t_to_zi=malloc(sizeof*t_to_zi*lenvol))) {
        printf("Error: Unable to malloc t_to_zi\n");
        return 0;
        }
    for(i=0;i<lenvol;i++) {
        dtimg[i] = (double)timg[i];
        df[i] = df_scalar;
        }
    t_to_z(dtimg,dzimg,lenvol,df,t_to_zi);
    for(i=0;i<lenvol;i++) zimg[i] = (float)dzimg[i];
    free(dtimg);
    free(dzimg);
    free(df);
    free(t_to_zi);
    return 1;
    }
void t_to_z(double *t,double *z,int lenvol,double *df,int *index){
    int j,i,count;
    double log2pi,lgonehalf,logbd,logp,z0sq,td;
    log2pi = (double)M_LN2 + log((double)M_PI);
    lgonehalf = lgamma(.5);
    for(count=j=0;j<lenvol;j++) {


        #if 0
        if((td=fabs(t[j]))==(double)UNSAMPLED_VOXEL) {
            z[j] = (double)UNSAMPLED_VOXEL;
            }
        else {
            if((td>=7.5&&df[j]>=15.)||df[j]<15.) {
                logbd = lgamma(.5*df[j])+lgonehalf-lgamma(.5*(df[j]+1));
                logp = -.5*log(df[j])-logbd-log(td)-.5*(df[j]-1)*log(1.+td*td/df[j])
                    +log(1.-df[j]/((df[j]+2.)*td*td)+3.*df[j]*df[j]/((df[j]+2.)*(df[j]+4.)*pow(td,4.)));
              #ifdef __sun__
                if(IsNANorINF(logp)) {
              #else
                if(isnan(logp) || isinf(logp)) {
              #endif
                    index[count++] = j;
                    }
                else if(logp < -14.5) {
                    z0sq = -2.*logp - log2pi;
                    z[j] = sqrt(z0sq);
                    for(i=0;i<3;i++) z[j] = sqrt(z0sq - 2.*log(z[j]) + 2.*log(1-pow(z[j],-2.)+3.*pow(z[j],-4.)));
                    z[j] *= t[j]<0. ? -1. : 1.;
                    }
                else {
                    index[count++] = j;
                    }
                }
            else {
                index[count++] = j;
                }
            }
        #endif
        //START160713
        #if 0
        if(t[j]==(double)UNSAMPLED_VOXEL||!isfinite(t[j])){
            z[j]=(double)UNSAMPLED_VOXEL;
            }
        else{
            if(((td=fabs(t[j]))>=7.5&&df[j]>=15.)||df[j]<15.){
                logbd = lgamma(.5*df[j])+lgonehalf-lgamma(.5*(df[j]+1));
                logp = -.5*log(df[j])-logbd-log(td)-.5*(df[j]-1)*log(1.+td*td/df[j])
                    +log(1.-df[j]/((df[j]+2.)*td*td)+3.*df[j]*df[j]/((df[j]+2.)*(df[j]+4.)*pow(td,4.)));
                if(!isfinite(logp)){
                    z[j]=(double)UNSAMPLED_VOXEL;
                    }
                else if(logp < -14.5){
                    z0sq = -2.*logp - log2pi;
                    z[j] = sqrt(z0sq);
                    for(i=0;i<3;i++) z[j] = sqrt(z0sq - 2.*log(z[j]) + 2.*log(1-pow(z[j],-2.)+3.*pow(z[j],-4.)));
                    z[j] *= t[j]<0. ? -1. : 1.;
                    }
                else{
                    index[count++]=j;
                    }
                }
            else{
                index[count++] = j;
                }
            }
        #endif
        //START160714
        if(t[j]==(double)UNSAMPLED_VOXEL||!isfinite(t[j])){
            z[j]=(double)UNSAMPLED_VOXEL;
            }
        else if(((td=fabs(t[j]))>=7.5&&df[j]>=15.)||df[j]<15.){
            logbd = lgamma(.5*df[j])+lgonehalf-lgamma(.5*(df[j]+1));
            logp = -.5*log(df[j])-logbd-log(td)-.5*(df[j]-1)*log(1.+td*td/df[j])
                +log(1.-df[j]/((df[j]+2.)*td*td)+3.*df[j]*df[j]/((df[j]+2.)*(df[j]+4.)*pow(td,4.)));
            if(!isfinite(logp)){
                z[j]=(double)UNSAMPLED_VOXEL;
                }
            else if(logp < -14.5){
                z0sq = -2.*logp - log2pi;
                z[j] = sqrt(z0sq);
                for(i=0;i<3;i++) z[j] = sqrt(z0sq - 2.*log(z[j]) + 2.*log(1-pow(z[j],-2.)+3.*pow(z[j],-4.)));
                z[j] *= t[j]<0. ? -1. : 1.;
                }
            else{
                index[count++]=j;
                }
            }
        else{
            index[count++] = j;
            }





        }
    if(count) {
        for(j=0;j<count;j++) {
            if(t[index[j]]==(double)UNSAMPLED_VOXEL) {
                z[index[j]] = (double)UNSAMPLED_VOXEL;
                }
            else {
                //printf("here102 t[%d]=%f df[%d]=%f\n",index[j],t[index[j]],index[j],df[index[j]]);fflush(stdout);
                td = gsl_cdf_tdist_Q(t[index[j]],df[index[j]]);
                z[index[j]] = gsl_cdf_ugaussian_Qinv(td);
                }
            }
        }
    }
