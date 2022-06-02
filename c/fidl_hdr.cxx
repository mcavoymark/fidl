/* Copyright 4/21/20 Washington University.  All Rights Reserved.
   fidl_hdr.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <cstring>
#include <cmath>
#include "fidl.h"
#include "header.h"
#include "stack.h"

int main(int argc,char **argv){
int i,j,nfiles=0,lcval=0,lcvalall=0;
Files_Struct *files=NULL;
if(argc<3){
    std::cerr<<"Print header information. Specifically, I want to know the time dimesion which hasn't made its way to json yet."<<std::endl;
    std::cerr<<" -files: glm, img, nifti, cifti, conc or list."<<std::endl;
    std::cerr<<" -val    print stack values > 1e-37"<<std::endl;
    std::cerr<<" -valall print all stack values"<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;++i){
    if(!strcmp(argv[i],"-files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfiles;
        if(!(files=read_files(nfiles,&argv[i+1])))exit(-1);
        i+=nfiles;
        }
    if(!strcmp(argv[i],"-val"))lcval=1;
    if(!strcmp(argv[i],"-valall"))lcvalall=1;
    }

#if 0
for(size_t i=0;i<files->nfiles;++i){
    header h0;
    if(!(h0.header0(files->files[i])))continue;
    std::cout<<"dim=";for(j=0;j<4;++j)std::cout<<" "<<h0.dim[j];std::cout<<" vol="<<h0.vol<<std::endl;
    std::cout<<"mmppixf=";for(j=0;j<3;++j)std::cout<<" "<<h0.mmppixf[j];std::cout<<std::endl;
    std::cout<<"centerf=";for(j=0;j<3;++j)std::cout<<" "<<h0.centerf[j];std::cout<<std::endl;
    std::cout<<"datatype="<<h0.datatype<<" "<<h0.datatypestr<<std::endl;
    }
if(lcval)for(size_t i=0;i<files->nfiles;++i){
    stack s0;
    float* f0;
    if(!(f0=s0.stack0(files->files[i])))exit(-1);
    for(int j=0;j<s0.vol;++j)if(std::abs(f0[j])>(float)UNSAMPLED_VOXEL)std::cout<<j<<" "<<f0[j]<<std::endl;
    }
if(lcvalall)for(size_t i=0;i<files->nfiles;++i){
    stack s0;
    float* f0;
    if(!(f0=s0.stack0(files->files[i])))exit(-1);
    for(int j=0;j<s0.vol;++j)std::cout<<j<<" "<<f0[j]<<std::endl;
    }
#endif
//START211115
for(size_t i=0;i<files->nfiles;++i){
    header h0;
    if(!(h0.header0(files->files[i])))continue;
    std::cout<<files->files[i]<<std::endl;
    std::cout<<"    dim=";for(j=0;j<4;++j)std::cout<<" "<<h0.dim[j];std::cout<<" vol="<<h0.vol<<std::endl;
    std::cout<<"    voxel_size=";for(j=0;j<3;++j)std::cout<<" "<<h0.voxel_size[j];std::cout<<std::endl;
    std::cout<<"    mmppixf=";for(j=0;j<3;++j)std::cout<<" "<<h0.mmppixf[j];std::cout<<std::endl;
    std::cout<<"    centerf=";for(j=0;j<3;++j)std::cout<<" "<<h0.centerf[j];std::cout<<std::endl;
    std::cout<<"    datatype="<<h0.datatype<<" "<<h0.datatypestr<<std::endl;
    if(lcval){
        stack s0;
        float* f0;
        if(!(f0=s0.stack0(files->files[i])))exit(-1);
        
        //for(int j=0;j<s0.vol;++j)if(std::abs(f0[j])>(float)UNSAMPLED_VOXEL)std::cout<<"    "<<j<<" "<<f0[j]<<std::endl;
        //START220308
        for(int j=0;j<s0.lenvol;++j)if(std::abs(f0[j])>(float)UNSAMPLED_VOXEL)std::cout<<"    "<<j<<" "<<f0[j]<<std::endl;

        }
    if(lcvalall){
        stack s0;
        float* f0;
        if(!(f0=s0.stack0(files->files[i])))exit(-1);

        //for(int j=0;j<s0.vol;++j)std::cout<<"    "<<j<<" "<<f0[j]<<std::endl;
        //START220308
        for(int j=0;j<s0.lenvol;++j)std::cout<<"    "<<j<<" "<<f0[j]<<std::endl;

        }
    }
}
