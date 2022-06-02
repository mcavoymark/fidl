/* Copyright 2/11/19 Washington University.  All Rights Reserved.
   fidl_tc_ss.cxx  $Revision: 1.1 $ */
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "dim_param2.h"
#include "get_atlas_param.h"
#include "mask.h"
#include "read_frames_file.h"
#include "subs_util.h"
#include "t4_atlas.h"
#include "get_atlas_coor.h"
#include "minmax.h"

#if 0
#include "region.h"
#include "find_regions_by_file_cover.h"
#endif
//START211020
#include "region3.h"
#include "find_regions_by_file_cover3.h"

#include "map_disk.h"
#include "checkOS.h"
#include "read_xform.h"
#include "twoA_or_twoB.h"
#include "get_atlas.h"
#include "check_dimensions.h"
#include "d2intvar.h"
#include "filetype.h"
#include "subs_nifti.h"
#include "write1.h"
#include "ptr.h"
#include "timestr.h"

//START210206
#include "ptrw.h"

//START190923
char const* Fileext[]={"",".4dfp.img",".dscalar.nii",".nii",".dtseries.nii"};

//static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_tc_ss.c,v 1.72 2015/12/23 21:44:00 mcavoy Exp $";
int main(int argc,char **argv)
{
char *regional_name="fidl_tc_ss.txt",*xform_file=NULL,filename[MAXNAME],*event_file=NULL,*strptr,write_back[MAXNAME],*scratchdir=NULL,
    *frames_file=NULL,*mask_file=NULL,dummy,**tc_names_cond=NULL,root[MAXNAME],*rootptr=NULL,atlas[7]="",*atlas_reg="",
     *singletrial="there is only a single trial",*zerotrial="there are no trials",*lutf=NULL;
size_t *t1;
int i,j,k,l,m,n,p,q,r=0,s,jj,pp,q1,r1,rr,num_regions=0,num_region_files=0,num_tc_files=0,*roi=NULL,nframes_cond=0,*frames_cond=NULL,
    num_tc_names=0,*tc_length,num_tc_cond=0,*tc_cond,tc_length_sum=0,num_tc=0,A_or_B_or_U=0,num_wfiles=1,lctrials=0,lctrials_only=0,
    tc_length_max=0,nstrings,*ntrials_cond,argc_tc=0,*slen=NULL,*temp_int,nplacing=0,*placing=NULL,noffset=0,*offset=NULL,SunOS_Linux,
    *tcibyf,lccond_and_frames=0,lc_voxels=0,lcsd=0,lcvar=0,lcmean=0,lcsem=0,lcsdavg=0,lcpooledsd=0,npooledsd=0,*minpooledsd=NULL,
    *cpooledsd,flag,count,lccleanup=0,lccompressed=0,*indices=NULL,min,max,lctc=0,lcmeanTR=0,lcsdTR=0,***ntrials_cond1,***ncond1=NULL,
    **ntrials_tc_names=NULL,*slen_tc_names,*maxtrial_tc_names=NULL,**rrr=NULL,**sss=NULL,lenbrain=0,nregval=0,*regval=NULL,
    lcniigz=0,lcniigzonly=0;

//START210412
//int64_t dims[4];

float *t4=NULL,*temp_float; //,center[3],mmppix[3];
double *temp_double,*stat,*weights,***timecourses_sum,***timecourses_sum2,***timecourses=NULL,***mean=NULL,***sem=NULL,*voxel_tc=NULL,
    *col,*row,*slice,*coor,***sd=NULL,*sem1,*sem2,*sd1,*sd2,w,***var=NULL,sum,sum2,*pooledsd,*temp_double2,***tc_sum=NULL,
    ***tc_sum2=NULL,***meanTR=NULL,***sdTR=NULL,***var_cond,***wmean=NULL,***wsem=NULL,mm3=0.,*dptr;
FILE *fp;
Regions **reg;
Regions_By_File *rbf=NULL,*fbf;
Atlas_Param *ap,*ap_reg=NULL;
Dim_Param2 *dp;
Memory_Map *mm=NULL,**mm_wfiles=NULL;
Interfile_header *ifh=NULL,*ifh_reg;
TC *tcs;
mask ms,ms0,*ms1;
FS *fs;
Files_Struct *tc_names=NULL,*tc_files=NULL,*region_files=NULL,*wfiles=NULL;
W1 *w1=NULL;
header h;
timestr t0;

if(argc<5){

    #if 0
    fprintf(stderr,"Mean, sem, and sd maps are all weighted means by the number of trials for each condition when summing across\n");
    fprintf(stderr,"conditions. If instead you just want to collapse across conditions without any weighting for the number of \n");
    fprintf(stderr,"trials, then you should make a new event file.\n");
    fprintf(stderr,"        -tc_files:            imgs or concs.\n");
    fprintf(stderr,"        -region_file:         *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"                              Timecourses are averaged over the region.\n");
    fprintf(stderr,"        -regions_of_interest: Compute timecourses for selected regions in the region file(s).\n");
    fprintf(stderr,"                              First region is one.\n");
    fprintf(stderr,"        -regval:              Used for niftis. Freesurfer numbers.\n");
    fprintf(stderr,"        -lut:                 Lookup table. Ex. $FREESURFER_HOME/FreeSurferColorLUT.txt\n");
    fprintf(stderr,"        -weight_files:        4dfp weight files. Provide weights for regional weighted means.\n");
    fprintf(stderr,"                              Without this option the regional arithmetic mean is computed.\n");
    fprintf(stderr,"        -regional_name:       Output filename for regional timecourses. Default is fidl_tc_ss.txt\n");
    fprintf(stderr,"        -tc_names:            Timcourse identifiers.\n");
    fprintf(stderr,"        -tc_cond:             Identifier from second column of event file.\n");
    fprintf(stderr,"        -xform_file:          Name of 2A or 2B t4 file defining the transform to atlas space.\n");
    fprintf(stderr,"        -trials               Regional timecourses for each trial.\n");
    fprintf(stderr,"        -trials ONLY          Do not average timecourses across trials.\n");
    fprintf(stderr,"        -event_file           Used for documentation.\n");
    fprintf(stderr,"        -mask:                Only voxels in the mask are analyzed.\n");
    fprintf(stderr,"        -placing:             1=TR 2=1/2TR 4=1/4TR Default is on the TR.\n");
    fprintf(stderr,"        -offset:              Trial offsets. Used with -placing.\n");
    fprintf(stderr,"    Use these\n");
    fprintf(stderr,"        -frames:              Frames to extract. First frame is 1.\n");
    fprintf(stderr,"        -frames_cond:         Identifier for each trial from second column of event file.\n");
    fprintf(stderr,"    Or\n");
    fprintf(stderr,"        -cond_and_frames      Identifier, frames.\n");
    fprintf(stderr,"                              Need not be comma separated, but easier on the eyes.\n");
    fprintf(stderr,"        -voxels               Output voxel values of regions.\n");
    fprintf(stderr,"    THESE ARE TRIAL BASED MEASURES.\n");
    #endif
    std::cout<<"Mean, sem, and sd maps are all weighted means by the number of trials for each condition when summing across"<<std::endl;
    std::cout<<"conditions. If instead you just want to collapse across conditions without any weighting for the number of"<<std::endl;
    std::cout<<"trials, then you should make a new event file."<<std::endl;
    std::cout<<"        -tc_files:            imgs or concs."<<std::endl;
    std::cout<<"        -region_file:         4dfp's or nifti's that specify the regions of interest."<<std::endl;
    std::cout<<"                              Timecourses are averaged over the region."<<std::endl;
    std::cout<<"        -regions_of_interest: Compute timecourses for selected regions in the region file(s)."<<std::endl;
    std::cout<<"                              First region is one."<<std::endl;
    std::cout<<"        -regval:              Used for niftis. Freesurfer numbers."<<std::endl;
    std::cout<<"        -lut:                 Lookup table. Ex. $FREESURFER_HOME/FreeSurferColorLUT.txt"<<std::endl;
    std::cout<<"        -weight_files:        4dfp weight files. Provide weights for regional weighted means."<<std::endl;
    std::cout<<"                              Without this option the regional arithmetic mean is computed."<<std::endl;
    std::cout<<"        -regional_name:       Output filename for regional timecourses. Default is fidl_tc_ss.txt"<<std::endl;
    std::cout<<"        -tc_names:            Timcourse identifiers."<<std::endl;
    std::cout<<"        -tc_cond:             Identifier from second column of event file."<<std::endl;
    std::cout<<"        -xform_file:          Name of 2A or 2B t4 file defining the transform to atlas space."<<std::endl;
    std::cout<<"        -trials               Regional timecourses for each trial."<<std::endl;
    std::cout<<"        -trials ONLY          Do not average timecourses across trials."<<std::endl;
    std::cout<<"        -event_file           Used for documentation."<<std::endl;
    std::cout<<"        -mask:                Only voxels in the mask are analyzed."<<std::endl;
    std::cout<<"        -placing:             1=TR 2=1/2TR 4=1/4TR Default is on the TR."<<std::endl;
    std::cout<<"        -offset:              Trial offsets. Used with -placing."<<std::endl;
    std::cout<<"    Use these"<<std::endl;
    std::cout<<"        -frames:              Frames to extract. First frame is 1."<<std::endl;
    std::cout<<"        -frames_cond:         Identifier for each trial from second column of event file."<<std::endl;
    std::cout<<"    Or"<<std::endl;
    std::cout<<"        -cond_and_frames      Identifier, frames."<<std::endl;
    std::cout<<"                              Need not be comma separated, but easier on the eyes."<<std::endl;
    std::cout<<"        -voxels               Output voxel values of regions."<<std::endl;
    std::cout<<"    THESE ARE TRIAL BASED MEASURES."<<std::endl;

    std::cout<<"        -mean                 Output mean map."<<std::endl;
    std::cout<<"        -sem                  Output sem map. Calculated using the WinCross method."<<std::endl;
    std::cout<<"        -var                  Output var map. Calculated using the WinCross method."<<std::endl;
    std::cout<<"        -sd                   Output sd map. Sqrt of var."<<std::endl;


    fprintf(stderr,"        -sdavg                Output sd map, averaged across time frames.\n");
    fprintf(stderr,"        -pooledsd:            Minimum number of frames of each trial must be included in the pooledsd.\n");
    fprintf(stderr,"                              One number for each timecourse. Default is to use all trials.\n");
    fprintf(stderr,"                              Variance is computed across each trial (ie one number for each trial) and\n");
    fprintf(stderr,"                              averaged, then the sqrt taken of the average. This gives it the feel of a rms\n");
    fprintf(stderr,"                              measure and is consistent with how amplitude power is computed by\n");
    fprintf(stderr,"                              fidl_zstat. No weighting is applied, with the thinking that at the trial\n");
    fprintf(stderr,"                              level, one trial is just as good as another, under the assumption that each\n");
    fprintf(stderr,"                              trial has the same number of timepoints, although this is not checked.\n");
    fprintf(stderr,"    THESE ARE NOT TRIAL BASED MEASURES.\n");
    fprintf(stderr,"        -meanTR               Mean is computed across all TRs.\n");
    fprintf(stderr,"        -sdTR                 Standard deviation is computed across all TRs.\n");
    fprintf(stderr,"    -scratchdir:          Location of precomputed scratchfiles. Include the backslash at the end.\n");
    fprintf(stderr,"    -clean_up             Delete files used to compute timecourses.\n");

    //START210205
    std::cout<<"        -nii.gz               Output nii.gz for regional values and text file."<<std::endl;
    std::cout<<"        -nii.gz ONLY          Output only nii.gz for regional values (no text file)."<<std::endl;

    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-tc_files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++num_tc_files;
        if(!(tc_files=read_files(num_tc_files,&argv[i+1])))exit(-1);
        strcpy(root,argv[i+1]);
        if(!(rootptr=get_tail_sans_ext(root))) exit(-1);
        i+=num_tc_files;
        }
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;
        if(!(roi=(int*)malloc(sizeof*roi*num_regions))) {
            printf("Error: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
        }


    //START200107
    if(!strcmp(argv[i],"-regval") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nregval;
        if(!(regval=(int*)malloc(sizeof*regval*nregval))) {
            printf("fidlError: Unable to malloc regval\n");
            exit(-1);
            }

        //for(j=0;j<nregval;j++) regval[j] = atoi(argv[++i]);
        //START200207
        for(j=0;j<nregval;j++)regval[j]=strtof(argv[++i],NULL);

        }

    //START200312
    if(!strcmp(argv[i],"-lut")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            lutf = argv[++i];
            }
        else {
            printf("fidlError: No lookup table specified after -lut option. Abort!\n");
            exit(-1);
            }
        }
 


    if(!strcmp(argv[i],"-weight_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_wfiles;
        num_wfiles--; /*Initialized to 1 instead of 0 in declaration.*/
        if(!(wfiles=get_files(num_wfiles,&argv[i+1]))) exit(-1);
        i += num_wfiles;
        }
    if(!strcmp(argv[i],"-frames") && argc > i+1)
        frames_file = argv[++i];
    if(!strcmp(argv[i],"-frames_cond") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nframes_cond;
        if(!(frames_cond=(int*)malloc(sizeof*frames_cond*nframes_cond))) {
            printf("fidlError: Unable to malloc frames_cond\n");
            exit(-1);
            }
        for(j=0;j<nframes_cond;j++) frames_cond[j] = atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-regional_name") && argc > i+1)
        regional_name = argv[++i];
    if(!strcmp(argv[i],"-tc_names") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_names;
        if(!(tc_names=get_files(num_tc_names,&argv[i+1]))) exit(-1);
        i += num_tc_names;
        }
    if(!strcmp(argv[i],"-tc_cond") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc;
        argc_tc = i+1;
        i += num_tc;
        }
    if(!strcmp(argv[i],"-xform_file") && argc > i+1)
        xform_file = argv[++i];
    if(!strcmp(argv[i],"-trials")) {
        lctrials = 1;
        if(argc > i+1 && !strcmp(argv[i+1],"ONLY")) {
            lctrials_only = 1;
            ++i;
            }
        lctc=1;
        }
    if(!strcmp(argv[i],"-event_file") && argc > i+1)
        event_file = argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-placing") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nplacing;
        if(!(placing=(int*)malloc(sizeof*placing*nplacing))) {
            printf("Error: Unable to malloc placing\n");
            exit(-1);
            }
        for(j=0;j<nplacing;j++) placing[j] = atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-offset") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++noffset;
        if(!(offset=(int*)malloc(sizeof*offset*noffset))) {
            printf("Error: Unable to malloc offset\n");
            exit(-1);
            }
        for(j=0;j<noffset;j++) offset[j] = atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-cond_and_frames") && argc > i+1) {
        frames_file = argv[++i];
        lccond_and_frames = 1;
        }
    if(!strcmp(argv[i],"-voxels"))
        lc_voxels = 1;
    if(!strcmp(argv[i],"-mean"))
        lcmean = 1;
    if(!strcmp(argv[i],"-sem"))
        lcsem = 1;
    if(!strcmp(argv[i],"-sd"))
        lcsd = 1;
    if(!strcmp(argv[i],"-var"))
        lcvar = 1;
    if(!strcmp(argv[i],"-sdavg"))
        lcsdavg = 1;
    if(!strcmp(argv[i],"-pooledsd")) {
        lcpooledsd = 1;
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++npooledsd;
        if(npooledsd) {
            if(!(minpooledsd=(int*)malloc(sizeof*minpooledsd*npooledsd))) {
                printf("fidlError: Unable to malloc minpooledsd\n");
                exit(-1);
                }
            for(j=0;j<npooledsd;j++) minpooledsd[j] = atoi(argv[++i]);
            }
        }
    if(!strcmp(argv[i],"-clean_up"))
        lccleanup=1;
    if(!strcmp(argv[i],"-meanTR"))
        lcmeanTR = 1;
    if(!strcmp(argv[i],"-sdTR"))
        lcsdTR = 1;
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1)
        scratchdir = argv[++i];

    //START210205
    if(!strcmp(argv[i],"-nii.gz")){
        lcniigz = 1;
        if(argc > i+1){
            if(!strcmp(argv[i+1],"ONLY")){
                lcniigzonly=1;++i;
                }
            }
        }

    }
//print_version_number(rcsid,stdout);
std::cout<<argv[0]<<std::endl;



if((SunOS_Linux=checkOS())==-1) exit(-1);

if(!frames_file) {
    printf("fidlError: Need to specify frames file with -frames option. Abort!\n");
    exit(-1);
    }
if(!(temp_int=(int*)malloc(sizeof*temp_int*num_tc))) {
    printf("fidlError: Unable to malloc temp_int\n");
    exit(-1);
    }
for(i=0;i<num_tc;i++) temp_int[i] = 1;
if(!(tcs=read_tc_string_new(num_tc,temp_int,argc_tc,argv))) exit(-1);
free(temp_int);
if(num_tc_names != tcs->num_tc) {
    printf("fidlError: num_tc_names=%d tcs->num_tc=%d They must be equal. Abort!\n",num_tc_names,tcs->num_tc);
    exit(-1);
    }
if(npooledsd) {
    if(npooledsd!=tcs->num_tc) {
        printf("fidlError: npooledsd=%d tcs->num_tc=%d They must be equal. Abort!\n",npooledsd,tcs->num_tc);
        exit(-1);
        }
    }
else if(lcpooledsd) {
    if(!(minpooledsd=(int*)malloc(sizeof*minpooledsd*tcs->num_tc))) {
        printf("fidlError: Unable to malloc minpooledsd\n");
        exit(-1);
        }
    for(i=0;i<tcs->num_tc;i++) minpooledsd[i]=10000;
    }

printf("Reading %s\n",frames_file);fflush(stdout);

if(!(fs=read_frames_file(frames_file,lccond_and_frames,0,0))) exit(-1);

//printf("fs->nframes=%d\n",fs->nframes);fflush(stdout);


if(lccond_and_frames) {
    nframes_cond = fs->nlines;
    frames_cond = fs->frames_cond;
    }

//printf("nframes_cond=%d\n",nframes_cond); 
//for(i=0;i<nframes_cond;i++) printf("frames_cond[%d]=%d frames_per_line[%d]=%d\n",i,frames_cond[i],i,fs->frames_per_line[i]); 
#if 0
printf("fs->ntc=%d fs->nlines=%d fs->nframes=%d\n",fs->ntc,fs->nlines,fs->nframes);
printf("fs->num_frames_to_sum\n"); for(i=0;i<fs->ntc;i++) printf("%d ",fs->num_frames_to_sum[i]); printf("\n"); fflush(stdout);
#endif

if(fs->nlines != nframes_cond) {
    printf("fidlError: %d lines read from %s. However nframes_cond = %d They must be the same. Abort!\n",fs->nlines,frames_file,
        nframes_cond);
    fflush(stdout);exit(-1);
    }
for(min=max=frames_cond[0],i=1;i<nframes_cond;i++){if(frames_cond[i]<min)min=frames_cond[i];if(frames_cond[i]>max)max=frames_cond[i];}

printf("min=%d max=%d\n",min,max);

for(num_tc_cond=1,i=min+1;i<=max;i++) {
    for(j=0;frames_cond[j]!=i && j<nframes_cond;j++);
    if(j < nframes_cond) num_tc_cond++;
    }
if(noffset) {
    if(noffset != nframes_cond) {
        printf("fidlError: noffset=%d nframes_cond=%d Must be equal. Abort!\n",noffset,nframes_cond);
        exit(-1);
        }
    }
else {
    if(!(offset=(int*)malloc(sizeof*offset*nframes_cond))) {
        printf("fidlError: Unable to malloc offset\n");
        exit(-1);
        }
    for(j=0;j<nframes_cond;j++) offset[j] = 0;
    }
if(nplacing) {
    if(nplacing != num_tc_cond) {
        printf("fidlError: nplacing=%d num_tc_names=%d Must be equal. Abort!\n",nplacing,num_tc_names);
        exit(-1);
        }
    }
else {
    if(!(placing=(int*)malloc(sizeof*placing*num_tc_cond))) {
        printf("fidlError: Unable to malloc placing\n");
        exit(-1);
        }
    for(j=0;j<num_tc_cond;j++) placing[j] = 1;
    }
printf("placing="); for(i=0;i<num_tc_cond;i++) printf("%d ",placing[i]); printf("\n");
/*printf("offset\n"); for(i=0;i<nframes_cond;i++) printf("%d ",offset[i]); printf("\n");*/
if(!(tc_cond=(int*)malloc(sizeof*tc_cond*num_tc_cond))) {
    printf("fidlError: Unable to malloc tc_cond\n");
    exit(-1);
    }
tc_cond[0] = (int)min;
tc_cond[num_tc_cond-1] = (int)max;
for(k=1,i=(int)min+1;i<(int)max;i++) {
    for(j=0;frames_cond[j]!=i && j<nframes_cond;j++);
    if(j < nframes_cond) tc_cond[k++] = i;
    }
printf("num_tc_cond=%d ",num_tc_cond); printf("tc_cond="); for(i=0;i<num_tc_cond;i++) printf("%d ",tc_cond[i]); printf("\n");
if(event_file) {
    if(!(fp = fopen_sub(event_file,"r"))) exit(-1);
    if(!fgets(filename,sizeof(filename),fp)){printf("fidlErorr: fgets returned a null pointer\n");fflush(stdout);exit(-1);}
    fclose(fp);
    nstrings = count_strings_new(filename,write_back,' ',&dummy) - 1;
    if(nstrings < num_tc_cond) {
        printf("fidlError: nstrings=%d num_tc_cond=%d. nstrings must be equal or larger.\n",nstrings,num_tc_cond);fflush(stdout);
        exit(-1);
        }
    if(!(t1=(size_t*)malloc(sizeof*t1*num_tc_cond))) {
        printf("fidlError: Unable to malloc t1\n");
        exit(-1);
        }
    for(strptr=grab_string(write_back,filename),j=i=0;i<nstrings;i++) {
        strptr = grab_string(strptr,filename);
        if(i==tc_cond[j]) t1[j++] = strlen(filename) + 1;
        }
    if(!(tc_names_cond = d2charvar(num_tc_cond,t1))) exit(-1);
    for(strptr=grab_string(write_back,filename),j=i=0;i<nstrings;i++) {
        strptr = grab_string(strptr,filename);
        if(i==tc_cond[j]) {
            strcpy(tc_names_cond[j++],filename);
            if(j==num_tc_cond) break;
            }
        }
    free(t1);
    }
if(!(tc_length=(int*)malloc(sizeof*tc_length*num_tc_cond))) {
    printf("fidlError: Unable to malloc tc_length\n");
    exit(-1);
    }
for(i=0;i<num_tc_cond;i++) tc_length[i] = 0; 
if(!(tcibyf=(int*)malloc(sizeof*tcibyf*nframes_cond))) {
    printf("fidlError: Unable to malloc tcibyf\n");
    exit(-1);
    }
for(tc_length_max=i=0;i<nframes_cond;i++) {
    for(j=0;frames_cond[i] != tc_cond[j];j++);
    tcibyf[i] = j;
    if(fs->frames_per_line[i] > tc_length[j]) tc_length[j] = fs->frames_per_line[i];
    if((s=fs->frames_per_line[i]*placing[tcibyf[i]]) > tc_length_max) tc_length_max = s;
    }
for(tc_length_sum=i=0;i<num_tc_cond;i++) tc_length_sum += tc_length[i]*placing[i];

#if 1
printf("tcibyf\n"); for(i=0;i<nframes_cond;i++) printf("%d ",tcibyf[i]); printf("\n");
printf("tc_length="); for(i=0;i<num_tc_cond;i++) printf("%d ",tc_length[i]); printf("\n");
printf("tc_length_max=%d\n",tc_length_max);
printf("tc_length_sum=%d\n",tc_length_sum);
#endif

if(!(ntrials_cond=(int*)malloc(sizeof*ntrials_cond*tc_length_sum))) {
    printf("fidlError: Unable to malloc ntrials_cond\n");
    exit(-1);
    }
for(i=0;i<tc_length_sum;i++) ntrials_cond[i]=0;
for(i=0;i<nframes_cond;i++) {
    if(!fs->frames_per_line[i]) {
        printf("ERROR: Zero frames detected for a %s event. Could be a mistake in your event file.\n",
            tc_names->files[frames_cond[i]]);
        printf("       Please see line %d in %s\n",i+1,frames_file);
        } 
    else {
        for(m=j=0;frames_cond[i]!=tc_cond[j];m+=tc_length[j++]*placing[tcibyf[i]]);
        /*printf("here1c m=%d j=%d i=%d tcibyf[%d]=%d placing[%d]=%d \n",m,j,i,i,tcibyf[i],tcibyf[i],placing[tcibyf[i]]);
        fflush(stdout);*/
        for(j=offset[i],k=0;k<fs->frames_per_line[i];k++,j+=placing[tcibyf[i]]) ntrials_cond[m+j]++;
        /*for(j=offset[i],k=0;k<fs->frames_per_line[i];k++,j+=placing[tcibyf[i]]) {
            printf("ntrials_cond[%d]=%d\n",m+j,ntrials_cond[m+j]);
            printf("m=%d j=%d\n",m,j);
            ntrials_cond[m+j]++;
            }*/
        }
    }

printf("ntrials_cond=");for(i=0;i<tc_length_sum;i++)printf("%d ",ntrials_cond[i]);printf("\n");

//if(!(dp=dim_param2(tc_files->nfiles,tc_files->files,SunOS_Linux)))exit(-1);
//START190920
if(!(dp=dim_param2(tc_files->nfiles,tc_files->files)))exit(-1);

std::cout<<"dp->xdim[0]="<<dp->xdim[0]<<" dp->ydim[0]="<<dp->ydim[0]<<" dp->zdim[0]="<<dp->zdim[0]<<" dp->vol[0]="<<dp->vol[0]<<std::endl;
std::cout<<"dp->tdim[0]="<<dp->tdim[0]<<" dp->tdimall="<<dp->tdimall<<" fs->nframes="<<fs->nframes<<std::endl;
if(dp->tdimall<fs->nframes){
    std::cout<<"Total frames in tc_files are less than that in the frames file. Abort!"<<std::endl;
    exit(-1);
    }


//START191206
mm3=(double)(dp->dx[0]*dp->dy[0]*dp->dz[0]);

if(!dp->volall==-1){printf("fidlError: All files must have the same volume.\n");fflush(stdout);exit(-1);}
if(xform_file) {
    if(!(t4=(float*)malloc(sizeof*t4*(size_t)T4SIZE))) {
        printf("fidlError: Unable to malloc t4\n");
        exit(-1);
        }
    if(!read_xform(xform_file,t4)) exit(-1);
    if((A_or_B_or_U=twoA_or_twoB(xform_file)) == 2) exit(-1);
    }
else {
    get_atlas(dp->volall,atlas);
    }

if(!num_region_files) {
    if(!(ms.get_mask(mask_file,dp->volall,(int*)NULL,(LinearModel*)NULL,dp->volall)))exit(-1);
    num_regions=lenbrain=ms.lenbrain;
    if(dp->volall==num_regions)get_atlas(ms.vol,atlas);

    //START210205
    ms1=&ms; 
    }

if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL,(char*)NULL))) exit(-1);
if(num_region_files||!atlas[0])ap->vol = dp->volall;
if(!(weights=(double*)malloc(sizeof*weights*ap->vol))) {
    printf("fidlError: Unable to malloc weights\n");
    exit(-1);
    }

