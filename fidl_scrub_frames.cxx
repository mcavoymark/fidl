/* Copyright 11/05/21 Washington University.  All Rights Reserved.
   fidl_scrub_frames.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include "files_struct.h"

int main(int argc,char **argv){
char *outf=NULL;
int i,j,nfiles=0;
Files_Struct *files=NULL;
if(argc<5){
    std::cout<<argv[0]<<std::endl;
    std::cout<<"Convert scrub files to a frames files to be used by fidl_tc_ss."<<std::endl;
    std::cout<<"    -files: Typically *scrub.txt. Single column. 1:keep 0:discard"<<std::endl;
    std::cout<<"            Individual files, concs or lists."<<std::endl;
    std::cout<<"    -out:   Typically *.dat"<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfiles;
        if(!(files=read_files(nfiles,&argv[i+1])))exit(-1);
        i+=nfiles;
        }
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        outf=argv[++i];
    }
if(!nfiles){std::cout<<"fidlError: Need to specify scrub files with -files"<<std::endl;exit(-1);}
if(!outf){std::cout<<"fidlError: Need to specify output name with -out"<<std::endl;exit(-1);}

std::ofstream ofile(outf);
ofile<<"0,  ";

//char plus=0;
//char plus=(char)0;
char plus='+';
int cnt=0;
j=1;
for(size_t i=0;i<files->nfiles;++i){
    std::cout<<"Reading "<<files->files[i]<<std::endl;

    std::ifstream ifile(files->files[i]);
    std::string str;

    for(;getline(ifile,str);++j){
        if(str.empty() || !str.find("#"))continue;

        if(std::stoi(str)==1){
            !cnt++?(ofile<<j):(ofile<<plus<<j);
            }

        }
    }
ofile<<std::endl;
std::cout<<"    Output written to "<<outf<<std::endl;
std::cout<<"    cnt="<<cnt<<"    "<<j-1<<" lines"<<std::endl;
}
