/* Copyright 9/12/14 Washington University.  All Rights Reserved.
   fidl_fdr.c  $Revision: 1.2 $ */
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "subs_util.h"
#include "read_data.h"
#include "dim_param2.h"
#include "filetype.h"
#include "subs_mask.h"
#include "spatial_extent2.h"
#include "write1.h"
#include "shouldiswap.h"
#include "subs_cifti.h"
#include "subs_nifti.h"
#include "minmax.h"
#include "fdr_guts.h"
#include "get_atlas_param.h"
#include "center_of_mass2.h"
#include "assign_region_names2.h"
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_fdr.c,v 1.2 2016/08/17 17:27:28 mcavoy Exp $";
int main(int argc,char **argv){
char *file=NULL,*mask=NULL,*montecarlopath=NULL,atlas[7]="",tablef[MAXNAME],zstr[2000],line[MAXNAME],*strptr,*saveptr,
    filename[MAXNAME];
int i,j,k,l,lcgaussT=0,lcgaussF=0,lcpeak=0,lccom=0,lcgaussT0=0,lcgaussF0=0,SunOS_Linux,mode=3,*nreg,*clussize; //,regstart=2;
size_t *p,j1;
int64_t dims[3];
float fwhm=-1.,*z,*actmask,min,max,regstart=2.;
double thresh=3.0,q=0.5,*crushed;
FILE *fp;
Interfile_header *ifh=NULL;
Data *data;
Dim_Param2 *dp;
Mask_Struct *ms;
Spatial_Extent2 *se;
W1 *w1=NULL;
XmlS *xmls;
Atlas_Param *ap=NULL;
center_of_mass2struct *com=NULL;
assign_region_names2struct *arn=NULL;
print_version_number(rcsid,stdout);
if(argc<7){
    fprintf(stderr,"Required.\n");
    fprintf(stderr,"    -file:           Gaussianized T or F.\n");
    fprintf(stderr,"    -montecarlopath: Path to cluster significance probabilities.\n");
    fprintf(stderr,"Optional.\n");
    fprintf(stderr,"    -thresh: Friston and Chumbley recommend a threshold of at least 2.5. Default is 3.0.\n");
    fprintf(stderr,"    -q:      This is the False Discovery Rate. Default is 0.5.\n");
    fprintf(stderr,"Only needed if not identified in the ifh.\n");
    fprintf(stderr,"    -mask:   Limit false discoveries to this mask.\n");
    fprintf(stderr,"    -fwhm:   Image smoothing in voxels.\n");
    fprintf(stderr,"    -gaussT  File is gaussianized T statistic.\n");
    fprintf(stderr,"    -gaussF  File is gaussianized F statistic.\n");
    fprintf(stderr,"Output. Default is to output the map and region file.\n");
    fprintf(stderr,"    -map_only       Output FDR corrected image only.\n");
    fprintf(stderr,"    -regions_only   Output region file only.\n");
    fprintf(stderr,"    -peak           Region name is peak atlas coordinate. Default. \n");
    fprintf(stderr,"    -center_of_mass Region name is center of mass atlas coordinate.\n");
    fprintf(stderr,"    If both -peak and -center_of_mass, then region name includes both.\n");
    exit(-1);
    } 
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-file") && argc > i+1) file = argv[++i];
    if(!strcmp(argv[i],"-montecarlopath") && argc > i+1) montecarlopath = argv[++i];
    if(!strcmp(argv[i],"-thresh") && argc > i+1) thresh=strtod(argv[++i],NULL);
    if(!strcmp(argv[i],"-q") && argc > i+1) q=strtod(argv[++i],NULL);
    if(!strcmp(argv[i],"-mask") && argc > i+1) mask = argv[++i];
    if(!strcmp(argv[i],"-fwhm") && argc > i+1) fwhm=strtof(argv[++i],NULL);
    if(!strcmp(argv[i],"-gaussT")) lcgaussT=1;
    if(!strcmp(argv[i],"-gaussF")) lcgaussF=1;
    if(!strcmp(argv[i],"-peak")) lcpeak=1; 
    if(!strcmp(argv[i],"-center_of_mass")) lccom=1;
    if(!strcmp(argv[i],"-regions_only")) mode ^= 1 << 0; //toggle bit 0 
    if(!strcmp(argv[i],"-map_only")) mode ^= 1 << 1; //toggle bit 1 
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!file){printf("fidlError: Need to specify -file Abort!\n");exit(-1);}
if(!montecarlopath){printf("fidlError: Need to specify -montecarlopath Abort!\n");exit(-1);}
if(!mode){printf("fidlError: By setting, -regions_only and -map_only you have turned off all output. Turning it back on!\n");mode=3;}
if(!lcpeak&&!lccom)lcpeak=1;
if(lcgaussT&&lcgaussF){printf("fidlError: Both -gaussT and -gaussF have been specified. Only one allowed. Abort!\n");exit(-1);}
if(!(dp=dim_param2(1,&file,SunOS_Linux)))exit(-1);
if(dp->filetype[0]==(int)IMG){
    if(!(ifh=read_ifh(file,(Interfile_header*)NULL)))exit(-1);
    if(!ifh->mask&&!mask){printf("fidlError: No mask listed in ifh. Need to specify with -mask Abort!\n");exit(-1);}
    if(ifh->mask){
        if(mask&&strcmp(ifh->mask,mask)){
            printf("fidlError: mask=%s in ifh but -mask %s. Must be the same. Abort!\n",ifh->mask,mask);exit(-1);
            }
        mask=ifh->mask;
        }
    if(ifh->fwhm<0.&&fwhm<0.){printf("fidlError: No smoothing listed in ifh. Need to specify with -fwhm Abort!\n");exit(-1);}
    if(ifh->fwhm>=0.){
        if(fwhm>=0.&&(ifh->fwhm!=fwhm)){
            printf("fidlError: fwhm=%f in ifh but -fwhm %f Must be equal. Abort!\n",ifh->fwhm,fwhm);exit(-1);
            }
        fwhm=ifh->fwhm;
        }
    if(!ifh->datades&&!lcgaussT&&!lcgaussF){
        printf("fidlError: No data description listed in ifh. Need to specify either -gaussT or -gaussF. Abort!\n");exit(-1);}
    if(ifh->datades){
        if(!strcmp("gaussianizedT",ifh->datades))lcgaussT0=1;
        else if(!strcmp("gaussianizedF",ifh->datades))lcgaussF0=1;
        if(lcgaussT&&!lcgaussT0){
            printf("fidlError: data description=%s but -gaussT has been set. Contradiction. Abort!\n",ifh->datades);exit(-1);
            }
        if(lcgaussF&&!lcgaussF0){
            printf("fidlError: data description=%s but -gaussF has been set. Contradiction. Abort!\n",ifh->datades);exit(-1);
            }
        lcgaussT=lcgaussT0;lcgaussF=lcgaussF0;
        }
    }
