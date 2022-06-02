/* Copyright 8/3/12 Washington University.  All Rights Reserved.
   fidl_combinations.c  $Revision: 1.3 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_combination.h>
#include "fidl.h"
int main(int argc,char **argv)
{
char *out=NULL,*strptr,timestr[23];
int i;
size_t i1,setsize;
gsl_combination *c;
FILE *fp;
if(argc<3) {
    fprintf(stderr,"    -setsize: Total number of elements in the set.\n");
    fprintf(stderr,"    -out:     Output filename.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-setsize") && argc > i+1) {
        setsize = strtoul(argv[++i],&strptr,0);
        if(argv[i]==strptr){printf("fidlError: Not a number\n");exit(-1);}
        }
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    }
if(!out) {
    if(!(out=malloc(sizeof*out*31))) {
        printf("fidlError: Unable to malloc out\n");
        exit(-1);
        }
    sprintf(out,"fidl_combinations%s.txt",make_timestr2(timestr));
    }

if(!(fp=fopen_sub(out,"w"))) exit(-1);
for(i1=1;i1<=setsize;i1++) {
    c = gsl_combination_calloc(setsize,i1);
    do{gsl_combination_fprintf(fp,c,"%u ");fprintf(fp,"\n");}while(gsl_combination_next(c)==GSL_SUCCESS);
    gsl_combination_free(c);
    }
fflush(fp);fclose(fp);
printf("Output written to %s\n",out);fflush(stdout);
exit(0);
}
