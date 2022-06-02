/* Copyright 11/18/19 Washington University.  All Rights Reserved.
   fidl_dice.cxx  $Revision: 1.1 $ */
//#include <cstdlib>
#include <iostream>
#include <cstring>
#include <set>
#include <vector>
#include <algorithm>

#include "stack.h"
//#include "subs_nifti.h"
//#include "lut.h"
int main(int argc,char **argv)
{
char *man1f=NULL,*man2f=NULL,*wmparcf=NULL;
unsigned short *man1=NULL,*man2=NULL;
float *wmparc=NULL;
int i,j;
stack fs1,fs2,fs3;


if(argc<5){
    std::cout<<"-man1: <manual_segmention>.nii.gz  unsigned short from ITK-SNAP"<<std::endl;
    std::cout<<"-man2: <manual_segmention>.nii.gz  unsigned short from ITK-SNAP"<<std::endl;
    std::cout<<"-wmparc: wmparc.nii.gz float"<<std::endl;
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-man1") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        man1f=argv[++i];
    if(!strcmp(argv[i],"-man2") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        man2f=argv[++i];
    if(!strcmp(argv[i],"-wmparc") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        wmparcf=argv[++i];
    }
if(!man1f){std::cout<<"-man1 not specified"<<std::endl;exit(-1);}
if(!man2f){std::cout<<"-man2 not specified"<<std::endl;exit(-1);}
if(!(man1=fs1.stack0us(man1f)))exit(-1);
if(!(man2=fs2.stack0us(man2f)))exit(-1);
if(fs1.vol!=fs2.vol){std::cout<<"fidlError: fs1.vol="<<fs1.vol<<" fs2.vol="<<fs2.vol<<" Must be equal!"<<std::endl;exit(-1);}
if(wmparcf){
    if(!(wmparc=fs3.stack0(wmparcf)))exit(-1);
    if(fs1.vol!=fs3.vol){std::cout<<"fidlError: fs1.vol="<<fs1.vol<<" fs3.vol="<<fs3.vol<<" Must be equal!"<<std::endl;exit(-1);}
    }

std::set<unsigned short> s1(man1,man1+fs1.vol);
std::set<unsigned short> s2(man2,man2+fs2.vol);
std::vector<unsigned short> v(fs1.vol);
std::vector<unsigned short>::iterator it; 
//it=std::set_intersection(man1,man1+fs1.vol,man2,man2+fs2.vol,v.begin());
//it=std::set_intersection(s1,s1+s1.size(),s2,s2+s2.size(),v.begin());
it=std::set_intersection(s1.begin(),s1.end(),s2.begin(),s2.end(),v.begin());
v.resize(it-v.begin());

std::cout<<"The intersection has "<<v.size()-1<<" elements:";for(it=v.begin();++it!=v.end();)std::cout<<' '<<*it;std::cout<<std::endl;

std::vector<int> AplusB(v.size()-1,0);
std::vector<int> AandB(v.size()-1,0);
std::vector<int> A(v.size()-1,0);
std::vector<int> B(v.size()-1,0);
std::vector<int> C(v.size()-1,0);
int lcA,lcB,lcC;
for(j=0;j<fs1.vol;j++){
    lcC=wmparcf?0:1;
    for(lcA=lcB=i=0,it=v.begin();++it!=v.end();++i){
        if(wmparcf){
            if(wmparc[j]==*it){C[i]++;lcC++;}
            }
        if(man1[j]==*it||man2[j]==*it){
            if(man1[j]==*it){A[i]++;lcA++;}
            if(man2[j]==*it){B[i]++;lcB++;}
            AplusB[i]++;
            if(man1[j]==*it&&man2[j]==*it){AplusB[i]++;AandB[i]++;}
            }
        if((lcA+lcB+lcC)==3)break;
        }
    }

for(i=0,it=v.begin();++it!=v.end();++i){
    std::cout<<"Region "<<*it<<std::endl;

    //std::cout<<"    "<<man1f<<" "<<A[i]<<" voxels"<<std::endl;
    //std::cout<<"    "<<man2f<<" "<<B[i]<<" voxels"<<std::endl;
    //START191126
    std::cout<<"    "<<man1f<<" "<<A[i]<<" voxels "<<A[i]*fs1.voxel_size[0]*fs1.voxel_size[1]*fs1.voxel_size[2]<<" mm^3"<<std::endl;
    std::cout<<"    "<<man2f<<" "<<B[i]<<" voxels "<<B[i]*fs1.voxel_size[0]*fs1.voxel_size[1]*fs1.voxel_size[2]<<" mm^3"<<std::endl;

    std::cout<<"    intersection "<<AandB[i]<<" voxels"<<std::endl;
    std::cout<<"    dice = "<<2.*(double)AandB[i]/(double)AplusB[i]<<std::endl;

    //if(wmparcf)std::cout<<"    "<<wmparcf<<" "<<C[i]<<" voxels"<<std::endl;
    //START191126
    if(wmparcf)std::cout<<"    "<<wmparcf<<" "<<C[i]<<" voxels "<<C[i]*fs1.voxel_size[0]*fs1.voxel_size[1]*fs1.voxel_size[2]<<" mm^3"<<std::endl;

    }
}
