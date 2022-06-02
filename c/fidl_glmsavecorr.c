/* Copyright 3/6/07 Washington University.  All Rights Reserved.
   fidl_glmsavecorr.c  $Revision: 1.3 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_glmsavecorr.c,v 1.3 2007/03/07 01:53:00 mcavoy Exp $";

main(int argc,char **argv)
{
char *strptr,string[MAXNAME],outfile[MAXNAME];
int i,j,k,l,m,num_glm_files=0,SunOS_Linux;
FILE *fp;
LinearModel *glm;
Files_Struct *glm_files;

if(argc < 2) {
    fprintf(stderr,"    -glm_files: List of *.glm files.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-glm_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_glm_files;
        if(!(glm_files=get_files(num_glm_files,&argv[i+1]))) exit(-1);
        i += num_glm_files;
        }
    }
print_version_number(rcsid,stdout);
if(!num_glm_files) {
    printf("Error: Need to specify -glm_files\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
for(i=0;i<num_glm_files;i++) {
    if(!(glm=read_glm(glm_files->files[i],(int)READ_ATAM1,SunOS_Linux))) {
        printf("Error: reading %s  Abort!\n",glm_files->files[i]);
        exit(-1);
        }
    strcpy(string,glm_files->files[i]);
    if(!(strptr=get_tail_sans_ext(string))) exit(-1);
    sprintf(outfile,"%s_coor.txt",strptr);
    if(!(fp=fopen_sub(outfile,"w"))) exit(-1);





    /*KEEP THIS*/
    #if 1
    fprintf(fp,"\t");
    for(j=0;j<glm->ifh->glm_all_eff;j++) {
        if(glm->ifh->glm_effect_length[j]==1) {
            fprintf(fp,"%s\t",glm->ifh->glm_effect_label[j]);
            }
        else {
            for(k=0;k<glm->ifh->glm_effect_length[j];k++) fprintf(fp,"%s_%d\t",glm->ifh->glm_effect_label[j],k+1);
            } 
        }    
    fprintf(fp,"\n");
    for(m=1,j=0;j<glm->ifh->glm_all_eff;j++) {
        for(k=0;k<glm->ifh->glm_effect_length[j];k++,m++) {
            if(glm->ifh->glm_effect_length[j]==1) {
                fprintf(fp,"%s\t",glm->ifh->glm_effect_label[j]);
                }
            else {
                fprintf(fp,"%s_%d\t",glm->ifh->glm_effect_label[j],k+1);
                }    
            for(l=1;l<=glm->ifh->glm_Mcol;l++) fprintf(fp,"%f\t",glm->ATAm1[m][l]/sqrt(glm->ATAm1[m][m]*glm->ATAm1[l][l]));
            fprintf(fp,"\n");
            }
        }    
    #endif

    #if 0
    fprintf(fp,"\t");
    for(j=0;j<glm->ifh->glm_tot_eff;j++) {
        if(glm->ifh->glm_effect_length[j]==1) {
            fprintf(fp,"%s\t",glm->ifh->glm_effect_label[j]);
            }
        else {
            for(k=0;k<glm->ifh->glm_effect_length[j];k++) fprintf(fp,"%s_%d\t",glm->ifh->glm_effect_label[j],k+1);
            } 
        }
    fprintf(fp,"\n");
    for(m=1,j=0;j<glm->ifh->glm_tot_eff;j++) {
        for(k=0;k<glm->ifh->glm_effect_length[j];k++,m++) {
            if(glm->ifh->glm_effect_length[j]==1) {
                fprintf(fp,"%s\t",glm->ifh->glm_effect_label[j]);
                }
            else {
                fprintf(fp,"%s_%d\t",glm->ifh->glm_effect_label[j],k+1);
                }
            for(l=1;l<=glm->ifh->glm_M_interest;l++) fprintf(fp,"%f\t",glm->ATAm1[m][l]);
            fprintf(fp,"\n");
            }
        }   
    #endif
    #if 0
    for(m=1,j=0;j<glm->ifh->glm_tot_eff;j++) {
        for(k=0;k<glm->ifh->glm_effect_length[j];k++,m++) {
            for(l=1;l<=glm->ifh->glm_M_interest;l++) fprintf(fp,"%f\t",glm->ATAm1[m][l]);
            fprintf(fp,"\n");
            }
        }   
    #endif



    fclose(fp);
    printf("Correlation matrix written to %s\n",outfile);
    free_glm(glm,(int)READ_ATAM1);
    }
}
