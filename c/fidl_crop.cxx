/* Copyright 3/13/19 Washington University.  All Rights Reserved.
   fidl_crop.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <cstdlib>
#include <cstring>
//#include <string>
#include "files_struct.h" 
#include "dim_param2.h" 
#include "checkOS.h" 
#include "stack.h" 
#include "constants.h" 
#include "fidl.h" 
#include "subs_nifti.h" 
int main(int argc,char **argv)
{
char string[MAXNAME],string2[MAXNAME],*strptr,*strptr2;
int i,j,k,l,m,nfiles=0,nroots=0,SunOS_Linux,d2first=0,d2last=0,d2fi,d2li,d2,d2e;
int64_t dims[4]={0,0,0,1};
size_t i1;
float *temp_float,*temp_float2,*tf2,center[3],mmppix[3],sform13;
Files_Struct *files=NULL,*roots=NULL;
Dim_Param2 *dp;
stack fs;

if(argc<5){
    std::cout<<"fidl_crop  Crop images, that is the output image is smaller than the input."<<std::endl;
    std::cout<<"    -files: Files to be cropped. *.conc and *.list ok."<<std::endl;
    std::cout<<"    -roots: This will be added in front, so you can use the subject name if it is lacking (as in the output of HCP prefreesurfer)."<<std::endl;
    std::cout<<"            Add a path in fron of the root if you want the file written to a different directory then the one you are in at runtime."<<std::endl;
    std::cout<<"    -d2first: First slice in the second dimension. Slices start at 1."<<std::endl;
    std::cout<<"    -d2last: Last slice in the second dimension."<<std::endl;
    std::cout<<""<<std::endl;
    std::cout<<"Example"<<std::endl;
    std::cout<<"-------"<<std::endl;
    std::cout<<"1. Align T1 and T2 with HCP prefreesurfer script."<<std::endl;
    std::cout<<"        For the resilience data, the T1 and T2 are slightly out of alignment after converting from dicom to nifti with dcm2niix."<<std::endl;   
    std::cout<<"        Resilience native resolution is 0.8 mm isotropic, 208 x 300 x 320 LPI."<<std::endl;   
    std::cout<<"        HCP prefreesurfer resolution is 0.7 mm isotropic, 260 x 311 x 260 RPI."<<std::endl;   
    std::cout<<"            This was done before I noticed that was 0.8 mm availabe in HCP."<<std::endl;   
    std::cout<<"            I am not redoing it because I would have to redo the manual segmentation."<<std::endl;   
    std::cout<<"2. Use this program to limit T2 slices to hippocampus."<<std::endl;
    std::cout<<"        We're going to use the 260 x 311 x 260 RPI because that is what we're currently segmenting in ITK-SNAP, so we can make an educated guess of the slice selection."<<std::endl; 
    std::cout<<"        AGBR-002 runs from 126-170 PA,"<<std::endl; 
    std::cout<<"            fidl_crop -d2first 121 -d2last 175"<<std::endl; 
    std::cout<<"        We shold also compare to a wider field of view,"<<std::endl; 
    std::cout<<"            fidl_crop -d2first 110 -d2last 185"<<std::endl; 
    std::cout<<"        Then either run the wider field of view on all subjects, or identify the most rostral and caudal slices on all subjects."<<std::endl; 
    std::cout<<"        Or perhaps we shuld redo the whole thing at 0.8 mm."<<std::endl; 
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfiles;
        if(!(files=read_files(nfiles,&argv[i+1])))exit(-1);
        i += nfiles;
        }
    if(!strcmp(argv[i],"-roots") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroots;
        if(!(roots=get_files(nroots,&argv[i+1]))) exit(-1);
        i += nroots;
        }
    //if(!strcmp(argv[i],"-d2first") && argc > i+1)d2first=strtod(argv[++i],NULL);
    //if(!strcmp(argv[i],"-d2last") && argc > i+1)d2last=strtod(argv[++i],NULL);
    //if(!strcmp(argv[i],"-d2first") && argc > i+1)d2first=std::stoi(argv[++i]);
    //if(!strcmp(argv[i],"-d2last") && argc > i+1)d2last=std::stoi(argv[++i]);
    if(!strcmp(argv[i],"-d2first") && argc > i+1)d2first=(int)strtol(argv[++i],NULL,10);
    if(!strcmp(argv[i],"-d2last") && argc > i+1)d2last=(int)strtol(argv[++i],NULL,10);
    }
if(!nfiles){
    std::cout<<"fidlError: Need to specify -files"<<std::endl;
    exit(-1);
    }
if(!nroots) {
    std::cout<<"fidlError: Need to specify -roots"<<std::endl;
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1)exit(-1);
if(!(dp=dim_param2(files->nfiles,files->files,SunOS_Linux)))exit(-1);
if(dp->tdimall!=1) {
    std::cout<<"fidlError: Time dimension > 1. Need to add code to split this type of stack."<<std::endl;
    exit(-1);
    }
std::cout<<"dp->xdim[0]="<<dp->xdim[0]<<" dp->ydim[0]="<<dp->ydim[0]<<" dp->zdim[0]="<<dp->zdim[0]<<std::endl;
std::cout<<"d2first="<<d2first<<" d2last="<<d2last<<std::endl;

temp_float=new float[dp->volall];

d2fi=d2first-1;d2li=d2last-1;d2=d2last-d2first+1;d2e=dp->ydim[0]-d2last;
temp_float2=new float[dp->xdim[0]*d2*dp->zdim[0]];

for(i1=0;i1<files->nfiles;i1++){
    if(!(fs.stack1(files->files[i1],temp_float,SunOS_Linux)))exit(-1);

    std::cout<<"here2 d2fi="<<d2fi<<" d2e="<<d2e<<" d2="<<d2<<std::endl;

    l=m=0;

    tf2=temp_float2; //use ptr to save yourself from rewinding
    for(i=0;i<dp->zdim[i1];i++){
        for(temp_float+=d2fi*dp->xdim[i1],j=0;j<d2;j++){
            for(k=0;k<dp->xdim[i1];k++)*tf2++=*temp_float++;
            }
        temp_float+=d2e*dp->xdim[i1];
        }

    //std::cout<<"here3 l="<<l<<" 260*311*260="<<260*311*260<<std::endl;
    //std::cout<<"here3 m="<<m<<" 260*55*260="<<260*55*260<<std::endl;

    #if 1
    dims[0]=(int64_t)dp->xdim[i1];dims[1]=(int64_t)d2;dims[2]=(int64_t)dp->zdim[i1];
    strcpy(string,files->files[i1]);
    if(!(strptr=get_tail_sans_ext(string)))exit(-1);
    strptr2=roots->files[i1]+strlen(roots->files[i1])-1;
    sprintf(string2,"%s%s%s_c%dto%d.nii",roots->files[i1],!strcmp(strptr2,"/")?"":"_",strptr,d2first,d2last);

    //START190419
    fs.assign(center,mmppix); 
    sform13=d2fi*mmppix[1]+center[1];

    //if(!nifti_write3(string2,files->files[i1],temp_float2,dims)){
    //START190416
    if(!nifti_write3(string2,files->files[i1],temp_float2,dims,sform13)){

        std::cout<<"fidlError: Writing "<<string2<<std::endl;
        exit(-1);
        }

    //START190416
    sprintf(string,"gzip -f %s",string2);
    if(system(string)==-1){std::cout<<"fidlError: Compressing "<<string2<<std::endl;exit(-1);}
    std::cout<<"Cropped image written to "<<string2<<".gz"<<std::endl;
    #endif

#if 0
    dims[0]=(int64_t)dp->xdim[i1];dims[1]=(int64_t)dp->ydim[i1];dims[2]=(int64_t)dp->zdim[i1];
    strcpy(string,files->files[i1]);
    if(!(strptr=get_tail_sans_ext(string)))exit(-1);
    strptr2=roots->files[i1]+strlen(roots->files[i1])-1;
    sprintf(string2,"%s%s%s_cropped_new_whole.nii",roots->files[i1],!strcmp(strptr2,"/")?"":"_",strptr);
    std::cout<<"here0"<<std::endl;
    if(!nifti_write3(string2,files->files[i1],temp_float,dims)){
        std::cout<<"fidlError: Writing "<<string2<<std::endl;
        exit(-1);
        }
    std::cout<<"Cropped image written to "<<string2<<std::endl;
#endif

    }
}
