/* Copyright 3/31/21 Washington University.  All Rights Reserved.
   movement.cxx  $Revision: 1.1 $ */

#include <iostream>
#include <fstream>
#include <sstream>
#include "movement.h" 

movement::movement(){
    MARMSmval=NULL;wts=NULL;
    }

movement::~movement(){
    if(MARMSmval) delete[] MARMSmval;
    if(wts) delete[] wts;
    }

int movement::movement0(Files_Struct* MARMSm){
    MARMSmval=new double[MARMSm->nfiles](); //empty parentheses zero intializes
    wts=new double[MARMSm->nfiles](); //empty parentheses zero intializes
    size_t i;
    for(i=0;i<MARMSm->nfiles;++i){
        std::ifstream ifile(MARMSm->files[i]);
        std::string str;
        int cnt=0;
        while(getline(ifile,str)){
            if(str.empty() || !str.find("#"))continue;
            std::istringstream iss(str);
            iss>>MARMSmval[i];
            cnt++;
            }
        if(cnt>1){
            std::cout<<"fidlError: "<<MARMSm->files[i]<<" must only contain a single value"<<std::endl;
            return 0;
            }
        if(!MARMSmval[i]){
            std::cout<<"fidlError: MARMSmval["<<i<<"]="<<MARMSmval[i]<<" Must be nonzero."<<std::endl;
            return 0;
            }
        wts[i]=1./MARMSmval[i];
        }
    std::cout<<"MARMSmval";for(i=0;i<MARMSm->nfiles;++i)std::cout<<" "<<MARMSmval[i];std::cout<<std::endl;
    std::cout<<"wts      ";for(i=0;i<MARMSm->nfiles;++i)std::cout<<" "<<wts[i];std::cout<<std::endl;
    return 1;
    }
