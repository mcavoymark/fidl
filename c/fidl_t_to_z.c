/* Copyright 10/12/07 Washington University.  All Rights Reserved.
   fidl_t_to_z.c  $Revision: 1.6 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <gsl/gsl_cdf.h>
int main(int argc,char **argv)
{
size_t i;
int SunOS_Linux=-1,vol,swapbytes,*index;
char *file=NULL,*strptr,filename[MAXNAME];
float *temp_float=NULL;
double *tstat,*zstat,*df;
Interfile_header *ifh=NULL;
Data *data=NULL;
FILE *fp;
if(argc < 3) {
    fprintf(stderr,"    -t:   *.4dfp.img or text file.\n");
    fprintf(stderr,"          For text files, the first column is the tstat and the second column is the df.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-t") && argc > i+1)
        file = argv[++i];
    }
if(!file) {
    printf("fidlError: Need to specify file with -t\n");
    exit(-1);
    }
if((strptr=strstr(file,".4dfp.img"))) {
    if((SunOS_Linux=checkOS())==-1) exit(-1);
    if(!(ifh=read_ifh(file,(Interfile_header*)NULL))) {
        printf("fidlError: Could not open %s\n",filename);
        exit(-1);
        }
    vol=ifh->dim1*ifh->dim2*ifh->dim3;
    if(!(temp_float=malloc(sizeof*temp_float*vol))) {
        printf("fidlError: Unable to malloc temp_float\n");
        exit(-1);
        }
    if(!readstack(file,(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    }
else {
    if(!(data=read_data(file,0,0,0,0))) exit(-1);
    vol = data->nsubjects;
    } 
if(!(tstat=malloc(sizeof*tstat*vol))) {
    printf("fidlError: Unable to malloc tstat\n");
    exit(-1);
    }
if(!(df=malloc(sizeof*df*vol))) {
    printf("fidlError: Unable to malloc df\n");
    exit(-1);
    }
if(!(zstat=malloc(sizeof*zstat*vol))) {
    printf("fidlError: Unable to malloc zstat\n");
    exit(-1);
    }
if(!(index=malloc(sizeof*index*vol))) {
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }
if(ifh) {
    if(!readstack(file,(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    for(i=0;i<vol;i++) {
        tstat[i]=(double)temp_float[i];
        df[i]=(double)ifh->df1;
        }
    }
else {
    for(i=0;i<data->nsubjects;i++) {
        tstat[i]=data->x[i][0];
        df[i]=(double)data->x[i][1];
        }
    }
t_to_z(tstat,zstat,vol,df,index);
if(ifh) {
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    min_and_max_doublestack(zstat,vol,&ifh->global_min,&ifh->global_max);
    for(i=0;i<vol;i++) temp_float[i]=(float)zstat[i];
    strcpy(filename,file);
    if((strptr=strstr(filename,"tstat")))
        *strptr = 'z';
    else {
        *(strrchr(filename,'.')-5)=0;
        strcat(filename,"_zstat.4dfp.img");
        }
    swapbytes = shouldiswap(SunOS_Linux,ifh->bigendian);
    if(!writestack(filename,temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
    if(!write_ifh(filename,ifh,0)) exit(-1);
    printf("Z statistics written to %s\n",filename);
    }
else {
    if(!(strptr=get_tail_sans_ext(file))) exit(-1);
    sprintf(filename,"%s_zstat.txt",strptr);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    fprintf(fp,"t\tdf\tz\tp(uncorrected,two sided)\n");
    for(i=0;i<data->nsubjects;i++) fprintf(fp,"%f\t%.2f\t%f\t%g\n",tstat[i],df[i],zstat[i],gsl_cdf_tdist_Q(tstat[i],df[i]));
    fclose(fp);
    printf("Output written to %s\n",filename);
    }
}
