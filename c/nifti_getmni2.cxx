/* Copyright 7/31/15 Washington University.  All Rights Reserved.
   nifti_getmni2.cxx  $Revision: 1.2 $*/
//#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "CiftiException.h"
#include "NiftiIO.h"
#include "VolumeSpace.h"
#include <cstdio>
#include <iostream>
using namespace std;
using namespace cifti;
extern "C" int nifti_getmni2(float *center,float *mmppix,int64_t *dims,float *indices,float *coor){
    try{
        float sform[12]={0,0,0,0,0,0,0,0,0,0,0,0};
        sform[0]=mmppix[0];sform[3]=center[0];
        sform[5]=mmppix[1];sform[7]=center[1];
        sform[10]=mmppix[2];sform[11]=center[2];
        VolumeSpace myVolSpace(dims,sform);
        myVolSpace.indexToSpace(indices,coor);
        }
    catch(CiftiException& e){
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 0;
        }
    return 1;
    }
extern "C" int _nifti_getmni2(int argc,char **argv){
    if(argc<5){
        cerr<<"fidlError: _nifti_getmni2 requires five arguments: float *center,float *mmppix,int64_t *dims,float *indices,"<<endl;
        cerr<<"           float *coor"<<endl;
        return 0;
        }
    float *center=(float*)argv[0];
    float *mmppix=(float*)argv[1];
    int64_t *dims=(intptr_t*)argv[2];
    float *indices=(float*)argv[3];
    float *coor=(float*)argv[4];
    if(!nifti_getmni2(center,mmppix,dims,indices,coor))return 0;

    //cout<<"indices[0]="<<indices[0]<<" indices[1]="<<indices[1]<<" indices[2]="<<indices[2]<<endl;
    //cout<<"coor[0]="<<coor[0]<<" coor[1]="<<coor[1]<<" coor[2]="<<coor[2]<<endl;

    return 1;
    }
