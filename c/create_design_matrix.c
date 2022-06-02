/* Copyright 12/31/99 Washington University.  All Rights Reserved.
$Id: create_design_matrix.c,v 1.5 2001/05/17 23:56:35 mcavoy Exp $
$Log: create_design_matrix.c,v $
 * Revision 1.5  2001/05/17  23:56:35  mcavoy
 * *** empty log message ***
 *
 * Revision 1.4  2001/05/16  23:39:23  mcavoy
 * *** empty log message ***
 *
 * Revision 1.3  2000/12/21  18:26:51  jmo
 * *** empty log message ***
 *
   create_design_matrix.c  $Revision: 1.5 $ */
/****************************************************************

 Routine: create_design_matrix
 
  Purpose: Create design matrix for use by linmod
  
   By: John Ollinger
   
    Date: 12/4/00
    
*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <nrutil.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/create_design_matrix.c,v 1.5 2001/05/17 23:56:35 mcavoy Exp $";

/************************/
main(int argc,char **argv)
/************************/

{
    
float	TR,scl,delay,fmin,version;
    
double	**A,**ATA,**ATAm1,fscl,ifreq,t;
    
int	i,ioff,j,joff,lc_trend,num_components,num_runs,num_skip,tdim_file,
        stat,lenvol,tcomp,next_col,run,effect,col,nF;
    
char hdrfile[MAXNAME],*glm_file,*bold_stem,date[10],dum[MAXNAME];
    
Interfile_header *ifh;
    
LinearModel	*glm;

sscanf(rcsid,"%s %s %f %s %s %s %s %s",dum,dum,&version,date,dum,dum,dum,dum);
fprintf(stdout,"\ncreate_design_matrix Version %3.2f, %s\n\n",version,date);
if (argc < 3) {
    fprintf(stderr,"Usage: create_design_matrix glm_file_name -trend -hipass num_components -num_runs number_of_runs -bold1 first_bold_4dfp -num_skip number_frames_to_skip -TR repetition_time_sec\n"); 
    fprintf(stderr,"glm_file: Output glm file name.\n");
    fprintf(stderr,"-trend: Model linear trend (but not mean).\n");
    fprintf(stderr,"hipass num_components: Include high pass filter with with a cutoff\n");
    fprintf(stderr,"of num_components*4/((N-1)*TR) where N is the number of usable frames\n");
    fprintf(stderr,"per run (i.e., exclude skips) and TR is the repetition time in seconds.\n");
    fprintf(stderr,"num_runs: Number of runs in data set (assumes equal lengths)\n");
    fprintf(stderr,"bold1: Name of first 4dfp file containing BOLD data (less .4dfp.img extension)");
    fprintf(stderr,"num_skip: Number of frames to skip at beginning of each run.\n");
    return(-1);
    }

glm_file = argv[1];
lc_trend = FALSE;
num_components = 0;
for(i=2;i<argc;i++) {
    if(!strcmp(argv[i],"-trend") )
        lc_trend = (int)TRUE;
    if(!strcmp(argv[i],"-hipass") && argc > i+1) 
        num_components = atoi(argv[++i]);
    if(!strcmp(argv[i],"-num_runs") && argc > i+1) 
        num_runs = atoi(argv[++i]);
    if(!strcmp(argv[i],"-TR") && argc > i+1) 
        TR = atof(argv[++i]);
    if(!strcmp(argv[i],"-bold1") && argc > i+1) 
        bold_stem = argv[++i];
    if(!strcmp(argv[i],"-num_skip") && argc > i+1) 
        num_skip = atoi(argv[++i]);
    }

strcpy(hdrfile,bold_stem);
strcat(hdrfile,".4dfp.ifh");
if((ifh = read_ifh(hdrfile)) == NULL) {
    fprintf(stderr,"Bold file not found, %s\n",hdrfile);
    exit(-1);
    }

GETMEM_0(glm,1,LinearModel)
glm->ifh = ifh;

glm->rev = GLM_FILE_REV;
glm->xdim = ifh->dim1;
glm->ydim = ifh->dim2;
glm->zdim = ifh->dim3;
lenvol = glm->xdim*glm->ydim*glm->zdim;
tdim_file = ifh->dim4;
glm->tdim = num_runs*tdim_file;
glm->dxdy = ifh->voxel_size_1;
glm->dz = ifh->voxel_size_3;
glm->tot_eff = 1;
glm->all_eff=0;
glm->TR = TR;
glm->period = 0;
glm->num_trials = 0;
glm->Ysim = NULL;
glm->M_interest = 0;
glm->num_files = num_runs;
glm->event_file = "N/A";

/*glm->funclen = 1;
glm->functype = BOYNTON;*/

GETMEM(glm->funclen,1,int);
GETMEM(glm->functype,1,int);
glm->funclen[i] = 1;
glm->functype[i] = (int)BOYNTON;

glm->Nrow = glm->tdim - num_runs*num_skip;
glm->all_eff = 0;
if(num_components > 0) {
    glm->Mcol = 2*num_components*num_runs;
    ++glm->all_eff;
    }
if(lc_trend == TRUE) {
    glm->Mcol += 2*num_runs;
    glm->all_eff += 2;
    }

A = dmatrix(1,glm->Nrow,1,glm->Mcol);
GETMEM(glm->valid_frms,glm->tdim,float)

glm->df  = glm->Nrow - glm->Mcol;
glm->nc = 1;
GETMEM(glm->contrast_labels,glm->nc,char *)
GETMEM(glm->c,glm->Mcol*glm->nc,float)
GETMEM(glm->cnorm,glm->tot_eff*glm->nc,float)
GETMEM(glm->valid_frms,glm->tdim,float)
GETMEM(glm->Ysim,glm->tdim*glm->tot_eff,float)
GETMEM(glm->delay,glm->tot_eff,float)
GETMEM(glm->stimlen,glm->tot_eff,float)
GETMEM(glm->effect_group,glm->tot_eff,short)
GETMEM(glm->lcfunc,glm->tot_eff,short)
GETMEM(glm->effect_label,glm->all_eff,char *)
GETMEM(glm->effect_length,glm->all_eff,int)
GETMEM(glm->effect_column,glm->all_eff,int)
GETMEM(glm->F_names,10,char *)

GETMEM(glm->contrast_labels[0],strlen("Dummy")+1,char)
strcpy(glm->contrast_labels[0],"Dummy");
effect = 0;
GETMEM(glm->F_names[nF],strlen("Omnibus")+1,char)
strcpy(glm->F_names[nF],"Omnibus");
nF = 1;
if(lc_trend == TRUE) {
    GETMEM(glm->effect_label[effect],strlen("Mean")+1,char)
    strcpy(glm->effect_label[effect],"Mean");
    ++effect;
    GETMEM(glm->effect_label[effect],strlen("Trend")+1,char)
    strcpy(glm->effect_label[effect],"Trend");
    ++effect;
    GETMEM(glm->effect_label[effect],strlen("Trend")+1,char)
    strcpy(glm->effect_label[effect],"Trend");
    }
if(num_components > 0) {
    GETMEM(glm->effect_label[effect],strlen("HiPass")+1,char)
    strcpy(glm->effect_label[effect],"HiPass");
    ++effect;
    GETMEM(glm->F_names[nF],strlen("HiPass")+1,char)
    strcpy(glm->F_names[nF],"HiPass");
    ++nF;
    }

for(run=0,ioff=0;run<num_runs;run++,ioff+=tdim_file) {
    for(i=0;i<tdim_file;i++) {
        if(i<num_skip)
            glm->valid_frms[i] = 0;
        else
            glm->valid_frms[i] = 1;
        }
    }

/* Initialize. */
for(i=1;i<=glm->Mcol;i++) {
    for(j=1;j<=glm->Nrow;j++)
        A[j][i] = 0.;
    }

next_col = 1;
effect = 0;
if(lc_trend == TRUE) {
    for(i=0,col=next_col,joff=0;i<num_runs;i++,col++,joff+=tdim_file-num_skip) {
        for (j=1;j<=tdim_file-num_skip;j++) {
            A[j+joff][col] = 1.;
            }
        }
    glm->effect_column[effect] = next_col-1;
    glm->effect_length[effect] = num_runs;
    ++effect;
    scl = 2./(float)(tdim_file-num_skip);
    for(i=0,joff=0;i<num_runs;i++,col++,joff+=tdim_file-num_skip) {
        for (j=1;j<=tdim_file-num_skip;j++) {
            A[j+joff][col] = (double)j*scl - 1.;
            }
        }
    glm->effect_column[effect] = next_col+num_runs-1;
    glm->effect_length[effect] = num_runs;
    ++effect;
    next_col += 2*num_runs;
    }

tcomp = tdim_file - num_skip;
if(num_components > 0) {
    fmin = 1./(tcomp*TR);
    fscl = 2*M_PI/tcomp;
    for(run=0,ioff=0;run<num_runs;run++,ioff+=tcomp) {
        for(i=1,t=0.;i<tcomp;i++,t+=1.) {
            for(ifreq=0.,j=next_col;ifreq<num_components;j+=2,ifreq+=1.) {
                A[i+ioff][j+2*num_components*run] = sin((ifreq+1)*fscl*t);
                A[i+ioff][j+2*num_components*run+1] = cos((ifreq+1)*fscl*t);
                }
            }
        }
    glm->effect_column[effect] = next_col-1;
    glm->effect_length[effect] = 2*num_components;
    ++effect;
    next_col += 2*num_components;
    }
           
ATA = dmatrix(1,glm->Mcol,1,glm->Mcol);
ATAm1 = dmatrix(1,glm->Mcol,1,glm->Mcol);
ATA = dmatrix_mult(A,A,glm->Nrow,glm->Mcol,glm->Nrow,glm->Mcol,TRANSPOSE_FIRST);
ATAm1 = pinv(ATA,glm->Mcol,glm->Mcol,(double)0.);

glm->A = matrix(1,glm->Nrow,1,glm->Mcol);
glm->ATAm1 = matrix(1,glm->Mcol,1,glm->Mcol);
for(i=1;i<=glm->Mcol;i++) {
    for(j=1;j<=glm->Nrow;j++)
        glm->A[j][i] = (float)A[j][i];
    for(j=1;j<=glm->Mcol;j++)
        glm->ATAm1[i][j] = (float)ATAm1[i][j];
    }
glm->nF = nF;

/* Write glm to disk. */
if((stat=write_glm(glm_file,glm,WRITE_GLM_DSGN,lenvol)) == ERROR) {
    fprintf(stderr,"Error writing design info to %s\n",glm_file);
    exit(-1);
    }



}
