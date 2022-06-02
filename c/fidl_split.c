/* Copyright 11/21/11 Washington University.  All Rights Reserved.
   fidl_split.c  $Revision: 1.8 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fidl.h"
#include "get_atlas_coor.h"
int main(int argc,char **argv)
{
char *mask_file=NULL,string[MAXNAME],string2[MAXNAME],filename[MAXNAME],*strptr,*rroot=NULL,*rrootptr,atlas[7]="";
int i,j,nfiles=0,nroots=0,SunOS_Linux,nL,nR,nant,npos,ninf,nsup,LRregonly=0;
size_t i1,j1,st[2];
float *temp_float,*L,*R,*ant,*pos,*inf,*sup,*LR,*antpos,*infsup;
double *coor,*col,*row,*slice,*dptr;
Dim_Param *dp;
Files_Struct *files=NULL,*roots=NULL;
Interfile_header *ifh;
Atlas_Param *ap;
Mask_Struct *ms;
if(argc<5) {
    fprintf(stderr,"-files: Files to split by hemisphere, anterior/posterior and inferior/superior.\n");
    fprintf(stderr,"-roots: Roots are used for the region files and the region names.\n");
    fprintf(stderr,"-rroot: Used in place of -roots for region names. One word.\n");

    /*START150113*/
    fprintf(stderr,"-LRregonly: Only output the hemispherical region file.\n");

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
if(!(dp=dim_param(nfiles,files->files,SunOS_Linux,0,(int*)NULL,(int*)NULL))) exit(-1);
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
if(!(ms=get_mask_struct(mask_file,dp->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp->vol))) exit(-1);
get_atlas(dp->vol,atlas);
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(!(temp_float=malloc(sizeof*temp_float*dp->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(L=malloc(sizeof*L*dp->vol))) {
    printf("Error: Unable to malloc L\n");
    exit(-1);
    }
if(!(R=malloc(sizeof*R*dp->vol))) {
    printf("Error: Unable to malloc R\n");
    exit(-1);
    }
if(!(ant=malloc(sizeof*ant*dp->vol))) {
    printf("Error: Unable to malloc ant\n");
    exit(-1);
    }
if(!(pos=malloc(sizeof*pos*dp->vol))) {
    printf("Error: Unable to malloc pos\n");
    exit(-1);
    }
if(!(inf=malloc(sizeof*inf*dp->vol))) {
    printf("Error: Unable to malloc inf\n");
    exit(-1);
    }
if(!(sup=malloc(sizeof*sup*dp->vol))) {
    printf("Error: Unable to malloc sup\n");
    exit(-1);
    }
if(!(LR=malloc(sizeof*LR*dp->vol))) {
    printf("Error: Unable to malloc LR\n");
    exit(-1);
    }
if(!(antpos=malloc(sizeof*antpos*dp->vol))) {
    printf("Error: Unable to malloc antpos\n");
    exit(-1);
    }
if(!(infsup=malloc(sizeof*infsup*dp->vol))) {
    printf("Error: Unable to malloc infsup\n");
    exit(-1);
    }
if(!(coor=malloc(sizeof*coor*ms->lenbrain*3))) {
    printf("Error: Unable to malloc coor\n");
    exit(-1);
    }
if(!(col=malloc(sizeof*col*ms->lenbrain))) {
    printf("Error: Unable to malloc col\n");
    exit(-1);
    }
if(!(row=malloc(sizeof*row*ms->lenbrain))) {
    printf("Error: Unable to malloc row\n");
    exit(-1);
    }
if(!(slice=malloc(sizeof*slice*ms->lenbrain))) {
    printf("Error: Unable to malloc slice\n");
    exit(-1);
    }
col_row_slice(ms->lenbrain,ms->brnidx,col,row,slice,ap);
get_atlas_coor(ms->lenbrain,col,row,slice,(double)ap->zdim,ap->center,ap->mmppix,coor);
for(i1=0;i1<files->nfiles;i1++) {
    if(!readstack(files->files[i1],(float*)temp_float,sizeof(float),(size_t)dp->vol,SunOS_Linux,dp->bigendian[i1])) exit(-1); 
    for(j1=0;j1<(size_t)dp->vol;j1++) L[j1]=R[j1]=LR[j1]=pos[j1]=ant[j1]=antpos[j1]=inf[j1]=sup[j1]=infsup[j1]=0.; 
    for(dptr=coor,nL=nR=npos=nant=ninf=nsup=j1=0;j1<(size_t)ms->lenbrain;j1++) {
        if(fabs(temp_float[ms->brnidx[j1]])<=(float)UNSAMPLED_VOXEL) {
            dptr+=3;
            continue;
            }
        if((*dptr++)<0) {
            L[ms->brnidx[j1]] = temp_float[ms->brnidx[j1]]; 
            LR[ms->brnidx[j1]] = 2.;
            nL++;
            }
        else {
            R[ms->brnidx[j1]] = temp_float[ms->brnidx[j1]];
            LR[ms->brnidx[j1]] = 3.;
            nR++;
            }
        if((*dptr++)<0) {
            pos[ms->brnidx[j1]] = temp_float[ms->brnidx[j1]]; 
            antpos[ms->brnidx[j1]] = 2.;
            npos++;
            }
        else {
            ant[ms->brnidx[j1]] = temp_float[ms->brnidx[j1]];
            antpos[ms->brnidx[j1]] = 3.;
            nant++;
            }
        if((*dptr++)<0) {
            inf[ms->brnidx[j1]] = temp_float[ms->brnidx[j1]]; 
            infsup[ms->brnidx[j1]] = 2.;
            ninf++;
            }
        else {
            sup[ms->brnidx[j1]] = temp_float[ms->brnidx[j1]];
            infsup[ms->brnidx[j1]] = 3.;
            nsup++;
            }
        }
    if(!(ifh=read_ifh(files->files[i1],(Interfile_header*)NULL))) exit(-1);
    free(ifh->file_name);
    if(!(ifh->file_name=malloc(sizeof*ifh->file_name*files->strlen_files[i1]))) {
        printf("Error: Unable to malloc ifh->file_name\n");
        exit(-1);
        }
    strcpy(ifh->file_name,files->files[i1]);
    strcpy(string,roots->files[i1]);
    if(!(strptr=get_tail_sans_ext(string))) exit(-1);
    if(!LRregonly) {
        sprintf(filename,"%s_L.4dfp.img",strptr);
        if(!writestack(filename,L,sizeof(float),(size_t)dp->vol,dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("L written to %s\n",filename);
        sprintf(filename,"%s_R.4dfp.img",strptr);
        if(!writestack(filename,R,sizeof(float),(size_t)dp->vol,dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("R written to %s\n",filename);
        sprintf(filename,"%s_pos.4dfp.img",strptr);
        if(!writestack(filename,pos,sizeof(float),(size_t)dp->vol,dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("Posterior written to %s\n",filename);
        sprintf(filename,"%s_ant.4dfp.img",strptr);
        if(!writestack(filename,ant,sizeof(float),(size_t)dp->vol,dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("Anterior written to %s\n",filename);
        sprintf(filename,"%s_inf.4dfp.img",strptr);
        if(!writestack(filename,inf,sizeof(float),(size_t)dp->vol,dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("Inferior written to %s\n",filename);
        sprintf(filename,"%s_sup.4dfp.img",strptr);
        if(!writestack(filename,sup,sizeof(float),(size_t)dp->vol,dp->swapbytes[i1])) exit(-1);
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
    if(!writestack(filename,LR,sizeof(float),(size_t)dp->vol,dp->swapbytes[i1])) exit(-1); 
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
        if(!writestack(filename,antpos,sizeof(float),(size_t)dp->vol,dp->swapbytes[i1])) exit(-1);
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
        if(!writestack(filename,infsup,sizeof(float),(size_t)dp->vol,dp->swapbytes[i1])) exit(-1);
        if(!write_ifh(filename,ifh,0)) exit(-1);
        printf("infsupreg written to %s\n",filename);
        }
    free_ifh(ifh,0);
    }
fflush(stdout);
}
