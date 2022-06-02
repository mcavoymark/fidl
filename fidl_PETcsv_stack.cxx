/* Copyright 4/12/21 Washington University.  All Rights Reserved.
   fidl_PETcsv_stack.cxx  $Revision: 1.1 $ */

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include "lut.h"
#include "subs_nifti.h"

int main(int argc,char **argv){
char *csvf=NULL,*lutf=NULL;
int i;
if(argc < 5) {
    std::cout<<argv[0]<<std::endl;
    std::cout<<"  -csv:        .img's, .nii's, .nii.gz's or a conc or list."<<std::endl;
    std::cout<<"  -lut:         Local connectivity density is computed for only voxels in the -mask"<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-csv") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        csvf=argv[++i];
    if(!strcmp(argv[i],"-lut") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        lutf=argv[++i];
    }
if(!csvf){std::cout<<"fidlError: -csv not specified"<<std::endl;exit(-1);}
if(!lutf){std::cout<<"fidlError: -lut not specified"<<std::endl;exit(-1);}

lut l0;
l0.lut2(lutf);
//for(std::map<int,std::string>::iterator it=l0.LUT.begin();it!=l0.LUT.end();++it)std::cout<<it->first<<" "<<it->second<<std::endl;

std::ifstream ifile(csvf);
std::string str;
std::getline(ifile,str,'\r');
//std::cout<<str<<"END"<<std::endl;

std::istringstream iss(str);
std::vector<std::string> vstr;
while(std::getline(iss,str,','))vstr.push_back(str);
//for(size_t i=0;i<vstr.size();++i)std::cout<<vstr[i]<<"END"<<std::endl;std::cout<<"vstr.size()="<<vstr.size()<<std::endl;

std::vector<int> idx;std::vector<size_t> vidx;
for(size_t i=7;i<vstr.size();++i){
    for(std::map<int,std::string>::iterator it=l0.LUT.begin();it!=l0.LUT.end();++it){
        if(!vstr[i].compare(it->second)){
            idx.push_back(it->first); 
            vidx.push_back(i);
            break;
            }
        }
    //std::cout<<"\n"<<std::endl;
    }
//for(size_t i=0;i<idx.size();++i)std::cout<<"l0.LUT["<<idx[i]<<"]="<<l0.LUT[idx[i]]<<std::endl;std::cout<<"idx.size()="<<idx.size()<<std::endl;

float* tf=new float[l0.regvalmaxplusone](); //zero
int64_t dims[]={l0.regvalmaxplusone,1,1,1};
float centerf[]={90,-126,-72}; //dummy
float mmppixf[]={-0.7,0.7,0.7}; //dummy

while(std::getline(ifile,str,'\r')){
    if(str.empty() || !str.find("#") || str.length()==1)continue;

    std::istringstream iss(str);
    std::vector<std::string> vstr;

    while(std::getline(iss,str,','))vstr.push_back(str);

    std::vector<int>::iterator it;std::vector<size_t>::iterator vit; 
    for(it=idx.begin(),vit=vidx.begin();it!=idx.end();++it,++vit)tf[*it]=std::stof(vstr[*vit]);;

    str=vstr[0].substr(vstr[0].find_first_not_of(" \t\n"),vstr[0].find_last_not_of(" \t\n")-vstr[0].find_first_not_of(" \t\n")+1)+"_"+
        vstr[1].substr(vstr[1].find_first_not_of(" \t\n"),vstr[1].find_last_not_of(" \t\n")-vstr[1].find_first_not_of(" \t\n")+1)+"_"+
        vstr[6].substr(vstr[6].find_first_not_of(" \t\n"),vstr[6].find_last_not_of(" \t\n")-vstr[6].find_first_not_of(" \t\n")+1)+".nii";

    //if(!nifti_write((char*)str.c_str(),dims,(float*)NULL,(float*)NULL,tf))exit(-1);
    if(!nifti_write((char*)str.c_str(),dims,centerf,mmppixf,tf))exit(-1);

    std::string str2="gzip -f "+str;
    if(system(str2.c_str())==-1)std::cout<<"fidlError: "<<str2<<std::endl;else std::cout<<"Output written to "<<str+".gz"<<std::endl;
    }
}
