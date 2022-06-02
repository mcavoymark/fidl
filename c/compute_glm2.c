/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   compute_glm2.c  $Revision: 1.23 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "fidl.h"
#include "checkOS.h"

//START190320
#if 0
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_glm2.c,v 1.23 2017/03/13 18:48:42 mcavoy Exp $";
int linmod6(Files_Struct *file_names,char *in_glm_file,char *out_glm_file_in,Files_Struct *xform_file,char *mask_file,int mode,
    float fwhm,unsigned short *seed,int *t4_identify,int num_regions,Files_Struct *region_files,int *roi,char *atlas,
    int SunOS_Linux,int bigendian,int lcdetrend,char *scratchdir,int lccompressb,int lcvoxels,int lcR2);
#endif
//START190320
#include "linmod6.h"

int main(int argc,char **argv){
char *in_glm_file=NULL,*out_glm_file=NULL,*mask_file=NULL,*seed_filename,*scratchdir=NULL,*uncompress=NULL,string[MAXNAME],atlas[7],*lutf=NULL; 
float	fwhm=0.;
int i,j,num_bold_files=0,mode=0,nseed=0,print_ATy=(int)FALSE,nt4_identify=0,*t4_identify=NULL,nxform_file=0,num_region_files=0,
    num_regions=0,*roi=NULL,bigendian=0,SunOS_Linux,lcATyfile=0,lcdetrend=0,lccompressb=0,lcvoxels=0,lccleanup=0,lcR2=0;
size_t i1;
FILE *fp;
unsigned short *seed=NULL;
Files_Struct *bold_files=NULL,*xform_file=NULL,*region_files=NULL,*roistr=NULL;
if(argc < 3) {
    fprintf(stderr,"Usage: compute_glm -bold_files file1 file2 ... -input_glm_file design_matrix_file_name -xform_file t4_file_name -fwhm gaussian_filter_width -output_glm_file output_file_name -mode mode\n");
    fprintf(stderr,"        -bold_files: List of bold files to be analyzed.\n");
    fprintf(stderr,"        -seed:         Three numbers specify the seed. Default is 0 0 0.\n");
    fprintf(stderr,"        -seed seed.dat Read seed from file seed.dat.\n");
    fprintf(stderr,"                       Use to generate white noise data in place of real data.\n");
    fprintf(stderr,"        -input_glm_file: *.glm file written by fidl that contains design matrix.\n");
    fprintf(stderr,"        -atlas:      Estimates computed in atlas space. Either 111, 222, 333, 222MNI. Use with -xform_file\n");
    fprintf(stderr,"        -xform_file: Name of t4(s) file defining transform to atlas space (same as for t4imgs).\n");
    fprintf(stderr,"                     Analysis is done in data space if this argument is not present.\n");
    fprintf(stderr,"        -t4: For use with more than a single t4. Identifies which t4 goes with each run. First t4 is 1.\n");
    fprintf(stderr,"        -output_glm_file: *.glm file that results are to be written to.\n");
    fprintf(stderr,"        -mask: File containing image that is nonzero for a set of voxels in the atlas covering the brain.\n");
    fprintf(stderr,"        -mode: Compute smoothness estimate.\n\tbits are defined as follows:\n\t\tbit 0: Smooth.\n\t\tbit 1: Transform to atlas coordinates.\n\t\tbit 2: Compute smoothness\n\tExample values:\n\t\t1: smooth only,\n\t\t2: transform only,\n\t\t3: smooth and transform.\n\t\t4: Estimate smoothness (assume preprocessing)\n\t\t7: Smooth, transform and estimate smoothness. */.\n");
    fprintf(stderr,"\tFor SPM_like analysis, use mode=7, for within-subject, unsmoothed analysis use mode=0.\n");
    fprintf(stderr,"        -fwhm: Full-width at half maximum of Gaussian smoothing filter.\n");
    fprintf(stderr,"               Smoothing is not performed if this arguments is zero or missing.\n");
    fprintf(stderr,"         -F:   labels, Additional F statistics to compute in addition to omnibus test.\n");
    fprintf(stderr,"               Syntax: -F label1 computes an F statistic for label1 where label1 is a label in the\n");
    fprintf(stderr,"               event file.  -F label1&label2 computes and F statistic over the effects corresponding\n"); 
    fprintf(stderr,"               label1 and label2. Label1 and label2 might be levels of a main effect, so this would\n");
    fprintf(stderr,"               compute an F statistic for the main effect.\n");
    fprintf(stderr,"        -print_ATy Output ATy matrix to a 4dfp stack.\n");
    fprintf(stderr,"\nExamples:\n\n1. For an SPM-style analysis:\ncompute_glm -bold_files b3.4dfp.img b4.4dfp.img -xform_file motion_t4 -input_glm_file motion.glm -output_glm_file motion_out.glm -mask (path)/glm_atlas_mask.4dfp.img -mode 7\n");
    fprintf(stderr,"\n2. For an SPM-style analysis specifying files using an fidl .conc file:\ncompute_glm -conc b3.conc -xform_file motion_t4 -input_glm_file motion.glm -output_glm_file motion_out.glm -mask (path)/glm_atlas_mask.4dfp.img -mode 7\n");
    fprintf(stderr,"\n3. For an unsmoothed analysis:\ncompute_glm -bold_files b3.4dfp.img b4.4dfp.img -input_glm_file motion.glm -output_glm_file motion_out.glm -mode 0\n");
    fprintf(stderr,"    -region_file: *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"                  BOLD data are averaged over the region.\n");
    fprintf(stderr,"    -regions_of_interest: Fit a GLM to selected regions in the region file(s). First region is one.\n");
    fprintf(stderr,"    -voxels:      Fit a GLM to voxels of selected regions in the region file(s). First region is one.\n");
    fprintf(stderr,"                  In effect, the regions are specifying a mask.\n");
    fprintf(stderr,"    -littleendian Write glm in little endian. Big endian is the default.\n");
    fprintf(stderr,"    -useATyfile   Added for debugging.\n");
    fprintf(stderr,"    -compress     GLM output in compressed format. eg detrending\n");
    fprintf(stderr,"    -detrend      No glm is output, rather the detrended bold runs are output.\n");
    fprintf(stderr,"    -scratchdir:  Scratch directory. Detrended runs are put here.\n");
    fprintf(stderr,"                  Include the backslash at the end.\n");
    fprintf(stderr,"    -uncompress:  bold_files are compressed. Output uncompressed glm.\n");
    fprintf(stderr,"    -R2:          Print R2 image(s).\n");
    fprintf(stderr,"    -clean_up     Remove the scratch directory and its contents.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-bold_files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++num_bold_files;
        if(!(bold_files=read_files(num_bold_files,&argv[i+1])))exit(-1);
        i+=num_bold_files;
        }
    if(!strcmp(argv[i],"-seed") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nseed;
        if(!(seed=malloc(sizeof*seed*3))) {
            printf("fidlError: Unable to malloc seed\n");
            exit(-1);
            }
        seed[0]=0; seed[1]=0; seed[2]=0;
	if(nseed == 1) {
	    seed_filename = argv[++i];
            if(!(fp=fopen(seed_filename,"r"))) {
                printf("fidlError: Could not open %s in compute_glm.\n",seed_filename);
                exit(-1);
                }
            if(fscanf(fp,"%hu %hu %hu",&seed[0],&seed[1],&seed[2]) != 3) {
		printf("fidlError: Could not read %s\n",seed_filename);
		exit(-1);
		}
            fclose(fp);
            /*printf("\n*** Seed read from %s *** seed = %u %u %u\n",seed_filename,seed[0],seed[1],seed[2]);*/
            printf("*** Seed read from %s *** seed = %u %u %u\n",seed_filename,seed[0],seed[1],seed[2]);
            }
        else if(nseed == 3)
            for(j=0;j<3;j++) seed[j] = atoi(argv[++i]);
        else { 
            printf("fidlError: Need to specify three seed numbers.\n");
            exit(-1);
            }
        }
    if(!strcmp(argv[i],"-input_glm_file") && argc > i+1)
        in_glm_file = argv[++i];
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        strcpy(atlas,argv[++i]);
    if(!strcmp(argv[i],"-xform_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nxform_file;
        if(!(xform_file=get_files(nxform_file,&argv[i+1]))) exit(-1);
        i += nxform_file;
        }
    if(!strcmp(argv[i],"-t4") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nt4_identify;
        if(!(t4_identify=malloc(sizeof*t4_identify*nt4_identify))) {
            printf("fidlError: Unable to malloc t4_identify\n");
            exit(-1);
            } 
        for(j=0;j<nt4_identify;j++) t4_identify[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-mode") && argc > i+1)
        mode = atoi(argv[++i]);
    if(!strcmp(argv[i],"-fwhm") && argc > i+1)
        fwhm = (float)atof(argv[++i]);
    if(!strcmp(argv[i],"-output_glm_file") && argc > i+1)
        out_glm_file = argv[++i];
    if(!strcmp(argv[i],"-print_ATy"))
        print_ATy = 1;
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;

        #if 0
        if(!(roi=malloc(sizeof*roi*num_regions))) {
            printf("fidlError: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
        #endif
        //START210902
        int lcisalpha=0,argc_roi=i+1;
        if(!(roistr=read_files(num_regions,&argv[argc_roi])))exit(-1); 
        for(size_t i=0;i<roistr->nfiles;++i){
            for(j=0;roistr->files[i][j];++j){
                if(isalpha(roistr->files[i][j])){lcisalpha=1;break;}
                }
            }
        if(!lcisalpha){
            if(!(roi=(int*)malloc(sizeof*roi*num_regions))) {
                printf("fidlError: Unable to malloc roi\n");
                exit(-1);
                }
            for(j=0;j<num_regions;j++) roi[j] = atoi(argv[argc_roi+j]) - 1;
            }
        i += num_regions;

        }
    if(!strcmp(argv[i],"-voxels"))
        lcvoxels = 1;
    if(!strcmp(argv[i],"-useATyfile"))
        lcATyfile = 1;
    if(!strcmp(argv[i],"-detrend"))
        lcdetrend = 1;
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        scratchdir = argv[++i];
    if(!strcmp(argv[i],"-uncompress") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        uncompress = argv[++i];
    if(!strcmp(argv[i],"-clean_up"))
        lccleanup = 1;
    if(!strcmp(argv[i],"-R2"))
        lcR2 = 1;

    #if 0
    else {
        printf("Warning: Unknown option %s\n",argv[i]);
        }
    #endif
    //START210902
    if(!strcmp(argv[i],"-lut")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            lutf = argv[++i];
            }
        else {
            printf("fidlError: No lookup table specified after -lut option. Abort!\n");fflush(stdout);
            exit(-1);
            }
        }

    }

//START160906
//if(lcdetrend&&lccompress) {lccompress=0;lccompressb=1;}

//if(lcdetrend&&lcvoxels) {lccompress=0;lccompressb=1;}
//START160908
if(lcdetrend&&lcvoxels)lccompressb=1;

//START190320
//if(!lccompressb) print_version_number(rcsid,stdout);

if(nt4_identify) {
    if(nt4_identify!=num_bold_files) {
        printf("fidlError: nt4_identify=%d num_bold_files=%d Must be equal. Abort!\n",nt4_identify,num_bold_files);
        exit(-1);
        }
    }
else {
    if(!(t4_identify=malloc(sizeof*t4_identify*num_bold_files))) {
        printf("fidlError: Unable to malloc t4_identify\n");
        exit(-1);
        } 
    for(j=0;j<num_bold_files;j++) t4_identify[j]=0;
    }
if(!seed) {
    fflush(stdout);
    if(!num_bold_files) {
        printf("fidlError: You have not provided any data. Need to use -bold_files  Abort!\n");
        exit(-1);
        }
    if(!lccompressb) {
        printf("BOLD files:\n");
        for(i1=0;i1<bold_files->nfiles;i1++) printf("    %s\n",bold_files->files[i1]);
        printf("Input GLM file: %s\n",in_glm_file);
        printf("Output GLM file: %s\n",out_glm_file);
        if(nxform_file) {
            printf("T4 file: %s\n",xform_file->files[0]);
            for(i=1;i<nxform_file;i++) printf("         %s\n",xform_file->files[i]);
            }
        if(mask_file) printf("Brain mask file: %s\n",mask_file);
        if(uncompress) printf("Uncompress file: %s\n",uncompress);
        printf("Mode: %d\n",mode);
        printf("FWHM of smoothing filter: %6.2f\n",fwhm);
        }
    }
else {
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("!! YOUR DATA WILL BE REPLACED WITH WHITE NOISE DATA. !!\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

if(nt4_identify&&!lccompressb) {
    printf("t4_identify="); for(i=0;i<nt4_identify;i++) printf("%d ",t4_identify[i]); printf("\n");
    }

#if 0
if(!linmod6(bold_files,in_glm_file,out_glm_file,xform_file,mask_file,mode,fwhm,seed,t4_identify,num_regions,region_files,
    roi,atlas,SunOS_Linux,bigendian,lcdetrend,scratchdir,lccompressb,lcvoxels,lcR2)){
    printf("fidlError: Problem in linmod6. Abort!\n");exit(-1);
    }
#endif
//START210902
if(!linmod6(bold_files,in_glm_file,out_glm_file,xform_file,mask_file,mode,fwhm,seed,t4_identify,num_regions,region_files,
    roi,atlas,SunOS_Linux,bigendian,lcdetrend,scratchdir,lccompressb,lcvoxels,lcR2,roistr,lutf)){
    printf("fidlError: Problem in linmod6. Abort!\n");exit(-1);
    }

if(lccleanup&&scratchdir) {
    sprintf(string,"rm -rf %s",scratchdir);
    if(system(string) == -1) printf("fidlError: unable to %s\n",string);
    }
exit(0);
}
