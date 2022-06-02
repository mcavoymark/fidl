/* Copyright 8/17/16 Washington University.  All Rights Reserved.
   fidl_conc.c  $Revision: 1.3 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h"
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_conc.c,v 1.3 2016/08/24 21:36:14 mcavoy Exp $";
int main(int argc,char **argv)
{
char *strptr,*conc=NULL; //,*conc=NULL,string[MAXNAME],filename[MAXNAME]; 
int i,j,nfiles=0; //,nreplace=0;
size_t i1; //,n1;
Files_Struct *files=NULL; //,*replace=NULL;
Interfile_header *ifh; //,*ifh1;
//FILE *fp;
if(argc<3){
    fprintf(stderr,"Number of frames in each run are output to the terminal.\n");
    fprintf(stderr,"    -files: imgs or concs.\n");

#if 0
    fprintf(stderr,"Creates a new conc from -replace and a text file that omits extra frames at the end.\n");
    fprintf(stderr,"    -files:   Original imgs or concs.\n");
    fprintf(stderr,"    -replace: These imgs or concs will replace the -files.\n");
#endif
    exit(-1);
    }
print_version_number(rcsid,stdout);
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")) {
            if(!(files=read_conc(nfiles,&argv[i+1])))exit(-1);
            
            //START160824
            conc=argv[i+1];

            }
        else if(!strcmp(strptr,".img")){
            if(!(files=get_files(nfiles,&argv[i+1])))exit(-1);
            }
        else {
            printf("fidlError: -files not concs or imgs. Abort!\n");fflush(stdout);exit(-1);
            }
        i+=nfiles;
        }

#if 0
    if(!strcmp(argv[i],"-replace") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nreplace;
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")) {
            if(!(replace=read_conc(nreplace,&argv[i+1])))exit(-1);
            }
        else if(!strcmp(strptr,".img")){
            if(!(replace=get_files(nreplace,&argv[i+1])))exit(-1);
            }
        else {
            printf("fidlError: -replace not concs or imgs. Abort!\n");fflush(stdout);exit(-1);
            }
        conc=argv[i+1];
        i+=nreplace;
        }
#endif
    }
if(!nfiles){printf("Need to specify -files\n");exit(-1);}

//START160824
if(conc)printf("%s\n",conc);

#if 0
if(!nreplace){
#endif
    for(i1=0;i1<files->nfiles;i1++){
        if(!(ifh=read_ifh(files->files[i1],(Interfile_header*)NULL)))exit(-1);
        printf("%s\t%d\n",files->files[i1],ifh->dim4);
        free_ifh(ifh,0);
        }
#if 0
    }
else{
    n1=files->nfiles<replace->nfiles?files->nfiles:replace->nfiles;
    strcpy(string,conc);
    if(!(strptr=get_tail_sans_ext(string)))exit(-1);

    sprintf(filename,"%s1.txt",strptr);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i1=0;i1<n1;i1++){
        if(!(ifh=read_ifh(files->files[i1],(Interfile_header*)NULL)))exit(-1);
        if(!(ifh1=read_ifh(replace->files[i1],(Interfile_header*)NULL)))exit(-1);

        j=ifh->dim4<ifh1->dim4?ifh->dim4:ifh1->dim4;
        for(i=0;i<j;i++)fpintf(fp,"1\n");
        

        }
    fclose(fp);
    printf("Scrub written to %s\n",filename);

    sprintf(filename,"%s1.conc",strptr);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i1=0;i1<n1;i1++)fprintf(fp,"%s\n",replace->files[i1]);
    fclose(fp);
    printf("Conc written to %s\n",filename);

    }
#endif

}
