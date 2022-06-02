/* Copyright 4/29/21 Washington University.  All Rights Reserved.
   fidl_lfcd2.cxx  $Revision: 1.1 $ */

#include <iostream>
#include <cstring>
#include <cmath>
#include "files_struct.h"
#include "mask.h"
#include "dim_param2.h"

//#include "lfcd.h"
//START210429
#include "lfcd2.h"

#include "subs_nifti.h"
#include "movement.h"

int main(int argc,char **argv){
char *maskf=NULL,*omean=NULL;
int i,j,nfiles=0,nMARMSm=0,meanonly=0; //k
double thresh=0.;
Files_Struct *files=NULL,*MARMSm=NULL;
Dim_Param2 *dp;
mask ms;

if(argc < 5) {
    std::cout<<argv[0]<<std::endl;
    std::cout<<"  -files:        .img's, .nii's, .nii.gz's or a conc or list."<<std::endl;
    std::cout<<"  -mask:         Local connectivity density is computed for only voxels in the -mask"<<std::endl;
    std::cout<<"  -thresh:       Typically 0.6. A pearson correlation greater than -thresh defines an edge."<<std::endl;
    std::cout<<"  -omean:        Output name or root of mean image computed voxelwise across all -files."<<std::endl;
    std::cout<<"  -Movement_AbsoluteRMS_mean: One for each -files. Weighted mean is calculated across -files in addition to the arithmetic mean."<<std::endl;
    std::cout<<"                              Movement_AbsoluteRMS_mean is inverted so that runs with less movement are weighted more heavily."<<std::endl;
    std::cout<<"  -meanonly      Only output the mean image."<<std::endl;
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
    if(!strcmp(argv[i],"-thresh") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        thresh=strtod(argv[++i],NULL);
    if(!strcmp(argv[i],"-omean") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        omean=argv[++i];
    if(!strcmp(argv[i],"-Movement_AbsoluteRMS_mean") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nMARMSm;
        if(!(MARMSm=read_files(nMARMSm,&argv[i+1])))exit(-1);
        i+=nMARMSm;
        }
    if(!strcmp(argv[i],"-meanonly"))
        meanonly=1;
    }

if(!(dp=dim_param2(files->nfiles,files->files)))exit(-1);
if(dp->volall==-1) {
    std::cout<<"fidlError: All -files must be of the same dimension."<<std::endl;
    exit(-1);
    }
if(!(ms.get_mask(maskf,dp->volall,(int*)NULL,(LinearModel*)NULL,dp->volall)))exit(-1);
if(dp->volall!=ms.vol){
    std::cout<<"fidlError: dp->volall="<<dp->volall<<" ms.vol="<<ms.vol<<" Must be equal."<<std::endl;
    exit(-1);
    }

double *wmeanloglFCD=NULL,*wmeanlFCDpearson=NULL,*wtmd=NULL;
movement mv;
if(MARMSm){
    if(MARMSm->nfiles!=files->nfiles){
        std::cout<<"fidlError: files->nfiles="<<files->nfiles<<" MARMSm->nfiles="<<MARMSm->nfiles<<" Must be equal."<<std::endl;
        exit(-1);
        }
    if(!mv.movement0(MARMSm))exit(-1);
    //wtmd=new double[ms.lenbrain](); //empty parentheses zero intializes
    //wmeanloglFCD=new double[ms.lenbrain](); //zero
    //wmeanlFCDpearson=new double[ms.lenbrain](); //zero
    wtmd=new double[ms.vol](); //empty parentheses zero intializes
    wmeanloglFCD=new double[ms.vol](); //zero
    wmeanlFCDpearson=new double[ms.vol](); //zero
    }

/* implement r(x,y)= (sum(xy) - n meanx meany) / ( (n-1) sd(x) sd(y) ) 
                 mean = sum(x) / n
                 sd = sqrt( sum( (x - meanx)^2 ) /(n-1) )
*/

std::cout<<"dp->lenvol_max="<<dp->lenvol_max<<" ms.vol="<<ms.vol<<" ms.lenbrain="<<ms.lenbrain<<std::endl;

float* tf=new float[dp->lenvol_max];
double* mean=new double[ms.vol];
double* sd=new double[ms.vol];
double* meanloglFCD=new double[ms.vol](); //zero
double* meanlFCDpearson=new double[ms.vol](); //zero
int* tdmn=new int[ms.vol](); //zero
    
//START210429
int* maskidx=new int[ms.vol];

double td1,td2;
float* tfp;

for(size_t i1=0;i1<files->nfiles;++i1){

    std::cout<<"here-1"<<std::endl;

    //lfcd se(ms.dim[0],ms.dim[1],ms.dim[2],ms.lenbrain,ms.brnidx);
    //START210429
    lfcd se(ms);

    std::cout<<"Loading "<<files->files[i1]<<std::endl;

    stack fs;
    if(!(fs.stack1(files->files[i1],tf)))exit(-1);
    for(i=0;i<dp->vol[i1];++i){mean[i]=sd[i]=0.;}
    //for(i=0;i<dp->vol[i1];++i){mean[i]=sd[i]=(double)UNSAMPLED_VOXEL;}

    #if 0
    for(k=i=0;i<dp->tdim[i1];++i,k+=dp->vol[i1]){
        for(j=0;j<ms.lenbrain;++j)mean[ms.brnidx[j]]+=(double)tf[k+ms.brnidx[j]];
        }
    for(i=0;i<ms.lenbrain;++i)mean[ms.brnidx[i]]/=(double)dp->tdim[i1];
    for(k=i=0;i<dp->tdim[i1];++i,k+=dp->vol[i1]){
        for(j=0;j<ms.lenbrain;++j)sd[ms.brnidx[j]]+=pow((double)tf[k+ms.brnidx[j]]-mean[ms.brnidx[j]],2.);
        }
    for(i=0;i<ms.lenbrain;++i)sd[ms.brnidx[i]]=sqrt(sd[ms.brnidx[i]]/(double)(dp->tdim[i1]-1));
    #endif
    //START210422
    std::cout<<"Computing mean"<<std::endl;
    for(tfp=tf,i=0;i<dp->tdim[i1];++i,tfp+=dp->vol[i1]){
        for(j=0;j<ms.lenbrain;++j)mean[ms.brnidx[j]]+=(double)*(tfp+ms.brnidx[j]);
        }
    for(i=0;i<ms.lenbrain;++i)mean[ms.brnidx[i]]/=(double)dp->tdim[i1];

    //residuals are typically zero mean
    //for(i=0;i<ms.lenbrain;++i)if(std::isnan(mean[ms.brnidx[i]]))std::cout<<"mean["<<ms.brnidx[i]<<"]="<<mean[ms.brnidx[i]]<<std::endl;
    //for(i=0;i<ms.lenbrain;++i)if(std::abs(mean[ms.brnidx[i]])<=(double)UNSAMPLED_VOXEL)std::cout<<"mean["<<ms.brnidx[i]<<"]="<<mean[ms.brnidx[i]]<<std::endl;

    //std::cout<<"mean[758804]="<<mean[758804]<<" mean[758805]="<<mean[758805]<<" mean[0]="<<mean[0]<<std::endl;

    std::cout<<"Computing sd"<<std::endl;
    for(tfp=tf,i=0;i<dp->tdim[i1];++i,tfp+=dp->vol[i1]){
        for(j=0;j<ms.lenbrain;++j)sd[ms.brnidx[j]]+=pow((double)*(tfp+ms.brnidx[j])-mean[ms.brnidx[j]],2.);
        }
    for(i=0;i<ms.lenbrain;++i)sd[ms.brnidx[i]]=sqrt(sd[ms.brnidx[i]]/(double)(dp->tdim[i1]-1));


    for(i=0;i<ms.lenbrain;++i)if(std::isnan(sd[ms.brnidx[i]])||std::abs(sd[ms.brnidx[i]])<=(double)UNSAMPLED_VOXEL){
        se.brnidx.erase(se.brnidx.begin()+i-ms.lenbrain+se.brnidx.size());
        }
    std::cout<<"ms.lenbrain="<<ms.lenbrain<<" se.brnidx.size()="<<se.brnidx.size()<<std::endl;

    
    //se.spatial_extent2lFCD(thresh,mean,sd,tf,dp->tdim[i1]);
    //START210429
    for(i=0;i<ms.vol;++i)maskidx[i]=-1;
    for(i=0;i<(int)se.brnidx.size();++i)maskidx[se.brnidx[i]]=i;


    se.spatial_extent2lFCD(thresh,mean,sd,tf,dp->tdim[i1],maskidx);

    std::cout<<"here0"<<std::endl;


    //for(i=0;i<se.nbrnidx;++i){
    for(size_t i=0;i<se.brnidx.size();++i){
        meanloglFCD[se.brnidx[i]]+=(td1=log(se.lFCD[i]));
        meanlFCDpearson[se.brnidx[i]]+=(td2=atanh(se.lFCDpearson[i]));
        tdmn[i]++;

        if(MARMSm){
            wmeanloglFCD[se.brnidx[i]]+=mv.wts[i1]*td1;
            wmeanlFCDpearson[se.brnidx[i]]+=mv.wts[i1]*td2;
            wtmd[se.brnidx[i]]+=mv.wts[i1];
            }
        }

    std::cout<<"here1"<<std::endl;

    if(!meanonly){
        for(i=0;i<dp->vol[i1];++i)tf[i]=0.;
        std::string str(files->files[i1]);
        str=str.substr(str.find_last_of("/")+1,str.find_last_of(".nii")-str.find_last_of("/")-5);

        for(size_t i=0;i<se.brnidx.size();++i)tf[se.brnidx[i]]=(float)se.lFCD[i];
        std::string str2=str+"_lFCD.nii";
        if(!(nifti_write_setdim4to1((char*)str2.c_str(),files->files[i1],tf)))exit(-1);
        std::string str3="gzip -f "+str2;
        if(system(str3.c_str())==-1)std::cout<<"fidlError: "<<str3<<std::endl;else std::cout<<"Output written to "<<str2+".gz"<<std::endl;

        for(size_t i=0;i<se.brnidx.size();++i)if(se.lFCD[i])tf[se.brnidx[i]]=(float)log(se.lFCD[i]);
        str2=str+"_loglFCD.nii";
        if(!(nifti_write_setdim4to1((char*)str2.c_str(),files->files[i1],tf)))exit(-1);
        str3="gzip -f "+str2;
        if(system(str3.c_str())==-1)std::cout<<"fidlError: "<<str3<<std::endl;else std::cout<<"Output written to "<<str2+".gz"<<std::endl;

        for(size_t i=0;i<se.brnidx.size();++i)tf[se.brnidx[i]]=(float)se.lFCDpearson[i];
        str2=str+"_lFCDpearson.nii";
        if(!(nifti_write_setdim4to1((char*)str2.c_str(),files->files[i1],tf)))exit(-1);
        str3="gzip -f "+str2;
        if(system(str3.c_str())==-1)std::cout<<"fidlError: "<<str3<<std::endl;else std::cout<<"Output written to "<<str2+".gz"<<std::endl;
        }

    std::cout<<"here2"<<std::endl;

    }

std::cout<<"here3"<<std::endl;

if(omean){
    for(i=0;i<ms.vol;++i)tf[i]=0.;
    std::size_t found;
    std::string str(omean);
    if((found=str.find(".nii"))!=std::string::npos)str.erase(found,std::string::npos);

    for(i=0;i<ms.lenbrain;++i)tf[ms.brnidx[i]]=tdmn[ms.brnidx[i]]?(float)(meanloglFCD[ms.brnidx[i]]/(double)tdmn[ms.brnidx[i]]):0.;
    std::string str2=str+"_meanloglFCD.nii";
    if(!(nifti_write_setdim4to1((char*)str2.c_str(),files->files[0],tf)))exit(-1);
    std::string str3="gzip -f "+str2;
    if(system(str3.c_str())==-1)std::cout<<"fidlError: "<<str3<<std::endl;else std::cout<<"Output written to "<<str2+".gz"<<std::endl;

    for(i=0;i<ms.lenbrain;++i)tf[ms.brnidx[i]]=tdmn[ms.brnidx[i]]?(float)tanh(meanlFCDpearson[ms.brnidx[i]]/(double)tdmn[ms.brnidx[i]]):0.;
    str2=str+"_meanlFCDpearson.nii";
    if(!(nifti_write_setdim4to1((char*)str2.c_str(),files->files[0],tf)))exit(-1);
    str3="gzip -f "+str2;
    if(system(str3.c_str())==-1)std::cout<<"fidlError: "<<str3<<std::endl;else std::cout<<"Output written to "<<str2+".gz"<<std::endl;

    if(MARMSm){
        for(i=0;i<ms.lenbrain;++i)tf[ms.brnidx[i]]=tdmn[ms.brnidx[i]]?(float)(wmeanloglFCD[ms.brnidx[i]]/(double)wtmd[ms.brnidx[i]]):0.;
        str2=str+"_wmeanloglFCD.nii";
        if(!(nifti_write_setdim4to1((char*)str2.c_str(),files->files[0],tf)))exit(-1);
        str3="gzip -f "+str2;
        if(system(str3.c_str())==-1)std::cout<<"fidlError: "<<str3<<std::endl;else std::cout<<"Output written to "<<str2+".gz"<<std::endl;

        for(i=0;i<ms.lenbrain;++i)tf[ms.brnidx[i]]=tdmn[ms.brnidx[i]]?(float)tanh(wmeanlFCDpearson[ms.brnidx[i]]/(double)wtmd[ms.brnidx[i]]):0.;
        str2=str+"_wmeanlFCDpearson.nii";
        if(!(nifti_write_setdim4to1((char*)str2.c_str(),files->files[0],tf)))exit(-1);
        str3="gzip -f "+str2;
        if(system(str3.c_str())==-1)std::cout<<"fidlError: "<<str3<<std::endl;else std::cout<<"Output written to "<<str2+".gz"<<std::endl;
        }
    }

std::cout<<"here3"<<std::endl;

}
