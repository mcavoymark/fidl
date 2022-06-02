/* Copyright 12/20/05 Washington University.  All Rights Reserved.
   fidl_pc.c  $Revision: 1.10 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <nrutil.h>

/*#ifndef LINUX*/
#ifdef __sun__

    #include <nan.h>
#endif

/*START150422*/
#include "read_frames_file.h"

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_pc.c,v 1.10 2015/04/22 23:36:14 mcavoy Exp $";

main(int argc,char **argv)
{
char *strptr,*xform_file=NULL,*mask_file=NULL,*scratchdir,filename[MAXNAME],XTXstr[MAXNAME];

int i,j,k,q,l,m,n,ll,num_tc_files=0,num_cov_files=0,num_region_files=0,nseeds=0,*seeds,ncorreg=0,*correg,num_exclude_frames=0,
    *exclude_frames,atlas=222,lcscratch=1,lccleanup=0,SunOS_Linux,nfoi,*foi,*sindexseeds,*sindexcor,*coi,ncor,goose,lengthX,toomany,
    ninfnan,*infnani,index,small,uns_count,start,sum,modulo=100,swapbytes;
 
float *temp_float,fwhm=0.;

double *tstat,*r,**X,**XTX,**XTXm1,**Y,**XTY,**B,cond,*dcol,*drow,*dslice,df,yty,btxty,var,*atlascoor;

FILE *fp;
Regions **reg,**cov;
Regions_By_File *fbf,*seedsbf,*covbf,*corregbf; 
Atlas_Param *ap;
Dim_Param *dp;
Memory_Map *mm; 
Interfile_header *ifh;
Mask_Struct *ms;
FS *fs;
Files_Struct *tc_files,*cov_files,*region_files,*wfiles=NULL;
Files_Struct_new *snseeds,*sncor,*sncov;
Scratch *s;

print_version_number(rcsid,stdout);
if(argc < 9) {
    fprintf(stderr,"        -tc_files:            imgs or a single conc\n");
    fprintf(stderr,"        -cov_files:           Partial out the effect of this stuff.\n");
    fprintf(stderr,"        -region_file:         fidl region files\n");
    fprintf(stderr,"        -seeds:               Seed regions. First region is 1.\n");
    fprintf(stderr,"        -correg:              Correlate seed regions with these regions. First region is 1.\n");
    fprintf(stderr,"        -exclude_frames:      Frames to excluded from the analysis. First frames is one.\n");
    fprintf(stderr,"        -xform_file:          t4 file defining the transform to atlas space.\n");
    fprintf(stderr,"        -atlas:               111, 222, or 333. Default is 222.\n");
    fprintf(stderr,"        -mask:                Only voxels in the mask are analyzed.\n");
    fprintf(stderr,"                              Seeds and cov_files are never smoothed.\n");
    fprintf(stderr,"        -scratchdir:          Location of precomputed scratchfiles. Include the backslash at the end.\n");
    fprintf(stderr,"        -clean_up             Delete the scratch files and directory.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-tc_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_tc_files;
        strptr = strrchr(argv[i+1],'.');
        if(!strcmp(strptr,".conc")) {
            if(!(tc_files=read_conc(argv[i+1]))) return 0;
            }
        else if(!strcmp(strptr,".img")){
            if(!(tc_files=get_files(num_tc_files,&argv[i+1]))) exit(-1);
            }
        else {
            printf("Error: -tc_files not conc or img. Abort!\n");
            exit(-1);
            }
        i += num_tc_files;
        }
    if(!strcmp(argv[i],"-cov_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_cov_files;
        if(!(cov_files=get_files(num_cov_files,&argv[i+1]))) exit(-1);
        i += num_cov_files;
        }
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-seeds") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nseeds;
        if(!(seeds=malloc(sizeof*seeds*nseeds))) {
            printf("Error: Unable to malloc seeds\n");
            exit(-1);
            }
        for(j=0;j<nseeds;j++) seeds[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-correg") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ncorreg;
        if(!(correg=malloc(sizeof*correg*ncorreg))) {
            printf("Error: Unable to malloc correg\n");
            exit(-1);
            }
        for(j=0;j<ncorreg;j++) correg[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-exclude_frames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_exclude_frames;
        if(!(exclude_frames=malloc(sizeof*exclude_frames*num_exclude_frames))) {
            printf("Error: Unable to malloc exclude_frames\n");
            exit(-1);
            }
        for(j=0;j<num_exclude_frames;j++) exclude_frames[j] = atoi(argv[++i]);
        }
    if(!strcmp(argv[i],"-xform_file") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        xform_file = argv[++i];
    if(!strcmp(argv[i],"-atlas") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        atlas = atoi(argv[++i]);
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
        scratchdir = argv[++i];
        lcscratch = 0;
        }
    if(!strcmp(argv[i],"-clean_up"))
        lccleanup = 1;
    }
if(!num_tc_files) {
    printf("Error: No -tc_files. Abort!\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) return 0;
printf("fwhm=%f\n",fwhm);

if(!(dp=dim_param(tc_files->nfiles,tc_files->files,SunOS_Linux))) exit(-1);
swapbytes=shouldiswap(SunOS_Linux,dp->bigendian[0]);

nfoi = dp->tdim_total - num_exclude_frames;
if(!(foi=malloc(sizeof*foi*nfoi))) {
    printf("Error: Unable to malloc foi\n");
    exit(-1);
    }
if(!num_exclude_frames) {
    for(k=0,i=1;i<=dp->tdim_total;i++,k++) foi[k] = i;
    }
else { 
    for(k=j=0,i=1;i<=dp->tdim_total;i++) {
        if(exclude_frames[j]==i) {
            if(j<(num_exclude_frames-1)) j++;
            }
        else { 
            foi[k++] = i;
            }
        }
    }
if(!(fs=make_FS(nfoi,foi))) exit(-1);
printf("fs->nframes=%d fs->frames=",fs->nframes); for(i=0;i<fs->nframes;i++) printf("%d ",fs->frames[i]); printf("\n");

if(!(sindexseeds=malloc(sizeof*sindexseeds*fs->nlines))) {
    printf("Error: Unable to malloc sindexseeds\n");
    exit(-1);
    }
if(!(sindexcor=malloc(sizeof*sindexcor*fs->nlines))) {
    printf("Error: Unable to malloc sindexcor\n");
    exit(-1);
    }
for(i=0;i<fs->nlines;i++) sindexseeds[i] = sindexcor[i] = 0; 

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

if(!check_dimensions(num_region_files,region_files->files,ap->vol)) exit(-1);
if(!(reg=malloc(sizeof*reg*num_region_files))) {
    printf("Error: Unable to malloc reg\n");
    exit(-1);
    }
for(i=0;i<num_region_files;i++) if(!(reg[i]=extract_regions(region_files->files[i],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) 
    exit(-1);
if(!(seedsbf=find_regions_by_file_cover(num_region_files,nseeds,reg,seeds))) exit(-1);
if(num_cov_files) {
    if(!check_dimensions(num_cov_files,cov_files->files,ap->vol)) exit(-1);
    if(!(coi=malloc(sizeof*coi*num_cov_files))) {
        printf("Error: Unable to malloc coi\n");
        exit(-1);
        }
    for(i=0;i<num_cov_files;i++) coi[i] = i;
    if(!(cov=malloc(sizeof*cov*num_cov_files))) {
        printf("Error: Unable to malloc cov\n");
        exit(-1);
        }
    for(i=0;i<num_cov_files;i++) if(!(cov[i]=extract_regions(cov_files->files[i],2,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) 
        exit(-1);
    if(!(covbf=find_regions_by_file_cover(num_cov_files,num_cov_files,cov,coi))) exit(-1);
    printf("covbf->nvoxels=%d\n",covbf->nvoxels);
    }
if(ncorreg) if(!(corregbf=find_regions_by_file_cover(num_region_files,ncorreg,reg,correg))) exit(-1);

ncor = ncorreg;
if(!ncorreg) {
    if(!(ms=get_mask_struct(mask_file,ap->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
    ncor = ms->lenbrain;
    }
printf("nseeds=%d ap->vol=%d fs->nlines=%d ncor=%d\n",nseeds,ap->vol,fs->nlines,ncor);

if(!(fbf=find_regions_by_file(tc_files->nfiles,fs->nframes,dp->tdim,fs->frames))) exit(-1);
if(lcscratch) if(!(scratchdir=make_scratchdir())) exit(-1);
if(!(snseeds=create_scratchnames(tc_files,fbf,scratchdir,"_seeds"))) exit(-1);
if(!(sncor=create_scratchnames(tc_files,fbf,scratchdir,"_cor"))) exit(-1);
if(num_cov_files) if(!(sncov=create_scratchnames(tc_files,fbf,scratchdir,"_cov"))) exit(-1);
if(lcscratch) {
    if(!(create_scratchfiles(tc_files,fbf,dp,snseeds,SunOS_Linux,xform_file,ap,1,seedsbf,nseeds,ms,wfiles,fs->nframes,0.))) 
        exit(-1);
    if(!(create_scratchfiles(tc_files,fbf,dp,sncor,SunOS_Linux,xform_file,ap,1,ncorreg?corregbf:(Regions_By_File*)NULL,
        ncor,ms,wfiles,fs->nframes,ncorreg?0.:fwhm))) exit(-1);
    if(num_cov_files) {
        if(!(create_scratchfiles(tc_files,fbf,dp,sncov,SunOS_Linux,xform_file,ap,1,covbf,num_cov_files,ms,wfiles,
            fs->nframes,0.))) exit(-1);
        }
    }

goose = 2;
lengthX = goose + num_cov_files;
toomany = fs->nlines * lengthX;
df = (double)(fs->nlines-lengthX); 
if(!ncorreg) if(!(ifh=init_ifh(4,ap->xdim,ap->ydim,ap->zdim,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],!SunOS_Linux?1:0))) exit(-1);
if(!(temp_float=malloc(sizeof*temp_float*ap->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<ap->vol;i++) temp_float[i] = 0.;
if(!(tstat=malloc(sizeof*tstat*ncor))) {
    printf("Error: Unable to malloc tstat\n");
    exit(-1);
    }
if(!(r=malloc(sizeof*r*ncor))) {
    printf("Error: Unable to malloc r\n");
    exit(-1);
    }
if(!(dcol=malloc(sizeof*dcol*seedsbf->nvoxels))) {
    printf("Error: Unable to malloc dcol\n");
    exit(-1);
    }
if(!(drow=malloc(sizeof*drow*seedsbf->nvoxels))) {
    printf("Error: Unable to malloc drow\n");
    exit(-1);
    }
if(!(dslice=malloc(sizeof*dslice*seedsbf->nvoxels))) {
    printf("Error: Unable to malloc dslice\n");
    exit(-1);
    }
if(!(atlascoor=malloc(sizeof*atlascoor*seedsbf->nvoxels*3))) {
    printf("Error: Unable to malloc atlascoor\n");
    exit(-1);
    }
if(!(s=malloc(sizeof*s))) {
    printf("Error: Unable to malloc s\n");
    exit(-1);
    }
if(!(infnani=malloc(sizeof*infnani*ncor))) {
    printf("Error: Unable to malloc infnani\n");
    exit(-1);
    }
X = dmatrix_0(1,lengthX,1,fs->nlines);
XTX = dmatrix(1,lengthX,1,lengthX);
XTXm1 = dmatrix(1,lengthX,1,lengthX);
Y = dmatrix(1,1,1,fs->nlines);
XTY = dmatrix(1,lengthX,1,1);
B = dmatrix(1,lengthX,1,1);

if(num_cov_files) {
    for(index=3,strptr=sncov->files,q=1,small=uns_count=l=m=0;m<tc_files->nfiles && !uns_count;m++) {
        if(fbf->num_regions_by_file[m]) {
            if(!(mm=map_disk_double(strptr,num_cov_files*fbf->num_regions_by_file[m],0))) exit(-1);
            for(i=0;i<fbf->num_regions_by_file[m] && !uns_count;i++,q++) {
                for(n=0;n<num_cov_files;n++) {
                    if((X[index+n][q]=mm->dptr[num_cov_files*i+n]) == (double)UNSAMPLED_VOXEL) uns_count++;
                    if(fabs(X[index+n][q]) < (double)UNSAMPLED_VOXEL) small++;
                    }
                }
            if(!unmap_disk(mm)) exit(-1);
            strptr += sncov->strlen_files[l++];
            }
        }
    printf("uns_count=%d small=%d\n",uns_count,small);
    }
for(sum=0,ll=q=0;q<nseeds;q++,ll++) {
    start = ll;
    sum += seedsbf->nvoxels_region[q]-1; 
    assignXnew(tc_files->nfiles,fbf->num_regions_by_file,snseeds,nseeds,1,goose,q,0,sindexseeds,fs,X,s);
    if(s->uns_count || s->small==toomany) {
        printf("q=%d Problem with this seed. s->uns_count=%d s->small=%d\n",q,s->uns_count,s->small);
        exit(-1);
        }
    dmatrix_mult_nomem(X,X,lengthX,fs->nlines,lengthX,fs->nlines,(int)TRANSPOSE_SECOND,XTX);
    sprintf(XTXstr,"XTX %s",seedsbf->region_names_ptr[q]);
    cond=inverse_cover(XTX,XTXm1,lengthX,10000.,XTXstr);
    printf("XTX condition number = %f\n",cond);
    #if 1
    printf("X\n");
    for(m=1;m<=fs->nlines;m++) {
        for(i=1;i<=lengthX;i++) printf("%g ",X[i][m]);
        printf("\n");
        }
    #endif

    for(ninfnan=j=0;j<ncor;j++) {
        tstat[j] = r[j] = (double)UNSAMPLED_VOXEL;
        if(!(j%modulo)) fprintf(stdout,"Processing voxel %d\n",j);
        if(!ncorreg) {
            if(seedsbf->indices[ll]==ms->brnidx[j]) {
                if(ll!=sum) ll++; 
                continue;
                }
            }
        assignXnew(tc_files->nfiles,fbf->num_regions_by_file,sncor,ncor,1,1,j,0,sindexcor,fs,Y,s);
        /*printf("Y\n"); for(m=1;m<=fs->nlines;m++) for(i=1;i<=lengthX;i++) printf("%g ",Y[1][m]); printf("\n");*/

        if(!s->uns_count && s->small!=toomany) {
            dmatrix_mult_nomem(X,Y,lengthX,fs->nlines,1,fs->nlines,(int)TRANSPOSE_SECOND,XTY);
            dmatrix_mult_nomem(XTXm1,XTY,lengthX,lengthX,lengthX,1,(int)TRANSPOSE_NONE,B);
            for(yty=0.,m=1;m<=fs->nlines;m++) yty += Y[1][m]*Y[1][m]; 
            for(btxty=0.,m=1;m<=lengthX;m++) btxty += B[m][1]*XTY[m][1]; 
            var = (yty-btxty)/df;
            tstat[j] = B[2][1]/sqrt(var*XTXm1[2][2]);
            r[j] = tstat[j]/sqrt(tstat[j]*tstat[j]+df);

          /*#ifndef LINUX*/
          #ifdef __sun__

            if(IsNANorINF(tstat[j])) {
          #else
            if(isnan(tstat[j]) || isinf(tstat[j])) {
          #endif
                infnani[ninfnan++] = j;
                }
            }
        }

    printf("seed region = %s\n",seedsbf->region_names_ptr[q]);
    col_row_slice(seedsbf->nvoxels_region[q],&seedsbf->indices[start],dcol,drow,dslice,ap);
    get_atlas_coor(seedsbf->nvoxels_region[q],dcol,drow,dslice,(double)ap->zdim,ap->center,ap->mmppix,atlascoor);
    printf("\nvoxels = "); 
    for(k=j=0;j<seedsbf->nvoxels_region[q];j++) printf("%d %d %d, ",rint(atlascoor[k++]),rint(atlascoor[k++]),rint(atlascoor[k++]));
    col_row_slice(ninfnan,infnani,dcol,drow,dslice,ap);
    get_atlas_coor(ninfnan,dcol,drow,dslice,(double)ap->zdim,ap->center,ap->mmppix,atlascoor);
    printf("\n\ninf or nan = ");
    for(k=j=0;j<ninfnan;j++) printf("%d %d %d tstat=%f, ",rint(atlascoor[k++]),rint(atlascoor[k++]),rint(atlascoor[k++]),
        tstat[infnani[j]]);
    printf("\n");

    if(!ncorreg) {
        for(j=0;j<ncor;j++) temp_float[ms->brnidx[j]] = (float)tstat[j];
        sprintf(filename,"seed_%s_tstat%s.4dfp.img",seedsbf->region_names_ptr[q],ap->str);
        if(!writestack(filename,temp_float,sizeof(float),(size_t)ap->vol,swapbytes)) exit(-1);
        min_and_max_init(&ifh->global_min,&ifh->global_max);
        min_and_max_doublestack(tstat,ncor,&ifh->global_min,&ifh->global_max);
        ifh->dim4 = 1;
        ifh->dof_condition = (float)df;
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
        printf("T stat written to %s\n",filename);

        for(j=0;j<ncor;j++) temp_float[ms->brnidx[j]] = (float)r[j];
        sprintf(filename,"seed_%s_r%s.4dfp.img",seedsbf->region_names_ptr[q],ap->str);
        if(!writestack(filename,temp_float,sizeof(float),(size_t)ap->vol,swapbytes)) exit(-1);
        min_and_max_init(&ifh->global_min,&ifh->global_max);
        min_and_max_doublestack(r,ncor,&ifh->global_min,&ifh->global_max);
        ifh->dim4 = 1;
        ifh->dof_condition = (float)df;
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
        printf("T stat written to %s\n",filename);
        }




    }

if(lccleanup) {
    sprintf(filename,"rm -r %s",scratchdir);
    if(system(filename) == -1) printf("Error: unable to %s\n",filename);
    }
exit(0);
}

/*START150422*/
/**********************************/
FS *make_FS(int num_frames,int *foi)
/**********************************/
{
    int i;
    FS *fs;

    if(!(fs=malloc(sizeof*fs))) {
        printf("Error: Unable to malloc fs\n");
        return NULL;
        }
    fs->ntc = fs->nframes = fs->nlines = num_frames;
    if(!(fs->frames_per_line=malloc(sizeof*fs->frames_per_line*fs->nlines))) {
        printf("Error: Unable to malloc fs->frames_per_line\n");
        return NULL;
        }
    if(!(fs->num_frames_to_sum=malloc(sizeof*fs->num_frames_to_sum*fs->ntc))) {
        printf("Error: Unable to malloc fs->num_frames_to_sum\n");
        return NULL;
        }
    for(i=0;i<num_frames;i++) fs->frames_per_line[i] = fs->num_frames_to_sum[i] = 1;
    if(!(fs->frames=malloc(sizeof*fs->frames*fs->nframes))) {
        printf("Error: Unable to malloc fs->frames\n");
        return NULL;
        }
    for(i=0;i<num_frames;i++) fs->frames[i] = foi[i] - 1;
    return fs;
}
