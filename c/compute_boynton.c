/* Copyright 5/24/02 Washington University.  All Rights Reserved.
   compute_boynton.c  $Revision: 1.10 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_boynton.c,v 1.10 2015/04/02 20:04:13 mcavoy Exp $";
main(int argc,char **argv)
{
char *output_file=NULL;
int i,j,k,m,num_timepoints=0,scaling=0,lc_column=0,lcprintextra=1,decimal=2;
float init_delay_frames=0,stimlen_frames=0,max,min;
double *time,*regressor,*regressor_impulse,hrf_delta=HRF_DELTA,hrf_tau=HRF_TAU,TR,shift_TR=0.,stimlen_sec=0.,init_delay_sec=0.;
FILE *fp;
Hrfparam param;

if(argc < 11) {
    fprintf(stderr,"        -hrf_delta:             The delay of the HRF in seconds. Default is 2 seconds\n");
    fprintf(stderr,"        -hrf_tau:               The time constant of the HRF in seconds. Default is 1.25 seconds.\n");
    fprintf(stderr,"        -TR:                    The sampling rate in seconds.\n");
    fprintf(stderr,"        -num_timepoints:        The length of the hemodynamic response.\n");
    fprintf(stderr,"        -initial_delay_frames:  Time-shift from hrf_delta. Default is 0 frames.\n");
    fprintf(stderr,"        -initial_delay_seconds: Time-shift from hrf_delta. Default is 0 seconds.\n");
    fprintf(stderr,"        -stimlen_frames:        The expected duration of neuronal firing in frames.\n");
    fprintf(stderr,"        -stimlen_seconds:       The expected duration of neuronal firing in seconds.\n");
    fprintf(stderr,"        -output:                Output filename that parameters and HRF are written to.\n");
    fprintf(stderr,"        -output HRF_ONLY        Only print the HRF.\n");
    fprintf(stderr,"        -column                 Output in column format.\n");
    fprintf(stderr,"        -unscaled               HRF is not scaled. Default.\n");
    fprintf(stderr,"        -max1                   Normalize HRF to have a maximum of 1.\n");
    fprintf(stderr,"        -magnorm                Normalize HRF to magnitude of 1.\n");
    fprintf(stderr,"        -shift_TR:              Fraction of shift from beginning of TR. 0 < shift_TR < 1\n");

    /*START120308*/
    fprintf(stderr,"        -decimal:               Number of decimal points to output in column format. Default is 2.\n");


    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-hrf_delta")) {
        if(argc > i+1) {
            k = strlen(argv[i+1]);
            for(m=j=0;j<k;j++) if(isdigit(argv[i+1][j])) m = 1;
            if(m) hrf_delta = atof(argv[++i]);
            }
        }
    if(!strcmp(argv[i],"-hrf_tau")) {
        if(argc > i+1) {
            k = strlen(argv[i+1]);
            for(m=j=0;j<k;j++) if(isdigit(argv[i+1][j])) m = 1;
            if(m) hrf_tau = atof(argv[++i]);
            }
        }
    if(!strcmp(argv[i],"-TR")) {
        if(argc > i+1) {
            k = strlen(argv[i+1]);
            for(m=j=0;j<k;j++) if(isdigit(argv[i+1][j])) m = 1;
            if(m) TR = atof(argv[++i]);
            }
        }
    if(!strcmp(argv[i],"-num_timepoints") && argc > i+1)
        num_timepoints = atoi(argv[++i]);
    if(!strcmp(argv[i],"-initial_delay_frames")) {
        if(argc > i+1) {
            k = strlen(argv[i+1]);
            for(m=j=0;j<k;j++) if(isdigit(argv[i+1][j])) m = 1;
            if(m) init_delay_frames = atof(argv[++i]);
            }
        }
    if(!strcmp(argv[i],"-initial_delay_seconds")) {
        if(argc > i+1) {
            k = strlen(argv[i+1]);
            for(m=j=0;j<k;j++) if(isdigit(argv[i+1][j])) m = 1;
            if(m) init_delay_sec = atof(argv[++i]);
            }
        }
    if(!strcmp(argv[i],"-stimlen_frames")) {
        if(argc > i+1) {
            k = strlen(argv[i+1]);
            for(m=j=0;j<k;j++) if(isdigit(argv[i+1][j])) m = 1;
            if(m) stimlen_frames = atof(argv[++i]);
            }
        }
    if(!strcmp(argv[i],"-stimlen_seconds")) {
        if(argc > i+1) {
            k = strlen(argv[i+1]);
            for(m=j=0;j<k;j++) if(isdigit(argv[i+1][j])) m = 1;
            if(m) stimlen_sec = atof(argv[++i]);
            }
        }
    if(!strcmp(argv[i],"-output") && argc > i+1) {
        if(!strcmp(argv[++i],"HRF_ONLY")) {
            lcprintextra = 0; 
            }
        else {
            output_file = argv[i];
            }
        }
    if(!strcmp(argv[i],"-column"))
        lc_column = 1;
    if(!strcmp(argv[i],"-unscaled"))
        scaling = 0;
    if(!strcmp(argv[i],"-max1"))
        scaling = 1;
    if(!strcmp(argv[i],"-magnorm"))
        scaling = 2;
    if(!strcmp(argv[i],"-shift_TR") && argc > i+1)
        shift_TR = atof(argv[++i]);

    /*START120308*/
    if(!strcmp(argv[i],"-decimal") && argc > i+1)
        decimal = atoi(argv[++i]);

    }
