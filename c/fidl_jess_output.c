/* Copyright 10/12/07 Washington University.  All Rights Reserved.
   fidl_jess_output.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_jess_output.c,v 1.2 2007/11/13 01:23:58 mcavoy Exp $";

main(int argc,char **argv)
{
char *root=NULL,filename[MAXNAME];
int i,j,k,l,m,dim,vol,size,size2,SunOS_Linux,count;
float *temp_float;
FILE *fp;
Interfile_header *ifh;


int nruns=0;

if(argc<5) {
    fprintf(stderr,"    -root:  Sans <>.4dfp.img\n");
    fprintf(stderr,"    -nruns: How many frames of data went into the correlation coefficient?\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-root") && argc > i+1)
        root = argv[++i];
    else if(!strcmp(argv[i],"-nruns") && argc > i+1)
        nruns = atoi(argv[++i]);
    else {
        printf("Warning: Unknown option %s\n",argv[i]);
        }
    }
if(!root) {
    printf("Error: Need to specify -root\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

for(i=0;i<nruns;i++) {
    sprintf(filename,"%s%d.4dfp.img",root,i);
    if(!(fp=fopen_sub(filename,"r"))) exit(-1);
    fclose(fp);
    }
sprintf(filename,"%s0.4dfp.img",root);
if(!(ifh=read_ifh(filename))) exit(-1);
size=ifh->dim1*ifh->dim2*ifh->dim3*ifh->dim4;
dim=ifh->dim1;
vol=dim*(dim-1)/2;
free_ifh(ifh,0);
for(i=1;i<nruns;i++) {
    sprintf(filename,"%s%d.4dfp.img",root,i);
    if(!(ifh=read_ifh(filename))) exit(-1);
    if((size2=ifh->dim1*ifh->dim2*ifh->dim3*ifh->dim4)!=size) {
        printf("Error: size=%d size2=%d %s  Must be equal!\n",size,size2,filename);
        exit(-1);
        }
    free_ifh(ifh,0);
    }
if(!(temp_float=malloc(sizeof*temp_float*size))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(count=i=0;i<nruns;i++) {
    sprintf(filename,"%s%d.4dfp.img",root,i);
    if(!readstack(filename,(float*)temp_float,sizeof(float),(size_t)size,SunOS_Linux)) exit(-1);
    for(m=l=j=0;j<dim;j++,m+=j) {
        for(k=j+1;k<dim;k++,l++,m++) {
            if(fabs(temp_float[m])>4.) count++;
            }
        }
    /*printf("l=%d\n",l);*/
    }
printf("count=%d\n",count);
exit(0);
}
