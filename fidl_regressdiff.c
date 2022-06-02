/* Copyright 1/2/09 Washington University.  All Rights Reserved.
   fidl_regressdiff.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_regressdiff.c,v 1.2 2009/01/06 21:05:51 mcavoy Exp $";

main(int argc,char **argv)
{
char *maskfile=NULL,*root=NULL,filename[MAXNAME],*tcx1file=NULL,*tcx2file=NULL,*tcy1file=NULL,*tcy2file=NULL,**fileptr;
int i,j,ii,SunOS_Linux,vol,xdim,ydim,zdim,swapbytes,lenvol,atlas,len,ncol;
float *temp_float,*mask,*tcx1,*tcx2,*tcy1,*tcy2;
double *Astack,*bstack,*dptr;
Interfile_header *ifh;
Mask_Struct *ms;
Dim_Param *dp;
Atlas_Param *ap;

if(argc<7) {
    fprintf(stderr,"    -mask:    Optional. Only mask voxels are included in the computation.\n");
    fprintf(stderr,"    -root:    Output root.\n");
    fprintf(stderr,"    -tcx1:      This is computed as the sum over all frames i abs(tcx1[i]-tcx2[i]).\n");
    fprintf(stderr,"    -tcx2:\n");
    fprintf(stderr,"    -tcy1:      This is computed as the sum over all frames i abs(tcy1[i]-tcy2[i]).\n");
    fprintf(stderr,"    -tcy2:\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        maskfile = argv[++i];
    if(!strcmp(argv[i],"-root") && argc > i+1)
        root = argv[++i];
    if(!strcmp(argv[i],"-tcx1") && argc > i+1)
        tcx1file = argv[++i];
    if(!strcmp(argv[i],"-tcx2") && argc > i+1)
        tcx2file = argv[++i];
    if(!strcmp(argv[i],"-tcy1") && argc > i+1)
        tcy1file = argv[++i];
    if(!strcmp(argv[i],"-tcy2") && argc > i+1)
        tcy2file = argv[++i];
    }
if(!root) {
    printf("Error: Need to specify -root\n");
    exit(-1);
    }
if(!tcx1file||!tcx2file) {
    printf("Error: Need to specify -tcx1 and -tcx2\n");
    exit(-1);
    }
if(!tcy1file||!tcy2file) {
    printf("Error: Need to specify -tcy1 and -tcy2\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(fileptr=malloc(sizeof*fileptr*4))) {
    printf("Error: Unable to malloc fileptr\n");
    exit(-1);
    }
fileptr[0]=tcx1file;
fileptr[1]=tcx2file;
fileptr[2]=tcy1file;
fileptr[3]=tcy2file;
if(!(dp=dim_param(4,fileptr,SunOS_Linux))) exit(-1);
vol=dp->vol;
if(!dp->all_tdim_same) {
    printf("Error: The time dimension of all files must be the same.\n");
    exit(-1);
    }
atlas=get_atlas(vol);
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);

/*printf("maskfile=%s\n",maskfile);*/
if(!(ms=get_mask_struct(maskfile,vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
/*for(i=0;i<ms->lenbrain;i++) printf("ms->brnidx[%d]=%d\n",i,ms->brnidx[i]);*/
/*exit(-1);*/

if(ms->lenvol!=vol) {
    printf("Error: ms->lenvol=%d vol=%d Must be equal.\n",ms->lenvol,vol);
    exit(-1);
    }

/*START0*/
len=dp->tdim_max;
lenvol=len*vol;
ncol=2;
if(!(tcx1=malloc(sizeof*tcx1*lenvol))) {
    printf("Error: Unable to malloc tcx1\n");
    exit(-1);
    }
if(!(tcx2=malloc(sizeof*tcx2*lenvol))) {
    printf("Error: Unable to malloc tcx2\n");
    exit(-1);
    }
if(!(tcy1=malloc(sizeof*tcy1*lenvol))) {
    printf("Error: Unable to malloc tcy1\n");
    exit(-1);
    }
if(!(tcy2=malloc(sizeof*tcy2*lenvol))) {
    printf("Error: Unable to malloc tcy2\n");
    exit(-1);
    }
if(!readstack(tcx1file,(float*)tcx1,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
if(!readstack(tcx2file,(float*)tcx2,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
if(!readstack(tcy1file,(float*)tcy1,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
if(!readstack(tcy2file,(float*)tcy2,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
if(!(Astack=malloc(sizeof*Astack*len*ncol))) {
    printf("Error: Unable to malloc Astack\n");
    exit(-1);
    }
if(!(bstack=malloc(sizeof*bstack*len))) {
    printf("Error: Unable to malloc bstack\n");
    exit(-1);
    }
gsl_matrix_view m = gsl_matrix_view_array(Astack,len,ncol);
gsl_vector_view b = gsl_vector_view_array(bstack,len);
gsl_vector *tau = gsl_vector_alloc(ncol);
gsl_vector *x = gsl_vector_alloc(ncol);
gsl_vector *residual = gsl_vector_alloc(len);
if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<vol;i++) temp_float[i]=0.;

for(j=0;j<ms->lenbrain;j++) {
    for(dptr=Astack,ii=ms->brnidx[j],i=0;i<len;i++,ii+=vol) {
        if(tcx1[ii]==(float)UNSAMPLED_VOXEL||tcx2[ii]==(float)UNSAMPLED_VOXEL||tcy1[ii]==(float)UNSAMPLED_VOXEL||
            tcy2[ii]==(float)UNSAMPLED_VOXEL) {
            temp_float[ms->brnidx[j]]=(float)UNSAMPLED_VOXEL;
            break;
            }
        *dptr++=1.;
        *dptr++=fabs((double)(tcx1[ii]-tcx2[ii]));
        bstack[i]=fabs((double)(tcy1[ii]-tcy2[ii]));
        }
    gsl_linalg_QR_decomp(&m.matrix,tau);
    gsl_linalg_QR_lssolve(&m.matrix,tau,&b.vector,x,residual);
    temp_float[ms->brnidx[j]]=(float)(gsl_vector_get(x,1)/gsl_vector_get(x,0));
    }
if(!(ifh=init_ifh(4,dp->xdim,dp->ydim,dp->zdim,1,dp->dxdy,dp->dxdy,dp->dz,dp->bigendian[0]))) exit(-1);
swapbytes = shouldiswap(SunOS_Linux,dp->bigendian[0]);
sprintf(filename,"%s.4dfp.img",root);
if(!writestack(filename,temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
min_and_max_init(&ifh->global_min,&ifh->global_max);
min_and_max_floatstack(temp_float,vol,&ifh->global_min,&ifh->global_max);
if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
printf("Output written to %s\n",filename);
/*END0*/
}
