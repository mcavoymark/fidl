/* Copyright 12/10/19 Washington University.  All Rights Reserved.
   fidl_extractlabels.cxx  $Revision: 1.1 $ */
#include <cstdlib>
#include <iostream>
#include <cstring>
#include "stack.h"
#include "subs_nifti.h"
int main(int argc,char **argv)
{
char *f0=NULL,*o0=NULL;
unsigned short *us;
int i,j,nv0=0;
float *v0=NULL,*p0;
stack s0;
if(argc<7){
    std::cout<<"fidl_extractlabels -f wmparc.nii.gz -v 17 53 -o LRhippo"<<std::endl;
    std::cout<<"Extract voxels with value 17 and 53 from wmparc.nii.gz and write to unsigned short file LRhippo.nii.gz "<<std::endl;
    std::cout<<"-f: Assumed to be a float file."<<std::endl;
    std::cout<<"-v: Voxel values to extract."<<std::endl;
    std::cout<<"-o: <output>.nii.gz\n"<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-f") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        f0=argv[++i];
    if(!strcmp(argv[i],"-v") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nv0;
        v0=new float[nv0];
        for(j=0;j<nv0;++j)v0[j]=strtof(argv[++i],NULL);
        }
    if(!strcmp(argv[i],"-o") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        o0=argv[++i];
    }
if(!f0){std::cout<<"-f not specified"<<std::endl;exit(-1);}
if(!nv0){std::cout<<"-v not specified"<<std::endl;exit(-1);}
if(!o0){std::cout<<"-o not specified"<<std::endl;exit(-1);}
if(!(p0=s0.stack0(f0)))exit(-1);
us=new unsigned short[s0.vol];
for(i=0;i<s0.vol;++i){
    us[i]=0;
    for(j=0;j<nv0;++j){
        if(p0[i]==v0[j]){
            us[i]=(unsigned short)v0[j];
            break;
            } 
        } 
    } 
std::string str(o0);
if(str.find(".nii")==std::string::npos)str.append(".nii");
if(!nifti_write2us((char*)str.c_str(),f0,us))exit(-1);
std::string str2="gzip -f "+str;
if(system(str2.c_str())==-1)
    std::cout<<"fidlError: "<<str2.c_str()<<std::endl;
else
    str.append(".gz"); 
std::cout<<"Output written to "<<str<<std::endl;
}
