/* Copyright 1/25/12 Washington University.  All Rights Reserved.
   fidl_language_sent_ev.c  $Revision: 1.7 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_language_sent_ev.c,v 1.7 2013/09/20 21:01:41 mcavoy Exp $";
main(int argc,char **argv)
{
char *txtfile=NULL,*concfile=NULL,*out=NULL,filename[MAXNAME],*strptr;
int i,j,i1,i2,skip=0,SunOS_Linux,firstrun=1,firstevent=0,nfiles,lcseparate=0,offset;
double TR=0.,duration=0.,time,time1,starttime;
Files_Struct *bolds;
Dim_Param *dp;
Data *data;
FILE *fp;
if(argc<7) {
    fprintf(stderr,"This is for sentence runs.\n");
    fprintf(stderr,"    -conc:     Conc file. Use the concatenated conc with -firstrun to concatenate event files.\n");
    fprintf(stderr,"    -TR:       Sampling rate (s).\n");
    fprintf(stderr,"    -firstrun:   Default is 1. Set >1 to concatenate event files.\n");
    fprintf(stderr,"    -firstevent: Default is 0. Set to nonzero to concatenate event files.\n");
    fprintf(stderr,"    -out:        Name of output file. Default is -txt with .fidl extension\n");
    fprintf(stderr,"  OLD FORMAT: First ten subjects\n");
    fprintf(stderr,"    -txt:      Name of eprime derived text file.\n");
    fprintf(stderr,"               Each run is 2 columns.\n");
    fprintf(stderr,"               The first is the sentence type (1=normal,2=incong or 3=rotated).\n");
    fprintf(stderr,"               The second is the ISI (ms).\n");
    fprintf(stderr,"    -skip:     Frames to skip at the begining of each run. Default is 0.\n");
    fprintf(stderr,"               Runs began with 10s of fix = 4 frames.\n");
    fprintf(stderr,"    -duration: Duration of each sentence (s).\n");
    fprintf(stderr,"               Each sentence lasted 3s.\n");
    fprintf(stderr,"  NEW FORMAT: Next ten subjects\n");
    fprintf(stderr,"    -txt:      Name of eprime derived text file.\n");
    fprintf(stderr,"               Each run is 4 columns.\n");
    fprintf(stderr,"               The first is TextDisplay1.RTTime or Pause1.OnsetTime (start of run)\n");
    fprintf(stderr,"               The second is Slide1.OnsetTime (start of sentence)\n");
    fprintf(stderr,"               The third is ISI.OnsetTime (duration = ISI.OnsetTime - Slide1.OnsetTime)\n");
    fprintf(stderr,"               The fourth is Condition (sentence type: 1=normal, 2=incong or 3=rotated)\n");

    /*START130916*/
    fprintf(stderr,"  -separate    Model sentence type separately for each run.\n");

    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-txt") && argc > i+1)
        txtfile = argv[++i];
    if(!strcmp(argv[i],"-conc") && argc > i+1)
        concfile = argv[++i];
    if(!strcmp(argv[i],"-TR") && argc > i+1)
        TR = atof(argv[++i]);
    if(!strcmp(argv[i],"-skip") && argc > i+1)
        skip = atoi(argv[++i]);
    if(!strcmp(argv[i],"-duration") && argc > i+1)
        duration = atof(argv[++i]);
    if(!strcmp(argv[i],"-firstrun") && argc > i+1)
        firstrun = atoi(argv[++i]);
    if(!strcmp(argv[i],"-firstevent") && argc > i+1)
        firstevent = atoi(argv[++i]);
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];

    /*START130916*/
    if(!strcmp(argv[i],"-separate"))
        lcseparate=1;

    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!txtfile) {
    printf("fidlError: need to specify -filename\n");
    exit(-1);
    }
if(!concfile) {
    printf("fidlError: need to specify -conc\n");
    exit(-1);
    }
if(TR<=0.) {
    printf("fidlError: TR=%f Need to specify -TR\n",TR);
    exit(-1);
    }
if(!(bolds=read_conc(concfile))) exit(-1);
if(!(dp=dim_param(bolds->nfiles,bolds->files,SunOS_Linux,0))) exit(-1);
if(!(data=read_data(txtfile,0,0,0,0)))exit(-1);
/*for(i=0;i<data->nsubjects;i++) {
    for(j=0;j<data->npoints;j++) printf("%g ",data->x[i][j]);
    printf("\n");
    }*/
if(out)
    strcpy(filename,out);
