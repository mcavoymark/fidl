/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   compute_temporal_cov_111.c  $Revision: 1.11 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/compute_temporal_cov_111.c,v 1.11 2007/01/24 19:49:40 mcavoy Exp $";

main(int argc,char **argv)
{
char *xform_file=NULL,*directory=NULL,string[MAXNAME],write_back[MAXNAME],*str_ptr,scratchfile[100],*concfile=NULL;

int i,j,k,l,m,n,p,q,ll,skip=0,num_bold_files=0,num_region_files=0,num_regions=0,*roi=NULL,
    tdim_usable,A_or_B_or_U,index,count,atlas=222,avi_seg=0,
    index_x,index_y,index_sumx,index_sumy,*sampled_per_frame,*sampled_per_region,SunOS_Linux;

float *t4,*cov,*temp_float;

double *temp_double,*stat,*sum,sum2;

FILE *fp,*op;
Interfile_header *ifh;
Memory_Map *mm;
Regions **reg;
Regions_By_File *rbf;
Atlas_Param *ap;
Files_Struct *region_files,*bold_files;
Dim_Param *dp;

print_version_number(rcsid,stdout);
if (argc < 5) {
    fprintf(stderr,"compute_temporal_cov\n");
    fprintf(stderr,"Separate file produced for each BOLD, dimension = valid_frams*valid_frames x num_regions\n");
    fprintf(stderr,"        -bold_files:          List of bold runs.\n");
    fprintf(stderr,"        -region_file:         *.4dfp.img file that specifies regions of interest.\n");
    fprintf(stderr,"        -regions_of_interest: Calculate statistics for selected regions in the region file.\n");
    fprintf(stderr,"                              First region is one.\n");
    fprintf(stderr,"        -avi_seg              Region file is an Avi segmented image. (1000=CSF 2000=gray 3000=white)\n");
    fprintf(stderr,"                              Note: You should also specify -atlas as 111 if applicable.\n");
    fprintf(stderr,"        -xform_file:          Name of 2A_t4 or 2B_t4 file defining transform to atlas space.\n");
    fprintf(stderr,"        -atlas:               Either 111, 222 or 333. Default is 222. Used with -xform_files option.\n");
    fprintf(stderr,"        -directory:           Specify output storage path.\n");
    fprintf(stderr,"        -skip:                Skip this number of initial frames. Default is zero.\n");
    fprintf(stderr,"        -conc:                Name of conc file. Include path as path specified with -directory does not apply.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-bold_files") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++num_bold_files;
        if(!(bold_files=get_files(num_bold_files,&argv[i+1]))) exit(-1);
        i += num_bold_files;
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
    if(!strcmp(argv[i],"-avi_seg"))
        avi_seg = 1;
    if(!strcmp(argv[i],"-xform_file") && argc > i+1)
        xform_file = argv[++i];
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        atlas = atoi(argv[++i]);
    if(!strcmp(argv[i],"-directory") && argc > i+1)
        directory = argv[++i];
    if(!strcmp(argv[i],"-skip") && argc > i+1)
        skip = atoi(argv[++i]);
    if(!strcmp(argv[i],"-conc") && argc > i+1)
        concfile = argv[++i];
    }
if(!num_bold_files) {
    printf("Error: No bold files. Abort!\n");
    exit(-1);
    }
if(!num_region_files) {
    printf("Error: No region file specified. Abort!\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(dp=dim_param(num_bold_files,bold_files->files))) exit(-1);
if(!xform_file) atlas=get_atlas(dp->vol);
if(!(ap=get_atlas_param(atlas))) exit(-1);
if(!xform_file) {
    ap->vol = dp->vol;
    }
else {
    if(!(t4=malloc(sizeof*t4*(size_t)T4SIZE))) {
        printf("Error: Unable to malloc t4\n");
        exit(-1);
        }
    if(!read_xform(xform_file,t4)) exit(-1);
    if((A_or_B_or_U=twoA_or_twoB(xform_file)) == 2) {
        fprintf(stdout,"Error: Illegally named t4 file.\n");
        exit(-1);
        }
    }
if(!(stat=malloc(sizeof*stat*ap->vol))) {
    printf("Error: Unable to malloc stat\n");
    exit(-1);
    }
if(!(temp_double=malloc(sizeof*temp_double*ap->vol))) {
    printf("Error: Unable to malloc temp_double\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*dp->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!check_dimensions(num_region_files,region_files->files,ap->vol)) exit(-1);
if(!(reg=malloc(sizeof*reg*num_region_files))) {
    printf("Error: Unable to malloc reg\n");
    exit(-1);
    }
for(m=0;m<num_region_files;m++) if(!(reg[m]=extract_regions(region_files->files[m],avi_seg,0,(float*)NULL,0,SunOS_Linux))) exit(-1);
if(!num_regions) for(m=0;m<num_region_files;m++) num_regions += reg[m]->nregions;
if(!(rbf=find_regions_by_file_cover(num_region_files,num_regions,reg,roi))) exit(-1);

    /*              dp->vol
          ----------------------------
          |                          |
          ----------------------------
          |                          |
          ----------------------------
    time  |      bold stack          |
          ----------------------------
          |                          |
          ----------------------------
          |                          |
          ----------------------------
          |                          |
          ----------------------------
    */
printf("Regions comprise a total of %d voxels\n",rbf->nvoxels);
if(!(sampled_per_region=malloc(sizeof*sampled_per_region*num_regions))) {
    printf("Error: Unable to malloc sampled_per_region\n");
    exit(-1);
    }
if(!(sampled_per_frame=malloc(sizeof*sampled_per_frame*dp->tdim_max))) {
    printf("Error: Unable to malloc sampled_per_frame\n"); 
    exit(-1);
    }
if(!(sum=malloc(sizeof*sum*dp->tdim_max*num_regions))) {
    printf("Error: Unable to malloc sum\n");
    exit(-1);
    }
if(!(cov=malloc(sizeof*cov*num_regions*dp->tdim_max*dp->tdim_max))) {
    printf("Error: Unable to malloc cov\n");
    exit(-1);
    }

if(concfile) {
    if(!(op=fopen_sub(concfile,"w"))) exit(-1);
    fprintf(op,"    number_of_files:%d\n",num_bold_files);
    }
sprintf(scratchfile,"fidlscratch_%s.4dfp.img",make_timestr());
for(i=0;i<num_bold_files;i++) {
    printf("Processing %s\n",bold_files->files[i]);
    tdim_usable = dp->tdim[i] - skip;
    if(!(mm=map_disk(bold_files->files[i],dp->vol*dp->tdim[i],0))) exit(-1);
    if(!(fp=fopen_sub(scratchfile,"w"))) exit(-1);
    for(j=0;j<tdim_usable;j++) {
        ll = dp->vol*(j+skip);
        if(!SunOS_Linux) {
            for(k=0;k<dp->vol;k++) temp_double[k] = mm->ptr[ll+k]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL :
                (double)mm->ptr[ll+k];
            }
        else {
            for(k=0;k<dp->vol;k++) temp_float[k] = mm->ptr[ll+k];
            swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)dp->vol);
            for(k=0;k<dp->vol;k++) temp_double[k] = temp_float[k]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL :
                (double)temp_float[k];
            }
        if(xform_file) {
            if(!t4_atlas(temp_double,stat,t4,dp->xdim,dp->ydim,dp->zdim,dp->dxdy,dp->dz,A_or_B_or_U,dp->orientation,ap)) exit(-1);
            for(k=0;k<ap->vol;k++) temp_double[k] = stat[k];
            }
        sampled_per_frame[j] = 0;
        for(n=k=0;k<rbf->nreg;k++) {
            sampled_per_region[k] = 0;
            for(p=0;p<rbf->nvoxels_region[k];p++,n++) {
                if(temp_double[rbf->indices[n]] != (double)UNSAMPLED_VOXEL) {
                    stat[sampled_per_frame[j]++] = temp_double[rbf->indices[n]];
                    sampled_per_region[k]++;
                    }
                }
            }
        if(!fwrite_sub(stat,sizeof(double),(size_t)sampled_per_frame[j],fp,0)) {
            fprintf(stdout,"Error: Could not write to %s\n",scratchfile);
            fprintf(stdout,"Error: wrote %d  expecting to write %d\n",count,sampled_per_frame[j]);
            exit(-1);
            }
        }
    fclose(fp);
    if(!unmap_disk(mm)) exit(-1);

    /*printf("Bold file: %s\nNumber of sampled voxels within the regions for each frame.\n",bold_files->files[i]);*/
    count = sampled_per_frame[0];
    for(k=j=0;j<tdim_usable;j++) {
        if(sampled_per_frame[j] != count) k = 1;
        /*printf("%5d  %10d\n",j+skip+1,sampled_per_frame[j]);*/
        }
    if(!k) {
        /*printf("All is well. We have consistency. Will use the %d sampled voxels.\n\n",count);*/
        printf("All is well. We have consistency. Will use the %d sampled voxels.\n",count);
        }
    else {
        printf("Number of sampled voxels not consistent. Abort!\n");
        if(remove(scratchfile)) printf("Error: Unable to remove %s\n",scratchfile);
        exit(-1);
        } 

    #if 0
    printf("sampled voxels for each region\n");
    for(j=0;j<num_regions;j++) {
        sprintf(string,"%d",j); 
        printf("%s %d\n",!avi_seg?rbf->region_names_ptr[j]:string,sampled_per_region[j]);
        }
    #endif

    if(!(mm = map_disk_double(scratchfile,sampled_per_frame[0]*tdim_usable,0))) exit(-1);
    for(count=n=j=0;j<tdim_usable;j++) {
        for(k=0;k<num_regions;k++,n++) {
            for(sum[n]=0.,m=0;m<sampled_per_region[k];m++,count++) {
                sum[n] += mm->dptr[count];
                }
            }
        }
    for(j=0;j<num_regions*tdim_usable*tdim_usable;j++) cov[j]=(float)UNSAMPLED_VOXEL;
    ll = tdim_usable*tdim_usable;
    for(j=0;j<tdim_usable;j++) {
        for(m=0;m<tdim_usable;m++) {
            if(m < j) {
                for(k=0;k<num_regions;k++) {
                    l = ll*k;
                    p = l+j*tdim_usable+m;
                    q = l+m*tdim_usable+j;
                    cov[p] = cov[q];
                    }
                }
            else {
                index_x = j*sampled_per_frame[0];
                index_y = m*sampled_per_frame[0];
                index_sumx = j*num_regions;
                index_sumy = m*num_regions;
                for(k=0;k<num_regions;k++,index_sumx++,index_sumy++) {
                    l = ll*k;
                    p = l+j*tdim_usable+m;
                    for(sum2=0.,n=0;n<sampled_per_region[k];n++,index_x++,index_y++) {
                        sum2 += mm->dptr[index_x]*mm->dptr[index_y];
                        }
                    if(sampled_per_region[k]>1) cov[p] = (float)((sum2-sum[index_sumx]*sum[index_sumy]/sampled_per_region[k]) /
                        (sampled_per_region[k]-1));
                    }
                }
            }
        }
    if(!unmap_disk(mm)) exit(-1);
    if(remove(scratchfile)) printf("Error: Unable to remove %s\n",scratchfile);

    strcpy(write_back,bold_files->files[i]);
    if(!(str_ptr=get_tail_sans_ext(write_back))) exit(-1);
    string[0] = 0;
    if(directory) strcat(string,directory);
    strcat(string,str_ptr);

    sprintf(write_back,"%s_temporal_cov_%dx.4dfp.img",string,skip);
    if(!writestack(write_back,cov,sizeof(float),num_regions*tdim_usable*tdim_usable,SunOS_Linux)) exit(-1);

    if(!(ifh = read_ifh(bold_files->files[i]))) exit(-1);
    min_and_max(cov,num_regions*tdim_usable*tdim_usable,&ifh->global_min,&ifh->global_max);
    ifh->dim1 = ifh->dim2 = tdim_usable;
    ifh->dim3 = num_regions;
    ifh->dim4 = 1;
    if(!write_ifh(write_back,ifh,(int)FALSE)) exit(-1);
    free_ifh(ifh,(int)FALSE);
    printf("Output written to %s\n",write_back);
    fprintf(op,"               file:%s\n",write_back);
    }
fclose(op);
printf("Conc written to %s\n",concfile);
} /*END*/
