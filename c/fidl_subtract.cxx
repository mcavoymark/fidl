/* Copyright 4/04/22 Washington University.  All Rights Reserved.
   fidl_subtract.cxx  $Revision: 1.1 $ */

#include <cstdio>
#include <iostream>
#include <cstring>
#include <cmath>
#include "fidl.h"
#include "mask.h"
#include "filetype.h"
#include "write1.h"

int main(int argc,char **argv){
char *file1=NULL,*file2=NULL,*maskf=NULL,*outf=NULL;
int i,j;
mask ms;
W1 *w1=NULL;

if(argc<5){
    std::cout<<argv[0]<<std::endl;
    std::cout<<"Subtract two images: f1 - f2"<<std::endl;
    std::cout<<"    -f1: nii or 4dfp"<<std::endl;
    std::cout<<"    -f2: nii or 4dfp"<<std::endl;
    std::cout<<"    -mask:  Only voxels in the mask are analyzed."<<std::endl;
    std::cout<<"    -out:   Output name or root."<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-f1") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        file1=argv[++i];
    if(!strcmp(argv[i],"-f2") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        file2=argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        maskf=argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        outf=argv[++i];
    }
if(!file1){std::cout<<"fidlError: Need to specify file1 with -file1"<<std::endl;exit(-1);}
if(!file2){std::cout<<"fidlError: Need to specify file2 with -file2"<<std::endl;exit(-1);}
if(!outf){std::cout<<"fidlError: Need to specify output name with -out"<<std::endl;exit(-1);}

float *f1,*f2;
stack s1,s2;
if(!(f1=s1.stack0(file1)))exit(-1);
if(!(f2=s2.stack0(file2)))exit(-1);
if(s1.vol!=s2.vol){std::cout<<"fidlError: s1.vol="<<s1.vol<<" s2.vol="<<s2.vol<<" Must be equal. Abort!"<<std::endl;exit(-1);}
if(!(ms.get_mask(maskf,s1.vol,(int*)NULL,(LinearModel*)NULL,s1.vol)))exit(-1);
if(maskf)if(s1.vol!=ms.vol){std::cout<<"fidlError: s1.vol="<<s1.vol<<" ms.vol="<<ms.vol<<" Must be equal. Abort!"<<std::endl;exit(-1);}
float* temp_float=new float[s1.vol]();

for(j=0;j<ms.lenbrain;j++){
    if(std::abs(f1[ms.brnidx[j]])>(float)UNSAMPLED_VOXEL&&!isnan(f1[ms.brnidx[j]]) &&
       std::abs(f2[ms.brnidx[j]])>(float)UNSAMPLED_VOXEL&&!isnan(f2[ms.brnidx[j]])){
        temp_float[ms.brnidx[j]]=(float)((double)f1[ms.brnidx[j]]-(double)f2[ms.brnidx[j]]);
        }
    }

if(!(w1=write1_init()))exit(-1);
w1->filetype=s1.filetype;
w1->swapbytes=0;
w1->temp_float=temp_float;
std::string str(outf);
if(s1.filetype==(int)IMG){
    w1->how_many=s1.vol;
    w1->ifh=s1.ifh;
    }
else if(s1.filetype==(int)NIFTI){
    w1->file_hdr=file1;
    if(str.find(".nii")==std::string::npos)
        str+=".nii";
    else if(!str.compare(str.size()-2,2,"gz")){
        str=str.substr(0,str.size()-3);
        }
    }
if(!write1((char*)str.c_str(),w1))exit(-1); //gzips
str.append(".gz");
std::cout<<"Output written to "<<str<<std::endl;
}
