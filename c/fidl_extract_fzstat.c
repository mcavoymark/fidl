/* Copyright 5/14/08 Washington University.  All Rights Reserved.
   fidl_extract_fzstat.c  $Revision: 1.4 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_extract_fzstat.c,v 1.4 2014/08/27 18:50:03 mcavoy Exp $";
int main(int argc,char **argv)
{
char *glmfile=NULL,*strptr,string[MAXNAME],filename[MAXNAME];
int i,j,nF=0,maxF,*F=NULL,SunOS_Linux,vol,swapbytes;
LinearModel *glm;
Interfile_header *ifh;
if(argc < 3) {
    fprintf(stderr,"    -glm_file: *.glm file\n");
    fprintf(stderr,"    -F:        First F contrast is 1.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-glm_file") && argc > i+1)
        glmfile = argv[++i];
    if(!strcmp(argv[i],"-F") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nF;
        if(!(F=malloc(sizeof*F*nF))) {
            printf("Error: Unable to malloc F\n");
            exit(-1);
            }
        for(maxF=j=0;j<nF;j++) {
            if((F[j]=atoi(argv[++i])-1)>maxF) maxF=F[j];
            } 
        }
    }
print_version_number(rcsid,stderr);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!glmfile) {
    printf("Error: Need to specify -glm_file  Abort!\n");
    exit(-1);
    }
if(!(glm=read_glm(glmfile,0,SunOS_Linux))) {
    printf("Error: reading %s  Abort!\n",glmfile);
    exit(-1);
    }
if(!glm->ifh->glm_nF) {
    printf("Error: %s does not contain any F contrasts. Abort!\n",glmfile);
    exit(-1);
    }
if(maxF>glm->ifh->glm_nF) {
    printf("Error: %s contains %d F contrasts. You cannot specify more than this with -F.\n",glmfile,glm->ifh->glm_nF);
    exit(-1);
    } 
vol = glm->ifh->glm_xdim*glm->ifh->glm_ydim*glm->ifh->glm_zdim;
strcpy(string,glmfile);
if(!(strptr=get_tail_sans_ext(string))) exit(-1);
swapbytes = shouldiswap(SunOS_Linux,glm->ifh->bigendian);
if(!(ifh=init_ifh(4,glm->ifh->glm_xdim,glm->ifh->glm_ydim,glm->ifh->glm_zdim,1,glm->ifh->glm_xdim,glm->ifh->glm_ydim,
    glm->ifh->glm_dz,glm->ifh->bigendian))) exit(-1);
if(!(ifh->file_name=malloc(sizeof*ifh->file_name*(strlen(glmfile)+1)))) {
    printf("Error: Unable to malloc ifh->file_name\n");
    exit(-1);
    }
strcpy(ifh->file_name,glmfile);
ifh->fwhm = glm->ifh->glm_fwhm;
for(i=0;i<nF;i++) {

    /*sprintf(filename,"%s_%s.4dfp.img",string,glm->ifh->glm_F_names[F[i]]);*/
    /*START140826*/
    sprintf(filename,"%s_%s.4dfp.img",string,glm->ifh->glm_F_names2[F[i]]); 

    if(!writestack(filename,&glm->fzstat[F[i]*vol],sizeof(float),(size_t)vol,swapbytes)) exit(-1);    
    min_and_max_init(&ifh->global_min,&ifh->global_max);
    min_and_max_floatstack(&glm->fzstat[i*vol],vol,&ifh->global_min,&ifh->global_max);
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    printf("Output written to %s\n",filename);
    }
exit(0);
}
