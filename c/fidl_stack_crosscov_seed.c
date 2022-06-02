/* Copyright 2/11/10 Washington University. All Rights Reserved.
   fidl_stack_crosscov_seed.c  $Revision: 1.5 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
#include <gsl/gsl_eigen.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_stack_crosscov_seed.c,v 1.5 2012/11/14 22:39:14 mcavoy Exp $";

main(int argc,char **argv)
{
char **boldf,**boldidentify=NULL,*mask_file=NULL,*strptr,string[MAXNAME],concoutroot[MAXNAME];
int i,j,k,j1,j2,j3,k1,nconc=0,num_region_files=0,num_regions=0,*roi,maxlik_unbias=1,lcnamesonly=0,lccleanup=0,SunOS_Linux,nbold,
    volreg,nlags=0,*brnidx,nregnames=0,lccrosscorr=0;
float *temp_float;
double *temp_double,*seedstack,*corr,*td,*sd1=NULL,*sd2=NULL;
Files_Struct *conc,*region_files,**concs,*regnames;
Interfile_header *ifh;
Regions **reg;
Regions_By_File *rbf;
Memory_Map *mm;
Dim_Param *dp;
Mask_Struct *ms;
if(argc < 5) {
    fprintf(stderr,"    -conc:                Conc file.\n");
    fprintf(stderr,"    -region_file:         Contain seed regions.\n");
    fprintf(stderr,"    -regions_of_interest: Specify seed regions. First region is one.\n");
    fprintf(stderr,"    -mask:                Only voxels within the mask are analyzed.\n");
    fprintf(stderr,"    -nlags:               eg. 40 lags will yield an 81 pt crosscov.\n");
    fprintf(stderr,"    -names_only:          No computation. Include region names.\n");
    fprintf(stderr,"    -clean_up             Delete files used to compute cross covariance.\n");

    /*START121113*/
    fprintf(stderr,"    -crosscorr            Compute cross-correlation instead of cross-covariance.\n");
    

    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-conc") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nconc;
        if(!(conc=get_files(nconc,&argv[i+1]))) exit(-1);
        i += nconc;
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
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-nlags") && argc > i+1)
        nlags = atoi(argv[++i]);
    if(!strcmp(argv[i],"-names_only") && argc > i+1) {
        lcnamesonly=1;
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nregnames;
        if(!(regnames=get_files(nregnames,&argv[i+1]))) exit(-1);
        i += nregnames;
        }
    if(!strcmp(argv[i],"-clean_up"))
        lccleanup=1;

    /*START121113*/
    if(!strcmp(argv[i],"-crosscorr"))
        lccrosscorr=1;

    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
print_version_number(rcsid,lcnamesonly?stderr:stdout);
if(!nconc) {
    printf("Error: Need to specify -conc\n");
    exit(-1);
    }
strcpy(concoutroot,conc->files[0]);
if(!get_tail_sans_ext(concoutroot)) exit(-1);

/*strcat(concoutroot,"_crosscov");*/
/*START121114*/
strcat(concoutroot,lccrosscorr?"_crosscor":"_crosscov");

if(!num_region_files) {
    printf("Error: Need to specify -region_file\n");
    exit(-1);
    }
if(!nlags) {
    printf("Error: Need to specify -nlags\n");
    exit(-1);
    }
if(!(concs=malloc(sizeof*concs*nconc))) {
    printf("Error: Unable to allocate concs\n");
    exit(-1);
    }
for(i=0;i<nconc;i++) if(!(concs[i]=read_conc(conc->files[i]))) exit(-1);

#if 0
for(nbold=i=0;i<nconc;i++) {
    if(!(concs[i]=read_conc(conc->files[i]))) exit(-1);
    nbold += concs[i]->nfiles;
    }
#endif
/*START121112*/
for(nbold=j=i=0;i<nconc;i++) {
    if(!(concs[i]=read_conc(conc->files[i]))) exit(-1);
    nbold += concs[i]->nfiles;
    if(concs[i]->identify) j++;
    }