else{
    if(!mask){printf("fidlError: Need to specify mask with -mask Abort!\n");exit(-1);}
    if(fwhm<0.){printf("fidlError: Need to specify smoothing in voxels with -fwhm Abort!\n");exit(-1);}
    if(!lcgaussT&&!lcgaussF){printf("fidlError: Need to specify either -gaussT or -gaussF. Abort!\n");exit(-1);}
    }
if(!get_atlas(dp->vol[0],atlas)){printf("fidlError: Unknown atlas. Abort!\n");exit(-1);}
sprintf(tablef,"%s/Z_fwhm%d_%s_table.txt",montecarlopath,(int)rint(fwhm),atlas);
if(!(fp=fopen_sub(tablef,"r")))exit(-1);
fgets(line,sizeof(line),fp);
saveptr=strtok_r(line,"# 	",&strptr);
if(strcmp(saveptr,mask)){printf("fidlError: mask=%s but the mask listed in %s is %s. Must be the same.  Abort!\n",mask,tablef,
    saveptr);exit(-1);}
fclose(fp);
if(!(ms=get_mask_struct(mask,0,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,0)))exit(-1);
if(dp->vol[0]!=ms->lenvol){
    printf("fidlError: dp->vol[0]=%d by ms->lenvol=%d Must be the same. Abort!\n",dp->vol[0],ms->lenvol);exit(-1);}
if(!(se=spatial_extent2init(ms->xdim,ms->ydim,ms->zdim,ms->lenbrain,ms->brnidx)))exit(-1);
printf("Reading %s\n",tablef);
if(!(data=read_data(tablef,0,1,0,0)))exit(-1);
//for(i=0;i<data->nsubjects;i++){
//    for(j=3;j<6;j++)printf("%f ",data->x[i][j]);
//    printf("\n");
//    }
if(dp->tdim[0]>1)printf("%s has a time dimension of %d. Each stack will be corrected.\n",file,dp->tdim[0]);
if(!(z=malloc(sizeof*z*dp->lenvol_max))){printf("fidlError: Unable to malloc z\n");exit(-1);}
if(!(crushed=malloc(sizeof*crushed*dp->vol[0]))){printf("fidlError: Unable to malloc crushed\n");exit(-1);}
for(i=0;i<dp->vol[0];i++)crushed[i]=0.;
if(!(actmask=malloc(sizeof*actmask*dp->lenvol_max))){printf("fidlError: Unable to malloc actmask\n");exit(-1);}
if(!(p=malloc(sizeof*p*ms->lenbrain))){printf("fidlError: Unable to malloc p\n");exit(-1);}
if(!(nreg=malloc(sizeof*nreg*dp->tdim[0]))){printf("fidlError: Unable to malloc nreg\n");exit(-1);}
if(!(clussize=malloc(sizeof*clussize*dp->tdim[0]))){printf("fidlError: Unable to malloc clussize\n");exit(-1);}
if(((mode>>1)&1)&&(dp->filetypeall==(int)IMG)&&(dp->tdim[0]==1)){
    if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL)))exit(-1);
    if(!(com=center_of_mass2init(ms->lenbrain,ms->lenbrain))) exit(-1);
    if(!(arn=assign_region_names2init(ms->lenbrain))) exit(-1);
    }
