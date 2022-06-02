/* Copyright 6/23/15 Washington University.  All Rights Reserved.
   nifti_gethdr.cxx  $Revision: 1.7 $*/
//#include <stdint.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "CiftiException.h"
#include "NiftiIO.h"
#include "VolumeSpace.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include "nifti_gethdr.h"

//using namespace std;
//START190924

using namespace cifti;

//int nifti_gethdr(char *file,int64_t *xdim,int64_t *ydim,int64_t *zdim,int64_t *tdim,float *center,float *mmppix,int *c_orient){
//START190924
//NiftiS* nifti_gethdr(char* file){
int nifti_gethdr(char* file,NiftiS *niftis){

    //NiftiS* niftis=NULL;

    try{
        NiftiIO inputFile;
        inputFile.openRead(file);
        int nc;
        if((nc=inputFile.getNumComponents())!=1){
            std::cerr<<"fidlError: "<<file<<" getNumComponents()="<<nc<<" Only one is allowed."<<std::endl;

            return 0;
            //START190924
            //return NULL;

            }
        if(inputFile.getDimensions().size()<3||inputFile.getDimensions().size()>4){
            std::cerr<<"fidlError: "<<file<<" Not a 3D or 4D volume"<<std::endl;
            std::cerr<<"fidlError: dims= ";
            std::vector<int64_t> dims=inputFile.getDimensions();
            for(std::vector<int64_t>::iterator i=dims.begin();i!=dims.end();++i)std::cerr<<*i<<' ';
            std::cerr<<std::endl;

            return 0;
            //START190924
            //return NULL;
            }

        const NiftiHeader& myHeader=inputFile.getHeader();
        VolumeSpace myVolSpace(myHeader.getDimensions().data(),myHeader.getSForm());

        #if 0
        *xdim = inputFile.getDimensions()[0];
        *ydim = inputFile.getDimensions()[1];
        *zdim = inputFile.getDimensions()[2];
        #endif
        //START190924
        //niftis = new NiftiS[1];

        niftis->dims[0] = inputFile.getDimensions()[0];
        niftis->dims[1] = inputFile.getDimensions()[1];
        niftis->dims[2] = inputFile.getDimensions()[2];
        niftis->dims[3] = inputFile.getDimensions().size()>3?inputFile.getDimensions()[3]:1; 

        VolumeSpace::OrientTypes orient[3];

        if(!myVolSpace.isPlumb()){
            std::cerr<<"fidlInfo: "<<file<<" oblique volume"<<std::endl;

            //std::cerr<<"fidlInfo: "<<*xdim<<" "<<*ydim<<" "<<*zdim<<std::endl;
            //START190925
            std::cerr<<"fidlInfo: "<<niftis->dims[0]<<" "<<niftis->dims[1]<<" "<<niftis->dims[2]<<std::endl;

            myVolSpace.getOrientation(orient);
            }
        else{

            //myVolSpace.getOrientAndSpacingForPlumb(orient,mmppix,center);
            //START190924
            myVolSpace.getOrientAndSpacingForPlumb(orient,niftis->mmppix,niftis->center);
            //myVolSpace.getOrientAndSpacingForPlumb(orient,&niftis->mmppix[0],&niftis->center[0]);
            #if 0
            float mmppix[3],center[3];
            myVolSpace.getOrientAndSpacingForPlumb(orient,mmppix,center);
            std::cout<<"here405"<<std::endl;
            for(int i=0;i<3;++i){niftis->mmppix[i]=mmppix[i];niftis->center[i]=center[i];}
            #endif

            }

        #if 0
        if(c_orient){
            c_orient[0]=(intptr_t)orient[0];
            c_orient[1]=(intptr_t)orient[1];
            c_orient[2]=(intptr_t)orient[2];
            }
        #endif
        //START190924
        for(int i=0;i<3;++i)niftis->c_orient[i]=(intptr_t)orient[i];
        niftis->datatype=myHeader.getDataType();




        //cout<<file<<endl;
        //if(c_orient){cout<<"orient[0]="<<orient[0]<<" orient[1]="<<orient[1]<<" orient[2]="<<orient[2]<<endl;}
        //cout<<"mmppix[0]="<<mmppix[0]<<" mmppix[1]="<<mmppix[1]<<" mmppix[2]="<<mmppix[2]<<endl;
        //cout<<"center[0]="<<center[0]<<" center[1]="<<center[1]<<" center[2]="<<center[2]<<endl;

        //START150708
        #if 0
        vector<vector<float> > sform=myHeader.getSForm();
        for(vector<vector<float> >::iterator i=sform.begin();i!=sform.end();++i){
            for(vector<float>::iterator j=i->begin();j!=i->end();++j)cout<<*j<<' ';
            cout<<endl;
            }

        //START150702
        size_t k=0;
        for(vector<vector<float> >::iterator i=sform.begin();i!=sform.end()-1;++i){
            for(vector<float>::iterator j=i->begin();j!=i->end();++j,++k)c_sform[k]=*j;
            }
        #endif

        #if 0
        //KEEP
        vector<vector<float> > sform=myHeader.getSForm();
        cout<<"sform"<<endl;
        for(size_t i=0;i<sform.size();++i){
            for(size_t j=0;j<sform[i].size();++j)cout<<sform[i][j]<<' ';
            cout<<endl;
            }
        #endif


        //START170620
        //cout << "nifti_gethdr inputFile.numBytesPerElem()=" << inputFile.numBytesPerElem() << endl;
        //cout << "nifti_gethdr myHeader.getDataType()=" << myHeader.getDataType() << endl;
        #if 0
        switch (myHeader.getDataType())
        {
            case NIFTI_TYPE_UINT8:
                std::cout << "NIFTI_TYPE_UINT8" << std::endl;
                break;
            case NIFTI_TYPE_RGB24:
                std::cout << "NIFTI_TYPE_RGB24" << std::endl;
                break;
            case NIFTI_TYPE_INT8:
                std::cout << "NIFTI_TYPE_INT8" << std::endl;
                break;
            case NIFTI_TYPE_UINT16:
                std::cout << "NIFTI_TYPE_UINT16" << std::endl;
                break;
            case NIFTI_TYPE_INT16:
                std::cout << "NIFTI_TYPE_INT16" << std::endl;
                break;
            case NIFTI_TYPE_UINT32:
                std::cout << "NIFTI_TYPE_UINT32" << std::endl;
                break;
            case NIFTI_TYPE_INT32:
                std::cout << "NIFTI_TYPE_UINT32" << std::endl;
                break;
            case NIFTI_TYPE_UINT64:
                std::cout << "NIFTI_TYPE_UINT64" << std::endl;
                break;
            case NIFTI_TYPE_INT64:
                std::cout << "NIFTI_TYPE_INT64" << std::endl;
                break;
            case NIFTI_TYPE_FLOAT32:
                std::cout << "NIFTI_TYPE_FLOAT32" << std::endl;
                break;
            case NIFTI_TYPE_COMPLEX64:
                std::cout << "NIFTI_TYPE_COMPLEX64" << std::endl;
                break;
            case NIFTI_TYPE_FLOAT64:
                std::cout << "NIFTI_TYPE_FLOAT64" << std::endl;
                break;
            case NIFTI_TYPE_COMPLEX128:
                std::cout << "NIFTI_TYPE_COMPLEX128" << std::endl;
                break;
            case NIFTI_TYPE_FLOAT128:
                std::cout << "NIFTI_TYPE_FLOAT128" << std::endl;
                break;
            case NIFTI_TYPE_COMPLEX256:
                std::cout << "NIFTI_TYPE_COMPLEX256" << std::endl;
                break;
            default:
                CiftiAssert(0);
                throw CiftiException("internal error, tell the developers what you just tried to do");
        }
        #endif
        //START211019
        switch (myHeader.getDataType())
        {
            case NIFTI_TYPE_UINT8:
                //std::cout << "NIFTI_TYPE_UINT8" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_UINT8");
                break;
            case NIFTI_TYPE_RGB24:
                //std::cout << "NIFTI_TYPE_RGB24" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_RGB24");
                break;
            case NIFTI_TYPE_INT8:
                //std::cout << "NIFTI_TYPE_INT8" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_INT8");
                break;
            case NIFTI_TYPE_UINT16:
                //std::cout << "NIFTI_TYPE_UINT16" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_UINT16");
                break;
            case NIFTI_TYPE_INT16:
                //std::cout << "NIFTI_TYPE_INT16" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_INT16");
                break;
            case NIFTI_TYPE_UINT32:
                //std::cout << "NIFTI_TYPE_UINT32" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_UINT32");
                break;
            case NIFTI_TYPE_INT32:
                //std::cout << "NIFTI_TYPE_INT32" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_INT32");
                break;
            case NIFTI_TYPE_UINT64:
                //std::cout << "NIFTI_TYPE_UINT64" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_UINT64");
                break;
            case NIFTI_TYPE_INT64:
                //std::cout << "NIFTI_TYPE_INT64" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_INT64");
                break;
            case NIFTI_TYPE_FLOAT32:
                //std::cout << "NIFTI_TYPE_FLOAT32" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_FLOAT32");
                break;
            case NIFTI_TYPE_COMPLEX64:
                //std::cout << "NIFTI_TYPE_COMPLEX64" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_COMPLEX64");
                break;
            case NIFTI_TYPE_FLOAT64:
                //std::cout << "NIFTI_TYPE_FLOAT64" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_FLOAT64");
                break;
            case NIFTI_TYPE_COMPLEX128:
                //std::cout << "NIFTI_TYPE_COMPLEX128" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_COMPLEX128");
                break;
            case NIFTI_TYPE_FLOAT128:
                //std::cout << "NIFTI_TYPE_FLOAT128" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_FLOAT128");
                break;
            case NIFTI_TYPE_COMPLEX256:
                //std::cout << "NIFTI_TYPE_COMPLEX256" << std::endl;
                strcpy(niftis->datatypestr,"NIFTI_TYPE_COMPLEX256");
                break;
            default:
                CiftiAssert(0);
                throw CiftiException("internal error, tell the developers what you just tried to do");
        }

        //cout<<"nifti_gethdr xdim="<<*xdim<<" ydim="<<*ydim<<" zdim="<<*zdim<<" tdim="<<*tdim<<endl;

        inputFile.close();
    } catch (CiftiException& e) {
        std::cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << std::endl;

        return 0;
        //START190924
        //if(niftis)nifti_delhdr(niftis); 
    }

    //cout<<"nifti_gethdr bottom"<<endl;

    return 1;
    //START190924
    //return niftis;
}
int _nifti_gethdr(int argc,char **argv){

    //cout<<"_nifti_gethdr top"<<endl;

    if(argc<8){
        std::cerr<<"fidlError: _nifti_gethdr requires eight arguments: char *file,int64_t *xdim,int64_t *ydim,int64_t *zdim,"<<std::endl;
        std::cerr<<"    int64_t *tdim,float *center,float *mmppix,int *c_orient"<<std::endl;
        return 0;
        }
    char *file=argv[0];
    int64_t *xdim=(intptr_t*)argv[1];
    int64_t *ydim=(intptr_t*)argv[2];
    int64_t *zdim=(intptr_t*)argv[3];
    int64_t *tdim=(intptr_t*)argv[4];
    float *center=(float*)argv[5];
    float *mmppix=(float*)argv[6];
    int *c_orient=(int*)argv[7];

    //if(!nifti_gethdr(file,xdim,ydim,zdim,tdim,center,mmppix,c_orient)) return 0;
    //START190924
    #if 0 
    NiftiS *niftis; 
    if(!(niftis=nifti_gethdr(file))){
        return 0;
        }
    *xdim=niftis->dims[0]; 
    *ydim=niftis->dims[1]; 
    *zdim=niftis->dims[2]; 
    *tdim=niftis->dims[3]; 
    for(int i=0;i<3;++i){center[i]=niftis->center[i];mmppix[i]=niftis->mmppix[i];c_orient[i]=niftis->c_orient[i];}
    nifti_delhdr(niftis);
    #endif
    //START190926
    NiftiS niftis; 

    #if 0
    if(nifti_gethdr(file,&niftis)){
        return 0;
        }
    #endif
    //START210414
    if(!nifti_gethdr(file,&niftis))return 0;


    *xdim=niftis.dims[0]; 
    *ydim=niftis.dims[1]; 
    *zdim=niftis.dims[2]; 
    *tdim=niftis.dims[3]; 
    for(int i=0;i<3;++i){center[i]=niftis.center[i];mmppix[i]=niftis.mmppix[i];c_orient[i]=niftis.c_orient[i];}


    //cout<<file<<endl;
    //cout<<"xdim="<<*xdim<<" ydim="<<*ydim<<" zdim="<<*zdim<<" tdim="<<*tdim<<endl;
    //cout<<"center[0]="<<center[0]<<" center[1]="<<center[1]<<" center[2]="<<center[2]<<endl;
    //cout<<"mmppix[0]="<<mmppix[0]<<" mmppix[1]="<<mmppix[1]<<" mmppix[2]="<<mmppix[2]<<endl;
    //cout<<"c_orient[0]="<<c_orient[0]<<" c_orient[1]="<<c_orient[1]<<" c_orient[2]="<<c_orient[2]<<endl;

    //cout<<"_nifti_gethdr bottom"<<endl;

    return 1;
    }

//void nifti_delhdr(NiftiS* niftis){delete[] niftis;}
