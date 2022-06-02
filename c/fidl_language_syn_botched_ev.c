/* Copyright 2/21/13 Washington University.  All Rights Reserved.
   fidl_language_syn_botched_ev.c  $Revision: 1.5 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_language_syn_botched_ev.c,v 1.5 2013/10/25 20:23:41 mcavoy Exp $";
main(int argc,char **argv)
{
char *run1[]={"math","syn","math","math","syn","syn","math","syn","syn","math"}; /*syn=0 math==1*/
char *run2[]={"syn","syn","math","math","syn","math","math","syn","syn","math"};
char *run3[]={"math","syn","math","syn","syn","math","syn","math","math","syn"};
char *txtfile=NULL,*concfile=NULL,filename[MAXNAME],*strptr;
int i,j,i1,SunOS_Linux,**cond,txt=0,txt3=0,txt4=0,lcrr=0;
double TR=0.,skipsec=0.,fix=0.,time,time1,time2,time3;
Files_Struct *bolds;
Dim_Param *dp;
Data *data;
FILE *fp;
if(argc<7) {
    fprintf(stderr,"This is for synonym runs.\n");
    fprintf(stderr,"  -conc:    Conc file.\n");
    fprintf(stderr,"  -TR:      Sampling rate (s).\n");
    fprintf(stderr,"OLD FORMAT\n");
    fprintf(stderr,"  -txt:     Name of eprime derived text file.\n");
    fprintf(stderr,"            Each run is 2 columns: TriggerScreen.RTTime WhiteCross.OnsetTime\n");
    fprintf(stderr,"  -skipsec: Initial fixation (white cross) at the begining of each run (s). Default is 0.\n");
    fprintf(stderr,"            Runs for vc36176 began with 3s of fixation. There was also a 2s cue. \n");
    fprintf(stderr,"  -fix:     Length of fixation (s) between blocks.\n");
    fprintf(stderr,"            For vc36176, 15s of fixation between blocks. There was also a 3s cue.\n");
    fprintf(stderr,"  -skipsec 5 -fix 18 Blocks will start at the stimuli.\n");
    fprintf(stderr,"  -skipsec 3 -fix 15 Blocks will start at the cue.\n");
    fprintf(stderr,"NEW FORMAT\n");
    fprintf(stderr,"  -txt3:    Name of eprime derived text file.\n");
    fprintf(stderr,"            Each run is 3 columns: TriggerScreen.RTTime Pause2.OnsetTime||GreenCross.OnsetTime WhiteCross.OnsetTime\n");
    fprintf(stderr,"  -txt4:    Name of eprime derived text file.\n");
    fprintf(stderr,"            Each run is 4 columns: TriggerScreen.RTTime\n");
    fprintf(stderr,"                                   Pause2.OnsetTime||GreenCross.OnsetTime\n");
    fprintf(stderr,"                                   WhiteCross.OnsetTime||WhiteCross1.OnsetTime\n");
    fprintf(stderr,"                                   Condition\n");

    /*START131017*/
    fprintf(stderr," -rr        Makes event file suitable for regional regressors.\n");
    }
