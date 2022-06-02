/* Copyright 2/20/19 Washington University.  All Rights Reserved.
   fidl_cov.cxx  $Revision: 1.1 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>

//#include "region.h"
//START210831
#include "region3.h"

#include "files_struct.h"
#include "dim_param2.h"
#include "get_atlas_param.h"

//#include "find_regions_by_file_cover.h"
//START210901
//#include "find_regions_by_file_cover2.h"
#include "find_regions_by_file_cover3.h"

#include "read_data.h"
#include "checkOS.h"
#include "get_atlas.h"
#include "check_dimensions.h"

//#include "globcov.h"
//START210909
#include "globcov2.h"

#include "subs_util.h"
#include "constants.h"
#include "strutil.h"

int read_giowtfile(char *confile,int how_many,char **region_names_ptr,double *contrasts);
void magnorm(double *out,int nfiles,int *tdim,int nt,int ncol,int *valid_frms,double *mag);
int main(int argc,char **argv){
char *out_file="fidl_junk.dat",*giowtfile=NULL,atlas[7]="",*lutf=NULL; /* *atlas=NULL */
size_t i0;
int i,j,k,jj,kk,i1,i2,j1,k1,idx,idx1,SunOS_Linux,nfiles,num_tc_files=0,nxform_file=0,nt4_identify=0,num_region_files=0,
    nmask_file=0,nt,tcol,ncol=1,num_exclude_frames=0,*t4_identify=NULL,*roi=NULL,*exclude_framesi=NULL,*valid_frms=NULL,*scalingv,
    argc_roi=0,nroi=0,scaling=-1,nvoxwtf=0,*weightsi=NULL,ncoli=0,*coli=NULL,nreadcol=0,argc_addsubroi=0,naddsubroi=0,
    lccompressed=0,*tdim=NULL,ntdim=0,tdim_total=0,nregname=0,argc_addsubroinorm2=0,naddsubroinorm2=0,argc_addsubroinonorm2=0,
    naddsubroinonorm2=0;
    //lcnamesonly=0
