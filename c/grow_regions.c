/* Copyright 3/13/01 Washington University.  All Rights Reserved.
   grow_regions.c  $Revision: 1.18 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

/*START140723*/
#include <gsl/gsl_cdf.h>

#define DIAMETER 200

main(int argc,char **argv)
{
char filename[MAXNAME],*mask_file,*AND_name=NULL,*Nimage_name=NULL,*subject_contributions_name=NULL,*atlas=NULL;

int i,j,k,m,vol,extent=1,fidl_coordinates[3],atlas_coordinates[3],num_mcomp_files=0,num_af3d_files=0,num_region_filenames=0,
    num_sig_reg,index,index_region,index_voxel,*mcomp_or_region,**region_stacks,**masks,*region_count,*subjects,SunOS_Linux,
    big_endian=1,swapbytes;

float threshold=0,thresholdp_float=.5,*actmask,*temp_float,*atlasmask,min,max,diameter=(float)DIAMETER,*limiting_sphere;

double *zstat,*sigprob;

FILE *fp;
Interfile_header *ifh;
Regions **reg;
Af3d **af3d;
Atlas_Param *ap;
Files_Struct *mcomp_files,*af3d_files,*region_filenames;

if (argc < 9) {
    fprintf(stderr,"Usage: grow_regions $MCOMP_FILES $REGION_FILENAMES $SEEDS $REGIONS $NIMAGE $AND\n");
    fprintf(stderr,"        -mcomp_files:           Multiple comparison corrected zmaps or region files.\n");
    fprintf(stderr,"        -af3d_files:            One for each mcomp_file. Can be an af3d file or a simple list of points.\n");
    fprintf(stderr,"        -region_filenames:      Names of desired region files to be made. One for each mcomp file.\n"); 
    fprintf(stderr,"        -AND:                   Region file showing common areas for the -mcomp_files.\n");
    fprintf(stderr,"        -Nimage:                Number of subjects at each voxel for the -mcomp_files.\n");
    fprintf(stderr,"        -subject_contributions: Text file showing which subjects contributed an acitivated voxel.\n");
    fprintf(stderr,"        -threshold:             Atlas coordinates whose value are less than this threshold are ignored.\n");
    fprintf(stderr,"                                The default is 0.\n");
    fprintf(stderr,"        -mask:                  Limits the extent of the grown regions.\n");
    fprintf(stderr,"        -diameter_in_mm:        Region is not grown beyond a sphere of this size.\n");
    fprintf(stderr,"                                Sphere is centered at the seed point.\n");
    fprintf(stderr,"        -little_endian          Data is stored in little endian.\n");
    exit(-1);
    }

for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-mcomp_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_mcomp_files;
        if(!(mcomp_files=get_files(num_mcomp_files,&argv[i+1]))) exit(-1);
        i += num_mcomp_files;
        }
    if(!strcmp(argv[i],"-af3d_files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_af3d_files;
        if(!(af3d_files=get_files(num_af3d_files,&argv[i+1]))) exit(-1);
        i += num_af3d_files;
        }
    if(!strcmp(argv[i],"-region_filenames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_filenames;
        if(!(region_filenames=get_files(num_region_filenames,&argv[i+1]))) exit(-1);
        i += num_region_filenames;
        }
    if(!strcmp(argv[i],"-threshold") && argc > i+1) {
        threshold = atof(argv[++i]);
        }
    if(!strcmp(argv[i],"-AND") && argc > i+1) {
        strcpy(filename,argv[++i]);
        if(!strstr(filename,".4dfp.img")) strcat(filename,".4dfp.img");
        if(!(AND_name=malloc(sizeof*AND_name*(strlen(filename)+1)))) {
            printf("Error: Unable to malloc AND_name\n");
            exit(-1);
            }
        strcpy(AND_name,filename);
        }
    if(!strcmp(argv[i],"-Nimage") && argc > i+1) {
        strcpy(filename,argv[++i]);
        if(!strstr(filename,".4dfp.img")) strcat(filename,".4dfp.img");
        if(!(Nimage_name=malloc(sizeof*Nimage_name*(strlen(filename)+1)))) {
            printf("Error: Unable to malloc AND_name\n");
            exit(-1);
            }
        strcpy(Nimage_name,filename);
        }
    if(!strcmp(argv[i],"-subject_contributions") && argc > i+1)
        subject_contributions_name = argv[++i]; 
    if(!strcmp(argv[i],"-mask") && argc>i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i]; 
    if(!strcmp(argv[i],"-diameter_in_mm") && argc > i+1)
        diameter = atof(argv[++i]);
    if(!strcmp(argv[i],"-little_endian"))
        big_endian = 0;
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
swapbytes = shouldiswap(SunOS_Linux,big_endian);
printf("threshold applied to af3d files = %f\nmask : %s\n\n",threshold,mask_file);
if(num_mcomp_files != num_af3d_files) {
    printf("num_mcomp_files = %d  num_af3d_files = %d  Should be equal. Abort!\n");
    exit(-1);
    }

if(!(mcomp_or_region=malloc(sizeof*mcomp_or_region*num_mcomp_files))) {
    printf("Error: Unable to malloc mcomp_or_region\n");
    exit(-1);
    }
for(i=0;i<num_mcomp_files;i++) {
    if(!(ifh=read_ifh(mcomp_files->files[i],(Interfile_header*)NULL))) exit(-1);
    mcomp_or_region[i] = !ifh->nregions ? 0 : 1;
    if(i != num_mcomp_files-1) free_ifh(ifh,0);
    }
vol = ifh->dim1*ifh->dim2*ifh->dim3;
if(!check_dimensions(num_mcomp_files,mcomp_files->files,vol)) exit(-1);

atlas = get_atlas(vol);
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(diameter < (float)DIAMETER) {
    if(!atlas) {
        printf("Error: Voxels need to be isotropic to utilize a limiting sphere. Abort!\n");
        exit(-1);
        }
    }

/*Convert mcomp_files to region files. Read in region files.*/
if(!(zstat=malloc(sizeof*zstat*vol))) {
    printf("Error: Unable to malloc zstat\n");
    exit(-1);
    }
if(!(sigprob=malloc(sizeof*sigprob*vol))) {
    printf("Error: Unable to malloc sigprob\n");
    exit(-1);
    }
if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(actmask=malloc(sizeof*actmask*vol))) {
    printf("Error: Unable to malloc actmask\n");
    exit(-1);
    }
if(!(atlasmask=malloc(sizeof*atlasmask*vol))) {
    printf("Error: Unable to malloc atlasmask\n");
    exit(-1);
    }
if(!(reg=malloc(sizeof*reg*num_mcomp_files))) {
    printf("Error: Unable to malloc reg\n");
    exit(-1);
    }
if(!(region_stacks = (int **)d2int(num_mcomp_files,vol))) exit(-1);
if(!(masks = (int **)d2int(num_mcomp_files,vol))) exit(-1);
if(!readstack(mask_file,(float*)atlasmask,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
for(i=0;i<num_mcomp_files;i++) {
    if(!readstack(mcomp_files->files[i],(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    if(mcomp_or_region[i]) {
        for(j=0;j<vol;j++) region_stacks[i][j] = (int)temp_float[j];
        /*if(!(reg[i]=extract_regions(mcomp_files->files[i],0,0,(float*)NULL,0,swapbytes))) exit(-1);*/
        if(!(reg[i]=extract_regions(mcomp_files->files[i],0,0,(float*)NULL,0,SunOS_Linux,(char**)NULL))) exit(-1);
        }
    else {

        #if 0
        for(j=0;j<vol;j++) zstat[j] = (double)temp_float[j];
        z_to_sigprob(zstat,sigprob,vol);
        #endif
        /*START140723*/
        for(j=0;j<vol;j++) {
            zstat[j] = (double)temp_float[j];
            sigprob[j] = gsl_cdf_ugaussian_Q(zstat[j]);
            }

        for(j=0;j<vol;j++) {
            temp_float[j] = atlasmask[j] ? (float)sigprob[j] : 1;
            actmask[j] = 0;
            }

        num_sig_reg = spatial_extent(temp_float,actmask,ifh->dim1,ifh->dim2,ifh->dim3,&thresholdp_float,1,&extent,&vol,0);

        /*printf("num_sig_reg = %d\n",num_sig_reg);*/
        if(!num_sig_reg) {
            printf("No regions found in %s  Abort!\n",mcomp_files->files[i]);
            exit(-1);
            }
        for(j=0;j<vol;j++) region_stacks[i][j] = (int)actmask[j];

        /*reg[i] = (Regions *)extract_regions_guts(num_sig_reg,vol,actmask,(int)FALSE);*/
        /*START140723*/
        reg[i] = extract_regions_guts(num_sig_reg,vol,actmask,0,(int*)NULL);

        }
    }
free(zstat);
free(sigprob);
free(atlasmask);


if(!(region_count=malloc(sizeof*region_count*num_mcomp_files))) {
    printf("Error: Unable to malloc region_count\n");
    exit(-1);
    }
for(i=0;i<num_mcomp_files;i++) region_count[i]=0;
if(!(af3d=malloc(sizeof*af3d*num_mcomp_files))) {
    printf("Error: Unable to malloc af3d\n");
    exit(-1);
    }
if(!(limiting_sphere=malloc(sizeof*limiting_sphere*vol))) {
    printf("Error: Unable to malloc limiting_sphere\n");
    exit(-1);
    }


if(diameter >= (float)DIAMETER) for(i=0;i<vol;i++) limiting_sphere[i] = 1;
for(i=0;i<num_mcomp_files;i++) {
    if(!(af3d[i]=read_af3d(af3d_files->files[i],ap,threshold,"af3d"))) exit(-1);
    printf("mcomp_file : %s\naf3d_file : %s\n",mcomp_files->files[i],af3d_files->files[i]);
    for(j=0;j<af3d[i]->nindex;j++) {
        if(diameter < (float)DIAMETER) {
            for(k=0;k<vol;k++) limiting_sphere[k] = 0;
            make_sphere(af3d[i]->x[j],af3d[i]->y[j],af3d[i]->z[j],limiting_sphere,1.,ifh->dim1,ifh->dim2,ifh->dim3,diameter,
                ap->voxel_size[0],(FILE*)NULL);

            /*for(m=k=0;k<vol;k++) if(limiting_sphere[k]) m++;
            printf("diameter=%f m=%d\n",diameter,m);*/
            }
        index = af3d[i]->index[j];
        if(region_stacks[i][index] < 2) {
            printf("    seed = (%.1f %.1f %.1f) is not an activated voxel.\n",af3d[i]->coordinates[j][0],
                af3d[i]->coordinates[j][1],af3d[i]->coordinates[j][2]);
            }
        else {
            if(masks[i][index]) {
                printf("    seed = (%.1f %.1f %.1f) is already a voxel in a region grown from seed = (%.1f %.1f %.1f).\n",
                    af3d[i]->coordinates[j][0],af3d[i]->coordinates[j][1],af3d[i]->coordinates[j][2],
                    af3d[i]->coordinates[masks[i][index]-2][0],af3d[i]->coordinates[masks[i][index]-2][1],
                    af3d[i]->coordinates[masks[i][index]-2][2]);
                }
            else {
                index_region = region_stacks[i][index]-2;
                for(k=0;k<reg[i]->nvoxels_region[index_region];k++) {
                    index_voxel = reg[i]->voxel_indices[index_region][k];
                    if(!masks[i][index_voxel]) {
                        /*printf("here0\n");*/
                        if(limiting_sphere[index_voxel]) {
                            /*printf("here1\n");*/
                            masks[i][index_voxel] = region_count[i]+2;
                            }
                        }
                    else {
                        voxel_index_to_fidl_and_atlas(index_voxel,ifh->dim1,ifh->dim2,ifh->dim3,ap->center,ap->mmppix,
                            atlas_coordinates,fidl_coordinates);
                        printf("    seed = (%.1f %.1f %.1f). Voxel already belongs to a region grown from seed = (%.1f %.1f %.1f).\n",
                            af3d[i]->coordinates[j][0],af3d[i]->coordinates[j][1],af3d[i]->coordinates[j][2],
                            af3d[i]->coordinates[masks[i][index_voxel]-2][0],af3d[i]->coordinates[masks[i][index_voxel]-2][1],
                            af3d[i]->coordinates[masks[i][index_voxel]-2][2]);
                        fprintf(stdout,"        Voxel fidl coordinates: slice = %d (col,row) = (%d,%d)\n",fidl_coordinates[2],
                            fidl_coordinates[0],fidl_coordinates[1]);
                        fprintf(stdout,"        Voxel atlas coordinates: (x y z) = %d %d %d\n\n",atlas_coordinates[0],
                            atlas_coordinates[1],atlas_coordinates[2]);
                        }
                    }
                region_count[i]++;
                }
            }
        }
    printf("\n");
    }

if(AND_name!=(char*)NULL || Nimage_name!=(char*)NULL) {

    /*GETMEM(subjects,num_mcomp_files,int);*/
    if(!(subjects=malloc(sizeof*subjects*num_mcomp_files))) {
        printf("Error: Unable to malloc subjects\n");
        exit(-1);
        }

    for(i=0;i<vol;i++) actmask[i] = temp_float[i] = 0;
                          /*Nimage        AND*/

    if(subject_contributions_name) {
        if(!(fp=fopen(subject_contributions_name,"w"))) {
            fprintf(stdout,"Error: Could not open %s for writing.\n",subject_contributions_name);
            fprintf(stdout,"Error: Do you have write permission in this directory? Are you out of memory?\n");
            exit(-1);
            }
        fprintf(fp,"Nimage : %s\n\n",Nimage_name);
        for(i=0;i<num_mcomp_files;i++) fprintf(fp,"%-4d%s\n",i+1,mcomp_files->files[i]);
        fprintf(fp,"\n");
        fprintf(fp,"fidl coordinates atlas coordinates\n");
        fprintf(fp,"slice (row,col)  x   y   z   subjects\n");
        fprintf(fp,"-----------------------------------------------\n");
        }

    for(i=0;i<vol;i++) {
        for(j=0;j<num_mcomp_files;j++) subjects[j] = 0;
        for(j=0;j<num_mcomp_files;j++) {
            subjects[j] = masks[j][i] ? 1 : 0;
            if(masks[j][i]) {
                if(!actmask[i] && subject_contributions_name) {
                    voxel_index_to_fidl_and_atlas(i,ifh->dim1,ifh->dim2,ifh->dim3,ap->center,ap->mmppix,atlas_coordinates,
                        fidl_coordinates);
                    fprintf(fp,"%-4d  (%d,%d)   %3d %3d %3d    ",fidl_coordinates[2],fidl_coordinates[0],fidl_coordinates[1],
                        atlas_coordinates[0],atlas_coordinates[1],atlas_coordinates[2]);
                        }
                actmask[i]++;
                }
            }
        /*if(subject_contributions_name) {*/
        if(actmask[i] && subject_contributions_name) {
            for(j=0;j<num_mcomp_files;j++) if(subjects[j]) fprintf(fp,"%d ",j+1);
            fprintf(fp,"\n");
            }
        }
    if(subject_contributions_name) {
        fclose(fp);
        fprintf(stdout,"Subject contributions text files written to %s\n",subject_contributions_name);
        }
    for(i=0;i<vol;i++) {
        if((int)actmask[i] == num_mcomp_files) temp_float[i] = 2;
        }
    }

if(AND_name!=(char*)NULL) {
    if(!writestack(AND_name,(float*)temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
    assign_region_names(ifh,temp_float,vol,1,(double*)NULL,(double*)NULL,(float*)NULL);
    ifh->global_min = 0;
    ifh->global_max = 2;
    if(!write_ifh(AND_name,ifh,(int)FALSE)) exit(-1);

    /*free_ifh(ifh,(int)TRUE);*/ /*Only region names are removed.*/
    free_ifhregnames(ifh);

    fprintf(stdout,"AND image written to %s\n",AND_name);
    }
if(Nimage_name!=(char*)NULL) {
    /*if(!write_float(Nimage_name,actmask,vol)) exit(-1);*/
    if(!writestack(Nimage_name,(float*)actmask,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
    min_and_max(actmask,vol,&ifh->global_min,&ifh->global_max);
    if(!write_ifh(Nimage_name,ifh,(int)FALSE)) exit(-1);
    fprintf(stdout,"Nimage image written to %s\n",Nimage_name);
    }
for(i=0;i<num_mcomp_files;i++) {
    for(j=0;j<vol;j++) temp_float[j] = (float)masks[i][j];
    /*if(!write_float(region_filenames->files[i],temp_float,vol)) exit(-1);*/
    if(!writestack(region_filenames->files[i],(float*)temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
    assign_region_names(ifh,temp_float,vol,region_count[i],(double*)NULL,(double*)NULL,(float*)NULL);
    ifh->global_min = 0;
    ifh->global_max = region_count[i] + 2;
    if(!write_ifh(region_filenames->files[i],ifh,(int)FALSE)) exit(-1);

    /*free_ifh(ifh,(int)TRUE);*/ /*Only region names are removed.*/
    free_ifhregnames(ifh);

    fprintf(stdout,"Region file written to %s\n",region_filenames->files[i]);
    }
printf("\nDONE\n");
}
