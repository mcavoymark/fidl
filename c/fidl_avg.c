/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   fidl_avg.c  $Revision: 1.9 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#include <fidl.h>
//START151029
#include "fidl.h"
#include "dim_param2.h"
#include "subs_util.h"
#include "minmax.h"

int main(int argc,char **argv)
{
char *avgname=NULL,*semname=NULL,*sumname=NULL,*molname=NULL,*ananame=NULL,*avgf=NULL,*semf=NULL,*sumf=NULL,*molf=NULL,*anaf=NULL;

//size_t i,j,k;
//START151029
int i,j,k;

int SunOS_Linux,nfiles=0,totalcount,countsummax,*tdim,tdim_max,*count;
float *temp_float,anamin,anamax,summin,summax,avgmin,avgmax,semmin,semmax,maxoverlap,molmax=0.;
double *temp_double,*sum,*sum2;

//Dim_Param *dp;
//START151029
Dim_Param2 *dp;

Files_Struct *files=NULL;
Interfile_header *ifh;
FILE *fp,*anaop=NULL,*sumop=NULL,*avgop=NULL,*semop=NULL,*molop=NULL;
if(argc < 4) {
    fprintf(stderr,"Usage: avg_4dfp -files $FILES -avg mean.4dfp.img -sd sd.4dfp.img\n");
    fprintf(stderr,"        -files:      List of files.\n");
    fprintf(stderr,"        -avg:        Filename for mean image.\n");
    fprintf(stderr,"        -sem:        Filename for standard error of the mean image.\n");
    fprintf(stderr,"        -sum:        Filename for summed image.\n");
    fprintf(stderr,"        -maxoverlap: Filename for percentage of voxels in region of maximum overlap.");
    fprintf(stderr,"        -anatave:    Average of first frames.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    if(!strcmp(argv[i],"-avg") && argc > i+1)
        avgname = argv[++i];
    if(!strcmp(argv[i],"-sem") && argc > i+1)
        semname = argv[++i];
    if(!strcmp(argv[i],"-sum") && argc > i+1)
        sumname = argv[++i];
    if(!strcmp(argv[i],"-maxoverlap") && argc > i+1)
        molname = argv[++i];
    if(!strcmp(argv[i],"-anatave"))
        ananame = argv[++i];
    }
if(!nfiles) {
    printf("fidlError: Need to specify -files\n");
    exit(-1);
    }
if(!avgname&&!semname&&!sumname&&!molname&&!ananame) {
    printf("fidlError: No output options specified.\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

//if(!(dp=dim_param(nfiles,files->files,SunOS_Linux,0))) exit(-1);
//START151029
if(!(dp=dim_param2(files->nfiles,files->files,SunOS_Linux)))exit(-1);

//if(!dp->all_tdim_same) {printf("fidlError: All files must have the same time dimension.\n");fflush(stdout);exit(-1);}
//START151029
if(!dp->tdimall==-1){printf("fidlError: All files must have the same time dimension.\n");fflush(stdout);exit(-1);}
if(!dp->volall==-1){printf("fidlError: All files must have the same volume.\n");fflush(stdout);exit(-1);}

//START151029
#if 0
for(i=0;i<files->nfiles;i++) {
     if(dp->number_format[i]!=(int)FLOAT_IF && dp->number_format[i]!=(int)DOUBLE_IF) {
        printf("fidlError: Need to add code to average this type of stack.\n");
        exit(-1);
        }
    }
#endif

if(ananame&&!sumname&&!avgname&&!semname&&!molname) {
    if(!(tdim=malloc(sizeof*tdim*files->nfiles))) {
        printf("Error: Unable to malloc tdim\n");
        exit(-1);
        }

    //for(i=0;i<files->nfiles;i++) tdim[i]=1;
    //START151029
    for(i=0;i<(int)files->nfiles;i++) tdim[i]=1;

    tdim_max=1;
    }
else {
    tdim=dp->tdim;
    tdim_max=dp->tdim_max;
    }

#if 0
if(!(temp_float=malloc(sizeof*temp_float*dp->vol))) {
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*dp->vol))) {
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(sum=malloc(sizeof*sum*dp->vol))) {
    printf("fidlError: Unable to malloc sum\n");
    exit(-1);
    }
if(!(sum2=malloc(sizeof*sum2*dp->vol))) {
    printf("fidlError: Unable to malloc sum2\n");
    exit(-1);
    }
if(!(count=malloc(sizeof*count*dp->vol))) {
    printf("fidlError: Unable to malloc count\n");
    exit(-1);
    }
#endif
//START151029
if(!(temp_float=malloc(sizeof*temp_float*dp->volall))) {
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*dp->volall))) {
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(sum=malloc(sizeof*sum*dp->volall))) {
    printf("fidlError: Unable to malloc sum\n");
    exit(-1);
    }
