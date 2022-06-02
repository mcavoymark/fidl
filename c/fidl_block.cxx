/* Copyright 5/14/21 Washington University.  All Rights Reserved.
   fidl_block.cxx  $Revision: 1.1 $ */

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <sstream>
#include "files_struct.h" 
#include "d2double.h" 

int main(int argc,char **argv){
char *out=NULL;
int i,j,nfiles=0,nage=0,ngrouplabels=0;
Files_Struct *files=NULL,*age=NULL,*grouplabels=NULL;
if(argc<3){
    std::cout<<"Event files are assembled into a single block diagonal form."<<std::endl;
    std::cout<<"  -files: Text event files or a file that lists them. First line is the labels."<<std::endl;
    std::cout<<"  -age: Text files or a file that lists them."<<std::endl;
    std::cout<<"        Single column of numbers. First line is the label."<<std::endl;
    std::cout<<"        Age is centered separately to the mean age of each group. Thus each group has its own age column."<<std::endl;
    std::cout<<"        See afni.nimh.nih.gov/pub/dist/doc/htmldoc/STATISTICS/center.html"<<std::endl;
    std::cout<<"  -out: Name of output txt event file. First line is the labels."<<std::endl;

    //START210521
    std::cout<<"  -grouplabels: Column labels for group if -files is not used."<<std::endl;

    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfiles;
        if(!(files=read_files(nfiles,&argv[i+1])))exit(-1);
        i+=nfiles;
        }
    if(!strcmp(argv[i],"-age") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nage;
        if(!(age=read_files(nage,&argv[i+1])))exit(-1);
        i+=nage;
        }
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        out=argv[++i];

    //START210521
    if(!strcmp(argv[i],"-grouplabels") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++ngrouplabels;
        if(!(grouplabels=read_files(ngrouplabels,&argv[i+1])))exit(-1); //may need to change to get_files
        i+=ngrouplabels;
        }
    }

#if 0
if(!files){
    std::cout<<"fidlError: Need to specify -files"<<std::endl;
    exit(-1);
    }
if(!out){
    std::cout<<"fidlError: Need to specify -out"<<std::endl;
    exit(-1);
    }
if(age)if(age->nfiles!=files->nfiles){
    std::cout<<"fidlError: age->nfiles="<<age->nfiles<<" files->nfiles="<<files->nfiles<<" Must be equal!"<<std::endl;
    exit(-1);
    }
#endif
//START210521
if(!out){
    std::cout<<"fidlError: Need to specify -out"<<std::endl;
    exit(-1);
    }
if(age&&files)if(age->nfiles!=files->nfiles){
    std::cout<<"fidlError: age->nfiles="<<age->nfiles<<" files->nfiles="<<files->nfiles<<" Must be equal!"<<std::endl;
    exit(-1);
    }
if(!files&&!age){
    std::cout<<"fidlError: Need to specify -files and/or -age"<<std::endl;
    exit(-1);
    }

//START210524
#if 0
if(!files)if(age->nfiles!=grouplabels->nfiles){
    std::cout<<"fidlError: age->nfiles="<<age->nfiles<<" grouplabels->nfiles="<<grouplabels->nfiles<<" Must be equal!"<<std::endl;
    exit(-1);
    }
#endif
//START220311
if(!files&&grouplabels)if(age->nfiles!=grouplabels->nfiles){
    std::cout<<"fidlError: age->nfiles="<<age->nfiles<<" grouplabels->nfiles="<<grouplabels->nfiles<<" Must be equal!"<<std::endl;
    exit(-1);
    }


std::vector<int> linesperfile;
std::vector<size_t> colperfile;
std::vector<int> h0;
std::vector<std::vector<std::string> > s0;
if(files){
    linesperfile.resize(files->nfiles);
    colperfile.resize(files->nfiles);
    h0.resize(files->nfiles,-1);
    int s0h0=0,ncol=0;
    const char* delim=",\t";
    for(size_t i=0;i<files->nfiles;++i){
        std::ifstream ifile(files->files[i]);
        std::string str;
        std::size_t found;
        while(getline(ifile,str)){
            if(str.empty() || !str.find("#"))continue;
            do{
                found=str.find_first_of(delim);
                str[found]=' '; 
                }while(found!=std::string::npos);
            std::istringstream iss(str);
            std::vector<std::string> vstr;
            while(std::getline(iss,str,' '))vstr.push_back(str);
            linesperfile[i]++;
            if(h0[i]==-1){
                colperfile[i]=vstr.size();
                ncol++;
                h0[i]=s0h0;
                }
            else{
                if(vstr.size()!=colperfile[i]){
                    std::cout<<"fidlError: "<<files->files[i]<<" line 1 has "<<colperfile[i]<<"strings while line "<<linesperfile[i]<<" has "<<vstr.size()<<" strings."<<std::endl; 
                    exit(-1);
                    }
                }
            s0.push_back(vstr);s0h0++; 
            }
        }
    }
