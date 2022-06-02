/* Copyright 9/30/19 Washington University.  All Rights Reserved.
   coor.cxx  $Revision: 1.1 $*/
#include <vector> 
#include <cstring> 
#include <iostream> 
#include "coor.h"
#include "get_atlas_coor.h"
#include "get_atlas.h"
#include "filetype.h"
#include "getxyz.h"
#include "nifti_getmni.h"

coor::coor(){
    coorf=NULL;
    }
coor::~coor(){
    if(coorf)delete[] coorf;
    }

float* coor::coor0(char *file){
    if(!read_mask(file,(LinearModel*)NULL))return NULL; 

    std::vector<double> col(vol),row(vol),slice(vol);
    col_row_slice2(lenbrain,brnidx,col.data(),row.data(),slice.data(),dim[0],dim[1]);

    coorf=new float[lenbrain*3];

    char atlas[7]="";
    get_atlas(vol,atlas);
    if(!strstr(atlas,"MNI")&&filetype==(int)IMG){
        std::cout<<"fidlInfo: Calling get_atlas_coorff in coor.cxx"<<std::endl;
        get_atlas_coorff(lenbrain,col.data(),row.data(),slice.data(),(double)dim[2],centerf,mmppixf,coorf);
        }
    else{
        int cf_flip=get_cf_flip();

        std::vector<float> xyz(vol);

        getxyz(lenbrain,col.data(),row.data(),slice.data(),cf_flip,dim[1],xyz.data(),atlas);

        if(!nifti_getmnis(file,xyz.data(),coorf,lenbrain)){
            delete[] coorf;
            coorf=(float*)NULL;
            }
        }
    return coorf;
    }