if(!(sum2=malloc(sizeof*sum2*dp->volall))) {
    printf("fidlError: Unable to malloc sum2\n");
    exit(-1);
    }
if(!(count=malloc(sizeof*count*dp->volall))) {
    printf("fidlError: Unable to malloc count\n");
    exit(-1);
    }


if(ananame) {
    
    //if(!(anaop=fopen_sub(ananame,"w"))) exit(-1);
    //START151103
    if(!strcmp(ananame+strlen(ananame)-9,".4dfp.img"))anaf=ananame;else{
        if(!(anaf=malloc(sizeof*anaf*(strlen(ananame)+10)))) {
            printf("fidlError: Unable to malloc anaf\n");
            exit(-1);
            }
        sprintf(anaf,"%s.4dfp.img",ananame);
        }
    if(!(anaop=fopen_sub(anaf,"w"))) exit(-1);

    min_and_max_init(&anamin,&anamax);
    }
if(sumname) {

    //if(!(sumop=fopen_sub(sumname,"w"))) exit(-1);
    //START151103
    if(!strcmp(sumname+strlen(sumname)-9,".4dfp.img"))sumf=sumname;else{
        if(!(sumf=malloc(sizeof*sumf*(strlen(sumname)+10)))) {
            printf("fidlError: Unable to malloc sumf\n");
            exit(-1);
            }
        sprintf(sumf,"%s.4dfp.img",sumname);
        }
    if(!(sumop=fopen_sub(sumf,"w"))) exit(-1);

    min_and_max_init(&summin,&summax);
    }
if(avgname) {

    //if(!(avgop=fopen_sub(avgname,"w"))) exit(-1);
    //START151103
    if(!strcmp(avgname+strlen(avgname)-9,".4dfp.img"))avgf=avgname;else{
        if(!(avgf=malloc(sizeof*avgf*(strlen(avgname)+10)))) {
            printf("fidlError: Unable to malloc avgf\n");
            exit(-1);
            }
        sprintf(avgf,"%s.4dfp.img",avgname);
        }
    if(!(avgop=fopen_sub(avgf,"w"))) exit(-1);

    min_and_max_init(&avgmin,&avgmax);
    }
if(semname) {

    //if(!(semop=fopen_sub(semname,"w"))) exit(-1);
    //START151103
    if(!strcmp(semname+strlen(semname)-9,".4dfp.img"))semf=semname;else{
        if(!(semf=malloc(sizeof*semf*(strlen(semname)+10)))) {
            printf("fidlError: Unable to malloc semf\n");
            exit(-1);
            }
        sprintf(semf,"%s.4dfp.img",semname);
        }
    if(!(semop=fopen_sub(semf,"w"))) exit(-1);

    min_and_max_init(&semmin,&semmax);
    }
if(molname) {

    //if(!(molop=fopen_sub(molname,"w"))) exit(-1);
    //START151103
    if(!strcmp(molname+strlen(molname)-9,".4dfp.img"))molf=molname;else{
        if(!(molf=malloc(sizeof*molf*(strlen(molname)+10)))) {
            printf("fidlError: Unable to malloc molf\n");
            exit(-1);
            }
        sprintf(molf,"%s.4dfp.img",molname);
        }
    if(!(molop=fopen_sub(molf,"w"))) exit(-1);

    }
