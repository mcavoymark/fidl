/* Copyright 3/4/06 Washington University.  All Rights Reserved.
   fidl_CO2.c  $Revision: 1.3 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_CO2.c,v 1.3 2008/03/06 00:24:28 mcavoy Exp $";

main(int argc,char **argv)
{
char *CO2=NULL,*out=NULL,string[MAXNAME],*strptr;
int i,j,k,nskip=0,SunOS_Linux,nCO2_to_avg=0,nout=0;
float *temp_float;
double sum;
Data *data;
Interfile_header *ifh;

print_version_number(rcsid,stdout);
if(argc < 5) {
    fprintf(stderr,"    -CO2:         CO2 file to be converted to 4dfp.\n");
    fprintf(stderr,"    -out:         Name of output 4dfp. Default is the same root as CO2 file.\n");
    fprintf(stderr,"    -nskip:       Skip this number of lines at the beginning of the CO2 file. Default is 0.\n");
    fprintf(stderr,"    -nCO2_to_avg: Average this many CO2 measurements for a single -nout data point. Default is zero.\n");
    fprintf(stderr,"    -nout:        Number of time frames to go into the output 4dfp.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-CO2") && argc > i+1)
        CO2 = argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    if(!strcmp(argv[i],"-nskip") && argc > i+1)
        nskip = atoi(argv[++i]);
    if(!strcmp(argv[i],"-nCO2_to_avg") && argc > i+1)
        nCO2_to_avg = atoi(argv[++i]);
    if(!strcmp(argv[i],"-nout") && argc > i+1)
        nout = atoi(argv[++i]);
    }
if(!CO2) {
    printf("Error: No CO2 file specified with -CO2 option. Abort!\n");
    exit(-1);
    }
if(!nout) {
    printf("Error: -nout not specified. Abort!\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(data=read_data(CO2,1,nskip,0))) exit(-1);
if((double)data->nsubjects/(double)nCO2_to_avg < nout) {
    printf("Error: %s has %d CO2 measurements. It needs %d. Abort!\n",CO2,data->nsubjects,nCO2_to_avg*nout);
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*nout))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(k=i=0;i<nout;i++) {
    for(sum=0.,j=0;j<nCO2_to_avg;j++,k++) sum += data->x[k][0]; 
    temp_float[i] = (float)(sum/(double)nCO2_to_avg);
    }
if(!out) {
    strcpy(string,CO2);
    if(!(strptr=get_tail_sans_ext(string))) exit(-1);
    strcat(strptr,".4dfp.img");
    out = strptr;
    }
if(!writestack(out,temp_float,sizeof*temp_float,(size_t)nout,0)) exit(-1);
if(!(ifh=init_ifh(4,1,1,1,nout,3,3,3,!SunOS_Linux?1:0))) exit(-1);
if(!write_ifh(out,ifh,(int)FALSE)) exit(-1);
printf("Output written to %s\n",out);
}
