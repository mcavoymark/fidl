/* Copyright 8/19/14 Washington University.  All Rights Reserved.
   fidl_lang_ttestRT.c  $Revision: 1.1 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_language_senttest_ev.c,v 1.6 2013/10/11 17:16:26 mcavoy Exp $";
main(int argc,char **argv)
{
char *txtfile=NULL,*txtfile6=NULL,*concfile=NULL,filename[MAXNAME],*out=NULL;
int i,j,k,i1,j1,skipblock=0,sentperblock=0,oldresponse=0,newresponse=0,SunOS_Linux,colperrun,nblock,cond,*condn, /*[5]={0,0,0,0,0},*/
    **condnrun,miss;
double TR=0.,duration=0.,fix=0.,fixstate=0.,time,time1,state=0.,total,*condnRT;
Files_Struct *bolds;
Dim_Param *dp;
Data *data;
FILE *fp=NULL;
if(argc<11) {
    fprintf(stderr,"This is for sentence test runs.\n");
    fprintf(stderr,"  -conc:         Conc file.\n");
    fprintf(stderr,"  -TR:           Sampling rate (s).\n");
    fprintf(stderr,"  -oldresponse:  Key number for old response (1,2,3,4).\n");
    fprintf(stderr,"  -sentperblock: Number of sentences for each block. Some had 8 others 6.\n");
    fprintf(stderr,"  -state:        Length of state effect (s).\n");
    fprintf(stderr,"  -out:          Name of output event file. NOEV for no event file (ie just RTs and accuracies)\n");
    fprintf(stderr,"OLD VERSION\n");
    fprintf(stderr,"  -txt:          Name of eprime derived text file. Each run is 4 columns.\n");
    fprintf(stderr,"                 The first is the sentence type (1=old,2=new).\n");
    fprintf(stderr,"                 The second is the ISI (ms).\n");
    fprintf(stderr,"                 The third is whether the sentence was identified as old or new. (Dependent on keyboard mapping.)\n");
    fprintf(stderr,"                 The fourth is the response time (ms).\n"); 
    fprintf(stderr,"  -duration:     Maximum duration of each sentence (s).\n");
    fprintf(stderr,"                 Each sentence was shown a maximum of 4s.\n");
    fprintf(stderr,"  -skipblock:    Frames to skip at the begining of each block. Default is 0.\n");
    fprintf(stderr,"                 Blocks began with 2.5s of blank screen = 1 frame.\n");
    fprintf(stderr,"  -fix:          Length of fixation (s) between blocks. Not modeled.\n");
    fprintf(stderr,"  -fixstate:     Length of fixation (s) between blocks. Modeled.\n");
    fprintf(stderr,"NEW VERSION\n");
    fprintf(stderr,"  -txt6:         Name of eprime derived text file. Each run is 6 columns.\n");
    fprintf(stderr,"                 The first is the start of the run (ms). TriggerScreen1.RTTime\n");
    fprintf(stderr,"                 The second is the start of the sentence (ms). Sentence.OnsetTime\n");
    fprintf(stderr,"                 The third is the sentence type (1=old,2=new). CR\n");
    fprintf(stderr,"                 The fourth is whether the sentence was identified as old or new.\n");
    fprintf(stderr,"                     (Dependent on keyboard mapping.) Sentence.RESP or ISI3.RESP\n");
    fprintf(stderr,"                     Also check Blank2.RESP for corrections.\n");
    fprintf(stderr,"                 The fifth is the response time (ms). Sentence.RT or ISI3.RTTime-Sentence.OnsetTime\n"); 
    fprintf(stderr,"                 The sixth is the start of the state of sentence reading (ms). Blank.OnsetTime (2.5s blank screen before first sentence)\n"); 
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-txt") && argc > i+1)
        txtfile = argv[++i];
    if(!strcmp(argv[i],"-txt6") && argc > i+1)
        txtfile6 = argv[++i];
    if(!strcmp(argv[i],"-conc") && argc > i+1)
        concfile = argv[++i];
    if(!strcmp(argv[i],"-TR") && argc > i+1)
        TR = atof(argv[++i]);
    if(!strcmp(argv[i],"-duration") && argc > i+1)
        duration = atof(argv[++i]);
    if(!strcmp(argv[i],"-skipblock") && argc > i+1)
        skipblock = atoi(argv[++i]);
    if(!strcmp(argv[i],"-sentperblock") && argc > i+1)
        sentperblock = atoi(argv[++i]);
    if(!strcmp(argv[i],"-fix") && argc > i+1)
        fix = atof(argv[++i]);
    if(!strcmp(argv[i],"-fixstate") && argc > i+1)
        fixstate = atof(argv[++i]);
    if(!strcmp(argv[i],"-oldresponse") && argc > i+1)
        oldresponse = atoi(argv[++i]);
    if(!strcmp(argv[i],"-state") && argc > i+1)
        state = atof(argv[++i]);
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!txtfile&&!txtfile6) {
    printf("fidlError: need to specify either -txt or -txt5\n");
    exit(-1);
    }