for(i=0;i<tdim_max;i++) {

    //for(j=0;j<dp->vol;j++) {sum[j]=sum2[j]=0.;count[j]=0;}
    //START151029
    for(j=0;j<dp->volall;j++) {sum[j]=sum2[j]=0.;count[j]=0;}

    //for(j=0;j<files->nfiles;j++) {
    //START151029
    for(j=0;j<(int)files->nfiles;j++) {

        if(!(fp=fopen_sub(files->files[j],"r"))) exit(-1);

        #if 0
        if(fseek(fp,dp->byte[j]*dp->vol*i,(int)SEEK_SET)) {
            printf("fidlError: occured while seeking to %d in %s.\n",dp->byte[j]*dp->vol*i,files->files[j]);
            exit(-1);
            }
        if(!fread_sub(dp->number_format[j]==(int)FLOAT_IF?(void*)temp_float:(void*)temp_double,dp->byte[j],dp->vol,fp,
            dp->swapbytes[j],files->files[j])) exit(-1);
        if(dp->number_format[j]==(int)FLOAT_IF)
            for(k=0;k<dp->vol;k++) temp_double[k]=temp_float[k]!=(float)UNSAMPLED_VOXEL?(double)temp_float[k]:(double)UNSAMPLED_VOXEL;
        #endif
        //START151029
        if(fseek(fp,(long)(sizeof(float)*dp->vol[j]*i),(int)SEEK_SET)) {
            printf("fidlError: occured while seeking to %ld in %s.\n",(long)(sizeof(float)*dp->vol[j]*i),files->files[j]);
            exit(-1);
            }
        if(!fread_sub((void*)temp_float,sizeof(float),dp->vol[j],fp,dp->swapbytes[j],files->files[j]))exit(-1);
        for(k=0;k<dp->vol[j];k++) temp_double[k]=temp_float[k]!=(float)UNSAMPLED_VOXEL?(double)temp_float[k]:(double)UNSAMPLED_VOXEL;


        for(k=0;k<dp->vol[j];k++)
            if(fabs(temp_double[k])!=(double)UNSAMPLED_VOXEL && !isnan(temp_double[k])) {
                sum[k] += temp_double[k];
                sum2[k] += temp_double[k]*temp_double[k];
                count[k]++;
                }
        fclose(fp);
        }

    #if 0
    if(ananame&&!i) {
        for(j=0;j<dp->vol;j++) temp_double[j] = count[j] ? sum[j]/(double)count[j] : (double)UNSAMPLED_VOXEL;
        min_and_max_doublestack(temp_double,dp->vol,&anamin,&anamax);
        for(j=0;j<dp->vol;j++) temp_float[j]=(float)temp_double[j];
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)dp->vol,anaop,0)) {
            printf("fidlError: Could not write to %s\n",ananame);
            exit(-1);
            }
        }
    if(sumname) {
        min_and_max_doublestack(sum,dp->vol,&summin,&summax);
        for(j=0;j<dp->vol;j++) temp_float[j]=(float)sum[j];
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)dp->vol,sumop,0)) {
            printf("fidlError: Could not write to %s\n",sumname);
            exit(-1);
            }
        }
    if(avgname) {    
        for(j=0;j<dp->vol;j++) temp_double[j] = count[j] ? sum[j]/(double)count[j] : (double)UNSAMPLED_VOXEL;
        min_and_max_doublestack(temp_double,dp->vol,&avgmin,&avgmax);
        for(j=0;j<dp->vol;j++) temp_float[j]=(float)temp_double[j];
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)dp->vol,avgop,0)) {
            printf("fidlError: Could not write to %s\n",avgname);
            exit(-1);
            }
        }
    if(semname) {
        for(j=0;j<dp->vol;j++) temp_double[j] = count[j] ?
            sqrt((sum2[j]-sum[j]*sum[j]/(double)count[j])/((double)count[j]*((double)count[j]-1.))) : (double)UNSAMPLED_VOXEL;
        min_and_max_doublestack(temp_double,dp->vol,&semmin,&semmax);
        for(j=0;j<dp->vol;j++) temp_float[j]=(float)temp_double[j];
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)dp->vol,semop,0)) {
            printf("fidlError: Could not write to %s\n",semname);
            exit(-1);
            }
        }
    if(molname) {
        if(!sumname) min_and_max_doublestack(sum,dp->vol,&summin,&summax);
        for(totalcount=countsummax=j=0;j<dp->vol;j++) {
            if(count[j]) totalcount++;
            if(sum[j]==summax) countsummax++;
            }
        if((maxoverlap=(float)((double)countsummax/(double)totalcount*100.))>molmax) molmax=maxoverlap;
        for(j=0;j<dp->vol;j++) temp_float[j]=sum[j]==summax?maxoverlap:0.;
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)dp->vol,molop,0)) {
            printf("fidlError: Could not write to %s\n",molname);
            exit(-1);
            }
        }
    #endif
    //START151102
    if(ananame&&!i) {
        for(j=0;j<dp->volall;j++) temp_double[j] = count[j] ? sum[j]/(double)count[j] : (double)UNSAMPLED_VOXEL;
        min_and_max_doublestack(temp_double,dp->volall,&anamin,&anamax);
        for(j=0;j<dp->volall;j++) temp_float[j]=(float)temp_double[j];
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)dp->volall,anaop,0)) {
            printf("fidlError: Could not write to %s\n",ananame);
            exit(-1);
            }
        }
    if(sumname) {
        min_and_max_doublestack(sum,dp->volall,&summin,&summax);
        for(j=0;j<dp->volall;j++) temp_float[j]=(float)sum[j];
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)dp->volall,sumop,0)) {
            printf("fidlError: Could not write to %s\n",sumname);
            exit(-1);
            }
        }
    if(avgname) {
        for(j=0;j<dp->volall;j++) temp_double[j] = count[j] ? sum[j]/(double)count[j] : (double)UNSAMPLED_VOXEL;
        min_and_max_doublestack(temp_double,dp->volall,&avgmin,&avgmax);
        for(j=0;j<dp->volall;j++) temp_float[j]=(float)temp_double[j];
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)dp->volall,avgop,0)) {
            printf("fidlError: Could not write to %s\n",avgname);
            exit(-1);
            }
        }
    if(semname) {
        for(j=0;j<dp->volall;j++) temp_double[j] = count[j] ?
            sqrt((sum2[j]-sum[j]*sum[j]/(double)count[j])/((double)count[j]*((double)count[j]-1.))) : (double)UNSAMPLED_VOXEL;
        min_and_max_doublestack(temp_double,dp->volall,&semmin,&semmax);
        for(j=0;j<dp->volall;j++) temp_float[j]=(float)temp_double[j];
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)dp->volall,semop,0)) {
            printf("fidlError: Could not write to %s\n",semname);
            exit(-1);
            }
        }
    if(molname) {
        if(!sumname) min_and_max_doublestack(sum,dp->volall,&summin,&summax);
        for(totalcount=countsummax=j=0;j<dp->volall;j++) {
            if(count[j]) totalcount++;
            if(sum[j]==summax) countsummax++;
            }
        if((maxoverlap=(float)((double)countsummax/(double)totalcount*100.))>molmax) molmax=maxoverlap;
        for(j=0;j<dp->volall;j++) temp_float[j]=sum[j]==summax?maxoverlap:0.;
        if(!fwrite_sub(temp_float,sizeof(float),(size_t)dp->volall,molop,0)) {
            printf("fidlError: Could not write to %s\n",molname);
            exit(-1);
            }
        }
    }