double *out=NULL,*weights=NULL,*mag=NULL,*out0=NULL;
Files_Struct *tc_files=NULL,*xform_file=NULL,*region_files=NULL,*mask_file=NULL,*voxwtf=NULL,*regname=NULL;
Dim_Param2 *dp=NULL;
Atlas_Param *ap=NULL;
Regions **reg;
Regions_By_File *rbf;
FILE *fp;
Data **voxwt=NULL;
if(argc<3) {
    std::cout<<"    -tc_files:            4dfps, nifitis, conc or list"<<std::endl; 
    std::cout<<"                          If not included then only regressor names are output."<<std::endl;
    std::cout<<"    -atlas:               Either 111, 222 or 333."<<std::endl;
    std::cout<<"    -xform_file:          t4 file(s) defining transform to atlas space."<<std::endl;
    std::cout<<"                          Analysis is done in data space if this argument is not present."<<std::endl;
    std::cout<<"    -t4:                  If more than a single t4. Identifies which t4 goes with each run. First t4 is 1."<<std::endl;
    std::cout<<"    -mask:                Inclusive to all voxels greater than or less than 1e-37."<<std::endl;
    std::cout<<"    -regname:             Used with -mask. One for each mask file. Regressor uses this name. Default is the name of the mask file."<<std::endl;
    std::cout<<"    -region_file:         fidl region files. For niftis, the filename is used as the region name."<<std::endl;
    std::cout<<"    -regions_of_interest: First region is 1."<<std::endl;
    std::cout<<"    -exclude_frames:      Frames to exclude from the analysis. First frame is 1."<<std::endl;
    std::cout<<"                          If not specified then all frames are included."<<std::endl;
    std::cout<<"    -out:                 Output filename. Default is fidl_junk.dat"<<std::endl;
    std::cout<<"    -scale0to1            Scale regressors 0 to 1."<<std::endl;
    std::cout<<"    -scale-1to1           Scale regressors -1 to 1."<<std::endl;
    std::cout<<"    -donotscale           Do not scale regressors."<<std::endl;
    std::cout<<"    -zeromean             Regressors will be zero mean."<<std::endl;
    std::cout<<"    -zeromeanmagnorm      Regressors will be zero mean and magnitude normalized."<<std::endl;
    std::cout<<"    -zeromeanslope        Regressors will be zero mean and zero slope."<<std::endl;
    std::cout<<"    -gio                  Compute correlation matrix, extract first principal component, compute timecourse"<<std::endl;
    std::cout<<"                          of first principal component."<<std::endl;
    std::cout<<"    -gio:                 File with PC wts. Each region is a row. Compute timecourse of first PC."<<std::endl;
    std::cout<<"    -voxwt:               Each region is a separate file. Each row is a voxel. Each column is a regressor and must have a text label."<<std::endl;
    std::cout<<"    -columns:             Which columns to use with -voxwt. If not present, then all."<<std::endl;
    std::cout<<"    -addsub_regions_of_interest: First region is 1. ||1||+||2|| and ||1||-||2||"<<std::endl;
    std::cout<<"    -addsubroinorm2:   First region is 1. ||1+2|| and ||1-2||"<<std::endl;
    std::cout<<"    -addsubroinonorm2: First region is 1. 1+2 and 1-2"<<std::endl;
    std::cout<<"    -tdim:           If tc_files only include valid frames, then tdim provides the dimensions for all frames."<<std::endl;

    //START210831
    std::cerr<<"    -lut:  Lookup table. Ex. $FREESURFER_HOME/FreeSurferColorLUT.txt"<<std::endl;
    std::cerr<<"           Not necessary for all files as some regions are identified by the file name."<<std::endl;
    std::cerr<<"           Need not be an actual freesurfer lookuptable. Just needs to follow the freesurfer format."<<std::endl;

    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-tc_files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++num_tc_files;
        if(!(tc_files=read_files(num_tc_files,&argv[i+1])))exit(-1);
        i+=num_tc_files;
        }
    if(!strcmp(argv[i],"-atlas") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        strcpy(atlas,argv[++i]);
    if(!strcmp(argv[i],"-xform_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nxform_file;
        if(!(xform_file=get_files(nxform_file,&argv[i+1]))) exit(-1);
        i += nxform_file;
        }
    if(!strcmp(argv[i],"-t4") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nt4_identify;
        if(!(t4_identify=(int*)malloc(sizeof*t4_identify*nt4_identify))) {
            printf("fidlError: Unable to malloc t4_identify\n");
            exit(-1);
            } 
        for(j=0;j<nt4_identify;j++) t4_identify[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nmask_file;
        if(!(mask_file=get_files(nmask_file,&argv[i+1]))) exit(-1);
        i += nmask_file;
        }
    if(!strcmp(argv[i],"-regname") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nregname;
        if(!(regname=get_files(nregname,&argv[i+1]))) exit(-1);
        i += nregname;
        }
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroi;
        //std::cerr<<"here0 nroi="<<nroi<<std::endl;
        argc_roi = i+1;
        i += nroi;
        }
    if(!strcmp(argv[i],"-exclude_frames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_exclude_frames;
        if(!(exclude_framesi=(int*)malloc(sizeof*exclude_framesi*num_exclude_frames))) {
            printf("fidlError: Unable to malloc exclude_framesi\n");
            exit(-1);
            }
        for(j=0;j<num_exclude_frames;j++) exclude_framesi[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-out") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        out_file = argv[++i];
    if(!strcmp(argv[i],"-scale0to1"))
        scaling = 2;
    if(!strcmp(argv[i],"-scale-1to1"))
        scaling = 1;
    if(!strcmp(argv[i],"-donotscale"))
        scaling = 0;
    if(!strcmp(argv[i],"-zeromean"))
        scaling = 3;
    if(!strcmp(argv[i],"-zeromeanmagnorm"))
        scaling = 4;
    if(!strcmp(argv[i],"-zeromeanslope"))
        scaling = 5;
    if(!strcmp(argv[i],"-gio")) {
        scaling = 6;
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) giowtfile = argv[++i];
        }
    if(!strcmp(argv[i],"-voxwt") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nvoxwtf;
        if(!(voxwtf=get_files(nvoxwtf,&argv[i+1]))) exit(-1);
        i += nvoxwtf;
        }
    if(!strcmp(argv[i],"-columns") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ncoli;
        if(!(coli=(int*)malloc(sizeof*coli*ncoli))) {
            printf("fidlError: Unable to malloc coli\n");
            exit(-1);
            }
        for(nreadcol=j=0;j<ncoli;j++) if((coli[j]=atoi(argv[++i])-1)>nreadcol) nreadcol=coli[j];
        nreadcol++;
        }
    if(!strcmp(argv[i],"-addsub_regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++naddsubroi;
        argc_addsubroi = i+1;
        i += naddsubroi;
        }
    if(!strcmp(argv[i],"-addsubroinorm2") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++naddsubroinorm2;
        argc_addsubroinorm2 = i+1;
        i += naddsubroinorm2;
        }
    if(!strcmp(argv[i],"-addsubroinonorm2") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++naddsubroinonorm2;
        argc_addsubroinonorm2 = i+1;
        i += naddsubroinonorm2;
        }
    if(!strcmp(argv[i],"-tdim") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++ntdim;
        if(!(tdim=(int*)malloc(sizeof*tdim*ntdim))) {
            std::cout<<"fidlError: Unable to malloc tdim"<<std::endl;
            exit(-1);
            }
        for(j=0;j<ntdim;j++) tdim[j] = atoi(argv[++i]);
        }

    //START210831
    if(!strcmp(argv[i],"-lut")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            lutf = argv[++i];
            }
        else {
            std::cerr<<"fidlError: No lookup table specified after -lut option. Abort!"<<std::endl;
            exit(-1);
            }
        }

    }
if(!num_tc_files) {

    //START200826
    //lcnamesonly=1;

    }
else if(ntdim) {
    if(ntdim!=(int)tc_files->nfiles) {

        //printf("fidlError: ntdim=%d tc_files->nfiles=%zd Must be equal. Abort!\n",ntdim,tc_files->nfiles);
        std::cout<<"fidlError: ntdim="<<ntdim<<" tc_files->nfiles="<<tc_files->nfiles<<" Must be equal. Abort!"<<std::endl;

        exit(-1);
        }
    }
if(nt4_identify) {
    if(nt4_identify!=(int)tc_files->nfiles) {

        //printf("Error: nt4_identify=%d tc_files->nfiles=%zd Must be equal. Abort!\n",nt4_identify,tc_files->nfiles);
        std::cout<<"fidlError: nt4_identify="<<nt4_identify<<" tc_files->nfiles="<<tc_files->nfiles<<" Must be equal. Abort!"<<std::endl;

        exit(-1);
        }
    }
else if(num_tc_files) {
    if(!(t4_identify=(int*)malloc(sizeof*t4_identify*tc_files->nfiles))) {
        std::cout<<"fidlError: Unable to malloc t4_identify"<<std::endl;
        exit(-1);
        } 
    for(i0=0;i0<tc_files->nfiles;i0++) t4_identify[i0]=0;
    }
if(naddsubroi&&naddsubroinorm2) {
    std::cout<<"fidlError: Not set up to do both -addsub_regions_of_interest and -addsubroinorm2 Abort!"<<std::endl;exit(-1);
    }
if(naddsubroi&&naddsubroinonorm2) {
    std::cout<<"fidlError: Not set up to do both -addsub_regions_of_interest and -addsubroinonorm2 Abort!"<<std::endl;exit(-1);
    }
if(naddsubroinorm2&&naddsubroinonorm2) {
    std::cout<<"fidlError: Not set up to do both -addsubroinorm2 and -addsubroinonorm2 Abort!"<<std::endl;exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);


if(num_tc_files){

    //if(!(dp=dim_param2(tc_files->nfiles,tc_files->files,SunOS_Linux)))exit(-1);
    //START190823
    if(!(dp=dim_param2(tc_files->nfiles,tc_files->files)))exit(-1);

    if(!dp->volall==-1){printf("fidlError: All files must have the same volume.\n");fflush(stdout);exit(-1);}

    //if(!xform_file) {if(!atlas[0]) get_atlas(dp->volall,atlas); else lccompressed=1;}
    //START190823
    //lccompressed=1; 
    if(!xform_file){
        if(!atlas[0]){
            get_atlas(dp->volall,atlas); 
            //printf("atlas=%s\n",atlas);
            if(!atlas[0])lccompressed=1;
            }
        else lccompressed=1;
        }

    if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL,(char*)NULL))) exit(-1);
    if(!xform_file&&!atlas[0]) ap->vol = dp->volall;
    if(!ntdim) {tdim=dp->tdim;tdim_total=dp->tdim_total;} else for(tdim_total=i=0;i<ntdim;i++) tdim_total+=tdim[i];
    if(!atlas[0]) {
        ap->xdim = dp->xdim[0];
        ap->ydim = dp->ydim[0];
        ap->zdim = dp->zdim[0];
        ap->voxel_size[0] = dp->dx[0];
        ap->voxel_size[1] = dp->dy[0];
        ap->voxel_size[2] = dp->dz[0];
        }
    }

