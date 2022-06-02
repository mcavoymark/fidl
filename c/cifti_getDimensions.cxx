/* Copyright 1/22/15 Washington University.  All Rights Reserved.
   cifti_getDimensions.cxx  $Revision: 1.5 $*/
#include "CiftiException.h"
#include "CiftiFile.h"
#include "MultiDimIterator.h"
#include <cstdio>
#include <iostream>
//#include <vector>
//#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
using namespace std;
using namespace cifti;
#include "cifti_getDimensions.h"
int cifti_getDimensions(char *file,int64_t *rowSize,int64_t *colSize){
    try{
        CiftiFile inputFile(file);
        *rowSize = inputFile.getDimensions()[0];
        *colSize = inputFile.getDimensions()[1];
        } 
    catch (CiftiException& e){
        cout << "Caught CiftiException: " + e.whatString().toStdString() << endl;
        return 0;
        }
    return 1;
    }
int _cifti_getDimensions(int argc,char **argv){
    if(argc<3){
        cout<<"_cifti_getDimensions requires three arguments: an input cifti file, int64_t rowSize and int64_t colSize"<<endl;
        return 0;
        }
    char *file=argv[0];
    int64_t *rowSize = (intptr_t*)argv[1];
    int64_t *colSize = (intptr_t*)argv[2];
    if(!cifti_getDimensions(file,rowSize,colSize)) return 0;
    //printf("rowsize=" "%" PRId64 "\tcolsize=" "%" PRId64 "\n",*rowSize,*colSize);fflush(stdout);
    return 1;
    }
