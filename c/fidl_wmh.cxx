/* Copyright 3/7/22 Washington University.  All Rights Reserved.
   fidl_wmh.cxx  $Revision: 1.1 $ */

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

//#include <algorithm>
//#include <map>
//include "lut.h"
//#include "fidl.h"
#include "mask.h"
#include "subs_nifti.h"

int main(int argc,char **argv){
char *csvf=NULL;
int i,lctxt=0;
if(argc < 3) {
    std::cout<<argv[0]<<std::endl;
    std::cout<<"  -csv: Col 1 identifies the wmparc, col 2 is wmh vol."<<std::endl;
    std::cout<<"        Ex. /data/nil-bluearc/vlassenko/HCP/sub/108002/180306RES/vglab/T1w/wmparc.nii.gz,8336"<<std::endl;
    std::cout<<"  Output is a binary float file. Indended to be used as the dep var in glm."<<std::endl;
    std::cout<<"  Ex. fidl_wmh -csv 220217RESg123_WMH.csv"<<std::endl;
    std::cout<<std::endl;
    std::cout<<"  -txt  Output is a text file."<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-csv") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        csvf=argv[++i];
    if(!strcmp(argv[i],"-txt"))
        lctxt = 1;
    }
if(!csvf){std::cout<<"fidlError: -csv not specified"<<std::endl;exit(-1);}

std::ifstream ifile(csvf);
std::string str,str2;
std::vector<float> f0;
for(i=0;std::getline(ifile,str);++i){
    if(str.empty() || !str.find("#"))continue;
    std::cout<<"str="<<str<<std::endl;

    std::istringstream iss(str);
    std::vector<std::string> vstr;
    //while(std::getline(iss,str,','))vstr.push_back(str);
    while(std::getline(iss,str2,','))vstr.push_back(str2);
    std::cout<<"vstr.size()="<<vstr.size()<<" i="<<i<<std::endl;

    if(vstr.size()<2){
        std::cout<<"line "<<i+1<<" "<<str<<" Has "<<vstr.size()<<" elements. Skipping."<<std::endl;
        continue;
        }

    mask ms;
    if(!(ms.read_mask((char*)vstr[0].c_str(),(LinearModel*)NULL))){
        std::cout<<"fidlError: "<<argv[0]<<" read_mask "<<vstr[0]<<std::endl;
        exit(-1);
        }
    //wmh are from 1 mm voxels, while wmparc is from 0.7 mm voxels, so need to convert wmparc to vol
    float tf=(float)(std::stod(vstr[1])/(double)ms.lenbrain/(double)ms.voxel_size[0]/(double)ms.voxel_size[1]/(double)ms.voxel_size[2]*100.);
    std::cout<<vstr[1]<<" "<<ms.lenbrain<<" "<<ms.voxel_size[0]<<" "<<tf<<std::endl;
     
    f0.push_back(tf);
    }
//std::cout<<"f0.size()="<<f0.size()<<std::endl;

str.assign(csvf);
std::size_t found;
if((found=str.find_last_of("."))!=std::string::npos)str.erase(found,std::string::npos);
str=str.substr(str.find_last_of("/")+1,std::string::npos)+(!lctxt?".nii":".txt");
if(!lctxt){
    int64_t dims[]={1,1,1,(int64_t)f0.size()};
    float centerf[]={90,-126,-72}; //dummy
    float mmppixf[]={-0.7,0.7,0.7}; //dummy

    #if 0
    str.assign(csvf);
    std::size_t found;
    if((found=str.find_last_of("."))!=std::string::npos)str.erase(found,std::string::npos);
    str=str.substr(str.find_last_of("/")+1,std::string::npos)+".nii";
    #endif

    if(!nifti_write((char*)str.c_str(),dims,centerf,mmppixf,f0.data()))exit(-1);
    str2="gzip -f "+str;
    if(system(str2.c_str())==-1)std::cout<<"fidlError: "<<str2<<std::endl;else std::cout<<"Output written to "<<str+".gz"<<std::endl;
    }
else{
    std::ofstream ofile(str); 
    if(!ofile.is_open()){std::cout<<"fidlError: cannot open "<<str<<std::endl;exit(-1);}
    for(size_t i=0;i<f0.size();++i)ofile<<f0[i]<<std::endl;
    ofile.close();
    std::cout<<"Output written to "<<str<<std::endl;
    }
}