#if 0    
if(!(nt=nroi+naddsubroi+nmask_file+naddsubroinorm2+naddsubroinonorm2)) {
    printf("fidlError: nt=%d nmask_file=%d nroi=%d naddsubroi=%d naddsubroinorm2=%d naddsubroinonorm2=%d nt should not be zero.\n",
        nt,nmask_file,nroi,naddsubroi,naddsubroinorm2,naddsubroinonorm2);fflush(stdout);exit(-1);
    }
if(!(roi=(int*)malloc(sizeof*roi*nt))) {
    printf("fidlError: Unable to malloc roi\n");
    exit(-1);
    }
for(j=0;j<nroi;j++) roi[j] = atoi(argv[argc_roi+j]) - 1;
for(i=0;i<naddsubroi;i++,j++) roi[j] = atoi(argv[argc_addsubroi+i]) - 1;
for(i=0;i<naddsubroinorm2;i++,j++) roi[j] = atoi(argv[argc_addsubroinorm2+i]) - 1;
for(i=0;i<naddsubroinonorm2;i++,j++) roi[j] = atoi(argv[argc_addsubroinonorm2+i]) - 1;
#endif
//START210901
if(!(nt=nroi+naddsubroi+nmask_file+naddsubroinorm2+naddsubroinonorm2)) {
    printf("fidlError: nt=%d nmask_file=%d nroi=%d naddsubroi=%d naddsubroinorm2=%d naddsubroinonorm2=%d nt should not be zero.\n",
        nt,nmask_file,nroi,naddsubroi,naddsubroinorm2,naddsubroinonorm2);fflush(stdout);exit(-1);
    }
