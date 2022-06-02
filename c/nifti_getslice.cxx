/* Copyright 6/25/15 Washington University.  All Rights Reserved.
   nifti_getslice.cxx  $Revision: 1.4 $*/
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

//extern "C" int nifti_getslice(char *file,int64_t z,int64_t t,int cf_flip,float *stack){
//START150807
extern "C" int nifti_getslice(char *file,int64_t z,int64_t t,float *stack){

    try{
        NiftiIO inputFile;
        inputFile.openRead(file);
        if(inputFile.getDimensions().size()<3||inputFile.getDimensions().size()>4){
            cerr << "fidlError: "<<file<<" Not a 3D or 4D volume" << endl;
            return 0;
            }
        vector<int64_t> mySelect(inputFile.getDimensions().size()-2,0);
        mySelect[0]=z;
        if(mySelect.size()>=2)mySelect[1]=t;
        inputFile.readData(stack,2,mySelect);

        //const NiftiHeader& myHeader=inputFile.getHeader();
        //VolumeSpace myVolSpace(myHeader.getDimensions().data(),myHeader.getSForm());
        //VolumeSpace::OrientTypes orient[3];
        //myVolSpace.getOrientation(orient);
        //cout<<"here0 orient[0]="<<orient[0]<<" orient[1]="<<orient[1]<<" orient[2]="<<orient[2]<<endl;

        //float spacing[3],origin[3];
        //myVolSpace.getOrientAndSpacingForPlumb(orient,spacing,origin);
        //cout<<"here1 orient[0]="<<orient[0]<<" orient[1]="<<orient[1]<<" orient[2]="<<orient[2]<<endl;

        //cout<<"nifti_getslice "<<cf_flip<<endl;

        //START150807
        #if 0 
        //if(orient[0]==0&&orient[1]==1&&orient[2]==2){
        if(cf_flip==12){
            size_t j=inputFile.getDimensions()[0]*inputFile.getDimensions()[1]/2
                +inputFile.getDimensions()[0]*inputFile.getDimensions()[1]%2;
            size_t k=inputFile.getDimensions()[0]*inputFile.getDimensions()[1]-1;
            float tf;
            for(size_t i=0;i<j;i++,k--){
                tf=stack[i];
                stack[i]=stack[k];
                stack[k]=tf;
                }
            }
        //START150710
        else if(cf_flip==412){
            size_t i,i1;
            size_t row=inputFile.getDimensions()[0];
            size_t col=inputFile.getDimensions()[1];
            float tf;
            for(i1=col-1,i=0;i<col/2;i++,i1--){
                for(size_t j=0;j<row;j++){
                    tf=stack[i*row+j];
                    stack[i*row+j]=stack[i1*row+j];
                    stack[i1*row+j]=tf;
                    }
                }
            }
        #endif

        inputFile.close();
        }
    catch(CiftiException& e){
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 0;
        }
    return 1;
    }
extern "C" int _nifti_getslice(int argc,char **argv){

    //if(argc<5){
    //    cerr << "_nifti_getslice requires five arguments: char *file,int64_t z,int64_t t,int cf_flip,float *stack" << endl;
    //START150807
    if(argc<4){
        cerr<<"fidlError: _nifti_getslice requires four arguments: char *file,int64_t z,int64_t t,float *stack" << endl;

        return 0;
        }
    char *file=argv[0];
    int64_t z=(intptr_t)argv[1];
    int64_t t=(intptr_t)argv[2];

    #if 0
    int cf_flip=(intptr_t)argv[3];
    float *stack=(float*)argv[4];
    if(!nifti_getslice(file,z,t,cf_flip,stack))return 0;
    #endif
    /*START150807*/
    float *stack=(float*)argv[3];
    if(!nifti_getslice(file,z,t,stack))return 0;

    return 1;
    }
