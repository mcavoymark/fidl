/* Copyright 10/28/08 Washington University.  All Rights Reserved.
   fidl_spatialcor.c  $Revision: 1.9 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <gsl/gsl_fit.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_spatialcor.c,v 1.9 2011/02/25 16:21:01 mcavoy Exp $";

main(int argc,char **argv)
{
char *seedvox=NULL,*maskfile=NULL,filename[MAXNAME],*normy="_normy",string[MAXNAME],*labelx=NULL,*labely=NULL,*label1=NULL,
    *label2=NULL;
int i,j,k,l,m,ii,iii,SunOS_Linux,vol,xdim,ydim,zdim,x,y,zi,x1,y1,zi1,area,index,col_row,ncoor,*coor,count,swapbytes,lenvol,
    lcnormalizey=0,lcsigned=0,atlas,*atlascoor,*fidlcoor,lcecho=0,lccorcoef=0,lcregress=0,num_region_files=0,num_regions=0,*roi,
    volreg,ntcx1=0,ntcx2=0,ntcy1=0,ntcy2=0,tdim_max,nroot=0,nfiles=0,*count12,*count1;
float fwhm=1.,*mapx,*mapy,*temp_float,*mask,*temp_float2,*temp_float3,*temp_float4,*tcx1,*tcx2,*tcy1,*tcy2;
double *temp_double,*stat,*w,sumwxy,sumwx,sumwy,sumw,sumwx2,sumwy2,X,Y,*temp_double2,*stat2,*X1,*Y1,*normalizey,*c0,*c1,*xx,*yy,
    cov00,cov01,cov11,chisq,TR=0.,*tcx1d,*tcx2d,*tcy1d,*tcy2d,*tcx1r,*tcx2r,*tcy1r,*tcy2r,*tcx1rs,*tcx2rs,*tcy1rs,*tcy2rs,*x12rs,
    *y12rs;
Interfile_header *ifh;
Mask_Struct *ms,*seedvoxms;
Dim_Param *dptcx1,*dptcx2,*dptcy1,*dptcy2;
Atlas_Param *ap;
Files_Struct *region_files,*tcx1file=NULL,*tcx2file=NULL,*tcy1file=NULL,*tcy2file=NULL,*root=NULL;
Regions **reg;
Regions_By_File *rbf;
FILE *fp;

if(argc<7) {
    fprintf(stderr,"    -seedvox: Optional. Spatial correlation will be computed for each voxel in this map.\n");
    fprintf(stderr,"    -mask:    Optional. Only mask voxels are included in the computation.\n");
    fprintf(stderr,"    -root:    Output root.\n");
    fprintf(stderr,"    -tcx1:      This is computed as the sum over all frames i abs(tcx1[i]-tcx2[i]).\n");
    fprintf(stderr,"    -tcx2:\n");
    fprintf(stderr,"    -tcy1:      This is computed as the sum over all frames i abs(tcy1[i]-tcy2[i]).\n");
    fprintf(stderr,"    -tcy2:\n");
    fprintf(stderr,"    -normalizey Divide by the [sum over all frames i (tcy1[i]+tcy2[i])]/[2*nframes]\n");
    fprintf(stderr,"    -signed     Don't remove signs from differences\n");
    fprintf(stderr,"    -echo       Printf some stuff to stdout\n");
    fprintf(stderr,"    -corcoef    Compure correlation.\n");
    fprintf(stderr,"    -regress    Compute weighted linear regression.\n");
    fprintf(stderr,"    Print subtract timecourses point by point and magnitude.\n");
    fprintf(stderr,"        -region_file:         fidl region files.\n");
    fprintf(stderr,"        -regions_of_interest: First region is one.\n");
    fprintf(stderr,"        -labelx:              Common label for tcx1 and tcx2. e.g. det\n");
    fprintf(stderr,"        -labely:              Common label for tcy1 and tcy2. e.g. sto\n");
    fprintf(stderr,"        -label1:              Common label for tcx1 and tcy1. e.g. closed\n");
    fprintf(stderr,"        -label2:              Common label for tcx2 and tcy2. e.g. fix\n");
    fprintf(stderr,"        -TR:                  TR e.g. 2.5\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-seedvox") && argc > i+1)
        seedvox = argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        maskfile = argv[++i];
    if(!strcmp(argv[i],"-root") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroot;
        if(!(root=get_files(nroot,&argv[i+1]))) exit(-1);
        i += nroot;
        }
    if(!strcmp(argv[i],"-tcx1") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntcx1;
        if(!(tcx1file=get_files(ntcx1,&argv[i+1]))) exit(-1);
        i += ntcx1;
        }
    if(!strcmp(argv[i],"-tcx2") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntcx2;
        if(!(tcx2file=get_files(ntcx2,&argv[i+1]))) exit(-1);
        i += ntcx2;
        }
    if(!strcmp(argv[i],"-tcy1") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntcy1;
        if(!(tcy1file=get_files(ntcy1,&argv[i+1]))) exit(-1);
        i += ntcy1;
        }
    if(!strcmp(argv[i],"-tcy2") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntcy2;
        if(!(tcy2file=get_files(ntcy2,&argv[i+1]))) exit(-1);
        i += ntcy2;
        }
    if(!strcmp(argv[i],"-normalizey"))
        lcnormalizey = 1;
    if(!strcmp(argv[i],"-signed"))
        lcsigned = 1;
    if(!strcmp(argv[i],"-echo"))
        lcecho = 1;
    if(!strcmp(argv[i],"-corcoef"))
        lccorcoef = 1;
    if(!strcmp(argv[i],"-regress"))
        lcregress = 1;
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;
        if(!(roi=malloc(sizeof*roi*num_regions))) {
            printf("Error: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-labelx") && argc > i+1)
        labelx = argv[++i];
    if(!strcmp(argv[i],"-labely") && argc > i+1)
        labely = argv[++i];
    if(!strcmp(argv[i],"-label1") && argc > i+1)
        label1 = argv[++i];
    if(!strcmp(argv[i],"-label2") && argc > i+1)
        label2 = argv[++i];
    if(!strcmp(argv[i],"-TR") && argc > i+1)
        TR = atof(argv[++i]);
    }
if(!tcx1file) {
    printf("Error: Need to specify -tcx1\n");
    exit(-1);
    }
if(!tcx2file) {
    printf("Error: Need to specify -tcx2\n");
    exit(-1);
    }
if(!tcy1file) {
    printf("Error: Need to specify -tcy1\n");
    exit(-1);
    }
if(!tcy2file) {
    printf("Error: Need to specify -tcy2\n");
    exit(-1);
    }
if(tcx1file->nfiles!=tcx2file->nfiles||tcy1file->nfiles!=tcy2file->nfiles||tcx1file->nfiles!=tcy1file->nfiles) {
    printf("Error: tcx1file->nfiles=%d tcx2file->nfiles=%d tcy1file->nfiles=%d tcy2file->nfiles=%d  Must be equal. Abort!\n",
        tcx1file->nfiles,tcx2file->nfiles,tcy1file->nfiles,tcy2file->nfiles);
    exit(-1);
    }
nfiles = tcx1file->nfiles;
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!num_region_files) {
    if(!root) {
        printf("Error: Need to specify -root\n");
        exit(-1);
        }
    }
else {
    if(!labelx) {
        printf("Error: Need to specify -labelx\n");
        exit(-1);
        }
    if(!labely) {
        printf("Error: Need to specify -labely\n");
        exit(-1);
        }
    if(!label1) {
        printf("Error: Need to specify -label1\n");
        exit(-1);
        }
    if(!label2) {
        printf("Error: Need to specify -label2\n");
        exit(-1);
        }
    if(TR==0.) {
        printf("Error: Need to specify -TR\n");
        exit(-1);
        }
    }
if(!(dptcx1=dim_param(tcx1file->nfiles,tcx1file->files,SunOS_Linux,0))) exit(-1);
if(!(dptcx2=dim_param(tcx2file->nfiles,tcx2file->files,SunOS_Linux,0))) exit(-1);
if(!(dptcy1=dim_param(tcy1file->nfiles,tcy1file->files,SunOS_Linux,0))) exit(-1);
if(!(dptcy2=dim_param(tcy2file->nfiles,tcy2file->files,SunOS_Linux,0))) exit(-1);
if(dptcx1->vol!=dptcx2->vol||dptcy1->vol!=dptcy2->vol||dptcx1->vol!=dptcy1->vol) {
    printf("Error: dptcx1->vol=%d dptcx2->vol=%d dptcy1->vol=%d dptcy2->vol=%d  Must be equal. Abort!\n",dptcx1->vol,dptcx2->vol,
        dptcy1->vol,dptcy2->vol);
    exit(-1);
    }
vol = dptcx1->vol;
if(!dptcx1->all_tdim_same) {
    printf("Error: The time dimension of all -tcx1 files must be the same.\n");
    exit(-1);
    }
if(!dptcx2->all_tdim_same) {
    printf("Error: The time dimension of all -tcx2 files must be the same.\n");
    exit(-1);
    }
if(!dptcy1->all_tdim_same) {
    printf("Error: The time dimension of all -tcy1 files must be the same.\n");
    exit(-1);
    }
if(!dptcy2->all_tdim_same) {
    printf("Error: The time dimension of all -tcy2 files must be the same.\n");
    exit(-1);
    }
if(dptcx1->tdim_max!=dptcx2->tdim_max||dptcy1->tdim_max!=dptcy2->tdim_max||dptcx1->tdim_max!=dptcy1->tdim_max) {
    printf("Error: dptcx1->tdim_max=%d dptcx2->tdim_max=%d dptcy1->tdim_max=%d dptcy2->tdim_max=%d  Must be equal. Abort!\n",
        dptcx1->tdim_max,dptcx2->tdim_max,dptcy1->tdim_max,dptcy2->tdim_max);
    exit(-1);
    }
tdim_max = dptcx1->tdim_max;

if(!(X1=malloc(sizeof*X1*vol))) {
    printf("Error: Unable to malloc X1\n");
    exit(-1);
    }
if(!(Y1=malloc(sizeof*Y1*vol))) {
    printf("Error: Unable to malloc Y1\n");
    exit(-1);
    }

lenvol=vol*tdim_max;
if(!(normalizey=malloc(sizeof*normalizey*vol))) {
    printf("Error: Unable to malloc normalizey\n");
    exit(-1);
    }
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

if(!num_region_files) {
    atlas=get_atlas(vol);
    if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
    if(!(atlascoor=malloc(sizeof*atlascoor*3))) {
        printf("Error: Unable to malloc atlascoor\n");
        exit(-1);
        }
    if(!(fidlcoor=malloc(sizeof*fidlcoor*3))) {
        printf("Error: Unable to malloc fidlcoor\n");
        exit(-1);
        }
    if(!(seedvoxms=get_mask_struct(seedvox,vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
    if(seedvoxms->lenvol!=vol) {
        printf("Error: seedvoxms->lenvol=%d vol=%d Must be equal.\n",seedvoxms->lenvol,vol);
        exit(-1);
        }
    if(!(ms=get_mask_struct(maskfile,vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
    if(ms->lenvol!=vol) {
        printf("Error: ms->lenvol=%d vol=%d Must be equal.\n",ms->lenvol,vol);
        exit(-1);
        }
    if(!readstack(maskfile,(float*)mask,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    if(!(temp_double=malloc(sizeof*temp_double*vol))) {
        printf("Error: Unable to malloc temp_double\n");
        exit(-1);
        }
    if(!(stat=malloc(sizeof*stat*vol))) {
        printf("Error: Unable to malloc stat\n");
        exit(-1);
        }
    if(!(temp_float=malloc(sizeof*temp_float*vol))) {
        printf("Error: Unable to malloc temp_float\n");
        exit(-1);
        }
    if(!(temp_float2=malloc(sizeof*temp_float2*vol))) {
        printf("Error: Unable to malloc temp_float2\n");
        exit(-1);
        }
    if(!(mask=malloc(sizeof*mask*vol))) {
        printf("Error: Unable to malloc mask\n");
        exit(-1);
        }
    if(lccorcoef) {
        if(!(temp_double2=malloc(sizeof*temp_double2*vol))) {
            printf("Error: Unable to malloc temp_double2\n");
            exit(-1);
            }
        if(!(stat2=malloc(sizeof*stat2*vol))) {
            printf("Error: Unable to malloc stat2\n");
            exit(-1);
            }
        if(!(temp_float3=malloc(sizeof*temp_float3*vol))) {
            printf("Error: Unable to malloc temp_float3\n");
            exit(-1);
            }
        if(!(temp_float4=malloc(sizeof*temp_float4*vol))) {
            printf("Error: Unable to malloc temp_float4\n");
            exit(-1);
            }
        }

    xdim=dptcx1->xdim;ydim=dptcx1->ydim;zdim=dptcx1->zdim;
    x=xdim/2;y=ydim/2;zi=zdim/2;
    area = xdim*ydim;
    index = zi*area + y*xdim + x;
    for(i=0;i<vol;i++) temp_double[i]=0.; 
    temp_double[index]=1.;
    if(!gauss_smoth(temp_double,stat,xdim,ydim,zdim,fwhm,fwhm)) exit(-1);
    for(ncoor=i=0;i<vol;i++) if(stat[i]>0.) ncoor++;
    if(!(coor=malloc(sizeof*coor*ncoor*3))) {
        printf("Error: Unable to malloc coor\n");
        exit(-1);
        }
    if(!(w=malloc(sizeof*w*ncoor))) {
        printf("Error: Unable to malloc w\n");
        exit(-1);
        }
    for(k=j=i=0;i<vol;i++) {
        if(stat[i]>0.) {
            zi1 = i/area;
            col_row = i-zi1*area;
            y1 = col_row/xdim;
            x1 = col_row - y1*xdim;
            coor[j++]=x1-x;
            coor[j++]=y1-y;

            /*coor[j++]=zi1-zi;*/
            /*START110201*/
            coor[j++]=zi1+1-zi;

            w[k++]=stat[i];
            printf("%d %d %d %f\n",coor[j-3],coor[j-2],coor[j-1],stat[i]);
            }
        }
    if(!(ifh=init_ifh(4,dptcx1->xdim,dptcx1->ydim,dptcx1->zdim,1,dptcx1->dxdy,dptcx1->dxdy,dptcx1->dz,dptcx1->bigendian[0]))) exit(-1);

    if(lcregress) {
        if(!(xx=malloc(sizeof*xx*ncoor))) {
            printf("Error: Unable to malloc xx\n");
            exit(-1);
            }
        if(!(yy=malloc(sizeof*yy*ncoor))) {
            printf("Error: Unable to malloc yy\n");
            exit(-1);
            }
        if(!(c0=malloc(sizeof*c0*vol))) {
            printf("Error: Unable to malloc c0\n");
            exit(-1);
            }
        if(!(c1=malloc(sizeof*c1*vol))) {
            printf("Error: Unable to malloc c1\n");
            exit(-1);
            }
        }
    for(m=0;m<nfiles;m++) {
        for(i=0;i<vol;i++) X1[i]=Y1[i]=temp_double[i]=temp_float[i]=temp_float2[i]=0.; 
        if(lccorcoef) for(i=0;i<vol;i++) temp_float3[i]=temp_float4[i]=0.;
        if(!readstack(tcx1file->files[m],(float*)tcx1,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
        if(!readstack(tcx2file->files[m],(float*)tcx2,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
        if(!readstack(tcy1file->files[m],(float*)tcy1,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
        if(!readstack(tcy2file->files[m],(float*)tcy2,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
        for(ii=i=0;i<tdim_max;i++,ii+=vol) {
            for(j=0;j<ms->lenbrain;j++) {
                k=ii+ms->brnidx[j];
                if(tcx1[k]==(float)UNSAMPLED_VOXEL||tcx2[k]==(float)UNSAMPLED_VOXEL||tcy1[k]==(float)UNSAMPLED_VOXEL||
                    tcy2[k]==(float)UNSAMPLED_VOXEL||X1[ms->brnidx[j]]==(double)UNSAMPLED_VOXEL||
                    Y1[ms->brnidx[j]]==(double)UNSAMPLED_VOXEL) {
                    X1[ms->brnidx[j]]=Y1[ms->brnidx[j]]=(double)UNSAMPLED_VOXEL;
                    }
                else {
                    if(lcsigned) {
                        X1[ms->brnidx[j]]+=(double)(tcx1[k]-tcx2[k]);
                        Y1[ms->brnidx[j]]+=(double)(tcy1[k]-tcy2[k]);
                        }
                    else {
                        X1[ms->brnidx[j]]+=fabs((double)(tcx1[k]-tcx2[k]));
                        Y1[ms->brnidx[j]]+=fabs((double)(tcy1[k]-tcy2[k]));
                        }
                    #if 0
                    X1[ms->brnidx[j]]+=((double)(tcx1[k]-tcx2[k])*(double)(tcx1[k]-tcx2[k]));
                    Y1[ms->brnidx[j]]+=((double)(tcy1[k]-tcy2[k])*(double)(tcy1[k]-tcy2[k]));
                    #endif
                    #if 0
                    X1[ms->brnidx[j]]+=(double)tcx1[k]*(double)tcx1[k]+(double)tcx2[k]*(double)tcx2[k];
                    Y1[ms->brnidx[j]]+=(double)tcy1[k]*(double)tcy1[k]+(double)tcy2[k]*(double)tcy2[k];
                    #endif 
                    normalizey[ms->brnidx[j]]+=tcy1[k]+tcy2[k];
                    }
                }
            } 
        #if 0
        for(i=0;i<ms->lenbrain;i++) {
            X1[ms->brnidx[i]] = sqrt(X1[ms->brnidx[i]]);
            Y1[ms->brnidx[i]] = sqrt(Y1[ms->brnidx[i]]);
            } 
        #endif
        if(lcnormalizey) {
            printf("Normalizing Y1\n");
            for(i=0;i<ms->lenbrain;i++) {
                if(Y1[ms->brnidx[i]]!=(double)UNSAMPLED_VOXEL) 
                    Y1[ms->brnidx[i]]/=(normalizey[ms->brnidx[i]]/((double)tdim_max*2.));
                }
            }
        if(lccorcoef) {
            for(l=i=0;i<seedvoxms->lenbrain;i++) {
                zi = seedvoxms->brnidx[i]/area;
                col_row = seedvoxms->brnidx[i]-zi*area;
                y = col_row/xdim;
                x = col_row - y*xdim;
                for(sumwxy=sumwx=sumwy=sumw=sumwx2=sumwy2=0.,count=k=j=0;j<ncoor;j++) {
                    x1=x+coor[k++];
                    y1=y+coor[k++];
                    zi1=zi+coor[k++];
                    index = zi1*area + y1*xdim + x1;
                    if(mask[index]>(float)UNSAMPLED_VOXEL&&X1[index]!=(double)UNSAMPLED_VOXEL&&Y1[index]!=(double)UNSAMPLED_VOXEL) {
                        X=X1[index];Y=Y1[index];
                        sumwxy+=w[j]*X*Y;
                        sumwx+=w[j]*X;
                        sumwy+=w[j]*Y;
                        sumw+=w[j];
                        sumwx2+=w[j]*X*X;
                        sumwy2+=w[j]*Y*Y;
                        count++;
                        }
                    }
                if(count<ncoor) {
                    temp_float[seedvoxms->brnidx[i]]=temp_float2[seedvoxms->brnidx[i]]=temp_float3[seedvoxms->brnidx[i]]=
                        temp_float4[seedvoxms->brnidx[i]]=(float)UNSAMPLED_VOXEL;
                    }
                else {
                    temp_double[l]=(sumwxy-sumwx*sumwy/sumw)/sqrt((sumwx2-sumwx*sumwx/sumw)*(sumwy2-sumwy*sumwy/sumw));
                    if(lcecho) {
                        voxel_index_to_fidl_and_atlas(seedvoxms->brnidx[i],ap->xdim,ap->ydim,ap->zdim,ap->center,ap->mmppix,fidlcoor,
                            atlascoor);
                        printf("%d %d %d  cov(x,y)=%f sd(x)=%f sd(y)=%f cc=%f\n",atlascoor[0],atlascoor[1],atlascoor[2],
                            sumwxy-sumwx*sumwy/sumw,sqrt(sumwx2-sumwx*sumwx/sumw),sqrt(sumwy2-sumwy*sumwy/sumw),temp_double[l]);
                        }
                    temp_float[seedvoxms->brnidx[i]]=(float)temp_double[l];
                    stat[l]=.5*log((1.+temp_double[l])/(1.-temp_double[l]));
                    temp_float2[seedvoxms->brnidx[i]]=(float)stat[l];
                    l++;
                    }
                }
            swapbytes = shouldiswap(SunOS_Linux,dptcx1->bigendian[m]);
            k=strlen(seedvox)+1;
            if(ifh->file_name) free(ifh->file_name);
            if(!(ifh->file_name=malloc(sizeof*ifh->file_name*k))) {
                printf("Error: Unable to malloc ifh->file_name\n");
                exit(-1);
                }
            sprintf(ifh->file_name,"%s",seedvox);
            strcpy(string,root->files[m]);
            strcat(string,lcsigned?"_signed":"_unsigned");
            if(lcnormalizey) strcat(string,"_normy");
            sprintf(filename,"%s_spatialcorcoef.4dfp.img",string);
            if(!writestack(filename,temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
            min_and_max_init(&ifh->global_min,&ifh->global_max);
            min_and_max_doublestack(temp_double,l,&ifh->global_min,&ifh->global_max);
            if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
            printf("Weighted correlation coefficient written to %s\n",filename);
            sprintf(filename,"%s_spatialfisherz.4dfp.img",string);
            if(!writestack(filename,temp_float2,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
            min_and_max_init(&ifh->global_min,&ifh->global_max);
            min_and_max_doublestack(stat,l,&ifh->global_min,&ifh->global_max);
            if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
            printf("Weighted correlation coefficient written to %s\n",filename);
            }
        if(lcregress) {
            for(l=i=0;i<seedvoxms->lenbrain;i++) {
                zi = seedvoxms->brnidx[i]/area;
                col_row = seedvoxms->brnidx[i]-zi*area;
                y = col_row/xdim;
                x = col_row - y*xdim;
                for(count=k=j=0;j<ncoor;j++,count++) {
                    x1=x+coor[k++];
                    y1=y+coor[k++];
                    zi1=zi+coor[k++];
                    index = zi1*area + y1*xdim + x1;
                    if(mask[index]<=(float)UNSAMPLED_VOXEL||X1[index]==(double)UNSAMPLED_VOXEL||Y1[index]==(double)UNSAMPLED_VOXEL) 
                        break;
                    xx[j]=X1[index];yy[j]=Y1[index];
                    }
                if(count<ncoor) {
                    temp_float[seedvoxms->brnidx[i]]=(float)UNSAMPLED_VOXEL;
                    }
                else {
                    gsl_fit_wlinear(xx,1,w,1,yy,1,ncoor,&c0[l],&c1[l],&cov00,&cov01,&cov11,&chisq);
                    temp_double[l]=c1[l]/c0[l]*100;
                    temp_float[seedvoxms->brnidx[i]]=(float)temp_double[l];
                    temp_float2[seedvoxms->brnidx[i]]=(float)c1[l];
                    l++;
                    }
                }
            swapbytes = shouldiswap(SunOS_Linux,dptcx1->bigendian[m]);
            k=strlen(seedvox)+1;
            if(ifh->file_name) free(ifh->file_name);
            if(!(ifh->file_name=malloc(sizeof*ifh->file_name*k))) {
                printf("Error: Unable to malloc ifh->file_name\n");
                exit(-1);
                }
            sprintf(ifh->file_name,"%s",seedvox);
            strcpy(string,root->files[m]);
            strcat(string,lcsigned?"_signed":"_unsigned");
            if(lcnormalizey) strcat(string,"_normy");
            sprintf(filename,"%s_beta.4dfp.img",string);
            if(!writestack(filename,temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
            min_and_max_init(&ifh->global_min,&ifh->global_max);
            min_and_max_doublestack(temp_double,l,&ifh->global_min,&ifh->global_max);
            if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
            printf("Betas written to %s\n",filename);
            sprintf(filename,"%s_c1.4dfp.img",string);
            if(!writestack(filename,temp_float2,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
            min_and_max_init(&ifh->global_min,&ifh->global_max);
            min_and_max_doublestack(c1,l,&ifh->global_min,&ifh->global_max);
            if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
            printf("c1 written to %s\n",filename);
            }
        }
    }
else {
    if(!(ifh = read_ifh(region_files->files[0],(Interfile_header*)NULL))) exit(-1);
    volreg = ifh->dim1*ifh->dim2*ifh->dim3;
    free_ifh(ifh,(int)FALSE);
    if(!check_dimensions(num_region_files,region_files->files,volreg)) exit(-1);
    if(!(reg=malloc(sizeof*reg*num_region_files))) {
        printf("Error: Unable to malloc reg\n");
        exit(-1);
        }
    for(i=0;i<num_region_files;i++) {
        if(!(reg[i]=extract_regions(region_files->files[i],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) exit(-1);
        }
    if(!num_regions) for(i=0;i<num_region_files;i++) num_regions += reg[i]->nregions;
    if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);
    if(vol!=volreg) {
        printf("Error: vol=%d volreg=%d  Must be equal. Abort!\n",vol,volreg);
        exit(-1);
        }
    if(!(tcx1d=malloc(sizeof*tcx1d*vol))) {
        printf("Error: Unable to malloc tcx1d\n");
        exit(-1);
        }
    if(!(tcx2d=malloc(sizeof*tcx2d*vol))) {
        printf("Error: Unable to malloc tcx2d\n");
        exit(-1);
        }
    if(!(tcy1d=malloc(sizeof*tcy1d*vol))) {
        printf("Error: Unable to malloc tcy1d\n");
        exit(-1);
        }
    if(!(tcy2d=malloc(sizeof*tcy2d*vol))) {
        printf("Error: Unable to malloc tcy2d\n");
        exit(-1);
        }
    if(!(tcx1r=malloc(sizeof*tcx1r*num_regions))) {
        printf("Error: Unable to malloc tcx1r\n");
        exit(-1);
        }
    if(!(tcx2r=malloc(sizeof*tcx2r*num_regions))) {
        printf("Error: Unable to malloc tcx2r\n");
        exit(-1);
        }
    if(!(tcy1r=malloc(sizeof*tcy1r*num_regions))) {
        printf("Error: Unable to malloc tcy1r\n");
        exit(-1);
        }
    if(!(tcy2r=malloc(sizeof*tcy2r*num_regions))) {
        printf("Error: Unable to malloc tcy2r\n");
        exit(-1);
        }
    if(!(tcx1rs=malloc(sizeof*tcx1rs*num_regions*tdim_max))) {
        printf("Error: Unable to malloc tcx1rs\n");
        exit(-1);
        }
    for(m=0;m<num_regions*tdim_max;m++) tcx1rs[m]=0.;
    if(!(tcx2rs=malloc(sizeof*tcx2rs*num_regions*tdim_max))) {
        printf("Error: Unable to malloc tcx2rs\n");
        exit(-1);
        }
    for(m=0;m<num_regions*tdim_max;m++) tcx2rs[m]=0.;
    if(!(tcy1rs=malloc(sizeof*tcy1rs*num_regions*tdim_max))) {
        printf("Error: Unable to malloc tcy1rs\n");
        exit(-1);
        }
    for(m=0;m<num_regions*tdim_max;m++) tcy1rs[m]=0.;
    if(!(tcy2rs=malloc(sizeof*tcy2rs*num_regions*tdim_max))) {
        printf("Error: Unable to malloc tcy2rs\n");
        exit(-1);
        }
    for(m=0;m<num_regions*tdim_max;m++) tcy2rs[m]=0.;
    if(!(x12rs=malloc(sizeof*x12rs*num_regions*tdim_max))) {
        printf("Error: Unable to malloc x12rs\n");
        exit(-1);
        }
    for(m=0;m<num_regions*tdim_max;m++) x12rs[m]=0.;
    if(!(y12rs=malloc(sizeof*y12rs*num_regions*tdim_max))) {
        printf("Error: Unable to malloc y12rs\n");
        exit(-1);
        }
    for(m=0;m<num_regions*tdim_max;m++) y12rs[m]=0.;

    if(!(count12=malloc(sizeof*count12*num_regions*tdim_max))) {
        printf("Error: Unable to malloc count12\n");
        exit(-1);
        }
    for(m=0;m<num_regions*tdim_max;m++) count12[m]=0;
    if(!(count1=malloc(sizeof*count1*num_regions))) {
        printf("Error: Unable to malloc count1\n");
        exit(-1);
        }
    for(m=0;m<num_regions;m++) count1[m]=0;

    for(m=0;m<num_regions;m++) X1[m]=Y1[m]=0.; 
    for(m=0;m<nfiles;m++) {
        printf("m=%d\n",m);
        if(!readstack(tcx1file->files[m],(float*)tcx1,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
        if(!readstack(tcx2file->files[m],(float*)tcx2,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
        if(!readstack(tcy1file->files[m],(float*)tcy1,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
        if(!readstack(tcy2file->files[m],(float*)tcy2,sizeof(float),(size_t)lenvol,SunOS_Linux)) exit(-1);
        for(iii=ii=i=0;i<tdim_max;i++,ii+=vol) {
            for(j=0;j<vol;j++) {
                tcx1d[j] = tcx1[ii+j]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL : (double)tcx1[ii+j];
                tcx2d[j] = tcx2[ii+j]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL : (double)tcx2[ii+j];
                tcy1d[j] = tcy1[ii+j]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL : (double)tcy1[ii+j];
                tcy2d[j] = tcy2[ii+j]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL : (double)tcy2[ii+j];
                }
            if(!(crs(tcx1d,tcx1r,rbf,(char*)NULL))) exit(-1);
            if(!(crs(tcx2d,tcx2r,rbf,(char*)NULL))) exit(-1);
            if(!(crs(tcy1d,tcy1r,rbf,(char*)NULL))) exit(-1);
            if(!(crs(tcy2d,tcy2r,rbf,(char*)NULL))) exit(-1);
            for(j=0;j<num_regions;j++,iii++) {
                if(tcx1r[j]==(float)UNSAMPLED_VOXEL||tcx2r[j]==(float)UNSAMPLED_VOXEL||tcy1r[j]==(float)UNSAMPLED_VOXEL||
                    tcy2r[j]==(float)UNSAMPLED_VOXEL||X1[j]==(double)UNSAMPLED_VOXEL||Y1[j]==(double)UNSAMPLED_VOXEL) {
                    continue;
                    }
                tcx1rs[iii] += tcx1r[j];
                tcx2rs[iii] += tcx2r[j];
                tcy1rs[iii] += tcy1r[j];
                tcy2rs[iii] += tcy2r[j];
                X=fabs((double)(tcx1r[j]-tcx2r[j])); Y=fabs((double)(tcy1r[j]-tcy2r[j]));
                x12rs[iii]+=X; y12rs[iii]+=Y;
                X1[j]+=X; Y1[j]+=Y;
                count12[iii]++; count1[j]++; 

                /*printf("%s\n",rbf->region_names_ptr[j]);
                printf("tcx1r[%d]=%f tcx2r[%d]=%f tcy1r[%d]=%f tcy2r[%d]=%f\n",j,tcx1r[j],j,tcx2r[j],j,tcy1r[j],j,tcy2r[j]);
                printf("X=%f Y=%f\n",X,Y);*/
                printf("%f %f %f %f\n",tcx1r[j],tcx2r[j],X,X1[j]);
                }
            }
        }
    for(iii=i=0;i<tdim_max;i++) {
        for(j=0;j<num_regions;j++,iii++) {
            printf("tcx1rs = %f ",tcx1rs[iii]);
            tcx1rs[iii] /= (double)count12[iii];
            printf("%f\n",tcx1rs[iii]);
            printf("tcx2rs = %f ",tcx2rs[iii]);
            tcx2rs[iii] /= (double)count12[iii];
            printf("%f\n",tcx2rs[iii]);
            tcy1rs[iii] /= (double)count12[iii];
            tcy2rs[iii] /= (double)count12[iii];
            printf("%f ",x12rs[iii]);
            x12rs[iii] /= (double)count12[iii];
            printf("%f\n",x12rs[iii]);
            y12rs[iii] /= (double)count12[iii];
            /*printf("count12[%d]=%d\n",iii,count12[iii]);*/
            }
        }
    for(j=0;j<num_regions;j++) {
        X1[j] /= (double)count1[j];
        Y1[j] /= (double)count1[j];
        printf("count1[%d]=%d\n",j,count1[j]);
        } 

    sprintf(filename,"mean_%s.txt",labelx);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<num_regions;i++) fprintf(fp,"\t%s",rbf->region_names_ptr[i]);
    fprintf(fp,"\n");
    for(iii=ii=i=0;i<tdim_max;i++) {
        fprintf(fp,"%f_%s",i*TR,label1);
        for(j=0;j<num_regions;j++,ii++) fprintf(fp,"\t%f",tcx1rs[ii]); 
        fprintf(fp,"\n");
        fprintf(fp,"%f_%s",i*TR,label2);
        for(j=0;j<num_regions;j++,iii++) fprintf(fp,"\t%f",tcx2rs[iii]);
        fprintf(fp,"\n");
        }
    fclose(fp);
    printf("Output written to %s\n",filename);

    sprintf(filename,"mean_%s.txt",labely);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<num_regions;i++) fprintf(fp,"\t%s",rbf->region_names_ptr[i]);
    fprintf(fp,"\n");
    for(iii=ii=i=0;i<tdim_max;i++) {
        fprintf(fp,"%f_%s",i*TR,label1);
        for(j=0;j<num_regions;j++,ii++) fprintf(fp,"\t%f",tcy1rs[ii]);
        fprintf(fp,"\n");
        fprintf(fp,"%f_%s",i*TR,label2);
        for(j=0;j<num_regions;j++,iii++) fprintf(fp,"\t%f",tcy2rs[iii]);
        fprintf(fp,"\n");
        }
    fclose(fp);
    printf("Output written to %s\n",filename);

    sprintf(filename,"mean_abs_%s-%s.txt",label1,label2);
    if(!(fp=fopen_sub(filename,"w"))) exit(-1);
    for(i=0;i<num_regions;i++) fprintf(fp,"\t%s",rbf->region_names_ptr[i]);
    fprintf(fp,"\n");
    for(iii=ii=i=0;i<tdim_max;i++) {
        fprintf(fp,"%f_%s",i*TR,labelx);
        for(j=0;j<num_regions;j++,ii++) fprintf(fp,"\t%f",x12rs[ii]);
        fprintf(fp,"\n");
        fprintf(fp,"%f_%s",i*TR,labely);
        for(j=0;j<num_regions;j++,iii++) fprintf(fp,"\t%f",y12rs[iii]);
        fprintf(fp,"\n");
        }
    fprintf(fp,"%f_%s",i*TR,labelx);
    for(j=0;j<num_regions;j++) fprintf(fp,"\t%f",X1[j]);
    fprintf(fp,"\n");
    fprintf(fp,"%f_%s",i*TR,labely);
    for(j=0;j<num_regions;j++) fprintf(fp,"\t%f",Y1[j]);
    fprintf(fp,"\n");
    fclose(fp);
    printf("Output written to %s\n",filename);
    }
}