else {
    strcpy(filename,txtfile);

    #if 0
    if(!get_tail_sans_ext(filename)) exit(-1);
    strcat(filename,".fidl");   
    #endif
    /*START130616*/
    if(!(strptr=get_tail_sans_ext(filename))) exit(-1);
    strcat(strptr,".fidl");   

    }
nfiles=dp->nfiles-firstrun+1;
for(starttime=0.,i=0;i<firstrun-1;i++) starttime+=dp->tdim[i]*TR;
if(duration>0.) {
    if(nfiles>data->npoints/2) {
        nfiles=data->npoints/2;
        }
    else if(nfiles<data->npoints/2) {
        printf("fidlError: nfiles=%d data->npoints/2=%d nfiles must be equal or greater.\n",nfiles,data->npoints/2);fflush(stdout);
        exit(-1);
        }
    printf("Using OLD FORMAT\n%s\n%s\nTR=%g skip = %d duration=%g nfiles=%d starttime=%g firstevent=%d firstrun=%d\n",txtfile,
        concfile,TR,skip,duration,nfiles,starttime,firstevent,firstrun);fflush(stdout);

    #if 0
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    fprintf(fp,"%g normal incong rotated\n",TR);
    #endif
    /*START130916*/
    if(!(fp=fopen_sub(strptr,"w"))) exit(-1);
    if(!lcseparate) 
        fprintf(fp,"%g normal incong rotated\n",TR);
    else
        fprintf(fp,"%g normal1 incong1 rotated1 normal2 incong2 rotated2 normal3 incong3 rotated3\n",TR);

    for(time=starttime,i2=firstrun-1,i1=i=0;i<nfiles;i++,time+=dp->tdim[i2++]*TR,i1+=2) {

        #if 0
        for(time1=time+(double)skip*TR,j=0;j<data->nsubjects;time1+=duration+data->x[j++][i1+1]/1000.)
            fprintf(fp,"%g\t%g\t%g\n",time1,firstevent+data->x[j][i1]-1,duration);
        #endif
        /*START130916*/
        offset = lcseparate ? i*3 : 0;
        for(time1=time+(double)skip*TR,j=0;j<data->nsubjects;time1+=duration+data->x[j++][i1+1]/1000.)
            fprintf(fp,"%g\t%g\t%g\n",time1,offset+firstevent+data->x[j][i1]-1,duration);

        }
    fclose(fp);
    }
else {
    if(nfiles>data->npoints/4) {
        nfiles=data->npoints/4;
        }
    else if(nfiles<data->npoints/4) {
        printf("fidlError: nfiles=%d data->npoints/4=%d nfiles must be equal or greater.\n",nfiles,data->npoints/4);fflush(stdout);
        exit(-1);
        }
    printf("Using NEW FORMAT\n%s\n%s\nTR=%g nfiles=%d starttime=%g firstevent=%d firstrun=%d\n",txtfile,concfile,TR,nfiles,
        starttime,firstevent,firstrun);fflush(stdout);

    #if 0
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    fprintf(fp,"%g normal incong rotated\n",TR);
    #endif
    /*START130916*/
    if(!(fp=fopen_sub(strptr,"w"))) exit(-1);
    if(!lcseparate) 
        fprintf(fp,"%g normal incong rotated\n",TR);
    else
        fprintf(fp,"%g normal1 incong1 rotated1 normal2 incong2 rotated2 normal3 incong3 rotated3\n",TR);

    for(time=starttime,i2=firstrun-1,i1=i=0;i<nfiles;i++,time+=dp->tdim[i2++]*TR,i1+=4) {

        #if 0
        for(j=0;j<data->nsubjects;j++)
            fprintf(fp,"%g\t%g\t%g\n",time+(data->x[j][i1+1]-data->x[j][i1])/1000.,firstevent+data->x[j][i1+3]-1,
                (data->x[j][i1+2]-data->x[j][i1+1])/1000.);
        #endif
        /*START130916*/
        offset = lcseparate ? i*3 : 0;
        for(j=0;j<data->nsubjects;j++)
            fprintf(fp,"%g\t%g\t%g\n",time+(data->x[j][i1+1]-data->x[j][i1])/1000.,offset+firstevent+data->x[j][i1+3]-1,
                (data->x[j][i1+2]-data->x[j][i1+1])/1000.);

        }
    fclose(fp);
    }

/*printf("Event file written to %s\n",filename);fflush(stdout);*/
/*START130916*/
printf("Event file written to %s\n",strptr);fflush(stdout);

}
