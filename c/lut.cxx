/* Copyright 10/1/19 Washington University.  All Rights Reserved.
   lut.cxx  $Revision: 1.1 $*/
#include <cstring> 
#include <string> 
#include <vector> 
#include <iostream> 
#include <fstream> 
#include <sstream> 
#include "lut.h"
#include "fidl.h"

lut::lut(){
    regvalmaxplusone=0;
    FS[17]="Lhippo";FS[53]="Rhippo";
    }
void lut::lut0(char* filename){
    std::cout<<"*** filename = "<<filename<<" ***"<<std::endl;
    if(strstr(filename,"wm")){
        std::cout<<"*** here wm ***"<<std::endl;
        Lval=65530;Rval=65531;
        }
    else if(strstr(filename,"csf")){
        std::cout<<"*** here csf ***"<<std::endl;
        Lval=65532;Rval=65533;
        }
    else if(strstr(filename,"brainmask")){
        std::cout<<"*** here brainmask ***"<<std::endl;
        Lval=65534;Rval=65535;
        }
    }
unsigned short lut::lutLval(){
    return Lval;
    }
unsigned short lut::lutRval(){
    return Rval;
    }

//void lut::lut2(char* lutf){
//START210504
int lut::lut2(char* lutf){

    std::ifstream ifile(lutf);

    //START210504
    if(!ifile.is_open()){
        std::cout<<"fidlError: Cannot open "<<lutf<<std::endl;
        return 0;
        }
    
    std::string str,name;
    int regval,R,G,B,A;
    while(getline(ifile,str)){
        if(str.empty() || !str.find("#"))continue;
        std::istringstream iss(str);
        iss>>regval>>name>>R>>G>>B>>A;
        LUT[regval]=name;
        if(regvalmaxplusone<regval)regvalmaxplusone=regval;
        }
    ++regvalmaxplusone;

    //START210504
    return 1;
    }

char** lut::lut1(char* filename,char* lutf){
    if(strstr(filename,"wm0")){FS[65530]="Lwm0";FS[65531]="Rwm0";}
    else if(strstr(filename,"wm1")){FS[65530]="Lwm1";FS[65531]="Rwm1";}
    else if(strstr(filename,"wm2")){FS[65530]="Lwm2";FS[65531]="Rwm2";}
    else if(strstr(filename,"wm3")){FS[65530]="Lwm3";FS[65531]="Rwm3";}
    else if(strstr(filename,"csf0")){FS[65532]="Lcsf0";FS[65533]="Rcsf0";}
    else if(strstr(filename,"csf1")){FS[65532]="Lcsf1";FS[65533]="Rcsf1";}
    else if(strstr(filename,"csf2")){FS[65532]="Lcsf2";FS[65533]="Rcsf2";}
    else if(strstr(filename,"csf3")){FS[65532]="Lcsf3";FS[65533]="Rcsf3";}
    else if(strstr(filename,"brainmask")){FS[65534]="Lbrainmask";FS[65535]="Rbrainmask";}
    std::string s(filename); 
    std::vector<char> v(s.begin(),s.end());
    v.push_back('\0');
    if(!(FS[1]=get_tail_sans_ext(v.data())))return NULL;

    //if(lutf)lut2(lutf); 
    //START210504
    if(lutf){
        if(!lut2(lutf))return NULL; 
        }
   
    return FS;
    }


#if 0
const char* lut_name(char* lutf,int regval){
    lut l0;
    l0.lut2(lutf);
    return l0.LUT[regval].c_str();
    }
#endif