Files_Struct* roistr=NULL;int lcisalpha=0;
if(nroi){
    if(!(roistr=read_files(nroi,&argv[argc_roi])))exit(-1);    
    for(size_t i=0;i<roistr->nfiles;++i){
        for(j=0;roistr->files[i][j];++j){
            if(isalpha(roistr->files[i][j])){lcisalpha=1;break;}
            }
        }
    }
if(!lcisalpha){
    if(!(roi=(int*)malloc(sizeof*roi*nt))) {
        printf("fidlError: Unable to malloc roi\n");
        exit(-1);
        }
    for(j=0;j<nroi;j++) roi[j] = atoi(argv[argc_roi+j]) - 1;
    for(i=0;i<naddsubroi;i++,j++) roi[j] = atoi(argv[argc_addsubroi+i]) - 1;
    for(i=0;i<naddsubroinorm2;i++,j++) roi[j] = atoi(argv[argc_addsubroinorm2+i]) - 1;
    for(i=0;i<naddsubroinonorm2;i++,j++) roi[j] = atoi(argv[argc_addsubroinonorm2+i]) - 1;
    }


nfiles = num_region_files + nmask_file;
if(!(reg=(Regions**)malloc(sizeof*reg*nfiles))) {
    std::cout<<"fidlError: Unable to malloc reg\n"<<std::endl;
    exit(-1);
    }

if(num_region_files) {
    if(num_tc_files){ 
        if(!atlas[0]&&lccompressed){
            /*do nothing*/
            }
        else{
            if(!check_dimensions(num_region_files,region_files->files,ap->vol))exit(-1);
            }
        }
    int lcflipreg=shouldiflip(region_files);
    std::cerr<<"lcflipreg="<<lcflipreg<<std::endl;

    for(i=0;i<num_region_files;i++){

        //if(!(reg[i]=get_reg(region_files->files[i],0,(float*)NULL,0,(char**)NULL,lcflipreg,num_tc_files?0:1,0,(int*)NULL,(char*)NULL)))exit(-1);
        //START210831
        //if(!(reg[i]=get_reg(region_files->files[i],0,(float*)NULL,0,(char**)NULL,lcflipreg,num_tc_files?0:1,0,(int*)NULL,lutf,0)))exit(-1);
        //if(!(reg[i]=get_reg(region_files->files[i],0,(float*)NULL,0,(char**)NULL,lcflipreg,num_tc_files?0:1,0,(int*)NULL,strstr(region_files->files[i],"wmparc")?lutf:(char*)NULL,0)))exit(-1);
        //START210902
        //if(!(reg[i]=get_reg(region_files->files[i],0,(float*)NULL,0,(char**)NULL,lcflipreg,num_tc_files?0:1,0,(int*)NULL,strstr(region_files->files[i],"wmparc")?lutf:(char*)NULL,1)))exit(-1);
        //if(!(reg[i]=get_reg(region_files->files[i],0,(float*)NULL,0,(char**)NULL,lcflipreg,num_tc_files?0:1,0,(int*)NULL,lutf,1)))exit(-1);
        if(!(reg[i]=get_reg(region_files->files[i],0,(float*)NULL,0,(char**)NULL,lcflipreg,num_tc_files?0:1,0,(int*)NULL,strstr(region_files->files[i],"wmparc")?lutf:(char*)NULL)))exit(-1);

        //std::cout<<"reg[i]->region_names[0]="<<reg[i]->region_names[0]<<std::endl;
        //std::cout<<"reg[i]->region_names[1]="<<reg[i]->region_names[1]<<std::endl;
        }

    //START190823
    if(num_tc_files&&!atlas[0]&&lccompressed)ap->vol=reg[0]->vol;

    }


