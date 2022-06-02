/* Copyright 6/25/15 Washington University.  All Rights Reserved.
   nifti_getstack.cxx  $Revision: 1.2 $*/

//#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "CiftiException.h"
#include "NiftiIO.h"
#include <cstdio>
#include <iostream>

using namespace std;
using namespace cifti;

#if 0
extern "C" int nifti_getstack(char *file,float *stack,int64_t *indexSelect) 
{
    try
    {
        NiftiIO inputFile;
        inputFile.openRead(file);

        //inputFile.readData(stack,2,indexSelect,(bool)0);
        int fullDims=2;
        bool tolerateShortRead = false;
        //std::vector<int64_t> idx(indexSelect,indexSelect+sizeof(indexSelect)/sizeof(int64_t));
        //vector<int64_t> idx(indexSelect,indexSelect+sizeof indexSelect/sizeof indexSelect[0]);
        //inputFile.readData(stack,&fullDims,idx,&tolerateShortRead);
        //inputFile.readData(stack,&fullDims,vector<int64_t>indexSelect,&tolerateShortRead);
        inputFile.readData(stack,&fullDims,indexSelect,&tolerateShortRead);

        inputFile.close();
    } catch (CiftiException& e) {
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 0;
    }
    return 1;
}
#endif
/*START150626*/
extern "C" int nifti_getstack(char *file,int64_t z,int64_t t,float *stack)
{
    try
    {
        NiftiIO inputFile;
        inputFile.openRead(file);
        if(inputFile.getDimensions().size()<3||inputFile.getDimensions().size()>4){
            cerr << "fidlError: "<<file<<" Not a 3D or 4D volume" << endl;
            return 0;
            }
        vector<int64_t> mySelect(inputFile.getDimensions().size()-2,0);
        mySelect[0]=z;
        if(mySelect.size()>=2)mySelect[1]=t;
        inputFile.readData(stack,2,mySelect);
        inputFile.close();
    } catch (CiftiException& e) {
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 0;
    }
    return 1;
}

#if 0
extern "C" int _nifti_getstack(int argc,char **argv) 
{
    if(argc<3){
        cerr << "_nifti_getstack requires three arguments: an input nifti file, float *image, int64_t* indexSelect" << endl;
        return 0;
        }
    char *file=argv[0];
    float *stack=(float*)argv[1];
    int64_t *indexSelect=(intptr_t*)argv[2];
    if(!nifti_getstack(file,stack,indexSelect))return 0;
    return 1;
}
#endif
/*START150626*/
extern "C" int _nifti_getstack(int argc,char **argv)
{
    if(argc<3){
        cerr << "_nifti_getstack requires three arguments: an input nifti file, float *image, int64_t* indexSelect" << endl;
        return 0;
        }
    char *file=argv[0];
    int64_t z=(intptr_t)argv[1];
    int64_t t=(intptr_t)argv[2];
    float *stack=(float*)argv[3];
    if(!nifti_getstack(file,z,t,stack))return 0;
    return 1;
}


#if 0
[  0%] Building CXX object CMakeFiles/fidlcifti.dir/home/hannah/mcavoy/idl/clib/nifti_getstack.cxx.o
/home/hannah/mcavoy/idl/clib/nifti_getstack.cxx: In function âint nifti_getstack(char*, float*, int64_t*)â:
/home/hannah/mcavoy/idl/clib/nifti_getstack.cxx:28: error: expected primary-expression before âindexSelectâ
/home/hannah/mcavoy/idl/clib/nifti_getstack.cxx: At global scope:
/home/hannah/mcavoy/idl/clib/nifti_getstack.cxx:16: warning: unused parameter âindexSelectâ
make[3]: *** [CMakeFiles/fidlcifti.dir/home/hannah/mcavoy/idl/clib/nifti_getstack.cxx.o] Error 1
make[2]: *** [CMakeFiles/fidlcifti.dir/all] Error 2
make[1]: *** [CMakeFiles/fidlcifti.dir/rule] Error 2
make: *** [fidlcifti] Error 2

/home/hannah/mcavoy/idl/clib/nifti_getstack.cxx:28: error: no matching function for call to âcifti::NiftiIO::readData(float*&,
int*, std::vector<long int, std::allocator<long int> >&, bool*)â

/home/hannah/mcavoy/idl/clib/nifti_getstack.cxx:27: error: no matching function for call to âcifti::NiftiIO::readData(float*&,
int*, std::vector<long int, std::allocator<long int> >&, bool*)â

void readData(T* dataOut, const int& fullDims, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead = false);
#endif
