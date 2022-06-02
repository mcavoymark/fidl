/* Copyright 10/9/19 Washington University.  All Rights Reserved.
   rbf.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <set>
#include "fidl.h"
#include "region.h"
#include "subs_util.h"
#include "constants.h"
#include "d2intvar.h"
#include "stack.h"
#include "filetype.h"
#include "lut.h"
#include "nifti1.h"
using namespace cifti;

reg::reg(){
    nregions=0;
    voxel_indices=NULL;
    region_names=NULL;
    }
reg::~reg(){
    if(region_names) {
        free(region_names[0]);
        free(region_names);
        }
    if(voxel_indices){
        free(voxel_indices[0]);
        free(voxel_indices);
        }
    }
int reg::reg0(char *region_file,int lcflip){
    char *strptr=NULL,string[MAXNAME];
    std::vector<float> image0;
    std::vector<unsigned short> image1;
    std::set<float> s0;
    std::set<unsigned short> s1;
    lut l0;char **FS=NULL; 
    if(!(header0(region_file)))return 0;
    int lcgetstack=filetype==(int)IMG&&lcnamesonly?0:1;
    if(lcgetstack){
        if(datatype==(int16_t)NIFTI_TYPE_FLOAT32){
            image0.resize(vol); 
            if(!(h.getstack(image0.data())))return 0;
            if(lcflip){
                int cf_flip;
                if((cf_flip=h.flip(image0.data()))==-1)return 0;
                c_orient[0]=0;c_orient[1]=5;c_orient[2]=2; 
                } 
            image=image0.data();
            }
        else if(datatype==(int16_t)NIFTI_TYPE_UINT16){
            image1.resize(vol); 
            if(!(h.getstack(image1.data())))return NULL;
            if(lcflip){
                int cf_flip;
                if((cf_flip=h.flip(image1.data()))==-1)return NULL;
                c_orient[0]=0;c_orient[1]=5;c_orient[2]=2; 
                } 
            image=image1.data();
            }
         }
    if(filetype==(int)IMG){
        nregions = ifh->nregions;
        }
    else if(filetype==(int)NIFTI){
        if(datatype==(int16_t)NIFTI_TYPE_FLOAT32){
            s0.insert(image0.data(),image0.data()+vol);
            nregions = s0.size()-1;
            }
        else if(datatype==(int16_t)NIFTI_TYPE_UINT16){
            s1.insert(image1.data(),image1.data()+vol);
            nregions = s1.size()-1;
            }
        FS=l0.lut1(region_file);
        }
    std::vector<int> regval(nregions);
    if(filetype==(int)IMG){
        for(i=0;i<nregions;++i)regval[i]=i+2;
        }
    else{
        i=0;
        if(datatype==(int16_t)NIFTI_TYPE_FLOAT32)for(auto it=s0.begin();++it!=s0.end();)regval[i++]=(int)*it;
        else if(datatype==(int16_t)NIFTI_TYPE_UINT16)for(auto it=s1.begin();++it!=s1.end();)regval[i++]=(int)*it;
        }
    if(!lcnamesonly(if(!(reg=get_reg_guts(image,regval.data())))return 0;
    length.resize(nregions,0);
    if(filetype==(int)IMG){
        for(i=0;i<nregions;i++) {
            strcpy(string,ifh->region_names[i]); /*gets decimated*/
            if(!(strptr=strtok(string," "))) {
                std::cout<<"fidlError: No token found - spot 1. Abort!"<<std::endl;
                return 0;
                }
            if(!(strptr = strtok((char*)NULL," "))) {
                std::cout<<"fidlError: No token found - spot 2. Abort!"<<std::endl;
                return 0;
                }
            length[i] = strlen(strptr) + 1;
            }
        }
    else{ 
        for(i=0;i<nregions;++i)length[i]=strlen(FS[regval[i]])+1;
        }
    if(!(region_names=d2charvar(nregions,length.data())))return 0;
    if(filetype==(int)IMG){
        for(i=0;i<reg->nregions;i++) {
            strcpy(string,ifh->region_names[i]); /*gets decimated*/
            if(!(strptr=strtok(string," "))) {
                std::cout<<"fidlError: No token found - spot 1. Abort!"<<std::endl;
                return 0;
                }
            if(!(strptr = strtok((char*)NULL," "))) {
                std::cout<<"fidlError: No token found - spot 2. Abort!"<<std::endl;
                return 0;
                }
            strcpy(region_names[i],strptr);
            if((strptr=strtok((char*)NULL," ")))reg->nvoxels_region[i]=(int)strtol(strptr,NULL,10);
            }
        }
    else{
        for(i=0;i<reg->nregions;++i)strcpy(reg->region_names[i],FS[regval[i]]);
        }
    }

int reg::reg0_guts(void* image,int* regval){
    int i,j;
    nvoxels_region.resize(nregions,0);
    if(datatype==(int16_t)NIFTI_TYPE_FLOAT32){
        float *superbird;
        superbird=(float*)image; 
        for(i=0;i<vol;++i)
            for(j=0;j<nregions;++j)
                if((int)superbird[i] == regval[j])nvoxels_region[j]++;
        }
    else if(datatype==(int16_t)NIFTI_TYPE_UINT16){
        unsigned short* superbird;
        superbird=(unsigned short*)image; 
        for(i=0;i<vol;++i)
            for(j=0;j<nregions;++j)
                if((int)superbird[i] == regval[j])nvoxels_region[j]++;
        }
    if(!(voxel_indices=d2intvar(nregions,nvoxels_region.data())))return 0;
    std::vector<int> count(nregions,0);
    if(datatype==(int16_t)NIFTI_TYPE_FLOAT32){
        float *superbird;
        superbird=(float*)image;
        for(i=0;i<vol;++i)
            for(j=0;j<nregions;++j)
                if((int)superbird[i] == regval[j]){
                    voxel_indices[j][count[j]] = i;
                    count[j]++;
                    break; 
                    }
        }
    else if(datatype==(int16_t)NIFTI_TYPE_UINT16){
        unsigned short* superbird;
        superbird=(unsigned short*)image;
        for(i=0;i<vol;++i)
            for(j=0;j<nregions;++j)
                if((int)superbird[i] == regval[j]){
                    voxel_indices[j][count[j]] = i;
                    count[j]++;
                    break; 
                    }
        }
}
