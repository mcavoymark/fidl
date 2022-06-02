/* Copyright 7/30/15 Washington University.  All Rights Reserved.
   nifti_getorient.cxx  $Revision: 1.3 $*/
//#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "CiftiException.h"
#include "NiftiHeader.h"
#include "VolumeSpace.h"
#include <cstdio>
#include <iostream>
using namespace std;
using namespace cifti;
extern "C" int nifti_getorient(float *center,float *mmppix,int *c_orient){
    try{
        int64_t dims[3]={0,0,0};
        float sform[12]={0,0,0,0,0,0,0,0,0,0,0,0};
        sform[0]=mmppix[0];sform[3]=center[0];
        sform[5]=mmppix[1];sform[7]=center[1];
        sform[10]=mmppix[2];sform[11]=center[2];
        VolumeSpace myVolSpace(dims,sform);
        VolumeSpace::OrientTypes orient[3];
        myVolSpace.getOrientation(orient);
        c_orient[0]=(intptr_t)orient[0];
        c_orient[1]=(intptr_t)orient[1];
        c_orient[2]=(intptr_t)orient[2];

        //cout<<"orient[0]="<<orient[0]<<" orient[1]="<<orient[1]<<" orient[2]="<<orient[2]<<endl;

        } 
     catch (CiftiException& e) {
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 0;
        }
    return 1;
    }
extern "C" int _nifti_getorient(int argc,char **argv){
    if(argc<3){
        cerr<<"fidlError: _nifti_getorient requires three arguments: float *center,float *mmppix,int *c_orient"<<endl;
        return 0;
        }
    float *center=(float*)argv[0];
    float *mmppix=(float*)argv[1];
    int *c_orient=(int*)argv[2];
    if(!nifti_getorient(center,mmppix,c_orient)) return 0;
    //cout<<"c_orient[0]="<<c_orient[0]<<" c_orient[1]="<<c_orient[1]<<" c_orient[2]="<<c_orient[2]<<endl;
    return 1;
    }
