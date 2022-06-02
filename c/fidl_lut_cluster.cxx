/* Copyright 2/25/21 Washington University.  All Rights Reserved.
   fidl_lut_cluster.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <cstring>
#include <unordered_map>
#include <cmath>

//#include "lut.h"
//START211214
#include "lut2.h"

#include "files_struct.h"
#include "stack.h"
#include "constants.h"
#include "subs_nifti.h"

int main(int argc,char **argv){
char *lutf=NULL,*wmparcf=NULL,*out=NULL;
int i,j,k,nclusterf=0,clusteri=-1,val=0;
float *wmparc=NULL;
Files_Struct *clusterf=NULL;
if(argc<3){
    std::cout<<"Combine freesurfer regions into a cluster. "<<std::endl;
    std::cout<<"  -lut:     Lookup table. Ex. $FREESURFER_HOME/FreeSurferColorLUT.txt"<<std::endl;
    std::cout<<"  -cluster: One file per cluster. List of freesurfer regions. Name of file becomes name of cluster."<<std::endl;
    std::cout<<"            Use *.list or *.conc so fidl knows to read the region names."<<std::endl;
    std::cout<<"  -wmparc:  wmparc.nii.gz  float from HCP structural pipeline"<<std::endl;

    //START210604
    std::cout<<"  -val:     All clusters will have this nonzero INTEGER value (eg 1). Default is integer valued starting at 1."<<std::endl;


    std::cout<<"  -out:     Name of output file."<<std::endl;
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

    //START210604
    if(!strcmp(argv[i],"-val") && argc > i+1)
        val=std::stoi(argv[++i],nullptr);

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
if(!out){
    std::cout<<"fidlError: Need to specify -out"<<std::endl;
    exit(-1);
    }
//std::cout<<"clusterf->nfileseach=";for(i=0;i<nclusterf;++i)std::cout<<" "<<clusterf->nfileseach[i];std::cout<<std::endl;

lut l0;
l0.lut2(lutf);

std::unordered_map<int,int> superbird;
for(k=i=0;i<nclusterf;++i){
    //std::cout<<argv[clusteri+i]<<std::endl;
    std::string str(argv[clusteri+i]);
    str=str.substr(str.find_last_of("/")+1,str.find_last_of(".")-str.find_last_of("/")-1);
    //std::cout<<str<<std::endl;
    for(j=0;j<clusterf->nfileseach[i];++j,++k){
        //for(std::map<int,std::string>::iterator it=l0.LUT.begin();it!=l0.LUT.end();++it){

        //std::map<int,std::string>::iterator it; 
        //for(it=l0.LUT.begin();it!=l0.LUT.end();++it){
        //START210604
        auto it=l0.LUT.begin(); 
        for(;it!=l0.LUT.end();++it){

            //std::cout<<"    clusterf->files["<<k<<"]="<<clusterf->files[k]<<" "<<it->second<<std::endl;

            if(!strcmp(it->second.c_str(),clusterf->files[k])){
                //std::cout<<"    clusterf->files["<<k<<"]="<<clusterf->files[k]<<" "<<it->first<<std::endl;

                //superbird[it->first]=i+1;
                //START210604
                superbird[it->first]=!val?i+1:val;

                break;
                }
            }
        if(it==l0.LUT.end()){
            std::cout<<"****************** "<<clusterf->files[k]<<" not found ******************"<<std::endl;
            exit(-1);
            }
        }
    }
//for(std::unordered_map<int,int>::iterator it=superbird.begin();it!=superbird.end();++it)std::cout<<it->first<<" "<<it->second<<std::endl;
//std::cout<<"superbird.size()="<<superbird.size()<<std::endl;

stack fs1;
if(!(wmparc=fs1.stack0(wmparcf)))exit(-1);
float* tf=new float[fs1.vol]();
for(i=0;i<fs1.vol;++i){
    if(std::abs(wmparc[i])>(float)UNSAMPLED_VOXEL&&!isnan(wmparc[i])){
        auto it=superbird.find((int)wmparc[i]);
        if(it!=superbird.end())tf[i]=it->second;
        }
    }
std::string str(out);
std::size_t found;
if((found=str.find(".gz"))!=std::string::npos)str.erase(found,std::string::npos);
if(!nifti_write2((char*)str.c_str(),wmparcf,tf))exit(-1);
std::string str2="gzip -f "+str;
if(system(str2.c_str())==-1)std::cout<<"fidlError: "<<str2<<std::endl;else std::cout<<"Output written to "<<str+".gz"<<std::endl;
} 
