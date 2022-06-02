/* Copyright 10/19/05 Washington University. All Rights Reserved.
   fidl_cc.c  $Revision: 1.19 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

/*#ifndef LINUX*/
#ifdef __sun__

    #include <nan.h>
#endif
#include "matrix2.h"

int read_logregwts_file(char *logregwts_file,int nwts,double *wts);

/*START140729*/
void assignX(int num_tc_files,int *num_regions_by_file,Files_Struct_new *sn,int num_regions,int num_wfiles,int goose,int j,
    int p,int *sindex,FS *fs,MAT *X,Scratch *s)


static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_cc.c,v 1.19 2014/07/29 18:42:13 mcavoy Exp $";

main(int argc,char **argv)
{
char *regional_name="fidl_cc.txt",*xform_file=NULL,filename[MAXNAME],filename2[MAXNAME],*mask_file=NULL,*str_ptr,**scratchfiles,
    *scratchdir,*frames_file=NULL,*logregwts_file=NULL,*ROCarea_name=NULL,Cxxstr[25],Cyystr[25];

int i,j,k,l,m,n,p,q,r,ii,jj,kk,ll,num_regions=0,num_region_files=0,num_tc_files=0,*roi=NULL,spaces,
    num_tc_names=0,ntc_frames=0,nscratchfiles,A_or_B_or_U,atlas=222,num_wfiles=1,increment,iterations,status,lcscratch=1,one_more,
    lccleanup=0,*temp_int,n0,n1,SunOS_Linux,*nframes,*sindex,half,lengthX,argc_tc_frames,
    sx,sy,pp,lcregwithvox=0,nreg=1,nvox,nroot=0,*sindexreg,toomanyreg,toomanyvox,startx,once,modulo=20000,
    ninf,*infi,flag,lccos=0,maxevi,*val,nval,nweights=0,swapbytes;
 
float *temp_float,**pval,***prob,*logregwts_stack;

double *temp_double,*w,*wstack,*wts,*mean,*td,*chisquare,minusv,cond,N,max,min,dcol,drow,dslice,maxev,*costheta,mag1,mag2,*df,*z,
       *weights,ip1,ip2,sumprod,atlas_coor[3];

/*START0*/
MAT *Q,*T,*X_re,*X_im,*X,*Xreg,*cov,*Cxx,*Cxxinv,*Cxy,*Cyy,*Cyyinv,*CxxinvCxy,*CyyinvCyx,*Wy,*Tcopy;
VEC *evals_re,*evals_im;


FILE *fp,*fp2;
Regions **reg;
Regions_By_File *rbf=NULL,*fbf,*fbfreg; /*NOTE THIS CHANGE*/
Atlas_Param *ap;
Dim_Param *dp;
Memory_Map *mm,*mm_w;  /*NOTE THIS CHANGE*/
Interfile_header *ifh;
Mask_Struct *ms;
FS *fs,*fsreg;
Files_Struct *tc_names,*tc_files,*region_files,*wfiles=NULL,*root;
TCnew *tc_frames;
Files_Struct_new *sn,*snreg; /*NOTE*/
/*PC *pc;*/
Scratch *s;

if(argc < 5) {
    fprintf(stderr,"        -tc_files:            4dfp timecourse files.\n");
    fprintf(stderr,"        -region_file:         *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"                              Timecourses are averaged over the region.\n");
    fprintf(stderr,"        -regions_of_interest: Compute timecourses for selected regions in the region file(s).\n");
    fprintf(stderr,"                              First region is one.\n");
    fprintf(stderr,"        -weight_files:        4dfp weight files. Provide weights for regional weighted means.\n");
    fprintf(stderr,"                              Without this option the regional arithmetic mean is computed.\n");
    fprintf(stderr,"        -frames:              Frames to extract. First frame is 1.\n");
    fprintf(stderr,"        -regional_name:       Output filename for regional results. Default is fidl_logreg_ss.txt\n");
    fprintf(stderr,"        -tc_names:            Timcourse identifiers.\n");
    fprintf(stderr,"        -tc_frames:           First timepoint is 1.\n");
    fprintf(stderr,"        -xform_file:          Name of 2A or 2B t4 file defining the transform to atlas space.\n");
    fprintf(stderr,"        -atlas:               Either 111, 222 or 333. Default is 222. Used with -xform_files option.\n");
    fprintf(stderr,"        -mask:                Only voxels in the mask are analyzed.\n");
    fprintf(stderr,"        -root:                Output root(s).\n");
    fprintf(stderr,"        -regwithvox           Correlate region with brain.\n");
    fprintf(stderr,"        -regwithvox:          Correlate inner product of region tc and logregwts with brain.\n");
    fprintf(stderr,"                              Logreg text output file.\n");
    /*fprintf(stderr,"        -weights:             Correlate these weights with brain.\n");*/
    fprintf(stderr,"        -scratchdir:          Location of precomputed scratchfiles. Include the backslash at the end.\n");
    fprintf(stderr,"        -clean_up             Delete the scratch files and directory.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-tc_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_files;
        if(!(tc_files=get_files(num_tc_files,&argv[i+1]))) exit(-1);
        i += num_tc_files;
        }
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
    if(!strcmp(argv[i],"-weight_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_wfiles;
        num_wfiles--; /*Initialized to 1 instead of 0 in declaration.*/
        if(!(wfiles=get_files(num_wfiles,&argv[i+1]))) exit(-1);
        i += num_wfiles;
        }
    if(!strcmp(argv[i],"-frames") && argc > i+1)
        frames_file = argv[++i];
    if(!strcmp(argv[i],"-regional_name") && argc > i+1)
        regional_name = argv[++i];
    if(!strcmp(argv[i],"-tc_names") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_names;
        if(!(tc_names=get_files(num_tc_names,&argv[i+1]))) exit(-1);
        i += num_tc_names;
        }
    if(!strcmp(argv[i],"-tc_frames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntc_frames;
        argc_tc_frames = i+1;
        i += ntc_frames;
        }
    if(!strcmp(argv[i],"-xform_file") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        xform_file = argv[++i];
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        atlas = atoi(argv[++i]);
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-root") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroot;
        if(!(root=get_files(nroot,&argv[i+1]))) exit(-1);
        i += nroot;
        }
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
        scratchdir = argv[++i];
        lcscratch = 0;
        }
    if(!strcmp(argv[i],"-regwithvox")) {
        lcregwithvox = 1;
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) logregwts_file = argv[++i];
        }
    #if 0
    if(!strcmp(argv[i],"-weights") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nweights;
        if(!(weights=malloc(sizeof*weights*nweights))) {
            printf("Error: Unable to malloc weights\n");
            exit(-1);
            }
        for(j=0;j<nweights;j++) weights[j] = atof(argv[++i]);
        }
    #endif
    if(!strcmp(argv[i],"-clean_up"))
        lccleanup = 1;
    }
print_version_number(rcsid,stdout);
if(!num_tc_files) {
    printf("Error: No timecourse files. Abort!\n");
    exit(-1);
    }
if(num_tc_names != 2) {
    printf("Error: Only two variables may be analyzed by canonical correlation. Abort!\n");
    exit(-1);
    }
if(!frames_file) {
    printf("Error: Need to specify frames file with -frames option. Abort!\n");
    exit(-1);
    }
if(!root) {
    printf("Error: Name output with -root\n");
    exit(-1);
    }


/*#ifndef DONTSWAP
    if((SunOS_Linux=checkOS())==-1) return 0;
#else
    SunOS_Linux = 0;
#endif*/
if((SunOS_Linux=checkOS())==-1) return 0;




if(!(tc_frames=read_tc_string_TCnew(ntc_frames,0,argc_tc_frames,argv,'+'))) exit(-1);
/*printf("tc_frames->eachi[0]=%d tc->frames->eachi[1]=%d tc_frames->each[0]=%d tc_frames->each[1]=%d\n",tc_frames->eachi[0],
    tc_frames->eachi[1],tc_frames->each[0],tc_frames->each[1]);*/

if(!lcregwithvox) {
    if(!(fs=read_frames_file(frames_file,0,0,0))) exit(-1);
    }
else {
    if(!(fsreg=read_frames_file(frames_file,0,tc_frames->eachi[0],tc_frames->each[0]))) exit(-1);
    if(!(fs=read_frames_file(frames_file,0,tc_frames->eachi[1],tc_frames->each[1]))) exit(-1);
    }

/*printf("fsreg->nframes=%d fs->nframes=%d\n",fsreg->nframes,fs->nframes);
for(i=0;i<fs->nframes;i++) printf("%d %d\n",fsreg->frames[i],fs->frames[i]);*/



for(i=1;i<fs->nlines;i++) {
    if(fs->frames_per_line[i] != fs->frames_per_line[0]) {
        printf("Error: All lines in %s must have the same number of frames.\n",frames_file);
        printf("Error: Discrepancy found at line %s.\n",i+1);
        exit(-1); 
        }
    }

if(lcregwithvox) {
    for(i=1;i<fsreg->nlines;i++) {
        if(fsreg->frames_per_line[i] != fsreg->frames_per_line[0]) {
            printf("Error: All lines in %s must have the same number of frames.\n",frames_file);
            printf("Error: Discrepancy found at line %s.\n",i+1);
            exit(-1);
            }
        }
    }

if(!(sindex=malloc(sizeof*sindex*fs->nlines))) {
    printf("Error: Unable to malloc sindex\n");
    exit(-1);
    }
if(!lcregwithvox) {
    for(i=0;i<fs->nlines;i++) sindex[i] = 0;
    }
else {
    if(!(sindexreg=malloc(sizeof*sindexreg*fs->nlines))) {
        printf("Error: Unable to malloc sindexreg\n");
        exit(-1);
        }
    if(!logregwts_file) {
        for(i=0;i<fs->nlines;i++) {
            sindexreg[i] = tc_frames->eachi[0];
            sindex[i] = tc_frames->eachi[1];
            }
        }
    else {
        for(i=0;i<fs->nlines;i++) {
            sindexreg[i] = 0;
            sindex[i] = 1;
            }
        }
    } 

if(!(dp=dim_param(num_tc_files,tc_files->files,SunOS_Linux))) exit(-1);
swapbytes=shouldiswap(SunOS_Linux,dp->bigendian[0]);
if(!xform_file) atlas = get_atlas(dp->vol);
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(!xform_file) ap->vol = dp->vol;
if(!atlas) {
    ap->xdim = dp->xdim;
    ap->ydim = dp->ydim;
    ap->zdim = dp->zdim;
    ap->voxel_size[0] = dp->dxdy;
    ap->voxel_size[1] = dp->dxdy;
    ap->voxel_size[2] = dp->dz;
    }

if(num_region_files) {
    if(!check_dimensions(num_region_files,region_files->files,ap->vol)) exit(-1);
    if(!(reg=malloc(sizeof*reg*num_region_files))) {
        printf("Error: Unable to malloc reg\n");
        exit(-1);
        }
    for(i=0;i<num_region_files;i++) if(!(reg[i]=extract_regions(region_files->files[i],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL)))
        exit(-1);
    if(!num_regions) for(m=0;m<num_region_files;m++) num_regions += reg[m]->nregions;
    if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);
    nreg = num_regions;
    printf("rbf->indices="); for(i=0;i<rbf->nvoxels;i++) printf("%d ",rbf->indices[i]); printf("\n");
    }
if(!num_region_files || lcregwithvox) {
    if(!(ms=get_mask_struct(mask_file,ap->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
    nvox = ms->lenbrain;
    }
printf("num_regions=%d ap->vol=%d fs->nlines=%d nreg=%d nvox=%d\n",num_regions,ap->vol,fs->nlines,nreg,nvox);

if(!(temp_float=malloc(sizeof*temp_float*ap->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*ap->vol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(lcscratch) if(!(scratchdir=make_scratchdir())) exit(-1);
/*printf("scratchdir=%s\n",scratchdir);*/

/*X
  --------------------
  weightsfile1 volume1
  --------------------
  weightsfile2 volume1
  --------------------
  weightsfile3 volume1
  --------------------
  ...
  --------------------
  weightsfile1 volume2
  --------------------
  weightsfile2 volume2
  --------------------
  weightsfile3 volume2
  --------------------
  ...
  --------------------*/

if(!(fbf=find_regions_by_file(num_tc_files,fs->nframes,dp->tdim,fs->frames))) exit(-1);
if(!(sn=create_scratchnames(tc_files,fbf,scratchdir,"_vox"))) exit(-1);
if(lcscratch) {
    if(!(create_scratchfiles(tc_files,fbf,dp,sn,SunOS_Linux,xform_file,ap,num_wfiles,!lcregwithvox?rbf:(Regions_By_File*)NULL,
        nvox,ms,wfiles,fs->nframes,0.))) exit(-1);
    }
if(lcregwithvox) {
    if(!(fbfreg=find_regions_by_file(num_tc_files,fsreg->nframes,dp->tdim,fsreg->frames))) exit(-1);
    if(!(snreg=create_scratchnames(tc_files,fbfreg,scratchdir,"_reg"))) exit(-1);
    if(lcscratch) {
        if(!(create_scratchfiles(tc_files,fbfreg,dp,snreg,SunOS_Linux,xform_file,ap,num_wfiles,rbf,nreg,ms,wfiles,
            fsreg->nframes,0.))) exit(-1);
        }
    }





sx = !logregwts_file ? tc_frames->each[0] : 1; 
sy = tc_frames->each[1];

#if 0
printf("ntc_frames=%f\n",ntc_frames);
if(ntc_frames==2) {
    sx = !logregwts_file ? tc_frames->each[0] : 1; 
    sy = tc_frames->each[1];
    }
else {
    sx = nweights; 
    sy = tc_frames->each[0];
    }
#endif




if(!(chisquare=malloc(sizeof*chisquare*ms->lenbrain))) {
    printf("Error: Unable to malloc chisquare\n");
    exit(-1);
    }
if(!num_region_files || lcregwithvox) {
    if(!(ifh=init_ifh(4,ap->xdim,ap->ydim,ap->zdim,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],!SunOS_Linux?1:0))) exit(-1);
    }
if(!(s=malloc(sizeof*s))) {
    printf("Error: Unable to malloc s\n");
    exit(-1);
    }
if(!(infi=malloc(sizeof*infi*nvox))) {
    printf("Error: Unable to malloc infi\n");
    exit(-1);
    }

if(!(val=malloc(sizeof*val*nvox))) {
    printf("Error: Unable to malloc val\n");
    exit(-1);
    }

if(sx==sy) lccos=1;
if(logregwts_file) {
    if(fsreg->frames_per_line[0]==sy) lccos=1;
    if(!(wts=malloc(sizeof*wts*num_regions*fsreg->frames_per_line[0]))) {
        printf("Error: Unable to malloc wts\n");
        exit(-1);
        }
    if(!read_logregwts_file(logregwts_file,fsreg->frames_per_line[0],wts)) exit(-1);
    printf("wts = \n");
    for(k=i=0;i<num_regions;i++) {
        for(j=0;j<fsreg->frames_per_line[0];j++,k++) printf("%f ",wts[k]);
        printf("\n");
        }
    }
if(lccos) {
    if(!(costheta=malloc(sizeof*costheta*ms->lenbrain))) {
        printf("Error: Unable to malloc costheta\n");
        exit(-1);
        }
    }
lengthX = sx + sy;
toomanyreg = fs->nlines * sx;



toomanyvox = fs->nlines * (!lcregwithvox ? lengthX : sy);
N = (double)(fs->nlines-1);
minusv = -N + .5*(sx+sy+1.);
printf("fs->nlines=%d N=%f minusv=%f\n",fs->nlines,N,minusv);
startx = !lcregwithvox ? 0 : sx;

#if 0
N = (double)(fs->nlines-1);
minusv = -N + .5*(sx+sy+1.);
printf("fs->nlines=%d N=%f minusv=%f\n",fs->nlines,N,minusv);
toomanyvox = fs->nlines * ((!lcregwithvox&&!nweights) ? lengthX : sy);
startx = (!lcregwithvox&&!nweights) ? 0 : sx;
#endif





printf("startx=%d sx=%d sy=%d\n\n",startx,sx,sy);
X = m_get(lengthX,fs->nlines);
if(logregwts_file) Xreg = m_get(fsreg->frames_per_line[0],fs->nlines);
if(!(mean=malloc(sizeof*mean*lengthX))) {
    printf("Error: Unable to malloc mean\n");
    exit(-1);
    }
if(!(td=malloc(sizeof*td*lengthX))) {
    printf("Error: Unable to malloc td\n");
    exit(-1);
    }

if(!(df=malloc(sizeof*df*ap->vol))) {
    printf("Error: Unable to malloc df\n");
    exit(-1);
    }
for(i=0;i<ap->vol;i++) df[i] = (double)(sx*sy);
if(!(z=malloc(sizeof*z*ap->vol))) {
    printf("Error: Unable to malloc z\n");
    exit(-1);
    }

cov = m_get(lengthX,lengthX);
Cxx = m_get(sx,sx);
Cxxinv = m_get(sx,sx);
Cxy = m_get(sx,sy);
Cyy = m_get(sy,sy);
Cyyinv = m_get(sy,sy);
CxxinvCxy = m_get(sx,sy);
CyyinvCyx = m_get(sy,sx);
T = m_get(sx,sx);
Q = m_get(sx,sx);
evals_re = v_get(sx);
evals_im = v_get(sx);
X_re = m_get(sx,sx);
X_im = m_get(sx,sx);
Wy = m_get(sy,sx);
Tcopy = m_get(sx,sx);

for(flag=pp=p=0;p<num_wfiles;p++,pp+=nvox*sx) {
    for(sum=0.,ll=once=q=0;q<nreg;q++,ll++) {
        if(lcregwithvox) {
            sum += rbf->nvoxels_region[q]-1;
            assignX(num_tc_files,fbfreg->num_regions_by_file,snreg,nreg,num_wfiles,0,q,p,sindexreg,fsreg,logregwts_file?Xreg:X,s);
            if(logregwts_file) {
                for(k=q*fsreg->frames_per_line[0],m=0;m<fsreg->nlines;m++) {
                    for(X->me[0][m]=0.,i=0;i<fsreg->frames_per_line[0];i++) X->me[0][m] += wts[k+i]*Xreg->me[i][m];
                    }

                printf("here0 wts=");
                for(k=q*fsreg->frames_per_line[0],i=0;i<fsreg->frames_per_line[0];i++) printf("%f ",wts[k+i]); 
                printf("\n");

                for(m=0;m<fsreg->nlines;m++) {
                    printf("Xreg->me[][%d]= ",m);
                    for(i=0;i<fsreg->frames_per_line[0];i++,j++) printf("%f ",Xreg->me[i][m]);
                    printf("  X->me[0][%d]= %f\n",m,X->me[0][m]);
                    }
                }
            if(s->uns_count || s->small==toomanyreg) {
                printf("Error: q=%d Problem with this region. s->uns_count=%d s->small=%d\n",q,s->uns_count,s->small);
                exit(-1);
                }
            else {
                for(k=0;k<sx;k++) mean[k] = 0.;
                for(i=0;i<fs->nlines;i++) for(k=0;k<sx;k++) mean[k] += X->me[k][i];
                for(k=0;k<sx;k++) mean[k] /= fs->nlines;
                }
            }
        #if 0
        else if(nweights) {
            for(i=0;i<fs->nlines;i++) for(k=0;k<sx;k++) X->me[k][i] = weights[k];
            for(k=0;k<sx;k++) mean[k] += weights[k];
            }
        #endif

        for(min=10000.,max=0.,nval=ninf=j=0;j<nvox;j++) {
            if(!(j%modulo)) fprintf(stdout,"Processing voxel %d\n",j);
            if(lcregwithvox) {
                /*printf("rbf->indices[%d]=%d ms->brnidx[%d]=%d\n",ll,rbf->indices[ll],j,ms->brnidx[j]);*/
                if(rbf->indices[ll]==ms->brnidx[j]) {
                    /*printf("HERE rbf->indices[%d]=%d ms->brnidx[%d]=%d\n",ll,rbf->indices[ll],j,ms->brnidx[j]);*/

                    col_row_slice(1,&rbf->indices[ll],&dcol,&drow,&dslice,ap);
                    get_atlas_coor(1,&dcol,&drow,&dslice,(double)ap->zdim,ap->center,ap->mmppix,atlas_coor);
                    /*printf("        %d %d %d\n",atlas_coor[0],atlas_coor[1],atlas_coor[2]);*/
                    printf("seed voxel %d %d %d\n",atlas_coor[0],atlas_coor[1],atlas_coor[2]);

                    chisquare[j] = (double)UNSAMPLED_VOXEL;         
                    /*for(i=0;i<sx;i++) cc[j+i*nvox] = (float)UNSAMPLED_VOXEL;*/
                    if(lccos) costheta[j] = (double)UNSAMPLED_VOXEL;
                    if(ll!=sum) ll++; 
                    continue;
                    }
                } 

            assignX(num_tc_files,fbf->num_regions_by_file,sn,nvox,num_wfiles,0,j,p,sindex,fs,X,s); 
            #if 0
            printf("X\n");
            for(m=0;m<fs->nlines;m++) {
                for(i=0;i<lengthX;i++) printf("%g ",X->me[i][m]);
                printf("\n");
                }
            printf("j=%d p=%d s->uns_count=%d s->small=%d nvox=%d\n",j,p,s->uns_count,s->small,nvox);
            #endif

            if(s->uns_count || s->small==toomanyvox) {
                /*for(i=0;i<sx;i++) cc[j+i*nvox] = (float)UNSAMPLED_VOXEL;*/
                chisquare[j] = (double)UNSAMPLED_VOXEL;
                if(lccos) costheta[j] = (double)UNSAMPLED_VOXEL;
                }
            else { 
                /*printf("HERE\n");*/
                #if 0
                printf("j=%d X\n",j);
                for(m=0;m<fs->nlines;m++) {
                    for(i=0;i<lengthX;i++) printf("%g ",X[i][m]);
                    printf("\n");
                    }
                printf("j=%d p=%d s->uns_count=%d s->small=%d nvox=%d\n",j,p,s->uns_count,s->small,nvox);
                #endif

                sprintf(Cxxstr,"Cxx voxel=%d",j);
                sprintf(Cyystr,"Cyy voxel=%d",j);

                for(k=startx;k<lengthX;k++) mean[k] = 0.;
                for(i=0;i<fs->nlines;i++) for(k=startx;k<lengthX;k++) mean[k] += X->me[k][i];
                for(k=startx;k<lengthX;k++) mean[k] /= fs->nlines;
                for(i=0;i<lengthX;i++) for(k=0;k<lengthX;k++) cov->me[i][k] = 0.;
                for(i=0;i<fs->nlines;i++) {
                    for(k=0;k<lengthX;k++) td[k] = X->me[k][i] - mean[k];
                    for(k=0;k<lengthX;k++) for(l=k;l<=lengthX;l++) cov->me[k][l] += td[k]*td[l];
                    }
                for(i=0;i<lengthX;i++) for(k=i;k<lengthX;k++) cov->me[i][k] /= N;

                if(!lcregwithvox || !once) {
                    /*printf("Computing Cxxinv\n");*/
                    for(i=0;i<sx;i++) for(k=i;k<sx;k++) Cxx->me[i][k] = Cxx->me[k][i] = cov->me[i][k];
                    m_inverse(Cxx,Cxxinv);
                    }
                for(i=0;i<sx;i++) for(l=sx,k=0;k<sy;k++,l++) Cxy->me[i][k] = cov->me[i][l];
                for(l=sx,i=0;i<sy;i++,l++) for(m=l,k=i;k<sy;k++,m++) Cyy->me[i][k] = Cyy->me[k][i] = cov->me[l][m];
                m_inverse(Cyy,Cyyinv);
                m_mlt(Cxxinv,Cxy,CxxinvCxy);
                mmtr_mlt(Cyyinv,Cxy,CyyinvCyx);
                m_mlt(CxxinvCxy,CyyinvCyx,T);
                m_copy(T,Tcopy);

                #if 0
                /*if(j==19367) {*/
                    printf("mean="); for(k=0;k<lengthX;k++) printf("%f ",mean[k]); printf("\n");
                    printf("cov\n");
                    for(i=0;i<lengthX;i++) {
                        for(k=0;k<lengthX;k++) printf("%g\t",cov->me[i][k]);
                        printf("\n");
                        }
                    printf("Cxx\n");
                    for(i=0;i<sx;i++) {
                        for(k=0;k<sx;k++) printf("%g ",Cxx->me[i][k]);
                        printf("\n");
                        }
                    printf("Cxy\n");
                    for(i=0;i<sx;i++) {
                        for(k=0;k<sy;k++) printf("%g ",Cxy->me[i][k]);
                        printf("\n");
                        }
                    printf("Cyy\n");
                    for(i=0;i<sy;i++) {
                        for(k=0;k<sy;k++) printf("%g ",Cyy->me[i][k]);
                        printf("\n");
                        }
                    printf("Cyyinv\n");
                    for(i=0;i<sy;i++) {
                        for(k=0;k<sy;k++) printf("%g ",Cyyinv->me[i][k]);
                        printf("\n");
                        }
                    printf("T\n");
                    for(i=0;i<sx;i++) {
                        for(k=0;k<sx;k++) printf("%f\t",T->me[i][k]);
                        printf("\n");
                        }
                    /*}*/
                #endif

                m_zero(Q);
                schur(T,Q);
                schur_evals(T,evals_re,evals_im);
                schur_vecs(T,Q,X_re,X_im);
                for(maxev=-10.,chisquare[j]=1.,maxevi=i=0;i<sx;i++) {
                    /*cc[j+i*nvox] = (float)sqrt(evals_re->ve[i]);*/
                    chisquare[j] *= (1.-evals_re->ve[i]);
                    if(evals_re->ve[i]>maxev) {
                        maxev = evals_re->ve[i];
                        maxevi = i;
                        }
                    }
                chisquare[j] = minusv*log(chisquare[j]);


                if(lccos) {
                    m_mlt(CyyinvCyx,X_re,Wy);
                    if(!logregwts_file) {
                        for(costheta[j]=mag1=mag2=0.,i=0;i<sx;i++) { /*mag1 is unnecessary bc X_re->me already normalized*/
                            costheta[j] += X_re->me[i][maxevi]*Wy->me[i][maxevi];
                            mag1 += X_re->me[i][maxevi]*X_re->me[i][maxevi];
                            mag2 += Wy->me[i][maxevi]*Wy->me[i][maxevi];
                            }
                        }
                    else {
                        for(k=q*sy,costheta[j]=mag1=mag2=0.,i=0;i<sy;i++) {
                            costheta[j] += wts[k+i]*Wy->me[i][maxevi];
                            mag1 += wts[k+i]*wts[k+i];
                            mag2 += Wy->me[i][maxevi]*Wy->me[i][maxevi];
                            }
                        /*printf("here1 wts="); for(k=q*sy,i=0;i<sy;i++) printf("%f ",wts[k+i]); printf("\n");*/
                        }



                    /*costheta[j] /= sqrt(mag1)*sqrt(mag2);*/

                    for(sumprod=0.,i=0;i<fs->nlines;i++) {
                        for(ip1=0.,k=0;k<sx;k++) ip1 += X->me[k][i]*X_re->me[k][maxevi];
                        for(ip2=0.,l=sx,k=0;k<sy;k++,l++) ip2 += X->me[l][i]*Wy->me[k][maxevi]; 
                        sumprod += ip1*ip2;
                        }
                    /*printf("before sumprod=%f\n",sumprod);*/
                    sumprod = sumprod < 0. ? -1. : 1.;
                    /*printf("after sumprod=%f\n",sumprod);*/
                    costheta[j] /= sumprod*sqrt(mag1)*sqrt(mag2);


                    /*printf("costheta[%d]=%f\n",j,costheta[j]);*/
                    }

                #if 0
                /*if(j==19367) {*/
                    printf("eigenvalues\n"); for(i=0;i<sx;i++) printf("%f+i%f\n",evals_re->ve[i],evals_im->ve[i]);
                    printf("eigenvectors (each column)\n");
                    for(i=0;i<sx;i++) {
                        for(k=0;k<sx;k++) printf("%f+i%.2f\t",X_re->me[i][k],X_im->me[i][k]);
                        printf("\n");
                        } 
                    printf("Wy\n"); 
                    for(i=0;i<sy;i++) {
                        for(k=0;k<sx;k++) printf("%f\t",Wy->me[i][k]); 
                        printf("\n");
                        } 
                    /*if(++flag==1) exit(-1);*/
                    /*}*/
                #endif

                if(X_re->error) {
                    printf("voxel %d\n",j); 
                    printf("mean="); for(k=0;k<lengthX;k++) printf("%f ",mean[k]); printf("\n");
                    printf("cov\n");
                    for(i=0;i<lengthX;i++) {
                        for(k=0;k<lengthX;k++) printf("%g\t",cov->me[i][k]);
                        printf("\n");
                        }
                    printf("Cxx\n");
                    for(i=0;i<sx;i++) {
                        for(k=0;k<sx;k++) printf("%g ",Cxx->me[i][k]);
                        printf("\n");
                        }
                    printf("Cxy\n");
                    for(i=0;i<sx;i++) {
                        for(k=0;k<sy;k++) printf("%g ",Cxy->me[i][k]);
                        printf("\n");
                        }
                    printf("Cyy\n");
                    for(i=0;i<sy;i++) {
                        for(k=0;k<sy;k++) printf("%g ",Cyy->me[i][k]);
                        printf("\n");
                        }
                    printf("Cyyinv\n");
                    for(i=0;i<sy;i++) {
                        for(k=0;k<sy;k++) printf("%g ",Cyyinv->me[i][k]);
                        printf("\n");
                        }
                    printf("Tcopy\n");
                    for(i=0;i<sx;i++) {
                        for(k=0;k<sx;k++) printf("%f\t",Tcopy->me[i][k]);
                        printf("\n");
                        }
                    printf("eigenvalues\n"); for(i=0;i<sx;i++) printf("%f+i%f\n",evals_re->ve[i],evals_im->ve[i]);
                    printf("eigenvectors (each column)\n");
                    for(i=0;i<sx;i++) {
                        for(k=0;k<sx;k++) printf("%f+i%.2f\t",X_re->me[i][k],X_im->me[i][k]);
                        printf("\n");
                        }
                    printf("Wy\n");
                    for(i=0;i<sy;i++) {
                        for(k=0;k<sx;k++) printf("%f\t",Wy->me[i][k]);
                        printf("\n");
                        }
                    printf("costheta=%f\n\n",costheta[j]);
                    X_re->error=0;
                    }

              /*#ifndef LINUX*/
              #ifdef __sun__

                if(IsNANorINF(chisquare[j])) {
              #else
                if(isnan(chisquare[j]) || isinf(chisquare[j])) {
              #endif
                    infi[ninf++] = j;
                    col_row_slice(1,&rbf->indices[ll],&dcol,&drow,&dslice,ap);
                    col_row_slice(1,&ms->brnidx[j],&dcol,&drow,&dslice,ap);
                    get_atlas_coor(1,&dcol,&drow,&dslice,(double)ap->zdim,ap->center,ap->mmppix,atlas_coor);
                    printf("ERROR: chisquare[%d]=%f %d %d %d\n",j,chisquare[j],atlas_coor[0],atlas_coor[1],atlas_coor[2]);
                    }
                else {
                    val[nval++] = j;
                    }
                once++;
                }
            /*printf("\n");*/
            } /*for(j=0;j<nvox;j++)*/ 

        printf("ninf=%d nval=%d\n",ninf,nval);

        for(j=0;j<ap->vol;j++) temp_float[j] = 0.;
        for(j=0;j<ninf;j++) chisquare[infi[j]] = max; 
        for(j=0;j<nvox;j++) temp_float[ms->brnidx[j]] = (float)chisquare[j];
        sprintf(filename,"%s_chisquare%s.4dfp.img",root->files[q],ap->str);
        if(!writestack(filename,temp_float,sizeof(float),(size_t)ap->vol,swapbytes)) exit(-1);
        for(j=0;j<nval;j++) temp_double[j] = chisquare[val[j]]; 
        min_and_max_init(&ifh->global_min,&ifh->global_max);
        min_and_max_doublestack(temp_double,nval,&ifh->global_min,&ifh->global_max);
        ifh->dim4 = 1;
        ifh->dof_condition = (float)(sx*sy);
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
        printf("Chisquare statistic written to %s\n",filename);



        /*START3*/
        for(j=0;j<nval;j++) temp_double[j] = chisquare[val[j]]; 
        if(!x2_to_z(temp_double,z,nval,df,ms->brnidx,ap)) exit(-1);


int x2_to_z(double *x,double *z,int vol,double *df,int *brnidx,Atlas_Param *ap);



        for(j=0;j<nvox;j++) temp_double[j] = chisquare[j];
        for(j=0;j<nval;j++) temp_double[val[j]] = z[j];
        for(j=0;j<nvox;j++) temp_float[ms->brnidx[j]] = (float)temp_double[j];
        sprintf(filename,"%s_zstat%s.4dfp.img",root->files[q],ap->str);
        if(!writestack(filename,temp_float,sizeof(float),(size_t)ap->vol,swapbytes)) exit(-1);
        for(j=0;j<nval;j++) temp_double[j] = z[j];
        min_and_max_init(&ifh->global_min,&ifh->global_max);
        min_and_max_doublestack(temp_double,nval,&ifh->global_min,&ifh->global_max);
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
        printf("Z statistic written to %s\n",filename);




        if(lccos) {
            for(j=0;j<nvox;j++) temp_float[ms->brnidx[j]] = (float)costheta[j];
            sprintf(filename,"%s_costheta%s.4dfp.img",root->files[q],ap->str);
            if(!writestack(filename,temp_float,sizeof(float),(size_t)ap->vol,swapbytes)) exit(-1);

            #if 0
            min_and_max_init(&ifh->global_min,&ifh->global_max);
            min_and_max_doublestack(costheta,nvox,&ifh->global_min,&ifh->global_max);
            #endif

            for(j=0;j<nval;j++) temp_double[j] = costheta[val[j]]; 
            min_and_max_init(&ifh->global_min,&ifh->global_max);
            min_and_max_doublestack(temp_double,nval,&ifh->global_min,&ifh->global_max);


            ifh->dim4 = 1;
            ifh->dof_condition = 0.;
            if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
            printf("Cosine(theta) written to %s\n",filename);
            }

        } /*for(q=0;q<nreg;q++)*/ 
    } /*for(pp=p=0;p<num_wfiles;p++,pp+=nvox*sx)*/ 

if(lccleanup) {
    sprintf(filename,"rm -r %s",scratchdir);
    if(system(filename) == -1) printf("Error: unable to %s\n",filename);
    }
}

int read_logregwts_file(char *logregwts_file,int nwts,double *wts)
{
    char line[MAXNAME],write_back[MAXNAME],dummy,*strptr,*strptr2;
    int npts,startlookingforwts,di,nstrings;
    double *dptr;
    FILE *fp;

    npts = nwts+1;
    dptr = wts;
    if(!(fp=fopen_sub(logregwts_file,"r"))) return 0;
    for(startlookingforwts=0;fgets(line,sizeof(line),fp);) {
        if(strstr(line,"REGION")) startlookingforwts=1;
        if(startlookingforwts) {
            if(strstr(line,"weights")) {
                printf("line=%s\n",line);
                if((nstrings=count_strings_new(line,write_back,' ',&dummy)-1) != npts) {
                    printf("Error: nstrings=%d line=%sEND\nError: Line must have %d numbers.\n",nstrings,line,npts);
                    return 0;
                    }
                printf("write_back=%s\n",write_back);
                strptr=grab_string_new(write_back,line,&di);
                printf("strptr=%s\n",strptr);
                strptr2=grab_string_new(strptr,line,&di);
                printf("strptr2=%s\n",strptr);
                strings_to_double(strptr2,dptr,nwts);
                dptr += nwts;
                startlookingforwts=0;
                } 
            }
        }
    return 1;
}

/*START140729*/
void assignX(int num_tc_files,int *num_regions_by_file,Files_Struct_new *sn,int num_regions,int num_wfiles,int goose,int j,
    int p,int *sindex,FS *fs,MAT *X,Scratch *s)
{
    char *strptr;
    int i,k,l,m,q,uns_count,small,index;
    Memory_Map *mm;

    if(goose==1) for(i=0;i<fs->nlines;i++) X->me[0][i]=1.;
    for(strptr=sn->files,q=0,small=uns_count=l=m=0;m<num_tc_files && !uns_count;m++) {
        if(num_regions_by_file[m]) {
            if(!(mm=map_disk_double(strptr,num_regions*num_regions_by_file[m]*num_wfiles,0))) exit(-1);
            /*if(goose==1) for(i=0;i<fs->nlines;i++) X->me[0][i]=1.;*/
            for(k=goose,i=0;i<num_regions_by_file[m] && !uns_count;i++) {
                /*printf("sindex[%d]=%d k=%d X[%d][%d]=%g\n",q,sindex[q],k,sindex[q]+k,q,
                    mm->dptr[num_regions*(p+num_wfiles*i)+j]);*/
                index = sindex[q]+k;
                if((X->me[index][q]=mm->dptr[num_regions*(p+num_wfiles*i)+j]) == (double)UNSAMPLED_VOXEL) uns_count++;
                if(fabs(X->me[index][q]) < (double)UNSAMPLED_VOXEL) small++;
                if(++k == (fs->frames_per_line[q]+goose)) {
                    k = goose;
                    q++;
                    }
                }
            if(!unmap_disk(mm)) exit(-1);
            strptr += sn->strlen_files[l++];
            }
        }
    s->uns_count = uns_count;
    s->small = small;
    /*printf("here101\n");*/
}
