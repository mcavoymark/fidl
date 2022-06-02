/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   assemble_temporal_cov_matrix.c  $Revision: 1.5 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

main(int argc,char **argv)
{
char *output_file;

int i,j,k,l,m,n,q,*length,num_bold_files=0,total_length,max_length,SunOS_Linux,nreg,cov_area,cov_vol,sp,ap;

float *cov,*temp_float;

Interfile_header *ifh;
Files_Struct *bold_files;

if (argc < 5) {
    fprintf(stderr,"assemble_temporal_cov_matrix\n");
    fprintf(stderr,"        -temporal_cov_files:   One for each run.\n");
    fprintf(stderr,"        -output_file:          The matrix is written to this *4dfp.img file.\n");
    fprintf(stderr,"   Note: additional code needs to be written to handle more than one region.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-temporal_cov_files") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++num_bold_files;
        if(!(bold_files=get_files(num_bold_files,&argv[i+1]))) exit(-1);
        i += num_bold_files;
        }
    if(!strcmp(argv[i],"-output_file") && argc > i+1)
        output_file = argv[++i];
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!num_bold_files) {
    printf("Error: No temporal_cov files. Abort!\n");
    exit(-1);
    }
if(!output_file) {
    printf("Error: output_file has not been specified. Abort!\n");
    exit(-1);
    }
if(!(length=malloc(sizeof*length*num_bold_files))) {
    printf("Error: Unable to malloc length\n");
    exit(-1);
    }
for(max_length=total_length=i=0;i<num_bold_files;i++) {
    if(!(ifh=read_ifh(bold_files->files[i]))) exit(-1);
    if(ifh->dim1>max_length) max_length = ifh->dim1;
    length[i] = ifh->dim1; 
    total_length += ifh->dim1;
    if(ifh->dim2 != ifh->dim1) {
        printf("ifh->dim2 = %d ifh->dim1 = %d They must be equal. Abort!\n",ifh->dim2,ifh->dim1);
        exit(-1);
        }
    if(!i) {
        nreg = ifh->dim3;
        }
    else if(ifh->dim3 != nreg) {
        printf("ifh->dim3=%d nreg=%d  Must be equal. Abort!\n",ifh->dim3,nreg);
        exit(-1);
        }
    if(ifh->dim4 != 1) {
        printf("ifh->dim4 = %d. It must equal 1. Abort!\n",ifh->dim4);
        exit(-1);
        }
    free_ifh(ifh,(int)FALSE);
    }
cov_area = total_length*total_length;
cov_vol = cov_area*nreg; 
printf("cov_area=%d cov_vol=%d\n",cov_area,cov_vol);
if(!(cov=malloc(sizeof*cov*cov_vol))) {
    printf("Error: Unable to malloc cov\n");
    exit(-1);
    }
for(i=0;i<cov_vol;i++) cov[i]=0.;
if(!(temp_float=malloc(sizeof*temp_float*max_length*max_length*nreg))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }

for(sp=i=0;i<num_bold_files;i++,sp+=total_length*length[i]+length[i]) {
    if(!readstack(bold_files->files[i],(float*)temp_float,sizeof(float),(size_t)(length[i]*length[i]*nreg),SunOS_Linux)) exit(-1);
    for(ap=sp,l=q=0;q<nreg;q++,ap+=cov_area) {
        for(n=j=0;j<length[i];j++,n+=total_length) {
            for(m=0,k=0;k<length[i];k++,m++,l++) {
                cov[ap+n+m] = temp_float[l];
                }
            }
        }
    }

if(!writestack(output_file,cov,sizeof(float),(size_t)cov_vol,SunOS_Linux)) return 0;
if(!(ifh=read_ifh(bold_files->files[0]))) exit(-1);
min_and_max(cov,total_length*total_length,&ifh->global_min,&ifh->global_max);
ifh->dim1 = ifh->dim2 = total_length;
ifh->dim3 = nreg;
ifh->dim4 = 1;
if(!write_ifh(output_file,ifh,(int)FALSE)) exit(-1);
printf("Output written to %s\n",output_file);
}