if(num_region_files) {
    if(!check_dimensions(num_region_files,region_files->files,dp->volall)) {
        lccompressed = 1;
        printf("    Error negated. Setting lccompressed. dp->volall=%d lccompressed=%d\n",dp->volall,lccompressed);fflush(stdout);
        }
    if(!(reg=(Regions**)malloc(sizeof*reg*num_region_files))) {
        printf("fidlError: Unable to malloc reg\n");
        exit(-1);
        }

    #if 0
    int lcflip=shouldiflip(region_files); 
    std::cout<<"lcflip="<<lcflip<<std::endl;
    for(i=0;i<num_region_files;i++)if(!(reg[i]=get_reg(region_files->files[i],0,(float*)NULL,0,(char**)NULL,lcflip,0,nregval,regval)))exit(-1);
    if(!num_regions) {
        for(m=0;m<num_region_files;m++) num_regions += reg[m]->nregions;
        if(!(roi=(int*)malloc(sizeof*roi*num_regions))) {
            printf("fidlError: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = j;
        }
    #endif
    //START200312
    for(i=0;i<num_region_files;i++)if(!(reg[i]=get_reg(region_files->files[i],0,(float*)NULL,0,(char**)NULL,0,0,nregval,regval,lutf)))exit(-1);
    if(!num_regions)for(i=0;i<num_region_files;i++)num_regions+=reg[i]->nregions;
    if(!roi){
        if(!(roi=(int*)malloc(sizeof*roi*num_regions))){
            printf("fidlError: Unable to malloc roi\n");
            exit(-1);
            }
        for(i=0;i<num_regions;i++)roi[i]=i;
        }


    //if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);
    //START211020
    if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi,NULL)))exit(-1);
                
    //START211102     
    //std::cout<<"rbf->nreg="<<rbf->nreg<<std::endl;std::cout<<"rbf->region_names_ptr=";for(j=0;j<rbf->nreg;++j)std::cout<<" "<<rbf->region_names_ptr[j];std::cout<<std::endl;

    if(wfiles) {
        if(!check_dimensions(num_wfiles,wfiles->files,ap->vol)) exit(-1);
        if(!(mm_wfiles=(Memory_Map**)malloc(sizeof*mm*num_wfiles))) {
            printf("fidlError: Unable to malloc mm_wfiles\n");
            exit(-1);
            }
        for(i=0;i<num_wfiles;i++) if(!(mm_wfiles[i] = map_disk(wfiles->files[i],ap->vol,0,sizeof(float)))) exit(-1);
        }
    else {
        for(i=0;i<ap->vol;i++) weights[i] = 1;
        }
    if(lccompressed) {
        if(rbf->nvoxels!=dp->volall) {
            printf("fidlError: rbf->nvoxels=%d dp->volall=%d  Must be equal.\n",rbf->nvoxels,dp->volall);
            exit(-1);
            }
        if(!(indices=(int*)malloc(sizeof*indices*rbf->nvoxels))) {
            printf("fidlError: Unable to malloc indices\n");
            exit(-1);
            }
        for(i=0;i<rbf->nvoxels;i++) indices[i]=rbf->indices[i];
        for(i=0;i<rbf->nvoxels;i++) rbf->indices[i]=i;
        if(!(ifh_reg=read_ifh(region_files->files[0],(Interfile_header*)NULL))) exit(-1);
        get_atlas(ifh_reg->dim1*ifh_reg->dim2*ifh_reg->dim3,atlas_reg);
        if(!(ap_reg=get_atlas_param(atlas_reg,(Interfile_header*)NULL,(char*)NULL))) exit(-1);
        }
    else {
        ap_reg = ap;
        }

    //if(!(ms.get_mask((char*)NULL,rbf->nvoxels,rbf->indices,SunOS_Linux,(LinearModel*)NULL,dp->volall)))exit(-1);
    //START190920
    if(!(ms.get_mask((char*)NULL,rbf->nvoxels,rbf->indices,(LinearModel*)NULL,dp->volall)))exit(-1);

    //START210205
    if(!(ms0.get_mask((char*)NULL,num_regions,(int*)NULL,(LinearModel*)NULL,num_regions)))exit(-1);
    ms1=&ms0;

    if(lc_voxels) {
        if(!(voxel_tc=(double*)malloc(sizeof*voxel_tc*rbf->nvoxels*fs->ntc))) {
            printf("Error: Unable to malloc voxel_tc\n");
            exit(-1);
            }
        }
    }

