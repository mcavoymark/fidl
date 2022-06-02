/* Copyright 1/11/11 Washington University.  All Rights Reserved.
   count_estimates.c  $Revision: 1.5 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include "fidl.h"
int _count_estimates(int argc,char **argv){
    float *AT = (float*)argv[0];
    #ifdef __sun__
        int n_interest = (int)argv[1];
        int t_valid = (int)argv[2];
    #else
        int n_interest = (intptr_t)argv[1];
        int t_valid = (intptr_t)argv[2];
    #endif
    int *cnt = (int*)argv[3];
    count_estimates(AT,n_interest,t_valid,cnt);
    return 1;
    }
void count_estimates(float *AT,int n_interest,int t_valid,int *cnt){
    float constrain;
    int i,j,k,count,count1;
    constrain = 1.-1./(float)n_interest;
    for(k=i=0;i<n_interest;i++) {
        for(count1=count=j=0;j<t_valid;j++,k++) {
            if(fabs(AT[k])>0.) count++;
            if(AT[k]==constrain) count1++;
            }
        cnt[i] = (count==t_valid)&&(count1>0) ? count1 : count;
        }
    }
