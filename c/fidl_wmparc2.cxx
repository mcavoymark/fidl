/* Copyright 8/31/21 Washington University.  All Rights Reserved.
   fidl_wmparc2.cxx  $Revision: 1.1 $ */
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <set>
#include "stack.h"
#include "subs_nifti.h"

#include "lut2.h"

int main(int argc,char **argv)
{
char *file=NULL,*lutf=NULL;
int i;

if(argc<3){
    std::cerr<<"-file: File with one of more regions."<<std::endl; 
    std::cerr<<"-lut:  Lookup table. Ex. $FREESURFER_HOME/FreeSurferColorLUT.txt"<<std::endl;
    std::cerr<<"       Not necessary for all files as some regions are identified by the file name."<<std::endl;
    std::cerr<<"       Need not be an actual freesurfer lookuptable. Just needs to follow the freesurfer format."<<std::endl;
    exit(-1);
    }

for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-file") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        file=argv[++i];
    if(!strcmp(argv[i],"-lut")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            lutf = argv[++i];
            }
        else {
            std::cerr<<"fidlError: No lookup table specified after -lut option. Abort!"<<std::endl;
            exit(-1);
            }
        }
    }

if(!file){std::cerr<<"-file not specified"<<std::endl;exit(-1);}
int lcwmparc=0;
std::string str(file);
if(str.find("wmparc")!=std::string::npos){
    lcwmparc=1;
    if(!lutf){std::cerr<<"-lut not specified. Needed for the wmparc."<<std::endl;exit(-1);}
    }

stack s0;
unsigned short *man=NULL;
if(!(man=s0.stack0us(file)))exit(-1);
std::set<unsigned short> s(man,man+s0.vol);

lut l0;
l0.lut1(file,!lcwmparc?(char*)NULL:lutf);

for(auto it=s.begin();++it!=s.end();){
    if(l0.LUT.find(*it)!=l0.LUT.end()){
        std::cout<<*it<<" "<<l0.LUT[*it]<<" ";
        }
    }
std::cout<<std::endl;
}
