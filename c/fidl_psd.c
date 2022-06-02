/* Copyright 6/2/05 Washington University.  All Rights Reserved.
   fidl_psd.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <nrutil.h>

main(int argc,char **argv)
{
char *psd_file=NULL,*xform_file=NULL,*mask_file=NULL;
int i,j,k,l,m,nfiles=0,num_region_files=0,num_regions=0,*roi,nfreq=0,argc_freq,noutput=0,SunOS_Linux,A_or_B_or_U,atlas;
float *t4;
Files_Struct *files,*region_files,*output; /* *freq */
TCnew *freq;

if(argc < 5) {
    fprintf(stderr,"        -files:               4dfp timecourse files.\n");
    fprintf(stderr,"        -freq:                Frequency bands of interest.\n");
    fprintf(stderr,"                              Output is mean squared power of the one sided power spectral density\n");
    fprintf(stderr,"                              in the specified frequency band in decibels (dB).\n");
    fprintf(stderr,"                              Ex. -freq 0.005-0.2\n");
    fprintf(stderr,"        -output:              Each frequency band specified with -freq needs an output name.\n");
    fprintf(stderr,"        -psd:                 Output power spectral density in dB. Provide output name.\n");
    fprintf(stderr,"        -mask:                Only voxels in the mask are analyzed.\n");
    fprintf(stderr,"        -region_file:         4dfp region files.\n");
    fprintf(stderr,"        -regions_of_interest: First region is one.\n");
    fprintf(stderr,"        -xform_file:          t4 file defining the transform to atlas space.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }

    #if 0
    if(!strcmp(argv[i],"-freq") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfreq;
        if(!(freq=get_files(nfreq,&argv[i+1]))) exit(-1);
        i += nfreq;
        }
    #endif
    if(!strcmp(argv[i],"-freq") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfreq;
        argc_freq = i+1;
        i += nfreq;
        }

    if(!strcmp(argv[i],"-output") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++noutput;
        if(!(output=get_files(noutput,&argv[i+1]))) exit(-1);
        i += noutput;
        }
    if(!strcmp(argv[i],"-psd") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        psd_file = argv[++i];
    if(!strcmp(argv[i],"-xform_file") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        xform_file = argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;
        if(!(roi=malloc(sizeof*roi*num_regions))) {
            printf("Error: Unable to malloc roi in fidl_psd\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
        }
    }
if(!nfiles) {
    printf("Error: No timecourse files. Abort!\n");
    exit(-1);
    }
if(!nfreq) {
    printf("Error: No freqeuncy bands specified. Abort!\n");
    exit(-1);
    }
if(!noutput) {
    printf("Error: No output names specified. Abort!\n");
    exit(-1);
    }
if(!num_region_files) {
    if(nfreq != noutput) {
        printf("Error: nfreq=%d noutput=%d. Need to specify an output name for each frequency. Abort!\n");
        exit(-1);
        }
    }
if((SunOS_Linux=checkOS())==-1) return 0;

if(psd_file) {
    if(!(freq=read_tc_string_TCnew(nfreq,(int*)NULL,argc_freq,argv,'-'))) exit(-1);
    }

if(!(dp=(Dim_Param *)dim_param(nfiles,files->files))) exit(-1);
if(xform_file) {
    if(!(t4=malloc(sizeof*t4*(size_t)T4SIZE))) {
        printf("Error: Unable to malloc t4\n");
        exit(-1);
        }
    if(!read_xform(xform_file,t4)) return 0;
    if((A_or_B_or_U=twoA_or_twoB(xform_file)) == 2) return 0;
    atlas = 222;
    }
else {
    atlas = (int)get_atlas(dp->vol);
    }
if(!(ap=get_atlas_param(atlas))) exit(-1);
if(!xform_file) ap->vol = dp->vol;
if(!atlas) {
    ap->xdim = dp->xdim;
    ap->ydim = dp->ydim;
    ap->zdim = dp->zdim;
    ap->voxel_size[0] = dp->dxdy;
    ap->voxel_size[1] = dp->dxdy;
    ap->voxel_size[2] = dp->dz;
    }


}
