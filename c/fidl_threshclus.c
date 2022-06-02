/* Copyright 8/27/14 Washington University.  All Rights Reserved.
   fidl_threshclus.c  $Revision: 1.4 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fidl.h"
#include "spatial_extent2.h"
#include "center_of_mass2.h"
#include "assign_region_names2.h"
/*static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_threshclus.c,v 1.4 2015/05/05 18:49:48 mcavoy Exp $";*/
int main(int argc,char **argv)
{
char *zmap=NULL,filename[MAXNAME],string[MAXNAME],*act_type_str[]={"all","pos","neg"},root[MAXNAME],*strptr,atlas[7]="";
int i,j,k,vol,nthresh=0,*extent=NULL,SunOS_Linux,lccom=0,lcregions_only=0,lcmap_only=0,lcpeak=0,swapbytes,lcallposneg[]={0,0,0};
float *z,*zc,*actmask;
double *zd,*temp_double,*thresh=NULL;
Interfile_header *ifh;
Atlas_Param *ap;
Mask_Struct *ms;
Spatial_Extent2 *se;
center_of_mass2struct *com=NULL;
assign_region_names2struct *arn=NULL;
if(argc<5) {
    fprintf(stderr,"        -zmap:                   Z map.\n");
    fprintf(stderr,"        -threshold_extent:       Thresholds with spatial extent.\n");
    fprintf(stderr,"                                 Ex. -threshold_extent \"3.75 18\" \"4 12\"\n");
    fprintf(stderr,"        -all                     Positives and negatives. This is the default.\n");
    fprintf(stderr,"        -pos                     Positives only.\n");
    fprintf(stderr,"        -neg                     Negatives only.\n");
    fprintf(stderr,"        -peak                    Region name is peak atlas coordinate.\n");
    fprintf(stderr,"        -center_of_mass          Region name is center of mass atlas coordinate.\n");
    fprintf(stderr,"     If both -peak and -center_of_mass, then region name includes both.\n");
    fprintf(stderr,"     If neither is specified, then regions are named voxel_value2, voxel_value3, ...\n");
    fprintf(stderr,"        -regions_only            Output region file only.\n");
    fprintf(stderr,"        -map_only                Output thresholded and clustered image only.\n");
    exit(-1);
    } 
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-zmap") && argc > i+1)
        zmap = argv[++i];
    if(!strcmp(argv[i],"-threshold_extent") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nthresh;
        if(!(thresh=malloc(sizeof*thresh*nthresh))) {
            printf("Error: Unable to malloc thresh\n");
            exit(-1);
            }
        if(!(extent=malloc(sizeof*extent*nthresh))) {
            printf("Error: Unable to malloc extent\n");
            exit(-1);
            }
        for(j=0;j<nthresh;j++) sscanf(argv[++i],"%lf %d",&thresh[j],&extent[j]);
        }
    if(!strcmp(argv[i],"-all")) lcallposneg[0]=1;
    if(!strcmp(argv[i],"-pos")) lcallposneg[1]=1;
    if(!strcmp(argv[i],"-neg")) lcallposneg[2]=1;
    if(!strcmp(argv[i],"-center_of_mass")) lccom = 1;
    if(!strcmp(argv[i],"-regions_only")) lcregions_only = 1;
    if(!strcmp(argv[i],"-map_only")) lcmap_only = 1;
    if(!strcmp(argv[i],"-peak")) lcpeak=1; 
    if(!strcmp(argv[i],"-center_of_mass")) lccom=1;
    }
if(!zmap) {
    printf("Error: Need to specify zmap with -zmap  Abort!\n");
    exit(-1);
    }