//if(!(fbf=find_regions_by_file(tc_files->nfiles,fs->nframes,dp->tdim,fs->frames))) exit(-1);
//START211020
if(!(fbf=find_regions_by_file(tc_files->nfiles,fs->nframes,dp->tdim,fs->frames,NULL)))exit(-1);

#if 0
printf("fbf->num_regions_by_file=");for(m=0;m<num_tc_files;m++)printf("%d ",fbf->num_regions_by_file[m]);printf("\n");
printf("fbf->file_index=");for(m=0;m<fs->nframes;m++)printf("%d ",fbf->file_index[m]);printf("\n");
printf("fbf->roi_by_file\n");
for(m=0;m<num_tc_files;m++) {
    for(i=0;i<fbf->num_regions_by_file[m];i++) printf("%d ",fbf->roi_by_file[m][i]); 
    printf("\n");
    }
fflush(stdout);
#endif

if(!(temp_double=(double*)malloc(sizeof*temp_double*ap->vol))) {
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(temp_double2=(double*)malloc(sizeof*temp_double2*ap->vol))) {
    printf("fidlError: Unable to malloc temp_double2\n");
    exit(-1);
    }
if(!(stat=(double*)malloc(sizeof*stat*ap->vol))) {
    printf("fidlError: Unable to malloc stat\n");
    exit(-1);
    }
if(!(sem1=(double*)malloc(sizeof*sem1*tc_length_max))) {
    printf("fidlError: Unable to malloc sem1\n");
    exit(-1);
    }
if(!(sem2=(double*)malloc(sizeof*sem2*tc_length_max))) {
    printf("fidlError: Unable to malloc sem2\n");
    exit(-1);
    }
if(!(sd1=(double*)malloc(sizeof*sd1*tc_length_max))) {
    printf("fidlError: Unable to malloc sd1\n");
    exit(-1);
    }
if(!(sd2=(double*)malloc(sizeof*sd2*tc_length_max))) {
    printf("fidlError: Unable to malloc sd2\n");
    exit(-1);
    }

if(!(timecourses_sum = d3double(num_wfiles,num_regions,tc_length_sum))) exit(-1);
if(!(timecourses_sum2 = d3double(num_wfiles,num_regions,tc_length_sum))) exit(-1);
if(!(var_cond = d3double(num_wfiles,num_regions,tc_length_sum))) exit(-1);
if(lcpooledsd&&!lctrials_only) lctrials=1;
if(lctrials) {
    /*printf("here0 num_wfiles=%d num_regions=%d fs->nlines=%d tc_length_max=%d\n",num_wfiles,num_regions,fs->nlines,tc_length_max);*/
    if(!(timecourses = d3double(num_wfiles*num_regions,fs->nlines,tc_length_max))) exit(-1);
    }
