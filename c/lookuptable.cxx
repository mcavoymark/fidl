/* Copyright 2/14/20 Washington University.  All Rights Reserved.
   lookuptable.cxx  $Revision: 1.1 $*/
#include "lookuptable.h"
//#include "lut.h"
#include "lut2.h"

//const char* lut_name(char* lutf,int regval){
char* lookuptable_name(char* lutf,int regval){
    lut l0;
    l0.lut2(lutf);
    return (char*)l0.LUT[regval].c_str();
    }
