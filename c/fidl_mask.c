/* Copyright 3/12/13 Washington University.  All Rights Reserved.
   fidl_mask.c  $Revision: 1.5 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#include <fidl.h>
//START151005
#include "fidl.h"
#include "dim_param2.h"
#include "subs_util.h"
#include "minmax.h"
#include "subs_mask.h"

int main(int argc,char **argv)
{
char *file=NULL,*mask=NULL,*out=NULL,string[MAXNAME];
int i,j,SunOS_Linux,*idx,nidx,*regvox=NULL,nregvox,*regi=NULL;
size_t *len=NULL;
float *temp_float,*and;
//double *temp_double;

//Dim_Param *dp;
//START151005
Dim_Param2 *dp;

Interfile_header *ifh;
Mask_Struct *ms;
Regions *reg=NULL;
if(argc<5) {
    fprintf(stderr,"    -file: Image to be masked.\n");
    fprintf(stderr,"    -mask: Mask.\n");
    fprintf(stderr,"    -out:  Output image name.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-file") && argc > i+1)
        file = argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        mask = argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    }
if(!file) {
    printf("fidlError: Need to specify image to be masked -file\n");
    exit(-1);
    }
if(!mask) {
    printf("fidlError: Need to specify mask -mask\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

//if(!(dp=dim_param(1,&file,SunOS_Linux,1))) exit(-1);
//START151005
if(!(dp=dim_param2(1,&file,SunOS_Linux)))exit(-1);

if(!(ms=get_mask_struct(mask,0,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,0))) exit(-1);

#if 0
if(ms->lenvol!=dp->vol) {
    printf("fidlError: ms->lenvol=%d dp->vol=%d  Must be equal.\n",ms->lenvol,dp->vol); 
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*dp->vol))) {
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*dp->vol))) {
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }
#endif
//START151005
if(ms->lenvol!=dp->vol[0]) {
    printf("fidlError: ms->lenvol=%d dp->vol[0]=%d  Must be equal.\n",ms->lenvol,dp->vol[0]); 
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*dp->vol[0]))) {
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(and=malloc(sizeof*and*dp->vol[0]))) {
    printf("fidlError: Unable to malloc and\n");
    exit(-1);
    }

if(!(idx=malloc(sizeof*idx*ms->lenbrain))) {
    printf("fidlError: Unable to malloc idx\n");
    exit(-1);
    }
if(!(ifh=read_ifh(file,(Interfile_header*)NULL))) exit(-1);
if(ifh->nregions) {
    if(!(reg=extract_regions(file,0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) exit(-1);
    if(!(regvox=malloc(sizeof*regvox*reg->nregions))) {
        printf("fidlError: Unable to malloc regvox\n");
        exit(-1);
        }
    if(!(regi=malloc(sizeof*regi*reg->nregions))) {
        printf("fidlError: Unable to malloc regi\n");
        exit(-1);
        }
    if(!(len=malloc(sizeof*len*reg->nregions))) {
        printf("fidlError: Unable to malloc len\n");
        exit(-1);
        }
    }

#if 0
if(dp->number_format[0]==(int)FLOAT_IF) {
    if(!readstack(file,(float*)temp_float,sizeof(float),(size_t)dp->vol,SunOS_Linux)) exit(-1); 
    for(i=0;i<dp->vol;i++) temp_double[i] = temp_float[i]!=(float)UNSAMPLED_VOXEL?(double)temp_float[i]:(double)UNSAMPLED_VOXEL;
    }
else {
    if(!readstack(file,(double*)temp_double,sizeof(double),(size_t)dp->vol,SunOS_Linux)) exit(-1); 
    }
for(i=0;i<dp->vol;i++) temp_float[i]=0.;
for(nidx=i=0;i<ms->lenbrain;i++) if(fabs(temp_double[ms->brnidx[i]])>(double)UNSAMPLED_VOXEL) 
    {temp_float[ms->brnidx[i]]=(float)temp_double[ms->brnidx[i]];idx[nidx++]=ms->brnidx[i];}
min_and_max(temp_float,dp->vol,&ifh->global_min,&ifh->global_max);
#endif
//START151005
if(!readstack(file,(float*)temp_float,sizeof(float),(size_t)dp->vol[0],SunOS_Linux,dp->bigendian[0]))exit(-1); 
for(i=0;i<dp->vol[0];i++)and[i]=0.;
for(nidx=i=0;i<ms->lenbrain;i++)if(fabs(temp_float[ms->brnidx[i]])>(float)UNSAMPLED_VOXEL&&!isnan(temp_float[ms->brnidx[i]])) 
    {and[ms->brnidx[i]]=temp_float[ms->brnidx[i]];idx[nidx++]=ms->brnidx[i];}

if(ifh->nregions) {
    for(nregvox=0,i=2;i<=(int)ifh->global_max;i++) { 
        for(regi[nregvox]=i-2,regvox[nregvox]=0,j=0;j<nidx;j++) {

            //if((int)temp_float[idx[j]]==i) {
            //START151005
            if((int)and[idx[j]]==i) {

                regvox[nregvox]++;

                //temp_float[idx[j]]=(float)nregvox+2.;
                //START151005
                and[idx[j]]=(float)nregvox+2.;

                }
            }
        if(regvox[nregvox])nregvox++;
        }
    free_ifhregnames(ifh);
    for(i=0;i<nregvox;i++) {
        sprintf(string,"%d %s %d",i,reg->region_names[regi[i]],regvox[i]),
        len[i] = strlen(string) + 1;
        }
    if(!(ifh->region_names=d2charvar(nregvox,len))) exit(-1);
    ifh->nregions = nregvox;
    for(i=0;i<nregvox;i++) sprintf(ifh->region_names[i],"%d %s %d",i,reg->region_names[regi[i]],regvox[i]);
    }
if(!out) {
    if(!(get_tail_sans_ext(file))) exit(-1);
    sprintf(string,"%s_masked.4dfp.img",file);
    out=string;
    }
else if(!(strstr(out,".4dfp.img"))) {
    strcpy(string,out);
    out=strcat(string,".4dfp.img");
    }

//if(!writestack(out,temp_float,sizeof(float),(size_t)dp->vol,dp->swapbytes[0])) exit(-1);
//START151005
if(!writestack(out,and,sizeof(float),(size_t)dp->vol[0],dp->swapbytes[0])) exit(-1);
min_and_max_init(&ifh->global_min,&ifh->global_max);
min_and_max_floatstack(and,dp->vol[0],&ifh->global_min,&ifh->global_max);


if(!write_ifh(out,ifh,0)) exit(-1);
printf("Output written to %s\n",out);fflush(stdout);
}
