/* Copyright 8/03/21 Washington University.  All Rights Reserved.
   fidl_PETcsv_stack2.cxx  $Revision: 1.1 $ */

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
//#include <map>
#include "lut.h"
#include "subs_nifti.h"

int main(int argc,char **argv){
char *csvf=NULL,*lutf=NULL;
int i,c1=0,c2=0,c3=0;
//size_t c0=0;
if(argc < 5) {
    std::cout<<argv[0]<<std::endl;
    std::cout<<"  -csv: Spreadsheet with PET values"<<std::endl;
    std::cout<<"  -lut: Freesurfer lookup table (eg /usr/local/pkg/freesurfer6.0/FreeSurferColorLUT.txt)"<<std::endl;
    //std::cout<<"  -col_region_names_start: First column is 1."<<std::endl;
    std::cout<<"  -col_RedCapID:           First column is 1. Vlassenko label."<<std::endl;
    std::cout<<"  -col_FreeSurferID:       First column is 1. Vlassenko label."<<std::endl;
    std::cout<<"  -col_Modality:           First column is 1. Vlassenko label."<<std::endl;
    //std::cout<<"  Ex. /home/usr/mcavoy/FIDL/build/fidl_PETcsv_stack2 -csv DF-01_final_official_2021_07_29_mm210803.csv -lut /usr/local/pkg/freesurfer6.0/FreeSurferColorLUT.txt -col_region_names_start 21 -col_RedCapID 1 -col_FreeSurferID 5 -col_Modality 20"<<std::endl;
    std::cout<<"  Ex. /home/usr/mcavoy/FIDL/build/fidl_PETcsv_stack2 -csv DF-01_final_official_2021_07_29_mm210803.csv -lut /usr/local/pkg/freesurfer6.0/FreeSurferColorLUT.txt -col_RedCapID 1 -col_FreeSurferID 5 -col_Modality 20"<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-csv") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        csvf=argv[++i];
    if(!strcmp(argv[i],"-lut") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        lutf=argv[++i];
    //if(!strcmp(argv[i],"-col_region_names_start") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
    //    sscanf(argv[++i],"%zu",&c0); 
    if(!strcmp(argv[i],"-col_RedCapID") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        sscanf(argv[++i],"%d",&c1); 
    if(!strcmp(argv[i],"-col_FreeSurferID") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        sscanf(argv[++i],"%d",&c2); 
    if(!strcmp(argv[i],"-col_Modality") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        sscanf(argv[++i],"%d",&c3); 
    }
if(!csvf){std::cout<<"fidlError: -csv not specified"<<std::endl;exit(-1);}
if(!lutf){std::cout<<"fidlError: -lut not specified"<<std::endl;exit(-1);}
//if(c0<1){std::cout<<"fidlError: -col_region_names_start not specified"<<std::endl;exit(-1);};--c0;
if(c1<1){std::cout<<"fidlError: -col_RedCapID not specified"<<std::endl;exit(-1);};--c1;
if(c2<1){std::cout<<"fidlError: -col_FreeSurferID not specified"<<std::endl;exit(-1);};--c2;
if(c3<1){std::cout<<"fidlError: -col_Modality not specified"<<std::endl;exit(-1);};--c3;

lut l0;
l0.lut2(lutf);
//for(std::map<int,std::string>::iterator it=l0.LUT.begin();it!=l0.LUT.end();++it)std::cout<<it->first<<" "<<it->second<<std::endl;

std::ifstream ifile(csvf);
std::string str;
std::getline(ifile,str,'\r');
//std::cout<<str<<"END"<<std::endl;

std::istringstream iss(str);
std::vector<std::string> vstr;

//while(std::getline(iss,str,','))vstr.push_back(str);
while(std::getline(iss,str)){ //stackoverflow.com/questions/35639083/c-csv-line-with-commas-and-strings-within-double-quotes

    const char* mystart=str.c_str();
    bool instring{false};
    for(const char* p=mystart;*p;p++){
        if(*p=='"')
            instring=!instring;
        else if(*p==','&&!instring){
            vstr.push_back(std::string(mystart,p-mystart));
            mystart=p+1;
            }
        }
    vstr.push_back(std::string(mystart));
    }

//for(size_t i=0;i<vstr.size();++i)std::cout<<vstr[i]<<"END"<<std::endl;std::cout<<"vstr.size()="<<vstr.size()<<std::endl;
//for(size_t i=0;i<vstr.size();++i)std::cout<<"vstr["<<i<<"]="<<vstr[i]<<std::endl;std::cout<<"vstr.size()="<<vstr.size()<<std::endl;

std::vector<int> idx;std::vector<size_t> vidx;

//for(size_t i=7;i<vstr.size();++i){
//for(size_t i=c0;i<vstr.size();++i){
for(size_t i=0;i<vstr.size();++i){

    //for(std::map<int,std::string>::iterator it=l0.LUT.begin();it!=l0.LUT.end();++it){
    for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it){

        if(!vstr[i].compare(it->second)){
            idx.push_back(it->first); 
            vidx.push_back(i);
            break;
            }
        }
    //std::cout<<"\n"<<std::endl;
    }
//for(size_t i=0;i<idx.size();++i)std::cout<<"l0.LUT["<<idx[i]<<"]="<<l0.LUT[idx[i]]<<std::endl;std::cout<<"idx.size()="<<idx.size()<<std::endl;
//for(size_t i=0;i<idx.size();++i)std::cout<<"l0.LUT["<<idx[i]<<"]="<<l0.LUT[idx[i]]<<" vidx["<<i<<"]="<<vidx[i]<<std::endl;
//std::cout<<"idx.size()="<<idx.size()<<std::endl;
//exit(-1);

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

    //str=vstr[0].substr(vstr[0].find_first_not_of(" \t\n"),vstr[0].find_last_not_of(" \t\n")-vstr[0].find_first_not_of(" \t\n")+1)+"_"+
    //    vstr[1].substr(vstr[1].find_first_not_of(" \t\n"),vstr[1].find_last_not_of(" \t\n")-vstr[1].find_first_not_of(" \t\n")+1)+"_"+
    //    vstr[6].substr(vstr[6].find_first_not_of(" \t\n"),vstr[6].find_last_not_of(" \t\n")-vstr[6].find_first_not_of(" \t\n")+1)+".nii";
    str=vstr[c1].substr(vstr[c1].find_first_not_of(" \t\n"),vstr[c1].find_last_not_of(" \t\n")-vstr[c1].find_first_not_of(" \t\n")+1)+"_"+
        vstr[c2].substr(vstr[c2].find_first_not_of(" \t\n"),vstr[c2].find_last_not_of(" \t\n")-vstr[c2].find_first_not_of(" \t\n")+1)+"_"+
        vstr[c3].substr(vstr[c3].find_first_not_of(" \t\n"),vstr[c3].find_last_not_of(" \t\n")-vstr[c3].find_first_not_of(" \t\n")+1)+".nii";

    //if(!nifti_write((char*)str.c_str(),dims,(float*)NULL,(float*)NULL,tf))exit(-1);
    if(!nifti_write((char*)str.c_str(),dims,centerf,mmppixf,tf))exit(-1);

    std::string str2="gzip -f "+str;
    if(system(str2.c_str())==-1)std::cout<<"fidlError: "<<str2<<std::endl;else std::cout<<"Output written to "<<str+".gz"<<std::endl;
    }
}
