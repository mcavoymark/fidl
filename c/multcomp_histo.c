/* Copyright 12/6/00 Washington University.  All Rights Reserved.
   multcomp_histo.c  $Revision: 1.11 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

enum{MAX_NTHRESH = 15,
     MAX_NEXTENT = 35};

main(int argc,char **argv)
{
void print_results(int nfiles,char **argv,int argc_files,float *fwhm,double *scls,int *nthresh,int **nextent,
    double **thresh,int ***extent,int ***count_regions);

char string[MAXNAME],write_back[MAXNAME],*mask_file=(char*)NULL;

int i,j,k,m,argc_files,nseed=0,nruns=0,nfiles=0,print=0,print_count=0,nlines,nstrings,*nthresh,**nextent,***extent,
    ***count_regions_2s,***count_regions_1sp,***count_regions_1sn,SunOS_Linux;

float *fwhm,threshp_float,*temp_float,*act_mask,min,max;

double *scls,**thresh,**threshp,*temp_double,*image /*,*gauss*/;

unsigned short seed[]={0,0,0};
Mask_Struct *ms;
FILE *fp;


if(argc < 5) {
    fprintf(stderr,"Usage: multcomp_histo -runs 10000 -threshold_extent_files thresh/fwhm2.dat thresh/fwhm3.dat thresh/fwhm4.dat -mask_file mask/glm_atlas_mask_222.4dfp.img\n");
    fprintf(stderr,"        -runs:                   Number of runs.\n");
    fprintf(stderr,"        -threshold_extent_files: Example file:\n");
    fprintf(stderr,"                                     FWHM 2  scls 0.175825\n");
    fprintf(stderr,"                                     3       12,13,14,15,16,17,18,19,20\n");
    fprintf(stderr,"                                     3.25    10,11,12,13,14,15,16,17,18\n");
    fprintf(stderr,"                                 First line indicates the smoothing.\n");
    fprintf(stderr,"                                 Remaining lines give the threshold and the comma separated extents.\n");
    fprintf(stderr,"        -mask_file:                   Provide an atlas mask (ie 222 or 333).\n");
    fprintf(stderr,"        -seed:                   3 numbers specify the seed. Default is 0 0 0.\n");
    fprintf(stderr,"        -print:                  Print results every this many runs.\n");
    fprintf(stderr,"                                 Results are only printed at the end if this otption is not used.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-runs") && argc > i+1)
        nruns = atoi(argv[++i]);
    if(!strcmp(argv[i],"-threshold_extent_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        argc_files = i+1;
        i += nfiles;
        }
    if(!strcmp(argv[i],"-mask_file") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-seed") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nseed;
        if(nseed != 3) {
            fprintf(stderr,"Need to specify three seed numbers.\n");
            exit(-1);
            }
        else
            for(j=0;j<3;j++) seed[j] = atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-print") && argc > i+1)
        print = atoi(argv[++i]);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
printf("seed = %d %d %d\n",seed[0],seed[1],seed[2]);
if(!nruns) {
    printf("runs = %d  Need to specify the number of runs with the -runs option.\n",nruns);
    exit(-1);
    }
if(!nfiles) {
    printf("Need to specify a file(s) containing thresholds and extents with the -threshold_extent_files option.\n");
    exit(-1);
    }
if(!mask_file) {
    printf("Need to specify a mask with -mask option.\n");
    exit(-1);
    }
if(!(ms=read_mask(mask_file,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
printf("mask = %s\n",mask_file);

printf("ms->xdim=%d ms->ydim=%d ms->zdim=%d ms->lenvol=%d ms->lenbrain=%d\n",ms->xdim,ms->ydim,ms->zdim,ms->lenvol,ms->lenbrain);
/*printf("ms->brnidx=");for(i=0;i<10;i++)printf("%d ",ms->brnidx[i]);printf("\n");*/

if(!(fwhm=malloc(sizeof*fwhm*nfiles))) {
    printf("Error: Unable to malloc fwhm\n");
    exit(-1);
    }
if(!(scls=malloc(sizeof*scls*nfiles))) {
    printf("Error: Unable to malloc scls\n");
    exit(-1);
    }
if(!(nthresh=malloc(sizeof*nthresh*nfiles))) {
    printf("Error: Unable to malloc nthresh\n");
    exit(-1);
    }
if(!(thresh = (double **)d2double(nfiles,(int)MAX_NTHRESH))) exit(-1);
if(!(threshp = (double **)d2double(nfiles,(int)MAX_NTHRESH))) exit(-1);
if(!(nextent = (int **)d2int(nfiles,(int)MAX_NTHRESH))) exit(-1);
if(!(extent=malloc(sizeof*extent*nfiles))) {
    printf("Error: Unable to malloc extent\n");
    exit(-1);
    }
if(!(count_regions_2s=malloc(sizeof*count_regions_2s*nfiles))) {
    printf("Error: Unable to malloc count_regions\n");
    exit(-1);
    }
if(!(count_regions_1sp=malloc(sizeof*count_regions_1sp*nfiles))) {
    printf("Error: Unable to malloc count_regions_1sp\n");
    exit(-1);
    }
if(!(count_regions_1sn=malloc(sizeof*count_regions_1sn*nfiles))) {
    printf("Error: Unable to malloc count_regions_1sn\n");
    exit(-1);
    }

for(i=0;i<nfiles;i++) {
    if(!(fp=fopen_sub(argv[argc_files+i],"r"))) exit(-1);
    for(nlines=0;fgets(string,sizeof(string),fp);nlines++);
    if(!nlines) {
        fprintf(stdout,"Error: File %s is empty. Abort!\n",argv[argc_files+1]);
        exit(-1);
        }
    if(!(extent[i] = (int **)d2int(nlines-1,(int)MAX_NEXTENT))) exit(-1);
    if(!(count_regions_2s[i] = (int **)d2int(nlines-1,(int)MAX_NEXTENT))) exit(-1);
    if(!(count_regions_1sp[i] = (int **)d2int(nlines-1,(int)MAX_NEXTENT))) exit(-1);
    if(!(count_regions_1sn[i] = (int **)d2int(nlines-1,(int)MAX_NEXTENT))) exit(-1);
    nthresh[i] = nlines - 1;
    rewind(fp);
    fscanf(fp,"%s %f %s %lf",&string,&fwhm[i],&write_back,&scls[i]);
    for(j=0;j<nlines-1;j++) {
        fscanf(fp,"%lf %s",&thresh[i][j],&string);
        if((nstrings=count_strings(string,write_back,' ')) < 1) {
            fprintf(stdout,"Error: %s Line %d has %d data points. Should have at least 2 data points.\n",
                argv[argc_files+1],j+2,nstrings+1);
            exit(-1);
            }
        nextent[i][j] = nstrings;
        strings_to_int(write_back,extent[i][j],nstrings);
        }
    fclose(fp);
    }

for(i=0;i<nfiles;i++) {
    if(!scls[i]) {
        fprintf(stdout,"Error: scls[%d] = %f  Should be nonzero.\n",i,scls[i]);
        exit(-1);
        }
    }
if(!(temp_double=malloc(sizeof*temp_double*ms->lenvol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(image=malloc(sizeof*image*ms->lenvol))) {
    printf("Error: Unable to malloc image\n");
    exit(-1);
    }
for(i=0;i<ms->lenvol;i++) image[i] = 0.;
if(!(temp_float=malloc(sizeof*temp_float*ms->lenvol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<ms->lenvol;i++) temp_float[i] = 0.;
if(!(act_mask=malloc(sizeof*act_mask*ms->lenvol))) {
    printf("Error: Unable to malloc act_mask\n");
    exit(-1);
    }
for(i=0;i<ms->lenvol;i++) act_mask[i] = 0.;

for(i=0;i<nruns;i++) {
    /*printf("i=%d\n",i);*/
    nrmlrv_doublestack(temp_double,ms->lenbrain,seed);
    for(j=0;j<ms->lenbrain;j++) image[ms->brnidx[j]] = temp_double[j];
    for(j=0;j<nfiles;j++) {


        #if 0
        if(!gauss_smoth(image,temp_double,ms->xdim,ms->ydim,ms->zdim,fwhm[j],fwhm[j])) exit(-1);
        for(k=0;k<ms->lenvol;k++) temp_double[k] /= scls[j];
        #endif
        if(fwhm[j]>0.) {
            if(!gauss_smoth(image,temp_double,ms->xdim,ms->ydim,ms->zdim,fwhm[j],fwhm[j])) exit(-1);
            for(k=0;k<ms->lenvol;k++) temp_double[k] /= scls[j];
            }
        else {
            for(k=0;k<ms->lenvol;k++) temp_double[k] = image[k];
            }


        for(k=0;k<nthresh[j];k++) { /*two sided*/
            for(m=0;m<ms->lenvol;m++) temp_float[m] = (float)temp_double[m];
            threshp_float = (float)thresh[j][k];
            for(m=0;m<nextent[j][k];m++) {
                count_regions_2s[j][k][m] += spatial_extent(temp_float,act_mask,ms->xdim,ms->ydim,ms->zdim,&threshp_float,
                    1,&extent[j][k][m],&ms->lenvol,1);
                }
            }
        for(k=0;k<nthresh[j];k++) { /*one sided positive*/
            for(m=0;m<ms->lenvol;m++) temp_float[m] = temp_double[m] < thresh[j][k] ? 0. : (float)temp_double[m];
            threshp_float = (float)thresh[j][k];
            for(m=0;m<nextent[j][k];m++) {
                count_regions_1sp[j][k][m] += spatial_extent(temp_float,act_mask,ms->xdim,ms->ydim,ms->zdim,&threshp_float,
                    1,&extent[j][k][m],&ms->lenvol,1);
                }
            }
        for(k=0;k<nthresh[j];k++) { /*one sided negative*/
            for(m=0;m<ms->lenvol;m++) temp_float[m] = temp_double[m] < -1*thresh[j][k] ? (float)temp_double[m] : 0.;
            threshp_float = (float)thresh[j][k];
            for(m=0;m<nextent[j][k];m++) {
                count_regions_1sn[j][k][m] += spatial_extent(temp_float,act_mask,ms->xdim,ms->ydim,ms->zdim,&threshp_float,
                    1,&extent[j][k][m],&ms->lenvol,1);
                }
            }

        }
    if(++print_count == (int)print) {
        print_count = 0;
        printf("two sided  RUNS = %d\n",i+1);
        print_results(nfiles,argv,argc_files,fwhm,scls,nthresh,nextent,thresh,extent,count_regions_2s);
        }
    if(++print_count == (int)print) {
        print_count = 0;
        printf("one sided positive  RUNS = %d\n",i+1);
        print_results(nfiles,argv,argc_files,fwhm,scls,nthresh,nextent,thresh,extent,count_regions_1sp);
        }
    if(++print_count == (int)print) {
        print_count = 0;
        printf("one sided negative  RUNS = %d\n",i+1);
        print_results(nfiles,argv,argc_files,fwhm,scls,nthresh,nextent,thresh,extent,count_regions_1sn);
        }
    }
printf("two sided  RUNS = %d\n",nruns);
print_results(nfiles,argv,argc_files,fwhm,scls,nthresh,nextent,thresh,extent,count_regions_2s);
printf("one sided positive  RUNS = %d\n",nruns);
print_results(nfiles,argv,argc_files,fwhm,scls,nthresh,nextent,thresh,extent,count_regions_1sp);
printf("one sided negative  RUNS = %d\n",nruns);
print_results(nfiles,argv,argc_files,fwhm,scls,nthresh,nextent,thresh,extent,count_regions_1sn);


printf("seed = %d %d %d\n",seed[0],seed[1],seed[2]);
}

void print_results(int nfiles,char **argv,int argc_files,float *fwhm,double *scls,int *nthresh,int **nextent,
    double **thresh,int ***extent,int ***count_regions)
{
    int i,j,k;

    for(i=0;i<nfiles;i++) {
        printf("%s fwhm=%f scls=%f nthresh=%d\n",argv[argc_files+i],fwhm[i],scls[i],nthresh[i]);
        for(j=0;j<nthresh[i];j++) {
            for(k=0;k<nextent[i][j];k++) {
                printf("threshold = %.2f\tnumber of regions with %d or more voxels = %d\n",thresh[i][j],extent[i][j][k],
                    count_regions[i][j][k]);
                }
            }
        }
}
