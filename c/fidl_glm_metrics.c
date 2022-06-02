/* Copyright 1/12/01 Washington University.  All Rights Reserved.
   fidl_glm_metrics.c  $Revision: 1.4 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_glm_metrics.c,v 1.4 2011/01/19 19:26:26 mcavoy Exp $";
main(int argc,char **argv)
{
char *glmfile=NULL,*txtfile=NULL,*outfile=NULL,*strptr,string[MAXNAME];
int i,j,k,SunOS_Linux,*cnt,*cnt1=NULL,t_valid,*t_validi,maxlen;
float *AT;
LinearModel *glm;
Data *data;
FILE *fp;
if(argc < 3) {
    fprintf(stderr,"Usage: fidl_glm_metrics -glm glm_file -txt txt_file -out out_file\n");
    fprintf(stderr,"    -glm: glm file\n");
    fprintf(stderr,"    -txt: Single column of 1s (valid frames) and 0s (invalid frames) file\n");
    fprintf(stderr,"    -out: Output file\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-glm") && argc > i+1)
	glmfile = argv[++i];
    if(!strcmp(argv[i],"-txt") && argc > i+1)
	txtfile = argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1)
	outfile = argv[++i];
    }
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!glmfile) {
    printf("-glm file not specified.\n");
    exit(-1);
    }
if(!(glm=read_glm(glmfile,(int)TRUE,SunOS_Linux))) {
    printf("Error: reading %s  Abort!\n",glmfile);
    exit(-1);
    }
if(!(cnt=malloc(sizeof*cnt*glm->ifh->glm_M_interest))) {
    printf("Error: Unable to malloc cnt\n");
    exit(-1);
    }
count_estimates(glm->AT[0],glm->ifh->glm_M_interest,glm->ifh->glm_Nrow,cnt);

if(txtfile) {
    if(!(data=read_data(txtfile,0,0,0,0))) exit(-1);
    if(glm->ifh->glm_tdim!=data->nsubjects) {
        printf("Error: glm->ifh->glm_tdim = %d, but %s has %d datapoints. Must be equal.\n",glm->ifh->glm_tdim,txtfile,
            data->nsubjects);
        exit(-1);
        }
    if(!(t_validi=malloc(sizeof*t_validi*glm->ifh->glm_Nrow))) {
        printf("Error: Unable to malloc t_validi\n");
        exit(-1);
        }
    for(t_valid=j=i=0;i<glm->ifh->glm_tdim;i++) {
        if(glm->valid_frms[i]==1.) {
            if(data->x[i][0]==1.) t_validi[t_valid++] = j;
            j++;
            }
        }
    if(j!=glm->ifh->glm_Nrow) {
        printf("Error: j = %d  glm->ifh->glm_Nrow=%d  Must be equal.\n"); fflush(stdout);
        exit(-1);
        }
    if(t_valid>glm->ifh->glm_Nrow) {
        printf("Error: t_valid = %d  Must be less than or equal to glm->ifh->glm_Nrow = %d\n",t_valid,glm->ifh->glm_Nrow);
        exit(-1);
        }

    /*if(t_valid<glm->ifh->glm_Nrow) {*/
    /*START110119*/
    else {

        if(!(AT=malloc(sizeof*AT*glm->ifh->glm_M_interest*t_valid))) {
            printf("Error: Unable to malloc AT\n");
            exit(-1);
            }
        if(!(cnt1=malloc(sizeof*cnt1*glm->ifh->glm_M_interest))) {
            printf("Error: Unable to malloc cnt1\n");
            exit(-1);
            }
        for(i=0;i<t_valid;i++) for(k=i,j=0;j<glm->ifh->glm_M_interest;j++,k+=t_valid) AT[k]=glm->AT[j][t_validi[i]];
        count_estimates(AT,glm->ifh->glm_M_interest,t_valid,cnt1);
        }
    }
if(!outfile) {
    strcpy(string,glmfile);
    if(!(strptr=get_tail_sans_ext(string))) exit(-1);
    strcat(string,"_metrics.txt");
    outfile = string;
    }
if(!(fp=fopen_sub(outfile,"w"))) exit(-1);
for(maxlen=6,i=0;i<glm->ifh->glm_tot_eff;i++) if(glm->ifh->glm_leffect_label[i]>maxlen) maxlen=glm->ifh->glm_leffect_label[i]; 
fprintf(fp,"# glmfile : %s\n",glmfile);
if(cnt1) fprintf(fp,"# temporalmask : %s\n",txtfile);
fprintf(fp,"#\n");

/*fprintf(fp,"%-*sframe number of trials\n%-*s----- ----------------\n",maxlen+1,"effect",maxlen+1,"------");*/
fprintf(fp,"#%-*sframe originaltrials remainingtrials\n#%-*s----- -------------- ---------------\n",maxlen+1,"effect",maxlen+1,"------");

for(k=i=0;i<glm->ifh->glm_tot_eff;i++) {
    for(j=0;j<glm->ifh->glm_effect_length[i];j++,k++) {
        fprintf(fp,"%-*s%3d\t%3d",maxlen+1,glm->ifh->glm_effect_label[i],j+1,cnt[k]);

        /*if(cnt1) fprintf(fp,"\t%3d",cnt1[k]);*/
        if(cnt1) fprintf(fp,"\t\t%3d",cnt1[k]);

        fprintf(fp,"\n");
        }
    }
fclose(fp);
printf("Output written to %s\n",outfile);
}
