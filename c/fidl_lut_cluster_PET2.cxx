/* Copyright 5/26/21 Washington University.  All Rights Reserved.
   fidl_lut_cluster_PET2.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <cstring>
//#include <unordered_map>
#include <cmath>
#include <vector>
#include <fstream>
#include "lut.h"
#include "files_struct.h"
#include "stack.h"
#include "constants.h"
#include "subs_nifti.h"
#include "region.h"

int main(int argc,char **argv){
char *lutf=NULL,*wmparcf=NULL,*out=NULL;//,*out=NULL;
int i,j,nclusterf=0,clusteri=-1,npetf=0,nlabels=0;
Files_Struct *clusterf=NULL,*petf=NULL,*labels=NULL;
Regions *reg=NULL;
if(argc<3){
    std::cout<<"  -lut:     Lookup table. Ex. $FREESURFER_HOME/FreeSurferColorLUT.txt"<<std::endl;
    std::cout<<"  -cluster: Either a file with the region names listed therein or the region names themselves."<<std::endl;
    std::cout<<"            For multiple files, each file is a separate cluster."<<std::endl;
    std::cout<<"  -wmparc:  wmparc.nii.gz  float from HCP structural pipeline"<<std::endl;
    std::cout<<"            PET values are weighted by region size."<<std::endl;
    std::cout<<"  -pet:     PET stacks"<<std::endl;
    std::cout<<"  -labels:  Labels for PET values (optional)."<<std::endl;
    std::cout<<"  -out:     Stack is output instead of values echoed to the terminal."<<std::endl;
    std::cout<<"            String is appended to output filename."<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-lut")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            lutf = argv[++i];
            }
        else {
            std::cout<<"fidlError: No lookup table specified after -lut option. Abort!"<<std::endl;
            exit(-1);
            }
        }
    if(!strcmp(argv[i],"-cluster") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nclusterf;
        if(!(clusterf=read_files(nclusterf,&argv[clusteri=i+1])))exit(-1);
        i+=nclusterf;
        }
    if(!strcmp(argv[i],"-wmparc") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        wmparcf=argv[++i];
    if(!strcmp(argv[i],"-pet") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++npetf;
        if(!(petf=read_files(npetf,&argv[i+1])))exit(-1);
        i+=npetf;
        }
    if(!strcmp(argv[i],"-labels") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nlabels;

        //if(!(labels=get_files(nlabels,&argv[i+1])))exit(-1);
        //START210526
        if(!(labels=read_files(nlabels,&argv[i+1])))exit(-1);

        i+=nlabels;
        }
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        out=argv[++i];
    }
if(!lutf){
    std::cout<<"fidlError: Need to specify lookup table with -lut"<<std::endl;
    exit(-1);
    }
if(!clusterf){
    std::cout<<"fidlError: Need to specify clusters with -cluster"<<std::endl;
    exit(-1);
    }
if(!wmparcf){
    std::cout<<"fidlError: Need to specify -wmparc"<<std::endl;
    exit(-1);
    }
if(!petf){
    std::cout<<"fidlError: Need to specify -pet"<<std::endl;
    exit(-1);
    }

#if 0
if(!out){
    std::cout<<"fidlError: Need to specify -out"<<std::endl;
    exit(-1);
    }
#endif

//std::cout<<"here0"<<std::endl;

if(labels){
    if(labels->nfiles!=petf->nfiles){
        std::cout<<"fidlError: labels->nfiles="<<labels->nfiles<<" petf->nfiles="<<petf->nfiles<<" Must be equal. Abort!"<<std::endl;
        exit(-1);
        }
    }

//std::cout<<"clusterf->nfileseach=";for(i=0;i<nclusterf;++i)std::cout<<" "<<clusterf->nfileseach[i];std::cout<<std::endl;

if(!(reg=get_reg(wmparcf,0,(float*)NULL,0,(char**)NULL,0,0,0,(int*)NULL,lutf)))exit(-1);

lut l0;
l0.lut2(lutf);


#if 0
std::unordered_map<int,int> superbird;
for(i=0;i<clusterf->nfiles;++i){
    auto it=l0.LUT.begin();
    for(;it!=l0.LUT.end();++it){
        if(!strcmp(it->second.c_str(),clusterf->files[i])){
            superbird[it->first]=i+1;
            break;
            }
        }
    if(it==l0.LUT.end()){
        std::cout<<"****************** "<<clusterf->files[i]<<" not found ******************"<<std::endl;
        exit(-1);
        }
    }
#endif
//START210526
#if 0
//std::vector<std::unordered_map<int,int>> superbird(nclusterf);
//std::unordered_map<int,int> superbird;
int k;
for(k=i=0;i<nclusterf;++i){
    for(j=0;j<clusterf->nfileseach[i];++j,++k){
        auto it=l0.LUT.begin(); 
        for(;it!=l0.LUT.end();++it){
            if(!strcmp(it->second.c_str(),clusterf->files[k])){
                superbird[it->first]=i;
                break;
                }
            }
        if(it==l0.LUT.end()){
            std::cout<<"****************** "<<clusterf->files[k]<<" not found ******************"<<std::endl;
            exit(-1);
            }
        }
    }
#endif
std::vector<int> superbird(clusterf->nfiles); //zero
for(size_t i=0;i<clusterf->nfiles;++i){
    auto it=l0.LUT.begin();
    for(;it!=l0.LUT.end();++it){
        if(!strcmp(it->second.c_str(),clusterf->files[i])){
            superbird[i]=it->first;
            break;
            }
        }
    if(it==l0.LUT.end()){
        std::cout<<"****************** "<<clusterf->files[i]<<" not found ******************"<<std::endl;
        exit(-1);
        }
    }


#if 0
std::vector<double> wtm(petf->nfiles); //zero
std::vector<double> wtmd(petf->nfiles); //zero
float* tf;
for(size_t i=0;i<petf->nfiles;++i){
    stack fs1;
    if(!(tf=fs1.stack0(petf->files[i])))exit(-1);
    if(fs1.vol!=reg->nregions){
        std::cout<<"fs1.vol="<<fs1.vol<<" reg->nregions="<<reg->nregions<<" Must be equal. Abort!"<<std::endl;
        exit(-1);
        }
    for(auto it=superbird.begin();it!=superbird.end();++it){
        wtm[i]+=(double)tf[it->first]*(double)reg->nvoxels_region[it->first];
        wtmd[i]+=(double)reg->nvoxels_region[it->first];
        }
    }
#endif
//START210526
std::vector<std::vector<double> > wtm(nclusterf,std::vector<double>(petf->nfiles)); //zero
std::vector<std::vector<double> > wtmd(nclusterf,std::vector<double>(petf->nfiles)); //zero
float* tf;
for(size_t i=0;i<petf->nfiles;++i){
    stack fs1;
    if(!(tf=fs1.stack0(petf->files[i])))exit(-1);
    if(fs1.vol!=reg->nregions){
        std::cout<<"fs1.vol="<<fs1.vol<<" reg->nregions="<<reg->nregions<<" Must be equal. Abort!"<<std::endl;
        exit(-1);
        }
    int l;
    for(l=j=0;j<nclusterf;++j){
        for(int k=0;k<clusterf->nfileseach[j];++k,++l){
            wtm[j][i]+=(double)tf[superbird[l]]*(double)reg->nvoxels_region[superbird[l]];
            wtmd[j][i]+=(double)reg->nvoxels_region[superbird[l]];
            } 
        } 
    }
for(j=0;j<nclusterf;++j){
    for(size_t i=0;i<petf->nfiles;++i){
        wtm[j][i]/=wtmd[j][i];
        }
    }

#if 0
if(labels){for(size_t i=0;i<labels->nfiles;++i)std::cout<<labels->files[i]<<"\t";std::cout<<std::endl;}
for(size_t i=0;i<petf->nfiles;++i)std::cout<<wtm[i]/wtmd[i]<<"\t";std::cout<<std::endl;
if(out){
    std::ofstream ofs(out);
    if(labels){for(size_t i=0;i<labels->nfiles;++i)ofs<<labels->files[i]<<"\t";ofs<<std::endl;}
    for(size_t i=0;i<petf->nfiles;++i)ofs<<wtm[i]/wtmd[i]<<"\t";ofs<<std::endl;
    ofs.close();
    std::cout<<"Output written to "<<out<<std::endl;
    }
#endif
//START210526
if(!out){
    if(labels){for(size_t i=0;i<labels->nfiles;++i)std::cout<<labels->files[i]<<"\t";std::cout<<std::endl;}
    for(j=0;j<nclusterf;++j)for(size_t i=0;i<petf->nfiles;++i)std::cout<<wtm[j][i]<<"\t";std::cout<<std::endl;
    }
else{
    int64_t dims[]={nclusterf,1,1,1};
    float centerf[]={90,-126,-72}; //dummy
    float mmppixf[]={-0.7,0.7,0.7}; //dummy
    std::vector<float> tf(nclusterf); //zero
    std::string outstr(out);
    //std::string pwd;
    //pwd.assign(getenv("PWD"));
    for(size_t i=0;i<petf->nfiles;++i){
        for(j=0;j<nclusterf;++j)tf[j]=(float)wtm[j][i];

        std::string str(petf->files[i]);
        //str=pwd+"/"+str.substr(str.find_last_of("/")+1,str.find_last_of(".nii.gz")-str.find_last_of("/")-7)+"_"+outstr+".nii";
        str=str.substr(str.find_last_of("/")+1,str.find_last_of(".nii.gz")-str.find_last_of("/")-7)+"_"+outstr+".nii";
        if(!nifti_write((char*)str.c_str(),dims,centerf,mmppixf,&tf[0]))exit(-1);
        std::string str2="gzip -f "+str;
        if(system(str2.c_str())==-1)std::cout<<"fidlError: "<<str2<<std::endl;else std::cout<<"Output written to "<<str+".gz"<<std::endl;
        //if(system(str2.c_str())==-1)std::cout<<"fidlError: "<<str2<<std::endl;else std::cout<<str+".gz"<<std::endl;
        }
    }

} 
