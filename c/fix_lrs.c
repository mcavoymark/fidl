/* Copyright 7/26/01 Washington University.  All Rights Reserved.
   fix_lrs.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

main(int argc,char **argv)
{
char *in=NULL,*out=NULL;

int i,vol;

float *temp_float,min,max;

double log10_null=0,log_null;

Interfile_header *ifh;

if(argc < 4) {
    fprintf(stderr,"        -file:          Likelihood ratio statistic to be fixed.\n");
    fprintf(stderr,"        -log10_null:    Log1o null for the file.\n");
    fprintf(stderr,"        -output:        Name of output file for the fixed statistic.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-file") && argc > i+1)
        in = argv[++i];
    if(!strcmp(argv[i],"-log10_null") && argc > i+1)
        log10_null = atof(argv[++i]);
    if(!strcmp(argv[i],"-output") && argc > i+1)
        out = argv[++i];
    }
if(!in) {
    printf("-file not specified. Abort!\n");
    exit(-1);
    }
if(!out) {
    printf("-output not specified. Abort!\n");
    exit(-1);
    }
if(log10_null > (double)0.) {
    printf("-log10_null must be a negative number. Abort!\n");
    exit(-1);
    }
    
if(!(ifh = read_ifh(in))) exit(-1);
vol = ifh->dim1*ifh->dim2*ifh->dim3;
if(!(temp_float=d1float(vol))) exit(-1);
if(!read_float(in,temp_float,vol)) exit(-1);
log_null = log(pow(10.,log10_null));
printf("log10_null=%f log_null=%f\n",log10_null,log_null);
for(i=0;i<vol;i++) temp_float[i] = (float)((double)2.*(log(pow(10.,(double).5*(double)temp_float[i] + log10_null)) - log_null));
if(!write_float(out,temp_float,vol)) exit(-1);
min_and_max(temp_float,vol,&ifh->global_min,&ifh->global_max);
if(!write_ifh(out,ifh,(int)FALSE)) exit(-1); 
printf("Fixed lrs written to %s\n",out);
}
