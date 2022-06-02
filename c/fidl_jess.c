/* Copyright 10/10/07 Washington University.  All Rights Reserved.
   fidl_jess.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_jess.c,v 1.2 2007/11/13 01:24:11 mcavoy Exp $";

main(int argc,char **argv)
{
char *root=NULL,*seed_filename=NULL,filename[MAXNAME];
int i,j,k,l,m,dim=0,nsubjects=0,nframes=0,nseed=0,vol,size,SunOS_Linux;
float *temp_float,min,max;
double *temp_double,sd;
unsigned short *seed;
FILE *fp;
Interfile_header *ifh;
if(argc<11) {
    fprintf(stderr,"    -dim:          Matrix is dim*dim\n");
    fprintf(stderr,"    -seed:         Three numbers specify the seed. Default is 0 0 0.\n");
    fprintf(stderr,"    -seed seed.dat Read seed from file seed.dat.\n");
    fprintf(stderr,"                   Use to generate white noise data in place of real data.\n");
    fprintf(stderr,"    -root:         Output root.\n");
    fprintf(stderr,"    -nsubjects:    Number of subjects. This many images will be output.\n");
    fprintf(stderr,"    -nframes:      How many frames of data went into the correlation coefficient?\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-dim") && argc > i+1)
        dim = atoi(argv[++i]);
    else if(!strcmp(argv[i],"-seed") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nseed;
        if(!(seed=malloc(sizeof*seed*3))) {
            printf("Error: Unable to malloc seed\n");
            exit(-1);
            }
        seed[0]=0; seed[1]=0; seed[2]=0;
        if(nseed == 1) {
            seed_filename = argv[++i];
            if(!(fp=fopen_sub(seed_filename,"r"))) exit(-1);
            if(fscanf(fp,"%hu %hu %hu",&seed[0],&seed[1],&seed[2]) != 3) {
                printf("Error: Could not read %s\n",seed_filename);
                exit(-1);
                }
            fclose(fp);
            printf("*** Seed read from %s *** seed = %u %u %u\n",seed_filename,seed[0],seed[1],seed[2]);
            }
        else if(nseed == 3)
            for(j=0;j<3;j++) seed[j] = atoi(argv[++i]);
        else {
            printf("Error: Need to specify three seed numbers.\n");
            exit(-1);
            }
        }
    else if(!strcmp(argv[i],"-root") && argc > i+1)
        root = argv[++i];
    else if(!strcmp(argv[i],"-nsubjects") && argc > i+1)
        nsubjects = atoi(argv[++i]);
    else if(!strcmp(argv[i],"-nframes") && argc > i+1)
        nframes = atoi(argv[++i]);
    else {
        printf("Warning: Unknown option %s\n",argv[i]);
        }
    }
if(!dim) {
    printf("Error: Need to specify -dim\n");
    exit(-1);
    }
size=dim*dim;
vol=dim*(dim-1)/2;
if(!nseed) {
    printf("Error: Need to specify -nseed\n");
    exit(-1);
    }
if(!root) {
    printf("Error: Need to specify -root\n");
    exit(-1);
    }
if(!nsubjects) {
    printf("Error: Need to specify -nsubjects\n");
    exit(-1);
    }
if(!nframes) {
    printf("Error: Need to specify -nframes\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
printf("Starting seed = %u %u %u\n",seed[0],seed[1],seed[2]);
if(!(temp_double=malloc(sizeof*temp_double*vol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*size))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<size;i++) temp_float[i]=0.;
if(!(ifh=init_ifh(4,dim,dim,1,1,1,1,1,!SunOS_Linux?1:0))) exit(-1);
/*ifh->number_format = ifh->bytes_per_pix = 8;*/
printf("ifh->smoothness=%g\n",ifh->smoothness);
sd=1./sqrt((double)(nframes-3));
for(i=0;i<nsubjects;i++) {
    nrmlrv_doublestack(temp_double,vol,seed);
    /*for(j=0;j<vol;j++) temp_double[j]*=sd;*/

    for(m=l=j=0;j<dim;j++,m+=j) {
        for(k=j+1;k<dim;k++,l++,m++) {
            /*printf("j=%d k=%d l=%d m=%d\n",j,k,l,m);*/
            temp_float[m]=(float)(temp_double[l]*sd); 
            }
        }
    /*exit(-1);*/

    sprintf(filename,"%s%d.4dfp.img",root,i);
    /*if(!writestack(filename,temp_double,sizeof(double),(size_t)vol,0)) exit(-1);*/
    if(!writestack(filename,temp_float,sizeof(float),(size_t)size,0)) exit(-1);
    if(!write_ifh(filename,ifh,0)) exit(-1);
    /*min_and_max_init(&min,&max);
    min_and_max_doublestack(temp_double,vol,&min,&max);
    printf("min=%f max=%f\n",min,max);*/
    }
if(!seed_filename) {
    if(!(seed_filename=malloc(sizeof*seed_filename*(strlen(root)+10)))) {
        printf("Error: Unable to malloc seed_filename\n");
        exit(-1);
        }
    sprintf(seed_filename,"%s_seed.dat",root);
    }
if(!(fp=fopen_sub(seed_filename,"w"))) exit(-1);
fprintf(fp,"%u %u %u\n",seed[0],seed[1],seed[2]);
fclose(fp);
printf("*** Seed written to %s *** seed = %u %u %u\n",seed_filename,seed[0],seed[1],seed[2]);
exit(0);
}
