/* Copyright 1/19/21 Washington University.  All Rights Reserved.
   fidl_rms.cxx  $Revision: 1.1 $ */

#include <cstdio>
#include <iostream>
#include <cstring>
#include <cmath>
#include "fidl.h"
#include "dim_param2.h"
#include "mask.h"
//#include "ptr.h"
#include "filetype.h"
#include "write1.h"
#include "movement.h"

int main(int argc,char **argv){
char *maskf=NULL,*outf=NULL; //,*outroot=NULL;
int i,j,nfiles=0,nMARMSm=0,rWmsonly=0;
size_t m;
Files_Struct *files=NULL,*MARMSm=NULL;
Dim_Param2 *dp;
mask ms;
W1 *w1=NULL;
header h;
if(argc<5){
    std::cout<<argv[0]<<std::endl;
    std::cout<<"Root mean square is computed across input files. Files are assumed to be single frame images."<<std::endl;
    std::cout<<"    -files: images, concs or lists"<<std::endl;
    std::cout<<"    -mask:  Only voxels in the mask are analyzed."<<std::endl;

    //std::cout<<"    -out:   Output name."<<std::endl;
    //START210331
    //std::cout<<"    -outroot:   Output name."<<std::endl;
    std::cout<<"    -out:   Output name or root."<<std::endl;
    std::cout<<"            Without -Movement_AbsoluteRMS_mean use a name or root."<<std::endl;
    std::cout<<"            With -Movement_AbsoluteRMS_mean use just a root."<<std::endl;
    std::cout<<"            With -Movement_AbsoluteRMS_mean, the arithmetic mean has 'rms' appended to -out while the wieghted mean has 'rWms' appended."<<std::endl;
    //std::cout<<"            Use with -Movement_AbsoluteRMS_mean"<<std::endl;

    //START210331
    std::cout<<"  -Movement_AbsoluteRMS_mean: One for each -files. Weighted mean is calculated in quadrature across -files in addition to the arithmetic mean in quadrature."<<std::endl;
    std::cout<<"                              Movement_AbsoluteRMS_mean is inverted so that runs with less movement are weighted more heavily."<<std::endl;

    //START211109
    std::cout<<"  -rWmsonly: Only outupt rWms."<<std::endl;

    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfiles;
        if(!(files=read_files(nfiles,&argv[i+1])))exit(-1);
        i+=nfiles;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        maskf=argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        outf=argv[++i];
    
    //START210331
    //if(!strcmp(argv[i],"-outroot") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
    //    outroot=argv[++i];
    if(!strcmp(argv[i],"-Movement_AbsoluteRMS_mean") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nMARMSm;
        if(!(MARMSm=read_files(nMARMSm,&argv[i+1])))exit(-1);
        i+=nMARMSm;
        }
    
    //START211109
    if(!strcmp(argv[i],"-rWmsonly"))
        rWmsonly=1;

    }
if(!outf){std::cout<<"fidlError: Need to specify output name with -out"<<std::endl;exit(-1);}
//START210331
#if 0
if(!outf&&!outroot){
    if(!outf){
        std::cout<<argv[0]<<"\n"<<"    fidlError: Need to specify -out"<<std::endl;
        exit(-1);
        }
    if(!outroot){
        std::cout<<argv[0]<<"\n"<<"    fidlError: Need to specify -outroot"<<std::endl;
        exit(-1);
        }
    }
#endif

if(!(dp=dim_param2(files->nfiles,files->files)))exit(-1);
if(!dp->volall==-1){std::cout<<"fidlError: All files must have the same volume."<<std::endl;exit(-1);}
if(!(ms.get_mask(maskf,dp->volall,(int*)NULL,(LinearModel*)NULL,dp->volall)))exit(-1);

//START210331
movement mv;
double *wtmd=NULL,*temp_double2=NULL;
float *temp_float2=NULL;
if(MARMSm){
    if(MARMSm->nfiles!=files->nfiles){
        std::cout<<"fidlError: files->nfiles="<<files->nfiles<<" MARMSm->nfiles="<<MARMSm->nfiles<<" Must be equal."<<std::endl;
        exit(-1);
        }
    if(!mv.movement0(MARMSm))exit(-1);
    wtmd=new double[ms.lenbrain](); //empty parentheses zero intializes
    temp_double2=new double[ms.lenbrain](); //zero initialized
    temp_float2=new float[dp->volall];
    }

float* temp_float=new float[dp->volall];
double* temp_double=new double[ms.lenbrain](); //zero initialized
int* count=new int[ms.lenbrain](); //zero initialized


//for(m=0;m<(int)files->nfiles;m++){
//START210331
for(m=0;m<files->nfiles;m++){

    #if 0
    ptr p0;
    if(!p0.ptr0(files->files[m]))exit(-1);
    std::cout<<"Processing "<<files->files[m]<<std::endl;
    if(!p0.ptrstack(temp_float,0))exit(-1);
    #endif
    //START210331
    stack fs; 
    if(!(fs.stack1(files->files[m],temp_float)))exit(-1); 


    for(j=0;j<ms.lenbrain;j++){

        //std::cout<<temp_float[ms.brnidx[j]]<<" ";
        //std::cout<<temp_float[ms.brnidx[j]]<<" "<<abs(temp_float[ms.brnidx[j]])<<" "<<(float)UNSAMPLED_VOXEL<<std::endl;

        //if(temp_float[ms.brnidx[j]]!=(float)UNSAMPLED_VOXEL) {
        //START210331
        if(std::abs(temp_float[ms.brnidx[j]])>(float)UNSAMPLED_VOXEL&&!isnan(temp_float[ms.brnidx[j]])){


            temp_double[j] += (double)temp_float[ms.brnidx[j]]*(double)temp_float[ms.brnidx[j]];
            count[j]++;

            //START210331
            if(MARMSm){
                temp_double2[j] += mv.wts[m]*(double)temp_float[ms.brnidx[j]]*mv.wts[m]*(double)temp_float[ms.brnidx[j]];
                wtmd[j]+=mv.wts[m]*mv.wts[m];
                }

            }


        }
    //std::cout<<std::endl;

    //START210331
    //if(!p0.ptrfree())exit(-1);
    
    }


for(j=0;j<dp->volall;j++)temp_float[j]=0.;
for(j=0;j<ms.lenbrain;j++){
    if(count[j]){
        temp_float[ms.brnidx[j]]=(float)sqrt(temp_double[j]/(double)count[j]);
        }
    }

//START210331
if(MARMSm){
    for(j=0;j<ms.lenbrain;j++){ 
        if(count[j]){
            temp_float2[ms.brnidx[j]]=(float)sqrt(temp_double2[j]/wtmd[j]);
            }
        }
    }

#if 0
if(!(w1=write1_init()))exit(-1);
w1->filetype=dp->filetypeall;
w1->swapbytes=0;
w1->temp_float=temp_float;
if(!(h.header0(files->files[0])))exit(-1);
std::string str(outf);
if(dp->filetypeall==(int)IMG){
    w1->how_many=dp->volall;
    w1->ifh=h.ifh;
    }
else if(dp->filetypeall==(int)NIFTI){
    w1->file_hdr=files->files[0];
    if(str.find(".nii")==std::string::npos)
        str+="_rms.nii";
    else if(!str.compare(str.size()-2,2,"gz")){
        str=str.substr(0,str.size()-3);
        }
    }
if(!write1((char*)str.c_str(),w1))exit(-1);
if(dp->filetypeall==(int)NIFTI){
    std::string str2="gzip -f "+str;
    if(system(str2.c_str())==-1)
        std::cout<<"fidlError: "<<str2.c_str()<<std::endl;
    else
        str.append(".gz");
    }
std::cout<<"Output written to "<<str<<std::endl;
#endif
//START211109
if(!(w1=write1_init()))exit(-1);
w1->filetype=dp->filetypeall;
w1->swapbytes=0;
if(!rWmsonly){
    w1->temp_float=temp_float;
    if(!(h.header0(files->files[0])))exit(-1);
    std::string str(outf);
    if(dp->filetypeall==(int)IMG){
        w1->how_many=dp->volall;
        w1->ifh=h.ifh;
        }
    else if(dp->filetypeall==(int)NIFTI){
        w1->file_hdr=files->files[0];
        if(str.find(".nii")==std::string::npos)
            str+="_rms.nii";
        else if(!str.compare(str.size()-2,2,"gz")){
            str=str.substr(0,str.size()-3);
            }
        }
    if(!write1((char*)str.c_str(),w1))exit(-1);
    if(dp->filetypeall==(int)NIFTI){
        std::string str2="gzip -f "+str;
        if(system(str2.c_str())==-1)
            std::cout<<"fidlError: "<<str2.c_str()<<std::endl;
        else
            str.append(".gz");
        }
    std::cout<<"Output written to "<<str<<std::endl;
    }

if(MARMSm){
    w1->temp_float=temp_float2;

    //START211109
    w1->file_hdr=files->files[0];

    std::string str(outf);
    str+="_rWms.nii";
    if(!write1((char*)str.c_str(),w1))exit(-1);
    std::string str2="gzip -f "+str;
    if(system(str2.c_str())==-1)
        std::cout<<"fidlError: "<<str2.c_str()<<std::endl;
    else
        str.append(".gz");
    std::cout<<"Output written to "<<str<<std::endl;
    }
}
