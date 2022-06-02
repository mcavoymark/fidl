/* Copyright 10/12/10 Washington University.  All Rights Reserved.
   fidl_francesca_ev.c  $Revision: 1.4 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_francesca_ev.c,v 1.4 2011/04/19 21:04:52 mcavoy Exp $";
main(int argc,char **argv)
{
char *filename=NULL,*concfile=NULL;
int i,j,k,skip=0,cycleframes,SunOS_Linux,quad=0,ncond,len,cyclesperrun;
double TR=0.,cycle=0.,time,lent;
Files_Struct *bolds;
Dim_Param *dp;
FILE *fp;
if(argc < 9) {
    fprintf(stderr,"    -filename: Name of event file to be created.\n");
    fprintf(stderr,"    -conc:     Conc file.\n");
    fprintf(stderr,"    -TR:       Sampling rate (s).\n");
    fprintf(stderr,"    -skip:     Frames to skip at the begining of each run.\n");
    fprintf(stderr,"    -cycle:    One complete cycle (s).\n");
    fprintf(stderr,"    -quad      Each quadrant is a separate condition.\n");
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
    if(!strcmp(argv[i],"-cycle") && argc > i+1)
        cycle = atof(argv[++i]);
    if(!strcmp(argv[i],"-quad"))
        quad = 1;
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
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
if(cycle<=0.) {
    printf("Error: Need to specify -cycle\n");
    exit(-1);
    }
if(!(bolds=read_conc(concfile))) exit(-1);
if(!(dp=dim_param(bolds->nfiles,bolds->files,SunOS_Linux,0))) exit(-1);
if(!(fp=fopen_sub(filename,"w"))) exit(-1);


#if 0
cycleframes = (int)rint(cycle/TR);
printf("TR=%f skip=%d cycle=%f cycleframes=%d\n",TR,skip,cycle,cycleframes);
fprintf(fp,"%.2f",TR); for(i=0;i<32;i++) fprintf(fp," pol%d",i+1); fprintf(fp,"\n");
for(time=0.,i=0;i<bolds->nfiles;i++) {
    time += skip*TR;
    for(k=0,j=0;j<dp->tdim[i];j++,time+=TR) {
        fprintf(fp,"%-8.2f%-3d 2\n",time,k);
        if(++k==cycleframes) k=0;
        }
    }
#endif

#if 0
/*START110119 KEEP polarangle*/
cycleframes = (int)rint(cycle/TR);
ncond = !quad ? cycleframes : 4;
len = cycleframes/ncond;
lent = (double)len * TR; 
printf("TR=%f skip=%d cycle=%f cycleframes=%d ncond=%d len=%d\n",TR,skip,cycle,cycleframes,ncond,len);
fprintf(fp,"%.2f",TR); for(i=0;i<ncond;i++) fprintf(fp," pol%d",i+1); fprintf(fp,"\n");
for(time=0.,i=0;i<bolds->nfiles;i++) {
    time += skip*TR;
    for(k=0,j=0;j<dp->tdim[i]/len;j++,time+=TR*len) {
        fprintf(fp,"%-8.2f%-3d %f\n",time,k,lent);
        if(++k==ncond) k=0;
        }
    }
#endif

#if 1
/*START110225 KEEP*/

/*fprintf(fp,"%.2f center up down\n",TR);*/
/*START110412*/
if(bolds->identify) fprintf(fp,"%.2f center up down\n",TR); else fprintf(fp,"%.2f pol\n",TR);

for(time=0.,k=i=0;i<bolds->nfiles;i++) {
    cyclesperrun = (dp->tdim[i]-skip)*TR/cycle;


    #if 0
    if(!strcmp(bolds->identify[i],"center")) k=0;
    else if(!strcmp(bolds->identify[i],"up")) k=1;
    else if(!strcmp(bolds->identify[i],"down")) k=2;
    #endif
    /*110412*/
    if(bolds->identify) {
        if(!strcmp(bolds->identify[i],"center")) k=0;
        else if(!strcmp(bolds->identify[i],"up")) k=1;
        else if(!strcmp(bolds->identify[i],"down")) k=2;
        }


    time += skip*TR;
    for(j=0;j<cyclesperrun;j++,time+=cycle) fprintf(fp,"%-8.2f%-3d %f\n",time,k,cycle);
    }
#endif

fclose(fp);
printf("Event file written to %s\n",filename);
}
