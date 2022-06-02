/* Copyright 7/22/15 Washington University.  All Rights Reserved.
   cifti_getColumn.cxx  $Revision: 1.2 $*/
#include "CiftiException.h"
#include "CiftiFile.h"
#include <cstdio>
#include <iostream>
//#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
using namespace std;
using namespace cifti;
extern "C" int cifti_getColumn(char *file,int64_t t,float *stack){
    try{
        CiftiFile inputFile(file);//on-disk reading by default
        const vector<int64_t>& dims = inputFile.getDimensions();
        if(dims.size()!=2){
            cerr << "fidlError: "<<file<<" Not a 2D cifti" << endl;
            return 0;
            }
        inputFile.getColumn(stack,t);
        }
    catch (CiftiException& e){
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 0;
        }
    return 1;
    }
extern "C" int _cifti_getColumn(int argc,char **argv){
    if(argc<3){
        cerr<<"fidlError: _cifti_getColumn requires three arguments: char *file,int64_t t,float *stack"<<endl;
        return 0;
        }
    char *file=argv[0];
    int64_t t=(intptr_t)argv[1];
    float *stack=(float*)argv[2];
    if(!cifti_getColumn(file,t,stack))return 0;
    return 1;
    }