if(!(boldf=malloc(sizeof*boldf*nbold))) {
    printf("Error: Unable to allocate boldf\n");
    exit(-1);
    }


#if 0
if(!(boldidentify=malloc(sizeof*boldidentify*nbold))) {
    printf("Error: Unable to allocate boldidentify\n");
    exit(-1);
    }
#endif
/*START121112*/
if(j) {
    if(!(boldidentify=malloc(sizeof*boldidentify*nbold))) {
        printf("Error: Unable to allocate boldidentify\n");
        exit(-1);
        }
    }


for(k=i=0;i<nconc;i++) {
    for(j=0;j<concs[i]->nfiles;j++,k++) {
        boldf[k] = concs[i]->files[j];

        /*boldidentify[k] = concs[i]->identify[j];*/
        /*START121112*/
        if(boldidentify) boldidentify[k] = concs[i]->identify[j];

        }
    }
if(lcnamesonly) {
    for(i=0;i<nregnames;i++) {
        sprintf(string,"%s_%s",concoutroot,regnames->files[i]);
        if(!write_conc(string,nbold,boldidentify,0)) exit(-1);
        }
    exit(0);
    }
if(!(dp=dim_param(nbold,boldf,SunOS_Linux,0))) exit(-1);
if(nlags>(dp->tdim_min-maxlik_unbias)) nlags = dp->tdim_min-maxlik_unbias;
if(!(ms=get_mask_struct(mask_file,dp->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp->vol))) exit(-1);
if(mask_file) {
    if((ms->lenvol!=dp->vol)&&(ms->lenbrain!=dp->vol)) {
        printf("Error: %s and %s not in the same space.\n",conc->files[0],mask_file);
        printf("Error: ms->lenvol=%d dp->vol=%d ms->lenbrain=%d\n",ms->lenvol,dp->vol,ms->lenbrain);
        exit(-1);
        }
    if(ms->lenvol==dp->vol) {
        brnidx = ms->brnidx;
        }
    else {
        if(!(brnidx=malloc(sizeof*brnidx*dp->vol))) {
            printf("Error: Unable to allocate brnidx\n");
            exit(-1);
            }
        for(i=0;i<dp->vol;i++) brnidx[i]=i;
        }
    }
if(!(temp_float=malloc(sizeof*temp_float*ms->lenvol))) {
    printf("Error: Unable to allocate temp_float\n");
    exit(-1);
    }
if(!(ifh = read_ifh(region_files->files[0],(Interfile_header*)NULL))) exit(-1);
if((volreg=ifh->dim1*ifh->dim2*ifh->dim3)!=ms->lenvol) {
    printf("volreg=%d dp->vol=%d  Must be equal.\n",volreg,ms->lenvol);
    exit(-1);
    }
free_ifh(ifh,0);
if(!check_dimensions(num_region_files,region_files->files,volreg)) exit(-1);
if(!(reg=malloc(sizeof*reg*num_region_files))) {
    printf("Error: Unable to malloc reg\n");
    exit(-1);
    }
for(i=0;i<num_region_files;i++) {
    if(!readstack(region_files->files[i],(float*)temp_float,sizeof(float),(size_t)volreg,SunOS_Linux)) exit(-1);
    for(j=0;j<ms->lenbrain;j++) temp_float[j] = temp_float[ms->brnidx[j]];
    if(!(reg[i]=extract_regions(region_files->files[i],0,ms->lenbrain,temp_float,0,SunOS_Linux,(char**)NULL))) exit(-1);
    }
if(!num_regions) for(i=0;i<num_region_files;i++) num_regions += reg[i]->nregions;
if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);


if(!(temp_double=malloc(sizeof*temp_double*ms->lenbrain*dp->tdim_max))) {
    printf("Error: Unable to allocate temp_double\n");
    exit(-1);
    }


/*START121114*/
if(lccrosscorr) {
    if(!(sd1=malloc(sizeof*sd1*1))) {
        printf("Error: Unable to allocate sd1\n");
        exit(-1);
        }
    if(!(sd2=malloc(sizeof*sd2*ms->lenbrain))) {
        printf("Error: Unable to allocate sd2\n");
        exit(-1);
        }
    }


