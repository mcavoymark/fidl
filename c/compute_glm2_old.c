/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   compute_glm2.c  $Revision: 1.6 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_glm2.c,v 1.6 2012/08/29 00:00:44 mcavoy Exp $";

#if 0
int linmod4(Files_Struct *file_names,char *in_glm_file,char *out_glm_file_in,Files_Struct *xform_file,char *mask_file,int mode,
    float fwhm,int nF,char **F_names,unsigned short *seed,int print_ATy,int *t4_identify,int num_regions,Files_Struct *region_files,
    int *roi,int atlas,int SunOS_Linux,int bigendian,int lcATyfile,int lccompress,int lcdetrend,char *scratchdir,int lccompressb,
    char *uncompress);
#endif
int linmod_old(Files_Struct *file_names,char *in_glm_file,char *out_glm_file_in,Files_Struct *xform_file,char *mask_file,int mode,
    float fwhm,int nF,char **F_names,unsigned short *seed,int print_ATy,int *t4_identify,int num_regions,Files_Struct *region_files,
    int *roi,int atlas,int SunOS_Linux,int bigendian,int lcATyfile,int lccompress,int lcdetrend,char *scratchdir,int lccompressb,
    char *uncompress);
/*START120827*/
#if 0
int linmod4(Files_Struct *file_names,char *in_glm_file,char *out_glm_file_in,Files_Struct *xform_file,char *mask_file,int mode,
    float fwhm,unsigned short *seed,int print_ATy,int *t4_identify,int num_regions,Files_Struct *region_files,
    int *roi,int atlas,int SunOS_Linux,int bigendian,int lcATyfile,int lccompress,int lcdetrend,char *scratchdir,int lccompressb,
    char *uncompress);
#endif

main(int argc,char **argv)
{
char	*in_glm_file,*out_glm_file,*mask_file=NULL,*seed_filename,*scratchdir=NULL,*strptr,*uncompress=NULL,**cF_tests;

float	fwhm=0.,version;

int	i,j,num_files=0,mode,stat,nseed=0,print_ATy=(int)FALSE,nt4_identify=0,*t4_identify,nxform_file=0,num_region_files=0,
        num_regions=0,*roi=NULL,atlas=0,bigendian=1,SunOS_Linux,lcATyfile=0,lccompress=0,lcdetrend=0,lccompressb=0,nF;

long	itime;

FILE	*fp;

time_t  *tptr=NULL;

unsigned short *seed=NULL;

Files_Struct *bold_files=NULL,*xform_file=NULL,*region_files=NULL;

/*START120111*/
/*print_version_number(rcsid,stdout);*/

if(argc < 3) {
    fprintf(stderr,"Usage: compute_glm -bold_files file1 file2 ... -input_glm_file design_matrix_file_name -xform_file t4_file_name -fwhm gaussian_filter_width -output_glm_file output_file_name -mode mode\n");
    fprintf(stderr,"        -bold_files: List of bold files to be analyzed.\n");

    /*START111007*/
    /*fprintf(stderr,"        -conc:       Conc file.\n");*/

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
    fprintf(stderr,"    -littleendian           Write glm in little endian. Big endian is the default.\n");
    fprintf(stderr,"    -useATyfile             Added for debugging.\n");

    /*START111206*/
    fprintf(stderr,"    -compress    GLM output in compressed format. eg detrending\n");
    /*START111215*/
    fprintf(stderr,"    -detrend     No glm is output, rather the detrended bold runs are output.\n");
    fprintf(stderr,"    -scratchdir: Scratch directory. Detrended runs are put here.\n");
    fprintf(stderr,"                 Include the backslash at the end.\n");

    /*START120810*/
    fprintf(stderr,"    -uncompress: bold_files are compressed. Output uncompressed glm.\n");

    exit(-1);
    }


/*START120827*/
#if 1
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-F") && argc > i+1) for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nF;
    }
if(nF) {
    if(!(cF_tests=malloc(sizeof*cF_tests*nF))) {
        printf("fidlError: Unable to malloc cF_tests\n");
        exit(-1);
        }
    }
#endif