if(!lctrials_only) {
    if(!(mean = d3double(num_wfiles,num_tc_names,num_regions*tc_length_max))) exit(-1);
    if(!(sem = d3double(num_wfiles,num_tc_names,num_regions*tc_length_max))) exit(-1);
    if(!(sd = d3double(num_wfiles,num_tc_names,num_regions*tc_length_max))) exit(-1);
    if(!(var = d3double(num_wfiles,num_tc_names,num_regions*tc_length_max))) exit(-1);
    if(!(slen=(int*)malloc(sizeof*slen*num_tc_names))) {
        printf("fidlError: fidl_tc_ss Unable to malloc slen\n");
        exit(-1);
        }
    for(i=0;i<num_tc_names;i++) slen[i]=0;
    if(!(wmean = d3double(num_wfiles,num_tc_names,num_regions*tc_length_max))) exit(-1);
    if(!(wsem = d3double(num_wfiles,num_tc_names,num_regions*tc_length_max))) exit(-1);
    if(!(slen_tc_names=(int*)malloc(sizeof*slen_tc_names*num_tc_names))) {
        printf("fidlError: fidl_tc_ss Unable to malloc slen_tc_names\n");
        exit(-1);
        }

    //for(i=0;i<num_tc_names;i++)slen_tc_names[i]=0;
    //START151215
    for(i=0;i<num_tc_names;i++)slen_tc_names[i]=1;

    #if 0
    if(!(notonetrial_tc_names=malloc(sizeof*notonetrial_tc_names*num_tc_names))) {
        printf("fidlError: fidl_tc_ss Unable to malloc notonetrial_tc_names\n");
        exit(-1);
        }
    for(i=0;i<num_tc_names;i++)notonetrial_tc_names[i]=0;
    #endif
    //START151215
    if(!(maxtrial_tc_names=(int*)malloc(sizeof*maxtrial_tc_names*num_tc_names))) {
        printf("fidlError: fidl_tc_ss Unable to malloc maxtrial_tc_names\n");
        exit(-1);
        }
    for(i=0;i<num_tc_names;i++)maxtrial_tc_names[i]=0;



    #if 0
    for(m=0;m<num_tc_names;m++){
        for(l=0;l<tcs->num_tc_to_sum[m][0];l++) {
            for(q=r=0;(int)tcs->tc[m][0][l]!=tc_cond[q];r+=tc_length[q]*placing[q],q++);
            if((s=tc_length[q]*placing[q])>slen_tc_names[m])slen_tc_names[m]=s;
            }
        }
    #endif
    #if 0
    //START151214
    for(m=0;m<num_tc_names;m++){
        printf("tcs->num_tc_to_sum[%d][0]=%d\n",m,tcs->num_tc_to_sum[m][0]);fflush(stdout);
        for(l=0;l<tcs->num_tc_to_sum[m][0];l++) {
            for(q=0;((int)tcs->tc[m][0][l]!=tc_cond[q])&&(q<num_tc_cond);q++);

            //printf("here5 tcs->tc[%d][0][%d]=%f tc_cond[%d]=%d\n",m,l,tcs->tc[m][0][l],q,tc_cond[q]);fflush(stdout);

            //if((s=tc_length[q]*placing[q])>slen_tc_names[m])slen_tc_names[m]=s;
            //START151217
            if(q<num_tc_cond)if((s=tc_length[q]*placing[q])>slen_tc_names[m])slen_tc_names[m]=s;

            }
        }
    #endif
    //START151218
    if(!(temp_int=(int*)malloc(sizeof*temp_int*num_tc))) {
        printf("fidlError: fidl_tc_ss Unable to malloc temp_int\n");
        exit(-1);
        }
    for(m=0;m<num_tc_names;m++)temp_int[m]=tcs->num_tc_to_sum[m][0];
    if(!(sss=d2intvar(num_tc_names,temp_int)))exit(-1); 
    if(!(rrr=d2intvar(num_tc_names,temp_int)))exit(-1); 
    free(temp_int);
    for(m=0;m<num_tc_names;m++){
        for(l=0;l<tcs->num_tc_to_sum[m][0];l++) {
            for(q=r=0;((int)tcs->tc[m][0][l]!=tc_cond[q])&&(q<num_tc_cond);q++)r+=tc_length[q]*placing[q];
            if(q<num_tc_cond){
                //if((sss[m][l]=tc_length[q]*placing[q])>slen_tc_names[m]){slen_tc_names[m]=slen[m]=sss[m][l];}
                if((sss[m][l]=tc_length[q]*placing[q])>slen[m]){slen[m]=sss[m][l];}
                rrr[m][l]=r;
                }
            }
        }
    for(m=0;m<num_tc_names;m++)slen_tc_names[m]=slen[m]?slen[m]:1;

    #if 0
    if(!(ntrials_tc_names=d2intvar(num_tc_names,slen_tc_names)))exit(-1); 
    for(m=0;m<num_tc_names;m++) {
        for(l=0;l<tcs->num_tc_to_sum[m][0];l++) {

            //for(q=r=0;(int)tcs->tc[m][0][l]!=tc_cond[q];r+=tc_length[q]*placing[q],q++);
            //s = tc_length[q]*placing[q];
            //for(i=0;i<s;i++) ntrials_tc_names[m][i]+=ntrials_cond[r+i];
            //START151215
            //for(q=r=0;((int)tcs->tc[m][0][l]!=tc_cond[q])&&(q<num_tc_cond);r+=tc_length[q]*placing[q],q++);
            //s = tc_length[q]*placing[q];
            //for(i=0;i<s;i++) ntrials_tc_names[m][i]+=ntrials_cond[r+i];
            //START151217
            for(q=r=0;((int)tcs->tc[m][0][l]!=tc_cond[q])&&(q<num_tc_cond);q++)r+=tc_length[q]*placing[q];
            if(q<num_tc_cond)for(s=tc_length[q]*placing[q],i=0;i<s;i++)ntrials_tc_names[m][i]+=ntrials_cond[r+i];

            }

        //for(i=l=0;l<slen_tc_names[m];l++)if(ntrials_tc_names[m][l]>1){notonetrial_tc_names[m]=1;break;}
        //START151215
        for(i=l=0;l<slen_tc_names[m];l++)if(ntrials_tc_names[m][l]>maxtrial_tc_names[m])maxtrial_tc_names[m]=ntrials_tc_names[m][l];
        }
    #endif
    //START151218
    if(!(ntrials_tc_names=d2intvar(num_tc_names,slen_tc_names)))exit(-1);
    for(m=0;m<num_tc_names;m++) {
        for(l=0;l<tcs->num_tc_to_sum[m][0];l++) {
            for(i=0;i<sss[m][l];i++)ntrials_tc_names[m][i]+=ntrials_cond[rrr[m][l]+i];
            }
        for(l=0;l<slen_tc_names[m];l++)if(ntrials_tc_names[m][l]>maxtrial_tc_names[m])maxtrial_tc_names[m]=ntrials_tc_names[m][l];
        }


    printf("slen_tc_names=");for(m=0;m<num_tc_names;m++)printf("%d ",slen_tc_names[m]);printf("\n");fflush(stdout);
    printf("slen=");for(m=0;m<num_tc_names;m++)printf("%d ",slen[m]);printf("\n");fflush(stdout);
    printf("ntrials_tc_names\n");for(m=0;m<num_tc_names;m++){
        printf("    %s ",tc_names->files[m]);for(i=0;i<slen_tc_names[m];i++)printf("%d ",ntrials_tc_names[m][i]);printf("\n");
        }
    printf("maxtrial_tc_names=");for(m=0;m<num_tc_names;m++)printf("%d ",maxtrial_tc_names[m]);printf("\n");
//STARTHERE
    }
if(!(temp_float=(float*)malloc(sizeof*temp_float*ap->vol))) {
    printf("fidlError: fidl_tc_ss Unable to malloc temp_float\n");
    exit(-1);
    }

//START151118
if(!(ntrials_cond1=d3int(num_wfiles,num_regions,tc_length_sum))) exit(-1);
if(lcmeanTR||lcsdTR){
    if(!(tc_sum=d3double(num_wfiles,num_regions,tc_length_sum)))exit(-1);
    if(!(tc_sum2=d3double(num_wfiles,num_regions,tc_length_sum)))exit(-1);
    if(!(ncond1=d3int(num_wfiles,num_regions,tc_length_sum)))exit(-1);
    if(lcmeanTR)if(!(meanTR=d3double(num_wfiles,num_tc_names,num_regions*tc_length_max)))exit(-1);
    if(lcsdTR)if(!(sdTR=d3double(num_wfiles,num_tc_names,num_regions*tc_length_max)))exit(-1);
    }


for(k=jj=r1=l=0;l<fs->nframes;) {
    for(m=0;m<(int)tc_files->nfiles;m++) {
        if(fbf->num_regions_by_file[m]) {

            ptr p0; 
            if(!p0.ptr0(tc_files->files[m]))exit(-1);

            printf("Processing %s\n",tc_files->files[m]);
            for(n=q1=0,i=0;i<fbf->num_regions_by_file[m]&&fbf->file_index[l]==m;i++,l++) {
                if(!fs->frames_per_line[k]) {
                    std::cout<<"fidlError: Zero frames detected for a "<<tc_names->files[frames_cond[k]]<<" event. Could be a mistake in your event file."<<std::endl;
                    std::cout<<"fidlError: Please see line "<<k+1<<" in "<<frames_file<<std::endl;
                    k++;
                    }
                else {
                    if(!n) {
                        for(q=r=0;frames_cond[k]!=tc_cond[q];r+=tc_length[q++]*placing[tcibyf[k]]);
                        for(q=0;q<offset[k];q++)r++;
                        }
                    if(!q1) for(j=0;j<dp->vol[m];j++) temp_double[j] = 0.;
                    p = dp->vol[m]*fbf->roi_by_file[m][i];

                    //printf("dp->vol[%d]=%d fbf->roi_by_file[%d][%d]=%d p=%d\n",m,dp->vol[m],m,i,fbf->roi_by_file[m][i],p);

                    #if 0
                    for(j=0;j<dp->vol[m];j++) temp_float[j] = mm->ptr[p+j];
                    if(dp->swapbytes[m]){
                        printf("Swapping bytes\n");
                        swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)dp->vol[m]);
                        }
                    #endif
                    #if 0
                    //START190920
                    if(dp->filetypeall==(int)IMG){
                        for(j=0;j<dp->vol[m];j++) temp_float[j] = mm->ptr[p+j];
                        if(dp->swapbytes[m])swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)dp->vol[m]);
                        }
                    else{
                        if(!nifti_getvol(niftiPtr,(int64_t)fbf->roi_by_file[m][i],temp_float))exit(-1);
                        }
                    #endif
                    //START190926
                    if(!p0.ptrstack(temp_float,fbf->roi_by_file[m][i]))exit(-1);


                    //for(j=0;j<dp->vol[m];j++)printf("%f ",temp_float[j]);printf("\n");fflush(stdout);exit(-1);
                    
                    
                    for(j=0;j<dp->vol[m];j++) {
                        if(temp_float[j]==(float)UNSAMPLED_VOXEL) {
                            temp_double[j] = (double)UNSAMPLED_VOXEL;
                            }
                        else if(temp_double[j]==(double)UNSAMPLED_VOXEL) {
                            /*do nothing*/
                            }
                        else {
                            temp_double[j] += (double)temp_float[j];
                            }
                        }
                    rr=!nplacing?r+n:r;
                    if(lcmeanTR||lcsdTR){
                        for(j=0;j<dp->vol[m];j++)
                            temp_double2[j]=temp_float[j]!=(float)UNSAMPLED_VOXEL?(double)temp_float[j]:(double)UNSAMPLED_VOXEL;
                        if(xform_file){
                            if(!t4_atlas(temp_double2,stat,t4,dp->xdim[m],dp->ydim[m],dp->zdim[m],dp->dx[m],dp->dz[m],A_or_B_or_U,
                               dp->orientation[m],ap,(double*)NULL)) exit(-1);
                            for(j=0;j<ap->vol;j++) temp_double2[j] = stat[j];
                            }
                        for(s=p=0;p<num_wfiles;p++) {
                            if(num_region_files) {

                                //if(lc_voxels) for(j=0;j<lenbrain;j++,jj++) voxel_tc[jj] = temp_double2[brnidx[j]];
                                //START191205
                                if(lc_voxels) for(j=0;j<lenbrain;j++,jj++) voxel_tc[jj] = temp_double2[ms.brnidx[j]];

                                if(wfiles) for(j=0;j<ap->vol;j++) weights[j] = (double)mm_wfiles[p]->ptr[j];
                                crsw(temp_double2,stat,rbf,weights,rbf->indices);

                                //for(j=0;j<num_regions;j++) temp_double2[j] = stat[j];
                                //START210205
                                dptr=stat; 

                                
                                }
                            else {

                                //if(dp->vol[m]!=lenbrain) for(j=0;j<lenbrain;j++) temp_double2[j]=temp_double2[ms.brnidx[j]];
                                //START210205
                                dptr=temp_double2; 

                                }

                            #if 0
                            for(j=0;j<num_regions;j++)if(temp_double2[j]!=(double)UNSAMPLED_VOXEL){
                                tc_sum[p][j][rr]+=temp_double2[j];
                                tc_sum2[p][j][rr]+=temp_double2[j]*temp_double2[j];
                                ncond1[p][j][rr]++;
                                }
                            #endif
                            //START210205
                            for(j=0;j<ms1->lenbrain;j++)if(fabs(dptr[ms1->brnidx[j]])>(double)UNSAMPLED_VOXEL){
                                tc_sum[p][j][rr]+=dptr[ms1->brnidx[j]];
                                tc_sum2[p][j][rr]+=dptr[ms1->brnidx[j]]*dptr[ms1->brnidx[j]];
                                ncond1[p][j][rr]++;
                                }

                            }



                        }


                    if(++q1 == fs->num_frames_to_sum[r1]) {
                        if(fs->num_frames_to_sum[r1]>1){
                            for(j=0;j<dp->vol[m];j++)
                                if(temp_double[j]!=(double)UNSAMPLED_VOXEL)temp_double[j]/=(double)fs->num_frames_to_sum[r1];
                            }
                        if(xform_file) {
                            if(!t4_atlas(temp_double,stat,t4,dp->xdim[m],dp->ydim[m],dp->zdim[m],dp->dx[m],dp->dz[m],A_or_B_or_U,
                                dp->orientation[m],ap,(double*)NULL)) exit(-1);
                            for(j=0;j<ap->vol;j++) temp_double[j] = stat[j];
                            }
                        for(s=p=0;p<num_wfiles;p++) {
                            if(num_region_files) {

                                //if(lc_voxels) for(j=0;j<lenbrain;j++,jj++) voxel_tc[jj] = temp_double[brnidx[j]];
                                //START191205
                                if(lc_voxels) for(j=0;j<lenbrain;j++,jj++) voxel_tc[jj] = temp_double[ms.brnidx[j]];

                                if(wfiles) for(j=0;j<ap->vol;j++) weights[j] = (double)mm_wfiles[p]->ptr[j];
                                crsw(temp_double,stat,rbf,weights,rbf->indices);
                                for(j=0;j<num_regions;j++) temp_double[j] = stat[j];
                                }
                            else {
                                //if(dp->vol[m]!=lenbrain) for(j=0;j<lenbrain;j++) temp_double[j] = temp_double[brnidx[j]];
                                //START191205
                                if(dp->vol[m]!=lenbrain) for(j=0;j<lenbrain;j++) temp_double[j] = temp_double[ms.brnidx[j]];
                                }
                            for(j=0;j<num_regions;j++)if(temp_double[j]!=(double)UNSAMPLED_VOXEL){
                                timecourses_sum[p][j][rr]+=temp_double[j];
                                timecourses_sum2[p][j][rr]+=temp_double[j]*temp_double[j];
                                ntrials_cond1[p][j][rr]++;
                                }
                            if(lctrials) for(j=0;j<num_regions;j++,s++) timecourses[s][k][n] = temp_double[j];
                            }
                        q1 = 0; r1++;
                        if(nplacing) r += placing[tcibyf[k]];
                        if(++n == fs->frames_per_line[k]) {
                            n = 0;
                            k++;
                            }
                        }
                    }
                }

            #if 0
            //if(!unmap_disk(mm)) exit(-1);
            //START190920
            if(dp->filetypeall==(int)IMG)if(!unmap_disk(mm))exit(-1);
            #endif
            //START190926
            if(!p0.ptrfree())exit(-1);

            }
        }
   } 