if(txtfile&&txtfile6) {
    printf("fidlError: need to specify either -txt or -txt6, not both\n");
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
if(!oldresponse) {
    printf("fidlError: need to specify -oldresponse\n");
    exit(-1);
    }
if(txtfile&&duration<=0.) {
    if(duration<=0.) {
        printf("fidlError: Need to specify -duration\n");
        exit(-1);
        }
    if(fix&&fixstate) {
        printf("fidlError: specify only one -fix or -fixstate, not both\n");
        exit(-1);
        }
    }
if(txtfile) {
    printf("%s\n%s\nTR=%g duration=%g\nskipblock=%d sentperblock=%d fix=%g oldresponse=%d state=%g fixstate=%g\n",txtfile,concfile,
        TR,duration,skipblock,sentperblock,fix,oldresponse,state,fixstate);
    colperrun=4;
    }
else {
    printf("%s\n%s\nTR=%g oldresponse=%d state=%g\n",txtfile6,concfile,TR,oldresponse,state);
    colperrun=6;
    }
fflush(stdout);
if(!(bolds=read_conc(concfile))) exit(-1);
if(!(dp=dim_param(bolds->nfiles,bolds->files,SunOS_Linux,0))) exit(-1);
if(!(data=read_data(txtfile?txtfile:txtfile6,0,0,0,0)))exit(-1);
for(i=0;i<data->nsubjects;i++) {
    for(j=0;j<data->npoints;j++) printf("%g\t",data->x[i][j]);
    printf("\n");
    }
if(dp->nfiles!=data->npoints/colperrun) {
    printf("fidlError: dp->nfiles=%d data->npoints/colperrun=%d Must be equal.\n",dp->nfiles,data->npoints/colperrun);
    fflush(stdout);exit(-1);
    }

if(!out) {
    strcpy(filename,txtfile);
    if(!get_tail_sans_ext(filename)) exit(-1);
    strcat(filename,".fidl");    
    out = filename;
    }

#if 0
if(!(fp=fopen_sub(out,"w"))) exit(-1);
fprintf(fp,"%g oldold oldnew newold newnew %s %s miss\n",TR,state?"state":"",fixstate?"fix":"");
#endif
/*START140819*/
if(!strcmp(out,"NOEV")) {
    if(!(fp=fopen_sub(out,"w"))) exit(-1);
    fprintf(fp,"%g oldold oldnew newold newnew %s %s miss\n",TR,state?"state":"",fixstate?"fix":"");
    }

miss=4;if(state)miss++;if(fixstate)miss++;
if(!(condn=malloc(sizeof*condn*(miss+1)))) {
    printf("fidlError: Unable to malloc condn\n");
    exit(-1);
    }
for(i=0;i<miss+1;i++) condn[i]=0;
if(!(condnrun=d2int(dp->nfiles,miss+1))) exit(-1);
for(i=0;i<dp->nfiles;i++)for(j=0;j<miss+1;j++)condnrun[i][j]=0;

/*START140819*/
if(!(condnRT=malloc(sizeof*condnRT*(miss+1)))) {
    printf("fidlError: Unable to malloc condnRT\n");
    exit(-1);
    }
for(i=0;i<miss+1;i++) condn[i]=0.;

nblock=data->nsubjects/sentperblock;
printf("nblock=%d\n",nblock);fflush(stdout);
if(txtfile) {
    for(time=0.,i1=i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR,i1+=colperrun) {
        for(time1=time,j1=j=0;j<nblock;j++,time1+=fix) {

            /*if(state) fprintf(fp,"%g\t4\t%g\n",time1,state);*/
            /*START140819*/
            if(state&&fp) fprintf(fp,"%g\t4\t%g\n",time1,state);

            for(time1+=(double)skipblock*TR,k=0;k<sentperblock;k++,time1+=duration+data->x[j1++][i1+1]/1000.) {
                cond = !((int)data->x[j1][i1+2]) ? miss : (((int)data->x[j1][i1]==1?0:2)+((int)data->x[j1][i1+2]==oldresponse?0:1));
                condn[cond]++;
                condnrun[i][cond]++;

                /*START140819*/
                condnRT[cond]+=data->x[j1][i1+3];

                /*fprintf(fp,"%g\t%d\t%g\n",time1,cond,data->x[j1][i1+3]/1000.);*/
                /*START140819*/
                if(fp) fprintf(fp,"%g\t%d\t%g\n",time1,cond,data->x[j1][i1+3]/1000.);

                }

            /*if(fixstate&&j<(nblock-1)) {fprintf(fp,"%g\t5\t%g\n",time1,fixstate);time1+=fixstate;}*/
            /*START140819*/
            if(fixstate&&j<(nblock-1)) {if(fp)fprintf(fp,"%g\t5\t%g\n",time1,fixstate);time1+=fixstate;}

            }
        }
    }
if(txtfile6) {
    for(time=0.,i1=i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR,i1+=colperrun) {
        for(j1=j=0;j<nblock;j++) {

            /*if(state) fprintf(fp,"%g\t4\t%g\n",time+(data->x[j1][i1+5]-data->x[j1][i1])/1000.,state);*/
            /*START140819*/
            if(state&&fp) fprintf(fp,"%g\t4\t%g\n",time+(data->x[j1][i1+5]-data->x[j1][i1])/1000.,state);

            for(k=0;k<sentperblock;k++,j1++) {
                cond = !((int)data->x[j1][i1+3]) ? miss : (((int)data->x[j1][i1+2]==1?0:2)+((int)data->x[j1][i1+3]==oldresponse?0:1));
                condn[cond]++;
                condnrun[i][cond]++;

                /*START140819*/
                condnRT[i][cond]+=data->x[j1][i1+4];

                /*fprintf(fp,"%g\t%d\t%g\n",time+(data->x[j1][i1+1]-data->x[j1][i1])/1000.,cond,data->x[j1][i1+4]/1000.);*/
                /*START140819*/
                if(fp) fprintf(fp,"%g\t%d\t%g\n",time+(data->x[j1][i1+1]-data->x[j1][i1])/1000.,cond,data->x[j1][i1+4]/1000.);

                }
            }
        }
    }

#if 0
fclose(fp);
printf("Event file written to %s\n    oldold=%2d oldnew=%2d newold=%2d newnew=%2d miss=%2d\n",out,condn[0],condn[1],condn[2],condn[3],
    condn[miss]);
for(i=0;i<dp->nfiles;i++) {
    for(total=j=0;j<4;j++) total+=condnrun[i][j];
    printf("    run %d  oldold=%2d oldnew=%2d newold=%2d newnew=%2d miss=%2d accuracy=%f\n",i+1,condnrun[i][0],condnrun[i][1],
        condnrun[i][2],condnrun[i][3],condnrun[i][miss],(condnrun[i][0]+condnrun[i][3])/total);
    }
#endif
/*START140819*/
if(fp) {
    fflush(fp);fclose(fp);
    printf("Event file written to %s\n    oldold=%2d oldnew=%2d newold=%2d newnew=%2d miss=%2d\n",out,condn[0],condn[1],condn[2],
        condn[3],condn[miss]);
    for(i=0;i<dp->nfiles;i++) {
        for(total=j=0;j<4;j++) total+=condnrun[i][j];
        printf("    run %d  oldold=%2d oldnew=%2d newold=%2d newnew=%2d miss=%2d accuracy=%f\n",i+1,condnrun[i][0],condnrun[i][1],
            condnrun[i][2],condnrun[i][3],condnrun[i][miss],(condnrun[i][0]+condnrun[i][3])/total);
        }
    }
printf("oldold=%2d oldnew=%2d newold=%2d newnew=%2d miss=%2d accuracy=%f\n",condn[0],condn[1],condn[2],condn[3],condn[miss],
    double(condn[0]+condn[3])/double(condn[0]+condn[1]+condn[2]+condn[3]));
printf("RT oldold=%f oldnew=%f newold=%f newnew=%f\n",condnRT[0]/(double)condn[0],condnRT[1]/(double)condn[1],
    condnRT[2]/(double)condn[2],condnRT[3]/(double)condn[3]);


fflush(stdout);
}
