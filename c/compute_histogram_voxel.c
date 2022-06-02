/* Copyright 1/31/07 Washington University.  All Rights Reserved.
   compute_histogram_voxel.c  $Revision: 1.3 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_histogram_voxel.c,v 1.3 2011/02/08 21:16:28 mcavoy Exp $";

main(int argc,char **argv)
{
char *fstat_stem=NULL,*mask_file=NULL,filename[MAXNAME];
int i,j,k,SunOS_Linux,swapbytes=0,vol,nfiles=0,nfthreshold=0;
unsigned int *count_voxels,voxels_tested;
float *temp_float,*fthreshold;
FILE *fp;
Interfile_header *ifh;
Mask_Struct *ms;

print_version_number(rcsid,stdout);
if (argc < 3) {
    fprintf(stderr,"    -number_of_fstat_files:\n");
    fprintf(stderr,"    -fstat_stem:\n");
    fprintf(stderr,"    -mask:            Limit computation to mask voxels.\n");
    fprintf(stderr,"    -fthreshold       Value of F test.\n");
    fprintf(stderr,"    -swapbytes\n");
    exit(-1);
    }

for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-number_of_fstat_files") && argc > i+1)
        nfiles = atoi(argv[++i]);
    if(!strcmp(argv[i],"-fstat_stem") && argc > i+1)
        fstat_stem =  argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-fthreshold") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfthreshold;
        if(!(fthreshold=malloc(sizeof*fthreshold*nfthreshold))) {
            printf("Error: Unable to malloc fthreshold\n");
            exit(-1);
            }
        for(j=0;j<nfthreshold;j++) fthreshold[j] = (float)atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-swapbytes") && argc > i+1)
        swapbytes=1;
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
printf("swabbytes=%d Bytes will%s be swapped.\n",swapbytes,!swapbytes?" not":"");
if(!nfiles) {
    printf("Error: Need to specify -number_of_fstat_files\n");
    exit(-1);
    }
if(!fstat_stem) {
    printf("Error: Need to specify -fstat_stem\n");
    exit(-1);
    }
if(!nfthreshold) {
    printf("Error: Need to specify -fthresholdt\n");
    exit(-1);
    } 
for(i=0;i<nfthreshold;i++) printf("fthreshold=%f\n",fthreshold[i]);
printf("fstat_stem=%s nfiles=%d\n",fstat_stem,nfiles);

sprintf(filename,"%s0.4dfp.img",fstat_stem);
if(!(ifh=read_ifh(filename,(Interfile_header*)NULL))) exit(-1);
vol = ifh->dim1*ifh->dim2*ifh->dim3;

if(!(ms=get_mask_struct(mask_file,vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
printf("vol=%d ms->lenbrain=%d\n",vol,ms->lenbrain);
printf("ifh->dof_condition = %f   ifh->dof_error = %f\n",ifh->dof_condition,ifh->dof_error);

if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(count_voxels=malloc(sizeof*count_voxels*nfthreshold))) {
    printf("Error: Unable to malloc count_voxels\n");
    exit(-1);
    }
for(i=0;i<nfthreshold;i++) count_voxels[i]=0;

#if 0
for(i=0;i<nfiles;i++) {
    sprintf(filename,"%s%d.4dfp.img",fstat_stem,i);
    if(!(fp=fopen_sub(filename,"r"))) exit(-1);
    fclose(fp);
    }
#endif
for(i=0;i<nfiles;i++) {
    sprintf(filename,"%s%d.4dfp.img",fstat_stem,i);
    if(!readstack(filename,(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    for(j=0;j<nfthreshold;j++) {
        for(k=0;k<ms->lenbrain;k++) if(temp_float[ms->brnidx[k]] > fthreshold[j]) count_voxels[j]++;
        }
    }
voxels_tested = (unsigned int)nfiles*(unsigned int)ms->lenbrain; 
printf("nfiles=%d voxels tested = %d\n",nfiles,voxels_tested);
for(i=0;i<nfthreshold;i++) {
    printf("fthreshold=%f  false positives=%d  false positive rate=%.8f\n",fthreshold[i],count_voxels[i],
        (double)count_voxels[i]/(double)voxels_tested);
    }
}
