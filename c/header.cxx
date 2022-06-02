/* Copyright 7/13/17 Washington University.  All Rights Reserved.
   header.cxx  $Revision: 1.1 $*/
#include <iostream>
#include <inttypes.h>
#include <math.h>

//START190926
#include "nifti1.h"
using namespace cifti;

#include "nifti_gethdr.h"
#include "filetype.h"
#include "header.h"
#include "cifti_getDimensions.h"

#if 0
//START190327
header::header(){
    c_orient[0]=0;c_orient[1]=0;c_orient[2]=0;
    }
#endif
//START190917
header::header(){
    c_orient[0]=-1;c_orient[1]=-1;c_orient[2]=-1;

    //START190927
    ifh=NULL;
    }

//START190927
header::~header(){

    //std::cout<<"~header() here0"<<std::endl;

    //if(filetype==(int)IMG)free_ifh(ifh,0);
    //START200107
    if(ifh)free_ifh(ifh,0);

    //std::cout<<"~header() here1"<<std::endl;

    }

int header::header0(char* file0){
    file=file0;
    if(!(filetype=get_filetype(file)))return 0;
    if(filetype==(int)GLM){
        dim[0]=dim[1]=dim[2]=0;
        }
    else if(filetype==(int)IMG){

        if(!(ifh=read_ifh(file,(Interfile_header*)NULL)))return 0;
        //START190927
        //Interfile_header ifh0; 
        //if(!(ifh=read_ifh(file,&ifh0)))return 0;

        dim[0]=ifh->dim1;dim[1]=ifh->dim2;dim[2]=ifh->dim3;dim[3]=ifh->dim4<1?1:ifh->dim4;
        voxel_size[0]=ifh->voxel_size_1;voxel_size[1]=ifh->voxel_size_2;voxel_size[2]=ifh->voxel_size_3;
        for(int i=0;i<3;i++){centerf[i]=ifh->center[i];mmppixf[i]=ifh->mmppix[i];}
        bigendian=ifh->bigendian;
        number_format=ifh->number_format;
        if((orientation=ifh->orientation) != (int)TRANSVERSE) {
            std::cout<<"ifh->orientation = "<<ifh->orientation<<" Needs to be transverse."<<std::endl;
            return 0;
            }
        c_orient[0]=0;c_orient[1]=5;c_orient[2]=2;
        datatype=(int16_t)NIFTI_TYPE_FLOAT32;
        
        //START211027
        datatypestr.assign("NIFTI_TYPE_FLOAT32");
        }
    else if(filetype==(int)NIFTI){

        #if 0
        //START190419
        if(!(nifti_gethdr(file,&dim64[0],&dim64[1],&dim64[2],&dim64[3],centerf,mmppixf,c_orient)))return 0;
        dim[0]=(int)dim64[0];dim[1]=(int)dim64[1];dim[2]=(int)dim64[2];dim[3]=(int)dim64[3];
        for(int i=0;i<3;i++)voxel_size[i]=fabsf(mmppixf[i]);
        #endif
        //START190924
        NiftiS niftis; 
        if(!nifti_gethdr(file,&niftis))return 0;
        for(int i=0;i<3;++i){
            dim64[i]=niftis.dims[i];
            dim[i]=(int)niftis.dims[i];
            centerf[i]=niftis.center[i];
            mmppixf[i]=niftis.mmppix[i];
            c_orient[i]=niftis.c_orient[i];
            voxel_size[i]=fabsf(niftis.mmppix[i]);
            }

        //START211019
        dim64[3]=niftis.dims[3];

        dim[3]=(int)niftis.dims[3];
        datatype=niftis.datatype;

        //START211019
        datatypestr.assign(niftis.datatypestr);

        }
    else if(filetype==(int)CIFTI||filetype==(int)CIFTI_DTSERIES){
        int64_t tdim,xdim;
        if(!cifti_getDimensions(file,&tdim,&xdim))return 0;
        dim[0]=(int)xdim;dim[1]=dim[2]=1;dim[3]=(int)tdim;
        }
    
    //START220308
    lenvol=dim[0]*dim[1]*dim[2]*dim[3];
    
    return vol=dim[0]*dim[1]*dim[2];
    }

