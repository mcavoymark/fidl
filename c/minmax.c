/* Copyright 8/29/05 Washington University.  All Rights Reserved.
   minmax.c  $Revision: 1.6 $*/
#include "minmax.h"
void min_and_max_init(float *min,float *max){
    *min = (float)1.e20;
    *max = (float)-1.e20;
    }
void min_and_max_intstack(int *stack,int size,float *min,float *max){
    int i;
    for(i=0;i<size;i++) {
        if((float)stack[i] > *max) *max = (float)stack[i];
        if((float)stack[i] < *min) *min = (float)stack[i];
        }
    }
void min_and_max_floatstack(float *stack,int size,float *min,float *max){
    int i;
    for(i=0;i<size;i++) {
        if(stack[i] > *max) *max = stack[i];
        if(stack[i] < *min) *min = stack[i];
        }
    }
void min_and_max_doublestack(double *stack,int size,float *min,float *max){
    int i;
    for(i=0;i<size;i++) {
        if((float)stack[i] > *max) *max = (float)stack[i];
        if((float)stack[i] < *min) *min = (float)stack[i];
        }
    }

//START170313
void min_and_max(float *stack,int size,float *min,float *max){
    int i;
    for(*min=1.e20,*max=-1.e20,i=0;i<size;i++){
        if(stack[i] > *max)
            *max = stack[i];
        if(stack[i] < *min)
            *min = stack[i];
        }
    }
void min_and_max_new(float *stack,int size,float *min,float *max,int initialize){
    int i;
    if(initialize){
        *min = 1.e20;
        *max = -1.e20;
        }
    for(i=0;i<size;i++){
        if(stack[i] > *max)
            *max = stack[i];
        if(stack[i] < *min)
            *min = stack[i];
        }
    }
