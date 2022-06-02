/* Copyright 8/18/04 Washington University.  All Rights Reserved.
   fidl_np600_ev.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_np600_correction.c,v 1.2 2006/09/25 21:06:15 mcavoy Exp $";

main(int argc,char **argv)
{
char *subject=NULL,*LCrunLC=NULL,*LOrunLC=NULL,*LCrunLO=NULL,*LOrunLO=NULL;
int i,j,k,nfiles=0,nruns=0;
FILE *fp;
Files_Struct *files,*runs;
Data *dLCrunLC,*dLOrunLC,*dLCrunLO,*dLOrunLO;

if(argc < 5) {
    fprintf(stderr,"    -files:   Names of correction file to be appended. One for each frequency.\n");
    fprintf(stderr,"    -subject: Subject identifier. ex. s02\n");
    fprintf(stderr,"    -runs:    LC or LO, identifier for each run\n");
    fprintf(stderr,"    -LCrunLC: LC run, LC condition\n");
    fprintf(stderr,"    -LOrunLC: LO run, LC condition\n");
    fprintf(stderr,"    -LCrunLO: LC run, LO condition\n");
    fprintf(stderr,"    -LOrunLO: LO run, LO condition\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    if(!strcmp(argv[i],"-subject") && argc > i+1)
        subject = argv[++i];
    if(!strcmp(argv[i],"-runs") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nruns;
        if(!(runs=get_files(nruns,&argv[i+1]))) exit(-1);
        i += nruns;
        }
    if(!strcmp(argv[i],"-LCrunLC") && argc > i+1)
        LCrunLC = argv[++i];
    if(!strcmp(argv[i],"-LOrunLC") && argc > i+1)
        LOrunLC = argv[++i];
    if(!strcmp(argv[i],"-LCrunLO") && argc > i+1)
        LCrunLO = argv[++i];
    if(!strcmp(argv[i],"-LOrunLO") && argc > i+1)
        LOrunLO = argv[++i];
    }
if(!subject) {
    printf("Error: Need to specify -subject\n");
    exit(-1);
    }
if(!LCrunLC) {
    printf("Error: Need to specify -LCrunLC\n");
    exit(-1);
    }
if(!LOrunLC) {
    printf("Error: Need to specify -LOrunLC\n");
    exit(-1);
    }
if(!LCrunLO) {
    printf("Error: Need to specify -LCrunLO\n");
    exit(-1);
    }
if(!LOrunLO) {
    printf("Error: Need to specify -LOrunLO\n");
    exit(-1);
    }

if(!(dLCrunLC=read_data(LCrunLC))) exit(-1);
if(!(dLOrunLC=read_data(LOrunLC))) exit(-1);
if(!(dLCrunLO=read_data(LCrunLO))) exit(-1);
if(!(dLOrunLO=read_data(LOrunLO))) exit(-1);

for(k=0;k<nfiles;k++) {
    if(!(fp=fopen_sub(files->files[k],"a"))) exit(-1);
    printf("%s\n",subject);
    fprintf(fp,"%s ",subject);
    for(i=0;i<nruns;i++) {
        if(!strcmp(runs->files[i],"LO")) {
            printf("    run %d %s long open\n",i+1,runs->files[i]);
            fprintf(fp,"%.4f ",dLOrunLC->x[k][0]);
            }
        else if(!strcmp(runs->files[i],"LC")) {
            printf("    run %d %s long closed\n",i+1,runs->files[i]);
            fprintf(fp,"%.4f ",dLCrunLC->x[k][0]);
            }
        else {
            printf("    Error: Unknown run type. Abort!\n");
            exit(-1);
            }
        }
    for(i=0;i<nruns;i++) {
        if(!strcmp(runs->files[i],"LO")) {
            printf("    run %d %s long open\n",i+1,runs->files[i]);
            fprintf(fp,"%.4f ",dLOrunLO->x[k][0]);
            }
        else if(!strcmp(runs->files[i],"LC")) {
            printf("    run %d %s long closed\n",i+1,runs->files[i]);
            fprintf(fp,"%.4f ",dLCrunLO->x[k][0]);
            }
        else {
            printf("    Error: Unknown run type. Abort!\n");
            exit(-1);
            }
        }
    fprintf(fp,"\n");
    fclose(fp);
    }
}