if(!num_timepoints) {
    printf("You failed to specify the number of timepoints. Abort!\n");
    exit(-1);
    }
if(!TR) {
    printf("You failed to specify the TR. Abort!\n");
    exit(-1);
    }
if(hrf_tau < 0.) {
    printf("hrf_tau = %f. Needs to be >= 0. Abort!\n",hrf_tau);
    exit(-1);
    }
/*fprintf(stderr,"0=unscaled 1=max1 2=magnorm  scaling = %d\n",scaling);*/
if(init_delay_frames) init_delay_sec = init_delay_frames * TR;
if(stimlen_frames > 0.) stimlen_sec = stimlen_frames * TR;
if(stimlen_sec < 0.) {
    printf("stimlen_sec = %f. Needs to be >= 0. Abort!\n",stimlen_sec);
    exit(-1);
    }

#if 0
param->delta = hrf_delta;
param->tau = hrf_tau;
param->A = (double)HRF_A;
param->C = (double)HRF_C;
param->duration = stimlen_sec;
#endif
param.delta = hrf_delta;
param.tau = hrf_tau;
param.A = (double)HRF_A;
param.C = (double)HRF_C;
param.duration = stimlen_sec;




if(!(time=malloc(sizeof*time*num_timepoints))) {
    printf("Error: Unable to malloc time\n");
    exit(-1);
    }
if(!(regressor=malloc(sizeof*regressor*num_timepoints))) {
    printf("Error: Unable to malloc regressor\n");
    exit(-1);
    }
for(m=0;m<num_timepoints;m++) if((time[m] = (double)(TR*(m+shift_TR) - init_delay_sec)) < 0.) time[m] = 0;

/*boynton_model(num_timepoints,time,param,0,regressor);*/
boynton_model(num_timepoints,time,&param,0,regressor);

if(scaling == 1) {
    min_and_max_init(&min,&max);
    min_and_max_doublestack(regressor,num_timepoints,&min,&max);
    for(m=0;m<num_timepoints;m++) regressor[m] /= max;
    }
else if(scaling == 2) {
    for(max=m=0;m<num_timepoints;m++) max += regressor[m];
    max /= (float)num_timepoints;
    for(m=0;m<num_timepoints;m++) regressor[m] -= max;
    for(max=m=0;m<num_timepoints;m++) max += regressor[m]*regressor[m];
    max = (float)sqrt((double)max);
    for(m=0;m<num_timepoints;m++) regressor[m] /= max;
    }
