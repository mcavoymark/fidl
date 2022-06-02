/* Copyright 7/3/01 Washington University.  All Rights Reserved.
   white_noise.c  $Revision: 1.14 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

main(int argc,char **argv)
{
char *root="white_noise",*str_ptr,filename[MAXNAME],*seed_filename=NULL;
int i,j,nseed=0,ndim=0,lenvol=1,dim[]={0,0,0,0},runs=1,runs_conc=1,swapbytes=0,SunOS_Linux; 
float *temp_float;
double add_constant=0,*temp_double,xyvoxsize=3.75,zvoxsize=8.;
unsigned short seed[]={0,0,0};
Interfile_header *ifh;
FILE *fp;

if (argc < 3) {
    fprintf(stderr,"Usage: white_noise -seed n1 n2 n3 -filename white_noise -add_constant 1000 -dimensions 64 64 16 128 -runs 12\n");
    fprintf(stderr,"        -seed:         Three numbers specify the seed. Default is 0 0 0.\n");
    fprintf(stderr,"        -seed seed.dat Read seed from file seed.dat.\n");
    fprintf(stderr,"                       Use to generate white noise data in place of real data.\n");
    fprintf(stderr,"        -filename:     Name of output file. Default is white_noise.4dfp.img.\n");
    fprintf(stderr,"        -add_constant: Add a constant to all data points. Default is 0.\n");
    fprintf(stderr,"        -dimensions:   Image dimesions.\n");
    fprintf(stderr,"        -xyvoxsize:    Voxel size of x and y plane (one number).\n");
    fprintf(stderr,"        -zvoxsize:     Voxel size of z plane.\n");
    fprintf(stderr,"        -swapbytes     Swap bytes of output stack.\n");
    fprintf(stderr,"        -runs:         Number of runs to generate. Each run is put into a separate file.\n");
    fprintf(stderr,"        -runs_conc:    Number of runs to put in the conc set.\n\n");
    fprintf(stderr,"        Example1  ... -runs 1 -runs_conc 12\n");
    fprintf(stderr,"                      Generate 1 run of white noise. Use this run to form a 12 run conc set.");
    fprintf(stderr,"                      This would facilitate the testing of an event file for estimatibility.");
    fprintf(stderr,"        Example2  ... -runs 12 -runs_conc 12\n");
    fprintf(stderr,"                      Generate 12 runs of white noise. Use these runs to form a 12 run conc set.");
    fprintf(stderr,"                      This would facilitate a white noise simulation.");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-seed") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nseed;
        if(nseed == 1) {
            seed_filename = argv[++i];
            if(!(fp=fopen(seed_filename,"r"))) {
                printf("Error: Could not open %s in white_noise.\n",seed_filename);
                exit(-1);
                }
            if(fscanf(fp,"%hu %hu %hu",&seed[0],&seed[1],&seed[2]) != 3) {
                printf("Error: Could not read %s\n",seed_filename);
                exit(-1);
                }
            fclose(fp);
            printf("*** Seed read from %s *** seed = %u %u %u\n",seed_filename,seed[0],seed[1],seed[2]);
            }
        else if(nseed == 3)
            for(j=0;j<3;j++) seed[j] = atoi(argv[++i]);
        else {
            printf("Error: Need to specify three seed numbers or a single filename.\n");
            exit(-1);
            }
        }
    if(!strcmp(argv[i],"-filename") && argc > i+1) {
        root = argv[++i];
        if((str_ptr=strstr(root,".4dfp.img"))) *str_ptr = 0;
        }
    if(!strcmp(argv[i],"-add_constant") && argc > i+1)
        add_constant = atof(argv[++i]);
    if(!strcmp(argv[i],"-dimensions") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ndim;
        if(ndim > 4) {
            printf("Error: Images are limited to 4 dimensions (x,y,z,t).\n"); 
            exit(-1);
            }
        for(j=0;j<ndim;j++) dim[j] = atoi(argv[++i]);
	}
    if(!strcmp(argv[i],"-xyvoxsize") && argc > i+1)
        xyvoxsize = atof(argv[++i]);
    if(!strcmp(argv[i],"-zvoxsize") && argc > i+1)
        zvoxsize = atof(argv[++i]);
    if(!strcmp(argv[i],"-swapbytes"))
        swapbytes = 1;
    if(!strcmp(argv[i],"-runs") && argc > i+1)
        runs = atoi(argv[++i]);
    if(!strcmp(argv[i],"-runs_conc") && argc > i+1)
        runs_conc = atoi(argv[++i]);
    }
if(!ndim) {
    fprintf(stderr,"You must specify the size of the image with -dimensions option.\n");
    exit(-1);
    }
for(j=0;j<ndim;j++) lenvol *= dim[j];

if((SunOS_Linux=checkOS())==-1) exit(-1);

if(!(ifh=init_ifh(4,dim[0],dim[1],dim[2],dim[3],xyvoxsize,xyvoxsize,zvoxsize,!SunOS_Linux?1:0))) exit(-1);
ifh->bigendian = shouldiswap(SunOS_Linux,swapbytes);
if(!(temp_double=malloc(sizeof*temp_double*lenvol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*lenvol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
#ifndef MONTE_CARLO
    printf("STARTING SEED = %u %u %u\n",seed[0],seed[1],seed[2]);
#endif
for(i=0;i<runs;i++) {
    nrmlrv_doublestack(temp_double,lenvol,seed);
    if(add_constant) {
        printf("Adding constant value of %f to Gaussian rv's.\n",add_constant);
        for(j=0;j<lenvol;j++) temp_double[j] += add_constant;
        }
    for(j=0;j<lenvol;j++) temp_float[j] = (float)temp_double[j];
    if(runs == 1)
        sprintf(filename,"%s.4dfp.img",root);
    else 
        sprintf(filename,"%s_b%d.4dfp.img",root,i+1);
    if(!writestack(filename,temp_float,sizeof(float),(size_t)lenvol,swapbytes)) exit(-1);
    printf("White noise run written to %s\n",filename);
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    }

#if 0
#ifndef MONTE_CARLO
    if(runs_conc > 1) if(!write_conc(root,runs_conc,(Files_Struct*)NULL)) exit(-1);
    printf("ENDING SEED = %u %u %u\n",seed[0],seed[1],seed[2]);
#else
    if(!(fp=fopen_sub("seed.dat","w"))) return 0;
        fprintf(fp,"%u %u %u\n",seed[0],seed[1],seed[2]);
        fclose(fp);
        printf("\n*** Seed written to seed.dat *** seed = %u %u %u\n",seed[0],seed[1],seed[2]);
        }
#endif
#endif

#ifndef MONTE_CARLO

    /*if(runs_conc > 1) if(!write_conc(root,runs_conc,(Files_Struct*)NULL)) exit(-1);*/
    /*START110909*/
    if(runs_conc > 1) if(!write_conc(root,runs_conc,(char**)NULL,runs>1?0:1)) exit(-1);

    printf("ENDING SEED = %u %u %u\n",seed[0],seed[1],seed[2]);
#endif
if(seed_filename) {
    if(!(fp=fopen_sub(seed_filename,"w"))) exit(-1);
    fprintf(fp,"%u %u %u\n",seed[0],seed[1],seed[2]);
    fclose(fp);
    printf("\n*** Seed written to seed.dat *** seed = %u %u %u\n",seed[0],seed[1],seed[2]);
    }


}
