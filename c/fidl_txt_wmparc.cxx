/* Copyright 10/18/21 Washington University.  All Rights Reserved.
   fidl_txt_wmparc.cxx  $Revision: 1.1 $ */

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

//START211028
#include <cctype>

#include "lut2.h"
#include "subs_nifti.h"
#include "stack.h"
#include "constants.h"

int main(int argc,char **argv){
char *csvf=NULL,*lutf=NULL,*wmparcf=NULL,*outf=NULL;
int i,c1=0,c2=0;

//if(argc < 13){
//START211028
if(argc<11){

    std::cerr<<argv[0]<<std::endl;
    std::cerr<<"Copy values from text file into wmparc."<<std::endl;
    std::cerr<<"  -txt:       e.g. anova_region_results_short.txt"<<std::endl;
    std::cerr<<"  -lut:       e.g. /usr/local/pkg/freesurfer6.0/FreeSurferColorLUT.txt"<<std::endl;
    std::cerr<<"              Need not be an actual freesurfer lookuptable. Just needs to follow the freesurfer format."<<std::endl;
    std::cerr<<"              Not needed if -col_label identifies numbers in the wmparc."<<std::endl;
    std::cerr<<"  -wmparc     e.g. wmparc.nii.gz"<<std::endl;
    std::cerr<<"              Need not be an actual freesurfer wmparc. Just needs to have voxels with numbers that match the -lut."<<std::endl;
    std::cerr<<"  -col_label  First column is 1. Column with the -lut label or number."<<std::endl;
    std::cerr<<"  -col_number First column is 1. Column with the number to be inserted into the wmparc."<<std::endl;
    std::cerr<<"  -out        e.g. *.nii.gz"<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-txt") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        csvf=argv[++i];
    if(!strcmp(argv[i],"-lut") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        lutf=argv[++i];
    if(!strcmp(argv[i],"-wmparc") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        wmparcf=argv[++i];
    if(!strcmp(argv[i],"-col_label") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        sscanf(argv[++i],"%d",&c1);
    if(!strcmp(argv[i],"-col_number") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        sscanf(argv[++i],"%d",&c2);
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        outf=argv[++i];
    }
if(!csvf){std::cout<<"fidlError: -txt not specified"<<std::endl;exit(-1);}

//START211028
//if(!lutf){std::cout<<"fidlError: -lut not specified"<<std::endl;exit(-1);}
//START211028
if(!lutf){std::cout<<"fidlWarning: -lut not specified"<<std::endl;}

if(!wmparcf){std::cout<<"fidlError: -wmparc not specified"<<std::endl;exit(-1);}
if(c1<1){std::cout<<"fidlError: -col_label not specified"<<std::endl;exit(-1);};--c1;
if(c2<1){std::cout<<"fidlError: -col_number not specified"<<std::endl;exit(-1);};--c2;
if(!outf){std::cout<<"fidlError: -out not specified"<<std::endl;exit(-1);}
size_t cmax=c1<c2?c2:c1;

lut l0;

//l0.lut2(lutf);
//START211028
if(lutf)l0.lut2(lutf);

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
//for(size_t i=0;i<vstr.size();++i)std::cout<<"vstr["<<i<<"]="<<vstr[i]<<std::endl;std::cout<<"vstr.size()="<<vstr.size()<<std::endl;

//std::vector<int> idx;std::vector<size_t> vidx;
//std::vector<int> idx;std::vector<float> vc2;
std::unordered_map<int,float> vc2;
for(size_t i=0;i<vstr.size();++i){
    //std::cout<<"vstr["<<i<<"]="<<vstr[i]<<std::endl;

    std::replace(vstr[i].begin(),vstr[i].end(),',',' ');
    //std::cout<<"vstr["<<i<<"]="<<vstr[i]<<std::endl;

    std::istringstream iss(vstr[i]);
    std::vector<std::string> wstr;
    for(;iss>>str;)wstr.push_back(str); 

    //for(size_t j=0;j<wstr.size();++j)std::cout<<"wstr["<<j<<"]="<<wstr[j]<<std::endl;std::cout<<"wstr.size()="<<wstr.size()<<std::endl;

    #if 0
    if(wstr.size()>cmax){
        auto it=l0.LUT.begin();
        for(;it!=l0.LUT.end();++it){
            if(!wstr[c1].compare(it->second)){
                vc2[it->first]=std::stof(wstr[c2]); 
                break;
                }
            }
        if(it==l0.LUT.end()){
            std::cout<<wstr[c1]<<" not found!"<<std::endl;
            }
        }
    #endif
    //START211028
    if(wstr.size()>cmax){
        if(!isdigit(*wstr[c1].c_str())){
            auto it=l0.LUT.begin();
            for(;it!=l0.LUT.end();++it){
                if(!wstr[c1].compare(it->second)){
                    vc2[it->first]=std::stof(wstr[c2]); 
                    break;
                    }
                }
            if(it==l0.LUT.end()){
                std::cout<<wstr[c1]<<" not found!"<<std::endl;
                }
            }
        else{
            vc2[std::stoi(wstr[c1])]=std::stof(wstr[c2]); 
            }
        }
    
    
    }
//for(size_t i=0;i<idx.size();++i)std::cout<<"l0.LUT["<<idx[i]<<"]="<<l0.LUT[idx[i]]<<" vidx["<<i<<"]="<<vidx[i]<<std::endl;
//for(size_t i=0;i<idx.size();++i)std::cout<<"l0.LUT["<<idx[i]<<"]="<<l0.LUT[idx[i]]<<" vc2["<<i<<"]="<<vc2[i]<<std::endl;std::cout<<"idx.size()="<<idx.size()<<std::endl;
//for(auto it=vc2.begin();it!=vc2.end();++it)std::cout<<"l0.LUT["<<it->first<<"]="<<l0.LUT[it->first]<<" vc2["<<it->first<<"]="<<vc2[it->first]<<std::endl;std::cout<<"vc2.size()="<<vc2.size()<<std::endl;
//for(auto it=vc2.begin();it!=vc2.end();++it)std::cout<<it->first<<" "<<it->second<<std::endl;std::cout<<"vc2.size()="<<vc2.size()<<std::endl;

#if 0
stack s0;
float *man=NULL;
if(!(man=s0.stack0(wmparcf)))exit(-1);
std::vector<float> out(s0.vol);
for(i=0;i<s0.vol;++i){
    if(std::abs(man[i])>(float)UNSAMPLED_VOXEL&&!isnan(man[i])){
        auto it=vc2.find((int)man[i]);
        if(it!=vc2.end()){
            out[i]=it->second;
            //std::cout<<"out["<<i<<"]="<<out[i]<<std::endl;
            } 
        }
    }
#endif
//START211019
stack s0;
int *wmparc=NULL;
if(!(wmparc=s0.stack0int(wmparcf)))exit(-1);
std::vector<float> out(s0.vol);
for(i=0;i<s0.vol;++i){
    if(wmparc[i]){
        auto it=vc2.find(wmparc[i]);
        if(it!=vc2.end()){
            out[i]=it->second;
            //std::cout<<"out["<<i<<"]="<<out[i]<<std::endl;
            } 
        }
    }




//std::cout<<"outf="<<outf<<std::endl;
str.assign(outf);
if(!str.compare(str.size()-2,2,"gz")){
    str=str.substr(0,str.size()-3);
    }

//std::cout<<"here0 s0.dim64[0]="<<s0.dim64[0]<<std::endl;
//std::cout<<"here0 s0.dim64[1]="<<s0.dim64[1]<<std::endl;
//std::cout<<"here0 s0.dim64[2]="<<s0.dim64[2]<<std::endl;
//std::cout<<"here0 s0.dim64[3]="<<s0.dim64[3]<<std::endl;

//if(!nifti_write2((char*)str.c_str(),wmparcf,out.data()))exit(-1); //wmparc is some type of int (unsigned long?)
if(!nifti_write((char*)str.c_str(),s0.dim64,s0.centerf,s0.mmppixf,out.data()))exit(-1);

//std::cout<<"here1"<<std::endl;

std::string str3="gzip -f "+str;
if(system(str3.c_str())==-1)std::cout<<"fidlError: "<<str3<<std::endl;else std::cout<<"Output written to "<<str+".gz"<<std::endl;
}
