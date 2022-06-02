/* Copyright 2/21/13 Washington University.  All Rights Reserved.
   fidl_language_syn_ev.c  $Revision: 1.3 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_language_syn_ev.c,v 1.3 2013/05/01 20:06:36 mcavoy Exp $";
main(int argc,char **argv)
{
char *run1[]={"math","syn","math","math","syn","syn","math","syn","syn","math"}; /*cue=0 syn=1 math==2*/
char *run2[]={"syn","syn","math","math","syn","math","math","syn","syn","math"};
char *run3[]={"math","syn","math","syn","syn","math","syn","math","math","syn"};
char *txtfile=NULL,*concfile=NULL,filename[MAXNAME];
int i,j,SunOS_Linux,**cond,nocue=0;
double TR=0.,skipsec=0.,fix=0.,cueduration=0.,time,time1;
Files_Struct *bolds;
Dim_Param *dp;
Data *data;
FILE *fp;
if(argc<11) {
    fprintf(stderr,"This is for synonym runs.\n");
    fprintf(stderr,"  -txt:          Name of eprime derived text file.\n");
    fprintf(stderr,"                 Each run is 1 column. That column includes the duration of each block.\n");
    fprintf(stderr,"  -conc:         Conc file.\n");
    fprintf(stderr,"  -TR:           Sampling rate (s).\n");
    fprintf(stderr,"  -skipsec:      Initial fixation at the begining of each run (s). Default is 0.\n");
    fprintf(stderr,"                 Runs for vc36516 began with 15s of fix.\n");
    fprintf(stderr,"  -fix:          Length of fixation (s) between blocks.\n");
    fprintf(stderr,"                 vc36516 had 15s of fix between blocks.\n");
    fprintf(stderr,"  -cueduration:  How long (s) the green crosshair was on.\n");
    fprintf(stderr,"                 For vc36516 the cue was on for 2.5s.\n");

    /*START130430*/
    fprintf(stderr,"  -nocue         Blocks will start on the cue with the duration increased by the duration of the cue.\n");

    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-txt") && argc > i+1)
        txtfile = argv[++i];
    if(!strcmp(argv[i],"-conc") && argc > i+1)
        concfile = argv[++i];
    if(!strcmp(argv[i],"-TR") && argc > i+1)
        TR = atof(argv[++i]);
    if(!strcmp(argv[i],"-skipsec") && argc > i+1)
        skipsec = atof(argv[++i]);
    if(!strcmp(argv[i],"-fix") && argc > i+1)
        fix = atof(argv[++i]);
    if(!strcmp(argv[i],"-cueduration") && argc > i+1)
        cueduration = atof(argv[++i]);

    /*START130430*/
    if(!strcmp(argv[i],"-nocue"))
        nocue = 1;

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
if(cueduration<=0.) {
    printf("fidlError: Need to specify -duration\n");
    exit(-1);
    }
printf("%s\n%s\nTR=%g skipsec=%g fix=%g cueduration=%g\n",txtfile,concfile,TR,skipsec,fix,cueduration);fflush(stdout);
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


#if 0
if(dp->nfiles>=1) for(i=0;i<data->nsubjects;i++) cond[0][i] = !strcmp(run1[i],"syn") ? 1 : 2; 
if(dp->nfiles>=2) for(i=0;i<data->nsubjects;i++) cond[1][i] = !strcmp(run2[i],"syn") ? 1 : 2; 
if(dp->nfiles==3) for(i=0;i<data->nsubjects;i++) cond[2][i] = !strcmp(run3[i],"syn") ? 1 : 2; 
#endif
/*START130430*/
if(dp->nfiles>=1) for(i=0;i<data->nsubjects;i++) cond[0][i] = (!strcmp(run1[i],"syn") ? 1 : 2)-nocue; 
if(dp->nfiles>=2) for(i=0;i<data->nsubjects;i++) cond[1][i] = (!strcmp(run2[i],"syn") ? 1 : 2)-nocue; 
if(dp->nfiles==3) for(i=0;i<data->nsubjects;i++) cond[2][i] = (!strcmp(run3[i],"syn") ? 1 : 2)-nocue; 


strcpy(filename,txtfile);
if(!get_tail_sans_ext(filename)) exit(-1);

/*START130430*/
if(nocue) strcat(filename,"_nocue"); 

strcat(filename,".fidl");    
if(!(fp=fopen_sub(filename,"w"))) exit(-1);

#if 0
fprintf(fp,"%g cue syn math\n",TR);
for(time=0.,i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR) {
    for(time1=time+skipsec,j=0;j<data->nsubjects;time1+=data->x[j++][i]+fix) {
        fprintf(fp,"%g\t0\t%g\n",time1,cueduration); 
        time1+=cueduration;
        fprintf(fp,"%g\t%d\t%g\n",time1,cond[i][j],data->x[j][i]); 
        }
    }
#endif
/*START130430*/
if(!nocue) {
    fprintf(fp,"%g cue syn math\n",TR);
    for(time=0.,i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR) {
        for(time1=time+skipsec,j=0;j<data->nsubjects;time1+=data->x[j++][i]+fix) {
            fprintf(fp,"%g\t0\t%g\n",time1,cueduration);
            time1+=cueduration;
            fprintf(fp,"%g\t%d\t%g\n",time1,cond[i][j],data->x[j][i]);
            }
        }
    }
else {
    fprintf(fp,"%g syn math\n",TR);
    for(time=0.,i=0;i<dp->nfiles;time+=dp->tdim[i++]*TR) {
        for(time1=time+skipsec,j=0;j<data->nsubjects;time1+=data->x[j++][i]+cueduration+fix) {
            fprintf(fp,"%g\t%d\t%g\n",time1,cond[i][j],data->x[j][i]+cueduration);
            }
        }
    }

fclose(fp);fflush(fp);
printf("Event file written to %s\n",filename);fflush(stdout);
}
