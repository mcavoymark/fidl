/* Copyright 9/11/12 Washington University.  All Rights Reserved.
   fidl_randskew.c  $Revision: 1.2 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_randskew.c,v 1.2 2012/09/21 17:29:43 mcavoy Exp $";
void randskew(double *stack,int size,unsigned short *seed,double alpha,double location,double scale);
main(int argc,char **argv)
{
char *strptr,*root="randskew",*seed_filename=NULL,*maskfile=NULL,filename[MAXNAME];
size_t i,j,nseed=0,start=0,nimages=1;
int SunOS_Linux;
unsigned short seed[]={0,0,0};
float fwhm=0.,*temp_float;
double alpha=0.,location=0.,scale=1.,*temp_double,*stat,*image,kernel;
Mask_Struct *ms;
FILE *fp;
Interfile_header *ifh;
if(argc<3) {
    fprintf(stderr,"    -alpha:        Shape parameter. Default is alpha=0 (ie normal distribution).\n");
    fprintf(stderr,"    -location:     Location parameter. Default is 0.\n");
    fprintf(stderr,"    -scale:        Scale paramater. Default is 1.\n");
    fprintf(stderr,"    Note alpha=0 with location=0 and scale=1, produces standard normal deviates.\n");
    fprintf(stderr,"    -root:         Output root. Default is 'randskew'.\n");
    fprintf(stderr,"    -start:        Starting number. Default is 0.\n");
    fprintf(stderr,"    -images:       Number of images to be created. Default is 1.\n");
    fprintf(stderr,"    -seed:         Three positive numbers specify the seed. Default is 0 0 0.\n");
    fprintf(stderr,"     or\n");
    fprintf(stderr,"    -seed seed.dat Read seed from file seed.dat.\n");
    fprintf(stderr,"    -mask:         Provides image and voxel sizes. Also may limit values to a brain mask.\n");
    fprintf(stderr,"    -gauss_smoth:  Amount of gaussian smoothing in units of voxels.\n");
    exit(-1);
    }
print_version_number(rcsid,stdout);
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-alpha") && argc > i+1)
        alpha = atof(argv[++i]);
    if(!strcmp(argv[i],"-location") && argc > i+1)
        location = atof(argv[++i]);
    if(!strcmp(argv[i],"-scale") && argc > i+1)
        scale = atof(argv[++i]);
    if(!strcmp(argv[i],"-root") && argc > i+1) {
        root = argv[++i];
        if((strptr=strstr(root,".4dfp.img"))) *strptr = 0;
        }
    if(!strcmp(argv[i],"-start") && argc > i+1)
        start = atoi(argv[++i]);
    if(!strcmp(argv[i],"-images") && argc > i+1)
        nimages = atoi(argv[++i]);
    if(!strcmp(argv[i],"-seed") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nseed;
        if(nseed == 1) {
            if(!(fp=fopen_sub(seed_filename=argv[++i],"r"))) exit(-1);
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
            printf("Error: Need to specify three seed numbers or a single filename.\n");
            exit(-1);
            }
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        maskfile = argv[++i];
    if(!strcmp(argv[i],"-gauss_smoth") && argc > i+1)
        fwhm = atof(argv[++i]);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!maskfile) {printf("fidlError: Need to specify mask file with -mask\n");fflush(stdout);exit(-1);}
printf("alpha= %f location= %f scale= %f\nseed= %d %d %d\nfwhm=%f\nmask=%s\n",alpha,location,scale,seed[0],seed[1],seed[2],fwhm,
    maskfile);
if(!(ms=get_mask_struct(maskfile,0,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,0))) exit(-1);

if(!(temp_double=malloc(sizeof*temp_double*ms->lenvol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(stat=malloc(sizeof*stat*ms->lenvol))) {
    printf("Error: Unable to malloc stat\n");
    exit(-1);
    }
if(!(image=malloc(sizeof*image*ms->lenvol))) {
    printf("Error: Unable to malloc image\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*ms->lenvol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(fwhm>0.) {
    for(i=0;i<ms->lenvol;i++) temp_double[i]=0.;
    temp_double[(size_t)(ms->zdim/2.*ms->xdim*ms->ydim + ms->ydim/2.*ms->xdim + ms->xdim/2.)] = 1.;
    if(!gauss_smoth(temp_double,stat,ms->xdim,ms->ydim,ms->zdim,fwhm,fwhm)) exit(-1);
    for(kernel=0.,i=0;i<ms->lenvol;i++) kernel += temp_double[i]*temp_double[i];
    kernel = sqrt(kernel);
    printf("kernel= %f\n",kernel);fflush(stdout);
    }
if(!(ifh=init_ifh(4,ms->xdim,ms->ydim,ms->zdim,1,ms->voxel_size_1,ms->voxel_size_2,ms->voxel_size_3,SunOS_Linux?0:1))) exit(-1);
for(i=0;i<ms->lenvol;i++) temp_float[i]=0.;
if(ms->lenbrain<ms->lenvol) for(i=0;i<ms->lenvol;i++) stat[i]=0.;
for(i=0;i<nimages;i++) {
    randskew(temp_double,ms->lenbrain,seed,alpha,location,scale);
    if(ms->lenbrain<ms->lenvol) for(j=0;j<ms->lenbrain;j++) stat[ms->brnidx[j]]=temp_double[j]; else stat=temp_double;
    if(fwhm>0.) {
        if(!gauss_smoth(stat,image,ms->xdim,ms->ydim,ms->zdim,fwhm,fwhm)) exit(-1);
        for(j=0;j<ms->lenbrain;j++) temp_float[ms->brnidx[j]] = (float)(image[ms->brnidx[j]]/kernel);
        }
    else {
        for(j=0;j<ms->lenbrain;j++) temp_float[ms->brnidx[j]] = (float)stat[ms->brnidx[j]];
        }
   
    /*sprintf(filename,"%s%s%d.4dfp.img",root,i<10?"00":(i<100?"0":""),i);*/
    sprintf(filename,"%s%04d.4dfp.img",root,i);

    if(!writestack(filename,temp_float,sizeof*temp_float,(size_t)ms->lenvol,0)) exit(-1);

    ifh->nextseed = seed;
    /*ifh->seed[0]=seed[0];ifh->seed[1]=seed[1];ifh->seed[2]=seed[2];*/

    if(!write_ifh(filename,ifh,0)) exit(-1);
    printf("Output written to %s\n",filename);fflush(stdout); 
    }
printf("seed = %hu %hu %hu\n",seed[0],seed[1],seed[2]);fflush(stdout);
}
void randskew(double *stack,int size,unsigned short *seed,double alpha,double location,double scale)
{
size_t i;
double  r,v1,v2,fac,u0,v,sigma,u1;
sigma = alpha / sqrt(1. + alpha*alpha);
for(i=0;i<size;i++) {
    do {
        v1 = 2.*erand48(seed) - 1.;
        v2 = 2.*erand48(seed) - 1.;
        r = v1*v1 + v2*v2;
        } while(r >= 1.);
    fac = sqrt(-2.*log(r)/r);
    u0 = v1*fac;
    v = v2*fac;
    u1 = sigma*u0 + sqrt(1.-sigma*sigma)*v;
    stack[i] = (u1 >= 0. ? u1 : -u1)*scale + location;
    }
}