if(mask_file) {
    for(j=i=0;i<num_region_files;i++) j += reg[i]->nregions;
    for(k1=nroi+naddsubroi+naddsubroinorm2+naddsubroinonorm2,k=num_region_files,i=0;i<nmask_file;i++,j++,k++) {
        roi[k1++] = j;

        //if(!(reg[k]=get_reg(mask_file->files[i],0,(float*)NULL,0,(char**)NULL,0,num_tc_files?0:1,0,(int*)NULL,(char*)NULL))) exit(-1);
        //START210831
        //if(!(reg[k]=get_reg(mask_file->files[i],0,(float*)NULL,0,(char**)NULL,0,num_tc_files?0:1,0,(int*)NULL,(char*)NULL,0))) exit(-1);
        if(!(reg[k]=get_reg(mask_file->files[i],0,(float*)NULL,0,(char**)NULL,0,num_tc_files?0:1,0,(int*)NULL,(char*)NULL))) exit(-1);

        }
    }

//if(!(rbf=find_regions_by_file_cover(nfiles,nt,reg,roi)))exit(-1);
//START20901
if(!(rbf=find_regions_by_file_cover(nfiles,nt,reg,roi,roistr)))exit(-1);

if(num_tc_files)for(i=0;i<rbf->nreg;++i)std::cerr<<"fidl_cov.cxx "<<rbf->region_names_ptr[i]<<" "<<rbf->nvoxels_region[i]<<std::endl;

if(mask_file&&nregname&&(nmask_file==nregname)) {
    free(rbf->region_names_ptr);
    rbf->region_names_ptr=regname->files;
    }
if(!nvoxwtf) {
    tcol=nt;
    if(giowtfile) {
        if(!(weights=(double*)malloc(sizeof*weights*nt))) {
            printf("fidlError: Unable to malloc weights\n");
            exit(-1);
            }
        if(!read_giowtfile(giowtfile,nt,rbf->region_names_ptr,weights)) exit(-1);
        fprintf(stderr,"weights=\n"); for(i=0;i<nt;i++) fprintf(stderr,"%f ",weights[i]); fprintf(stderr,"\n");
        }
    } 
else {
    if(nvoxwtf!=nroi) {
        printf("fidlError: Number of voxwt files must equal the number of regions. nvoxwtf=%d nroi=%d\n",nvoxwtf,nroi);
        exit(-1);
        }
    if(!(voxwt=(Data**)malloc(sizeof*voxwt*nvoxwtf))) {
        printf("Error: Unable to malloc voxwt\n");
        exit(-1);
        }
    for(i=0;i<nvoxwtf;i++) {
        fprintf(stderr,"Reading %s\n",voxwtf->files[i]);
        if(!(voxwt[i]=read_data(voxwtf->files[i],0,0,nreadcol,1))) exit(-1); 
        /*printf("voxwt[%d]->nsubjects=%d\n",i,voxwt[i]->nsubjects); fflush(stdout);
        for(j=0;j<voxwt[i]->nsubjects;j++) {
            for(k=0;k<voxwt[i]->npoints;k++) printf("%f\t",voxwt[i]->x[j][k]); 
            printf("\n");
            }
        printf("nreadcol=%d\n",nreadcol);*/
        }
    tcol = ncoli*nvoxwtf;
    for(i=1;i<nvoxwtf;i++) {
        if(voxwt[i]->ncol!=voxwt[0]->ncol) {
            printf("Error: All voxwt files must have the same number of columns.\n");
            for(j=0;j<nvoxwtf;j++) printf("Error:     %s  number of columns = %d\n",voxwtf->files[j],voxwt[j]->ncol);
            exit(-1);
            }
        }
    for(i=0;i<nvoxwtf;i++) {
        if(voxwt[i]->nsubjects!=rbf->nvoxels_region[i]) {
            printf("Error: %s  voxwt[%d]->nsubjects=%d rbf->nvoxels_region[%d]=%d  Must be equal.\n",voxwtf->files[i],i,
                voxwt[i]->nsubjects,i,rbf->nvoxels_region[i]);
            exit(-1);
            }
        }
    if(!(weights=(double*)malloc(sizeof*weights*rbf->nvoxels*ncoli))) {
        printf("fidlError: Unable to malloc weights\n");
        exit(-1);
        }
    for(kk=i=0;i<ncoli;i++) for(j=0;j<nvoxwtf;j++) for(k=0;k<voxwt[j]->nsubjects;k++,kk++) weights[kk] = voxwt[j]->x[k][coli[i]]; 
    /*for(i=0;i<rbf->nvoxels*ncoli;i++) printf("%f ",weights[i]); printf("\n");
    printf("rbf->nvoxels*ncoli=%d\n",rbf->nvoxels*ncoli);*/
    if(!(weightsi=(int*)malloc(sizeof*weightsi*rbf->nvoxels))) {
        printf("fidlError: Unable to malloc weightsi\n");
        exit(-1);
        }
    for(kk=j=0;j<nvoxwtf;j++) for(k=0;k<voxwt[j]->nsubjects;k++,kk++) weightsi[kk] = kk; 
    /*for(i=0;i<rbf->nvoxels;i++) printf("%d ",weightsi[i]); printf("\n");*/
    ncol = ncoli;
    }
