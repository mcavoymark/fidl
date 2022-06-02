/* Copyright 12/14/07 Washington University.  All Rights Reserved.
   fidl_IBImeansd.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_IBImeansd.c,v 1.2 2007/12/31 23:27:56 mcavoy Exp $";

main(int argc,char **argv)
{
char line[MAXNAME],write_back[MAXNAME];
int i,j,k,l,nconcs=0,nruntypes=0,*count,nskip,nstrings;
double *sum,*sum2;
FILE *fp;
Files_Struct *concs,*runtypes,**concss;
Data *data;

print_version_number(rcsid,stdout);
if(argc < 3) {
    fprintf(stderr,"    -concs:    Conc files contain *ascii_dump_EKG_diag.txt files.\n");
    fprintf(stderr,"    -runtypes: Not an identifier for each run, rather get mean and sd IBI over runs labeled as such.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-concs") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nconcs;
        if(!(concs=get_files(nconcs,&argv[i+1]))) exit(-1);
        i += nconcs;
        }
    if(!strcmp(argv[i],"-runtypes") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nruntypes;
        if(!(runtypes=get_files(nruntypes,&argv[i+1]))) exit(-1);
        i += nruntypes;
        }
    }
if(!nconcs) {
    printf("Error: Need to specify -concs\n");
    exit(-1);
    }
if(!(concss=malloc(sizeof*concss*nconcs))) {
    printf("Error: Unable to allocate concss\n");
    exit(-1);
    }
for(i=0;i<nconcs;i++) if(!(concss[i]=read_conc(concs->files[i]))) exit(-1);
if(!(sum=malloc(sizeof*sum*runtypes->nfiles))) {
    printf("Error: Unable to malloc sum\n");
    exit(-1);
    }
for(i=0;i<runtypes->nfiles;i++) sum[i]=0.;
if(!(sum2=malloc(sizeof*sum2*runtypes->nfiles))) {
    printf("Error: Unable to malloc sum2\n");
    exit(-1);
    }
for(i=0;i<runtypes->nfiles;i++) sum2[i]=0.;
if(!(count=malloc(sizeof*count*runtypes->nfiles))) {
    printf("Error: Unable to malloc count\n");
    exit(-1);
    }
for(i=0;i<runtypes->nfiles;i++) count[i]=0.;
for(i=0;i<nconcs;i++) {
    printf("%s\n",concs->files[i]);
    for(j=0;j<concss[i]->nfiles;j++) {

        if(!(fp=fopen_sub(concss[i]->files[j],"r"))) exit(-1);
        for(nskip=1;fgets(line,sizeof(line),fp);nskip++) {
            if((nstrings=count_strings(line,write_back,' '))) {
                if(write_back[0]!='#') break;
                }
            }
        fclose(fp);
        /*printf("nskip=%d\n",nskip);*/

        if(!(data=read_data(concss[i]->files[j],0,nskip,3))) exit(-1); 
        for(k=0;k<nruntypes;k++) {
            if(!(strcmp(concss[i]->identify[j],runtypes->files[k]))) {
                for(l=0;l<data->nsubjects;l++) {
                    sum[k]+=data->x[l][2];
                    sum2[k]+=data->x[l][2]*data->x[l][2];
                    }
                count[k]+=data->nsubjects;
                }
            }
        free_data(data);
        }
    }
printf("\tmean\tsd\n");
for(i=0;i<nruntypes;i++) {
    printf("%s\t%f\t%f\n",runtypes->files[i],sum[i]/(double)count[i],
        sqrt((sum2[i]-sum[i]*sum[i]/(double)count[i])/((double)count[i]-1.)));
    }
}