//if(!(ifh=init_ifh(4,dp->xdim,dp->ydim,dp->zdim,tdim_max,dp->dxdy,dp->dxdy,dp->dz,SunOS_Linux?0:1))) exit(-1);
//START151102
if(!(ifh=read_ifh(files->files[0],(Interfile_header*)NULL))) exit(-1);
ifh->dim4=dp->tdim_max;
ifh->bigendian=SunOS_Linux?0:1;

if(ananame) {
    fclose(anaop);
    ifh->global_min=anamin;ifh->global_max=anamax;

    //if(!write_ifh(ananame,ifh,0)) exit(-1);
    //printf("Anatomy average written to %s\n",ananame);
    //START151103
    if(!write_ifh(anaf,ifh,0)) exit(-1);
    printf("Anatomy average written to %s\n",anaf);

    }
if(sumname) {
    fclose(sumop);
    ifh->global_min=summin;ifh->global_max=summax;

    //if(!write_ifh(sumname,ifh,0)) exit(-1);
    //printf("Sum written to %s\n",sumname);
    //START151103
    if(!write_ifh(sumf,ifh,0)) exit(-1);
    printf("Sum written to %s\n",sumf);

    }
if(avgname) {
    fclose(avgop);
    ifh->global_min=avgmin;ifh->global_max=avgmax;

    //if(!write_ifh(avgname,ifh,0)) exit(-1);
    //printf("Mean written to %s\n",avgname);
    //START151103
    if(!write_ifh(avgf,ifh,0)) exit(-1);
    printf("Mean written to %s\n",avgf);

    }
if(semname) {
    fclose(semop);
    ifh->global_min=semmin;ifh->global_max=semmax;

    //if(!write_ifh(semname,ifh,0)) exit(-1);
    //printf("SEM written to %s\n",semname);
    //START151103
    if(!write_ifh(semf,ifh,0)) exit(-1);
    printf("SEM written to %s\n",semf);

    }
if(molname) {
    fclose(molop);
    ifh->global_min=0.;
    ifh->global_max=molmax;

    //if(!write_ifh(molname,ifh,0)) exit(-1);
    //printf("Maxoverlap written to %s\n",molname);
    //START151103
    if(!write_ifh(molf,ifh,0)) exit(-1);
    printf("Maxoverlap written to %s\n",molf);

    }
}
