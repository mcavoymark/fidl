/* Copyright 5/8/07 Washington University.  All Rights Reserved.
   fidl_replace_dsgnmat.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <nrutil.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_replace_dsgnmat.c,v 1.2 2007/06/05 21:07:09 mcavoy Exp $";

main(int argc,char **argv)
{
char *glmfile=NULL,*event_file=NULL,*out=NULL;
int i,j,k,jj,SunOS_Linux,bigendian=1;
float **A,tf,max;
LinearModel *glm;
Sng *sng;

if(argc < 3) {
    fprintf(stderr,"    -glm_file:    *.glm file that contains the linear model.\n");
    fprintf(stderr,"    -event_file:  Replace design matrix with this event file..\n");
    fprintf(stderr,"    -out:         Name of output glm. Default is the input glm.\n");
    fprintf(stderr,"    -littleendian Write glm in little endian. Big endian is the default.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-glm_file") && argc > i+1)
        glmfile = argv[++i];
    if(!strcmp(argv[i],"-event_file") && argc > i+1)
        event_file = argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    if(!strcmp(argv[i],"-littleendian"))
        bigendian = 0;
    }
if(!glmfile) {
    printf("Error: Need to specify -glm_file\n");
    exit(-1);
    }
if(!event_file) {
    printf("Error: Need to specify -event_file\n");
    exit(-1);
    }
if(!out) out = glmfile; 

if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(glm=read_glm(glmfile,0,SunOS_Linux))) {
    printf("Error: reading %s  Abort!\n",glmfile);
    exit(-1);
    }
if(!(sng=read_ev(event_file))) {
    printf("Error: reading %s  Abort!\n",glmfile);
    exit(-1);
    }
printf("sng->frames="); for(i=0;i<sng->num_trials;i++) printf("%d:%d ",i,sng->frames[i]); printf("\n");

printf("glm->ifh->glm_tot_eff=%d sng->num_levels=%d sng->num_trials=%d\n",glm->ifh->glm_tot_eff,sng->num_levels,sng->num_trials);
if(glm->ifh->glm_tot_eff!=sng->num_levels || glm->ifh->glm_tot_eff!=sng->num_trials) {
    printf("Error: glm->ifh->glm_tot_eff=%d sng->num_levels=%d. Must be equal!\n",glm->ifh->glm_tot_eff,sng->num_levels);
    exit(-1);
    }
printf("sng->frames[sng->num_trials-1]=%d glm->ifh->glm_tdim=%d\n",sng->frames[sng->num_trials-1],glm->ifh->glm_tdim);
if(sng->frames[sng->num_trials-1]>glm->ifh->glm_tdim) {
    printf("Error: sng->frames[sng->num_trials-1]=%d glm->ifh->glm_tdim=%d Must be equal!\n",sng->frames[sng->num_trials-1],
        glm->ifh->glm_tdim);
    exit(-1);
    }

A=matrix(1,glm->ifh->glm_tdim,1,sng->num_trials);
for(i=0;i<sng->num_trials;i++) for(j=0;j<glm->ifh->glm_tdim;j++) A[j+1][i+1]=0.;



#if 0
for(i=0;i<sng->num_trials;i++) {
    for(j=0;j<(int)((sng->stimlen_vs_t[i]+20.)/sng->TR);j++) {
        A[sng->frames[i]+1+j][i+1]=boynton_model((float)(sng->TR*j),sng->stimlen_vs_t[i],(float)HRF_DELTA,(float)HRF_TAU,(int)TRUE);
        }
    }
#endif

for(i=0;i<sng->num_trials;i++) {
    jj=(int)((sng->stimlen_vs_t[i]+20.)/sng->TR);
    for(max=0.,j=0;j<jj;j++) {
        if((tf=boynton_model((float)(sng->TR*j),sng->stimlen_vs_t[i],(float)HRF_DELTA,(float)HRF_TAU,(int)TRUE))>max) max=tf;
        A[sng->frames[i]+1+j][i+1]=tf;
        }
    for(j=0;j<jj;j++) A[sng->frames[i]+1+j][i+1]/=max; 
    }




for(i=0;i<sng->num_trials;i++) {
    for(k=1,j=0;j<glm->ifh->glm_tdim;j++) {
        if(glm->valid_frms[j]) glm->A[k++][i+1]=A[j+1][i+1];
        } 
    }
if(!write_glm(out,glm,(int)WRITE_GLM_DSGNONLY,0,0,SunOS_Linux,bigendian)) {
    printf("Error: writing WRITE_GLM_DSGNONLY to %s\n",out);
    exit(-1);
    }

exit(0);
}