if(!(corr=malloc(sizeof*corr*(2*nlags+1)))) {
    printf("Error: Unable to malloc corr\n");
    exit(-1);
    }
if(!(seedstack=malloc(sizeof*seedstack*num_regions*dp->tdim_max))) {
    printf("Error: Unable to allocate seedstack\n");
    exit(-1);
    }
if(!(td=malloc(sizeof*td*num_regions))) {
    printf("Error: Unable to allocate td\n");
    exit(-1);
    }
for(i=0;i<nbold;i++) {
    for(j=0;j<ms->lenbrain*dp->tdim[i];j++) temp_double[j]=0.;
    printf("Processing %s\n",boldf[i]);
    fflush(stdout);
    if(dp->number_format[i]==(int)FLOAT_IF) { 
        if(!(mm=map_disk(boldf[i],dp->vol*dp->tdim[i],0))) exit(-1);
        }
    else if(dp->number_format[i]==(int)DOUBLE_IF) {
        if(!(mm=map_disk_double(boldf[i],dp->vol*dp->tdim[i],0))) exit(-1);
        }
    else {
        printf("Error: Unrecognized number format dp->number_format[%d]=%d\n",i,dp->number_format[i]);
        printf("Error: %s\n",boldf[i]);
        exit(-1);
        }
    for(j3=j2=j1=j=0;j<dp->tdim[i];j++,j1+=dp->vol,j2+=num_regions,j3+=ms->lenbrain) {
        if(dp->number_format[i]==(int)FLOAT_IF) { 
            for(k=0;k<ms->lenbrain;k++) temp_float[k] = mm->ptr[j1+brnidx[k]];
            if(dp->swapbytes[i]) swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)ms->lenbrain);
            for(k=0;k<ms->lenbrain;k++)
                temp_double[j3+k]=temp_float[k]==(float)UNSAMPLED_VOXEL?(double)UNSAMPLED_VOXEL:(double)temp_float[k];
            }
        else {
            for(k=0;k<ms->lenbrain;k++) temp_double[j3+k] = mm->dptr[j1+brnidx[k]];
            if(dp->swapbytes[i]) swap_bytes((unsigned char *)(&temp_double[j3]),sizeof(double),(size_t)ms->lenbrain);
            }
        crs(&temp_double[j3],td,rbf,(char*)NULL);
        for(k1=j,k=0;k<num_regions;k++,k1+=dp->tdim[i]) seedstack[k1] = td[k];
        }
    if(!unmap_disk(mm)) exit(-1);
    if(!(ifh=read_ifh(boldf[i],(Interfile_header*)NULL))) exit(-1);
    ifh->dim4 = ms->lenbrain;
    ifh->dim1 = 2*nlags+1;
    ifh->dof_condition = (float)(dp->tdim[i]-maxlik_unbias);
    for(j1=j=0;j<num_regions;j++,j1+=dp->tdim[i]) {
        sprintf(string,"%s_%s_b%d.4dfp.img",concoutroot,rbf->region_names_ptr[j],i+1);

        /*crosscov_guts2(string,corr,1,&seedstack[j1],ms->lenbrain,temp_double,dp->tdim[i],nlags,maxlik_unbias);*/
        /*START121114*/
        crosscov_guts3(string,corr,1,&seedstack[j1],ms->lenbrain,temp_double,dp->tdim[i],nlags,maxlik_unbias,sd1,sd2);

        if(!write_ifh(string,ifh,0)) return 0;
        printf("Output written to %s\n",string);
        }
    free_ifh(ifh,0);
    }
for(i=0;i<num_regions;i++) {
    sprintf(string,"%s_%s",concoutroot,rbf->region_names_ptr[i]);
    if(!write_conc(string,nbold,boldidentify,0)) exit(-1);
    }
if(lccleanup) {
    delete_scratch_files(boldf,nbold);
    }
exit(0);
}
