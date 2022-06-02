/* Copyright 6/25/15 Washington University.  All Rights Reserved.
   nifti_flipslice.cxx  $Revision: 1.3 $*/
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <iostream>
#include "nifti_flipslice.h"

int nifti_flipslice(int64_t xdim,int64_t ydim,int cf_flip,float *stack){
    float tf;
    if(cf_flip==12){
        int64_t j=xdim*ydim/2+xdim*ydim%2;
        int64_t k=xdim*ydim-1;
        for(int64_t i=0;i<j;i++,k--){
            tf=stack[i];
            stack[i]=stack[k];
            stack[k]=tf;
            }
        }
    else if(cf_flip==412){
        int64_t i,i1,j;
        for(i1=ydim-1,i=0;i<ydim/2;i++,i1--){
            for(j=0;j<xdim;j++){
                tf=stack[i*xdim+j];
                stack[i*xdim+j]=stack[i1*xdim+j];
                stack[i1*xdim+j]=tf;
                }
            }
        }
    else{
        std::cerr<<"fidlError: Unknown cf_flip="<<cf_flip<<std::endl;
        return 0;
        }
    return 1;
    }

//START190926
int nifti_flipsliceus(int64_t xdim,int64_t ydim,int cf_flip,unsigned short *stack){
    unsigned short tf;
    if(cf_flip==12){
        int64_t j=xdim*ydim/2+xdim*ydim%2;
        int64_t k=xdim*ydim-1;
        for(int64_t i=0;i<j;i++,k--){
            tf=stack[i];
            stack[i]=stack[k];
            stack[k]=tf;
            }
        }
    else if(cf_flip==412){
        int64_t i,i1,j;
        for(i1=ydim-1,i=0;i<ydim/2;i++,i1--){
            for(j=0;j<xdim;j++){
                tf=stack[i*xdim+j];
                stack[i*xdim+j]=stack[i1*xdim+j];
                stack[i1*xdim+j]=tf;
                }
            }
        }
    else{
        std::cerr<<"fidlError: Unknown cf_flip="<<cf_flip<<std::endl;
        return 0;
        }
    return 1;
    }

//START201008
int nifti_flipslicesi(int64_t xdim,int64_t ydim,int cf_flip,int* stack){
    int tf;
    if(cf_flip==12){
        int64_t j=xdim*ydim/2+xdim*ydim%2;
        int64_t k=xdim*ydim-1;
        for(int64_t i=0;i<j;i++,k--){
            tf=stack[i];
            stack[i]=stack[k];
            stack[k]=tf;
            }
        }
    else if(cf_flip==412){
        int64_t i,i1,j;
        for(i1=ydim-1,i=0;i<ydim/2;i++,i1--){
            for(j=0;j<xdim;j++){
                tf=stack[i*xdim+j];
                stack[i*xdim+j]=stack[i1*xdim+j];
                stack[i1*xdim+j]=tf;
                }
            }
        }
    else{
        std::cerr<<"fidlError: Unknown cf_flip="<<cf_flip<<std::endl;
        return 0;
        }
    return 1;
    }


int _nifti_flipslice(int argc,char **argv){
    if(argc<4){
        std::cerr<<"fidlError: _nifti_flipslice requires four arguments: int64_t xdim,int64_t ydim,int cf_flip,float *stack"<<std::endl;
        return 0;
        }
    int64_t xdim=(intptr_t)argv[0];
    int64_t ydim=(intptr_t)argv[1];
    int cf_flip=(intptr_t)argv[2];
    float *stack=(float*)argv[3];
    if(!nifti_flipslice(xdim,ydim,cf_flip,stack))return 0;
    return 1;
    }
