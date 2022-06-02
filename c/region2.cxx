/* Copyright 3/16/18 Washington University.  All Rights Reserved.
   region2.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#include <unordered_set>
//START191008
#include <set>

#include "fidl.h"
#include "region.h"
#include "subs_util.h"
#include "constants.h"
#include "d2intvar.h"
#include "stack.h"
#include "filetype.h"

//START191003
#include "lut.h"

#include "nifti1.h"
using namespace cifti;

int shouldiflip(Files_Struct* files){
    int i,lcIMG=0,lcNIFTI=0;
    for(size_t j=0;j<files->nfiles;j++){
        if((i=get_filetype(files->files[j]))==(int)IMG)lcIMG=1;else if(i==(int)NIFTI)lcNIFTI=1;
        }
    return lcIMG&&lcNIFTI?1:0;
    }
Regions *get_reg(char *region_file,int vol,void *image,int nreg,char **ifh_region_names,int lcflip,int lcnamesonly,int nregval0,int* regval0,char* lutf){
    char *strptr=NULL,string[MAXNAME],**region_names_ptr=NULL;
    int nregions=0,ifh_nregions=0,i,c_orient[3]={-1}; //filetype
    Regions *reg=NULL;

    std::vector<int> image2;
    std::set<int> s2;

    lut l0;char **FS=NULL; 
    stack h;
    int lcregval=0,*regval=NULL;
    if(!region_file){
        nregions = nreg;
        }
    else{
        int vol0;
        if(!(vol0=h.header0(region_file)))return NULL;
        c_orient[0]=h.c_orient[0];c_orient[1]=h.c_orient[1];c_orient[2]=h.c_orient[2];
        if(!image)vol=vol0; 
        int lcgetstack=0;
        if(h.filetype==(int)IMG){
            if(!image&&!lcnamesonly)lcgetstack=1;
            lcregval=1; 
            }
        else if(h.filetype==(int)NIFTI){
            lcgetstack=1; 

            //lcregval=!regval0?1:0; 
            //START200213
            if(!lutf)lcregval=!regval0?1:0; 

            regval=regval0;
            //START200211
            //if(regval0)regval=regval0;
            
            }
                
        //START210416
        //std::cerr<<"***** get_reg HERE0 *****"<<std::endl;

        if(lcgetstack){
            image2.resize(vol);
            if(!(h.getstack(image2.data())))return NULL;
            if(lcflip){

                #if 0
                int cf_flip;
                if((cf_flip=h.flip(image2.data()))==-1)return NULL;
                #endif
                //START210416
                std::cerr<<"***** get_reg Flip region file. *****"<<std::endl;
                if((h.flip(image2.data()))==-1)return NULL;


                c_orient[0]=0;c_orient[1]=5;c_orient[2]=2;
                }
            image=image2.data();
            }
        if(h.filetype==(int)IMG){
            nregions = ifh_nregions=h.ifh->nregions;
            region_names_ptr=h.ifh->region_names;
            
            //std::cout<<"here0 nregions="<<nregions<<std::endl;
            //std::cout<<"here0 region_names_ptr=";for(i=0;i<nregions;++i)std::cout<<region_names_ptr[i]<<" ";std::cout<<std::endl; 

            }
        else if(h.filetype==(int)NIFTI){
            s2.insert(image2.data(),image2.data()+vol);

            //for(auto it=s2.begin();++it!=s2.end();)std::cout<<*it<<" ";std::cout<<std::endl;
            //for(auto it=s2.begin();it!=s2.end();++it)std::cout<<*it<<" ";std::cout<<std::endl;

            nregions=lcregval?s2.size()-1:nregval0;
            
            //FS=l0.lut1(region_file,lutf);
            //START210504
            if(!(FS=l0.lut1(region_file,lutf)))return NULL;

            //if(lutf&&l0.regvalmaxplusone)nregions=l0.regvalmaxplusone;
            //START210503
            if(lutf&&l0.regvalmaxplusone){


                std::string superbird(lutf);
                //nregions=superbird.find("FreeSurferColorLUT.txt")!=std::string::npos?l0.regvalmaxplusone:l0.LUT.size();
                if(superbird.find("FreeSurferColorLUT.txt")!=std::string::npos)
                    nregions=l0.regvalmaxplusone;
                else{
                    nregions=l0.LUT.size();
                    lcregval=1;
                    }
                }

            if(!nregions){
                std::cout<<"fidlError: region2.cxx nregions="<<nregions<<" Must be greater than zero."<<std::endl;
                return NULL;
                }
            
            //std::cout<<"here101 nregions = "<<nregions<<" l0.LUT.size()="<<l0.LUT.size()<<std::endl;

            }

        //std::cout<<"here1 nregions="<<nregions<<std::endl;
        //std::cout<<"here1 ifh->region_names=";for(i=0;i<nregions;++i)std::cout<<ifh->region_names[i]<<" ";std::cout<<std::endl; 
        //std::cout<<"here1 region_names_ptr=";for(i=0;i<nregions;++i)std::cout<<region_names_ptr[i]<<" ";std::cout<<std::endl; 

        }
    //std::cout<<"here200 nregions="<<nregions<<std::endl;
    //std::cout<<"here2 ifh->region_names=";for(i=0;i<nregions;++i)std::cout<<ifh->region_names[i]<<" ";std::cout<<std::endl; 
    //std::cout<<"here2 region_names_ptr=";for(i=0;i<nregions;++i)std::cout<<region_names_ptr[i]<<" ";std::cout<<std::endl; 
            
    //std::cout<<"here102 nregions = "<<nregions<<std::endl;

    if(lcregval){

        //std::cout<<"here102a nregions = "<<nregions<<std::endl;

        regval = new int[nregions];
        if(h.filetype==(int)IMG){
            for(i=0;i<nregions;i++)regval[i]=i+2;
            }
        else{
            //std::cout<<"here103 nregions = "<<nregions<<" lcregval="<<lcregval<<std::endl;

            //i=0;
            //for(auto it=s2.begin();++it!=s2.end();)regval[i++]=(int)*it;
            //START210503
            int i=0;

#if 0
            std::cout<<"here103a nregions = "<<nregions<<" lcregval="<<lcregval<<std::endl;

            std::string superbird(lutf);

            std::cout<<"here104 nregions = "<<nregions<<" lcregval="<<lcregval<<std::endl;

            if(superbird.find("FreeSurferColorLUT.txt")!=std::string::npos){ 

                std::cout<<"here104a nregions = "<<nregions<<" lcregval="<<lcregval<<std::endl;

                for(auto it=s2.begin();++it!=s2.end();)regval[i++]=(int)*it;
                }
            else{

                std::cout<<"here104b nregions = "<<nregions<<" lcregval="<<lcregval<<std::endl;

                for(auto it=s2.begin();++it!=s2.end();)if(*it)regval[i++]=(int)*it;
                }
#endif
            //START210817
            int lcsuperbird=0; 
            if(lutf){ 
                std::string superbird(lutf);
                if(superbird.find("FreeSurferColorLUT.txt")!=std::string::npos){ 
                    for(auto it=s2.begin();++it!=s2.end();)regval[i++]=(int)*it;
                    lcsuperbird=1;
                    }
                }
            if(!lcsuperbird){ 
                for(auto it=s2.begin();++it!=s2.end();)if(*it)regval[i++]=(int)*it;
                }


            }
        }

    //std::cout<<"here105 nregions = "<<nregions<<std::endl;


    //if(!(reg=get_reg_guts(nregions,vol,image,regval,lcnamesonly,h.datatype)))return NULL;
    //START202013
    //if(!(reg=get_reg_guts(nregions,vol,image,regval,lcnamesonly,lutf)))return NULL;
    //START200622
    if(!(reg=get_reg_guts(nregions,vol,image,regval,lcnamesonly,lutf,h.filetype)))return NULL;

    //std::cout<<"here104 nregions = "<<nregions<<std::endl;

    reg->vol = vol;
    for(i=0;i<3;++i)reg->c_orient[i]=c_orient[i]; 

    if(!(reg->harolds_num=(int*)malloc(sizeof*reg->harolds_num*nregions))) {
        std::cout<<"fidlError: Unable to malloc reg->harolds_num"<<std::endl;
        return NULL;
        }
    if(ifh_region_names) region_names_ptr=ifh_region_names;
    if(region_file||ifh_region_names) {
        if(!(reg->length=(size_t*)malloc(sizeof*reg->length*nregions))) {
            std::cout<<"fidlError: Unable to malloc reg->length"<<std::endl;
            return NULL;
            }
        for(i=0;i<nregions;i++)reg->length[i]=0; 

        if(ifh_nregions){
            for(i=0;i<reg->nregions;i++) {
                strcpy(string,region_names_ptr[i]); /*gets decimated*/

                //std::cout<<"region_names_ptr["<<i<<"]="<<region_names_ptr[i]<<std::endl;
                //std::cout<<"string = "<<string<<std::endl;

                if(!(strptr=strtok(string," "))) {
                    std::cout<<"fidlError: No token found - spot 1. Abort!"<<std::endl;
                    return NULL;
                    }
                reg->harolds_num[i] = atoi(strptr) + 1;
                if(!(strptr = strtok((char*)NULL," "))) {
                    std::cout<<"fidlError: No token found - spot 2. Abort!"<<std::endl;
                    return NULL;
                    }
                reg->length[i] = strlen(strptr) + 1;
                }
            }
        else{ 

            #if 0
            if(!lcregval){
                strcpy(string,region_file); /*gets decimated*/
                if(!(strptr=get_tail_sans_ext(string))) return NULL;
                reg->harolds_num[0] = 1;
                reg->length[0]=strlen(strptr)+1;
                } 
            else{
                //for(i=0;i<reg->nregions;++i) std::cout<<"regval["<<i<<"]="<<regval[i]<<std::endl;
                for(i=0;i<reg->nregions;++i)reg->length[i]=strlen(FS[regval[i]])+1;
                }
            #endif
            //START200108
            //for(i=0;i<reg->nregions;++i)reg->length[i]=strlen(FS[regval[i]])+1;
            //START200213
            if(!lutf){ 
                for(i=0;i<reg->nregions;++i)reg->length[i]=strlen(FS[regval[i]])+1;
                }
            else{
                for(i=0;i<reg->nregions;++i)reg->length[i]=l0.LUT[i].size()+1;
                }
            }
        if(!(reg->region_names=d2charvar(reg->nregions,reg->length))) return NULL;
        if(ifh_nregions){
            for(i=0;i<reg->nregions;i++) {
                strcpy(string,region_names_ptr[i]); /*gets decimated*/
                if(!(strptr=strtok(string," "))) {
                    std::cout<<"fidlError: No token found - spot 1. Abort!"<<std::endl;
                    return NULL;
                    }
                if(!(strptr = strtok((char*)NULL," "))) {
                    std::cout<<"fidlError: No token found - spot 2. Abort!"<<std::endl;
                    return NULL;
                    }
                strcpy(reg->region_names[i],strptr);
                if(ifh_region_names) if((strptr=strtok((char*)NULL," ")))reg->nvoxels_region[i]=atoi(strptr);
                }
            }
        else{

            #if 0
            if(!lcregval){
                strcpy(reg->region_names[0],strptr);
                }
            else{
                for(i=0;i<reg->nregions;++i)strcpy(reg->region_names[i],FS[regval[i]]);
                }
            #endif
            //START200108
            //for(i=0;i<reg->nregions;++i)strcpy(reg->region_names[i],FS[regval[i]]);
            //START200213
            if(!lutf){ 
                for(i=0;i<reg->nregions;++i)strcpy(reg->region_names[i],FS[regval[i]]);
                }
            else{
                for(i=0;i<reg->nregions;++i)l0.LUT[i].copy(reg->region_names[i],l0.LUT[i].size(),0);
                }

            }
        }
    else {
        reg->length = (size_t*)NULL;
        for(i=0;i<reg->nregions;i++) reg->harolds_num[i] = i+1;
        reg->region_names = (char**)NULL;
        }

    //if(regval)delete[] regval; 
    //START200107
    if(lcregval)delete[] regval; 

    //std::cout<<"here110 nregions = "<<nregions<<std::endl;

    return reg;
}

