/* Copyright 7/1/19 Washington University.  All Rights Reserved.
   fidl_wmparc.cxx  $Revision: 1.1 $ */
#include <cstdlib>
#include <iostream>
#include <cstring>

//#include <unordered_set>
//START191008
#include <set>

#include "stack.h"
#include "subs_nifti.h"
#include "lut.h"
int main(int argc,char **argv)
{
char *wmparcf=NULL,*manf=NULL,*outf=NULL;
unsigned short *man=NULL;
int i,j,xdim1,ydim1,zdim1,xdim2,ydim2,zdim2,vol;
float *wmparc=NULL;
stack fs1,fs2;

#if 0
if(argc<6){
    std::cout<<"-wmparc: wmparc.nii.gzi  float from HCP structural pipeline"<<std::endl;
    std::cout<<"-man: <manual_segmention>.nii.gz  unsigned short from ITK-SNAP"<<std::endl;
    std::cout<<"-out: <output>.nii.gz"<<std::endl;
    std::cout<<"Ex. fidl_wmparc -wmparc /data/nil-bluearc/vlassenko/mcavoy/MAN/AGBR-048/wmparc.nii.gz"<<std::endl;
    std::cout<<"Ex.             -man /data/nil-bluearc/vlassenko/mcavoy/MAN/AGBR-048/190603_AGBR-048.nii.gz"<<std::endl;
    std::cout<<"Ex.             -out /data/nil-bluearc/vlassenko/mcavoy/MAN/AGBR-048/wmparc_man.nii.gz"<<std::endl;
    std::cout<<"This program does the following:"<<std::endl;
    std::cout<<"    1. All integer values greater than 0 in <man> are set to 0 in <wmparc>"<<std::endl;
    std::cout<<"       For example, say <man> includes a left hippocampus with voxel values of 17 and a right"<<std::endl;
    std::cout<<"       hippocampus with voxel values of 53. Then all voxels in <wmparc> with values 17 and 53"<<std::endl;
    std::cout<<"       are set to zero."<<std::endl;
    std::cout<<"    2. Nonzero voxels in <man> replace those in <wmparc>"<<std::endl;
    std::cout<<"       Each voxel in <man> with a value of 17 now has a value of 17 in <wmparc>, and each"<<std::endl;
    std::cout<<"       voxel in <man> with a value of 53 now has a value of 53 in <wmparc>."<<std::endl;
    exit(-1);
    }
#endif
//START190812
if(argc<3){
    std::cout<<"-wmparc: wmparc.nii.gz  float from HCP structural pipeline"<<std::endl;
    std::cout<<"-man: <manual_segmention>.nii.gz  unsigned short from ITK-SNAP"<<std::endl;
    std::cout<<"-out: <output>.nii.gz\n"<<std::endl;
    std::cout<<"Ex1. fidl_wmparc -wmparc /data/nil-bluearc/vlassenko/mcavoy/MAN/AGBR-048/wmparc.nii.gz \\"<<std::endl;
    std::cout<<"                 -man /data/nil-bluearc/vlassenko/mcavoy/MAN/AGBR-048/190603_AGBR-048.nii.gz \\"<<std::endl;
    std::cout<<"                 -out /data/nil-bluearc/vlassenko/mcavoy/MAN/AGBR-048/wmparc_man.nii.gz"<<std::endl;
    std::cout<<"    Does the following:"<<std::endl;
    std::cout<<"    1. All integer values greater than 0 in <man> are set to 0 in <wmparc>"<<std::endl;
    std::cout<<"       For example, say <man> includes a left hippocampus with voxel values of 17 and a right"<<std::endl;
    std::cout<<"       hippocampus with voxel values of 53. Then all voxels in <wmparc> with values 17 and 53"<<std::endl;
    std::cout<<"       are set to zero."<<std::endl;
    std::cout<<"    2. Nonzero voxels in <man> replace those in <wmparc>"<<std::endl;
    std::cout<<"       Each voxel in <man> with a value of 17 now has a value of 17 in <wmparc>, and each"<<std::endl;
    std::cout<<"       voxel in <man> with a value of 53 now has a value of 53 in <wmparc>.\n"<<std::endl;
    std::cout<<"Ex2. fidl_wmparc -man /data/nil-bluearc/vlassenko/mcavoy/MAN/AGBR-048/190603_AGBR-048.nii.gz"<<std::endl;
    std::cout<<"    Outputs the unique nonzero integers to the terminal (space separated). For this file they are: 17 53"<<std::endl;
    exit(-1);
    }

for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-wmparc") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        wmparcf=argv[++i];
    if(!strcmp(argv[i],"-man") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        manf=argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        outf=argv[++i];
    }

#if 0
if(!wmparcf){std::cout<<"-wmparc not specified"<<std::endl;exit(-1);}
if(!manf){std::cout<<"-man not specified"<<std::endl;exit(-1);}
if(!outf){std::cout<<"-out not specified"<<std::endl;exit(-1);}
if(!(wmparc=fs1.stack0(wmparcf)))exit(-1);
if(!(man=fs2.stack0us(manf)))exit(-1);
fs1.assign(xdim1,ydim1,zdim1);
fs2.assign(xdim2,ydim2,zdim2);
if(xdim1!=xdim2||ydim1!=ydim2||zdim1!=zdim2){
    std::cout<<wmparcf<<" xdim = "<<xdim1<<" ydim = "<<ydim1<<" zdim = "<<zdim1<<std::endl;
    std::cout<<manf<<" xdim = "<<xdim2<<" ydim = "<<ydim2<<" zdim = "<<zdim2<<std::endl;
    std::cout<<"    Must be equal. "<<std::endl;
    }
vol=xdim1*ydim1*zdim1;
std::unordered_set<unsigned short> s(man,man+vol);
#endif
#if 1
//START190812
if(!manf){std::cout<<"-man not specified"<<std::endl;exit(-1);}
if(!(man=fs2.stack0us(manf)))exit(-1);
fs2.assign(xdim2,ydim2,zdim2);
vol=xdim2*ydim2*zdim2;
std::set<unsigned short> s(man,man+vol);
#endif
#if 0
//START200311
if(!manf){std::cout<<"-man not specified"<<std::endl;exit(-1);}
if(!(man=fs2.stack0int(manf)))exit(-1);
vol=fs2.vol;
std::set<int> s(man,man+vol);
#endif

if(!wmparcf&&!outf){

    #if 1
    lut l0; 
    char** FS=l0.lut1(manf,(char*)NULL); 
    for(auto it=s.begin();++it!=s.end();){
        if(FS[*it])std::cout<<*it<<" "<<FS[*it]<<" ";
        }
    std::cout<<std::endl;
    #endif
#if 0
    //START200311
    int nreg=s.size()-1; 
    if(nreg>2){
        std::cout<<nreg<<" freesurfer_regions"
    lut l0; 
    char** FS=l0.lut1(manf); 
    for(auto it=s.begin();++it!=s.end();){
        if(FS[*it])std::cout<<*it<<" "<<FS[*it]<<" ";
        }
    std::cout<<std::endl;
#endif

    }
else{
    if(!wmparcf){std::cout<<"-wmparc not specified"<<std::endl;exit(-1);}
    if(!outf){std::cout<<"-out not specified"<<std::endl;exit(-1);}


    if(!(wmparc=fs1.stack0(wmparcf)))exit(-1);
    fs1.assign(xdim1,ydim1,zdim1);
    if(xdim1!=xdim2||ydim1!=ydim2||zdim1!=zdim2){
        std::cout<<wmparcf<<" xdim = "<<xdim1<<" ydim = "<<ydim1<<" zdim = "<<zdim1<<std::endl;
        std::cout<<manf<<" xdim = "<<xdim2<<" ydim = "<<ydim2<<" zdim = "<<zdim2<<std::endl;
        std::cout<<"    Must be equal. "<<std::endl;
        }
    std::cout<<"-wmparc  "<<wmparcf<<std::endl;
    std::cout<<"-man     "<<manf<<std::endl;
    std::cout<<"         Has "<<s.size()<<" values: ";
    for(auto it=s.begin();it!=s.end();++it)std::cout<<" "<<*it;std::cout<<std::endl;
    for(auto it=s.begin();it!=s.end();++it){
        if(*it){
            for(j=i=0;i<vol;i++){
                if(man[i]==*it){
                    j++; 
                    }
                }
            std::cout<<"    Found "<<j<<" voxels in man with value "<<*it<<std::endl;
            }
        }
    std::cout<<"xdim1 = "<<xdim1<<" ydim1 = "<<ydim1<<" zdim1 = "<<zdim1<<" vol = "<<vol<<"\n"<<std::endl;
    
    for(auto it=s.begin();it!=s.end();++it){
        if(*it){
            std::cout<<"Setting values of "<<*it<<" in wmparc to zero"<<std::endl;
            for(j=i=0;i<vol;i++){
                if(wmparc[i]==*it){
                    wmparc[i]=0.; 
                    j++; 
                    }
                }
            std::cout<<"    Found "<<j<<" voxels in wmparc with value "<<*it<<std::endl;
            #if 0
            /*CHECK*/
            for(j=i=0;i<vol;i++){
                if(wmparc[i]==*it){
                    j++; 
                    }
                }
            std::cout<<"    Found "<<j<<" voxels in wmparc with value "<<*it<<std::endl;
            #endif
            }
        }

    for(auto it=s.begin();it!=s.end();++it){
        if(*it){
            for(j=i=0;i<vol;i++){
                if(man[i]==*it){
                    wmparc[i]=*it;
                    j++;
                    }
                }
            std::cout<<"Assigned "<<j<<" voxels in wmparc to value "<<*it<<std::endl;
            #if 0
            /*CHECK*/
            for(j=i=0;i<vol;i++){
                if(wmparc[i]==*it){
                    j++; 
                    }
                }
            std::cout<<"    Found "<<j<<" voxels in wmparc with value "<<*it<<std::endl;
            #endif
            }
        }
    
    #if 0
    if(!nifti_write2(outf,wmparcf,wmparc))exit(-1);
    std::cout<<"Output written to "<<outf<<std::endl;
    #endif
    //START211202
    std::string str(outf);
    if(str.find(".nii")==std::string::npos)
        str+=".nii";
    else if(!str.compare(str.size()-2,2,"gz")){
        str=str.substr(0,str.size()-3);
        }
    if(!nifti_write2((char*)str.c_str(),wmparcf,wmparc))exit(-1);
    std::string str2="gzip -f "+str;
    if(system(str2.c_str())==-1)std::cout<<"fidlError: "<<str2<<std::endl;else std::cout<<"Output written to "<<str+".gz"<<std::endl;

    }
}
