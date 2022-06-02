/* Copyright 1/4/06 Washington University.  All Rights Reserved.
   fidl_genconc.c  $Revision: 1.7 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_genconc.c,v 1.7 2008/03/06 22:43:03 mcavoy Exp $";

main(int argc,char **argv)
{
char *glmfile=NULL,*concfile=NULL,*betafile=NULL,*outfile=NULL,string[MAXNAME],string2[MAXNAME],dir[MAXNAME],*strptr,*dirptr; 

int i,j,k,kk,l,p,SunOS_Linux,swapbytes;

float *temp_float;

double *b,y;

Interfile_header *ifh;
LinearModel *glm;
Files_Struct *bold;
Data *beta;
Dim_Param *dp;
FILE *fp,*fpw;

if(argc < 7) {
    fprintf(stderr,"    -glm:  This design matrix is used.\n");
    fprintf(stderr,"           Labels must match that given by -beta.\n");
    fprintf(stderr,"    -conc: Only looks at the ifh.\n");
    fprintf(stderr,"    -beta: The new conc has these shapes.\n");
    fprintf(stderr,"    -out:  Name of the new conc.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-glm") && argc > i+1)
        glmfile = argv[++i];
    if(!strcmp(argv[i],"-conc") && argc > i+1)
        concfile = argv[++i];
    if(!strcmp(argv[i],"-beta") && argc > i+1)
        betafile = argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1)
        outfile = argv[++i];
    }
if(!glmfile) {
    printf("Error: Need -glm. Abort!\n");
    exit(-1);
    }
if(!concfile) {
    printf("Error: Need -conc. Abort!\n");
    exit(-1);
    }
if(!outfile) {
    printf("Error: Need -out. Abort!\n");
    exit(-1);
    }
if(!print_version_number(rcsid,stderr)) exit(-1);
if((SunOS_Linux=checkOS())==-1) exit(-1);

if(!(glm=read_glm(glmfile,1,SunOS_Linux))) {
    printf("Error reading %s  Abort!\n",glmfile);
    exit(-1);
    }
if(!(bold=read_conc(concfile))) exit(-1);

if(!(dp=dim_param(bold->nfiles,bold->files,SunOS_Linux))) exit(-1);
swapbytes=shouldiswap(SunOS_Linux,dp->bigendian[0]);
if(glm->ifh->glm_tdim!=dp->tdim_total) {
    printf("Error: glm->ifh->glm_tdim=%d dp->tdim_total=%d Must be equal. Abort!\n",glm->ifh->glm_tdim,dp->tdim_total);
    exit(-1);
    }
if(!(b=malloc(sizeof*b*glm->ifh->glm_Mcol))) {
    printf("Error: Unable to malloc b\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*dp->tdim_max))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }


#if 0
for(i=0;i<beta->nsubjects;i++) {
    for(j=0;j<glm->tot_eff;j++) {
        if(!strcmp(beta->subjects[i],glm->effect_label[j])) {
            if(beta->npoints_per_line[i]!=glm->effect_length[j]) {
                printf("%s beta->npoints_per_line[%d]=%d glm->effect_length[%d]=%d. Must be equal. Abort!\n",beta->subjects[i],i,
                    beta->npoints_per_line[i],j,glm->effect_length[j]);
                exit(-1);
                }
            for(k=0;k<glm->effect_length[j];k++) b[glm->effect_column[j]+k] = beta->x[i][k];
            break;
            }
        }
    if(j==glm->tot_eff) {
        printf("Error: %s not found in %s Abort!\n",beta->subjects[i],glmfile);
        exit(-1); 
        }
    }
#endif

if(!betafile) {
    /*for(i=0;i<glm->ifh->glm_M_interest;i++) b[i]=1.;*/
    for(i=0;i<glm->ifh->glm_Mcol;i++) b[i]=1.;
    }
else {
    if(!(beta=read_data(betafile,1,0,0))) exit(-1);
    for(i=0;i<beta->nsubjects;i++) {
        for(j=0;j<glm->ifh->glm_tot_eff;j++) {
            if(!strcmp(beta->subjects[i],glm->ifh->glm_effect_label[j])) {
                if(beta->npoints_per_line[i]!=glm->ifh->glm_effect_length[j]) {
                    printf("%s beta->npoints_per_line[%d]=%d glm->ifh->glm_effect_length[%d]=%d. Must be equal. Abort!\n",
                        beta->subjects[i],i,beta->npoints_per_line[i],j,glm->ifh->glm_effect_length[j]);
                    exit(-1);
                    }
                for(k=0;k<glm->ifh->glm_effect_length[j];k++) b[glm->ifh->glm_effect_column[j]+k] = beta->x[i][k];
                break;
                }
            }
        if(j==glm->ifh->glm_tot_eff) {
            printf("Error: %s not found in %s Abort!\n",beta->subjects[i],glmfile);
            exit(-1);
            }
        }
    }



/*printf("glm->ifh->glm_M_interest=%d\n",glm->ifh->glm_M_interest);*/
strcpy(string,outfile);
if(!(strptr=get_tail_sans_ext(string))) exit(-1);
strcpy(dir,outfile);
if(!(dirptr=get_dir(dir))) exit(-1);
if(!(fpw=fopen_sub(outfile,"w"))) exit(-1);
fprintf(fpw,"    number_of_files:%d\n",bold->nfiles);
if(!(ifh=read_ifh(bold->files[0]))) return 0;
ifh->dim1 = 1;
ifh->dim2 = 1;
ifh->dim3 = 1;
for(p=1,l=i=0;i<bold->nfiles;i++) {
    for(j=0;j<dp->tdim[i];j++,l++) {
        y = 1000.;
        if(glm->valid_frms[l]>0.) {
            /*for(k=0;k<glm->ifh->glm_M_interest;k++)  {*/
            for(k=0;k<glm->ifh->glm_Mcol;k++)  {
                kk = k+1;
                if(glm->A[p][kk]) y += (double)glm->A[p][kk]*b[k];
                }
             if(p<glm->ifh->glm_Nrow) ++p;
             }
        temp_float[j] = (float)y;
        }
    sprintf(string2,"%s%s_b%d.4dfp.img",dirptr,strptr,i+1);
    if(!(fp=fopen_sub(string2,"w"))) exit(-1);
    if(!writestack(string2,temp_float,sizeof(float),(size_t)dp->tdim[i],swapbytes)) exit(-1);
    min_and_max(temp_float,dp->tdim[i],&ifh->global_min,&ifh->global_max);
    ifh->dim4 = dp->tdim[i];
    if(!write_ifh(string2,ifh,(int)FALSE)) exit(-1);
    fprintf(stderr,"Bold file written to %s\n",string2);
    fprintf(fpw,"               file:%s\n",string2);
    }
fclose(fpw);
printf("Concatenated file written to %s\n",outfile);
exit(0);
}
