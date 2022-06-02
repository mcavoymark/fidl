/* Copyright 1/9/19 Washington University.  All Rights Reserved.
   fidl_split.cxx  $Revision: 1.1 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "get_atlas_coor.h"
#include "dim_param2.h"
#include "constants.h"
#include "files_struct.h"
#include "checkOS.h"
#include "get_atlas.h"
#include "subs_util.h"
#include "stack.h"
#include "filetype.h"
#include "getxyz.h"
#include "nifti_getmni.h"

int main(int argc,char **argv)
{
char string[MAXNAME],string2[MAXNAME],filename[MAXNAME],*strptr,*rroot=NULL,*rrootptr,atlas[7]="";
int i,j,j1,nfiles=0,nroots=0,SunOS_Linux,nL,nR,nant,npos,ninf,nsup,LRregonly=0,*brnidx=NULL,cf_flip=-1;
size_t i1,st[2];
float *temp_float,*L,*R,*ant,*pos,*inf,*sup,*LR,*antpos,*infsup,*xyz,*coorf,*dptr;
double *col,*row,*slice;  //*coor,*dptr;
Files_Struct *files=NULL,*roots=NULL;
Interfile_header *ifh=NULL;
//Atlas_Param *ap;
Dim_Param2 *dp;
stack fs;

if(argc<5) {
    printf("-files: Files to split by hemisphere, anterior/posterior and inferior/superior.\n");
    printf("-roots: Roots are used for the region files and the region names.\n");
    printf("-rroot: Used in place of -roots for region names. One word.\n");
    printf("-LRregonly: Only output the hemispherical region file.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    if(!strcmp(argv[i],"-roots") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroots;
        if(!(roots=get_files(nroots,&argv[i+1]))) exit(-1);
        i += nroots;
        }
    if(!strcmp(argv[i],"-rroot") && argc > i+1)
        rroot = argv[++i];

    /*START150113*/
    if(!strcmp(argv[i],"-LRregonly"))
        LRregonly=1;

    }
if(!nfiles) {
    printf("Error: Need to specify -files\n");
    exit(-1);
    }
