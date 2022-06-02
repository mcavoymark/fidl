/**********
check_glm.c
**********/

/*$Revision: 1.2 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fidl.h>

main(int argc,char **argv)
{

char *input_file,glmfile[MAXNAME];

int i,lc_echo=FALSE,lc_glm_4dfp=FALSE;

LinearModel *glm;

FILE *fp;


if (argc < 4) {
    fprintf(stderr,"Usage: check_glm -glm_file file_name -echo -glm_4dfp\n");
    fprintf(stderr,"        -glm_file:     *.glm file that contains the linear model.\n");
    fprintf(stderr,"        -echo          Echos some of the glm fields to the terminal.\n"); 
    fprintf(stderr,"        -glm_4dfp      Creates .dat and .dof files.\n"); 
    fprintf(stderr,"       You must select include either -echo or -glm_4dfp (or both).\n"); 
    exit(-1);
    }

for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-glm_file") && argc > i+1)
        input_file = argv[++i];
    if(!strcmp(argv[i],"-echo"))
        lc_echo = TRUE;
    if(!strcmp(argv[i],"-glm_4dfp"))
        lc_glm_4dfp = TRUE;
    }
if(lc_echo==FALSE && lc_glm_4dfp==FALSE) {
    fprintf(stdout,"Error: You must select include either -echo or -glm_4dfp (or both).\n");
    exit(-1);
    }

*(strrchr(input_file,'.')) = 0;
sprintf(glmfile,"%s.glm",input_file);
glm = read_glm(glmfile,TRUE); 

if(lc_echo == TRUE) {
    fprintf(stdout,"%s\n",glmfile);
    fprintf(stdout,"total number of effects = %d\n",glm->ifh->glm_all_eff);
    for(i=0;i<glm->ifh->glm_all_eff;i++)
        fprintf(stdout,"        %d  %s\n",i+1,glm->ifh->glm_effect_label[i]);
    fprintf(stdout,"number of contrasts     = %d\n",glm->ifh->glm_nc);
    for(i=0;i<glm->ifh->glm_nc;i++)
        fprintf(stdout,"        %d  %s\n",i+1,glm->contrast_labels[i]);
    fprintf(stdout,"degrees of freedom      = %f\n",glm->ifh->glm_df);
    fprintf(stdout,"BOLD response duration  = %d\n",glm->ifh->glm_period);
    fprintf(stdout,"TR                      = %f\n",glm->ifh->glm_TR);
    }

if(lc_glm_4dfp == TRUE) {
    sprintf(glmfile,"%s.dat",input_file);
    if(!(fp = fopen(glmfile,"w"))) {
        fprintf(stderr,"Could not open %s in check_glm.\n",glmfile);
        exit(-1);
        }
    fprintf(fp,"TR %f\n",glm->ifh->glm_TR);
    fprintf(fp,"prestim 0.000000\n");
    fprintf(fp,"nbins 1\n");
    fprintf(fp,"nperevent %d\n",glm->ifh->glm_period);
    fclose(fp);
    fprintf(stdout,"Created file %s\n",glmfile);

    sprintf(glmfile,"%s.dof",input_file);
    if(!(fp = fopen(glmfile,"w"))) {
        fprintf(stderr,"Could not open %s in check_glm.\n",glmfile);
        exit(-1);
        }
    fprintf (fp, "0 %d %d\n",glm->ifh->glm_num_trials,(int)(glm->ifh->glm_df+.5));
    fclose(fp);
    fprintf(stdout,"Created file %s\n",glmfile);
    }
    
} 
