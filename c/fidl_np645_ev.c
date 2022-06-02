/* Copyright 12/7/06 Washington University.  All Rights Reserved.
   fidl_np645_ev.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_np645_ev.c,v 1.2 2006/12/13 00:05:31 mcavoy Exp $";

main(int argc,char **argv)
{
char *filename=NULL,*concfile=NULL;
int i,skip=0,interval=0,frames,frames_remaining,condition;
float TR=0.,interval_s,time;
Files_Struct *bolds;
Dim_Param *dp;
FILE *fp;

if(argc < 9) {
    fprintf(stderr,"    -filename: Name of event file to be created.\n");
    fprintf(stderr,"    -conc:     Includes identifier for run type.\n");
    fprintf(stderr,"    -TR:       Sampling rate.\n");
    fprintf(stderr,"    -skip:     Frames to skip at the begining of each run.\n");
    fprintf(stderr,"    -interval: Block length in frames.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-filename") && argc > i+1)
        filename = argv[++i];
    if(!strcmp(argv[i],"-conc") && argc > i+1)
        concfile = argv[++i];
    if(!strcmp(argv[i],"-TR") && argc > i+1)
        TR = atof(argv[++i]);
    if(!strcmp(argv[i],"-skip") && argc > i+1)
        skip = atoi(argv[++i]);
    if(!strcmp(argv[i],"-interval") && argc > i+1)
        interval = atoi(argv[++i]);
    }
if(!filename) {
    printf("Error: need to specify -filename\n");
    exit(-1);
    }
if(!concfile) {
    printf("Error: need to specify -conc\n");
    exit(-1);
    }
if(TR<=0.) {
    printf("Error: TR=%f Need to specify -TR\n",TR);
    exit(-1);
    }
if(!interval) {
    printf("Error: Need to specify -interval\n");
    exit(-1);
    }
interval_s = interval*TR;
printf("skip=%d\n",skip);
if(!(bolds=read_conc(concfile))) exit(-1);
if(!(dp=dim_param(bolds->nfiles,bolds->files))) exit(-1);

if(!(fp=fopen_sub(filename,"w"))) exit(-1);
fprintf(fp,"%.2f fixation open closed\n",TR); 
for(time=0.,i=0;i<bolds->nfiles;i++) {
    time += skip*TR;
    if(!(strcmp("fixation",bolds->identify[i]))) {
        condition=0;
        }
    else if(!(strcmp("open",bolds->identify[i]))) {
        condition=1;
        }
    else if(!(strcmp("closed",bolds->identify[i]))) {
        condition=2;
        }
    else {
        printf("Error: bolds->identify[%d]=%s Unknown. Abort!\n",i,bolds->identify[i]);
        exit(-1);
        }
    for(frames=skip;frames<dp->tdim[i];frames+=interval) {
        if((frames_remaining=dp->tdim[i]-frames)>=interval) {
            fprintf(fp,"%-7.2f %d %.2f\n",time,condition,interval_s);
            time += interval_s;
            }
        else {
            fprintf(fp,"%-7.2f -%d\n",time,frames_remaining);
            time += frames_remaining*TR;
            }
        }
    }
fclose(fp);
printf("Event file written to %s\n",filename);
}
