/* Copyright 1/23/21 Washington University.  All Rights Reserved.
   ptrw.cxx  $Revision: 1.1 $ */
#include <iostream>
#include "NiftiIO.h"
#include "ptrw.h"
#include "filetype.h"
#include "subs_nifti.h"
#include "subs_util.h"
#include "minmax.h"
using namespace cifti;

ptrw::ptrw(){
    SunOS_Linux=1;
    void* ret=NULL;
    }

int ptrw::ptrw0(char* file0,W1* w1){
    file=file0;
    filetype=w1->filetype;

    if(filetype==(int)IMG){
        min_and_max_init(&min,&max);
        if(!(ret=fopen_sub(file,"w")))return 0;
        }
    else if(filetype==(int)NIFTI){
        if(!(ret=nifti_writeNew(file,w1->dims,w1->center,w1->mmppix)))return 0;
        }
    else{
        std::cout<<"fidlError: Unknown file type  ptrw.cxx ptr0"<<std::endl;
        return 0;
        }
    return 1;
    }

int ptrw::ptrwstack(float *temp_float,int how_many,int idx){
    if(filetype==(int)IMG){
        min_and_max_new(temp_float,how_many,&min,&max,0);
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)how_many,(FILE*)ret,0)){
            std::cout<<"fidlError: Could not write to "<<file<<"  ptrw.cxx ptrw1"<<std::endl;
            return 0;
            }
        }
    else if(filetype==(int)NIFTI){
        if(!nifti_putvol(ret,(int64_t)idx,temp_float)){
            std::cout<<"fidlError: Could not write to "<<file<<"  ptrw.cxx ptrw1"<<std::endl;
            return 0;
            }
        }
    return 1;
    }

char* ptrw::ptrwfree(Interfile_header *ifh){
    std::string str(file);
    if(filetype==(int)IMG){
        fclose((FILE*)ret);
        ifh->global_min=min;ifh->global_max=max;
        if(!write_ifh(file,ifh,0))return NULL;
        } 
    else if(filetype==(int)NIFTI){
        delete (NiftiIO*)ret;
        std::string str2="gzip -f "+str;
        if(system(str2.c_str())==-1){
            std::cout<<"fidlError: "<<str2.c_str()<<std::endl;
            return NULL;
            }
        else
            strcat(file,".gz"); 
        }
    return file;
    }
