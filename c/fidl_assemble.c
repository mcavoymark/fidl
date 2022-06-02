/* Copyright 1/8/16 Washington University.  All Rights Reserved.
   fidl_assemble.c  $Revision: 1.3 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h"
#include "subs_mask.h"
#include "dim_param2.h"
#include "write1.h"
#include "filetype.h"
#include "minmax.h"
#include "subs_util.h"
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_assemble.c,v 1.3 2016/08/03 20:42:08 mcavoy Exp $";
int main(int argc,char **argv)
{
char *mask=NULL,*cleanupdir=NULL,*strptr,string[MAXNAME];
int i,j,k,nfiles=0,nroots=0,lccleanup=0,SunOS_Linux; //kk,off
size_t i1,j1;
float *temp_float;
Files_Struct *files=NULL,*roots=NULL;
Mask_Struct *ms=NULL;
Dim_Param2 *dp;
void *fp=NULL;
Interfile_header *ifh=NULL;
Memory_Map *mm=NULL;
W1 *w1=NULL;
if(argc<5){
    printf("fidl_assemble This program was written to reassemble a bunch of 1 frame files listed in concs.\n");
    printf("For example, We have 186 concs in -files and 72 -roots. Each conc lists 72 files.\n");
    printf("The output will be 72 roots each with 186 frames.\n");
    printf("The first root will include the first file in each conc. The second root will include the second file in each conc.\n");
    fprintf(stderr,"  -files:    Input images or concs.\n");
    fprintf(stderr,"  -roots:    Output roots.\n");
    fprintf(stderr,"  -mask:     If files are compressed, then files are uncompressed into the mask.\n");
    fprintf(stderr,"  -clean_up: Delete files. If a directory is listed, then the entire directory is deleted.\n");
    exit(-1);
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!nfiles){
            printf("fidlError: nfiles=%d Need to provide *.conc, *.img, *.nii or *.nii.gz after -files\n",nfiles);
            exit(-1);
            }
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")){
            if(!(files=read_conc(nfiles,&argv[i+1]))) exit(-1);
            }
        else if(!strcmp(strptr,".img")||!strcmp(strptr,".nii")||!strcmp(strptr-4,".nii.gz")){
            if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
            }
        else{
            printf("fidlError: -files not *.conc, *.img, *.nii or *.nii.gz. Abort!\n");fflush(stdout);exit(-1);
            }
        i+=nfiles;
        }
    if(!strcmp(argv[i],"-roots") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroots;
        if(!(roots=get_files(nroots,&argv[i+1])))exit(-1);
        i+=nroots;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]){
        mask=argv[++i];
        }
    if(!strcmp(argv[i],"-clean_up")){
        lccleanup=1;
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])cleanupdir=argv[++i];
        }
    }
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!nfiles) {
    printf("fidlError: No -files. Abort!\n");
    exit(-1);
    }
fflush(stdout);
if(!(dp=dim_param2(files->nfiles,files->files,SunOS_Linux))) exit(-1);
//printf("nfiles=%d files->nfiles=%zd dp->tdim_total=%d roots->nfiles=%zd\n",nfiles,files->nfiles,dp->tdim_total,roots->nfiles);
if(dp->volall==-1){printf("fidlError: fidl_assemble All files must have the same vol. Abort!\n");fflush(stdout);exit(-1);}
if(dp->tdimall==-1){printf("fidlError: fidl_assemble All files must have the same tdim. Abort!\n");fflush(stdout);exit(-1);}

#if 0
if(dp->tdimall!=(int)roots->nfiles){
    printf("fidlError: fidl_assemble dp->tdimall=%d roots->nfiles=%zd\n",dp->tdimall,roots->nfiles);fflush(stdout);exit(-1);}
#endif
//START160112
if(dp->tdim_total!=(int)files->nfiles){
    printf("fidlError: fidl_assemble dp->tdim_total=%d files->nfiles=%zd Must be equal\n",dp->tdim_total,files->nfiles);
    printf("fidlError: fidl_assemble This program was written to reassemble a bunch of 1 frame files listed in concs.\n");
    fflush(stdout);exit(-1);
    }

if(dp->filetypeall==-1){printf("fidlError: fidl_assemble All files must be a single type. Abort!\n");fflush(stdout);exit(-1);}
if(!(ms=get_mask_struct(mask,dp->volall,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp->volall)))exit(-1);
if(!(temp_float=malloc(sizeof*temp_float*ms->lenvol))){
    printf("fidlError: fidl_assemble Unable to malloc temp_float\n");fflush(stdout);exit(-1);}
for(i=0;i<ms->lenvol;i++)temp_float[i]=0.;
if(!(w1=malloc(sizeof*w1))){printf("fidlError: fidl_assemble Unable to malloc w1\n");fflush(stdout);exit(-1);}
w1->filetype=(int)IMG;

//if(!(ifh=init_ifh(4,ms->xdim,ms->ydim,ms->zdim,files->nfiles,ms->voxel_size_1,ms->voxel_size_2,ms->voxel_size_3,0)))exit(-1);
//START160112
if(!(ifh=init_ifh(4,ms->xdim,ms->ydim,ms->zdim,nfiles,ms->voxel_size_1,ms->voxel_size_2,ms->voxel_size_3,0)))exit(-1);

#if 0
for(off=i1=0;i1<roots->nfiles;i1++,off+=dp->volall){
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    sprintf(string,"%s.4dfp.img",roots->files[i1]);
    printf("Assembling %s\n",string);
    if(!(fp=open2(string,w1)))exit(-1);
    for(j1=0;j1<files->nfiles;j1++){
        if(!(mm=map_disk(files->files[j1],dp->vol[j1],0,sizeof(float))))exit(-1);
        min_and_max_floatstack(mm->ptr+off,dp->vol[j1],&ifh->global_min,&ifh->global_max);
        for(kk=off,k=0;k<ms->lenbrain;k++,kk++)temp_float[ms->brnidx[k]]=mm->ptr[kk];
        if(!unmap_disk(mm))exit(-1);
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)ms->lenvol,fp,0))exit(-1);
        }
    fclose(fp);
    if(!write_ifh(string,ifh,0))exit(-1);
    printf("Output written to %s\n",string);
    }
#endif
//START160112
//for(off=i1=0;i1<roots->nfiles;i1++,off+=dp->volall){
for(i1=0;i1<roots->nfiles;i1++){

    min_and_max_init(&ifh->global_min,&ifh->global_max);
    sprintf(string,"%s.4dfp.img",roots->files[i1]);
    printf("Assembling %s\n",string);
    if(!(fp=open2(string,w1)))exit(-1);

    //for(j1=0;j1<files->nfiles;j1++){
    for(j1=i1,j=0;j<nfiles;j++,j1+=roots->nfiles){

        if(!(mm=map_disk(files->files[j1],dp->vol[j1],0,sizeof(float))))exit(-1);

        //min_and_max_floatstack(mm->ptr+off,dp->vol[j1],&ifh->global_min,&ifh->global_max);
        min_and_max_floatstack(mm->ptr,dp->vol[j1],&ifh->global_min,&ifh->global_max);


        //for(kk=off,k=0;k<ms->lenbrain;k++,kk++)temp_float[ms->brnidx[k]]=mm->ptr[kk];
        for(k=0;k<ms->lenbrain;k++)temp_float[ms->brnidx[k]]=mm->ptr[k];

        if(!unmap_disk(mm))exit(-1);
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)ms->lenvol,fp,0))exit(-1);
        }
    fclose(fp);
    if(!write_ifh(string,ifh,0))exit(-1);
    printf("Output written to %s\n",string);
    }


if(lccleanup){
    if(cleanupdir) {
        sprintf(string,"rm -rf %s",cleanupdir);
        if(system(string)==-1)printf("fidlError: fidl_assemble Unable to %s\n",string);
        }
    else {
        delete_scratch_files(files->files,files->nfiles);
        }
    }
}