void header::assign(int& filetype0){
    filetype0=filetype;
    }

//START190910
void header::assign(int& filetype0,int *c_orient0){
    filetype0=filetype;
    for(int i=0;i<3;i++)c_orient0[i]=c_orient[i];
    }

//START190925
void header::assign(int& filetype0,int *c_orient0,int16_t& datatype0){
    filetype0=filetype;
    for(int i=0;i<3;i++)c_orient0[i]=c_orient[i];
    datatype0=datatype;
    }

void header::assign(int& xdim,int& ydim,int& zdim){
    xdim=dim[0];ydim=dim[1];zdim=dim[2];
    }
void header::assign(int& xdim,int& ydim,int& zdim,float& voxel_size_1,float& voxel_size_2,float& voxel_size_3,float *center,
    float *mmppix){
    xdim=dim[0];ydim=dim[1];zdim=dim[2];
    voxel_size_1=voxel_size[0];voxel_size_2=voxel_size[1];voxel_size_3=voxel_size[2];
    for(int i=0;i<3;i++){center[i]=centerf[i];mmppix[i]=mmppixf[i];}
    }
void header::assign(int& xdim,int& ydim,int& zdim,int& tdim,float& voxel_size_1,float& voxel_size_2,float& voxel_size_3,
    float *center,float *mmppix,int& bigendian0,int& orientation0,int *c_orient0){
    xdim=dim[0];ydim=dim[1];zdim=dim[2];tdim=dim[3];
    voxel_size_1=voxel_size[0];voxel_size_2=voxel_size[1];voxel_size_3=voxel_size[2];
    for(int i=0;i<3;i++){center[i]=centerf[i];mmppix[i]=mmppixf[i];}
    bigendian0=bigendian;orientation0=orientation; 
    for(int i=0;i<3;i++)c_orient0[i]=c_orient[i];
    }

//START210422
#if 0
void header::assign(int& xdim,int& ydim,int& zdim,int& tdim,float& voxel_size_1,float& voxel_size_2,float& voxel_size_3,
    float *center,float *mmppix,int& bigendian0,int& orientation0,int *c_orient0,int& number_format0){
    xdim=dim[0];ydim=dim[1];zdim=dim[2];tdim=dim[3];
    voxel_size_1=voxel_size[0];voxel_size_2=voxel_size[1];voxel_size_3=voxel_size[2];
    for(int i=0;i<3;i++){center[i]=centerf[i];mmppix[i]=mmppixf[i];}
    bigendian0=bigendian;orientation0=orientation;
    for(int i=0;i<3;i++)c_orient0[i]=c_orient[i];
    number_format0=number_format;
    }
#endif

//START190419
void header::assign(float *center,float *mmppix){
    for(int i=0;i<3;i++){center[i]=centerf[i];mmppix[i]=mmppixf[i];}
    }
//START190509
void header::assign(float& voxel_size_1,float& voxel_size_2,float& voxel_size_3,float *center,float *mmppix){
    voxel_size_1=voxel_size[0];voxel_size_2=voxel_size[1];voxel_size_3=voxel_size[2];
    for(int i=0;i<3;i++){center[i]=centerf[i];mmppix[i]=mmppixf[i];}
    }

//START190920
void header::assign(int64_t *dim640,float *center,float *mmppix){
    #if 0
    dim640[0]=dim64[0];dim640[1]=dim64[1];dim640[2]=dim64[2];dim640[3]=dim64[3];
    for(int i=0;i<3;i++){center[i]=centerf[i];mmppix[i]=mmppixf[i];}
    #endif
    //START191007
    for(int i=0;i<3;i++){dim640[i]=dim64[i];center[i]=centerf[i];mmppix[i]=mmppixf[i];}dim640[3]=dim64[3];
    }

//START191007
void header::assign(int64_t *dim640,float *center,float *mmppix,int *c_orient0){
    for(int i=0;i<3;i++){dim640[i]=dim64[i];center[i]=centerf[i];mmppix[i]=mmppixf[i];c_orient0[i]=c_orient[i];}dim640[3]=dim64[3];
    }

