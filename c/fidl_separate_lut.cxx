/* Copyright 11/12/21 Washington University.  All Rights Reserved.
   fidl_separate_lut.cxx  $Revision: 1.1 $ */

#include <cstring>
#include <iostream>
#include <string>
#include <cmath>

#include "lut2.h"
#include "subs_nifti.h"
#include "stack.h"

int main(int argc,char **argv){
char *file=NULL,*lutf=NULL;
int i,lcrms=0,lcrmsonly=0;

if(argc<5){
    std::cerr<<argv[0]<<std::endl;
    std::cerr<<"Parse file into separate stacks. LUT identifies where in the stack and how to name. First location in stack is zero."<<std::endl;
    std::cerr<<"Each stack holds a single value. This has been added to compare L and R regions."<<std::endl;
    std::cerr<<"Root Mean Square can also be computed across regions. (Essential for averaging standard deviations.)"<<std::endl;
    std::cerr<<""<<std::endl;
    std::cerr<<"  -file:      File to separate"<<std::endl;
    std::cerr<<"  -lut:       e.g. /usr/local/pkg/freesurfer6.0/FreeSurferColorLUT.txt"<<std::endl;
    std::cerr<<"              Need not be an actual freesurfer lookuptable. Just needs to follow the freesurfer format."<<std::endl;
    std::cerr<<"  -rms        Root Mean Square is computed across regions."<<std::endl;

    //START211117
    std::cerr<<"  -rmsonly    Only the rms is output."<<std::endl;

    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-file") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        file=argv[++i];
    if(!strcmp(argv[i],"-lut") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        lutf=argv[++i];
    if(!strcmp(argv[i],"-rms"))
        lcrms=1;

    //START211117
    if(!strcmp(argv[i],"-rmsonly"))
        lcrmsonly=1;

    }
if(!file){std::cout<<"fidlError: -file not specified"<<std::endl;exit(-1);}
if(!lutf){std::cout<<"fidlError: -lut not specified"<<std::endl;exit(-1);}

lut l0;
l0.lut2(lutf);

stack s0;
float* f0;
if(!(f0=s0.stack0(file)))exit(-1);

int64_t dim64[]={1,1,1,1};
float centerf[3]={},mmppixf[3]={};

std::string str(file);
str=str.substr(str.find_last_of("/")+1,str.find_last_of(".nii.gz")-str.find_last_of("/")-7);

double rms=0.;
for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it){
    float f1 = f0[it->first];
    rms+=(double)f0[it->first]*(double)f0[it->first];

    #if 0
    std::string superbird=str+"_"+it->second+".nii.gz";
    if(!nifti_write((char*)superbird.c_str(),dim64,centerf,mmppixf,&f1))exit(-1);
    std::cout<<"Output written to "<<superbird<<std::endl;
    #endif
    //START211117
    if(!lcrmsonly){ 
        std::string superbird=str+"_"+it->second+".nii.gz";
        if(!nifti_write((char*)superbird.c_str(),dim64,centerf,mmppixf,&f1))exit(-1);
        std::cout<<"Output written to "<<superbird<<std::endl;
        }

    }

//if(lcrms){
//START211117
if(lcrms||lcrmsonly){

    float rmsf=(float)sqrt(rms/(double)l0.LUT.size());
    std::string superbird=str+"_rms.nii.gz";
    if(!nifti_write((char*)superbird.c_str(),dim64,centerf,mmppixf,&rmsf))exit(-1);
    std::cout<<"Output written to "<<superbird<<std::endl;
    }
}
