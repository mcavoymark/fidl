/* Copyright 2/9/05 Washington University.  All Rights Reserved.
   fidl_flipreg.c  $Revision: 1.11 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fidl.h"
#include "assign_region_names2.h"
int main(int argc,char **argv)
{
char *outfile="flipreg.4dfp.img",*strptr,string[MAXNAME],atlas[7]=""; /* *atlas=NULL */
int i,j,k,nregion_files=0,nregnames=0,*roi=NULL,SunOS_Linux,swapbytes,*xi,*yi,*zi,*index; 
size_t nroi=0;
float f,*temp_float;
double *col,*row,*slice,*coor,*xd,*yd,*zd;
/*FILE *fp;*/
Interfile_header *ifh;
Files_Struct *region_files=NULL,*regnames=NULL;
Dim_Param *dp;
Atlas_Param *ap;
Regions **reg;
Regions_By_File *rbf;
assign_region_names2struct *arn;
if(argc < 5) {
    fprintf(stderr,"        -region_file:         *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"        -regions_of_interest: Calculate statistics for selected regions in the region file(s).\n"); 
    fprintf(stderr,"                              First region is one.\n"); 
    fprintf(stderr,"        -out_file:            Flipped regions are written to this file.\n");
    fprintf(stderr,"        -regions:             List of names to be assigned to the flipped regions.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nregion_files;
        if(!(region_files=get_files(nregion_files,&argv[i+1]))) exit(-1);
        i += nregion_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroi;
        if(!(roi=malloc(sizeof*roi*nroi))) {
            printf("Error: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<nroi;j++) roi[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-out_file") && argc > i+1)
        outfile = argv[++i];

    /*START141201*/
    #if 0
    if(!strcmp(argv[i],"-td_file") && argc > i+1)
        tdfile = argv[++i];
    #endif

    if(!strcmp(argv[i],"-regions") && argc > i+1) {
        for(j=1;i+j<argc;j++) {
            strptr = argv[i+j] + 1;

            /*if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit(*strptr)) break;*/
            /*START141202*/
            if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit((int)*strptr)) break;

            ++nregnames;
            }
        if(!(regnames=get_files(nregnames,&argv[i+1]))) exit(-1);
        i += nregnames;
        }
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!nregion_files) {
    printf("Error: No region files specified with -region_file option.\n");
    exit(-1);
    }
if(nroi!=nregnames || (!nroi&&!nregnames)) {
    printf("You have specified %lu regions with the -regions_of_interest option.\n",(unsigned long)nroi);
    printf("You have provided %d region names for the flipped regions with the -regions option.\n",nregnames);
    printf("They both must be the same nonzero number. Abort!\n");
    }
if(!(dp=dim_param(nregion_files,region_files->files,SunOS_Linux,0,(int*)NULL,(int*)NULL))) exit(-1);
if(!(reg=malloc(sizeof*reg*nregion_files))) {
    printf("Error: Unable to malloc reg\n");
    exit(-1);
    }
for(i=0;i<nregion_files;i++) if(!(reg[i] = extract_regions(region_files->files[i],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL)))
    exit(-1);
if(!(rbf=find_regions_by_file_cover(nregion_files,nroi,reg,roi))) exit(-1);
if(!(temp_float=malloc(sizeof*temp_float*dp->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<dp->vol;i++) temp_float[i]=0.;

/*START150408*/
#if 0
if(!(tst=malloc(sizeof*tst*nroi))) {
    printf("Error: Unalble to malloc tst\n");
    exit(-1);
    }
#endif

if(!(ifh=read_ifh(region_files->files[0],(Interfile_header*)NULL))) exit(-1);
free_ifhregnames(ifh);

/*START150408*/
get_atlas(dp->vol,atlas);

if(!(ap=get_atlas_param(atlas,ifh))) exit(-1);
if(!(coor=malloc(sizeof*coor*rbf->nvoxels*3))) {
    printf("Error: Unable to malloc coor\n");
    exit(-1);
    }
if(!(col=malloc(sizeof*col*rbf->nvoxels))) {
    printf("Error: Unable to malloc col\n");
    exit(-1);
    }
if(!(row=malloc(sizeof*row*rbf->nvoxels))) {
    printf("Error: Unable to malloc row\n");
    exit(-1);
    }
if(!(slice=malloc(sizeof*slice*rbf->nvoxels))) {
    printf("Error: Unable to malloc slice\n");
    exit(-1);
    }
if(!(xi=malloc(sizeof*xi*rbf->nvoxels))) {
    printf("Error: Unable to malloc xi\n");
    exit(-1);
    }
if(!(yi=malloc(sizeof*yi*rbf->nvoxels))) {
    printf("Error: Unable to malloc yi\n");
    exit(-1);
    }
if(!(zi=malloc(sizeof*zi*rbf->nvoxels))) {
    printf("Error: Unable to malloc zi\n");
    exit(-1);
    }
if(!(xd=malloc(sizeof*xd*rbf->nvoxels))) {
    printf("Error: Unable to malloc xd\n");
    exit(-1);
    }
if(!(yd=malloc(sizeof*yd*rbf->nvoxels))) {
    printf("Error: Unable to malloc yd\n");
    exit(-1);
    }
if(!(zd=malloc(sizeof*zd*rbf->nvoxels))) {
    printf("Error: Unable to malloc zd\n");
    exit(-1);
    }
if(!(index=malloc(sizeof*index*rbf->nvoxels))) {
    printf("Error: Unable to malloc index\n");
    exit(-1);
    }
col_row_slice(rbf->nvoxels,rbf->indices,col,row,slice,ap);
get_atlas_coor(rbf->nvoxels,col,row,slice,(double)ap->zdim,ap->center,ap->mmppix,coor);
for(j=i=0;i<rbf->nvoxels;i++,j+=3) coor[j] *= -1.;
atlas_to_index2(rbf->nvoxels,coor,ap,xi,yi,zi,xd,yd,zd,index);
for(f=(float)nroi+1.,k=rbf->nvoxels-1,i=nroi-1;i>=0;i--,f-=1.) { /*Start at bottom to preserve top regions.*/
    for(j=0;j<rbf->nvoxels_region[i];j++,k--) {
        temp_float[index[k]] = f;
        }
    }


#if 0
for(i=0;i<nroi;i++) {
    sprintf(string,"%d %s %d",i,regnames->files[i],rbf->nvoxels_region[i]);
    tst[i] = strlen(string)+1;
    }
if(!(ifh->region_names=d2charvar(nroi,tst))) exit(-1);
for(i=0;i<nroi;i++) {
    sprintf(string,"%d %s %d",i,regnames->files[i],rbf->nvoxels_region[i]);
    strcpy(ifh->region_names[i],string);
    }
ifh->nregions = nroi;
#endif
/*START150408*/
if(!(arn=assign_region_names2init(nroi))) exit(-1);
if(!assign_region_names2(nroi,arn,ifh,rbf->nvoxels_region,(double*)NULL,(double*)NULL,(float*)NULL,regnames->files))exit(-1);


ifh->global_min = 0.;
ifh->global_max = (float)nroi+1.;
swapbytes=shouldiswap(SunOS_Linux,ifh->bigendian);
if(!writestack(outfile,temp_float,sizeof(float),(size_t)ap->vol,swapbytes)) exit(-1);
if(!write_ifh(outfile,ifh,(int)FALSE)) exit(-1);
printf("Flipped regions written to %s\n",outfile);
}
