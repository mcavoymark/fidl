/* Copyright 1/25/07 Washington University.  All Rights Reserved.
   compute_histogram_new.c  $Revision: 1.6 $*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_histogram_new.c,v 1.6 2011/02/08 21:16:40 mcavoy Exp $";

main(int argc,char **argv)
{
char *fstat_stem=NULL,*mask_file=NULL,filename[MAXNAME],*zstat_stem=NULL,*stem;
int i,j,k,l,ii,jj,SunOS_Linux,vol,nfiles=0,n_threshold_extent=0,nmax_extent=0,*extent,*max_extent,total,*count_regions,nregions,
    *nvoxels_region; /*swapbytes=0*/
float *temp_float,*actmask,*atlas_z,threshold_float;
double epsilon1=1.,epsilon2=1.,*threshold,*temp_double,*stat,*df1,*df2;
FILE *fp;
Interfile_header *ifh;
Mask_Struct *ms;

print_version_number(rcsid,stdout);
if(argc < 3) {

    /*fprintf(stderr,"    -number_of_fstat_files:\n");*/
    fprintf(stderr,"    -number_of_files:\n");

    fprintf(stderr,"    -fstat_stem:\n");
    fprintf(stderr,"    -zstat_stem:\n");
    fprintf(stderr,"    -mask:                  Limit computation to mask voxels.\n");
    fprintf(stderr,"    -epsilon1:              Correction for numerator degrees of freedom.\n");
    fprintf(stderr,"    -epsilon2:              Correction for denominator degrees of freedom.\n");
    fprintf(stderr,"    -threshold_extent       Threshold with starting extent.\n");
    fprintf(stderr,"                            Ex. -threshold_extent \"3.75 18\" \"4 12\"\n");
    fprintf(stderr,"    -max_extent             One for each threshold_extent.\n");
    /*fprintf(stderr,"    -swapbytes\n");*/
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    /*if(!strcmp(argv[i],"-number_of_fstat_files") && argc > i+1)*/
    if(!strcmp(argv[i],"-number_of_files") && argc > i+1)
        nfiles = atoi(argv[++i]);
    if(!strcmp(argv[i],"-fstat_stem") && argc > i+1)
        fstat_stem =  argv[++i];
    if(!strcmp(argv[i],"-zstat_stem") && argc > i+1)
        zstat_stem =  argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-epsilon1") && argc > i+1)
        epsilon1 = atof(argv[++i]);
    if(!strcmp(argv[i],"-epsilon2") && argc > i+1)
        epsilon2 = atof(argv[++i]);
    if(!strcmp(argv[i],"-threshold_extent") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++n_threshold_extent;
        if(!(threshold=malloc(sizeof*threshold*n_threshold_extent))) {
            printf("Error: Unable to malloc threshold\n");
            exit(-1);
            }
        if(!(extent=malloc(sizeof*extent*n_threshold_extent))) {
            printf("Error: Unable to malloc extent\n");
            exit(-1);
            }
        for(j=0;j<n_threshold_extent;j++) sscanf(argv[++i],"%lf %d",&threshold[j],&extent[j]);
        }
    if(!strcmp(argv[i],"-max_extent") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nmax_extent;
        if(!(max_extent=malloc(sizeof*max_extent*nmax_extent))) {
            printf("Error: Unable to malloc max_extent\n");
            exit(-1);
            }
        for(j=0;j<nmax_extent;j++) max_extent[j] = atoi(argv[++i]);
        }

    #if 0
    if(!strcmp(argv[i],"-swapbytes") && argc > i+1)
        swapbytes=1;
    #endif
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
/*printf("swabbytes=%d Bytes will%s be swapped.\n",swapbytes,!swapbytes?" not":"");*/
if(!nfiles) {
    printf("Error: Need to specify -number_of_fstat_files\n");
    exit(-1);
    }


/*if(!fstat_stem) {
    printf("Error: Need to specify -fstat_stem\n");
    exit(-1);
    }*/
if(!fstat_stem&&!zstat_stem) {
    printf("Error: Need to specify either -fstat_stem or -zstat_stem\n");
    exit(-1);
    }
if(fstat_stem&&zstat_stem) {
    printf("Error: Need to specify either -fstat_stem or -zstat_stem, but no both.\n");
    exit(-1);
    }



if(!n_threshold_extent) {
    printf("Error: Need to specify -threshold_extent\n");
    exit(-1);
    } 
if(n_threshold_extent != nmax_extent) {
    printf("n_threshold_extent=%d nmax_extent=%d  Must be equal!\n",n_threshold_extent,nmax_extent);
    exit(-1);
    }
for(i=0;i<n_threshold_extent;i++) printf("threshold=%f extent=%d max_extent=%d\n",threshold[i],extent[i],max_extent[i]);



/*printf("fstat_stem=%s nfiles=%d\n",fstat_stem,nfiles);
sprintf(filename,"%s0.4dfp.img",fstat_stem);*/

stem = fstat_stem ? fstat_stem : zstat_stem;
printf("stem=%s nfiles=%d\n",stem,nfiles);
sprintf(filename,"%s0.4dfp.img",stem);




if(!(ifh=read_ifh(filename,(Interfile_header*)NULL))) exit(-1);
vol = ifh->dim1*ifh->dim2*ifh->dim3;

if(!(ms=get_mask_struct(mask_file,vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);
printf("vol=%d ms->lenbrain=%d\n",vol,ms->lenbrain);

if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*ms->lenbrain))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }


/*if(!(stat=malloc(sizeof*stat*ms->lenbrain))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }*/
if(fstat_stem) {
    if(!(stat=malloc(sizeof*stat*ms->lenbrain))) {
        printf("Error: Unable to malloc temp_double\n");
        exit(-1);
        }
    }



#if 0
df1 = epsilon1*(double)ifh->dof_condition;
if(!(df2=malloc(sizeof*df2*ms->lenbrain))) {
    printf("Error: Unable to malloc df2\n");
    exit(-1);
    }
for(i=0;i<ms->lenbrain;i++) df2[i] = epsilon2*(double)ifh->dof_error;
printf("ifh->dof_condition = %f   ifh->dof_error = %f\n",ifh->dof_condition,ifh->dof_error);
printf("epsilon1 = %f\n",epsilon1);
printf("epsilon2 = %f\n",epsilon2);
printf("df1=%f df2=%f\n",df1,df2[0]);
#endif

if(fstat_stem) {
    if(!(df1=malloc(sizeof*df1*ms->lenbrain))) {
        printf("Error: Unable to malloc df1\n");
        exit(-1);
        }
    for(i=0;i<ms->lenbrain;i++) df1[i] = epsilon1*(double)ifh->dof_condition;
    if(!(df2=malloc(sizeof*df2*ms->lenbrain))) {
        printf("Error: Unable to malloc df2\n");
        exit(-1);
        }
    for(i=0;i<ms->lenbrain;i++) df2[i] = epsilon2*(double)ifh->dof_error;
    printf("ifh->dof_condition = %f   ifh->dof_error = %f\n",ifh->dof_condition,ifh->dof_error);
    printf("epsilon1 = %f\n",epsilon1);
    printf("epsilon2 = %f\n",epsilon2);
    printf("df1=%f df2=%f\n",df1[0],df2[0]);
    }



if(!(atlas_z=malloc(sizeof*atlas_z*vol))) {
    printf("Error: Unable to malloc atlas_z\n");
    exit(-1);
    }
for(i=0;i<vol;i++) atlas_z[i]=0.;
if(!(actmask=malloc(sizeof*actmask*vol))) {
    printf("Error: Unable to malloc actmask\n");
    exit(-1);
    }
for(total=i=0;i<n_threshold_extent;i++) total += max_extent[i]-extent[i]+1;
if(!(count_regions=malloc(sizeof*count_regions*total))) {
    printf("Error: Unable to malloc count_regions\n");
    exit(-1);
    }
for(i=0;i<total;i++) count_regions[i]=0;
printf("total=%d\n",total);

#if 0
for(i=0;i<nfiles;i++) {
    sprintf(filename,"%s%d.4dfp.img",fstat_stem,i);
    if(!(fp=fopen_sub(filename,"r"))) exit(-1);
    fclose(fp);
    }
#endif
for(i=0;i<nfiles;i++) {

    sprintf(filename,"%s%d.4dfp.img",stem,i);
    if(!readstack(filename,(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    for(j=0;j<ms->lenbrain;j++) temp_double[j] = temp_float[ms->brnidx[j]]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL :
        (double)temp_float[ms->brnidx[j]];



    /*if(!f_to_z(temp_double,stat,ms->lenbrain,df1,df2)) exit(-1);*/
    if(fstat_stem) {
        if(!f_to_z(temp_double,stat,ms->lenbrain,df1,df2)) exit(-1);
        }
    else {
        stat = temp_double;
        }



    for(j=0;j<ms->lenbrain;j++) atlas_z[ms->brnidx[j]] = (float)stat[j];
    for(jj=j=0;j<n_threshold_extent;j++) {
        for(k=0;k<vol;k++) actmask[k] = 0.;
        threshold_float = (float)threshold[j];
        if((nregions=spatial_extent(atlas_z,actmask,ifh->dim1,ifh->dim2,ifh->dim3,&threshold_float,1,&extent[j],&vol,1))==-1) 
            exit(-1);
        /*printf("threshold=%f extent=%d nregions = %d\n",threshold_float,extent[j],nregions);*/

        if(!(nvoxels_region=malloc(sizeof*nvoxels_region*nregions))) {
            printf("Error: Unable to malloc nvoxels_region\n");
            return 0;
            }
        for(k=0;k<nregions;k++) nvoxels_region[k]=0;
        for(k=0;k<ms->lenbrain;k++) for(l=0;l<nregions;l++) if((int)actmask[ms->brnidx[k]] == l+2) nvoxels_region[l]++;

        count_regions[jj++] += nregions; 
        for(k=extent[j]+1;k<=max_extent[j];k++,jj++) {
            for(l=0;l<nregions;l++) if(nvoxels_region[l]>=k) count_regions[jj]++;
            }
        }
    free(nvoxels_region);

    #if 0
        for(jj=j=0;j<n_threshold_extent;j++) {
            for(k=extent[j];k<=max_extent[j];k++,jj++) {
                printf("threshold=%f extent=%d nregions=%d\n",threshold[j],k,count_regions[jj]);
                }
            }
    #endif
    }
printf("nfiles=%d\n",nfiles);
for(ii=i=0;i<n_threshold_extent;i++) {
    for(j=extent[i];j<=max_extent[i];j++,ii++) {
        printf("threshold=%f extent=%d nregions=%d\n",threshold[i],j,count_regions[ii]);
        }
    }
}
