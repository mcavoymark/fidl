/* Copyright 2/10/17 Washington University.  All Rights Reserved.
   fidl_pcat.c  $Revision: 1.2 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "dim_param2.h"
#include "subs_mask.h"
#include "subs_util.h"
#include "read_frames_file.h"
#include "filetype.h"
#include "subs_cifti.h"
#include "subs_nifti.h"
#include "gsl_svd.h"
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_pcat.c,v 1.2 2017/03/09 21:25:25 mcavoy Exp mcavoy $";
int main(int argc,char **argv)
{
char *mask=NULL,*out=NULL,*frames_file=NULL,*strptr,*scratchdir=NULL,timestr[23],string[MAXNAME],dataf[MAXNAME],script[MAXNAME],
    rscriptcall[MAXNAME],ofile0[MAXNAME],ofile1[MAXNAME];
int i,j,k,l,q,r,nfiles=0,nfeatures=0,num_frames=0,argc_frames=0,ndep,SunOS_Linux,lcrm=0;
long val;
size_t i1;
float *temp_float,*temp_float2=NULL;
double *temp_double,*Y,*A,*V,*S,*work,*X,cond_norm2,td,*Y0;
Files_Struct *files=NULL,*features=NULL;
Mask_Struct *ms=NULL;
Dim_Param2 *dp;
void* niftiPtr=NULL;
Memory_Map *mm=NULL;
FS *fs;
Regions_By_File *fbf;
FILE *fp=NULL,*op;
Interfile_header *ifh;
if(argc<5){

    #if 0
    fprintf(stderr,"  -files:    imgs,niftis,cifitis or concs.\n");
    fprintf(stderr,"  -features: Labels (one for each file frame) or a text file(s) (one per row) that lists the features.\n");
    fprintf(stderr,"  -mask:     Only voxels in the mask are analyzed.\n");
    fprintf(stderr,"  -out:      Name of output file.\n");
    fprintf(stderr,"  -frames:   Frames of interest or file containing frames of interest. First frame is 1.\n");
    fprintf(stderr,"             If not specified, then all frames are used.\n");
    #endif
    //START170501
    printf("Principal Component Analysis and CUR decomposition\n");
    printf("Mahoney and Drineas PNAS 2009. We call the rCUR package.\n");
    printf("  -files:    imgs,niftis,cifitis or concs.\n");
    printf("  -features: Labels (one for each file frame) or a text file(s) (one per row) that lists the features.\n");
    printf("  -mask:     Only voxels in the mask are analyzed.\n");
    printf("  -out:      Name of output file.\n");
    printf("  -frames:   Frames of interest or file containing frames of interest. First frame is 1.\n");
    printf("             If not specified, then all frames are used.\n");

    exit(-1);
    }
for(i=1;i<argc;i++) {

    #if 0
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")) {
            if(!(files=read_conc(nfiles,&argv[i+1]))) exit(-1);
            }
        else if(!strcmp(strptr,".img")){
            if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
            }
        else {
            printf("fidlError: -files not concs or imgs. Abort!\n");fflush(stdout);
            exit(-1);
            }
        i+=nfiles;
        }
    #endif
    //START170501
    if(!strcmp(argv[i],"-files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++nfiles;
        if(!(files=read_files(nfiles,&argv[i+1])))exit(-1);
        i+=nfiles;
        }



    if(!strcmp(argv[i],"-features") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfeatures;
        if(!(features=get_files(nfeatures,&argv[i+1]))) exit(-1);
        i+=nfeatures;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask=argv[++i];
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        out=argv[++i];
    if(!strcmp(argv[i],"-frames") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++num_frames;
        if(num_frames){
            val=strtol(argv[++i],&strptr,10);
            if((strptr==argv[i])||(*strptr!='\0')){
                frames_file=argv[i];
                }
            else{
                argc_frames=i;
                }
            i+=num_frames-1;
            }
        }
    } 
print_version_number(rcsid,stdout);
if((SunOS_Linux=checkOS())==-1)exit(-1);
if(!nfiles){printf("fidlError: No -files. Abort!\n");exit(-1);}
if(!(dp=dim_param2(files->nfiles,files->files,SunOS_Linux)))exit(-1);
if(dp->volall==-1){printf("fidlError: All files must be the same size. Abort!\n");exit(-1);}
if(!(ms=get_mask_struct(mask,dp->volall,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp->volall)))exit(-1);
if(!frames_file){
    if(!(frames_file=malloc(sizeof*frames_file*MAXNAME))){
        printf("fidlError: Unable to malloc frames_file\n");
        exit(-1);
        }
    sprintf(frames_file,"%sfidlframes%s.dat",scratchdir?scratchdir:"",make_timestr2(timestr));
    if(!(fp=fopen_sub(frames_file,"w"))) exit(-1);
    if(num_frames){
        for(j=0;j<num_frames;j++)fprintf(fp,"%s\n",argv[argc_frames+j]);
        }
    else{
        for(j=0;j<dp->tdim_total;j++) fprintf(fp,"%d\n",j+1);
        }
    fclose(fp);
    lcrm=1;
    }
if(!(fs=read_frames_file(frames_file,0,0,0))) exit(-1);
if(features->nfiles!=(size_t)fs->nframes){
    printf("fidlError: features->nfiles=%zd fs->nframes=%d Must be equal.\n",features->nfiles,fs->nframes);exit(-1);
    }
if(!(fbf=find_regions_by_file(files->nfiles,fs->nframes,dp->tdim,fs->frames)))exit(-1);
if(!(Y=malloc(sizeof*Y*ms->lenbrain*fs->nframes))){
    printf("fidlError: Unable to malloc Y\n");
    exit(-1);
    }
if(!(Y0=malloc(sizeof*Y0*ms->lenbrain*fs->nframes))){
    printf("fidlError: Unable to malloc Y0\n");
    exit(-1);
    }
if(!(A=malloc(sizeof*A*ms->lenbrain*fs->nframes))){
    printf("fidlError: Unable to malloc A\n");
    exit(-1);
    }

#if 0
if(!(temp_double=malloc(sizeof*temp_double*ms->lenbrain))){
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }
#endif
//START170531
if(!(temp_double=malloc(sizeof*temp_double*(ms->lenbrain>fs->nframes?ms->lenbrain:fs->nframes)))){
    printf("fidlError: Unable to malloc temp_double\n");
    exit(-1);
    }

if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
    if(!(temp_float2=malloc(sizeof*temp_float2*dp->lenvol_max)))
        {printf("fidlError: Unable to malloc temp_float2\n");exit(-1);}
    }

#if 0
if(!(temp_float=malloc(sizeof*temp_float*ms->lenbrain))){
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }
#endif
//START170531
if(!(temp_float=malloc(sizeof*temp_float*ms->lenvol))){
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }


if(!(V=malloc(sizeof*V*fs->nframes*fs->nframes))){
    printf("fidlError: Unable to malloc V\n");
    exit(-1);
    }
if(!(S=malloc(sizeof*S*fs->nframes))){
    printf("fidlError: Unable to malloc S\n");
    exit(-1);
    }
if(!(work=malloc(sizeof*work*fs->nframes))) {
    printf("fidlError: Unable to malloc work\n");
    exit(-1);
    }
if(!(X=malloc(sizeof*X*fs->nframes*fs->nframes))) {
    printf("fidlError: Unable to malloc X\n");
    exit(-1);
    }
for(r=l=0;l<fs->nframes;){
    for(i1=0;i1<files->nfiles;i1++){
        if(fbf->num_regions_by_file[i1]){
            if(dp->filetypeall==(int)IMG){
                if(!(mm=map_disk(files->files[i1],dp->vol[i1]*dp->tdim[i1],0,sizeof(float))))exit(-1);
                }
            else if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
                if(!cifti_getstack(files->files[i1],temp_float2))exit(-1);
                }
            else if(!(niftiPtr=nifti_openRead(files->files[i1])))return 0;
            for(q=0,i=0;i<fbf->num_regions_by_file[i1]&&fbf->file_index[l]==(int)i1;i++,l++) {
                if(!q)for(j=0;j<ms->lenbrain;j++)temp_double[j]=0;
                for(k=dp->vol[i1]*fbf->roi_index[l],j=0;j<ms->lenbrain;j++)temp_float[j]=mm->ptr[k+ms->brnidx[j]];
                if(dp->swapbytes[i1])swap_bytes((unsigned char*)temp_float,sizeof(float),(size_t)ms->lenbrain);
                for(j=0;j<ms->lenbrain;j++) {
                    if(temp_float[j]==(float)UNSAMPLED_VOXEL) {
                        temp_double[j] = (double)UNSAMPLED_VOXEL;
                        }
                    else if(temp_double[j]==(double)UNSAMPLED_VOXEL) {
                        /*do nothing*/
                        }
                    else {
                        temp_double[j] += (double)temp_float[j];
                        }
                    }
                if(q==(fs->num_frames_to_sum[r]-1)){
                    if(fs->num_frames_to_sum[r]>1){
                        for(j=0;j<ms->lenbrain;j++)
                            if(temp_double[j]!=(double)UNSAMPLED_VOXEL)temp_double[j]/=(double)fs->num_frames_to_sum[r];
                        }
                    for(k=l,j=0;j<ms->lenbrain;j++,k+=fs->nframes)Y[k]=temp_double[j];
                    }
                }
            if(q++ == (fs->num_frames_to_sum[r]-1)){
                q=0;
                r++;
                }
            if(dp->filetypeall==(int)IMG){if(!unmap_disk(mm))exit(-1);}
            }
        }
    }

