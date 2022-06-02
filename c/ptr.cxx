/* Copyright 9/26/19 Washington University.  All Rights Reserved.
   ptr.cxx  $Revision: 1.1 $ */
#include <iostream>
#include "ptr.h"
#include "filetype.h"
#include "shouldiswap.h"
#include "subs_nifti.h"
#include "subs_util.h"

ptr::ptr(){
    SunOS_Linux=1;
    }

int ptr::ptr0(char *file){
    if(!header0(file))return 0;

    if(filetype==(int)IMG){
        if(!(mm=map_disk(file,dim[3]*vol,0,sizeof(float))))return 0;
        swapbytes=shouldiswap(SunOS_Linux,bigendian);
        }
    else if(filetype==(int)NIFTI){
        if(!(niftiPtr=nifti_openRead(file)))return 0;
        }
    else{
        std::cout<<"fidlError: Unknown file type  ptr.cxx ptr0"<<std::endl;
        return 0;
        }
    return 1;
    }

int ptr::ptrstack(float *temp_float,int idx){
    if(filetype==(int)IMG){

        //int p=vol*idx;
        //for(int j=0;j<vol;j++)temp_float[j] = mm->ptr[p+j];
        float *mm0;
        mm0=&mm->ptr[vol*idx];
        for(int j=0;j<vol;++j)*temp_float++ = *mm0++;

        if(swapbytes)swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)vol);
        }
    else if(filetype==(int)NIFTI){
        if(!nifti_getvol(niftiPtr,(int64_t)idx,temp_float))return 0;
        }
    return 1;
    }

int ptr::ptrfree(){
    if(filetype==(int)IMG){
        //if(!unmap_disk(mm))return 0;
        //START211207
        if(unmap_disk(mm))return 0;
        }
    else if(filetype==(int)NIFTI){
        nifti_free(niftiPtr);
        }
    return 1;
    }
