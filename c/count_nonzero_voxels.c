/*********************
count_nonzero_voxels.c
*********************/

/*$Revision: 1.2 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

main(int argc,char **argv)
{

char *filename;

int lenvol,i,count;

float *temp_float,threshold=0;

Interfile_header *ifh;


if (argc < 3) {
    fprintf(stderr,"Usage: count_nonzero_voxels -filename time_fstat_333_t88.4dfp.img -threshold 1.727387\n");
    fprintf(stderr,"        -filename:     4dfp stack.\n");
    fprintf(stderr,"        -threshold:    Count number of nonzero voxels above threshold. Default is zero.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-filename") && argc > i+1)
        filename = argv[++i];
    if(!strcmp(argv[i],"-threshold") && argc > i+1)
        threshold = atof(argv[++i]);
    }

ifh = read_ifh(filename); 
lenvol = ifh->dim1*ifh->dim2*ifh->dim3;
GETMEM(temp_float,lenvol,float)
read_float(filename,temp_float,lenvol);
for(count=i=0;i<lenvol;i++) if(temp_float[i]>threshold) count++;
printf("%d\n",count);

}
