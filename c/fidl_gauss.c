/* Copyright 8/22/02 Washington University.  All Rights Reserved.
   fidl_gauss.c  $Revision: 1.4 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "gauss_smoth2.h"
#include "dim_param2.h"
#include "subs_mask.h"
#include "minmax.h"
#include "subs_util.h"
int main(int argc,char **argv){
char fwhmstr[10],string[MAXNAME],write_back[MAXNAME],*strptr,*directory=NULL,*mask_file=NULL;
int i,j,k,m,length_directory,SunOS_Linux,nfile=0;
size_t i1;
float *temp_float;
double *temp_double,*stat,fwhm=0.;
Interfile_header *ifh;
Memory_Map *mm;
FILE *fp;
Dim_Param2 *dp;
Mask_Struct *ms;
Files_Struct *file=NULL;
gauss_smoth2_struct *gs=NULL;
if(argc < 5) {
    fprintf(stderr,"     -file:        conc or 4dfps.\n");
    fprintf(stderr,"     -gauss_smoth: Amount of smoothing to be done in units of voxels at fwhm with a 3D gaussian filter.\n");
    fprintf(stderr,"     -directory:   Specify directory for output.\n\n");
    fprintf(stderr,"     -mask:        Voxels outside the mask are set to zero after smoothing.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 

    #if 0
    if(!strcmp(argv[i],"-file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfile;
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")) {
            if(!(file=read_conc(1,&argv[i+1]))) exit(-1);
            }
        else if(!strcmp(strptr,".img")){
            if(!(file=get_files(nfile,&argv[i+1]))) exit(-1);
            }
        else {
            printf("Error: -file not conc or img. Abort!\n");fflush(stdout);
            exit(-1);
            }
        }
    #endif
    //START170313
    if(!strcmp(argv[i],"-file") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfile;
        if(!(file=read_files(nfile,&argv[i+1])))exit(-1);
        i+=nfile;
        }

    if(!strcmp(argv[i],"-gauss_smoth") && argc > i+1) {
        fwhm = atof(argv[++i]);
        sprintf(fwhmstr,"_fwhm%.1f",fwhm);
        }
    if(!strcmp(argv[i],"-directory") && argc > i+1) {
        length_directory = strlen(argv[i+1]);
        if(!(directory=malloc(sizeof*directory*(length_directory+1)))) {
            printf("Error: Unable to malloc directory\n");
            exit(-1);
            }
        strcpy(directory,argv[++i]);
        if(directory[length_directory-2] == '/') directory[length_directory-2] = 0;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        mask_file = argv[++i];
    }
if(!nfile) {
    printf("fidlError: Need to specify file to be smoothed with -file\n");
    exit(-1);
    }
if(!fwhm) {
    printf("fidlError: Need to specify smoothing with -gauss_smoth\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(dp=dim_param2(file->nfiles,file->files,SunOS_Linux)))exit(-1);
if(!dp->volall==-1){printf("fidlError: All files must have the same volume.\n");fflush(stdout);exit(-1);}
if(!(ms=get_mask_struct(mask_file,dp->volall,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp->volall))) exit(-1);
if(ms->lenvol != dp->volall) {
    printf("fidlError: Image files and  %s not in the same space.\n",mask_file);
    exit(-1);
    }
if(!(gs=gauss_smoth2_init(dp->xdim[0],dp->ydim[0],dp->zdim[0],fwhm,fwhm))) exit(-1);
if(!(temp_double=malloc(sizeof*temp_double*dp->volall))){
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(stat=malloc(sizeof*stat*dp->volall))){
    printf("fidlError: Unable to malloc stat\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*dp->volall))){
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i1=0;i1<file->nfiles;i1++) {
    string[0] = 0;
    if(directory) strcat(string,directory);
    strcpy(write_back,file->files[i1]);
    if(!(strptr=get_tail_sans_ext(write_back))) exit(-1);
    strcat(string,strptr);
    strcat(string,fwhmstr);
    strcat(string,".4dfp.img");
    if(!(ifh=read_ifh(file->files[i1],(Interfile_header*)NULL))) exit(-1);
    ifh->bigendian = SunOS_Linux ? 0:1;
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    if(!(mm=map_disk(file->files[i1],dp->vol[i1]*dp->tdim[i1],0,sizeof(float))))exit(-1);
    if(!(fp=fopen_sub(string,"w")))exit(-1);
    for(m=j=0;j<dp->tdim[i1];j++) {
        for(k=0;k<dp->vol[i1];k++,m++) temp_float[k] = mm->ptr[m];
        if(dp->swapbytes[i1]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)dp->vol[i1]);
        for(k=0;k<dp->vol[i1];k++)temp_double[k]=temp_float[k]==(float)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:(double)temp_float[k];
        gauss_smoth2(temp_double,stat,gs);
        for(k=0;k<dp->vol[i1];k++) temp_float[k] = 0.;
        for(k=0;k<ms->lenbrain;k++) temp_float[ms->brnidx[k]]=(float)stat[ms->brnidx[k]];
        if(!fwrite_sub(temp_float,sizeof*temp_float,(size_t)dp->vol[i1],fp,0))exit(-1);
        min_and_max_floatstack(temp_float,dp->vol[i1],&ifh->global_min,&ifh->global_max);
        }
    unmap_disk(mm);
    fclose(fp);
    if(!write_ifh(string,ifh,0)) exit(-1);
    free_ifh(ifh,0);
    printf("Output written to %s\n",string);
    }
}
