/* Copyright 10/25/01 Washington University.  All Rights Reserved.
   boynton_contrast.c  $Revision: 1.11 $ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include "fidl.h"
short _boynton_contrast(int argc,char **argv)
{
    short *effect_length_short;
    int tot_eff,*effect_length,i,Mcol;
    float *effect_TR,*effect_shift_TR,dt,init_delay,*stimlen,*cnorm,*c;
    effect_length_short = (short *)argv[1];
    effect_TR = (float *)argv[2];
    effect_shift_TR = (float *)argv[3];
    dt = *(float *)argv[4];
    init_delay = *(float *)argv[5];
    stimlen = (float *)argv[6];
    cnorm = (float *)argv[7];
    c = (float *)argv[9];
    #ifdef __sun__
        tot_eff = (int)argv[0];
        Mcol = (int)argv[8];
    #else
        tot_eff = (intptr_t)argv[0];
        Mcol = (intptr_t)argv[8];
    #endif
    if(!(effect_length=malloc(sizeof*effect_length*tot_eff))) {
        printf("Error: Unable to malloc effect_length in _boynton_contrast\n");
        return 0;
        }
    for(i=0;i<tot_eff;i++) effect_length[i] = (int)effect_length_short[i];
    boynton_contrast(tot_eff,effect_length,effect_TR,effect_shift_TR,dt,init_delay,stimlen,cnorm,Mcol,c);
    free(effect_length);
    return 1;
}

int boynton_contrast(int tot_eff,int *effect_length,float *effect_TR,float *effect_shift_TR,float dt,float init_delay,
    float *stimlen,float *cnorm,int Mcol,float *c)
{
    int i,i1,eff,dont_use_c=0,max_effect_length;
    double *t,*cd;
    double mean,rsumsq;
    Hrfparam param;

    /*fprintf(stderr,"tot_eff=%d dt=%f init_delay=%f\n",tot_eff,dt,init_delay);
    fprintf(stderr,"effect_length "); for(i=0;i<tot_eff;i++) fprintf(stderr,"%d ",effect_length[i]); fprintf(stderr,"\n");
    fprintf(stderr,"effect_TR "); for(i=0;i<tot_eff;i++) fprintf(stderr,"%f ",effect_TR[i]); fprintf(stderr,"\n");
    fprintf(stderr,"effect_shift_TR "); for(i=0;i<tot_eff;i++) fprintf(stderr,"%f ",effect_shift_TR[i]); fprintf(stderr,"\n");
    fprintf(stderr,"stimlen "); for(i=0;i<tot_eff;i++) fprintf(stderr,"%f ",stimlen[i]); fprintf(stderr,"\n");
    fprintf(stderr,"cnorm "); for(i=0;i<tot_eff;i++) fprintf(stderr,"%f ",cnorm[i]); fprintf(stderr,"\n");*/


    for(max_effect_length=eff=0;eff<tot_eff;eff++) 
        if(effect_length[eff]>max_effect_length&&fabs(cnorm[eff])>.01) max_effect_length=effect_length[eff]; 
    if(!(t=malloc(sizeof*t*max_effect_length))) {
        printf("Error: Unable to malloc t in boynton_contrast\n");
        return -1;
        }
    if(!(cd=malloc(sizeof*cd*max_effect_length))) {
        printf("Error: Unable to malloc cd in boynton_contrast\n");
        return -1;
        }
    param.delta = (double)HRF_DELTA;
    param.tau = (double)HRF_TAU;
    param.A = (double)HRF_A;
    param.C = (double)HRF_C;

    for(i=0;i<Mcol;i++) c[i] = 0.;
    for(i1=eff=0;eff<tot_eff;eff++) {
        if(effect_length[eff] > 1 && fabs(cnorm[eff])>.01) {
            dont_use_c = 1;

            #if 0
            for(i=0;i<effect_length[eff];i++) {
                if((t = effect_TR[eff]*i + effect_TR[eff]*effect_shift_TR[eff] - dt - init_delay) < 0.) t = 0;
                c[i+i1] = boynton_model(t,stimlen[eff],(float)HRF_DELTA,(float)HRF_TAU,(int)TRUE);
                }
            #endif
            #if 0
            for(i=0;i<effect_length[eff];i++) {
                if((t[i] = (double)(effect_TR[eff]*i + effect_TR[eff]*effect_shift_TR[eff] - dt - init_delay)) < 0.) t[i]=0.;
                }
            boynton_model(effect_length[eff],t,(double)stimlen[eff],(double)HRF_DELTA,(double)HRF_TAU,1,cd);
            for(i=0;i<effect_length[eff];i++) c[i+i1] = (float)cd[i]; 
            #endif
            for(i=0;i<effect_length[eff];i++) {
                if((t[i] = (double)(effect_TR[eff]*i + effect_TR[eff]*effect_shift_TR[eff] - dt - init_delay)) < 0.) t[i]=0.;
                }
            param.duration = (double)stimlen[eff];
            boynton_model(effect_length[eff],t,&param,1,cd);
            for(i=0;i<effect_length[eff];i++) c[i+i1] = (float)cd[i];



            /*printf("boynton_model c="); for(i=0;i<effect_length[eff];i++) printf(" %f",c[i+i1]); printf("\n");*/

            for(mean=0,i=0;i<effect_length[eff];i++) mean += c[i1+i];
            mean /= effect_length[eff];
            for(i=0;i<effect_length[eff];i++) c[i1+i] -= mean;

            /*printf("boynton_model c(zero mean)="); for(i=0;i<effect_length[eff];i++) printf(" %f",c[i+i1]); printf("\n");*/

            /*This is calculated after subtracting the mean.*/
            for(rsumsq=0,i=0;i<effect_length[eff];i++) rsumsq += c[i1+i]*c[i1+i];
            rsumsq = sqrt(rsumsq);

            for(i=0;i<effect_length[eff];i++) c[i1+i] *= (cnorm[eff]/rsumsq); /*contrast is created from scratch so cnorm is necessary*/

            }
        i1 += effect_length[eff];
        }
    /*printf("boynton_contrast c="); for(i=0;i<Mcol;i++) printf(" %f",c[i]); printf("\n");*/
    free(cd);
    free(t);
    return dont_use_c;
}