//START170502
for(i=0;i<fs->nframes*ms->lenbrain;i++)Y0[i]=Y[i];


for(i=0;i<fs->nframes;i++){
    for(td=0.,k=i,j=0;j<ms->lenbrain;j++,k+=fs->nframes)td+=Y[k];
    td/=ms->lenbrain;
    for(k=i,j=0;j<ms->lenbrain;j++,k+=fs->nframes)Y[k]-=td;
    }
for(i=0;i<fs->nframes*ms->lenbrain;i++)Y[i]/=sqrt((double)ms->lenbrain-1.);

printf("zero mean check= ");for(i=0;i<fs->nframes;i++){
    //for(td=0.,k=i,j=0;j<ms->lenbrain;j++,k+=fs->nframes)td+=Y[k];
    for(td=0.,k=i,j=0;j<ms->lenbrain;j++,k+=fs->nframes){
        td+=Y[k];
        //printf("%d ",k);
        }
    //printf("\n\n");
    printf("%f ",td);
    }printf("\n\n");

#if 0
if(!(fp=fopen_sub(out,"w")))exit(-1);
for(i1=0;i1<files->nfiles;i1++){printf("%s\n",files->files[i1]);fprintf(fp,"%s\n",files->files[i1]);}
#endif
printf("\nmask= %s\n\n",mask);fprintf(fp,"\nmask= %s\n\n",mask);
for(i=0;i<ms->lenbrain*fs->nframes;i++)A[i]=Y[i];
td=gsl_svd_golubreinsch(A,ms->lenbrain,fs->nframes,0.,V,S,NULL,&cond_norm2,&ndep,work);
printf("gsl_svd_golubreinsch cond_norm2=%f ndep=%d tol=%g\n",cond_norm2,ndep,td);
printf("    S= ");for(i=0;i<fs->nframes;i++)printf("%f ",S[i]);printf("\n");
printf("    variance= ");for(i=0;i<fs->nframes;i++)printf("%f ",S[i]*S[i]);printf("\n");
for(td=0.,i=0;i<fs->nframes;i++)td+=S[i]*S[i];
printf("    %%variance= ");for(i=0;i<fs->nframes;i++)printf("%f ",S[i]*S[i]/td*100.);printf("\n");
#if 0
fprintf(fp,"gsl_svd_golubreinsch cond_norm2=%f ndep=%d tol=%g\n",cond_norm2,ndep,td);
fprintf(fp,"    S= ");for(i=0;i<fs->nframes;i++)fprintf(fp,"%f ",S[i]);fprintf(fp,"\n");
fprintf(fp,"    variance= ");for(i=0;i<fs->nframes;i++)fprintf(fp,"%f ",S[i]*S[i]);fprintf(fp,"\n");
fprintf(fp,"    %%variance= ");for(i=0;i<fs->nframes;i++)fprintf(fp,"%f ",S[i]*S[i]/td*100.);fprintf(fp,"\n");
#endif
printf("Columns of V are the components\n");
printf("V=");for(k=i=0;i<fs->nframes;i++){
    for(j=0;j<fs->nframes;j++,k++)printf("\t%f",V[k]);printf("\n");
    }printf("\n");
