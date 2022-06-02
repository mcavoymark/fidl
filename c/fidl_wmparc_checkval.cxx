/* Copyright 2/13/20 Washington University.  All Rights Reserved.
   fidl_wmparc_checkval.cxx  $Revision: 1.1 $ */
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <set>

#include "fidl.h"
#include "stack.h"
#include "subs_nifti.h"
#include "lut.h"
int main(int argc,char **argv)
{
char *outf=NULL;
int i,j,nwmparc=0,cnt[14176];
Files_Struct *wmparc=NULL;

if(argc<3){
    std::cout<<"-wmparc: wmparc.nii.gz from HCP structural pipeline. One or many."<<std::endl;
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

    if(!strcmp(argv[i],"-wmparc") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nwmparc;
        if(!(wmparc=read_files(nwmparc,&argv[i+1])))exit(-1);
        //strcpy(root,argv[i+1]);
        //if(!(rootptr=get_tail_sans_ext(root))) exit(-1);
        i+=nwmparc;
        }

    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        outf=argv[++i];
    }
if(!nwmparc){std::cout<<"-wmparc not specified"<<std::endl;exit(-1);}
for(int i=0;i<14176;++i)cnt[i]=0;
for(size_t i1=0;i1<wmparc->nfiles;++i1){
    stack s0;float *f0;
    if(!(f0=s0.stack0(wmparc->files[i1])))exit(-1);
    std::set<float> s(f0,f0+s0.vol);
    for(auto it=s.begin();++it!=s.end();){
        cnt[(size_t)*it]++;
        }
    }
for(j=0,i=1;i<14176;++i){
    if(cnt[i]){
        std::cout<<i<<" "<<cnt[i]<<std::endl;
        j++;
        }
    } 
std::cout<<j<<" regions with at least one subject"<<std::endl;
std::cout<<std::endl;
for(int i=1;i<14176;++i){
    if(cnt[i]&&cnt[i]<(int)wmparc->nfiles){
        std::cout<<i<<" "<<cnt[i]<<std::endl;
        }
    }
}
