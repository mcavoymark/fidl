/* Copyright 5/4/17 Washington University.  All Rights Reserved.
   fidl_sumconcpair.c  $Revision: 1.2 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h"
#include "subs_mask.h"
#include "dim_param2.h"
#include "get_atlas_param.h"
#include "subs_util.h"
#include "t4_atlas.h"
#include "filetype.h"
#include "subs_cifti.h"
#include "subs_nifti.h"
#include "write1.h"
//#include "minmax.h"
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_sumconcpair.c,v 1.2 2017/05/09 16:43:46 mcavoy Exp $";
char atlas2[7]="";
int main(int argc,char **argv){
char *mask_file=NULL,atlas[7]="",string1[MAXNAME],string2[MAXNAME],*strptr1,*strptr2,filename[MAXNAME],concname[MAXNAME],*pwd,
    atlas1[7]="";
int i,j,k,nfiles1=0,nfiles2=0,nxform_file1=0,SunOS_Linux,*A_or_B_or_U1=NULL,tdim_min0,tdim_min,vol,t1,t2,argc1=0,argc2=0;
size_t i1,files_min;
float *t41=NULL,*temp_float,*temp_float1=NULL,*temp_float2=NULL,*temp_float3,*fptr;
double *temp_double,*dptr,*stat=NULL;
Files_Struct *files1=NULL,*files2=NULL,*xform_file1=NULL;
Dim_Param2 *dp1,*dp2;
Mask_Struct *ms,*ms_bold1=NULL,*ms_bold2=NULL;
Atlas_Param *ap;
W1 *w1=NULL;
Interfile_header *ifh=NULL;
XmlS *xmls;
int64_t dims[3];
Memory_Map *mm1=NULL,*mm2=NULL;
void *niftiPtr1=NULL,*niftiPtr2=NULL;
FILE *op=NULL,*fp;
if(argc<5) {
    fprintf(stderr,"Sum two volumes. Output has shortest time dimension.\n");
    fprintf(stderr,"  -files1:      imgs, concs, lists\n");
    fprintf(stderr,"  -files2:      imgs, concs, lists\n");
    fprintf(stderr,"  -xform_file1: t4s, concs, lists; t4(s) for -files1.\n");
    fprintf(stderr,"  -mask:        Only voxels in the mask are analyzed. Output dimensions are kept.\n");
    }
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-files1") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfiles1;
        //if(!(files1=read_files(nfiles1,&argv[i+1])))exit(-1);
        if(!(files1=read_files(nfiles1,&argv[(argc1=i+1)])))exit(-1);
        i+=nfiles1;
        }
    if(!strcmp(argv[i],"-files2") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfiles2;
        if(!(files2=read_files(nfiles2,&argv[(argc2=i+1)])))exit(-1);
        i+=nfiles2;
        }
    if(!strcmp(argv[i],"-xform_file1") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nxform_file1;
        if(!(xform_file1=read_files(nxform_file1,&argv[i+1])))exit(-1);
        i+=nxform_file1;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file=argv[++i];
    }
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1)exit(-1);
if(!(pwd=getenv("PWD"))) {
    printf("fidlError: Unable to get environment variable PWD\n");
    exit(-1);
    }
if(!nfiles1){printf("fidlError: Need to specify -files1\n");exit(-1);}
if(!nfiles2){printf("fidlError: Need to specify -files2\n");exit(-1);}
if(nxform_file1>1){
    if(xform_file1->nfiles!=files1->nfiles)
        {printf("fidlError: xform_file1->nfiles=%zd files1->nfiles=%zd Must be equal.\n",xform_file1->nfiles,files1->nfiles);exit(-1);}
    }
if(!(dp1=dim_param2(files1->nfiles,files1->files,SunOS_Linux)))exit(-1);
if(!(dp2=dim_param2(files2->nfiles,files2->files,SunOS_Linux)))exit(-1);
if(!(ms_bold2=ms=get_mask_struct(mask_file,dp2->volall,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp2->volall)))exit(-1);
if((vol=!xform_file1?dp1->volall:ms_bold2->lenvol)!=dp2->volall)
    {printf("%s=%d dp2->volall=%d Must be equal.\n",!xform_file1?"ms_bold2->lenvol":"dp1->volall",vol,dp2->volall);exit(-1);}
if(!get_atlas(dp2->volall,atlas)){printf("dp2->volall=%d Unknown atlas.\n",dp2->volall);exit(-1);}


#if 0
if(get_atlas(dp1->volall,atlas1)){
    if(!strcmp(atlas,atlas1)){
        printf("fidlInfo: -xform_file1 will not be used because already in %s\n",atlas1); 
        xform_file1=NULL;
        }
    }
#endif
//START170516
if(nxform_file1)if(get_atlas(dp1->volall,atlas1)){
    if(!strcmp(atlas,atlas1)){
        printf("fidlInfo: -xform_file1 will not be used because already in %s\n",atlas1);
        xform_file1=NULL;
        }
    }


if(!xform_file1)
    {ms_bold1=ms_bold2;}
else{
    if(!(ms_bold1=get_mask_struct((char*)NULL,dp2->volall,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp2->volall)))exit(-1);
    if(!(t41=malloc(sizeof*t41*(size_t)T4SIZE*xform_file1->nfiles))){
        printf("fidlError: Unable to malloc t41\n");
        exit(-1);
        }
    if(!(A_or_B_or_U1=malloc(sizeof*A_or_B_or_U1*xform_file1->nfiles))){
        printf("fidlError: Unable to malloc A_or_B_or_U1\n");
        exit(-1);
        }
    for(i1=0;i1<xform_file1->nfiles;i1++){
        if(!read_xform(xform_file1->files[i1],&t41[i1*(int)T4SIZE]))exit(-1);
        if((A_or_B_or_U1[i1]=twoA_or_twoB(xform_file1->files[i1]))==2)exit(-1);
        }
    }
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL)))exit(-1);
if(!xform_file1)ap->vol=dp2->volall;
if(!atlas[0]){
    ap->xdim=dp2->xdim[0];
    ap->ydim=dp2->ydim[0];
    ap->zdim=dp2->zdim[0];
    ap->voxel_size[0]=dp2->dxdy[0];
    ap->voxel_size[1]=dp2->dxdy[0];
    ap->voxel_size[2]=dp2->dz[0];
    }
if(xform_file1)if(!(stat=malloc(sizeof*stat*ap->vol))){printf("fidlError: Unable to malloc stat\n");exit(-1);}
if(!(temp_double=malloc(sizeof*temp_double*ap->vol))){printf("fidlError: Unable to malloc temp_double\n");exit(-1);}
if(!(temp_float=malloc(sizeof*temp_float*(dp1->volall>dp2->volall?dp1->volall:dp2->volall))))
    {printf("fidlError: Unable to malloc temp_float\n");exit(-1);}
if(dp1->filetypeall==(int)CIFTI||dp1->filetypeall==(int)CIFTI_DTSERIES)
    {if(!(temp_float1=malloc(sizeof*temp_float1*dp1->lenvol_max))){printf("fidlError: Unable to malloc temp_float1\n");exit(-1);}}
if(dp2->filetypeall==(int)CIFTI||dp2->filetypeall==(int)CIFTI_DTSERIES)
    {if(!(temp_float2=malloc(sizeof*temp_float2*dp2->lenvol_max))){printf("fidlError: Unable to malloc temp_float2\n");exit(-1);}}
if(!(w1=write1_init()))exit(-1);
w1->filetype=dp1->filetypeall;
w1->swapbytes=0;
w1->temp_float=temp_float2;
if(dp2->filetypeall==(int)IMG){
    if(!(ifh=read_ifh(files2->files[0],(Interfile_header*)NULL)))exit(-1);
    ifh->bigendian=!SunOS_Linux?1:0;
    }
else if(dp2->filetypeall==(int)CIFTI||dp2->filetypeall==(int)CIFTI_DTSERIES){
    if(!(xmls=cifti_getxml(files2->files[0])))exit(-1);
    w1->xmlsize=xmls->size;;
    w1->cifti_xmldata=xmls->data;
    dims[0]=(int64_t)dp2->xdim[0];dims[1]=(int64_t)dp2->ydim[0];dims[2]=(int64_t)dp2->zdim[0];
    w1->dims=dims;
    }
else if(dp2->filetypeall==(int)NIFTI){
    w1->file_hdr=files2->files[0];
    }
files_min=files1->nfiles<files2->nfiles?files1->nfiles:files2->nfiles;
if(dp2->filetypeall==(int)CIFTI||dp2->filetypeall==(int)CIFTI_DTSERIES)
    {for(tdim_min0=dp2->tdim[0],i1=0;i1<files_min;i1++)
        if(dp1->tdim[i1]<dp2->tdim[i1]&&dp1->tdim[i1]>tdim_min0)tdim_min0=dp1->tdim[i1];}
else
    {tdim_min0=1;}
if(!(temp_float3=malloc(sizeof*temp_float3*ap->vol*tdim_min0))){printf("fidlError: Unable to malloc temp_float3\n");exit(-1);}
for(i=0;i<ap->vol*tdim_min0;i++)temp_float3[i]=0.;
strcpy(string1,argv[argc1]);
if(!(strptr1=get_tail_sans_ext(string1)))exit(-1);
strcpy(string2,argv[argc2]);
if(!(strptr2=get_tail_sans_ext(string2)))exit(-1);
sprintf(concname,"%s+%s.conc",strptr1,strptr2);
if(!(fp=fopen_sub(concname,"w"))) exit(-1);
for(i1=0;i1<files_min;i1++){
    if(dp1->filetypeall==(int)IMG){
        if(!(mm1=map_disk(files1->files[i1],dp1->vol[i1]*dp1->tdim[i1],0,sizeof(float))))exit(-1);
        }
    else if(dp1->filetypeall==(int)CIFTI||dp1->filetypeall==(int)CIFTI_DTSERIES){
        if(!cifti_getstack(files1->files[i1],temp_float1))exit(-1);
        }
    else if(!(niftiPtr1=nifti_openRead(files1->files[i1])))return 0;
    if(dp2->filetypeall==(int)IMG){
        if(!(mm2=map_disk(files2->files[i1],dp2->vol[i1]*dp2->tdim[i1],0,sizeof(float))))exit(-1);
        }
    else if(dp2->filetypeall==(int)CIFTI||dp2->filetypeall==(int)CIFTI_DTSERIES){
        if(!cifti_getstack(files2->files[i1],temp_float2))exit(-1);
        }
    else if(!(niftiPtr2=nifti_openRead(files2->files[i1])))return 0;
    strcpy(string1,files1->files[i1]);
    if(!(strptr1=get_tail_sans_ext(string1)))exit(-1);
    strcpy(string2,files2->files[i1]);
    if(!(strptr2=get_tail_sans_ext(string2)))exit(-1);
    sprintf(filename,"%s+%s%s",strptr1,strptr2,Fileext[dp2->filetypeall]);
    if(dp1->filetypeall==(int)IMG||dp1->filetypeall==(int)NIFTI){if(!(op=open2(filename,w1)))exit(-1);}
    tdim_min=dp1->tdim[i1]<dp2->tdim[i1]?dp1->tdim[i1]:dp2->tdim[i1];
    for(i=0;i<tdim_min;i++){
        t1=dp1->vol[i1]*i;
        if(!xform_file1){for(j=0;j<dp1->vol[i1];j++)temp_double[j]=0.;}
        if(dp1->filetypeall==(int)IMG){
            if(!dp1->swapbytes[i1]){
                fptr=&mm1->ptr[t1];
                }
            else{
                for(k=t1,j=0;j<dp1->vol[i1];j++,k++)temp_float[j]=mm1->ptr[k];
                swap_bytes_idx((unsigned char *)temp_float,sizeof(float),ms_bold1->lenbrain,ms_bold1->brnidx);
                fptr=temp_float;
                }
            }
        else if(dp1->filetypeall==(int)CIFTI||dp1->filetypeall==(int)CIFTI_DTSERIES){
            fptr=&temp_float1[t1];
            }
        else{
            if(!nifti_getvol(niftiPtr1,(int64_t)t1,temp_float))return 0;
            fptr=temp_float;
            }
        for(j=0;j<ms_bold1->lenbrain;j++)temp_double[ms_bold1->brnidx[j]]=
            fptr[ms_bold1->brnidx[j]]==(float)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:(double)fptr[ms_bold1->brnidx[j]]; 
        dptr=temp_double;
        if(xform_file1){
            if(!t4_atlas(temp_double,stat,&t41[(xform_file1->nfiles>1?i1:0)*(size_t)T4SIZE],dp1->xdim[i1],dp1->ydim[i1],dp1->zdim[i1],
                dp1->dxdy[i1],dp1->dz[i1],A_or_B_or_U1[i1],dp1->orientation[i1],ap,(double*)NULL))exit(-1);
            dptr=stat;
            }
        t2=dp2->vol[i1]*i;
        if(dp2->filetypeall==(int)IMG){
            if(!dp2->swapbytes[i1]){
                fptr=&mm2->ptr[t2];
                }
            else{
                for(k=t2,j=0;j<dp2->vol[i1];j++,k++)temp_float[j]=mm2->ptr[k];
                swap_bytes_idx((unsigned char *)temp_float,sizeof(float),ms_bold2->lenbrain,ms_bold2->brnidx);
                fptr=temp_float;
                }
            }
        else if(dp2->filetypeall==(int)CIFTI||dp2->filetypeall==(int)CIFTI_DTSERIES){
            fptr=&temp_float2[t2];
            }
        else{
            if(!nifti_getvol(niftiPtr2,(int64_t)t2,temp_float))return 0;
            fptr=temp_float;
            }
        k=dp1->filetypeall==(int)CIFTI||dp1->filetypeall==(int)CIFTI_DTSERIES?t2:0;
        for(j=0;j<ms->lenbrain;j++)temp_float3[k+ms->brnidx[j]]=
            fptr[ms->brnidx[j]]==(float)UNSAMPLED_VOXEL||dptr[ms->brnidx[j]]==(double)UNSAMPLED_VOXEL?
            (float)UNSAMPLED_VOXEL:(float)(dptr[ms->brnidx[j]]+(double)fptr[ms->brnidx[j]]);
        if(dp1->filetypeall==(int)IMG){
            if(!fwrite_sub(temp_float3,sizeof(float),(size_t)ap->vol,op,0)){
                printf("fidlError: Could not write to %s\n",filename);
                exit(-1);
                }
            }
        else if(dp1->filetypeall==(int)NIFTI){
            if(!nifti_putvol(op,(int64_t)t2,temp_float3))exit(-1);
            }
        }
    if(dp1->filetypeall==(int)IMG){if(!unmap_disk(mm1))exit(-1);}else if(dp1->filetypeall==(int)NIFTI){nifti_free(niftiPtr1);}
    if(dp2->filetypeall==(int)IMG){if(!unmap_disk(mm2))exit(-1);}else if(dp2->filetypeall==(int)NIFTI){nifti_free(niftiPtr2);}
    if(op)fclose(op);
    if(dp2->filetypeall==(int)IMG){
        ifh->dim4=tdim_min;
        if(!write_ifh(filename,ifh,0))exit(-1);
        }
    else if(dp2->filetypeall==(int)CIFTI||dp2->filetypeall==(int)CIFTI_DTSERIES){
        w1->tdim=(int64_t)tdim_min;
        if(!write1(filename,w1))exit(-1);
        }
    printf("Output written to %s\n",filename);
    fprintf(fp,"%s/%s\n",pwd,filename);
    }
fclose(fp);
printf("Output written to %s\n",concname);
exit(0);
}
