/* Copyright 3/02/12 Washington University.  All Rights Reserved.
   fidl_motion.c  $Revision: 1.1 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fidl.h>
/*static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_eog.c,v 1.2 2012/03/27 20:30:49 mcavoy Exp $";*/
int main(int argc,char **argv)
{
char *valsf=NULL,*outf=NULL,*strptr,filename[MAXNAME];
int nboldf=0,contiguousframes=1,SunOS_Linux,cnt;
size_t i,j,k,i1,k1;
double dvarsthresh=1000.;
Files_Struct *boldf=NULL;
Data *data;
Dim_Param *dp;
FILE *fp;
/*print_version_number(rcsid,stdout);*/
if(argc<3) {
    fprintf(stderr,"  -vals:             vals file, a single column of numbers. One for each frame in the conc.\n");
    fprintf(stderr,"  -dvarsthresh:      val must be less than or equal for frame to be retained.\n");
    fprintf(stderr,"                     Default is all frames retained.\n");
    fprintf(stderr,"  -bold:             imgs or concs. Used to determine run boundaries\n");
    fprintf(stderr,"  -contiguousframes: This many contiguous frames for retention. Default is 1.\n");
    fprintf(stderr,"  -out:              Name of output file. Default is <vals file>_scrub.txt\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-vals") && argc > i+1) valsf = argv[++i];
    if(!strcmp(argv[i],"-dvarsthresh") && argc > i+1) dvarsthresh = atof(argv[++i]);
    if(!strcmp(argv[i],"-bold") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nboldf;
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")) {
            if(!(boldf=read_conc(nboldf,&argv[i+1]))) exit(-1);
            }
        else if(!strcmp(strptr,".img")){
            if(!(boldf=get_files(nboldf,&argv[i+1]))) exit(-1);
            }
        else {
            printf("Error: -bold not concs or imgs. Abort!\n");fflush(stdout);
            exit(-1);
            }
        i+=nboldf;
        }
    if(!strcmp(argv[i],"-contiguousframes") && argc > i+1) contiguousframes = atoi(argv[++i]);
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) outf = argv[++i];
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!valsf) {printf("fidlError: Need to specify a vals file with -vals. Abort!\n");fflush(stdout); exit(-1);}
if(!nboldf) {printf("fidlError: Need to specify concs or imgs with -bold. Abort!\n");fflush(stdout); exit(-1);}
printf("%s\ndvarsthresh=%f contiguousframes=%d\n",valsf,dvarsthresh,contiguousframes);fflush(stdout);
if(!(dp=dim_param(boldf->nfiles,boldf->files,SunOS_Linux,0))) exit(-1);
if(!(data=read_data(valsf,0,0,1,0)))exit(-1);
if(dp->tdim_total!=data->nsubjects) {
    printf("fidlError: dp->tdim_total=%d data->nsubjects=%d Must be equal.\n",dp->tdim_total,data->nsubjects);
    fflush(stdout);exit(-1);
    }
for(i=0;i<data->nsubjects-1;i++) data->x[i][0] = data->x[i][0]<=dvarsthresh ? 1. : 0.;
for(i1=i=0;i<dp->nfiles;i++) {

    printf("here3 dp->tdim[%d]=%d i1=%d\n",i,dp->tdim[i],i1);fflush(stdout);

    for(j=0;j<dp->tdim[i];j++,i1++) {

        printf("    here4 dp->tdim[%d]=%d j=%d data->x[%d][0]=%f\n",i,dp->tdim[i],j,i1,data->x[i1][0]);fflush(stdout);

        if(data->x[i1][0]) {

            #if 0
            for(k1=i1;cnt=0,data->x[i1][0]&&j<dp->tdim[i];j++,i1++,cnt++); /* cnt--; */
            #endif
            for(k1=i1,cnt=0;data->x[i1][0]&&j<dp->tdim[i];j++,i1++,cnt++) {
                printf("        here5 data->x[%d][0]=%f\n",i1,data->x[i1][0]);fflush(stdout);
                } 

            printf("            here6 cnt=%d j=%d i1=%d\n",cnt,j,i1);fflush(stdout);
            if(cnt<contiguousframes&&j<dp->tdim[i]) for(i1=k1,k=0;k<cnt;k++,i1++) data->x[i1][0]=0.;
            i1--;
            }
        }
    }
if(!outf) {
    strcpy(filename,valsf);
    if(!(outf=get_tail_sans_ext(filename))) exit(-1);
    strcat(outf,"_scrub.txt");
    }
if(!(fp=fopen_sub(outf,"w"))) exit(-1);
for(cnt=i=0;i<data->nsubjects;i++) {
    fprintf(fp,"%g\n",data->x[i][0]);
    if(data->x[i][0]) cnt++;
    }
fflush(fp);fclose(fp);
printf("%s\n    %d retained frames, %.2f%%",valsf,cnt,(double)cnt/(double)data->nsubjects*100.);
printf("    Output written to %s\n",outf);
}
