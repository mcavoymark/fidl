/* Copyright 3/12/19 Washington University.  All Rights Reserved.
   fidl_tc_ss_cull.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include "fidl.h"
#include "lut.h"
#include "region.h"

#if 0
class cull{
    public:
        float vol,sd; 
        cull(){vol=0;sd=0.;};
        cull(float vol0,float sd0){vol=vol0;sd=sd0;};
        //virtual ~cull(){};
    };
#endif
//START200401
class cull{
    public:
        float vol,sd;
        int nvox;
        cull(){vol=0;sd=0.;nvox=0;};
        cull(float vol0,float sd0,int nvox0){vol=vol0;sd=sd0;nvox=nvox0;};
    };

void read0(char* file,std::map<std::string,cull>& sub){
    std::ifstream ifile(file);
    std::string str,REGION,name,vox,mm3,tag;
    float vol,sd;
    int nvox;
    while(getline(ifile,str)){
        if(str.empty() || !str.find("#") || str.find("REGION:"))continue;
        std::istringstream iss(str);
        iss>>REGION>>name>>nvox>>vox>>vol>>mm3;
        //std::cout<<"nvox="<<nvox<<std::endl;
        if(nvox){
            getline(ifile,str);
            std::istringstream iss(str);
            iss>>tag>>sd;
            cull c0(vol,sd,nvox);
            sub[name]=c0;
            } 
        }
    ifile.close();
    }


int main(int argc,char **argv){
char *lutf=NULL,*outf=NULL;
int i,j,nfiles=0,nbf=0;
Files_Struct *files=NULL,*bf=NULL;
if(argc<7){
    std::cerr<<" -files: Text files in a format like fidl_tc_ss puts out. Can be a list."<<std::endl;
    std::cerr<<" -brainmasks: One for each file. Can be a list.  Ex. brainmask_fs.nii.gz"<<std::endl;
    std::cerr<<" -lut:   Lookup table. Ex. $FREESURFER_HOME/FreeSurferColorLUT.txt "<<std::endl;
    std::cerr<<" -out:   Name of output file. Extension is stripped off; only the root is used."<<std::endl;
    std::cerr<<"         Outputs include vol, normvol and sd."<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;++i){
    if(!strcmp(argv[i],"-files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfiles;
        if(!(files=read_files(nfiles,&argv[i+1])))exit(-1);
        i+=nfiles;
        }
    if(!strcmp(argv[i],"-brainmasks") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nbf;
        if(!(bf=read_files(nbf,&argv[i+1])))exit(-1);
        i+=nbf;
        }
    if(!strcmp(argv[i],"-lut")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            lutf = argv[++i];
            }
        else {
            std::cerr<<"fidlError: No lookup table specified after -lut option. Abort!"<<std::endl;
            exit(-1);
            }
        }
    if(!strcmp(argv[i],"-out") && argc > i+1)
        outf = argv[++i];
    }

std::vector<Regions*> reg;
if(nbf){
    if(bf->nfiles!=files->nfiles){
        std::cerr<<"fidlError: bf->nfiles="<<bf->nfiles<<" files->nfiles="<<files->nfiles<<" Must be equal. Abort!"<<std::endl;exit(-1);
        }

    #if 0
    if(!(reg=(Regions**)malloc(sizeof*reg*bf->nfiles))) {
        std::cerr<<"fidlError: Unable to malloc reg"<<std::endl;
        exit(-1);
        }
    maskidx=new int[vol];
    #endif
    //reg=new Regions* [bf->nfiles];
    reg.resize(bf->nfiles); 


    for(size_t i=0;i<bf->nfiles;++i)if(!(reg[i]=get_reg(bf->files[i],0,(float*)NULL,0,(char**)NULL,0,0,0,(int*)NULL,(char*)NULL)))exit(-1);
    }

cull c0;
std::vector<std::map<std::string,cull>> sub(files->nfiles);
for(size_t i=0;i<files->nfiles;++i)read0(files->files[i],sub[i]);

for(size_t i=0;i<files->nfiles;++i)std::cout<<"here1 sub["<<i<<"].size()="<<sub[i].size()<<std::endl;


lut l0;
l0.lut2(lutf);
#if 0
i=0;
for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it){
    std::cout<<it->first<<" => "<<it->second<<std::endl;
    ++i;
    }
std::cout<<"l0.LUT.size()="<<l0.LUT.size()<<" i="<<i<<std::endl;
#endif
std::cout<<"here2 l0.LUT.size()="<<l0.LUT.size()<<std::endl;

//if(sub[0].find("Left-Cerebral-White-Matter")!=sub[0].end())std::cout<<"found it"<<std::endl;
#if 0
i=0;
for(auto it=sub[0].begin();it!=sub[0].end();++it){
    std::cout<<it->first<<std::endl;
    ++i;
    }
std::cout<<"sub[0].size()="<<sub[0].size()<<" i="<<i<<std::endl;
#endif

#if 0
size_t i1;
for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it){
    for(i1=0;i1<files->nfiles;++i1){
        if(sub[i1]->find(it->second)!=sub[i1]->end())break;
        }
    if(i1==files->nfiles)l0.LUT.erase(it);
    }
#endif

size_t i1;
for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it){
    for(i1=0;i1<files->nfiles;++i1){
        if(sub[i1].find(it->second)!=sub[i1].end())break;
        }
    if(i1==files->nfiles)l0.LUT.erase(it);
    }
#if 0
for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it){
    std::cout<<it->first<<" => "<<it->second<<std::endl;
    }
#endif
std::cout<<"here3 l0.LUT.size()="<<l0.LUT.size()<<std::endl;


//std::vector<std::string> names(files->nfiles);
//for(i1=0;i1<files->nfiles;++i1)names[i1].assign(files->files[i1];
//std::vector<std::string> names(files->files,files->files+files->nfiles);
//for(std::string x : names)std::cout<<x<<endl;
//for(auto it=names.begin();it!=names.end();++it)std::cout<<*it<<std::endl;

//for(auto it=names.begin();it!=names.end();++it){
//    std::cout<<it.find_last_of("/")<<std::endl;
//    }
#if 0
std::vector<std::string> names(files->files,files->files+files->nfiles);
for(auto it=names.begin();it!=names.end();++it)std::cout<<*it<<std::endl;
for(i1=0;i1<files->nfiles;++i1){
    std::cout<<names[i1].find_last_of("/")<<std::endl;
    std::cout<<names[i1].find_first_of("_fidl")<<std::endl;
    }
#endif
 
#if 0
std::vector<std::string> names(files->nfiles);
for(i1=0;i1<files->nfiles;++i1){
    std::string str(files->files[i1]);
    names[i1]=str.substr(str.find_last_of("/")+1,str.find("_fidl")-str.find_last_of("/")-1);
    std::cout<<names[i1]<<std::endl;
    }
#endif


std::string str(outf);
std::string volf=str.substr(0,str.find_last_of(".")-1);
volf.append("_vol.txt");
std::cout<<"volf = "<<volf<<std::endl;
std::string sdf=str.substr(0,str.find_last_of(".")-1);

//sdf.append("_sd.txt");
//START200418
std::string msg;
if(strstr(files->files[0],"sdTR")){
    sdf.append("_sdTR.txt");
    msg.assign("Standard deviations");
    }
else if(strstr(files->files[0],"meanTR")){
    sdf.append("_meanTR.txt");
    msg.assign("Means");
    }

std::cout<<"sdf = "<<sdf<<std::endl;
#if 0
if(nbf){
    std::string volnormf=str.substr(0,str.find_last_of(".")-1);
    volnormf.append("_volnorm.txt");
    std::cout<<"volnormf = "<<volnormf<<std::endl;
    }
#endif

std::ofstream ofs(volf);
ofs<<"subject";for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it)ofs<<"\t"<<it->second;ofs<<std::endl;
std::ofstream ofs1(sdf);
ofs1<<"subject";for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it)ofs1<<"\t"<<it->second;ofs1<<std::endl;
#if 0
if(nbf){
    std::ofstream ofs2(volnormf);
    ofs2<<"subject";for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it)ofs2<<"\t"<<it->second;ofs2<<std::endl;
    }
#endif

std::map<std::string,cull>::iterator it0;
std::vector<std::string> root(files->nfiles);

for(i1=0;i1<files->nfiles;++i1){
    std::string str(files->files[i1]);
    root[i1]=str.substr(str.find_last_of("/")+1,str.find("_fidl")-str.find_last_of("/")-1);
    ofs<<root[i1];ofs1<<root[i1];
    for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it){
        ofs<<"\t";ofs1<<"\t";
        if((it0=sub[i1].find(it->second))!=sub[i1].end()){
            ofs<<it0->second.vol;ofs1<<it0->second.sd;
            }
        }
    ofs<<std::endl;ofs1<<std::endl;
    }
ofs.close();ofs1.close();
std::cout<<"Volumes written to "<<volf<<std::endl;

//std::cout<<"Standard deviations written to "<<sdf<<std::endl;
//START200418
std::cout<<msg<<" written to "<<sdf<<std::endl;

if(nbf){
    std::string volnormf=str.substr(0,str.find_last_of(".")-1);
    volnormf.append("_volnorm.txt");
    std::cout<<"volnormf = "<<volnormf<<std::endl;
    std::ofstream ofs2(volnormf);
    ofs2<<"subject";for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it)ofs2<<"\t"<<it->second;ofs2<<std::endl;
    for(i1=0;i1<files->nfiles;++i1){
        ofs2<<root[i1];
        for(auto it=l0.LUT.begin();it!=l0.LUT.end();++it){
            ofs2<<"\t";
            if((it0=sub[i1].find(it->second))!=sub[i1].end()){
                ofs2<<(double)it0->second.nvox/(double)reg[i1]->nvoxels*100.;
                }
            }
        ofs2<<std::endl;
        }
    ofs2.close();
    std::cout<<"Normalized volumes as percent of brainmask written to "<<volnormf<<std::endl;
    }
}