std::vector<std::string> age0;
std::vector<std::vector<double> > age1;
if(age){
    age0.resize(age->nfiles);
    for(size_t i=0;i<age->nfiles;++i){
        std::vector<double> d0;
        std::ifstream ifile(age->files[i]);
        std::string str;
        for(j=0;getline(ifile,str);){
            if(str.empty() || !str.find("#"))continue;
            if(!j){

                //START220311
                std::string::size_type begin=str.find_first_not_of("\t");
                std::string::size_type end=str.find_last_not_of("\t");
                str=str.substr(begin,end-begin+1);

                age0[i]=str;
                j++;
                }
            else{
                d0.push_back(std::stod(str));
                }
            }

        #if 0
        if((int)d0.size()!=(linesperfile[i]-1)){
            std::cout<<"fidlError: d0.size()="<<d0.size()<<" linesperfile[i]-1="<<linesperfile[i]-1<<" Must be equal."<<std::endl; 
            exit(-1);
            }
        #endif
        //START210524
        if(files)if((int)d0.size()!=(linesperfile[i]-1)){
            std::cout<<"fidlError: d0.size()="<<d0.size()<<" linesperfile[i]-1="<<linesperfile[i]-1<<" Must be equal."<<std::endl; 
            exit(-1);
            }

        double td=0.;for(auto& n : d0)td+=n;td/=(double)d0.size();
        for(auto& n : d0)n-=td;
        age1.push_back(d0);
        }
    }


//START210524
double **g0=NULL;
size_t col=0;

//if(!files){
//START220311
if(!files&&grouplabels){

    size_t row=0;for(auto& n : age1)row+=n.size();
    col=age1.size();
    if(!(g0=d2double((int)row,(int)col)))exit(-1);
    size_t k=0; 
    for(size_t i=0;i<col;++i){
        for(size_t j=0;j<age1[i].size();++j,++k){
            g0[k][i]=1.; 
            }
        }
    }


#if 0
std::ofstream ofile(out);
for(size_t i=0;i<files->nfiles;++i){
    for(size_t j=0;j<colperfile[i];++j)ofile<<s0[h0[i]][j]<<"\t";
    if(age)ofile<<age0[i]<<"\t"; 
    }
ofile<<std::endl;
#endif
//START210524
std::ofstream ofile(out);
if(files){
    for(size_t i=0;i<files->nfiles;++i){
        for(size_t j=0;j<colperfile[i];++j)ofile<<s0[h0[i]][j]<<"\t";
        if(age)ofile<<age0[i]<<"\t"; 
        }
    ofile<<std::endl;
    }
else{

    //for(size_t i=0;i<grouplabels->nfiles;++i)ofile<<grouplabels->files[i]<<"\t";
    //START220311
    if(grouplabels)for(size_t i=0;i<grouplabels->nfiles;++i)ofile<<grouplabels->files[i]<<"\t";

    for(size_t i=0;i<age0.size();++i)ofile<<age0[i]<<"\t"; 
    ofile<<std::endl;
    size_t l=0; 
    for(size_t i=0;i<age1.size();++i){
        for(size_t j=0;j<age1[i].size();++j,++l){

            //for(size_t k=0;k<col;++k)ofile<<g0[l][k]<<"\t"; 
            //START220311
            if(g0)for(size_t k=0;k<col;++k)ofile<<g0[l][k]<<"\t"; 

            //pre zero fill
            for(size_t k=0;k<i;k++)ofile<<"0\t";              
            //values
            ofile<<age1[i][j]<<"\t";
            //post zero fill
            for(size_t k=i+1;k<age1.size();++k)ofile<<"0\t";
            ofile<<std::endl;              
            }
        }
    }


if(files){
    int m=0;
    for(size_t i=0;i<files->nfiles;++i){
        for(j=0;j<linesperfile[i];++j,++m){
            if(!j)continue;//hdr
    
            //pre zero fill
            for(size_t k=0;k<i;k++){
                for(size_t l=0;l<colperfile[k];++l){
                    ofile<<"0\t";              
                    } 
                if(age)ofile<<"0\t";
                } 
            //values
            for(size_t k=0;k<colperfile[i];++k){
                ofile<<s0[m][k]<<"\t";
                //ofile<<s0[m][k];
                //if(k<colperfile[i]-1)ofile<<"\t";
                }
            if(age){
                ofile<<age1[i][j-1]<<"\t";
                //ofile<<"\t"<<age1[i][j-1];
                }
            //post zero fill
            for(size_t k=i+1;k<files->nfiles;++k){
                for(size_t l=0;l<colperfile[k];++l){
                    ofile<<"0\t";              
                    //ofile<<"\t0";              
                    } 
                if(age)ofile<<"0\t";
                } 
            ofile<<std::endl;              
            }
        }
    }
std::cout<<"Output written to "<<out<<std::endl;
}