/*for(p=0;p<num_wfiles;p++) {
    for(k=m=0;m<num_tc_names;m++) {
        for(i=0;i<tc_length[m];i++,k++) {
            for(j=0;j<num_regions;j++) printf("%f ",timecourses_sum[p][j][k]);
            printf("\n");
            }
        }
    }*/

//START151118
printf("ntrials_cond=");for(i=0;i<tc_length_sum;i++)printf("%d ",ntrials_cond[i]);printf("\n");
//printf("ntrials_cond1[0][0]=");for(i=0;i<tc_length_sum;i++)printf("%d ",ntrials_cond1[0][0][i]);printf("\n");

if(!lctrials_only) {

    //std::cout<<"here0"<<std::endl;

    #if 0
    for(p=0;p<num_wfiles;p++) {
        for(k=m=0;m<num_tc_cond;m++) {
            for(i=0;i<tc_length[m]*placing[m];i++,k++) {
                for(j=0;j<num_regions;j++) {
                    timecourses_sum2[p][j][k] =
                        (timecourses_sum2[p][j][k]-timecourses_sum[p][j][k]*timecourses_sum[p][j][k]/ntrials_cond[k])/
                        (ntrials_cond[k]-1);
                    }
                }
            }
        }
    #endif
    //START151119
    for(p=0;p<num_wfiles;p++) {
        for(k=m=0;m<num_tc_cond;m++) {
            for(i=0;i<tc_length[m]*placing[m];i++,k++) {
                for(j=0;j<num_regions;j++) {

                    #if 1
                    var_cond[p][j][k] =
                        (timecourses_sum2[p][j][k]-timecourses_sum[p][j][k]*timecourses_sum[p][j][k]/ntrials_cond1[p][j][k])/
                        (ntrials_cond1[p][j][k]-1.);
                    #endif
                    #if 0
                    KEEP Will eventually go this when we have a good example, and the NaN produced above will catch that good example.
                    //START151209
                    var_cond[p][j][k]=ntrials_cond1[p][j][k]==1?(double)UNSAMPLED_VOXEL:
                        (timecourses_sum2[p][j][k]-timecourses_sum[p][j][k]*timecourses_sum[p][j][k]/ntrials_cond1[p][j][k])/
                        (ntrials_cond1[p][j][k]-1.);
                    #endif

                    }
                }
            }
        }

    //std::cout<<"here1"<<std::endl;


    for(p=0;p<num_wfiles;p++) {
        for(j=0;j<num_regions;j++) {

            //std::cout<<"here2"<<std::endl;

            for(m=0;m<num_tc_names;m++) {
 
                //START151217
                if(!maxtrial_tc_names[m])continue;
 
                //for(l=0;l<tc_length_max;l++) temp_double[l]=stat[l]=sem1[l]=sem2[l]=sd1[l]=0.;
                //START151119
                for(l=0;l<tc_length_max;l++) temp_double[l]=temp_double2[l]=stat[l]=sem1[l]=sem2[l]=sd1[l]=sd2[l]=0.;

                for(l=0;l<tcs->num_tc_to_sum[m][0];l++) {

                    #if 0
                    for(q=r=0;(int)tcs->tc[m][0][l]!=tc_cond[q];r+=tc_length[q]*placing[q],q++);
                    s = tc_length[q]*placing[q];
                    for(i=0;i<s;i++) stat[i] += (double)ntrials_cond[r+i];
                    if(s>slen[m]) slen[m] = s;
                    #endif
                    #if 0
                    //START151215
                    for(q=r=0;((int)tcs->tc[m][0][l]!=tc_cond[q])&&(q<num_tc_cond);r+=tc_length[q]*placing[q],q++);
                    s = tc_length[q]*placing[q];
                    for(i=0;i<s;i++) stat[i] += (double)ntrials_cond1[p][j][r+i];
                    if(s>slen[m]) slen[m] = s;
                    #endif
                    #if 0
                    //START151217
                    for(q=r=0;((int)tcs->tc[m][0][l]!=tc_cond[q])&&(q<num_tc_cond);q++)r+=tc_length[q]*placing[q];
                    for(s=tc_length[q]*placing[q],i=0;i<s;i++) stat[i] += (double)ntrials_cond1[p][j][r+i];
                    if(s>slen[m]) slen[m] = s;
                    #endif
                    //START151218
                    for(i=0;i<sss[m][l];i++)stat[i]+=(double)ntrials_cond1[p][j][rrr[m][l]+i];
                  

                    }

                #if 0
                for(l=0;l<tcs->num_tc_to_sum[m][0];l++) {

                    //for(q=r=0;(int)tcs->tc[m][0][l]!=tc_cond[q];r+=tc_length[q]*placing[q],q++);
                    //START151215
                    for(q=r=0;((int)tcs->tc[m][0][l]!=tc_cond[q])&&(q<num_tc_cond);r+=tc_length[q]*placing[q],q++);

                    s = tc_length[q]*placing[q];
                    for(i=0;i<s;i++) {
                        temp_double[i] += timecourses_sum[p][j][r+i];

                        #if 0
                        w=(double)ntrials_cond[r+i]/stat[i];
                        sem1[i] += (double)ntrials_cond[r+i]*w*w*timecourses_sum2[p][j][r+i];
                        sem2[i] += (double)ntrials_cond[r+i]*w;
                        #endif
                        //START151118
                        #if 0
                        w=(double)ntrials_cond1[p][j][r+i]/stat[i];
                        temp_double2[i] += timecourses_sum[p][j][r+i]*w;
                        sem1[i] += (double)ntrials_cond1[p][j][r+i]*w*w*timecourses_sum2[p][j][r+i];
                        sem2[i] += (double)ntrials_cond1[p][j][r+i]*w;

                        sd1[i] += w*w*timecourses_sum2[p][j][r+i];
                        #endif
                        //START151119
                        w=(double)ntrials_cond1[p][j][r+i]/stat[i];
                        temp_double2[i] += timecourses_sum[p][j][r+i]*w;
                        sem1[i] += (double)ntrials_cond1[p][j][r+i]*w*w*var_cond[p][j][r+i];
                        sem2[i] += (double)ntrials_cond1[p][j][r+i]*w;
                        sd1[i] += w*w*var_cond[p][j][r+i];
                        sd2[i] += timecourses_sum2[p][j][r+i];

                        }
                    }
                #endif
                //START151218
                for(l=0;l<tcs->num_tc_to_sum[m][0];l++) {
                    for(i=0;i<sss[m][l];i++) {
                        temp_double[i] += timecourses_sum[p][j][rrr[m][l]+i];
                        w=(double)ntrials_cond1[p][j][rrr[m][l]+i]/stat[i];
                        temp_double2[i]+=timecourses_sum[p][j][rrr[m][l]+i]*w;
                        sem1[i]+=(double)ntrials_cond1[p][j][rrr[m][l]+i]*w*w*var_cond[p][j][rrr[m][l]+i];
                        sem2[i]+=(double)ntrials_cond1[p][j][rrr[m][l]+i]*w;
                        sd1[i]+=w*w*var_cond[p][j][rrr[m][l]+i];
                        sd2[i]+=timecourses_sum2[p][j][rrr[m][l]+i];
                        }
                    }





                for(l=0;l<slen[m];l++) {

                    #if 0
                    mean[p][m][l*num_regions+j] = temp_double[l]/stat[l];
                    sem[p][m][l*num_regions+j] = sqrt(sem1[l]/pow(sem2[l],2.));
                    sd[p][m][l*num_regions+j] = sqrt(sd1[l]);
                    var[p][m][l*num_regions+j] = sd1[l];
                    #endif
                    //START151119
                    mean[p][m][l*num_regions+j] = temp_double[l]/stat[l];
                    sem[p][m][l*num_regions+j] = sqrt((sd2[l]-temp_double[l]*temp_double[l]/stat[l])/((stat[l]-1.)*stat[l])); 
                    wmean[p][m][l*num_regions+j] = temp_double2[l];
                    wsem[p][m][l*num_regions+j] = sqrt(sem1[l]/pow(sem2[l],2.));
                    sd[p][m][l*num_regions+j] = sqrt(sd1[l]);
                    var[p][m][l*num_regions+j] = sd1[l];

                    }
                }
            }
        }
    //printf("slen=");for(m=0;m<num_tc_names;m++)printf("%d ",slen[m]);printf("\n");

    if(lcmeanTR||lcsdTR){
        for(p=0;p<num_wfiles;p++) {
            for(j=0;j<num_regions;j++) {

                if(num_region_files)if(!rbf->nvoxels_region[j])continue;

                for(m=0;m<num_tc_names;m++) {
                    if(!maxtrial_tc_names[m])continue;
                    for(l=0;l<tc_length_max;l++)temp_double[l]=stat[l]=sd1[l]=0.;
                    for(l=0;l<tcs->num_tc_to_sum[m][0];l++) {
                        for(i=0;i<sss[m][l];i++)stat[i]+=(double)ncond1[p][j][rrr[m][l]+i];
                        }
                    for(l=0;l<tcs->num_tc_to_sum[m][0];l++) {
                        for(i=0;i<sss[m][l];i++) {
                            temp_double[i]+=tc_sum[p][j][rrr[m][l]+i];
                            sd1[i]+=tc_sum2[p][j][rrr[m][l]+i];
                            }
                        }
                    for(l=0;l<slen[m];l++) {
                        if(lcmeanTR)meanTR[p][m][l*num_regions+j]=temp_double[l]/stat[l];
                        if(lcsdTR)sdTR[p][m][l*num_regions+j]=sqrt((sd1[l]-temp_double[l]*temp_double[l]/stat[l])/(stat[l]-1.));
                        }
                    }
                }
            }
        }
    }