if(stimlen_sec < 0.5) {
    if(!(regressor_impulse=malloc(sizeof*regressor_impulse*num_timepoints))) {
        printf("Error: Unable to malloc regressor_impulse\n");
        exit(-1);
        }

    /*boynton_model(num_timepoints,time,param,1,regressor_impulse);*/
    boynton_model(num_timepoints,time,&param,1,regressor_impulse);

    if(scaling == 1) {
        min_and_max_init(&min,&max);
        min_and_max_doublestack(regressor_impulse,num_timepoints,&min,&max);
        for(m=0;m<num_timepoints;m++) regressor_impulse[m] /= max;
        }
    else if(scaling == 2) {
        for(max=m=0;m<num_timepoints;m++) max += regressor_impulse[m];
        max /= (float)num_timepoints;
        for(m=0;m<num_timepoints;m++) regressor_impulse[m] -= max;
        for(max=m=0;m<num_timepoints;m++) max += regressor_impulse[m]*regressor_impulse[m];
        max = (float)sqrt((double)max);
        for(m=0;m<num_timepoints;m++) regressor_impulse[m] /= max;
        }
    }





if(lcprintextra) {
    print_version_number(rcsid,stderr);
    printf("TR = %f s\nhrf_delta = %f s\nhrf_tau = %f s\ninitial_delay = %f s\nduration = %f s\n\n",TR,hrf_delta,hrf_tau,
        init_delay_sec,stimlen_sec);
    }
if(!lc_column) {
    printf("Convolution Model\n-----------------\n");
    printf("HRF = "); for(m=0;m<num_timepoints;m++) printf("%.4f ",regressor[m]); printf("\n");
    if(stimlen_sec < 0.5) {
        printf("\nImpulse Model\n-----------------\n");
        printf("HRF = "); for(m=0;m<num_timepoints;m++) printf("%.4f ",regressor_impulse[m]); printf("\n");
        }
    }
else {
    if(stimlen_sec < 0.5) {
        for(m=0;m<num_timepoints;m++) printf("%.*f\n",lcprintextra?2:6,regressor_impulse[m]);
        }
    else {
        for(m=0;m<num_timepoints;m++) printf("%.*f\n",lcprintextra?2:6,regressor[m]);
        }

    }
if(lcprintextra) printf("\n\n");

if(output_file) {
    if(!(fp = fopen_sub(output_file,"a"))) exit(-1);
    fprintf(fp,"TR = %f s\nhrf_delta = %f s\nhrf_tau = %f s\ninitial_delay = %f s\nduration = %f s\n\n",TR,hrf_delta,hrf_tau,
        init_delay_sec,stimlen_sec);
    if(!lc_column) {
        fprintf(fp,"Convolution Model\n-----------------\n");
        fprintf(fp,"HRF = "); for(m=0;m<num_timepoints;m++) fprintf(fp,"%.4f ",regressor[m]); fprintf(fp,"\n");
        if(stimlen_sec < 0.5) {
            fprintf(fp,"\nImpulse Model\n-----------------\n");
            fprintf(fp,"HRF = "); for(m=0;m<num_timepoints;m++) fprintf(fp,"%.4f ",regressor_impulse[m]); fprintf(fp,"\n");
            }
        }
    else {

        #if 0
        if(stimlen_sec < 0.5) {
            for(m=0;m<num_timepoints;m++) fprintf(fp,"%.2f\n",regressor_impulse[m]);
            }
        else {
            for(m=0;m<num_timepoints;m++) fprintf(fp,"%.2f\n",regressor[m]);
            }
        #endif
        /*START120308*/
        if(stimlen_sec < 0.5)
            for(m=0;m<num_timepoints;m++) fprintf(fp,"%.*f\n",decimal,regressor_impulse[m]);
        else 
            for(m=0;m<num_timepoints;m++) fprintf(fp,"%.*f\n",decimal,regressor[m]);

        }
    fprintf(fp,"\n\n");
    fclose(fp);
    }
}
