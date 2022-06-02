/* Copyright 9/29/10 Washington University. All Rights Reserved.
   trend.c  $Revision: 1.6 $ */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "trend.h"

void trend(int nfiles,int tdim_total,int *tdim,int *valid_frms,double *slope,int *iframes){
    int i,j,jj,nframes;
    double mean,scl;
    for(i=0;i<tdim_total;i++) slope[i]=0.;
    for(jj=i=0;i<nfiles;i++) {
        scl = 2./(double)(tdim[i]-1);
        for(mean=0.,nframes=j=0;j<tdim[i];j++,jj++) {
            if(valid_frms[jj]) {
                mean += slope[jj] = scl*(double)j - 1.;
                iframes[nframes++] = jj;
                }
            }
        mean /= (double)nframes;
        for(j=0;j<nframes;j++) slope[iframes[j]] -= mean;
        }
    }

int _trend(int argc,char **argv){
    int i,tdim_max,*iframes,*valid_frmsi;
    #ifdef __sun__
        int nfiles = (int)argv[0];
        int tdim_total = (int)argv[1];
    #else
        int nfiles = (intptr_t)argv[0];
        int tdim_total = (intptr_t)argv[1];
    #endif
    int *tdim = (int*)argv[2];
    float *valid_frms = (float*)argv[3];
    double *slope = (double*)argv[4];
    for(tdim_max=i=0;i<nfiles;i++) if(tdim_max<tdim[i]) tdim_max=tdim[i];
    if(!(iframes=malloc(sizeof*iframes*tdim_max))) {
        printf("fidlError: Unable to malloc iframes\n");
        return 0;
        }
    if(!(valid_frmsi=malloc(sizeof*valid_frmsi*tdim_total))) {
        printf("fidlError: Unable to malloc valid_frmsi\n");
        return 0;
        }
    for(i=0;i<tdim_total;i++) valid_frmsi[i] = (int)valid_frms[i];
    trend(nfiles,tdim_total,tdim,valid_frmsi,slope,iframes);
    free(valid_frmsi);
    free(iframes);
    return 1;
    }

