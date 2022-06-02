/* Copyright 11/1/17 Washington University.  All Rights Reserved.
   mask.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <vector>
//#include <math.h>
#include <cmath>
#include "checkOS.h"
#include "subs_util.h"
#include "shouldiswap.h"
#include "filetype.h"
#include "mask.h"
#include "checkOS.h"

mask::mask(){
    unsi=maskidx=brnidx=NULL;
    lcbrnidx=lcmaskidx=0;
    }

mask::~mask(){
    if(unsi) delete[] unsi;
    if(lcmaskidx) delete[] maskidx;
    if(lcbrnidx) delete[] brnidx;
    }

int mask::read_mask(char *mask_file,LinearModel *glm_in){

    long start_b=0;
    int i,j,k,swapbytes=0; 
    FILE *fp;
    LinearModel *glm;

    if(!header0(mask_file))return 0;
    if(filetype==(int)GLM){
        if(!glm_in) {

            //if(!(glm=read_glm(mask_file,0,SunOS_Linux))) {
            //START220111
            if(!(glm=read_glm(mask_file,0))){

                printf("fidlError: read_mask Unable to read %s  Abort!\n",mask_file);
                return 0;
                }
            }
        else {
            glm = glm_in;
            }
        assign(glm->ifh->glm_xdim,glm->ifh->glm_ydim,glm->ifh->glm_zdim,glm->ifh->voxel_size_1,glm->ifh->voxel_size_2,
            glm->ifh->voxel_size_3,glm->ifh->center,glm->ifh->mmppix);
        start_b = find_b(glm);
        swapbytes = shouldiswap(SunOS_Linux,glm->ifh->bigendian);
        if(!glm_in)free_glm(glm);
        }
    try{

        //float* temp_float=new float[vol];
        //START190221
        std::vector<float> temp_float(vol);

        if(filetype==(int)GLM){
            if(!(fp=fopen_sub(mask_file,"r")))return 0;
            if(fseek(fp,(long)start_b,(int)SEEK_SET)){
                std::cout<<"fidlError: read_mask occured while seeking to "<<start_b<<" in "<<mask_file<<std::endl;
                return 0;
                }

            //if(!fread_sub(temp_float,sizeof(float),(size_t)vol,fp,swapbytes,mask_file)) {
            //START190221
            if(!fread_sub(temp_float.data(),sizeof(float),(size_t)vol,fp,swapbytes,mask_file)) {

                std::cout<<"fidlError: read_mask reading parameter estimates from "<<mask_file<<std::endl;
                return 0;
                }
            fclose(fp);
            }
        else{

            //if(!getstack(temp_float,SunOS_Linux))return 0;
            //START190221
            //if(!getstack(temp_float.data(),SunOS_Linux))return 0;
            //START190730
            if(!getstack(temp_float.data()))return 0;

            }

        #if 1
        maskidx=new int[vol];lcmaskidx=1;
        for(i=0;i<vol;i++)maskidx[i]=-1;
        for(lenbrain=i=0;i<vol;i++)  //Could be binary mask or image.

            //if(temp_float[i]>(float)UNSAMPLED_VOXEL || temp_float[i]<(-(float)UNSAMPLED_VOXEL))maskidx[i]=lenbrain++;
            //START190801
            //if(fabsf(temp_float[i])>(float)UNSAMPLED_VOXEL)maskidx[i]=lenbrain++;
            //START210331
            if(std::abs(temp_float[i])>(float)UNSAMPLED_VOXEL&&!isnan(temp_float[i]))maskidx[i]=lenbrain++;

        brnidx=new int[lenbrain];lcbrnidx=1;
        nuns=vol-lenbrain;

        //std::cout<<"here300 vol="<<vol<<" lenbrain="<<lenbrain<<" nuns="<<nuns<<std::endl;

        unsi=new int[nuns];
        for(k=j=i=0;i<vol;i++)if(maskidx[i]>=0){brnidx[j++]=i;}else{unsi[k++]=i;}
        #endif
        //START190219
        #if 0
        std::vector<int> maskidx(vol,-1);
        for(lenbrain=i=0;i<vol;i++)  //Could be binary mask or image.
            if(fabsf(temp_float[i])>(float)UNSAMPLED_VOXEL)maskidx[i]=lenbrain++;
        std::vector<int> brnidx(lenbrain);
        std::vector<int> unsi((nuns=vol-lenbrain));
        for(k=j=i=0;i<vol;i++)if(maskidx[i]>=0){brnidx[j++]=i;}else{unsi[k++]=i;}
        #endif

        //for(i=0;i<lenbrain;i++)std::cout<<brnidx[i]<<std::endl;

        if(j!=lenbrain) {
            std::cout<<"fidlError: read_mask j="<<j<<" lenbrain="<<lenbrain<<" Must be equal."<<std::endl;
            return 0;
            }
        if(k!=nuns){
            std::cout<<"fidlError: read_mask k="<<k<<" nuns="<<nuns<<" Must be equal."<<std::endl;
            return 0;
            }

        //START190221
        //delete[] temp_float;

        }
    catch(std::bad_alloc& ba){

        //std::cout<<"bad_alloc caught: "<<ba.what()<<std::endl;
        //START190221
        std::cout<<"read_mask vol="<<vol<<" lenbrain="<<lenbrain<<" nuns="<<nuns<<" "<<ba.what()<<std::endl;
        return 0;

        }
    return 1;
    }



int mask::get_mask(char *mask_file,int vol0,int *indices,LinearModel *glm_in,int msvol){
    int i;

    if(mask_file){
        if(!(read_mask(mask_file,glm_in))){
            std::cout<<"fidlError: get_mask from read_mask"<<std::endl;
            return 0;
            }
        if(msvol&&(vol0!=msvol)){ //changed from vol0 to msvol for compressed files
            std::cout<<"fidlError: get_mask vol="<<vol<<" vol0="<<vol0<<" Must be equal."<<std::endl;
            return 0;
            }
        }
    else {


        try{ 
            if(!indices) {
                vol=lenbrain=vol0;
 
                #if 1
                brnidx=new int[lenbrain];lcbrnidx=1;
                for(i=0;i<lenbrain;i++)brnidx[i]=i;
                maskidx=brnidx;
                #endif
                //START190219
                #if 0
                std::vector<int> brnidx(lenbrain);
                for(i=0;i<lenbrain;i++)brnidx[i]=i;
                maskidx=brnidx;
                //std::vector<int> maskidx=brnidx;
                #endif

                }
            else {
                lenbrain=vol0;
                vol=msvol;

                #if 1
                brnidx=indices;
                maskidx=new int[vol];lcmaskidx=1;
                for(i=0;i<vol;i++)maskidx[i]=-1;
                for(i=0;i<lenbrain;i++)maskidx[brnidx[i]]=i;
                #endif
                //START190219
                #if 0
                std::vector<int> brnidx(indices,indices+lenbrain);
                std::vector<int> maskidx(vol,-1);
                for(i=0;i<lenbrain;i++)maskidx[brnidx[i]]=i;
                #endif

                }
            }
        catch(std::bad_alloc& ba){
            std::cout<<"bad_alloc caught: "<<ba.what()<<std::endl;
            }

        nuns=0;
        }
    return 1;
    }

#if 0
int* mask::get_brnidx(int& lenbrain0){
    lenbrain0=lenbrain;
    return brnidx.data();
    }
int* mask::get_brnidx(int& lenbrain0,int &vol0){
    lenbrain0=lenbrain;
    vol0=vol;
    return brnidx.data();
    }
int* mask::get_brnidx(int& lenbrain0,int& xdim,int& ydim,int& zdim){
    lenbrain0=lenbrain;
    assign(xdim,ydim,zdim);
    return brnidx.data();
    }
int* mask::get_brnidx(int& lenbrain0,int &vol0,int& xdim,int& ydim,int& zdim){
    lenbrain0=lenbrain;
    vol0=vol;
    assign(xdim,ydim,zdim);
    return brnidx.data();
    }
#endif
//START190221
int* mask::get_brnidx(int& lenbrain0){
    lenbrain0=lenbrain;
    return brnidx;
    }
int* mask::get_brnidx(int& lenbrain0,int &vol0){
    lenbrain0=lenbrain;
    vol0=vol;
    return brnidx;
    }
int* mask::get_brnidx(int& lenbrain0,int& xdim,int& ydim,int& zdim){
    lenbrain0=lenbrain;
    assign(xdim,ydim,zdim);
    return brnidx;
    }
int* mask::get_brnidx(int& lenbrain0,int &vol0,int& xdim,int& ydim,int& zdim){
    lenbrain0=lenbrain;
    vol0=vol;
    assign(xdim,ydim,zdim);
    return brnidx;
    }

//START190320
int* mask::get_maskidx(){
    return maskidx;
    }

//START190917
int* mask::get_unsi(int& nuns0){
    nuns0=nuns;
    return unsi;
    }

int _read_mask(int argc,char **argv){
    char *maskf = (char *)argv[0];
    int lenbrain;
    mask ms;
    if(!(ms.read_mask(maskf,(LinearModel*)NULL)))return 0;
    ms.get_brnidx(lenbrain);
    return lenbrain;
    }

