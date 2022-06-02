/* Copyright 2/25/21 Washington University.  All Rights Reserved.
   fidl_lut.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <cstring>
#include "lut.h"

int main(int argc,char **argv){
char *lutf=NULL;
int i;
if(argc<3){
    std::cout<<"  -lut:     Lookup table. Ex. $FREESURFER_HOME/FreeSurferColorLUT.txt"<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-lut")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            lutf = argv[++i];
            }
        else {
            std::cout<<"fidlError: No lookup table specified after -lut option. Abort!"<<std::endl;
            exit(-1);
            }
        }
    }
if(!lutf){
    std::cout<<"fidlError: Need to specify lookup table with -lut"<<std::endl;
    exit(-1);
    }
lut l0;
l0.lut2(lutf);
std::cout<<l0.regvalmaxplusone<<std::endl;
} 