//Regions *get_reg_guts(int nregions,int vol,void *image,int *reg_val,int lcnamesonly,int16_t datatype){
//START200213
//Regions *get_reg_guts(int nregions,int vol,void *image,int *reg_val,int lcnamesonly,char* lutf){
//START200622
Regions *get_reg_guts(int nregions,int vol,void *image,int *reg_val,int lcnamesonly,char* lutf,int filetype){

    //std::cout<<"here200"<<std::endl;

    int i,j;
    Regions *reg;
    if(!(reg=(Regions*)malloc(sizeof*reg))){
        std::cout<<"fidlError: Unable to malloc reg"<<std::endl;
        return NULL;
        }
    reg->voxel_indices = NULL;
    reg->nregions = nregions;
    if(lcnamesonly)return reg;
    if(!(reg->nvoxels_region=(int*)malloc(sizeof*reg->nvoxels_region*reg->nregions))) {
        std::cout<<"fidlError: region.cxx Unable to malloc reg->nvoxels_region"<<std::endl;
        return NULL;
        }
    for(i=0;i<reg->nregions;i++) reg->nvoxels_region[i] = 0;
    if(image) {
        if(reg_val){
            if(filetype==(int)IMG){
                float* superbird;
                superbird=(float*)image;
                for(i=0;i<vol;i++)
                    for(j=0;j<reg->nregions;j++)
                        if(superbird[i] == reg_val[j])reg->nvoxels_region[j]++;
                }
            else{
                int* superbird;
                superbird=(int*)image;
                for(i=0;i<vol;i++)
                    for(j=0;j<reg->nregions;j++)
                        if(superbird[i] == reg_val[j])reg->nvoxels_region[j]++;
                }
            //std::cout<<"here70 reg->nvoxels_region=";for(i=0;i<reg->nregions;++i)std::cout<<reg->nvoxels_region[i]<<" ";std::cout<<std::endl;
            //std::cout<<"here70 reg_val=";for(i=0;i<reg->nregions;++i)std::cout<<reg_val[i]<<" ";std::cout<<std::endl;
            }
        else {
            if(!lutf){ 
                if(reg->nregions>1){
                    std::cout<<"fidlError: region.cxx reg->nregions="<<reg->nregions<<" Should be one."<<std::endl;
                    return NULL;
                    }
                float *superbird;
                superbird=(float*)image; 
                for(i=0;i<vol;i++)
                    for(j=0;j<reg->nregions;j++)
                        if(fabsf(superbird[i]) > (float)UNSAMPLED_VOXEL)
                            reg->nvoxels_region[j]++;
                }
            else{
                int* superbird;
                superbird=(int*)image;
                for(i=0;i<vol;i++)if(superbird[i])reg->nvoxels_region[superbird[i]]++;
                }


            }

        //std::cout<<"reg->nvoxels_region=";for(i=0;i<reg->nregions;++i)std::cout<<reg->nvoxels_region[i]<<" ";std::cout<<std::endl;
        //if(reg_val){std::cout<<"reg_val=";for(i=0;i<reg->nregions;++i)std::cout<<reg_val[i]<<" ";std::cout<<std::endl;}
        //std::cout<<"reg->nregions="<<reg->nregions<<std::endl;

        if(!(reg->voxel_indices=d2intvar(reg->nregions,reg->nvoxels_region))) return NULL;
        std::vector<int> count(reg->nregions,0);

        if(reg_val){
            if(filetype==(int)IMG){
                float* superbird;
                superbird=(float*)image;
                for(i=0;i<vol;i++)
                    for(j=0;j<reg->nregions;j++)
                        if(superbird[i] == reg_val[j]){
                            reg->voxel_indices[j][count[j]] = i;
                            count[j]++;
                            break;
                            }
                }
            else{
                int* superbird;
                superbird=(int*)image;
                for(i=0;i<vol;i++)
                    for(j=0;j<reg->nregions;j++)
                        if(superbird[i] == reg_val[j]){
                            reg->voxel_indices[j][count[j]] = i;
                            count[j]++;
                            break;
                            }
                }
            }
        else {
            if(!lutf){ 
                float *superbird;
                superbird=(float*)image; 
                for(i=0;i<vol;i++)
                    for(j=0;j<reg->nregions;j++)
                        if(fabsf(superbird[i]) > (float)UNSAMPLED_VOXEL) {
                            reg->voxel_indices[j][count[j]] = i;
                            count[j]++;
                            break; 
                            }
                }
            else{
                int* superbird;
                superbird=(int*)image;
                for(i=0;i<vol;i++)
                    if(superbird[i]){
                        reg->voxel_indices[superbird[i]][count[superbird[i]]]=i;
                        count[superbird[i]]++;
                        }
                }

            }

        //START200224
        for(reg->nvoxels=i=0;i<reg->nregions;++i)reg->nvoxels+=reg->nvoxels_region[i]; 

        }/*if(image)*/
    return reg;
}
void free_reg(Regions *reg)
{
    if(reg->region_names) {
        free(reg->region_names[0]);
        free(reg->region_names);
        }
    if(reg->length) free(reg->length);
    free(reg->harolds_num);
    free(reg->voxel_indices[0]);
    free(reg->voxel_indices);
    free(reg->nvoxels_region);
    free(reg);
}
