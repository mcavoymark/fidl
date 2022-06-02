/* Copyright 12/27/10 Washington University.  All Rights Reserved.
   fidl_r_to_z.c  $Revision: 1.3 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
int main(int argc,char **argv)
{
int SunOS_Linux,vol,swapbytes,nsub=0,*idx;
size_t i;
char *file=NULL,*strptr,filename[MAXNAME];
float *temp_float;
double *r,*t,*z,*df;
Interfile_header *ifh;
if(argc < 5) {
    fprintf(stderr,"    -r:    4dfp correlation coefficient\n");
    fprintf(stderr,"    -nsub: number of subjects\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-r") && argc > i+1)
        file = argv[++i];
    if(!strcmp(argv[i],"-nsub") && argc > i+1)
        nsub = atoi(argv[++i]);
    }
if(!file) {printf("fidlError: Need to specify correlation coefficient with -r\n");fflush(stdout);exit(-1);}
if(!nsub) {printf("fidlError: Need to specify number of subjects -nsub\n");fflush(stdout);exit(-1);}
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(ifh=read_ifh(file,(Interfile_header*)NULL))) {printf("fidlError: Could not open %s\n",filename);fflush(stdout);exit(-1);}
vol=ifh->dim1*ifh->dim2*ifh->dim3*ifh->dim4;
if(!(temp_float=malloc(sizeof*temp_float*vol))) {printf("fidlError: Unable to malloc temp_float\n");fflush(stdout);exit(-1);}
if(!(r=malloc(sizeof*r*vol))) {printf("fidlError: Unable to malloc r\n");fflush(stdout);exit(-1);}
if(!(t=malloc(sizeof*t*vol))) {printf("fidlError: Unable to malloc t\n");fflush(stdout);exit(-1);}
if(!(z=malloc(sizeof*z*vol))) {printf("fidlError: Unable to malloc z\n");fflush(stdout);exit(-1);}
if(!(df=malloc(sizeof*df*vol))) {printf("fidlError: Unable to malloc df\n");fflush(stdout);exit(-1);}
if(!(idx=malloc(sizeof*idx*vol))) {printf("fidlError: Unable to malloc idx\n");fflush(stdout);exit(-1);}
if(!readstack(file,(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
for(i=0;i<vol;i++) {
    r[i] = (double)temp_float[i];
    df[i] = (double)(nsub-2);
    }
r_to_z(r,t,z,df,vol,idx);
for(i=0;i<vol;i++) temp_float[i] = (float)z[i];
ifh->df1 = (float)(nsub-2);
min_and_max_init(&ifh->global_min,&ifh->global_max);
min_and_max_doublestack(z,vol,&ifh->global_min,&ifh->global_max);
swapbytes = shouldiswap(SunOS_Linux,ifh->bigendian);
strcpy(filename,file);
if(!(strptr=get_tail_sans_ext(filename))) exit(-1);
strcat(strptr,"_zstat.4dfp.img");
if(!writestack(strptr,temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
if(!write_ifh(strptr,ifh,0)) exit(-1);
printf("Z statistic written to %s\n",strptr);
}