if(num_tc_files) {
    if(!(valid_frms=(int*)malloc(sizeof*valid_frms*tdim_total))) {
        printf("fidlError: Unable to malloc valid_frms\n");
        exit(-1);
        }
    for(i=0;i<tdim_total;i++) valid_frms[i]=1;
    for(i=0;i<num_exclude_frames;i++) valid_frms[exclude_framesi[i]]=0;
    if(!(out=(double*)malloc(sizeof*out*tcol*tdim_total))) {
        printf("fidlError: Unable to malloc out\n");
        exit(-1);
        }
    if(naddsubroinorm2) if(!(out0=(double*)malloc(sizeof*out0*tcol*tdim_total))) {
        printf("fidlError: Unable to malloc out0\n");
        exit(-1);
        }
    }
if(!(scalingv=(int*)malloc(sizeof*scalingv*nt))) {
    printf("fidlError: Unable to malloc scalingv\n");
    exit(-1);
    }
if(scaling==-1) {
    for(j=0;j<nroi+naddsubroi+naddsubroinorm2+naddsubroinonorm2;j++) scalingv[j]=2;
    if(mask_file) for(i=0;i<nmask_file;i++,j++) scalingv[j]=1;
    }
else {
    for(i=0;i<nt;i++) scalingv[i]=scaling;
    }
for(i=0;i<nt;i++) {
    if(scalingv[i]==0) fprintf(stderr,"%d unscaled\n",i+1);
    else if(scalingv[i]==1) fprintf(stderr,"%d scale -1 to 1\n",i+1);
    else if(scalingv[i]==2) fprintf(stderr,"%d scale 0 to 1\n",i+1);
    else if(scalingv[i]==3) fprintf(stderr,"%d scale zero mean\n",i+1);
    else if(scalingv[i]==4) fprintf(stderr,"%d scale zero mean, magnitude normalized\n",i+1);
    else if(scalingv[i]==5) fprintf(stderr,"%d scale zero mean, zero slope\n",i+1);
    else if(scalingv[i]==6) fprintf(stderr,"%d scale gio\n",i+1);
    } 
/*printf("scalingv="); for(i=0;i<num_regions;i++) printf("%d ",scalingv[i]); printf("\n");*/
/*printf("valid_frms\n"); for(i=0;i<dp->tdim_total;i++) printf("%d %d\n",i+1,valid_frms[i]);*/
/*printf("ncol=%d nt=%d\n",ncol,nt); fflush(stdout);*/

//printf("here0 lccompressed=%d ap->vol=%d\n",lccompressed,ap->vol);fflush(stdout);


if(num_tc_files)

    //std::cout<<"here5"<<std::endl;

    //if(!globcov_guts(tc_files,dp,valid_frms,out,SunOS_Linux,scalingv,rbf,xform_file,t4_identify,ap,weights,weightsi,ncol,
    //    lccompressed,tdim)) exit(-1);
    //START190909 
    if(!globcov_guts(tc_files,dp,valid_frms,out,scalingv,rbf,xform_file,t4_identify,ap,weights,weightsi,ncol,lccompressed,tdim))exit(-1);
    //int superbird; 
    //superbird=globcov_guts(tc_files,dp,valid_frms,out,scalingv,rbf,xform_file,t4_identify,ap,weights,weightsi,ncol,lccompressed,tdim);

    //std::cout<<"here6 superbird="<<superbird<<std::endl;


//printf("out\n"); for(i=0;i<tcol*dp->tdim_total;i++) printf("%f\n",out[i]); printf("\n");
//printf("tcol=%d dp->tdim_total=%d tcol*dp->tdim_total=%d\n",tcol,dp->tdim_total,tcol*dp->tdim_total); fflush(stdout);