//if(num_region_files) {
//START210207
if(num_region_files&&!lcniigzonly){

    if(lcmeanTR){
        std::string str(regional_name);
        str.erase(str.end()-4,str.end());

        //str.append("_meanTR.txt");
        //START191205
        //std::cout<<"t0._timestr()="<<t0._timestr()<<"END"<<std::endl;
        str.append("_meanTR");
        str.append(t0._timestr());
        str.append(".txt");


        if(!(fp = fopen_sub((char*)str.c_str(),"w"))) exit(-1);
        fprintf(fp,"TIMECOURSE FILES:\n");
        for(m=0;m<(int)tc_files->nfiles;m++) fprintf(fp,"    %s\n",tc_files->files[m]);
        fprintf(fp,"REGION FILES:\n");
        for(m=0;m<num_region_files;m++) fprintf(fp,"    %s\n",region_files->files[m]);
        fprintf(fp,"\n");
        for(p=0;p<num_wfiles;p++) {
            if(wfiles) fprintf(fp,"WEIGHT FILE: %s\n\n",wfiles->files[p]);
            for(j=0;j<num_regions;j++) {

                //fprintf(fp,"REGION : %s %d\n",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                //START191226
                //fprintf(fp,"REGION : %s %d vox %.1f mm3\n",rbf->region_names_ptr[j],rbf->nvoxels_region[j],(double)rbf->nvoxels_region[j]*mm3);
                //START200312
                fprintf(fp,"REGION: %s %d vox %.1f mm3\n",rbf->region_names_ptr[j],rbf->nvoxels_region[j],(double)rbf->nvoxels_region[j]*mm3);

                for(m=0;m<num_tc_names;m++) {
                    fprintf(fp,"%s%*.4f ",tc_names->files[m],(int)(tc_names->max_length+10-tc_names->strlen_files[m]),meanTR[p][m][j]);
                    for(l=1;l<slen[m];l++) fprintf(fp,"%10.4f ",meanTR[p][m][l*num_regions+j]);
                    fprintf(fp,"\n");
                    }
                fprintf(fp,"\n");
                }
            }
        fclose(fp);
        printf("meanTR written to %s\n",str.c_str());
        }
    if(lcsdTR){
        std::string str(regional_name);
        str.erase(str.end()-4,str.end());

        //str.append("_sdTR.txt");
        //START191205
        //std::cout<<"t0._timestr()="<<t0._timestr()<<"END"<<std::endl;
        str.append("_sdTR");
        str.append(t0._timestr());
        str.append(".txt");


        if(!(fp = fopen_sub((char*)str.c_str(),"w"))) exit(-1);
        fprintf(fp,"TIMECOURSE FILES:\n");
        for(m=0;m<(int)tc_files->nfiles;m++) fprintf(fp,"    %s\n",tc_files->files[m]);
        fprintf(fp,"REGION FILES:\n");
        for(m=0;m<num_region_files;m++) fprintf(fp,"    %s\n",region_files->files[m]);
        fprintf(fp,"\n");
        for(p=0;p<num_wfiles;p++) {
            if(wfiles) fprintf(fp,"WEIGHT FILE: %s\n\n",wfiles->files[p]);
            for(j=0;j<num_regions;j++) {

                //fprintf(fp,"REGION : %s %d\n",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                //START191226
                fprintf(fp,"REGION: %s %d vox %.1f mm3\n",rbf->region_names_ptr[j],rbf->nvoxels_region[j],(double)rbf->nvoxels_region[j]*mm3);

                for(m=0;m<num_tc_names;m++) {
                    fprintf(fp,"%s%*.4f ",tc_names->files[m],(int)(tc_names->max_length+10-tc_names->strlen_files[m]),sdTR[p][m][j]);
                    for(l=1;l<slen[m];l++) fprintf(fp,"%10.4f ",sdTR[p][m][l*num_regions+j]);
                    fprintf(fp,"\n");
                    }
                fprintf(fp,"\n");
                }
            }
        fclose(fp);
        printf("sdTR written to %s\n",str.c_str());
        }




    //if(!lctrials_only) {
    //START191015
    if(!lctrials_only&&lcmean){

        if(!(fp = fopen_sub(regional_name,"w"))) exit(-1);
        fprintf(fp,"TIMECOURSE FILES:\n");
        for(m=0;m<(int)tc_files->nfiles;m++) fprintf(fp,"    %s\n",tc_files->files[m]);
        fprintf(fp,"REGION FILES:\n");
        for(m=0;m<num_region_files;m++) fprintf(fp,"    %s\n",region_files->files[m]);
        fprintf(fp,"\n");
        for(p=0;p<num_wfiles;p++) {
            if(wfiles) fprintf(fp,"WEIGHT FILE: %s\n\n",wfiles->files[p]);
            for(j=0;j<num_regions;j++) {
                fprintf(fp,"REGION : %s %d\n",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                for(m=0;m<num_tc_names;m++) {
                    fprintf(fp,"%s%*.4f ",tc_names->files[m],(int)(tc_names->max_length+10-tc_names->strlen_files[m]),mean[p][m][j]);
                    for(l=1;l<slen[m];l++) fprintf(fp,"%10.4f ",mean[p][m][l*num_regions+j]);
                    fprintf(fp,"\n");
                    fprintf(fp,"sem%*.4f ",(int)tc_names->max_length+10-4,sem[p][m][j]);
                    for(l=1;l<slen[m];l++) fprintf(fp,"%10.4f ",sem[p][m][l*num_regions+j]);
                    fprintf(fp,"\n");
                    if(tcs->num_tc_to_sum[m][0]>1){
                        fprintf(fp,"wmean%*.4f ",(int)(tc_names->max_length+10-6),wmean[p][m][j]);
                        for(l=1;l<slen[m];l++) fprintf(fp,"%10.4f ",wmean[p][m][l*num_regions+j]);
                        fprintf(fp,"\n");
                        fprintf(fp,"sem%*.4f ",(int)tc_names->max_length+10-4,wsem[p][m][j]);
                        for(l=1;l<slen[m];l++) fprintf(fp,"%10.4f ",wsem[p][m][l*num_regions+j]);
                        fprintf(fp,"\n");
                        }

                    }
                fprintf(fp,"\n");
                }
            }
        fclose(fp);
        printf("Timecourses written to %s\n",regional_name);
        }
    if(lctrials) {
        for(s=p=0;p<num_wfiles;p++) {
            for(j=q=0;q<num_region_files;q++) {
                for(r=0;r<rbf->num_regions_by_file[q];r++,j++,s++) {
                    sprintf(filename,"%s_trialstc_",rootptr);
                    if(wfiles) {
                        strcpy(write_back,wfiles->files[p]);
                        if(!(strptr=get_tail_sans_ext(write_back))) exit(-1);
                        strcat(filename,strptr);
                        strcat(filename,"_");
                        }
                    strcat(filename,rbf->region_names_ptr[j]);
                    strcat(filename,".txt");
                    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
                    fprintf(fp,"REGION FILE: %s\n",region_files->files[q]);
                    if(wfiles) fprintf(fp,"WEIGHT FILE: %s\n",wfiles->files[p]);
                    if(event_file) fprintf(fp,"EVENT FILE: %s\n",event_file);
                    if(xform_file) fprintf(fp,"T4 FILE: %s\n",xform_file);
                    fprintf(fp,"\n");
                    fprintf(fp,"REGION : %s %d\n",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                    if(tc_names_cond) for(m=0;m<num_tc_cond;m++) fprintf(fp,"%-3d %s\n",tc_cond[m],tc_names_cond[m]);
                    fprintf(fp,"\n");
                    if(!nplacing) {
                        for(m=0;m<fs->nlines;m++) {
                            fprintf(fp,"%-3d ",frames_cond[m]);
                            for(i=0;i<fs->frames_per_line[m]-1;i++) fprintf(fp,"%10.4f ",timecourses[s][m][i]);
                            fprintf(fp,"%10.4f\n",timecourses[s][m][i]);
                            }
                        }
                    else {
                        for(m=0;m<fs->nlines;m++) {
                            fprintf(fp,"%-3d ",frames_cond[m]);
                            for(i=l=0;l<fs->frames_per_line[m];l++) {
                                for(k=0;k<offset[m];k++) fprintf(fp,"%10.4f ",0.);
                                fprintf(fp,"%10.4f ",timecourses[s][m][i++]);
                                for(k=offset[m]+1;k<placing[tcibyf[m]];k++) fprintf(fp,"%10.4f ",0.);
                                }
                            fprintf(fp,"\n");
                            }
                        }
                    fclose(fp);
                    printf("Timecourses written to %s\n",filename);
                    }
                }
            }
        }

    if(lc_voxels) {
        if(!(coor=(double*)malloc(sizeof*coor*rbf->nvoxels*3))) {
            printf("fidlError: Unable to malloc coor in fidl_tc_ss\n");
            exit(-1);
            }
        if(!(col=(double*)malloc(sizeof*col*rbf->nvoxels))) {
            printf("fidlError: Unable to malloc col in fidl_tc_ss\n");
            exit(-1);
            }
        if(!(row=(double*)malloc(sizeof*row*rbf->nvoxels))) {
            printf("fidlError: Unable to malloc row in fidl_tc_ss\n");
            exit(-1);
            }
        if(!(slice=(double*)malloc(sizeof*slice*rbf->nvoxels))) {
            printf("fidlError: Unable to malloc slice in fidl_tc_ss\n");
            exit(-1);
            }
        col_row_slice(rbf->nvoxels,!lccompressed?rbf->indices:indices,col,row,slice,ap_reg);
        get_atlas_coor(rbf->nvoxels,col,row,slice,(double)ap_reg->zdim,ap_reg->center,ap_reg->mmppix,coor);
        for(l=pp=j=q=0;q<num_region_files;q++) {
            for(r=0;r<rbf->num_regions_by_file[q];r++,j++,s++) {
                sprintf(filename,"%s_trialstc_%s_voxels.txt",rootptr,rbf->region_names_ptr[j]);
                if(!(fp=fopen_sub(filename,"w"))) exit(-1);
                fprintf(fp,"REGION FILE: %s\n",region_files->files[q]);
                if(event_file) fprintf(fp,"EVENT FILE: %s\n",event_file);
                if(xform_file) fprintf(fp,"T4 FILE: %s\n",xform_file);
                fprintf(fp,"\n");
                fprintf(fp,"REGION : %s %d\n",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                for(m=0;m<num_tc_cond;m++) fprintf(fp,"%-3d %s\n",tc_cond[m],tc_names_cond[m]);
                fprintf(fp,"\n");

                #if 0
                for(p=0;p<rbf->nvoxels_region[j];p++,pp++) {
                    fprintf(fp,"%.1f %.1f %.1f\n",coor[l++],coor[l++],coor[l++]);
                #endif
                //START151117
                for(p=0;p<rbf->nvoxels_region[j];p++,pp++,l+=3) {
                    fprintf(fp,"%.1f %.1f %.1f\n",coor[l],coor[l+1],coor[l+2]);

                    for(k=m=0;m<fs->nlines;m++) {
                        fprintf(fp,"%-3d",frames_cond[m]);
                        /*for(i=0;i<fs->frames_per_line[m]-1;i++,k+=rbf->nvoxels) fprintf(fp," %10.4f ",voxel_tc[pp+k]);*/
                        for(i=0;i<fs->frames_per_line[m];i++,k+=rbf->nvoxels) fprintf(fp," %10.4f ",voxel_tc[pp+k]);
                        fprintf(fp,"\n");
                        }
                    fprintf(fp,"\n");
                    }
                fclose(fp);
                printf("Timecourses written to %s\n",filename);
                }
            }
        }
    }



//else {
//START210205
if(!num_region_files||lcniigz){

    if(!(w1=write1_init()))exit(-1);
    w1->filetype=dp->filetypeall;
    if(!(h.header0(tc_files->files[0])))exit(-1);    
    if(dp->filetypeall==(int)IMG){
        ifh=h.getifh();
        if(xform_file||dp->volall==lenbrain) {
            ifh->dim1 = ap->xdim;
            ifh->dim2 = ap->ydim;
            ifh->dim3 = ap->zdim;
            ifh->voxel_size_1 = ap->voxel_size[0];
            ifh->voxel_size_2 = ap->voxel_size[1];
            ifh->voxel_size_3 = ap->voxel_size[2];
            }
        ifh->mmppix[0] = ap->mmppix[0];
        ifh->mmppix[1] = ap->mmppix[1];
        ifh->mmppix[2] = ap->mmppix[2];
        ifh->center[0] = ap->center[0];
        ifh->center[1] = ap->center[1];
        ifh->center[2] = ap->center[2];
        }        
    else if(dp->filetypeall==(int)NIFTI){

        #if 0
        h.assign(dims,center,mmppix);
        w1->dims=dims;w1->center=center;w1->mmppix=mmppix;
        #endif
        //START210412
        w1->dims=h.dim64;w1->center=h.centerf;w1->mmppix=h.mmppixf; 

        }        
    for(m=0;m<ap->vol;m++) temp_float[m]=0.;
    if(lcmean||lcsem||lcsd||lcvar||lcsdavg) {
        for(m=0;m<num_tc_names;m++) {
            if(lcmean){
                sprintf(filename,"%s_%s%s_mean%s",rootptr,tc_names->files[m],ap->str,Fileext[dp->filetypeall]);
                if(!maxtrial_tc_names[m]){
                    std::cout<<"fidlInfo: "<<filename<<" will not be printed because "<<(maxtrial_tc_names[m]==1?singletrial:zerotrial)<<std::endl;
                    }
                else{
                    if(w1->filetype==(int)NIFTI)w1->dims[3]=slen[m];
                    if(!(fp=(FILE*)open2(filename,w1)))exit(-1);
                    for(l=0;l<slen[m];l++) {
                        for(j=0;j<num_regions;j++) temp_float[ms.brnidx[j]] = (float)mean[0][m][l*num_regions+j];
                        if(dp->filetypeall==(int)IMG){
                            if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,0)) {
                                std::cout<<"fidlError: fidl_tc_ss Could not write to "<<filename<<std::endl;
                                exit(-1);
                                }
                            }
                        else if(dp->filetypeall==(int)NIFTI){
                            if(!nifti_putvol(fp,(int64_t)l,temp_float))exit(-1);
                            }

                        }
                    if(dp->filetypeall==(int)IMG){
                        fclose(fp);
                        min_and_max_init(&ifh->global_min,&ifh->global_max);
                        min_and_max_doublestack(meanTR[0][m],num_regions*slen[m],&ifh->global_min,&ifh->global_max);
                        ifh->dim4 = slen[m];
                        ifh->number_format = (int)FLOAT_IF;
                        ifh->bigendian = 0;
                        if(!write_ifh(filename,ifh,0)) exit(-1);
                        }
                    else if(dp->filetypeall==(int)NIFTI){
                        sprintf(write_back,"gzip -f  %s",filename);
                        if(system(write_back)==-1){
                            std::cout<<"fidlError: "<<write_back<<std::endl;
                            exit(-1);
                            }
                        else
                            strcat(filename,".gz");
                       }
                    std::cout<<"Mean timecourse written to "<<filename<<std::endl;
                    if(tcs->num_tc_to_sum[m][0]>1){
                        sprintf(filename,"%s_%s%s_wmean.4dfp.img",rootptr,tc_names->files[m],ap->str);
                        if(!(fp = fopen_sub(filename,"w"))) exit(-1);
                        for(l=0;l<slen[m];l++) {
                            for(j=0;j<num_regions;j++) temp_float[ms.brnidx[j]] = (float)wmean[0][m][l*num_regions+j];
                            if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,0)) {
                                std::cout<<"fidlError: Could not write to "<<filename<<std::endl;
                                exit(-1);
                                }
                            }
                        fclose(fp);
                        min_and_max_init(&ifh->global_min,&ifh->global_max);
                        min_and_max_doublestack(wmean[0][m],num_regions*slen[m],&ifh->global_min,&ifh->global_max);
                        ifh->dim4 = slen[m];
                        ifh->number_format = (int)FLOAT_IF;
                        ifh->bigendian = 0;
                        if(!write_ifh(filename,ifh,0)) exit(-1);
                        std::cout<<"Weighted mean timecourse written to "<<filename<<std::endl;
                        }
                    }
                }



            if(lcsem) {
                sprintf(filename,"%s_%s%s_sem.4dfp.img",rootptr,tc_names->files[m],ap->str);

                //if(!notonetrial_tc_names[m]){
                //    printf("%s will not be printed because there is only a single trial.\n",filename);
                //START151215
                if(maxtrial_tc_names[m]<=1){
                    printf("%s will not be printed because %s.\n",filename,maxtrial_tc_names[m]==1?singletrial:zerotrial);

                    }
                else{
                    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
                    for(l=0;l<slen[m];l++) {

                        //for(j=0;j<num_regions;j++) temp_float[brnidx[j]] = (float)sem[0][m][l*num_regions+j];
                        //START191205
                        for(j=0;j<num_regions;j++) temp_float[ms.brnidx[j]] = (float)sem[0][m][l*num_regions+j];

                        if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,0)) {
                            printf("Error: Could not write to %s\n",filename);
                            exit(-1);
                            }
                        }
                    fclose(fp);
                    min_and_max_init(&ifh->global_min,&ifh->global_max);
                    min_and_max_doublestack(sem[0][m],num_regions*slen[m],&ifh->global_min,&ifh->global_max);
                    ifh->dim4 = slen[m];
                    ifh->number_format = (int)FLOAT_IF;
                    ifh->bigendian = !SunOS_Linux ? 1 : 0;
                    if(!write_ifh(filename,ifh,0)) exit(-1);
                    printf("Standard error of the mean timecourse written to %s\n",filename);
                    }
                if(tcs->num_tc_to_sum[m][0]>1){
                    sprintf(filename,"%s_%s%s_wsem.4dfp.img",rootptr,tc_names->files[m],ap->str);
                    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
                    for(l=0;l<slen[m];l++) {

                        //for(j=0;j<num_regions;j++) temp_float[brnidx[j]] = (float)wsem[0][m][l*num_regions+j];
                        //START191205
                        for(j=0;j<num_regions;j++) temp_float[ms.brnidx[j]] = (float)wsem[0][m][l*num_regions+j];

                        if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,0)) {
                            printf("Error: Could not write to %s\n",filename);
                            exit(-1);
                            }
                        }
                    fclose(fp);
                    min_and_max_init(&ifh->global_min,&ifh->global_max);
                    min_and_max_doublestack(wsem[0][m],num_regions*slen[m],&ifh->global_min,&ifh->global_max);
                    ifh->dim4 = slen[m];
                    ifh->number_format = (int)FLOAT_IF;
                    ifh->bigendian = !SunOS_Linux ? 1 : 0;
                    if(!write_ifh(filename,ifh,0)) exit(-1);
                    printf("Standard error of the weighted mean timecourse written to %s\n",filename);
                    }
                } 



            if(lcsd) {
                sprintf(filename,"%s_%s%s_sd%s",rootptr,tc_names->files[m],ap->str,Fileext[dp->filetypeall]);
                if(!maxtrial_tc_names[m]){
                    std::cout<<"fidlInfo: "<<filename<<" will not be printed because "<<(maxtrial_tc_names[m]==1?singletrial:zerotrial)<<std::endl;
                    }
                else{
                    if(w1->filetype==(int)NIFTI)w1->dims[3]=slen[m];
                    if(!(fp=(FILE*)open2(filename,w1)))exit(-1);
                    for(l=0;l<slen[m];l++) {
                        for(j=0;j<num_regions;j++) temp_float[ms.brnidx[j]] = (float)sd[0][m][l*num_regions+j];
                        if(dp->filetypeall==(int)IMG){
                            if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,0)) {
                                std::cout<<"fidlError: fidl_tc_ss Could not write to "<<filename<<std::endl;
                                exit(-1);
                                }
                            }
                        else if(dp->filetypeall==(int)NIFTI){
                            if(!nifti_putvol(fp,(int64_t)l,temp_float))exit(-1);
                            }
                        }
                    if(dp->filetypeall==(int)IMG){
                        fclose(fp);
                        min_and_max_init(&ifh->global_min,&ifh->global_max);
                        min_and_max_doublestack(sd[0][m],num_regions*slen[m],&ifh->global_min,&ifh->global_max);
                        ifh->dim4 = slen[m];
                        ifh->number_format = (int)FLOAT_IF;
                        ifh->bigendian = 0;
                        if(!write_ifh(filename,ifh,0)) exit(-1);
                        }
                    else if(dp->filetypeall==(int)NIFTI){
                        sprintf(write_back,"gzip -f  %s",filename);
                        if(system(write_back)==-1){
                            std::cout<<"fidlError: "<<write_back<<std::endl;
                            exit(-1);
                            }
                        else
                            strcat(filename,".gz");
                       }
                    std::cout<<"Standard deviation timecourse written to "<<filename<<std::endl;
                    }

                }
            if(lcvar) {
                sprintf(filename,"%s_%s%s_var.4dfp.img",rootptr,tc_names->files[m],ap->str);

                //if(!notonetrial_tc_names[m]){
                //    printf("%s will not be printed because there is only a single trial.\n",filename);
                //START151215
                if(maxtrial_tc_names[m]<=1){
                    printf("%s will not be printed because %s.\n",filename,maxtrial_tc_names[m]==1?singletrial:zerotrial);


                    }
                else{
                    if(!(fp = fopen_sub(filename,"w"))) exit(-1);
                        for(l=0;l<slen[m];l++) {

                        //for(j=0;j<num_regions;j++) temp_float[brnidx[j]] = (float)var[0][m][l*num_regions+j];
                        //START191205
                        for(j=0;j<num_regions;j++) temp_float[ms.brnidx[j]] = (float)var[0][m][l*num_regions+j];

                        if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,0)) {
                            printf("Error: Could not write to %s\n",filename);
                            exit(-1);
                            }
                        }
                    fclose(fp);
                    min_and_max_init(&ifh->global_min,&ifh->global_max);
                    min_and_max_doublestack(var[0][m],num_regions*slen[m],&ifh->global_min,&ifh->global_max);
                    ifh->dim4 = slen[m];
                    ifh->number_format = (int)FLOAT_IF;
                    ifh->bigendian = !SunOS_Linux ? 1 : 0;
                    if(!write_ifh(filename,ifh,0)) exit(-1);
                    printf("Variance of the %smean timecourse written to %s\n",tcs->num_tc_to_sum[m][0]>1?"weighted ":"",filename);
                    } 
                }
            if(lcsdavg) {
                sprintf(filename,"%s_%s%s_sdavg.4dfp.img",rootptr,tc_names->files[m],ap->str);
                    for(j=0;j<num_regions;j++) {
                    for(temp_double[j]=0.,l=0;l<slen[m];l++) temp_double[j] += sd[0][m][l*num_regions+j];
                    temp_double[j] /= (double)slen[m];

                    //temp_float[brnidx[j]] = (float)temp_double[j];
                    //START191205
                    temp_float[ms.brnidx[j]] = (float)temp_double[j];

                    }
                if(!writestack(filename,temp_float,sizeof(float),(size_t)ap->vol,0)) exit(-1);
                min_and_max_init(&ifh->global_min,&ifh->global_max);
                min_and_max_doublestack(temp_double,num_regions,&ifh->global_min,&ifh->global_max);
                ifh->dim4 = 1;
                ifh->number_format = (int)FLOAT_IF;
                ifh->bigendian = !SunOS_Linux ? 1 : 0;
                if(!write_ifh(filename,ifh,0)) exit(-1);
                printf("Average across frames of standard deviation of the %smean timecourse written to %s\n",
                    tcs->num_tc_to_sum[m][0]>1?"weighted ":"",filename);
                }
            }
        }
    if(lcpooledsd) {
        if(!(pooledsd=(double*)malloc(sizeof*pooledsd*num_tc_names*num_regions))) {
            printf("fidlError: Unable to malloc pooledsd\n");
            exit(-1);
            }
        for(j=0;j<num_tc_names*num_regions;j++) pooledsd[j]=0.;
        if(!(cpooledsd=(int*)malloc(sizeof*cpooledsd*num_tc_names))) {
            printf("fidlError: Unable to malloc cpooledsd\n");
            exit(-1);
            }
        for(j=0;j<num_tc_names;j++) cpooledsd[j]=0;
        for(j=0;j<fs->nlines;j++) {
            for(flag=m=0;m<num_tc_names;m++) {
                for(l=0;l<tcs->num_tc_to_sum[m][0];l++) if((int)tcs->tc[m][0][l]==frames_cond[j]) {flag=1;break;}
                if(flag==1) break;
                }
            if(!flag) {
                printf("fidlError: flag=%d  Cannot be zero.\n",flag);
                exit(-1);
                }
            if(fs->frames_per_line[j]>=minpooledsd[m]) {
                cpooledsd[m]++;
                for(n=m*num_regions,k=0;k<num_regions;k++,n++) {
                    for(sum=sum2=0.,i=0;i<fs->frames_per_line[j];i++) {
                        sum += timecourses[k][j][i];
                        sum2 += timecourses[k][j][i]*timecourses[k][j][i];
                        }


                    pooledsd[n] += (sum2-sum*sum/(double)fs->frames_per_line[j])/((double)fs->frames_per_line[j]-1.);

                    //printf("sum=%f sum2=%f fs->frames_per_line[%d]=%d pooledsd[%d]=%f\n",sum,sum2,j,fs->frames_per_line[j],n,
                    //    pooledsd[n]);
                    }
                }
            else {
                printf("pooledsd: Trial %d has %d frames, needs %d frames, excluded.\n",j+1,fs->frames_per_line[j],minpooledsd[m]);
                fflush(stdout);
                }
            }
        for(m=j=0;j<num_tc_names;j++) {
            sprintf(filename,"%s_%s%s_pooledsd.4dfp.img",rootptr,tc_names->files[j],ap->str);
            if(!maxtrial_tc_names[m]){
                printf("%s will not be printed because %s.\n",filename,maxtrial_tc_names[m]==1?singletrial:zerotrial);
                }
            else{

                for(k=0;k<num_regions;k++,m++) {
                    pooledsd[m]=sqrt(pooledsd[m]/(double)cpooledsd[j]);

                    //temp_float[brnidx[k]]=(float)pooledsd[m];
                    //START191205
                    temp_float[ms.brnidx[k]]=(float)pooledsd[m];

                    }
                if(!writestack(filename,temp_float,sizeof(float),(size_t)ap->vol,0)) exit(-1);
                min_and_max_init(&ifh->global_min,&ifh->global_max);
                min_and_max_doublestack(&pooledsd[j*num_regions],num_regions,&ifh->global_min,&ifh->global_max);
                ifh->dim4 = 1;
                ifh->number_format = (int)FLOAT_IF;
                ifh->bigendian = !SunOS_Linux ? 1 : 0;
                if(!write_ifh(filename,ifh,0)) exit(-1);
                printf("Pooledsd written to %s\n",filename);
                }

            }
        }               
    if(lctc) {
        for(m=0;m<num_tc_names;m++) {
            sprintf(filename,"%s_%s%s.4dfp.img",rootptr,tc_names->files[m],ap->str);
            if(!maxtrial_tc_names[m]){
                printf("%s will not be printed because %s.\n",filename,maxtrial_tc_names[m]==1?singletrial:zerotrial);
                }
            else{

                if(!(fp = fopen_sub(filename,"w"))) exit(-1);
                min_and_max_init(&ifh->global_min,&ifh->global_max);
                for(count=j=0;j<fs->nlines;j++) {
                    for(flag=l=0;l<tcs->num_tc_to_sum[m][0];l++) if((int)tcs->tc[m][0][l]==frames_cond[j]) {flag=1;break;}
                    if(flag==1) {
                        count+=fs->frames_per_line[j];
                        for(i=0;i<fs->frames_per_line[j];i++) {
                            for(k=0;k<num_regions;k++) {

                                #if 0
                                temp_float[brnidx[k]] = (float)timecourses[k][j][i];
                                if(temp_float[brnidx[k]]>ifh->global_max) ifh->global_max=temp_float[brnidx[k]];
                                else if(temp_float[brnidx[k]]<ifh->global_min) ifh->global_min=temp_float[brnidx[k]];
                                #endif
                                //START191205
                                temp_float[ms.brnidx[k]] = (float)timecourses[k][j][i];
                                if(temp_float[ms.brnidx[k]]>ifh->global_max) ifh->global_max=temp_float[ms.brnidx[k]];
                                else if(temp_float[ms.brnidx[k]]<ifh->global_min) ifh->global_min=temp_float[ms.brnidx[k]];

                                /*printf("timecourses[%d][%d][%d]=%f\n",k,j,i,timecourses[k][j][i]);*/
                                }
                            if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,0)) {
                                printf("Error: Could not write to %s\n",filename);
                                exit(-1);
                                }
                            }
                        }
                    }
                fclose(fp);
                ifh->dim4 = count;
                ifh->bigendian = !SunOS_Linux ? 1 : 0;
                ifh->number_format = (int)FLOAT_IF;
                if(!write_ifh(filename,ifh,0)) exit(-1);
                printf("Timecourses written to %s\n",filename);
                }


            }
        }

    //START210205
    if(w1->filetype==(int)NIFTI){
        if(num_region_files){w1->dims[0]=num_regions;w1->dims[1]=w1->dims[2]=1;}
        }

    if(lcmeanTR) {
        for(m=0;m<num_tc_names;m++) {
            sprintf(filename,"%s_%s%s_meanTR%s",rootptr,tc_names->files[m],ap->str,Fileext[dp->filetypeall]);
            if(!maxtrial_tc_names[m]){
                std::cout<<"fidlInfo: "<<filename<<" will not be printed because "<<(maxtrial_tc_names[m]==1?singletrial:zerotrial)<<std::endl;
                }
            else{
                if(w1->filetype==(int)NIFTI)w1->dims[3]=slen[m];     

                //if(!(fp=(FILE*)open2(filename,w1)))exit(-1);
                //START210206
                ptrw p0;
                if(!p0.ptrw0(filename,w1))exit(-1);

                for(l=0;l<slen[m];l++) {

                    #if 0
                    for(j=0;j<num_regions;j++) temp_float[ms.brnidx[j]] = (float)meanTR[0][m][l*num_regions+j];
                    if(dp->filetypeall==(int)IMG){
                        if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,0)){
                            std::cout<<"fidlError: fidl_tc_ss Could not write to "<<filename<<std::endl;
                            exit(-1);
                            }
                        }
                    else if(dp->filetypeall==(int)NIFTI){
                        if(!nifti_putvol(fp,(int64_t)l,temp_float))exit(-1);
                        }
                    #endif
                    //START210206
                    for(j=0;j<ms1->lenbrain;j++) temp_float[ms1->brnidx[j]] = (float)meanTR[0][m][l*ms1->lenbrain+j];
                    if(!p0.ptrwstack(temp_float,ap->vol,l))exit(-1);

                    }
                if(dp->filetypeall==(int)IMG){
                    min_and_max_init(&ifh->global_min,&ifh->global_max);
                    min_and_max_doublestack(meanTR[0][m],num_regions*slen[m],&ifh->global_min,&ifh->global_max);
                    ifh->dim4 = slen[m];
                    ifh->number_format = (int)FLOAT_IF;
                    ifh->bigendian = 0;
                    }
                if(!(strptr=p0.ptrwfree(ifh)))exit(-1);
                std::cout<<"Mean written to "<<strptr<<std::endl;

                }
            }
        }
    if(lcsdTR) {
        for(m=0;m<num_tc_names;m++) {
            sprintf(filename,"%s_%s%s_sdTR%s",rootptr,tc_names->files[m],ap->str,Fileext[dp->filetypeall]);
            if(!maxtrial_tc_names[m]){
                printf("%s will not be printed because %s.\n",filename,maxtrial_tc_names[m]==1?singletrial:zerotrial);
                }
            else{
                if(w1->filetype==(int)NIFTI)w1->dims[3]=slen[m];     

                //if(!(fp=(FILE*)open2(filename,w1)))exit(-1);
                //START210206
                ptrw p0;
                if(!p0.ptrw0(filename,w1))exit(-1);

                for(l=0;l<slen[m];l++) {

                    #if 0
                    //for(j=0;j<num_regions;j++) temp_float[brnidx[j]] = (float)sdTR[0][m][l*num_regions+j];
                    //START191205
                    //for(j=0;j<num_regions;j++) temp_float[ms.brnidx[j]] = (float)sdTR[0][m][l*num_regions+j];
                    //START210206
                    for(j=0;j<ms1->lenbrain;j++) temp_float[ms1->brnidx[j]] = (float)sdTR[0][m][l*ms1->lenbrain+j];
                    if(dp->filetypeall==(int)IMG){
                        if(!fwrite_sub(temp_float,sizeof(float),(size_t)ap->vol,fp,0)){
                            std::cout<<"fidlError: fidl_tc_ss Could not write to "<<filename<<std::endl;
                            exit(-1);
                            }
                        }
                    else if(dp->filetypeall==(int)NIFTI){
                        if(!nifti_putvol(fp,(int64_t)l,temp_float))exit(-1);
                        }
                    #endif
                    //START210206
                    for(j=0;j<ms1->lenbrain;j++) temp_float[ms1->brnidx[j]] = (float)sdTR[0][m][l*ms1->lenbrain+j];
                    if(!p0.ptrwstack(temp_float,ap->vol,l))exit(-1);

                    }
                if(dp->filetypeall==(int)IMG){
                    min_and_max_init(&ifh->global_min,&ifh->global_max);
                    min_and_max_doublestack(sdTR[0][m],num_regions*slen[m],&ifh->global_min,&ifh->global_max);
                    ifh->dim4 = slen[m];
                    ifh->number_format = (int)FLOAT_IF;
                    ifh->bigendian = 0;
                    }
                if(!(strptr=p0.ptrwfree(ifh)))exit(-1);
                std::cout<<"Standard deviation written to "<<strptr<<std::endl;

                #if 0
                //START211102     
                std::string str(strptr);
                std::string superbird=str.substr(0,str.find_last_of(".nii.gz")-str.find_last_of("/")-7)+"_LUT.txt"; 
                std::ofstream ofile(superbird); 
                ofile<<"#No. Label Name:                            R   G   B   A"<<std::endl;
                for(j=0;j<rbf->nreg;++j)ofile<<j<<" "<<rbf->region_names_ptr[j]<<" "<<"70  130 180 0"<<std::endl; 
                ofile.close();
                std::cout<<"Look up table written to "<<superbird<<std::endl;
                #endif
                //START220125
                if(rbf){                
                    std::string str(strptr);
                    std::string superbird=str.substr(0,str.find_last_of(".nii.gz")-str.find_last_of("/")-7)+"_LUT.txt";
                    std::ofstream ofile(superbird);
                    ofile<<"#No. Label Name:                            R   G   B   A"<<std::endl;
                    for(j=0;j<rbf->nreg;++j)ofile<<j<<" "<<rbf->region_names_ptr[j]<<" "<<"70  130 180 0"<<std::endl;
                    ofile.close();
                    std::cout<<"Look up table written to "<<superbird<<std::endl;
                    }
                
                }
            }
        }
    }

//START190920
write1_free(w1);


if(lccleanup){
    if(scratchdir) {
        sprintf(filename,"rm -r %s",scratchdir);
        if(system(filename) == -1) printf("fidlError: fidl_tc_ss Unable to %s\n",filename);
        }
    else {

        //delete_scratch_files(tc_files->files,tc_files->nfiles);
        //START2107
        for(size_t i=0;i<tc_files->nfiles;i++){
            std::string str(tc_files->files[i]);  
            std::string str2="rm -f "+str.substr(0,str.find_last_of("."))+"*";
            if(system(str2.c_str())==-1){
                std::cout<<"fidlError: "<<str2.c_str()<<std::endl;
                }
            }

        }
    }
}