#if 0
fprintf(fp,"Columns of V are the components\n");
fprintf(fp,"V=");for(k=i=0;i<fs->nframes;i++){
    for(j=0;j<fs->nframes;j++,k++)fprintf(fp,"\t%f",V[k]);fprintf(fp,"\n");
    }fprintf(fp,"\n");
#endif

for(i=0;i<ms->lenbrain*fs->nframes;i++)A[i]=Y[i];
td=gsl_svd_modgolubreinsch(A,ms->lenbrain,fs->nframes,0.,V,S,NULL,&cond_norm2,&ndep,work,X);
printf("gsl_svd_modgolubreinsch cond_norm2=%f ndep=%d tol=%g\n",cond_norm2,ndep,td);
printf("    S= ");for(i=0;i<fs->nframes;i++)printf("%f ",S[i]);printf("\n");
printf("    variance= ");for(i=0;i<fs->nframes;i++)printf("%f ",S[i]*S[i]);printf("\n");
for(td=0.,i=0;i<fs->nframes;i++)td+=S[i]*S[i];
printf("    %%variance= ");for(i=0;i<fs->nframes;i++)printf("%f ",S[i]*S[i]/td*100.);printf("\n");
#if 0
fprintf(fp,"gsl_svd_modgolubreinsch cond_norm2=%f ndep=%d tol=%g\n",cond_norm2,ndep,td);
fprintf(fp,"    S= ");for(i=0;i<fs->nframes;i++)fprintf(fp,"%f ",S[i]);fprintf(fp,"\n");
fprintf(fp,"    variance= ");for(i=0;i<fs->nframes;i++)fprintf(fp,"%f ",S[i]*S[i]);fprintf(fp,"\n");
fprintf(fp,"    %%variance= ");for(i=0;i<fs->nframes;i++)fprintf(fp,"%f ",S[i]*S[i]/td*100.);fprintf(fp,"\n");
#endif

