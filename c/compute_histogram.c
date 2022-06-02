/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   compute_histogram.c  $Revision: 1.28 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

enum{MAX_NTHRESH = 11,
     MAX_NEXTENT = 25};

main(int argc,char **argv)
{
char **zstat,**fstat,filename[MAXNAME],*fstat_stem=NULL,*zstat_stem=NULL,*uncompress_file=NULL,*mask_file=NULL,string[MAXNAME],
     write_back[MAXNAME],**strings,*F_threshold_file_voxel_level_analysis=NULL,*Z_threshold_file_region_level_analysis=NULL;

int i,j,k,m,lenvol,n,*count,nfiles[]={0,0},nlines,nstrings,lc_convert_f_to_z=0,lc_check_files=0,nthresh,*nextent,**extent,
    **count_regions,SunOS_Linux;

float *temp_float,*act_mask,*p_values,*image,*thresh,dummy_float;

double *temp_double,*stat,*df1,*dferror,epsilon1=1.,epsilon2=1.;

Interfile_header *ifh;
Mask_Struct *us,*ms;
FILE *fp;

if (argc < 3) {
    fprintf(stderr,"Usage: compute_histogram -filenames zstat.4dfp.img -thresholds 1.959964 4.9447\n");
    fprintf(stderr,"        -zstat:        Z-statistic files.\n");
    fprintf(stderr,"        -fstat:        F-statistic files.\n");
    fprintf(stderr,"        For a large number of files you may get 'Word to long'. So try this.\n");
    fprintf(stderr,"            -number_of_zstat_files:       Number of Z-statistic files.\n");
    fprintf(stderr,"            -number_of_fstat_files:       Number of F-statistic files.\n");
    fprintf(stderr,"            -zstat_stem:                  Stem for Z-statistic.\n");
    fprintf(stderr,"            -fstat_stem:                  Stem for F-statistic.\n");
    fprintf(stderr,"        -F_threshold_file_voxel_level_analysis:\n"); 
    fprintf(stderr,"                           The histogram is calculated for each F threshold in the file.\n");
    fprintf(stderr,"                           Two numbers per line. The first number is the significance level.\n");
    fprintf(stderr,"                           The second number is the threshold.\n");
    fprintf(stderr,"        -uncompress:    Specify mask file to put back into image format.\n");
    fprintf(stderr,"        -mask:          Specify mask file for statistical analysis.\n");
    fprintf(stderr,"                        If none is given, then the area of interest is assumed to be the uncompress mask.\n");
    fprintf(stderr,"                        This is used for a regional analysis on atlas spaced images.\n");
    fprintf(stderr,"These options are for testing the new multiple comparsion thresholds.\n");
    fprintf(stderr,"        -convert_f_to_z    Convert F-statistics to Z-statistics.\n");
    fprintf(stderr,"        -box_correction:   F-statistics are gaussianized by reducing the degrees of freedom.\n");
    fprintf(stderr,"        -Z_threshold_file_region_level_analysis: Example file:\n");
    fprintf(stderr,"                                     3       12,13,14,15,16,17,18,19,20\n");
    fprintf(stderr,"                                     3.25    10,11,12,13,14,15,16,17,18\n");
    fprintf(stderr,"                                 Lines give the threshold and the comma separated extents.\n");
    fprintf(stderr,"        -check_files       Check all files can be opened. NO COMPUTATION.\n");
    fprintf(stderr,"        -epsilon1          Correction for the numerator degrees of freedom.\n");
    fprintf(stderr,"        -epsilon2          Correction for the denominator degrees of freedom.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-zstat") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles[0];
        GETMEM(zstat,nfiles[0],char *)
        for(j=0;j<nfiles[0];j++) {
	    GETMEM(zstat[j],strlen(argv[i+1+j])+1,char)
	    strcpy(zstat[j],argv[i+1+j]);
	    }
        i += nfiles[0];
        }
    if(!strcmp(argv[i],"-fstat") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles[1];
        GETMEM(fstat,nfiles[1],char *)
        for(j=0;j<nfiles[1];j++) {
            GETMEM(fstat[j],strlen(argv[i+1+j])+1,char)
            strcpy(fstat[j],argv[i+1+j]);
            }
        i += nfiles[1];
        }
    if(!strcmp(argv[i],"-number_of_zstat_files") && argc > i+1)
        nfiles[0] = atoi(argv[++i]);
    if(!strcmp(argv[i],"-number_of_fstat_files") && argc > i+1)
        nfiles[1] = atoi(argv[++i]);
    if(!strcmp(argv[i],"-zstat_stem") && argc > i+1) {
        GETMEM(zstat_stem,strlen(argv[i+1])+1,char)
        strcpy(zstat_stem,argv[++i]);
	}
    if(!strcmp(argv[i],"-fstat_stem") && argc > i+1) {
        GETMEM(fstat_stem,strlen(argv[i+1])+1,char)
        strcpy(fstat_stem,argv[++i]);
	}
    if(!strcmp(argv[i],"-F_threshold_file_voxel_level_analysis") && argc > i+1)
	F_threshold_file_voxel_level_analysis = argv[++i];
    if(!strcmp(argv[i],"-Z_threshold_file_region_level_analysis") && argc > i+1)
	Z_threshold_file_region_level_analysis = argv[++i];
    if(!strcmp(argv[i],"-uncompress") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        uncompress_file = argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-convert_f_to_z"))
        lc_convert_f_to_z = (int)TRUE;
    if(!strcmp(argv[i],"-epsilon1") && argc > i+1)
        epsilon1 = atof(argv[++i]);
    if(!strcmp(argv[i],"-epsilon2") && argc > i+1)
        epsilon2 = atof(argv[++i]);
    if(!strcmp(argv[i],"-check_files"))
        lc_check_files = (int)TRUE;
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
printf("epsilon1 = %f\n",epsilon1);
printf("epsilon2 = %f\n",epsilon2);

if(F_threshold_file_voxel_level_analysis) {
    if(!(fp=fopen_sub(F_threshold_file_voxel_level_analysis,"r"))) exit(-1);
    printf("%s\n",F_threshold_file_voxel_level_analysis);
    for(nthresh=0;fgets(string,sizeof(string),fp);nthresh++); 
    if(!nthresh) {
	fprintf(stderr,"File %s is empty. Abort!\n",F_threshold_file_voxel_level_analysis);
	exit(-1);
        }
    if(!(p_values=malloc(sizeof*p_values*nthresh))) {
        printf("Error: Unable to malloc p_values\n");
        exit(-1);
        }
    if(!(thresh=malloc(sizeof*thresh*nthresh))) {
        printf("Error: Unable to malloc thresh\n");
        exit(-1);
        }
    if(!(strings=malloc(sizeof*strings*2))) {
        printf("Error: Unable to malloc strings\n");
        exit(-1);
        }
    for(rewind(fp),i=0;fgets(string,sizeof(string),fp);i++) { 
        if((nstrings=count_strings(string,write_back,' ')) != 2) {
            fprintf(stdout,"Error: Line %d has %d data points. Should have 2 data points.\n",i+1,nstrings);
            exit(-1);
            }
        get_strings(write_back,strings,2);
	p_values[i] = atof(strings[0]);
	thresh[i] = atof(strings[1]);
        for(j=0;j<2;j++) free(strings[j]);
	}
    free(strings);
    if(!(count=malloc(sizeof*count*nthresh))) {
        printf("Error: Unable to malloc count\n");
        exit(-1);
        }

    }
else if(Z_threshold_file_region_level_analysis) {
    /*BEGIN:READ THRESHOLD-EXTENT FILES*/
    if(!(thresh=malloc(sizeof*thresh*MAX_NTHRESH))) {
        printf("Error: Unable to malloc thresh\n");
        exit(-1);
        } 
    if(!(nextent=malloc(sizeof*nextent*MAX_NTHRESH))) {
        printf("Error: Unable to malloc nextent\n");
        exit(-1);
        }
    if(!(fp=fopen(Z_threshold_file_region_level_analysis,"r"))) exit(-1);
    printf("%s\n",Z_threshold_file_region_level_analysis);
    for(nlines=0;fgets(string,sizeof(string),fp);nlines++);
    if(!nlines) {
        fprintf(stdout,"Error: File %s is empty. Abort!\n",Z_threshold_file_region_level_analysis);
        exit(-1);
        }
    if(!(extent = (int **)d2int(nlines,(int)MAX_NEXTENT))) exit(-1);
    if(!(count_regions = (int **)d2int(nlines,(int)MAX_NEXTENT))) exit(-1);
    nthresh = nlines;
    rewind(fp);
    for(j=0;j<nlines;j++) {
        fscanf(fp,"%f %s",&thresh[j],&string);
        if((nstrings=count_strings(string,write_back,' ')) < 1) {
            fprintf(stdout,"Error: %s Line %d has %d data points. Should have at least 2 data points.\n",
                Z_threshold_file_region_level_analysis,j+2,nstrings+1);
            exit(-1);
            }
        nextent[j] = nstrings;
        strings_to_int(write_back,extent[j],nstrings);
        }
    fclose(fp);
    for(j=0;j<nthresh;j++) {
        printf("z=%.2f\t",thresh[j]);
        for(k=0;k<nextent[j];k++) printf("%d ",extent[j][k]);
        printf("\n");
        }
    /*END:READ THRESHOLD-EXTENT FILES*/
    }

if(!nthresh) {
    fprintf(stdout,"No thresholds specified. Abort!\n");
    exit(-1);
    }
if(nfiles[0]) {
    if(!zstat_stem) { 
        if(!(ifh=read_ifh(zstat[0]))) exit(-1);
        }
    else {
	sprintf(filename,"%s0.4dfp.img",zstat_stem);
        if(!(ifh=read_ifh(filename))) exit(-1);
	}
    }
else if(nfiles[1]) {
    if(!lc_convert_f_to_z && Z_threshold_file_region_level_analysis) {
        printf("Incorrect opions. Use -zstat_stem and -number_of_zstat_file\n");
        exit(-1);
        }
    if(!fstat_stem) { 
        if(!(ifh=read_ifh(fstat[0]))) exit(-1);
        }
    else {
	sprintf(filename,"%s0.4dfp.img",fstat_stem);
        if(!(ifh=read_ifh(filename))) exit(-1);
        printf("ifh->dof_condition = %f   ifh->dof_error = %f\n",ifh->dof_condition,ifh->dof_error);
	}
    }
else {
    fprintf(stdout,"No zstat or fstat files specified. Abort!\n");
    exit(-1);
    }

if(uncompress_file) {
    printf("uncompress file : %s\n",uncompress_file);
    /*if(!(us=read_mask(uncompress_file,SunOS_Linux))) exit(-1);*/
    if(!(us=read_mask(uncompress_file,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
    }
if(mask_file) {
    printf("mask file : %s\n",mask_file);
    if(!(ms=read_mask(mask_file,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
    }

#if 0
else {
    ms = us;
    }
#endif
if(!uncompress_file && !mask_file) {
    printf("Need either -mask and/or -uncompress.\n");
    exit(-1);
    }
else if(!uncompress_file) {
    us = ms;
    }
else if(!mask_file) {
    ms = us;
    }




printf("ms->lenbrain=%d ms->lenvol=%d us->lenbrain=%d us->lenvol=%d\n",ms->lenbrain,ms->lenvol,us->lenbrain,us->lenvol);
if(uncompress_file || mask_file) {
    if(!(image=malloc(sizeof*image*ms->lenvol))) {
        printf("Error: Unable to malloc image\n");
        exit(-1);
        } 
    }
lenvol = ifh->dim1*ifh->dim2*ifh->dim3;
if(!(temp_float=malloc(sizeof*temp_float*lenvol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(uncompress_file) {
    if(lenvol != us->lenbrain) {
        printf("lenvol=%f us->lenbrain=%f. They must match for proper uncompression. Termination.\n",lenvol,us->lenbrain);
        exit(-1);   
        }
    }
if(lc_convert_f_to_z) {
    if(!(temp_double=malloc(sizeof*temp_double*lenvol))) {
        printf("Error: Unable to malloc temp_double\n");
        exit(-1);
        }
    if(!(stat=malloc(sizeof*stat*lenvol))) {
        printf("Error: Unable to malloc stat\n");
        exit(-1);
        }
    if(!(df1=malloc(sizeof*df1*lenvol))) {
        printf("Error: Unable to malloc df1\n");
        exit(-1);
        }
    if(!(dferror=malloc(sizeof*dferror*lenvol))) {
        printf("Error: Unable to malloc dferror\n");
        exit(-1);
        }
    }

for(k=0;k<2;k++) {
    for(i=0;i<nfiles[k];i++) {
	if(!k) {
            if(!zstat_stem) {
	        strcpy(filename,zstat[i]);
                }
	    else {
	        sprintf(filename,"%s%d.4dfp.img",zstat_stem,i);
                }
            }
	else {
            if(!fstat_stem) {
	        strcpy(filename,fstat[i]);
                }
            else {
	        sprintf(filename,"%s%d.4dfp.img",fstat_stem,i);
                }
            }
        if(!read_float(filename,temp_float,lenvol)) exit(-1);

        if(!lc_check_files) {
	    if(k && lc_convert_f_to_z) {
	        for(j=0;j<lenvol;j++) temp_double[j] = (double)temp_float[j];

                #if 0
                dferror[0] = epsilon*(double)ifh->dof_error;
	        for(j=1;j<lenvol;j++) dferror[j] = dferror[0]; 
	        f_to_z(temp_double,stat,lenvol,epsilon*(double)ifh->dof_condition,dferror);
	        for(j=0;j<lenvol;j++) temp_float[j] = (float)stat[j];
                f_to_z(temp_double,stat,lenvol,epsilon*(double)ifh->dof_condition,dferror);
                #endif
                df1[0] = epsilon1*(double)ifh->dof_condition;
                dferror[0] = epsilon2*(double)ifh->dof_error;
                for(j=1;j<lenvol;j++) {
                    df1[j] = df1[0];
                    dferror[j] = dferror[0];
                    }
                f_to_z(temp_double,stat,lenvol,df1,dferror);

                for(j=0;j<lenvol;j++) temp_float[j] = (float)stat[j];
	        }
            if(F_threshold_file_voxel_level_analysis) {
                for(j=0;j<lenvol;j++) for(m=0;m<nthresh;m++) if(temp_float[j]>thresh[m]) count[m]++;
	        }
            else if(Z_threshold_file_region_level_analysis) {
                for(j=0;j<nthresh;j++) {
                    for(m=0;m<us->lenvol;m++) image[m] = 0;
                    for(n=m=0;m<us->lenbrain;m++) {
                        if(us->brnidx[m] == ms->brnidx[n]) {
                            /*printf("temp_float[%d]=%f thresh=%f\n",m,temp_float[m],thresh[j]);*/
                            if(temp_float[m] >= thresh[j]) image[ms->brnidx[n]] = 1;
                            n++;
                            }
                        }
                    for(m=0;m<nextent[j];m++) {
                        /*printf("extent=%d\n",extent[j][m]);*/
                        /*count_regions[j][m] += spatial_extent(image,act_mask,ms->xdim,ms->ydim,ms->zdim,&dummy_float,
                            1,&extent[j][m],&ms->lenvol);*/

                        dummy_float = .5;             
                        count_regions[j][m] += spatial_extent(image,act_mask,ms->xdim,ms->ydim,ms->zdim,&dummy_float,
                            1,&extent[j][m],&ms->lenvol,1);
                        }
                    }
                }
            }
        }
    }

if(lc_check_files == (int)FALSE) {
    if(F_threshold_file_voxel_level_analysis) {
        for(i=0;i<nthresh;i++)
            printf("%g\t\t\t%g\t\t%.15f\n",p_values[i],thresh[i],(double)count[i]/((nfiles[0]+nfiles[1])*lenvol));
        }
    else if(Z_threshold_file_region_level_analysis) {
        for(j=0;j<nthresh;j++) {
            for(k=0;k<nextent[j];k++) {
                printf("threshold = %.2f\tnumber of regions with %d or more voxels = %d\n",thresh[j],extent[j][k],
                    count_regions[j][k]);
                }
            }
        }
    }

}
