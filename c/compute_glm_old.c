/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   compute_glm_old.c  $Revision: 12.105 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fidl.h>
/*#include <time.h>*/

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_glm_old.c,v 12.105 2006/08/02 18:05:33 mcavoy Exp $";

int linmod_old(char **file_names,int n_file_names,char *in_glm_file,char *out_glm_file_in,Files_Struct *xform_file,
           char *mask_file,int mode,float fwhm,int nF,char **cF_tests,unsigned short *seed,int print_ATy,
           int *t4_identify,int num_regions,Files_Struct *region_files,int *roi,int atlas,int SunOS_Linux,int bigendian);

main(int argc,char *argv[])
{
char	*in_glm_file,*out_glm_file,*mask_file=NULL,*conc_file,line[200],*string,**cF_tests,
	input_recfile[MAXNAME],output_recfile[MAXNAME],dum[MAXNAME],
	*asctime,date[10],cmd[2000],stem[MAXNAME],*user,*seed_filename;

float	fwhm=0.,version;

int	i,j,num_files=0,mode,nF=0,stat,nseed=0,print_ATy=(int)FALSE,nt4_identify=0,*t4_identify,nxform_file=0,num_region_files=0,
        num_regions=0,*roi,atlas=0,lcdontswap=0,swapbytes,SunOS_Linux,bigendian=1;

long	itime;

FILE	*fp;

time_t  *tptr=NULL;

unsigned short *seed=NULL;

Files_Struct *bold_files,*xform_file=NULL,*region_files=NULL;

print_version_number(rcsid,stdout);
if(argc < 3) {
    fprintf(stderr,"Usage: compute_glm -bold_files file1 file2 ... -input_glm_file design_matrix_file_name -xform_file t4_file_name -fwhm gaussian_filter_width -output_glm_file output_file_name -mode mode\n");
    fprintf(stderr,"        -bold_files: List of bold files to be analyzed.\n");
    /*fprintf(stderr,"        -conc_file: fidl file describing concatenated files .\n");*/
    fprintf(stderr,"        -seed:         Three numbers specify the seed. Default is 0 0 0.\n");
    fprintf(stderr,"        -seed seed.dat Read seed from file seed.dat.\n");
    fprintf(stderr,"                       Use to generate white noise data in place of real data.\n");
    fprintf(stderr,"        -input_glm_file: *.glm file written by fidl that contains design matrix.\n");
    fprintf(stderr,"        -atlas:      Estimates computed in atlas space. Either 111, 222 or 333. Use with -xform_file.\n");
    fprintf(stderr,"        -xform_file: Name of t4(s) file defining transform to atlas space (same as for t4imgs).\n");
    fprintf(stderr,"                     Analysis is done in data space if this argument is not present.\n");
    fprintf(stderr,"        -t4: For use with more than a single t4. Identifies which t4 goes with each run. First t4 is 1.\n");
    fprintf(stderr,"        -output_glm_file: *.glm file that results are to be written to.\n");
    fprintf(stderr,"        -mask_file: File containing image that is nonzero for a set of voxels in the atlas covering the brain.\n");
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
    fprintf(stderr,"\nExamples:\n\n1. For an SPM-style analysis:\ncompute_glm -bold_files b3.4dfp.img b4.4dfp.img -xform_file motion_t4 -input_glm_file motion.glm -output_glm_file motion_out.glm -mask_file (path)/glm_atlas_mask.4dfp.img -mode 7\n");
    fprintf(stderr,"\n2. For an SPM-style analysis specifying files using an fidl .conc file:\ncompute_glm -conc b3.conc -xform_file motion_t4 -input_glm_file motion.glm -output_glm_file motion_out.glm -mask_file (path)/glm_atlas_mask.4dfp.img -mode 7\n");
    fprintf(stderr,"\n3. For an unsmoothed analysis:\ncompute_glm -bold_files b3.4dfp.img b4.4dfp.img -input_glm_file motion.glm -output_glm_file motion_out.glm -mode 0\n");
    fprintf(stderr,"    -region_file: *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"                  BOLD data are averaged over the region.\n");
    fprintf(stderr,"    -regions_of_interest: Fit a GLM to selected regions in the region file(s). First region is one.\n");
    fprintf(stderr,"    -dontswap: Don't swap bytes of output glm.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-F") && argc > i+1) for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nF;
    }
if(nF) {
    if(!(cF_tests=malloc(sizeof*cF_tests*nF))) {
        printf("Error: Unable to malloc cF_tests\n");
        exit(-1);
        }
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-bold_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_files;
        if(!(bold_files=get_files(num_files,&argv[i+1]))) exit(-1);
        i += num_files;
        }
    if(!strcmp(argv[i],"-seed") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nseed;
        if(!(seed=malloc(sizeof*seed*3))) {
            printf("Error: Unable to malloc seed\n");
            exit(-1);
            }
        seed[0]=0; seed[1]=0; seed[2]=0;
	if(nseed == 1) {
	    seed_filename = argv[++i];
            if(!(fp=fopen(seed_filename,"r"))) {
                printf("Error: Could not open %s in compute_glm.\n",seed_filename);
                exit(-1);
                }
            if(fscanf(fp,"%hu %hu %hu",&seed[0],&seed[1],&seed[2]) != 3) {
		printf("Error: Could not read %s\n",seed_filename);
		exit(-1);
		}
            fclose(fp);
            /*printf("\n*** Seed read from %s *** seed = %u %u %u\n",seed_filename,seed[0],seed[1],seed[2]);*/
            printf("*** Seed read from %s *** seed = %u %u %u\n",seed_filename,seed[0],seed[1],seed[2]);
            }
        else if(nseed == 3)
            for(j=0;j<3;j++) seed[j] = atoi(argv[++i]);
        else { 
            printf("Error: Need to specify three seed numbers.\n");
            exit(-1);
            }
        }
    if(!strcmp(argv[i],"-input_glm_file") && argc > i+1)
        in_glm_file = argv[++i];
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        atlas = atoi(argv[++i]);
    if(!strcmp(argv[i],"-xform_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nxform_file;
        if(!(xform_file=get_files(nxform_file,&argv[i+1]))) exit(-1);
        i += nxform_file;
        }
    if(!strcmp(argv[i],"-t4") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nt4_identify;
        if(!(t4_identify=malloc(sizeof*t4_identify*nt4_identify))) {
            printf("Error: Unable to malloc t4_identify\n");
            exit(-1);
            } 
        for(j=0;j<nt4_identify;j++) t4_identify[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-mask_file") && argc > i+1)
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-mode") && argc > i+1)
        mode = atoi(argv[++i]);
    if(!strcmp(argv[i],"-fwhm") && argc > i+1)
        fwhm = (float)atof(argv[++i]);
    if(!strcmp(argv[i],"-output_glm_file") && argc > i+1)
        out_glm_file = argv[++i];
    if(!strcmp(argv[i],"-F") && argc > i+1) {
        for(j=0;j<nF;j++) cF_tests[j] = argv[++i];
        }
    if(!strcmp(argv[i],"-print_ATy"))
        print_ATy = (int)TRUE;
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;
        if(!(roi=malloc(sizeof*roi*num_regions))) {
            printf("Error: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-dontswap"))
        lcdontswap = 0;
    }
if(nt4_identify) {
    if(nt4_identify!=num_files) {
        printf("Error: nt4_identify=%d num_files=%d Must be equal. Abort!\n",nt4_identify,num_files);
        exit(-1);
        }
    }
else {
    if(!(t4_identify=malloc(sizeof*t4_identify*num_files))) {
        printf("Error: Unable to malloc t4_identify\n");
        exit(-1);
        } 
    for(j=0;j<num_files;j++) t4_identify[j]=0;
    }

#ifndef MONTE_CARLO
    printf("BOLD files:\n");
    for(i=0;i<num_files;i++) printf("    %s\n",bold_files->files[i]);
    printf("Input GLM file: %s\n",in_glm_file);
    printf("Output GLM file: %s\n",out_glm_file);
    if(nxform_file) {
        printf("T4 file: %s\n",xform_file->files[0]);
        for(i=1;i<nxform_file;i++) printf("         %s\n",xform_file->files[i]);
        }
    if(mask_file) printf("Brain mask file: %s\n",mask_file);
    printf("Mode: %d\n",mode);
    printf("FWHM of smoothing filter: %6.2f\n",fwhm);
    for(i=0;i<nF;i++) printf("F test #%d: %s\n",i,cF_tests[i]);
#endif 

if((swapbytes=checkOS())==-1) exit(-1);
if(lcdontswap==1) swapbytes=0;
if(!swapbytes) printf("Output will not be swapped.\n");

if((SunOS_Linux=checkOS())==-1) exit(-1);

if(seed) {
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("!! YOUR DATA WILL BE REPLACED WITH WHITE NOISE DATA. !!\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }

/*printf("here200 region_files->nfiles=%d\n",region_files->nfiles);
fflush(stdout);*/

if(nt4_identify) printf("t4_identify="); for(i=0;i<nt4_identify;i++) printf("%d ",t4_identify[i]); printf("\n");
if(!linmod_old(bold_files->files,num_files,in_glm_file,out_glm_file,xform_file,mask_file,mode,fwhm,nF,cF_tests,seed,
    print_ATy,t4_identify,num_regions,region_files,roi,atlas,SunOS_Linux,bigendian)) {
    printf("Error: Problem in linmod. Abort!\n");
    exit(-1);
    }
}