for(i=1;i<argc;i++) {

    #if 0
    if(!strcmp(argv[i],"-txt") && argc > i+1)
        txtfile = argv[++i];
    if(!strcmp(argv[i],"-txt3") && argc > i+1)
        txtfile3 = argv[++i];
    if(!strcmp(argv[i],"-txt4") && argc > i+1)
        txtfile4 = argv[++i];
    #endif
    /*START130718*/
    if(!strcmp(argv[i],"-txt") && argc > i+1) {
        txtfile = argv[++i];
        txt=1;
        }
    if(!strcmp(argv[i],"-txt3") && argc > i+1) {
        txtfile = argv[++i];
        txt3=1;
        }
    if(!strcmp(argv[i],"-txt4") && argc > i+1) {
        txtfile = argv[++i];
        txt4=1;
        }

    if(!strcmp(argv[i],"-conc") && argc > i+1)
        concfile = argv[++i];
    if(!strcmp(argv[i],"-TR") && argc > i+1)
        TR = atof(argv[++i]);
    if(!strcmp(argv[i],"-skipsec") && argc > i+1)
        skipsec = atof(argv[++i]);
    if(!strcmp(argv[i],"-fix") && argc > i+1)
        fix = atof(argv[++i]);

    /*START131017*/
    if(!strcmp(argv[i],"-rr"))
        lcrr=1;

    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
i=txt+txt3+txt4;
if(!i||i>1) {
    printf("fidlError: need to specify either -txtfile or -txtfile3 or -txtfile4\n");
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
printf("%s\n%s\nTR=%g skipsec=%g fix=%g\n",txtfile,concfile,TR,skipsec,fix);fflush(stdout);
if(!(bolds=read_conc(concfile))) exit(-1);
if(!(dp=dim_param(bolds->nfiles,bolds->files,SunOS_Linux,0))) exit(-1);
if(!(data=read_data(txtfile,0,0,0,0)))exit(-1);
for(i=0;i<data->nsubjects;i++) {
    for(j=0;j<data->npoints;j++) printf("%g ",data->x[i][j]);
    printf("\n");
    }
if(!(cond=d2int(dp->nfiles,data->nsubjects))) {
    printf("fidlError: Unable to malloc cond\n");
    exit(-1);
    }
if(dp->nfiles>=1) for(i=0;i<data->nsubjects;i++) cond[0][i] = !strcmp(run1[i],"syn") ? 0 : 1; 
if(dp->nfiles>=2) for(i=0;i<data->nsubjects;i++) cond[1][i] = !strcmp(run2[i],"syn") ? 0 : 1; 
if(dp->nfiles==3) for(i=0;i<data->nsubjects;i++) cond[2][i] = !strcmp(run3[i],"syn") ? 0 : 1; 
if(txt||txt3) {
    printf("cond\n");for(i=0;i<dp->nfiles;i++) {
        for(j=0;j<data->nsubjects;j++) printf("%d ",cond[i][j]);
        printf("\n");
        }
    }

#if 0
strcpy(filename,txtfile);
if(get_tail_sans_ext(filename)) exit(-1);
strcat(filename,"_nocue.fidl");
#endif
/*START131017*/
if(!(strptr=get_tail_sans_ext(txtfile))) exit(-1);
sprintf(filename,"%s_nocue%s.fidl",strptr,lcrr?"rr":"");



if(!(fp=fopen_sub(filename,"w"))) exit(-1);
if(txt) {
    printf("TWO COLUMN TXTFILE\n");fflush(stdout);
    if(!lcrr) {
        fprintf(fp,"%g syn math\n",TR);
        for(time=0.,i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR) {
            time2=time+dp->tdim[i]*TR;
            fprintf(fp,"%g\t%d\t%g\n",(time1=time+skipsec),cond[i][0],(data->x[0][i*2+1]-data->x[0][i*2]-skipsec*1000.)/1000.);
            for(j=1;j<data->nsubjects;j++) {
                if((time1=time+(data->x[j-1][i*2+1]-data->x[j-1][i*2]+fix*1000.)/1000.)<time2)
                    fprintf(fp,"%g\t%d\t%g\n",time1,cond[i][j],(data->x[j][i*2+1]-data->x[j-1][i*2+1]-fix*1000.)/1000.);
                }
            }
        }
    else {
        fprintf(fp,"%g syn math fix\n",TR);
        for(time=0.,i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR) {
            time2=time+dp->tdim[i]*TR;
            if(skipsec) fprintf(fp,"%g\t2\t%g\n",time,skipsec);
            fprintf(fp,"%g\t%d\t%g\n",(time1=time+skipsec),cond[i][0],(data->x[0][i*2+1]-data->x[0][i*2]-skipsec*1000.)/1000.+15.);
            for(j=1;j<data->nsubjects;j++)
                if((time1=time+(data->x[j-1][i*2+1]-data->x[j-1][i*2]+fix*1000.)/1000.)<time2)
                    fprintf(fp,"%g\t%d\t%g\n",time1,cond[i][j],time3=(data->x[j][i*2+1]-data->x[j-1][i*2+1])/1000.);
            if((time1+time3)<time2) fprintf(fp,"%g\t2\t%g\n",time1+time3,time2-(time1+time3));
            }
        }

    }
if(txt3) {
    printf("THREE COLUMN TXTFILE\n");fflush(stdout);
    if(!lcrr) { 
        fprintf(fp,"%g syn math\n",TR);
        for(time=0.,i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR) {
            time2=time+dp->tdim[i]*TR;
            for(j=0;j<data->nsubjects;j++)
                if((time1=time+(data->x[j][i*3+1]-data->x[j][i*3])/1000.)<time2)
                    fprintf(fp,"%g\t%d\t%g\n",time1,cond[i][j],(data->x[j][i*3+2]-data->x[j][i*3+1])/1000.);
            }
        }
    else {
        fprintf(fp,"%g syn math fix\n",TR);
        for(time=0.,i1=i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR,i1+=3) {
            time2=time+dp->tdim[i]*TR;
            for(j=0;j<data->nsubjects;j++)
                if((time1=time+(data->x[j][i1+1]-data->x[j][i1])/1000.)<time2) {
                    if(!j&&(time1>time)) fprintf(fp,"%g\t2\t%g\n",time,time1-time);
                    fprintf(fp,"%g\t%d\t%g\n",time1,cond[i][j],time3=j<(data->nsubjects-1)?
                        (data->x[j+1][i1+1]-data->x[j][i1+1])/1000.:((data->x[j][i1+2]-data->x[j][i1+1])/1000.+15.));
                    }
            if((time1+time3)<time2) fprintf(fp,"%g\t2\t%g\n",time1+time3,time2-(time1+time3));
            }
        }
    }
if(txt4) {
    printf("FOUR COLUMN TXTFILE\n");fflush(stdout);
    if(!lcrr) {
        fprintf(fp,"%g abstract sensory number\n",TR);
        for(time=0.,i1=i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR,i1+=4) {
            time2=time+dp->tdim[i]*TR;
            for(j=0;j<data->nsubjects;j++)
                if((time1=time+(data->x[j][i1+1]-data->x[j][i1])/1000.)<time2) {
                    fprintf(fp,"%g\t%g\t%g\n",time1,data->x[j][i1+3],(data->x[j][i1+2]-data->x[j][i1+1])/1000.);
                    }
            }
        }
    else {
        fprintf(fp,"%g abstract sensory number fix\n",TR);
        for(time=0.,i1=i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR,i1+=4) {
            time2=time+dp->tdim[i]*TR;
            for(j=0;j<data->nsubjects;j++)
                if((time1=time+(data->x[j][i1+1]-data->x[j][i1])/1000.)<time2) {
                    if(!j&&(time1>time)) fprintf(fp,"%g\t3\t%g\n",time,time1-time);
                    fprintf(fp,"%g\t%g\t%g\n",time1,data->x[j][i1+3],(time3=j<(data->nsubjects-1)?
                        (data->x[j+1][i1+1]-data->x[j][i1+1])/1000.:((data->x[j][i1+2]-data->x[j][i1+1])/1000.+15.)));
                    }
            if((time1+time3)<time2) fprintf(fp,"%g\t3\t%g\n",time1+time3,time2-(time1+time3));
            }
        }
    }
fclose(fp);fflush(fp);
printf("Event file written to %s\n",filename);fflush(stdout);
}