if(!(w1=malloc(sizeof*w1))){
    printf("fidlError: Unable to malloc w1\n");
    exit(-1);
    }
w1->filetype=dp->filetypeall;
w1->temp_float=z;
w1->swapbytes=shouldiswap(SunOS_Linux,dp->bigendian[0]); //need for region file which is IMG
if(dp->filetypeall==(int)IMG){
    w1->dontminmax=1;
    w1->how_many=dp->lenvol_max;
    w1->ifh=ifh;
    }
else if(dp->filetype[i]==(int)CIFTI||dp->filetype[i]==(int)CIFTI_DTSERIES){
    if(!(xmls=cifti_getxml(file)))exit(-1);
    w1->xmlsize=xmls->size;;
    w1->cifti_xmldata=xmls->data;
    dims[0]=(int64_t)dp->xdim[0];dims[1]=(int64_t)dp->ydim[0];dims[2]=(int64_t)dp->zdim[0];
    w1->dims=dims;
    w1->tdim=(int64_t)dp->tdim[0];
    }
else if(dp->filetype[i]==(int)NIFTI){
    w1->file_hdr=file;
    }
if(dp->filetypeall==(int)IMG){if(!readstack(file,z,sizeof*z,(size_t)dp->lenvol_max,SunOS_Linux,dp->bigendian[0]))exit(-1);}
else if(dp->filetypeall==(int)CIFTI){if(!cifti_getstack(file,z))exit(-1);}
else if(!nifti_read(file,z))exit(-1);
min_and_max_init(&min,&max);
for(k=i=0;i<dp->tdim[0];i++,k+=dp->vol[0]){
    for(j=0;j<ms->lenbrain;j++)
        crushed[ms->brnidx[j]]=z[k+ms->brnidx[j]]==(float)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:(double)z[k+ms->brnidx[j]];
    nreg[i]=fdr_guts(crushed,&z[k],&actmask[k],thresh,se,data->x,q,&regstart,&clussize[i],&min,&max,p);
    //printf("nreg[%d]=%d regstart=%f clussize[%d]=%d min=%f max=%f\n",i,nreg[i],regstart,i,clussize[i],min,max);
    }
strcpy(zstr,file);
if(!(strptr=get_tail_sans_ext(zstr)))exit(-1);
if(dp->tdim[0]==1)sprintf(line,"%s_FDRz%.2fq%gn%d",strptr,thresh,q,clussize[0]);
else sprintf(line,"%s_FDRz%.2fq%g",strptr,thresh,q);
if(mode&1){
    sprintf(filename,"%s.4dfp.img",line);
    if(!write1(filename,w1))exit(-1);
    printf("FDR corrected image written to %s\n",filename);fflush(stdout);
    }
if(nreg[0]&&com){
    sprintf(filename,"%s_reg.4dfp.img",line);
    if(!writestack(filename,actmask,sizeof(float),(size_t)dp->vol[0],w1->swapbytes))exit(-1);
    for(j=i=0;i<se->nreg;i++)if(se->regsize[i]>=clussize[0])p[j++]=(size_t)i;
    if(j!=nreg[0]){printf("fidlError: j=%d nreg[0]=%d Must be equal. Abort!\n",j,nreg[0]);exit(-1);}
    for(k=i=0;i<nreg[0];i++){
        for(l=0,j1=0;j1<p[i];j1++)l+=se->regsize[j1];
        for(j=0;j<se->regsize[p[i]];j++,k++)se->reglist[k]=se->reglist[l++];
        }
    for(se->nvox=i=0;i<nreg[0];i++)se->nvox+=(se->regsize[i]=se->regsize[p[i]]);
    center_of_mass2(z,com,se->nvox,se->reglist,nreg[0],se->regsize,ap);
    if(!assign_region_names2(nreg[0],arn,ifh,se->regsize,!lccom?(double*)NULL:com->coor,!lcpeak?(double*)NULL:com->peakcoor,
        !lcpeak?(float*)NULL:com->peakval,(char**)NULL))exit(-1);
    ifh->global_min=2.;ifh->global_max=(float)(nreg[0]+1);
    if(!write_ifh(filename,ifh,0)) exit(-1);
    printf("Region file written to %s\n",filename);
    }
}
