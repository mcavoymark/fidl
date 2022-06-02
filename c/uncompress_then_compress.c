/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   uncompress_then_compress.c $Revision: 1.4 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

main(int argc,char **argv)
{
char *uncompress_mask_file,*compress_mask_file,*directory_uncompress=(char*)NULL,*directory_compress=(char*)NULL,
     *fstat_stem=(char*)NULL,filename[MAXNAME];

int i,j,nfiles=0,compress_len,uncompress_len,*compress_idx,*uncompress_idx,length_directory,lenvol,starting_index=0;

float *temp_float,*uncompressed_image;

Interfile_header *ifh;

if(argc < 13) {
    fprintf(stderr,"        -uncompress:            Specify mask file to put back into image format.\n");
    fprintf(stderr,"        -compress:              Specify mask file to put back into compressed format.\n");
    fprintf(stderr,"        -number_of_fstat_files: Number of F-statistic files.\n");
    fprintf(stderr,"        -fstat_stem:            Stem for F-statistic.\n");
    fprintf(stderr,"        -directory_uncompress:  Specify directory for input files to be uncompressed.\n");
    fprintf(stderr,"        -directory_compress:    Specify directory for output compressed files.\n\n");
    fprintf(stderr,"        -starting_index:        Index of first file (default=0).\n\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-uncompress") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        uncompress_mask_file = argv[++i];
    if(!strcmp(argv[i],"-compress") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        compress_mask_file = argv[++i];
    if(!strcmp(argv[i],"-fstat_stem") && argc > i+1)
        fstat_stem = argv[++i];
    if(!strcmp(argv[i],"-number_of_fstat_files") && argc > i+1)
        nfiles = atoi(argv[++i]);
    if(!strcmp(argv[i],"-directory_uncompress") && argc > i+1) {
        length_directory = strlen(argv[i+1]);
        GETMEM(directory_uncompress,length_directory+1,char)
        strcpy(directory_uncompress,argv[++i]);
        if(directory_uncompress[length_directory-2] == '/') directory_uncompress[length_directory-2] = 0;
        }
    if(!strcmp(argv[i],"-directory_compress") && argc > i+1) {
        length_directory = strlen(argv[i+1]);
        GETMEM(directory_compress,length_directory+1,char)
        strcpy(directory_compress,argv[++i]);
        if(directory_compress[length_directory-2] == '/') directory_compress[length_directory-2] = 0;
        }
    if(!strcmp(argv[i],"-starting_index") && argc > i+1)
        starting_index = atoi(argv[++i]);
    }
if(directory_uncompress == (char *)NULL) {
    fprintf(stdout,"Need to specify -directory_uncompress\n");
    exit(-1);
    }
if(directory_compress == (char *)NULL) {
    fprintf(stdout,"Need to specify -directory_compress\n");
    exit(-1);
    }
if(fstat_stem == (char *)NULL) {
    fprintf(stdout,"Need to specify -fstat_stem\n");
    exit(-1);
    }
printf("starting_index = %d\n",starting_index);

GETMEM(temp_float,(int)VOL_222,float)
GETMEM_0(uncompressed_image,(int)VOL_222,float)

read_float(uncompress_mask_file,temp_float,(int)VOL_222);
for(uncompress_len=i=0;i<(int)VOL_222;i++)
    if(temp_float[i] > 0) uncompress_len++;
GETMEM(uncompress_idx,uncompress_len,int)
for(uncompress_len=i=0;i<(int)VOL_222;i++)
    if(temp_float[i] > 0) uncompress_idx[uncompress_len++] = i;

read_float(compress_mask_file,temp_float,(int)VOL_222);
for(compress_len=i=0;i<(int)VOL_222;i++)
    if(temp_float[i] > 0) compress_len++;
GETMEM(compress_idx,compress_len,int)
for(compress_len=i=0;i<(int)VOL_222;i++)
    if(temp_float[i] > 0) compress_idx[compress_len++] = i;


if(!nfiles) {
    fprintf(stdout,"Need to specify -number_of_fstat_files\n");
    exit(-10);
    }
sprintf(filename,"%s/%s0.4dfp.img",directory_uncompress,fstat_stem);
ifh = read_ifh(filename);
lenvol = ifh->dim1*ifh->dim2*ifh->dim3;
if(lenvol != uncompress_len) {
    fprintf(stdout,"Size of compressed image = %d. Number of voxels in mask = %d. They do not match.\n",lenvol,uncompress_len);
    exit(-1);
    }

ifh->dim1 = compress_len;
ifh->dim2 = 1;
ifh->dim3 = 1;
ifh->mmppix[0] = 0;
ifh->mmppix[1] = 0;
ifh->mmppix[2] = 0;
ifh->center[0] = 0;
ifh->center[1] = 0;
ifh->center[2] = 0;
sprintf(filename,"%s/%s0.4dfp.img",directory_compress,fstat_stem);
if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);

for(i=starting_index;i<nfiles;i++) {
    sprintf(filename,"%s/%s%d.4dfp.img",directory_uncompress,fstat_stem,i);
    read_float(filename,temp_float,lenvol);
    for(j=0;j<uncompress_len;j++) uncompressed_image[uncompress_idx[j]] = temp_float[j];
    for(j=0;j<compress_len;j++) temp_float[j] = uncompressed_image[compress_idx[j]];

    sprintf(filename,"%s/%s%d.4dfp.img",directory_compress,fstat_stem,i);
    if(!write_float(filename,temp_float,compress_len)) exit(-1);
    /*min_and_max(temp_float,compress_len,&ifh->global_min,&ifh->global_max);*/
    }

}