if(!nthresh) {
    printf("Error: Need to specify threshold and extent with -threshold_extent  Abort!\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!lcallposneg[0]&&!lcallposneg[1]&&!lcallposneg[2]) lcallposneg[0]=1;
if(!(ifh=read_ifh(zmap,(Interfile_header*)NULL))) exit(-1);
vol = ifh->dim1*ifh->dim2*ifh->dim3;
swapbytes=shouldiswap(SunOS_Linux,ifh->bigendian);
if(!(z=malloc(sizeof*z*vol))) {
    printf("fidlError: Unable to malloc z\n");
    exit(-1);
    }
if(!(zd=malloc(sizeof*zd*vol))) {
    printf("fidlError: Unable to malloc zd\n");
    exit(-1);
    }
if(!(actmask=malloc(sizeof*actmask*vol))) {
    printf("fidlError: Unable to malloc actmask\n");
    exit(-1);
    }
if(!(zc=malloc(sizeof*zc*vol))) {
    printf("fidlError: Unable to malloc zc\n");
    exit(-1);
    }
for(i=0;i<vol;i++) zc[i]=0.;
if(!readstack(zmap,(float*)z,sizeof(float),(size_t)vol,SunOS_Linux,ifh->bigendian)) exit(-1);
get_atlas(vol,atlas);
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(nthresh>1) {
    if(!(temp_double=malloc(sizeof*temp_double*vol))) {
        printf("fidlError: Unable to malloc temp_double\n");
        exit(-1);
        }
    }
else {
    temp_double=zd;
    }
if(!(ms=get_mask_struct(ifh->mask,vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,vol))) exit(-1);
if(!(se=spatial_extent2init(ifh->dim1,ifh->dim2,ifh->dim3,ms->lenbrain,ms->brnidx))) exit(-1);
if(!lcmap_only) {
    if(lcpeak||lccom) if(!(com=center_of_mass2init(ms->lenbrain,ms->lenbrain))) exit(-1);
    if(!(arn=assign_region_names2init(ms->lenbrain))) exit(-1);
    }
strcpy(string,zmap);
if(!(strptr=get_tail_sans_ext(string))) exit(-1);
for(i=0;i<3;i++) {
    if(lcallposneg[i]) {
        if(!i) {
            for(j=0;j<vol;j++) zd[j]=(double)z[j];
            }
        else if(i==1) {
            for(j=0;j<vol;j++) zd[j]=z[j]<0.?0.:(double)z[j];
            }
        else if(i==2) {
            for(j=0;j<vol;j++) zd[j]=z[j]>0.?0.:(double)z[j];
            }
        for(j=0;j<nthresh;j++) {
            if(nthresh>1) for(k=0;k<vol;k++) temp_double[k] = zd[k];
            sprintf(root,"%s_z%gn%d_%s",strptr,thresh[j],extent[j],act_type_str[i]);
            spatial_extent2(temp_double,actmask,thresh[j],extent[j],vol,1,se);
            for(k=0;k<ms->lenbrain;k++) /*crushed image includes extra voxels*/
                zc[ms->brnidx[k]]=!actmask[ms->brnidx[k]]&&z[ms->brnidx[k]]!=(float)UNSAMPLED_VOXEL?0.:z[ms->brnidx[k]];
            fprintf(stderr,"se->nreg = %d\n",se->nreg);
            if(!lcregions_only) {
                sprintf(filename,"%s.4dfp.img",root);
                if(!writestack(filename,zc,sizeof(float),(size_t)vol,swapbytes)) return 0;
                min_and_max(zc,vol,&ifh->global_min,&ifh->global_max);
                if(!write_ifh(filename,ifh,0)) exit(-1);
                printf("Output written to %s\n",filename);
                }
            if(!lcmap_only&&se->nreg) {
                sprintf(filename,"%s_reg.4dfp.img",root);
                if(!writestack(filename,actmask,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
                if(com) center_of_mass2(zc,com,se->nvox,se->reglist,se->nreg,se->regsize,ap);
                
                /*for(l=k=0;k<se->nreg;k++,l+=3) printf("%f %f %f com->peakval[%d]=%f\n",com->peakcoor[l],com->peakcoor[l+1],
                    com->peakcoor[l+2],k,com->peakval[k]);fflush(stdout);*/

                if(!assign_region_names2(se->nreg,arn,ifh,se->regsize,!lccom?(double*)NULL:com->coor,
                    !lcpeak?(double*)NULL:com->peakcoor,!lcpeak?(float*)NULL:com->peakval,(char**)NULL)) exit(-1);
                min_and_max(actmask,vol,&ifh->global_min,&ifh->global_max);
                if(!write_ifh(filename,ifh,0)) exit(-1);
                free_ifhregnames(ifh);
                printf("Region file written to %s\n",filename);
                }
            }
        }
    }
}
