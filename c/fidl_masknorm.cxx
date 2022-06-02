/* Copyright 8/1/19 Washington University.  All Rights Reserved.
   fidl_masknorm.cxx  $Revision: 1.1 $ */

#include <cstdlib>
#include <iostream>
#include <fstream>
//#include <sstream>
#include <cstring>
#include <cmath>
#include "files_struct.h"
#include "dim_param2.h"
#include "mask.h"
#include "filetype.h"
#include "fidl.h"
#include "subs_util.h"
#include "subs_nifti.h"
#include "check_dimensions.h"
#include "movement.h"
#include "get_grand_mean_struct.h"

int main(int argc,char **argv){
char *maskf=NULL,*onameappend=NULL,*omean=NULL,string[MAXNAME],*strptr,filename[MAXNAME]; //*omeanroot=NULL
int i,j,nfiles=0,ndiv=0,meanonly=0,wmeanonly=0,*tdmn=NULL,nospatialmean=0,nMARMSm=0,rms=0,lcunscaled=0; 
size_t i1;
float *tf,*tf2=NULL;
double meanvox=0,meanvox2=0,td,*tdm=NULL,*wtm=NULL,*wtmd=NULL;
Files_Struct *files=NULL,*div=NULL,*MARMSm=NULL;
Dim_Param2 *dp=NULL,*dp2=NULL;
mask ms;
Interfile_header *ifh=NULL;

if(argc < 5) {
    std::cout<<argv[0]<<std::endl;

    //std::cout<<"  -files:        .img's, .nii's, .nii.gz's or a conc or list."<<std::endl;
    //START220111
    std::cout<<"  -files:        .img's, .nii's, .nii.gz's or glm's or conc(s) or list(s)."<<std::endl;

    std::cout<<"  -div:          .img's, .nii's, .nii.gz's or a conc or list. One -div for each -files."<<std::endl;
    std::cout<<"                 If specified, then -files is divided by the spatial mean of -div. "<<std::endl;
    std::cout<<"                 If not specified, then -files is divided by the spatial mean of -files. "<<std::endl;
    std::cout<<"  -nospatialmean -files is divided by -div, rather than the default behavior of the spatial mean of -div as stated above."<<std::endl;
    std::cout<<"  -rms           Spatial mean is computed as an rms value, rather than the default behavior of the arithmetic mean."<<std::endl;
    std::cout<<"                 Use this option to normalize to a standard deviation."<<std::endl;
    std::cout<<"  -mask:         Spatial mean is computed for only voxels in the -mask"<<std::endl;
    std::cout<<"                 However, all image voxels are analyzed. Thus -mask can be a reference region utilized for normalization."<<std::endl;
    std::cout<<"  -onameappend:  Append this string onto the filename"<<std::endl;
    std::cout<<"  -omean:        Output name or root of mean image computed voxelwise across all -files."<<std::endl;
    std::cout<<"  -Movement_AbsoluteRMS_mean: One for each -files. Weighted mean is calculated across -files in addition to the arithmetic mean."<<std::endl;
    std::cout<<"                              Movement_AbsoluteRMS_mean is inverted so that runs with less movement are weighted more heavily."<<std::endl;

    //std::cout<<"  -meanonly      Only output the mean image."<<std::endl;
    //START210914
    std::cout<<"  -meanonly      Only output the mean and wmean."<<std::endl;
    std::cout<<"  -wmeanonly     Only output the wmean."<<std::endl;

    //START220128
    std::cout<<"  -unscaled      Option for glm's. Variances in MR units rather than scaled to the intercept which is the default."<<std::endl;
    std::cout<<"                 If there are multiple glms per subject, and an F contrast will be applied, then scaling to the intercept is recommended."<<std::endl;
    std::cout<<"                 Regardless, the units of the variance (denominator of F contrast) must match the units of the estimates (numerator of F contrast)."<<std::endl;

    std::cout<<"  Ex. /home/usr/mcavoy/FIDL/build/fidl_masknorm -files 1011_016_mMR_B1_Baseline_222_t88_unscaled.4dfp.img -mask /home/usr/fidl/lib/glm_atlas_mask_222.4dfp.img -onameappend normatlasmask"<<std::endl;
    std::cout<<"  Ex. /home/usr/mcavoy/FIDL/build/fidl_masknorm -files ../1011_016_mMR_B1_Baseline_222_t88_unscaled.4dfp.img ../1011_016_mMR_B2_Baseline_2_222_t88_unscaled.4dfp.img -mask /home/usr/fidl/lib/glm_atlas_mask_222.4dfp.img -onameappend normatlasmask -omean 1011_016_mean.4dfp.img"<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfiles;
        if(!(files=read_files(nfiles,&argv[i+1])))exit(-1);
        i+=nfiles;
        }
    if(!strcmp(argv[i],"-div") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++ndiv;
        if(!(div=read_files(ndiv,&argv[i+1])))exit(-1);
        i+=ndiv;
        }
    if(!strcmp(argv[i],"-nospatialmean"))
        nospatialmean=1;

    //210406
    if(!strcmp(argv[i],"-rms"))
        rms=1;

    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        maskf=argv[++i];
    if(!strcmp(argv[i],"-onameappend") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        onameappend=argv[++i];
    if(!strcmp(argv[i],"-omean") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        omean=argv[++i];

    //START210406
    //if(!strcmp(argv[i],"-omeanroot") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
    //    omeanroot=argv[++i];

    if(!strcmp(argv[i],"-Movement_AbsoluteRMS_mean") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nMARMSm;
        if(!(MARMSm=read_files(nMARMSm,&argv[i+1])))exit(-1);
        i+=nMARMSm;
        }
    if(!strcmp(argv[i],"-meanonly"))
        meanonly=1;

    //START210914
    if(!strcmp(argv[i],"-wmeanonly"))
        wmeanonly=1;

    //START220128
    if(!strcmp(argv[i],"-unscaled"))
        lcunscaled=1;
    }
if(!nfiles){
    std::cout<<argv[0]<<"\n"<<"    fidlError: Need to specify -files"<<std::endl;
    exit(-1);
    }

//if(!maskf){
//START210331
if(!maskf&&!nospatialmean){

    std::cout<<argv[0]<<"\n"<<"    fidlError: Need to specify -mask"<<std::endl;
    exit(-1);
    }

#if 0
if(!omean&&!omeanroot){
    if(!omean){
        std::cout<<argv[0]<<"\n"<<"    fidlError: Need to specify -omean"<<std::endl;
        exit(-1);
        }
    if(!omeanroot){
        std::cout<<argv[0]<<"\n"<<"    fidlError: Need to specify -omeanroot"<<std::endl;
        exit(-1);
        }
    }
#endif
//START210406
if(!omean){
    std::cout<<argv[0]<<"\n"<<"    fidlError: Need to specify -omean"<<std::endl;
    exit(-1);
    }


#if 0
if(!(dp=dim_param2(files->nfiles,files->files)))exit(-1);
if(dp->filetypeall==(int)IMG&&!onameappend&&(!meanonly&&!wmeanonly)){
    std::cout<<argv[0]<<"\n"<<"    fidlError: Need to specify -onameappend"<<std::endl;
    exit(-1);
    }
if(dp->tdimall!=1) {
    std::cout<<"fidlError: Time dimension > 1. Need to add code to normalize this type of stack."<<std::endl;
    exit(-1);
    }
if(dp->volall==-1) {
    std::cout<<"fidlError: All -files must be of the same dimension."<<std::endl;
    exit(-1);
    }
if(!(ms.get_mask(maskf,dp->volall,(int*)NULL,(LinearModel*)NULL,dp->volall)))exit(-1);
if(dp->volall!=ms.vol){
    std::cout<<"fidlError: dp->volall="<<dp->volall<<" ms.vol="<<ms.vol<<" Must be equal."<<std::endl;
    exit(-1);
    }
#endif
//START220111
LinearModel **glmstack=NULL;
Grand_Mean_Struct **gmsstack=NULL;
int ft0,vol=-1;
if((ft0=get_filetype(files->files[0]))!=(int)GLM){
    if(!(dp=dim_param2(files->nfiles,files->files)))exit(-1);
    if(dp->filetypeall==(int)IMG&&!onameappend&&(!meanonly&&!wmeanonly)){
        std::cout<<argv[0]<<"\n"<<"    fidlError: Need to specify -onameappend"<<std::endl;
        exit(-1);
        }
    if(dp->tdimall!=1) {
        std::cout<<"fidlError: Time dimension > 1. Need to add code to normalize this type of stack."<<std::endl;
        exit(-1);
        }
    if(dp->volall==-1) {
        std::cout<<"fidlError: All -files must be of the same dimension."<<std::endl;
        exit(-1);
        }
    vol=dp->volall;
    }
else{
    int vol0;
    glmstack=new LinearModel*[files->nfiles];
    gmsstack=new Grand_Mean_Struct*[files->nfiles];
    for(i1=0;i1<files->nfiles;++i1){
        if(!(glmstack[i1]=read_glm(files->files[i1],(int)SMALL))) {
            std::cout<<"fidlError: reading "<<files->files[i1]<<" Abort!"<<std::endl;
            exit(-1);
            }
        if(!i1)vol=glmstack[i1]->ifh->glm_xdim*glmstack[i1]->ifh->glm_ydim*glmstack[i1]->ifh->glm_zdim;
        else if(vol!=(vol0=glmstack[i1]->ifh->glm_xdim*glmstack[i1]->ifh->glm_ydim*glmstack[i1]->ifh->glm_zdim)){
            std::cout<<"fidlError: "<<files->files[i1]<<" vol="<<vol0<<" Must have vol="<<vol<<" Abort!"<<std::endl;
            exit(-1);
            } 
        if(!(gmsstack[i1]=get_grand_mean(glmstack[i1],files->files[i1],stdout)))exit(-1);
        }
    }
if(!(ms.get_mask(maskf,vol,(int*)NULL,(LinearModel*)NULL,vol)))exit(-1);
if(vol!=ms.vol){
    std::cout<<"fidlError: vol="<<vol<<" ms.vol="<<ms.vol<<" Must be equal."<<std::endl;
    exit(-1);
    }


if(div){
    if(div->nfiles!=files->nfiles){
        std::cout<<"fidlError: files->nfiles="<<files->nfiles<<" div->nfiles="<<div->nfiles<<" Must be equal."<<std::endl;
        exit(-1);
        }
    if(!(dp2=dim_param2(div->nfiles,div->files)))exit(-1);
    if(dp2->volall>1){
        if(!check_dimensions(div->nfiles,div->files,dp->volall))exit(-1);
        }
    }
movement mv;
if(MARMSm){
    if(MARMSm->nfiles!=files->nfiles){
        std::cout<<"fidlError: files->nfiles="<<files->nfiles<<" MARMSm->nfiles="<<MARMSm->nfiles<<" Must be equal."<<std::endl;
        exit(-1);
        }
    if(!mv.movement0(MARMSm))exit(-1);
    wtm=new double[ms.vol](); //empty parentheses zero intializes
    wtmd=new double[ms.vol](); //empty parentheses zero intializes
    }
tf=new float[ms.vol];
tdm=new double[ms.vol](); //empty parentheses zero intializes
tdmn=new int[ms.vol](); //empty parentheses zero intializes

//if(nospatialmean||wtm)tf2=new float[ms.vol];
//START220112
if(nospatialmean||wtm)tf2=new float[ms.vol](); //zero initialization for glm output
double df0=0.,df0d=0.;

for(i1=0;i1<files->nfiles;++i1){
    stack fs,fs2;
    if(!nospatialmean){ //glms won't go here b/c we just want to motion weight
        if(!(fs.stack1(!div?files->files[i1]:div->files[i1],tf)))exit(-1);
        int cnt=0,cntnan=0; 
        for(meanvox=meanvox2=0.,j=0;j<ms.lenbrain;j++)
            if(std::abs(tf[ms.brnidx[j]])>(float)UNSAMPLED_VOXEL&&!isnan(tf[ms.brnidx[j]])){meanvox+=(double)tf[ms.brnidx[j]];meanvox2+=(double)tf[ms.brnidx[j]]*(double)tf[ms.brnidx[j]];cnt++;}
            else cntnan++; 
        meanvox/=(double)cnt;
        meanvox2=sqrt(meanvox2/(double)cnt); 
        }
    if(div)if(!(fs.stack1(files->files[i1],tf)))exit(-1);
    if(!nospatialmean){
        double divisor=!rms?meanvox:meanvox2;
        for(j=0;j<ms.vol;++j)if(std::abs(tf[j])>(float)UNSAMPLED_VOXEL&&!isnan(tf[j])){
            tdm[j]+=(td=(double)tf[j]/divisor);tdmn[j]++;
            tf[j]=(float)td;
            if(wtm){
                wtm[j]+=mv.wts[i1]*td;
                wtmd[j]+=mv.wts[i1];
                }
            }
        }
    else if(div){
        if(!(fs2.stack1(div->files[i1],tf2)))exit(-1);
        if(dp2->volall>1){
            for(j=0;j<ms.vol;++j)if(std::abs(tf[j])>(float)UNSAMPLED_VOXEL&&std::abs(tf2[j])>(float)UNSAMPLED_VOXEL&&!isnan(tf[j])&&!isnan(tf2[j])){
                tdm[j]+=(td=(double)tf[j]/(double)tf2[j]);tdmn[j]++;
                tf[j]=(float)td;
                if(wtm){
                    wtm[j]+=mv.wts[i1]*td;
                    wtmd[j]+=mv.wts[i1];
                    }
                }
            }
        else{ 
            for(j=0;j<ms.vol;++j)if(std::abs(tf[j])>(float)UNSAMPLED_VOXEL&&!isnan(tf[j])){
                tdm[j]+=(td=(double)tf[j]/(double)tf2[0]);tdmn[j]++;
                tf[j]=(float)td;
                if(wtm){
                    wtm[j]+=mv.wts[i1]*td;
                    wtmd[j]+=mv.wts[i1];
                    }
                }
            }
        }
    else{

        #if 0
        if(!(fs.stack1(files->files[i1],tf)))exit(-1);
        for(j=0;j<ms.vol;++j)if(std::abs(tf[j])>(float)UNSAMPLED_VOXEL&&!isnan(tf[j])){
            tdm[j]+=(td=(double)tf[j]);tdmn[j]++;
            if(wtm){
                wtm[j]+=mv.wts[i1]*td;
                wtmd[j]+=mv.wts[i1];
                }
            }
        #endif
        //START220112
        if(ft0!=(int)GLM){ 
            if(!(fs.stack1(files->files[i1],tf)))exit(-1);
            for(j=0;j<ms.vol;++j)if(std::abs(tf[j])>(float)UNSAMPLED_VOXEL&&!isnan(tf[j])){
                tdm[j]+=(td=(double)tf[j]);tdmn[j]++;
                if(wtm){
                    wtm[j]+=mv.wts[i1]*td;
                    wtmd[j]+=mv.wts[i1];
                    }
                }
            }
        else{

            #if 0
            if(glmstack[i1]->ifh->glm_masked){
                for(j=0;j<glmstack[i1]->nmaski;++j){
                    if(glmstack[i1]->var[j]>0.){
                        wtm[glmstack[i1]->maski[j]]+=mv.wts[i1]*glmstack[i1]->var[j];
                        wtmd[glmstack[i1]->maski[j]]+=mv.wts[i1];
                        }
                    }
                }
            else{
                for(j=0;j<ms.lenbrain;++j){
                    if(glmstack[i1]->var[ms.brnidx[j]]>0.){
                        wtm[ms.brnidx[j]]+=mv.wts[i1]*glmstack[i1]->var[ms.brnidx[j]];
                        wtmd[ms.brnidx[j]]+=mv.wts[i1];
                        }
                    }
                }
            #endif
            //START220128 All glms are "masked" and the grand mean.
            if(lcunscaled){ 
                for(j=0;j<glmstack[i1]->nmaski;++j){
                    if(glmstack[i1]->var[j]>(double)UNSAMPLED_VOXEL){
                        wtm[glmstack[i1]->maski[j]]+=mv.wts[i1]*glmstack[i1]->var[j];
                        wtmd[glmstack[i1]->maski[j]]+=mv.wts[i1];
                        }
                    }
                }
            else{ 
                for(j=0;j<glmstack[i1]->nmaski;++j){
                    if(glmstack[i1]->var[j]>(double)UNSAMPLED_VOXEL&&gmsstack[i1]->grand_mean[j]>=gmsstack[i1]->grand_mean_thresh){
                        wtm[glmstack[i1]->maski[j]]+=mv.wts[i1]*glmstack[i1]->var[j]*gmsstack[i1]->pct_chng_scl/gmsstack[i1]->grand_mean[j];
                        wtmd[glmstack[i1]->maski[j]]+=mv.wts[i1];
                        }
                    }
                }

#if 0
                            for(n=0;n<glmstack[glmi]->nmaski;n++)td1[glmstack[glmi]->maski[n]]=
                                td1un[glmstack[glmi]->maski[n]]==(double)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:
                                (td1un[glmstack[glmi]->maski[n]]*(gms_stack[glmi]->grand_mean[n]<gms_stack[glmi]->grand_mean_thresh?
                                0.:gms_stack[glmi]->pct_chng_scl/gms_stack[glmi]->grand_mean[n]));
#endif
 


            std::cout<<"glmstack["<<i1<<"]->ifh->glm_df="<<glmstack[i1]->ifh->glm_df<<std::endl;
            df0+=mv.wts[i1]*(double)glmstack[i1]->ifh->glm_df;
            df0d+=mv.wts[i1];
            }
        
        } 
    if(!meanonly&&!wmeanonly){ 
        if(dp->filetype[i1]==(int)IMG){
            if(!(ifh=read_ifh(files->files[i1],(Interfile_header*)NULL)))exit(-1);
            ifh->bigendian=0;
            if(ifh->file_name)free(ifh->file_name);
            if(!(ifh->file_name=(char*)malloc(sizeof*ifh->file_name*files->strlen_files[i1]))){
                std::cout<<"fidlError: Unable to malloc ifh->file_name"<<std::endl;
                exit(-1);
                }
            strcpy(ifh->file_name,files->files[i1]);
            if(dp->filetype[i1]==(int)IMG||!i1){
                if(ifh->mask)free(ifh->mask);
                if(!(ifh->mask=(char*)malloc(sizeof*ifh->mask*(strlen(maskf)+1)))){
                    std::cout<<"fidlError: Unable to malloc ifh->mask"<<std::endl;
                    exit(-1);
                    }
                strcpy(ifh->mask,maskf);
                }
            strcpy(string,files->files[i1]);
            if(!(strptr=get_tail_sans_ext(string)))exit(-1);
            sprintf(filename,"%s_%s.4dfp.img",strptr,onameappend);
            if(!writestack(filename,tf,sizeof(float),(size_t)dp->vol[i1],0))exit(-1);
            if(!write_ifh(filename,ifh,0))exit(-1);
            std::cout<<"Output written to "<<filename<<std::endl;
            if(dp->filetype[i1]==(int)IMG)free_ifh(ifh,0);
            }
        else{
            std::size_t found;
            std::string str(omean);
            if((found=str.find(".nii"))!=std::string::npos)str.erase(found,std::string::npos);
            if(!nospatialmean&&rms)str+="_rms";
            str+=".nii";
            if(!nifti_write2((char*)str.c_str(),files->files[i1],tf))exit(-1);
            std::string str2="gzip -f "+str;
            if(system(str2.c_str())==-1)std::cout<<"fidlError: "<<str2<<std::endl;else std::cout<<"Output written to "<<str+".gz"<<std::endl;
            } 
        } 
    }


#if 0
float *wmean=NULL;
if(files->nfiles>1){
    for(j=0;j<ms.vol;j++)tf[j]=!tdmn[j]?0.:(float)(tdm[j]/(double)tdmn[j]);
    if(wtm){
        for(j=0;j<ms.vol;j++)tf2[j]=!tdmn[j]?0.:(float)(wtm[j]/wtmd[j]);
        wmean=tf2;
        }
    }
else{
    wmean=tf;
    wmeanonly=1;
    }
#endif
//START220112
float *wmean=NULL;
if(files->nfiles>1||ft0==(int)GLM){
    if(ft0!=(int)GLM)for(j=0;j<ms.vol;j++)tf[j]=!tdmn[j]?0.:(float)(tdm[j]/(double)tdmn[j]);
    if(wtm){
        for(j=0;j<ms.vol;j++)tf2[j]=!wtm[j]?0.:(float)(wtm[j]/wtmd[j]);
        wmean=tf2;
        }
    }
else{
    wmean=tf;
    wmeanonly=1;
    }
if(ft0==(int)GLM)df0/=df0d;



if(files->nfiles>1||meanonly||wmeanonly){

    //if(dp->filetypeall==(int)IMG){
    //START220112
    if(ft0==(int)IMG){

        if(!writestack(omean,tf,sizeof(float),(size_t)ms.vol,0))exit(-1);
        if(!(ifh=read_ifh(files->files[0],(Interfile_header*)NULL)))exit(-1);
        ifh->bigendian=0;
        if(ifh->mask)free(ifh->mask);
        if(!(ifh->mask=(char*)malloc(sizeof*ifh->mask*(strlen(maskf)+1)))){
            std::cout<<"fidlError: Unable to malloc ifh->mask"<<std::endl;
            exit(-1);
            }
        strcpy(ifh->mask,maskf);
        if(!write_ifh(omean,ifh,0))exit(-1);
        std::cout<<"Output written to "<<omean<<std::endl;
        }
    else{
        std::size_t found;
        std::string str(omean);
        if((found=str.find(".nii"))!=std::string::npos)str.erase(found,std::string::npos);
        if(!nospatialmean&&rms)str+="_rms";
        if(!wmeanonly){        
            std::string str2=str+(files->nfiles>1||!rms?"_mean.nii":".nii");
            if(!nifti_write2((char*)str2.c_str(),files->files[0],tf))exit(-1);
            std::string str3="gzip -f "+str2;
            if(system(str3.c_str())==-1)std::cout<<"fidlError: "<<str3<<std::endl;else std::cout<<"here1 Output written to "<<str2+".gz"<<std::endl;
            }
        if(wmean){
            
            //std::string str2=str+(files->nfiles>1||!rms?"_wmean.nii":".nii");
            //START220112
            std::string str2=str;

            //if(ft0==(int)GLM)str2+="_var";
            if(ft0==(int)GLM){
                str2+="_var";
                if(lcunscaled)str2+="_unscaled";
                }

            str2+=files->nfiles>1||!rms?"_wmean.nii":".nii";

            //if(!nifti_write2((char*)str2.c_str(),files->files[0],wmean))exit(-1);
            //START220112
            if(ft0!=(int)GLM){
                if(!nifti_write2((char*)str2.c_str(),files->files[0],wmean))exit(-1);
                }
            else{
                int64_t dims[4]={glmstack[0]->ifh->glm_xdim,glmstack[0]->ifh->glm_ydim,glmstack[0]->ifh->glm_zdim,1};
                if(!nifti_write((char*)str2.c_str(),dims,glmstack[0]->ifh->center,glmstack[0]->ifh->mmppix,wmean))exit(-1);
                }
            std::string str3="gzip -f "+str2;
            if(system(str3.c_str())==-1)std::cout<<"fidlError: "<<str3<<std::endl;else std::cout<<"Output written to "<<str2+".gz"<<std::endl;

            //START220112
            if(ft0==(int)GLM){
                std::string str2=str+"_df_wmean.dat";
                std::ofstream ofile(str2);
                ofile<<df0<<std::endl;
                std::cout<<"Output written to "<<str2<<std::endl;
                }

            }

        }
    }
}
