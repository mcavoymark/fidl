/* Copyright 14/2/17 Washington University.  All Rights Reserved.
   fidl_txt2conc_subcol.c. $Revision: 1.2 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_txt2conc_subcol.c,v 1.2 2014/02/18 16:32:31 mcavoy Exp $";
main(int argc,char **argv)
{
char *file=NULL,*root=NULL,line[10000],string[MAXNAME],string2[MAXNAME];
float *temp_float;
int SunOS_Linux;
size_t i,j;
Data *data;
Interfile_header *ifh;
FILE *fp;
print_version_number(rcsid,stdout);
if(argc<5) {
    fprintf(stderr,"    -file: Text file. Header is optional.\n");
    fprintf(stderr,"           First img is col1-col2. Second img is col3-col4, and so on.\n");
    fprintf(stderr,"    -root: Output root for imgs and conc. Should include directory. Each img is a 1 voxel 4dfp.\n"); 
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-file") && argc > i+1)
        file = argv[++i];
    if(!strcmp(argv[i],"-root") && argc > i+1)
        root = argv[++i];
    }
if(!file) {
    printf("Error: No text file specified with -file option. Abort!\n");
    exit(-1);
    }
if(!root) {
    printf("Error: No output root specified with -root option. Abort!\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(data=read_data(file,0,0,0))) exit(-1); 
if(!(temp_float=malloc(sizeof*temp_float*data->nsubjects))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(ifh=init_ifh(4,1,1,1,data->nsubjects,2.,2.,2.,SunOS_Linux?0:1))) exit(-1);
if(!(ifh->file_name=malloc(sizeof*ifh->file_name*(strlen(file)+1)))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
strcpy(ifh->file_name,file);
sprintf(string2,"%s.conc",root);
if(!(fp=fopen_sub(string2,"w"))) exit(-1);
for(i=0;i<data->npoints/2;i++) {
    for(j=0;j<data->nsubjects;j++) temp_float[j]=(float)(data->x[j][i*2]-data->x[j][i*2+1]);

    /*for(j=0;j<data->nsubjects;j++)printf("%f\n",temp_float[j]);printf("\n");*/

    sprintf(string,"%s_b%d.4dfp.img",root,i+1);
    if(!writestack(string,temp_float,sizeof(float),(size_t)data->nsubjects,0)) exit(-1);
    min_and_max(temp_float,data->nsubjects,&ifh->global_min,&ifh->global_max);
    if(!write_ifh(string,ifh,0)) exit(-1);
    printf("Output written to %s\n",string);
    fprintf(fp,"%s\n",string);
    } 
fclose(fp);
printf("Output written to %s\n",string2);fflush(stdout);
exit(0);
}