for(i=0;i<ms->lenbrain*fs->nframes;i++)A[i]=Y[i];
td=gsl_svd_jacobi(A,ms->lenbrain,fs->nframes,0.,V,S,NULL,&cond_norm2,&ndep);
printf("gsl_svd_jacobi cond_norm2=%f ndep=%d tol=%g\n",cond_norm2,ndep,td);
printf("    S= ");for(i=0;i<fs->nframes;i++)printf("%f ",S[i]);printf("\n");
printf("    variance= ");for(i=0;i<fs->nframes;i++)printf("%f ",S[i]*S[i]);printf("\n");
for(td=0.,i=0;i<fs->nframes;i++)td+=S[i]*S[i];
printf("    %%variance= ");for(i=0;i<fs->nframes;i++)printf("%f ",S[i]*S[i]/td*100.);printf("\n");
#if 0
fprintf(fp,"gsl_svd_jacobi cond_norm2=%f ndep=%d tol=%g\n",cond_norm2,ndep,td);
fprintf(fp,"    S= ");for(i=0;i<fs->nframes;i++)fprintf(fp,"%f ",S[i]);fprintf(fp,"\n");
fprintf(fp,"    variance= ");for(i=0;i<fs->nframes;i++)fprintf(fp,"%f ",S[i]*S[i]);fprintf(fp,"\n");
fprintf(fp,"    %%variance= ");for(i=0;i<fs->nframes;i++)fprintf(fp,"%f ",S[i]*S[i]/td*100.);fprintf(fp,"\n");
fclose(fp);
printf("Ouput written to %s\n",out);
#endif

