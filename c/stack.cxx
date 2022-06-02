/* Copyright 2/21/18 Washington University.  All Rights Reserved.
   stack.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "checkOS.h"
#include "stack.h"
#include "nifti_flipslice.h"
#include "filetype.h"
#include "subs_util.h"
#include "subs_nifti.h"

stack::stack(){
    SunOS_Linux=1;
    f=NULL;us=NULL;i0=NULL; 
    }

//START190930
stack::~stack(){

    //std::cout<<"~stack() here0"<<std::endl;

    if(f)delete[] f;

    //std::cout<<"~stack() here1"<<std::endl;

    if(us)delete[] us;

    //std::cout<<"~stack() here2"<<std::endl;

    if(i0)delete[] i0;

    //std::cout<<"~stack() here3"<<std::endl;
    }


float* stack::stack0(char *file){
    if(!header0(file))return NULL;
    try{

        //f=new float[vol];
        //START220308
        f=new float[lenvol];

        }
    catch(std::bad_alloc& ba){
        std::cout<<"stack0 vol="<<vol<<" bad_alloc caught: "<<ba.what()<<std::endl;
        return NULL;
        }

    if(!(getstack(f)))return NULL;
    return f;
    }

//START191001
float* stack::stack0(){
    if(!header0(file))return NULL;
    try{
        f=new float[vol];
        }
    catch(std::bad_alloc& ba){
        std::cout<<"stack0 vol="<<vol<<" bad_alloc caught: "<<ba.what()<<std::endl;
        return NULL;
        }

    if(!(getstack(f)))return NULL;
    return f;
    }

unsigned short* stack::stack0us(char *file){
    if(!header0(file))return NULL;
    try{
        us=new unsigned short[vol];
        }
    catch(std::bad_alloc& ba){
        std::cout<<"stack0 vol="<<vol<<" bad_alloc caught: "<<ba.what()<<std::endl;
        return NULL;
        }

    if(!(getstack(us)))return NULL;
    return us;
    }

//START191001
unsigned short* stack::stack0us(){
    if(!header0(file))return NULL;
    try{
        us=new unsigned short[vol];
        }
    catch(std::bad_alloc& ba){
        std::cout<<"stack0 vol="<<vol<<" bad_alloc caught: "<<ba.what()<<std::endl;
        return NULL;
        }
    if(!(getstack(us)))return NULL;
    return us;
    }

#if 1
//START200311
int* stack::stack0int(char *file){
    if(!header0(file))return NULL;
    try{
        i0=new int[vol];
        }
    catch(std::bad_alloc& ba){
        std::cout<<"stack0int vol="<<vol<<" bad_alloc caught: "<<ba.what()<<std::endl;
        return NULL;
        }
    if(!(getstack(i0)))return NULL;
    return i0;
    }
#endif

int stack::stack1(char *file,float *temp_float){
    if(!header0(file))return 0;
    if(!(getstack(temp_float)))return 0;
    return 1;
    }
int stack::getstack(float* stack){
    if(filetype==(int)GLM){
        /*do nothing*/
        }
    else if(filetype==(int)IMG){
        if(!readstack(file,(float*)stack,sizeof*stack,(size_t)vol,SunOS_Linux,bigendian))return 0;
        }
    else if(filetype==(int)NIFTI){
        if(!nifti_read(file,stack))return 0;
        }
    return 1;
}
int stack::getstack(unsigned short* stack){
    if(filetype==(int)GLM){
        /*do nothing*/
        }
    else if(filetype==(int)IMG){
        if(!readstack(file,(unsigned short*)stack,sizeof*stack,(size_t)vol,SunOS_Linux,bigendian))return 0;
        }
    else if(filetype==(int)NIFTI){
        if(!nifti_readus(file,stack))return 0;
        }
    return 1;
    }

//START200108
int stack::getstack(int* stack){
    if(filetype==(int)GLM){
        /*do nothing*/
        }
    else if(filetype==(int)IMG){
        if(!readstack(file,(int*)stack,sizeof*stack,(size_t)vol,SunOS_Linux,bigendian))return 0;
        }
    else if(filetype==(int)NIFTI){
        if(!nifti_readsi(file,stack))return 0;
        }
    return 1;
    }



int stack::flip(float* f0){
    int cf_flip;
    if((cf_flip=c_orient[0]*100+c_orient[1]*10+c_orient[2])!=52){
        int64_t i64;
        float *fptr;
        for(fptr=f0,i64=0;i64<dim64[2];i64++,fptr+=dim64[0]*dim64[1]){
            if(!nifti_flipslice(dim64[0],dim64[1],cf_flip,fptr))return -1;
            }
        }
    return cf_flip;
    }

//START190926
int stack::flip(unsigned short* f0){
    int cf_flip;
    if((cf_flip=c_orient[0]*100+c_orient[1]*10+c_orient[2])!=52){
        int64_t i64;
        unsigned short *fptr;
        for(fptr=f0,i64=0;i64<dim64[2];i64++,fptr+=dim64[0]*dim64[1]){
            if(!nifti_flipsliceus(dim64[0],dim64[1],cf_flip,fptr))return -1;
            }
        }
    return cf_flip;
    }

//START201008
int stack::flip(int* f0){
    int cf_flip;
    if((cf_flip=c_orient[0]*100+c_orient[1]*10+c_orient[2])!=52){
        int64_t i64;
        int *fptr;
        for(fptr=f0,i64=0;i64<dim64[2];i64++,fptr+=dim64[0]*dim64[1]){
            if(!nifti_flipslicesi(dim64[0],dim64[1],cf_flip,fptr))return -1;
            }
        }
    return cf_flip;
    }



//KEEP
#if 0

//START180223
//float* stack::flip(){
//float* stack::flip(float* f0){
//START180226
int stack::flip(float* f0){
    char string[4];
    int cf_flip;
    int* c_orient=get_c_orient();
    sprintf(string,"%d%d%d",c_orient[0],c_orient[1],c_orient[2]);
    if((cf_flip=(int)strtol(string,NULL,10))!=52){
        int64_t* dim64=get_dim64();
        int64_t i64;
        float *fptr; 

        #if 0
        header::getvol(f,1);
        cout<<"here6"<<endl;
        cout<<"here6 f0[5399059]="<<f0[5399059]<<endl;
        cout<<"here6 f[5399059]="<<f[5399059]<<endl;

        try{
            float* f=new float[vol];
            header::getvol(f,1);
            }
        catch(bad_alloc& ba){
            cout<<"bad_alloc caught: "<<ba.what()<<endl;
            return NULL;
            }
        #endif

        #if 0
        //Doens't work. f is not preserved.
        for(fptr=f,i64=0;i64<dim64[2];i64++,fptr+=dim64[0]*dim64[1]){
            if(!nifti_flipslice(dim64[0],dim64[1],cf_flip,fptr))return NULL;
            }
        #endif
        for(fptr=f0,i64=0;i64<dim64[2];i64++,fptr+=dim64[0]*dim64[1]){
            //if(!nifti_flipslice(dim64[0],dim64[1],cf_flip,fptr))return NULL;
            //START180226
            if(!nifti_flipslice(dim64[0],dim64[1],cf_flip,fptr))return -1;
            }
        }
    //return f0;
    //START180226
    return cf_flip;
    }

#endif
