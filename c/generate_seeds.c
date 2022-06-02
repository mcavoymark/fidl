/***************
generate_seeds.c
***************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>


main(int argc,char **argv)
{

char *filename="list_of_seeds.dat";
int i,j,k,m,nseed=0,ndim=0,lenvol=1,subjects,runs=1,seeds=1;
float dummy1,dummy2;
double *stack;
unsigned short seed[]={0,0,0};
FILE *fp;

if (argc < 3) {
    fprintf(stderr,"Usage: generate_seeds -initial_seed 0 0 0 -filename list_of_seeds.dat -dimensions 64 64 16 256 -subjects 12 -runs 100 -seeds 10\n");
    fprintf(stderr,"        -initial_seed: 3 numbers specify the seed. Default is 0 0 0.\n");
    fprintf(stderr,"        -filename:     Name of output file. Default is list_of_seeds.dat\n");
    fprintf(stderr,"        -dimensions:   Image dimesions. Can be a single number or many.\n");
    fprintf(stderr,"        -subjects:     Number of subjects.\n");
    fprintf(stderr,"        -runs:         One run is equal to dimensions*subjects.\n");
    fprintf(stderr,"        -seeds:        One seed is equal to dimensions*subjects*runs.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-initial_seed") && argc > i+1) {
	for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nseed;
	if(nseed != 3) {
	    fprintf(stderr,"Need to specify three seed numbers.\n");
	    exit(-1);
	    }
	else
	    for(j=0;j<3;j++) seed[j] = atoi(argv[++i]);
	}
    if(!strcmp(argv[i],"-filename") && argc > i+1)
        filename = argv[++i];
    if(!strcmp(argv[i],"-dimensions") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ndim;
        for(j=0;j<ndim;j++) lenvol *= atoi(argv[++i]);
	}
    if(!strcmp(argv[i],"-subjects") && argc > i+1)
        subjects = atoi(argv[++i]);
    if(!strcmp(argv[i],"-runs") && argc > i+1)
        runs = atoi(argv[++i]);
    if(!strcmp(argv[i],"-seeds") && argc > i+1)
        seeds = atoi(argv[++i]);
    }
if(!ndim) {
    fprintf(stderr,"You must specify the size ot the image with -dimensions option.\n");
    exit(-1);
    }

if(!(fp = fopen(filename,"w"))) {
    fprintf(stderr,"Could not open %s in generate_seeds.\n",filename);
    exit(-1);
    }
GETMEM(stack,lenvol,double);
for(m=0;m<seeds;m++) {
    fprintf(fp,"%u %u %u\n",seed[0],seed[1],seed[2]);
    for(k=0;k<runs;k++)
        for(i=0;i<subjects;i++)
            /*for(j=0;j<lenvol;j+=2) nrmlrv(&dummy1,&dummy2,seed);*/
	    nrmlrv_doublestack(stack,lenvol,seed);
    }
fclose(fp);
fprintf(stdout,"next seed: %u %u %u\n",seed[0],seed[1],seed[2]);

}
