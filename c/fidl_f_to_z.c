/* Copyright 6/12/09 Washington University.  All Rights Reserved.
   fidl_f_to_z.c  $Revision: 1.3 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

main(int argc,char **argv)
{
char *file=NULL,filename[MAXNAME],*strptr;
int i,j,SunOS_Linux,vol,swapbytes;
float *temp_float;
double *fstatmap,*zstat,*df1,*df2;
Interfile_header *ifh;

if(argc < 3) {
    fprintf(stderr,"Usage: fidl_f_to_z -fstat fstat.4dfp.img\n");
    fprintf(stderr,"        -fstat:  F statistic 4dfp\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-fstat") && argc > i+1)
        file=argv[++i];
    }
if(!file) {
    printf("Error: Need -fstat\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

/*if(!(ifh=read_ifh(file))) exit(-1);*/
/*START111109*/
if(!(ifh=read_ifh(file,(Interfile_header*)NULL))) exit(-1);

vol = ifh->dim1*ifh->dim2*ifh->dim3;
if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(fstatmap=malloc(sizeof*fstatmap*vol))) {
    printf("Error: Unable to malloc fstatmap\n");
    exit(-1);
    }
if(!(zstat=malloc(sizeof*zstat*vol))) {
    printf("Error: Unable to malloc zstat\n");
    exit(-1);
    }
if(!(df1=malloc(sizeof*df1*vol))) {
    printf("Error: Unable to malloc df1\n");
    exit(-1);
    }
if(!(df2=malloc(sizeof*df2*vol))) {
    printf("Error: Unable to malloc df2\n");
    exit(-1);
    }
if(!readstack(file,(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
for(i=0;i<vol;i++) {
    fstatmap[i] = (double)temp_float[i];
    df1[i] = (double)ifh->dof_condition;
    df2[i] = (double)ifh->dof_error;
    }
f_to_z(fstatmap,zstat,vol,df1,df2);
ifh->dof_condition=ifh->dof_error=0.;
min_and_max_init(&ifh->global_min,&ifh->global_max);
min_and_max_doublestack(zstat,vol,&ifh->global_min,&ifh->global_max);
for(j=0;j<vol;j++) temp_float[j] = (float)zstat[j];


#if 0
strcpy(filename,file);
if((strptr=strstr(filename,"fstat")))
    *strptr = 'z';
else {
    *(strrchr(filename,'.')-5)=0;
    strcat(filename,"_zstat.4dfp.img");
    }
#endif
/*START111109*/
if((strptr=strstr(filename,"fstat"))) {
    strcpy(filename,file);
    *strptr = 'z';
    }
else {
    if(!(strptr=get_tail_sans_ext(file))) exit(-1);
    sprintf(filename,"%s_zstat.4dfp.img",strptr);
    }


swapbytes = shouldiswap(SunOS_Linux,ifh->bigendian);
if(!writestack(filename,temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
if(!write_ifh(filename,ifh,0)) exit(-1);
printf("Z statistics written to %s\n",filename);
}