if(!(fp=fopen_sub(out_file,"w"))) exit(-1);
if(scaling<6) {
    if(num_tc_files&&(scaling==4||naddsubroi||naddsubroinorm2)) {
        if(!(mag=(double*)malloc(sizeof*mag*nt*ncol))) {
            printf("fidlError: Unable to malloc mag\n");
            exit(-1);
            }
        }
    if(naddsubroi) {
        if(ncol>1) {printf("fidlError: ncol=%d Not set up for ncol>1\n",ncol);fflush(stdout);exit(-1);}
        for(i=0;i<nt;i+=2) fprintf(fp,"%sL2_%s+%sL2_%s\t%sL2_%s-%sL2_%s\t",scalingv[i]==3?"zm":"",rbf->region_names_ptr[i],
            scalingv[i+1]==3?"zm":"",rbf->region_names_ptr[i+1],scalingv[i]==3?"zm":"",rbf->region_names_ptr[i],
            scalingv[i+1]==3?"zm":"",rbf->region_names_ptr[i+1]);
        fprintf(fp,"\n");
        if(num_tc_files) {
            magnorm(out,tc_files->nfiles,tdim,nt,ncol,valid_frms,mag);
            for(jj=i0=0;i0<tc_files->nfiles;jj+=tdim[i0++]) {
                for(i1=jj,j=0;j<tdim[i0];j++,i1++) {
                    for(j1=0;j1<nt;j1+=2) {
                        idx = nt*ncol*i1+j1;
                        idx1 = nt*ncol*i1+j1+1;
                        fprintf(fp,"%f\t%f\t",out[idx]+out[idx1],out[idx]-out[idx1]);
                        }
                    fprintf(fp,"\n");
                    }
                }
            }
        }
    else if(naddsubroinorm2) {
        if(ncol>1) {printf("fidlError: ncol=%d Not set up for ncol>1\n",ncol);fflush(stdout);exit(-1);}
        for(i=0;i<nt;i+=2) fprintf(fp,"%s_%s+%s_%s_L2\t%s_%s-%s_%s_L2\t",scalingv[i]==3?"zm":"",rbf->region_names_ptr[i],
            scalingv[i+1]==3?"zm":"",rbf->region_names_ptr[i+1],scalingv[i]==3?"zm":"",rbf->region_names_ptr[i],
            scalingv[i+1]==3?"zm":"",rbf->region_names_ptr[i+1]);
        fprintf(fp,"\n");
        if(num_tc_files) {
            for(i2=jj=i0=0;i0<tc_files->nfiles;jj+=tdim[i0++]) {
                for(i1=jj,j=0;j<tdim[i0];j++,i1++) {
                    for(j1=0;j1<nt;j1+=2) {
                        idx = nt*ncol*i1+j1;
                        idx1 = nt*ncol*i1+j1+1;
                        out0[i2++] = out[idx]+out[idx1];
                        out0[i2++] = out[idx]-out[idx1];
                        }
                    }
                }
            magnorm(out0,tc_files->nfiles,tdim,nt,ncol,valid_frms,mag);
            for(i=0;i<tdim_total;i++) {
                for(j=0;j<nt;j++) for(k=0;k<ncol;k++) fprintf(fp,"%f\t",out0[nt*ncol*i+nt*k+j]);
                fprintf(fp,"\n");
                }
            }
        }

    /*START140924*/
    else if(naddsubroinonorm2) {
        if(ncol>1) {printf("fidlError: ncol=%d Not set up for ncol>1\n",ncol);fflush(stdout);exit(-1);}
        for(i=0;i<nt;i+=2) fprintf(fp,"%s_%s+%s_%s\t%s_%s-%s_%s\t",scalingv[i]==3?"zm":"",rbf->region_names_ptr[i],
            scalingv[i+1]==3?"zm":"",rbf->region_names_ptr[i+1],scalingv[i]==3?"zm":"",rbf->region_names_ptr[i],
            scalingv[i+1]==3?"zm":"",rbf->region_names_ptr[i+1]);
        fprintf(fp,"\n");
        if(num_tc_files) {
            for(i2=jj=i0=0;i0<tc_files->nfiles;jj+=tdim[i0++]) {
                for(i1=jj,j=0;j<tdim[i0];j++,i1++) {
                    for(j1=0;j1<nt;j1+=2) {
                        idx = nt*ncol*i1+j1;
                        idx1 = nt*ncol*i1+j1+1;
                        fprintf(fp,"%f\t%f\t",out[idx]+out[idx1],out[idx]-out[idx1]);
                        }
                    fprintf(fp,"\n");
                    }
                }
            }
        }


    else {
        if(ncol==1) {
            for(i=0;i<nt;i++) fprintf(fp,"%s%s%s%s\t",scaling>=3&&scaling<=5?"zm":"",scaling==4?"L2":"",
                scaling==3||scaling==4?"_":"",rbf->region_names_ptr[i]);
            fprintf(fp,"\n");
            }
        else {
            for(i=0;i<nt;i++) for(j=0;j<ncol;j++) fprintf(fp,"%s_%s\t",rbf->region_names_ptr[i],voxwt[i]->colptr[j]);
            fprintf(fp,"\n");
            }
        if(num_tc_files) {
            if(scaling==4) magnorm(out,tc_files->nfiles,tdim,nt,ncol,valid_frms,mag);
            for(i=0;i<tdim_total;i++) {
                for(j=0;j<nt;j++) for(k=0;k<ncol;k++) fprintf(fp,"%f\t",out[nt*ncol*i+nt*k+j]);
                fprintf(fp,"\n");
                }
            }
        }





    }