for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-bold_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_files;
        if(num_files) {
            strptr = strrchr(argv[i+1],'.');
            if(!strcmp(strptr,".conc")) {
                if(num_files>1) {printf("fidlError: Only set up to handle a single conc. Abort!\n");fflush(stdout);exit(-1);}
                if(!(bold_files=read_conc(argv[++i]))) exit(-1);
                }
            else if(!strcmp(strptr,".img")){
                if(!(bold_files=get_files(num_files,&argv[i+1]))) exit(-1);
                i += num_files;
                }
            else {
                printf("Error: -file not conc or img. Abort!\n");fflush(stdout);exit(-1);
                }
            }
        }

    else if(!strcmp(argv[i],"-seed") && argc > i+1) {
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
    else if(!strcmp(argv[i],"-input_glm_file") && argc > i+1)
        in_glm_file = argv[++i];
    else if(!strcmp(argv[i],"-atlas") && argc > i+1)
        atlas = atoi(argv[++i]);
    else if(!strcmp(argv[i],"-xform_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nxform_file;
        if(!(xform_file=get_files(nxform_file,&argv[i+1]))) exit(-1);
        i += nxform_file;
        }
    else if(!strcmp(argv[i],"-t4") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nt4_identify;
        if(!(t4_identify=malloc(sizeof*t4_identify*nt4_identify))) {
            printf("fidlError: Unable to malloc t4_identify\n");
            exit(-1);
            } 
        for(j=0;j<nt4_identify;j++) t4_identify[j] = atoi(argv[++i]) - 1;
        }
    else if(!strcmp(argv[i],"-mask_file") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    else if(!strcmp(argv[i],"-mode") && argc > i+1)
        mode = atoi(argv[++i]);
    else if(!strcmp(argv[i],"-fwhm") && argc > i+1)
        fwhm = (float)atof(argv[++i]);
    else if(!strcmp(argv[i],"-output_glm_file") && argc > i+1)
        out_glm_file = argv[++i];

    /*START120827*/
    #if 1
    else if(!strcmp(argv[i],"-F") && argc > i+1) {
        for(j=0;j<nF;j++) cF_tests[j] = argv[++i];
        }
    #endif

    else if(!strcmp(argv[i],"-print_ATy"))
        print_ATy = (int)TRUE;
    else if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    else if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;
        if(!(roi=malloc(sizeof*roi*num_regions))) {
            printf("fidlError: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
        }
    else if(!strcmp(argv[i],"-littleendian"))
        bigendian = 0;
    else if(!strcmp(argv[i],"-useATyfile"))
        lcATyfile = 1;

    /*START111011*/
    else if(!strcmp(argv[i],"-compress"))
        lccompress = 1;
    /*START111215*/
    else if(!strcmp(argv[i],"-detrend"))
        lcdetrend = 1;

    #if 0
    else if(!strcmp(argv[i],"-scratchdir") && argc > i+1)
        scratchdir = argv[++i];
    #endif
    /*START120810*/
    else if(!strcmp(argv[i],"-scratchdir") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        scratchdir = argv[++i];
    else if(!strcmp(argv[i],"-uncompress") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        uncompress = argv[++i];


    else {
        printf("Warning: Unknown option %s\n",argv[i]);
        }
    }

/*START111215*/
/*if(!scratchdir&&lcdetrend) if(!(scratchdir = make_scratchdir(1))) exit(-1);*/
if(lcdetrend&&lccompress) {lccompress=0;lccompressb=1;}

/*START120111*/
if(!lccompressb) print_version_number(rcsid,stdout);


if(nt4_identify) {
    if(nt4_identify!=num_files) {
        printf("fidlError: nt4_identify=%d num_files=%d Must be equal. Abort!\n",nt4_identify,num_files);
        exit(-1);
        }
    }
else {
    if(!(t4_identify=malloc(sizeof*t4_identify*num_files))) {
        printf("fidlError: Unable to malloc t4_identify\n");
        exit(-1);
        } 
    for(j=0;j<num_files;j++) t4_identify[j]=0;
    }
if(!seed) {
    fflush(stdout);
    if(!num_files) {
        printf("fidlError: You have not provided any data. Need to use -bold_files  Abort!\n");
        exit(-1);
        }
    if(!lccompressb) {
        printf("BOLD files:\n");

        /*for(i=0;i<num_files;i++) printf("    %s\n",bold_files->files[i]);*/
        /*START120810*/
        for(i=0;i<bold_files->nfiles;i++) printf("    %s\n",bold_files->files[i]);

        printf("Input GLM file: %s\n",in_glm_file);
        printf("Output GLM file: %s\n",out_glm_file);
        if(nxform_file) {
            printf("T4 file: %s\n",xform_file->files[0]);
            for(i=1;i<nxform_file;i++) printf("         %s\n",xform_file->files[i]);
            }
        if(mask_file) printf("Brain mask file: %s\n",mask_file);

        /*START120810*/
        if(uncompress) printf("Uncompress file: %s\n",uncompress);

        printf("Mode: %d\n",mode);
        printf("FWHM of smoothing filter: %6.2f\n",fwhm);

        /*START120827*/
        /*for(i=0;i<nF;i++) printf("F test #%d: %s\n",i,cF_tests[i]);*/

        }
    }
else {
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("!! YOUR DATA WILL BE REPLACED WITH WHITE NOISE DATA. !!\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(nt4_identify) {
    printf("t4_identify="); for(i=0;i<nt4_identify;i++) printf("%d ",t4_identify[i]); printf("\n");
    }

#if 0
if(!linmod4(bold_files,in_glm_file,out_glm_file,xform_file,mask_file,mode,fwhm,nF,cF_tests,seed,print_ATy,t4_identify,num_regions,
    region_files,roi,atlas,SunOS_Linux,bigendian,lcATyfile,lccompress,lcdetrend,scratchdir,lccompressb,uncompress)) {
    printf("fidlError: Problem in linmod. Abort!\n");
    exit(-1);
    }
#endif
if(!linmod_old(bold_files,in_glm_file,out_glm_file,xform_file,mask_file,mode,fwhm,nF,cF_tests,seed,print_ATy,t4_identify,num_regions,
    region_files,roi,atlas,SunOS_Linux,bigendian,lcATyfile,lccompress,lcdetrend,scratchdir,lccompressb,uncompress)) {
    printf("fidlError: Problem in linmod. Abort!\n");
    exit(-1);
    }
/*START120827*/
#if 0
if(!linmod4(bold_files,in_glm_file,out_glm_file,xform_file,mask_file,mode,fwhm,seed,print_ATy,t4_identify,num_regions,
    region_files,roi,atlas,SunOS_Linux,bigendian,lcATyfile,lccompress,lcdetrend,scratchdir,lccompressb,uncompress)) {
    printf("fidlError: Problem in linmod. Abort!\n");
    exit(-1);
    }
#endif

exit(0);
}
