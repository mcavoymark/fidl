/* Copyright 21/8/31 Washington University.  All Rights Reserved.
   lut2.cxx  $Revision: 1.1 $*/
#include <cstring> 
#include <string> 
#include <vector> 
#include <iostream> 
#include <fstream> 
#include <sstream> 
#include "lut2.h"
#include "fidl.h"

lut::lut(){
    regvalmaxplusone=0;

    //START210831
    //FS[17]="Lhippo";FS[53]="Rhippo";

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

int lut::lut2(char* lutf){
    std::ifstream ifile(lutf);
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
    return 1;
    }


#if 0
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
    if(lutf){
        if(!lut2(lutf))return NULL; 
        }
    return FS;
    }
#endif
//START210831
int lut::lut1(char* filename,char* lutf){
    if(strstr(filename,"wm0")){LUT[65530]=std::string("Lwm0");LUT[65531]=std::string("Rwm0");}
    else if(strstr(filename,"wm1")){LUT[65530]=std::string("Lwm1");LUT[65531]=std::string("Rwm1");}
    else if(strstr(filename,"wm2")){LUT[65530]=std::string("Lwm2");LUT[65531]=std::string("Rwm2");}
    else if(strstr(filename,"wm3")){LUT[65530]=std::string("Lwm3");LUT[65531]=std::string("Rwm3");}
    else if(strstr(filename,"csf0")){LUT[65532]=std::string("Lcsf0");LUT[65533]=std::string("Rcsf0");}
    else if(strstr(filename,"csf1")){LUT[65532]=std::string("Lcsf1");LUT[65533]=std::string("Rcsf1");}
    else if(strstr(filename,"csf2")){LUT[65532]=std::string("Lcsf2");LUT[65533]=std::string("Rcsf2");}
    else if(strstr(filename,"csf3")){LUT[65532]=std::string("Lcsf3");LUT[65533]=std::string("Rcsf3");}
    else if(strstr(filename,"brainmask")){LUT[65534]=std::string("Lbrainmask");LUT[65535]=std::string("Rbrainmask");}

    std::string str(filename); 
    root=str.substr(str.find_last_of("/")+1,str.find_last_of(".nii.gz")-str.find_last_of("/")-7);
    //std::cout<<"root="<<root<<std::endl;

    if(lutf){
        if(!lut2(lutf))return 0; 
        }
    return 1;
    }