else {
    fprintf(fp,"tcPC1\n");
    for(k=i=0;i<dp->tdim_total;i++,k+=nt) fprintf(fp,"%f\n",out[k]);

    #if 0
    /*fprintf(fp,"tcPC1\ttcPC2\n");
    for(k=i=0;i<dp->tdim_total;i++,k+=num_regions) fprintf(fp,"%f\t%f\n",out[k],out[k+1]);*/

    /*fprintf(fp,"tcPC1\ttcPC2\ttcPC3\n");
    for(k=i=0;i<dp->tdim_total;i++,k+=num_regions) fprintf(fp,"%f\t%f\t%f\n",out[k],out[k+1],out[k+2]);*/

    /*fprintf(fp,"tcPC1\ttcPC2\ttcPC3\ttcPC4\n");
    for(k=i=0;i<dp->tdim_total;i++,k+=num_regions) fprintf(fp,"%f\t%f\t%f\t%f\n",out[k],out[k+1],out[k+2],out[k+3]);*/

    fprintf(fp,"tcPC1\ttcPC2\ttcPC3\ttcPC4\ttcPC5\ttcPC6\ttcPC7\ttcPC8\ttcPC9\ttcPC10\ttcPC11\t\n");
    for(k=i=0;i<dp->tdim_total;i++,k+=num_regions) fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
        out[k],out[k+1],out[k+2],out[k+3],out[k+4],out[k+5],out[k+6],out[k+7],out[k+8],out[k+9],out[k+10]);
    #endif
    }
fflush(fp);
fclose(fp);



printf("Output written to %s\n",out_file);
}


int read_giowtfile(char *confile,int how_many,char **region_names_ptr,double *contrasts)
{
    char line[MAXNAME],write_back[MAXNAME],*write_back_ptr,last_char,first_char;
    int i,j,num_factors,regcol,len,nstrings;
    FILE *fp;
    if(!(fp=fopen_sub(confile,"r"))) return 0;
    if(!fgets(line,sizeof(line),fp)) {
        printf("%s is empty.\n",confile);
        return 0;
        }
    if((num_factors=count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,' ') - 1) < 1){/*subtract 1 for "contrast"*/
        printf("Error: Need to list 'region weight' on the first line of %s.\n",confile);
        return 0;
        }

    for(write_back_ptr=write_back,regcol=-1,j=0;j<num_factors;j++) {
        write_back_ptr = grab_string_new(write_back_ptr,line,&len);
        if(!strcmp("region",line)) regcol=j;
        }
    if(regcol==-1) {
        printf("Error: Need to list 'region' on the frist line of %s.\n",confile);
        return 0;
        }
    for(i=0;i<how_many;i++) contrasts[i]=(double)UNSAMPLED_VOXEL;
    for(i=0;i<how_many;i++) {
        rewind(fp);
        fgets(line,sizeof(line),fp);
        for(;fgets(line,sizeof(line),fp);) {
            nstrings = count_strings_new3(line,write_back,' ',' ',&last_char,&first_char,0,' ')-1;
            if(nstrings>0) {
                write_back_ptr = write_back;
                write_back_ptr = grab_string_new(write_back_ptr,line,&len);
                if(strcmp(region_names_ptr[i],line)) continue;
                strings_to_double(write_back_ptr,&contrasts[i],nstrings);
                break;
                }
            }
        }
    fclose(fp);
    return 1;
}
void magnorm(double *out,int nfiles,int *tdim,int nt,int ncol,int *valid_frms,double *mag)
{
    int i,i1,j,jj,j1,j2,k,idx;
    for(jj=i=0;i<nfiles;jj+=tdim[i++]) {
        for(j=0;j<nt*ncol;j++) mag[j]=0.;
        for(i1=jj,j=0;j<tdim[i];j++,i1++) {
            if(!valid_frms[i1]) continue;
            for(j2=j1=0;j1<nt;j1++) for(k=0;k<ncol;k++,j2++) {
                idx = nt*ncol*i1+nt*k+j1;
                mag[j2] += out[idx]*out[idx];
                }
            }
        for(j=0;j<nt*ncol;j++) mag[j]=sqrt(mag[j]);
        for(i1=jj,j=0;j<tdim[i];j++,i1++) {
            if(!valid_frms[i1]) continue;
            for(j2=j1=0;j1<nt;j1++) for(k=0;k<ncol;k++,j2++) {
                idx = nt*ncol*i1+nt*k+j1;
                out[idx]/=mag[j2];
                }
            }
        }
}
