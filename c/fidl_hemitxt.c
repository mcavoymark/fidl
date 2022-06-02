/* Copyright 4/22/11 Washington University.  All Rights Reserved.
   fidl_hemitxt.c  $Revision: 1.2 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
main(int argc,char **argv)
{
char *Lseedmap=NULL,*Rseedmap=NULL,*mask=NULL,*Lhemivox=NULL,*Rhemivox=NULL,**files;
int i,l,r,SunOS_Linux,nfiles=2,atlas;
float *Lmap,*Rmap,*LvoxLmap,*LvoxRmap,*RvoxLmap,*RvoxRmap;
double *coor,*col,*row,*slice,*dptr;
FILE *fp;
Dim_Param *dp;
Atlas_Param *ap;
Mask_Struct *ms;
if(argc < 11) {
    fprintf(stderr,"-Lseedmap: Map from left seed region.\n");
    fprintf(stderr,"-Rseedmap: Map from right seed region.\n");
    fprintf(stderr,"-mask:     Voxels to output.\n");
    fprintf(stderr,"-Lhemivox: Text output file for left hemisphere voxels. Left column is Lseedmap values. Right column is right seed map values.\n");
    fprintf(stderr,"-Rhemivox: Text output file for right hemisphere voxels. Left column is Lseedmap values. Right column is right seed map values.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-Lseedmap") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) Lseedmap = argv[++i];
    if(!strcmp(argv[i],"-Rseedmap") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) Rseedmap = argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) mask = argv[++i];
    if(!strcmp(argv[i],"-Lhemivox") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) Lhemivox = argv[++i];
    if(!strcmp(argv[i],"-Rhemivox") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) Rhemivox = argv[++i];
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!Lseedmap) {
    printf("Error: Need to specify -Lseedmap\n");
    exit(-1);
    }
if(!Rseedmap) {
    printf("Error: Need to specify -Rseedmap\n");
    exit(-1);
    }
if(!Lhemivox) {
    printf("Error: Need to specify -Lhemivox\n");
    exit(-1);
    }
if(!Rhemivox) {
    printf("Error: Need to specify -Rhemivox\n");
    exit(-1);
    }
if(!(files=malloc(sizeof*files*nfiles))) {
    printf("Error: Unable to malloc files\n");
    exit(-1);
    }
files[0]=Lseedmap;files[1]=Rseedmap;
if(!(dp=dim_param(nfiles,files,SunOS_Linux,0))) exit(-1);
atlas = get_atlas(dp->vol);
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(!(ms=get_mask_struct(mask,dp->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
if(ms->lenvol != dp->vol) {
    printf("Error: %s, %s and %s not in the same space.\n",Lseedmap,Rseedmap,mask);
    printf("Error: dp->vol=%d  ms->lenvol=%d\n",dp->vol,ms->lenvol);
    exit(-1);
    }



if(!(LvoxLmap=malloc(sizeof*LvoxLmap*ms->lenbrain))) {
    printf("Error: Unable to malloc LvoxLmap\n");
    exit(-1);
    }
if(!(LvoxRmap=malloc(sizeof*LvoxRmap*ms->lenbrain))) {
    printf("Error: Unable to malloc LvoxRmap\n");
    exit(-1);
    }
if(!(RvoxLmap=malloc(sizeof*RvoxLmap*ms->lenbrain))) {
    printf("Error: Unable to malloc RvoxLmap\n");
    exit(-1);
    }
if(!(RvoxRmap=malloc(sizeof*RvoxRmap*ms->lenbrain))) {
    printf("Error: Unable to malloc RvoxRmap\n");
    exit(-1);
    }
if(!(Lmap=malloc(sizeof*Lmap*dp->vol))) {
    printf("Error: Unable to malloc Lmap\n");
    exit(-1);
    }
if(!(Rmap=malloc(sizeof*Rmap*dp->vol))) {
    printf("Error: Unable to malloc Rmap\n");
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

if(!readstack(Lseedmap,(float*)Lmap,sizeof(float),(size_t)dp->vol,SunOS_Linux)) exit(-1);
if(!readstack(Rseedmap,(float*)Rmap,sizeof(float),(size_t)dp->vol,SunOS_Linux)) exit(-1);

for(dptr=coor,l=r=i=0;i<ms->lenbrain;i++,dptr+=3) {
    if((*dptr)<0) {
        LvoxLmap[l] = Lmap[ms->brnidx[i]]; 
        LvoxRmap[l] = Rmap[ms->brnidx[i]]; 
        l++;
        }
    else {
        RvoxLmap[r] = Lmap[ms->brnidx[i]]; 
        RvoxRmap[r] = Rmap[ms->brnidx[i]]; 
        r++;
        }
    }

if(!(fp=fopen_sub(Lhemivox,"w"))) exit(-1);
for(i=0;i<l;i++) fprintf(fp,"%f\t%f\n",LvoxLmap[i],LvoxRmap[i]);
fclose(fp);
printf("Output written to %s\n",Lhemivox);fflush(stdout); 

if(!(fp=fopen_sub(Rhemivox,"w"))) exit(-1);
for(i=0;i<r;i++) fprintf(fp,"%f\t%f\n",RvoxLmap[i],RvoxRmap[i]);
fclose(fp);
printf("Output written to %s\n",Rhemivox);fflush(stdout); 
}