//START170531
sprintf(string,"%sfidlr%s",scratchdir?scratchdir:"",make_timestr2(timestr));
sprintf(dataf,"%s.dat",string);
if(!writestack(dataf,Y0,sizeof*Y0,(size_t)(ms->lenbrain*fs->nframes),0))exit(-1);
sprintf(script,"%s.r",string);

sprintf(ofile0,"%s_rotation.4dfp.img",string);
sprintf(ofile1,"%s_transpose.4dfp.img",string);

if(!(op=fopen_sub(script,"w")))exit(-1);
fprintf(op,"#!/usr/bin/env Rscript\n");
fprintf(op,"p.ifile='%s'\n",dataf);
fprintf(op,"p.byrow=TRUE\n");
fprintf(op,"p.row=%d\n",ms->lenbrain);
fprintf(op,"p.col=%d\n",fs->nframes);

//fprintf(op,"p.ofile1='%s_transpose.4dfp.img'\n",root);
//fprintf(op,"p.ofile0='%s_rotation.4dfp.img'\n",root);
fprintf(op,"p.ofile0='%s'\n",ofile0);
fprintf(op,"p.ofile1='%s'\n",ofile1);


fprintf(op,"Av=double(p.row*p.col)\n");
fprintf(op,"Av=readBin(p.ifile,double(),p.row*p.col)\n");
fprintf(op,"A=matrix(Av,nrow=p.row,ncol=p.col,byrow=p.byrow)\n");
fprintf(op,"PCA01=prcomp(A,retx=TRUE)\n");
fprintf(op,"fp=file(p.ofile0,'wb')\n");
fprintf(op,"writeBin(as.vector(PCA01$rotation),fp)\n");
fprintf(op,"close(fp)\n");
fprintf(op,"fp=file(p.ofile1,'wb')\n");
fprintf(op,"writeBin(as.vector(PCA01$x),fp)\n");
fprintf(op,"close(fp)\n");
fclose(op);
sprintf(rscriptcall,"Rscript --vanilla %s",script);
printf("%s\n",rscriptcall);

//if(system(rscriptcall)==-1){printf("fidlError: %s\n",rscriptcall);exit(-1);}
if(system(rscriptcall)==-1)
    printf("fidlError: %s\n",rscriptcall);
else{
    if(!readstack(ofile0,(double*)temp_double,sizeof*temp_double,(size_t)(fs->nframes*fs->nframes),SunOS_Linux,
        SunOS_Linux?0:1))exit(-1);
    printf("Principal components from prcomp\n");
    for(i=0;i<fs->nframes;i++)printf("\tPC%d",i+1);printf("\n");
    for(i=0;i<fs->nframes;i++){for(k=i,j=0;j<fs->nframes;j++,k+=fs->nframes)printf("\t%f",temp_double[k]);printf("\n");}

    if(!readstack(ofile1,(double*)Y,sizeof*temp_double,(size_t)(fs->nframes*ms->lenbrain),SunOS_Linux,SunOS_Linux?0:1))exit(-1);
    if(!(ifh=init_ifh(4,ms->xdim,ms->ydim,ms->zdim,1,ms->voxel_size_1,ms->voxel_size_1,ms->voxel_size_1,SunOS_Linux?0:1,
        ms->center,ms->mmppix)))exit(-1);
    for(i=0;i<ms->lenvol;i++)temp_float[i]=0.;
    for(k=i=0;i<fs->nframes;i++){
        for(j=0;j<ms->lenbrain;j++,k++)temp_float[ms->brnidx[j]]=(float)Y[k];
        sprintf(string,"data_projected_on_PC%d.4dfp.img",i+1);
        if(!writestack(string,temp_float,sizeof*temp_float,(size_t)(ms->lenvol),0))exit(-1);
        if(!write_ifh(string,ifh,0))exit(-1);
        printf("Output written to %s\n",string);
        }
    }

if(lcrm){
    sprintf(string,"rm -rf %s",frames_file);
    if(system(string)==-1)printf("fidlError: unable to %s\n",string);
    }
}