if(!nroots) {
    printf("Error: Need to specify -roots\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

//if(!(dp=dim_param(nfiles,files->files,SunOS_Linux,0,(int*)NULL,(int*)NULL))) exit(-1);
//START190109
if(!(dp=dim_param2(files->nfiles,files->files,SunOS_Linux)))exit(-1);

#if 0
for(i1=0;i1<files->nfiles;i1++) {
     if(dp->number_format[i1]!=(int)FLOAT_IF) {
        printf("Error: Number format not float. Need to add code to split this type of stack.\n");
        exit(-1);
        }
     if(dp->tdim[i1]!=1) {
        printf("Error: Time dimension > 1. Need to add code to split this type of stack.\n");
        exit(-1);
        }
    }
#endif
//START190109
if(dp->tdimall!=1) {
    printf("fidlError: Time dimension > 1. Need to add code to split this type of stack.\n");
    exit(-1);
    }

//START190109
//if(!(ms=get_mask_struct(mask_file,dp->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp->vol))) exit(-1);
//get_atlas(dp->vol,atlas);
//if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);


if(!(temp_float=(float*)malloc(sizeof*temp_float*dp->volall))) {
    printf("fidlError: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(L=(float*)malloc(sizeof*L*dp->volall))) {
    printf("fidlError: Unable to malloc L\n");
    exit(-1);
    }
if(!(R=(float*)malloc(sizeof*R*dp->volall))) {
    printf("fidlError: Unable to malloc R\n");
    exit(-1);
    }
if(!(ant=(float*)malloc(sizeof*ant*dp->volall))) {
    printf("fidlError: Unable to malloc ant\n");
    exit(-1);
    }
if(!(pos=(float*)malloc(sizeof*pos*dp->volall))) {
    printf("fidlError: Unable to malloc pos\n");
    exit(-1);
    }
if(!(inf=(float*)malloc(sizeof*inf*dp->volall))) {
    printf("fidlError: Unable to malloc inf\n");
    exit(-1);
    }
if(!(sup=(float*)malloc(sizeof*sup*dp->volall))) {
    printf("fidlError: Unable to malloc sup\n");
    exit(-1);
    }
if(!(LR=(float*)malloc(sizeof*LR*dp->volall))) {
    printf("fidlError: Unable to malloc LR\n");
    exit(-1);
    }
if(!(antpos=(float*)malloc(sizeof*antpos*dp->volall))) {
    printf("fidlError: Unable to malloc antpos\n");
    exit(-1);
    }
if(!(infsup=(float*)malloc(sizeof*infsup*dp->volall))) {
    printf("fidlError: Unable to malloc infsup\n");
    exit(-1);
    }


#if 0
if(!(coor=(double*)malloc(sizeof*coor*lenbrain*3))) {
    printf("fidlError: Unable to malloc coor\n");
    exit(-1);
    }
if(!(col=(double*)malloc(sizeof*col*lenbrain))) {
    printf("fidlError: Unable to malloc col\n");
    exit(-1);
    }
if(!(row=(double*)malloc(sizeof*row*lenbrain))) {
    printf("fidlError: Unable to malloc row\n");
    exit(-1);
    }
if(!(slice=(double*)malloc(sizeof*slice*lenbrain))) {
    printf("fidlError: Unable to malloc slice\n");
    exit(-1);
    }
col_row_slice(lenbrain,brnidx,col,row,slice,ap);
get_atlas_coor(lenbrain,col,row,slice,(double)ap->zdim,ap->center,ap->mmppix,coor);
#endif
//START190109
#if 0
if(!(coor=(double*)malloc(sizeof*coor*dp->volall*3))) {
    printf("fidlError: Unable to malloc coor\n");
    exit(-1);
    }
#endif
//START190124
if(!(coorf=(float*)malloc(sizeof*coorf*dp->volall*3))) {
    printf("fidlError: Unable to malloc coorf\n");
    exit(-1);
    }

if(!(col=(double*)malloc(sizeof*col*dp->volall))) {
    printf("fidlError: Unable to malloc col\n");
    exit(-1);
    }
if(!(row=(double*)malloc(sizeof*row*dp->volall))) {
    printf("fidlError: Unable to malloc row\n");
    exit(-1);
    }
if(!(slice=(double*)malloc(sizeof*slice*dp->volall))) {
    printf("fidlError: Unable to malloc slice\n");
    exit(-1);
    }
if(!(brnidx=(int*)malloc(sizeof*brnidx*dp->volall))) {
    printf("fidlError: Unable to malloc brnidx\n");
    exit(-1);
    }
for(i=0;i<dp->volall;i++)brnidx[i]=i;



//START190110
get_atlas(dp->volall,atlas);
if(dp->filetypeall==(int)NIFTI){

    //if(!(ifh=init_ifh(4,dp->xdim[0],dp->ydim[0],dp->zdim[0],1,dp->dx[0],dp->dy[0],dp->dz[0],dp->bigendianall,dp->centerf[0],
    //    dp->mmppixf[0]))) exit(-1);
    //START190128
    if(!(ifh=init_ifh(4,dp->xdim[0],dp->ydim[0],dp->zdim[0],1,dp->dx[0],dp->dy[0],dp->dz[0],0,dp->centerf[0],dp->mmppixf[0]))) 
        exit(-1);

    //printf("c_orient = %d %d %d\n",dp->c_orient[0][0],dp->c_orient[0][1],dp->c_orient[0][2]);fflush(stdout);
    //START190124
    cf_flip=100*dp->c_orient[0][0]+10*dp->c_orient[0][1]+dp->c_orient[0][2];
    //printf("c_orient = %d %d %d cf_flip=%d\n",dp->c_orient[0][0],dp->c_orient[0][1],dp->c_orient[0][2],cf_flip);fflush(stdout);

    if(dp->c_orient[0][0]==4&&dp->c_orient[0][1]==1){
        ifh->mmppix[0]*=-1.;ifh->mmppix[1]*=-1.;
        ifh->center[0]*=-1;ifh->center[1]=-ifh->center[0];
        }
    }


#if 0
if(!(ap=get_atlas_param(atlas,!strstr(atlas,"MNI")?(Interfile_header*)NULL:ifh,files->files[0]))) exit(-1);
col_row_slice(dp->volall,brnidx,col,row,slice,ap);
get_atlas_coor(dp->volall,col,row,slice,(double)ap->zdim,ap->center,ap->mmppix,coor);
#endif
#if 0
//START190124
if(!(ap=get_atlas_param(atlas,!strstr(atlas,"MNI")?(Interfile_header*)NULL:ifh,files->files[0]))) exit(-1);

col_row_slice(dp->volall,brnidx,col,row,slice,ap);
//START190128
//col_row_slice2(dp->volall,brnidx,col,row,slice,dp->xdim[0],dp->ydim[0]);

if(!strstr(atlas,"MNI")){
    get_atlas_coorf(dp->volall,col,row,slice,(double)ap->zdim,ap->center,ap->mmppix,coorf);
    }
else{
    if(!(xyz=(float*)malloc(sizeof*xyz*dp->volall*3))) {
        printf("fidlError: Unable to malloc xyz\n");
        exit(-1);
        }

    //getxyz(int n,double *col,double *row,double *slice,int cf_flip,int ydim,float *xyz)
    getxyz(dp->volall,col,row,slice,cf_flip,ap->ydim,xyz,atlas);

    if(!nifti_getmnis(files->files[0],xyz,coorf,dp->volall))exit(-1);
    }
#endif
#if 0
//START190128
if(!(xyz=(float*)malloc(sizeof*xyz*dp->volall*3))) {
    printf("fidlError: Unable to malloc xyz\n");
    exit(-1);
    }
col_row_slice2(dp->volall,brnidx,col,row,slice,dp->xdim[0],dp->ydim[0]);
//printf("cf_flip=%d\n",cf_flip);fflush(stdout);
getxyz(dp->volall,col,row,slice,cf_flip,dp->ydim[0],xyz,atlas);
if(!nifti_getmnis(files->files[0],xyz,coorf,dp->volall))exit(-1);
#endif
//START190508
col_row_slice2(dp->volall,brnidx,col,row,slice,dp->xdim[0],dp->ydim[0]);
if(!strstr(atlas,"MNI")){
    get_atlas_coorff(dp->volall,col,row,slice,(double)dp->zdim[0],dp->centerf[0],dp->mmppixf[0],coorf);
    }
else{
    if(!(xyz=(float*)malloc(sizeof*xyz*dp->volall*3))) {
        printf("fidlError: Unable to malloc xyz\n");
        exit(-1);
        }
    getxyz(dp->volall,col,row,slice,cf_flip,dp->ydim[0],xyz,atlas);
    if(!nifti_getmnis(files->files[0],xyz,coorf,dp->volall))exit(-1);
    }




for(i1=0;i1<files->nfiles;i1++) {



    //if(!readstack(files->files[i1],(float*)temp_float,sizeof(float),(size_t)dp->vol,SunOS_Linux,dp->bigendian[i1])) exit(-1); 
    //START190110
    if(!(fs.stack1(files->files[i1],temp_float,SunOS_Linux)))exit(-1);


    //START190114
    //if(fs.flip(temp_float)==-1)exit(-1);


    for(j1=0;j1<dp->volall;j1++) L[j1]=R[j1]=LR[j1]=pos[j1]=ant[j1]=antpos[j1]=inf[j1]=sup[j1]=infsup[j1]=0.; 


    #if 0
    for(dptr=coor,nL=nR=npos=nant=ninf=nsup=j1=0;j1<lenbrain;j1++) {
        if(fabs(temp_float[brnidx[j1]])<=(float)UNSAMPLED_VOXEL) {
            dptr+=3;
            continue;
            }
        if((*dptr++)<0) {
            L[brnidx[j1]] = temp_float[brnidx[j1]]; 
            LR[brnidx[j1]] = 2.;
            nL++;
            }
        else {
            R[brnidx[j1]] = temp_float[brnidx[j1]];
            LR[brnidx[j1]] = 3.;
            nR++;
            }
        if((*dptr++)<0) {
            pos[brnidx[j1]] = temp_float[brnidx[j1]]; 
            antpos[brnidx[j1]] = 2.;
            npos++;
            }
        else {
            ant[brnidx[j1]] = temp_float[brnidx[j1]];
            antpos[brnidx[j1]] = 3.;
            nant++;
            }
        if((*dptr++)<0) {
            inf[brnidx[j1]] = temp_float[brnidx[j1]]; 
            infsup[brnidx[j1]] = 2.;
            ninf++;
            }
        else {
            sup[brnidx[j1]] = temp_float[brnidx[j1]];
            infsup[brnidx[j1]] = 3.;
            nsup++;
            }
        }
    #endif
    #if 1
    //START190109
    for(dptr=coorf,nL=nR=npos=nant=ninf=nsup=j1=0;j1<dp->volall;j1++) {
        if(fabsf(temp_float[j1])<=(float)UNSAMPLED_VOXEL) {
            dptr+=3;
            continue;
            }
        if((*dptr++)<0) {
            L[j1] = temp_float[j1];
            LR[j1] = 2.;
            nL++;
            }
        else {
            R[j1] = temp_float[j1];
            LR[j1] = 3.;
            nR++;
            }
        if((*dptr++)<0) {
            pos[j1] = temp_float[j1];
            antpos[brnidx[j1]] = 2.;
            npos++;
            }
        else {
            ant[j1] = temp_float[j1];
            antpos[j1] = 3.;
            nant++;
            }
        if((*dptr++)<0) {
            inf[j1] = temp_float[j1];
            infsup[j1] = 2.;
            ninf++;
            }
        else {
            sup[j1] = temp_float[j1];
            infsup[j1] = 3.;
            nsup++;
            }
        }
    #endif
    #if 0
    //START190114
    for(nL=nR=npos=nant=ninf=nsup=j1=0;j1<dp->volall;j1++) {
        if(fabsf(temp_float[j1])<=(float)UNSAMPLED_VOXEL)continue;
        getxyz(j1,dp->xdim[i1],dp->ydim[i1],xyz);
        if(!nifti_getmni(files->files[i1],xyz,coorf))exit(-1);
        if(coorf[0]<0) {
            L[j1] = temp_float[j1];
            LR[j1] = 2.;
            nL++;
            }
        else {
            R[j1] = temp_float[j1];
            LR[j1] = 3.;
            nR++;
            }
        if(coorf[1]<0) {
            pos[j1] = temp_float[j1];
            antpos[brnidx[j1]] = 2.;
            npos++;
            }
        else {
            ant[j1] = temp_float[j1];
            antpos[j1] = 3.;
            nant++;
            }
        if(coorf[2]<0) {
            inf[j1] = temp_float[j1];
            infsup[j1] = 2.;
            ninf++;
            }
        else {
            sup[j1] = temp_float[j1];
            infsup[j1] = 3.;
            nsup++;
            }
        }
    #endif





    #if 0
    if(!(ifh=read_ifh(files->files[i1],(Interfile_header*)NULL))) exit(-1);
    free(ifh->file_name);
    #endif
    //START190110
    if(dp->filetype[i1]==(int)IMG)if(!(ifh=read_ifh(files->files[i1],(Interfile_header*)NULL)))exit(-1);
    if(ifh->file_name)free(ifh->file_name);

    //START190508
    ifh->bigendian=0;


    if(!(ifh->file_name=(char*)malloc(sizeof*ifh->file_name*files->strlen_files[i1]))) {
        printf("fidlError: Unable to malloc ifh->file_name\n");
        exit(-1);
        }
    strcpy(ifh->file_name,files->files[i1]);
    strcpy(string,roots->files[i1]);
    if(!(strptr=get_tail_sans_ext(string))) exit(-1);
    if(!LRregonly) {
        sprintf(filename,"%s_L.4dfp.img",strptr);
        if(!writestack(filename,L,sizeof(float),(size_t)dp->vol[i1],dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("L written to %s\n",filename);
        sprintf(filename,"%s_R.4dfp.img",strptr);
        if(!writestack(filename,R,sizeof(float),(size_t)dp->vol[i1],dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("R written to %s\n",filename);
        sprintf(filename,"%s_pos.4dfp.img",strptr);
        if(!writestack(filename,pos,sizeof(float),(size_t)dp->vol[i1],dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("Posterior written to %s\n",filename);
        sprintf(filename,"%s_ant.4dfp.img",strptr);
        if(!writestack(filename,ant,sizeof(float),(size_t)dp->vol[i1],dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("Anterior written to %s\n",filename);
        sprintf(filename,"%s_inf.4dfp.img",strptr);
        if(!writestack(filename,inf,sizeof(float),(size_t)dp->vol[i1],dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("Inferior written to %s\n",filename);
        sprintf(filename,"%s_sup.4dfp.img",strptr);
        if(!writestack(filename,sup,sizeof(float),(size_t)dp->vol[i1],dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("Superior written to %s\n",filename);
        }
    rrootptr = rroot ? rroot : roots->files[i1];
    sprintf(string2,"0 L%s %d",rrootptr,nL);
    st[0] = strlen(string2) + 1;
    sprintf(filename,"1 R%s %d",rrootptr,nR);
    st[1] = strlen(filename) + 1;
    if(ifh->region_names) free_ifhregnames(ifh);
    if(!(ifh->region_names=d2charvar(2,st))) return 0;
    ifh->nregions = 2;
    strcpy(ifh->region_names[0],string2);
    strcpy(ifh->region_names[1],filename);
    sprintf(filename,"%s_LRreg.4dfp.img",strptr);


    //START190114
    if(fs.flip(LR)==-1)exit(-1);


    //if(!writestack(filename,LR,sizeof(float),(size_t)dp->vol[i1],dp->swapbytes[i1])) exit(-1); 
    //START180114
    if(!writestack(filename,LR,sizeof(float),(size_t)dp->vol[i1],0))exit(-1); 

    if(!write_ifh(filename,ifh,0)) exit(-1);
    printf("LRreg written to %s\n",filename);
    if(!LRregonly) {
        sprintf(string2,"0 pos%s %d",rrootptr,npos);
        st[0] = strlen(string2) + 1;
        sprintf(filename,"1 ant%s %d",rrootptr,nant);
        st[1] = strlen(filename) + 1;
        if(ifh->region_names) free_ifhregnames(ifh);
        if(!(ifh->region_names=d2charvar(2,st))) return 0;
        ifh->nregions = 2;
        strcpy(ifh->region_names[0],string2);
        strcpy(ifh->region_names[1],filename);
        sprintf(filename,"%s_antposreg.4dfp.img",strptr);
        if(!writestack(filename,antpos,sizeof(float),(size_t)dp->vol[i1],dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("antposreg written to %s\n",filename);
        sprintf(string2,"0 inf%s %d",rrootptr,ninf);
        st[0] = strlen(string2) + 1;
        sprintf(filename,"1 sup%s %d",rrootptr,nsup);
        st[1] = strlen(filename) + 1;
        if(ifh->region_names) free_ifhregnames(ifh);
        if(!(ifh->region_names=d2charvar(2,st))) return 0;
        ifh->nregions = 2;
        strcpy(ifh->region_names[0],string2);
        strcpy(ifh->region_names[1],filename);
        sprintf(filename,"%s_infsupreg.4dfp.img",strptr);
        if(!writestack(filename,infsup,sizeof(float),(size_t)dp->vol[i1],dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("infsupreg written to %s\n",filename);
        }

    //free_ifh(ifh,0);
    //START190110
    if(dp->filetype[i1]==(int)IMG)free_ifh(ifh,0);
    }
fflush(stdout);
}
