/* Copyright 9/2/21 Washington University.  All Rights Reserved.
   find_regions_by_file_cover3.h  $Revision: 1.1 $*/
#ifndef __FIND_REGIONS_BY_FILE_COVER_H__
    #define __FIND_REGIONS_BY_FILE_COVER_H__
 
    //#include "region.h"
    //START210901
    #include "region3.h"
    #include "files_struct.h"

    typedef struct {
        int **roi_by_file,*num_regions_by_file,*nvoxels_region,*length,*indices,*harolds_num,max_length,nvoxels,nreg,*file_index,
            nvoxels_region_max,*roi_index,*indices_uniqsort,nindices_uniqsort,num_regions_by_file_max,c_orient[3];
        char **region_names_ptr;
        } Regions_By_File;

#if 0
    #ifdef __cplusplus
        extern "C" {
    #endif
#endif


    #if 0
    Regions_By_File *find_regions_by_file_cover(int num_region_files,int num_regions,Regions **reg,int *roi);
    #endif
    //START210901
    Regions_By_File *find_regions_by_file_cover(int num_region_files,int num_regions,Regions **reg,int *roi,Files_Struct* roistr);
    Regions_By_File *init_rbf(int num_region_files,int num_regions);


    Regions_By_File *find_regions_by_file(int num_region_files,int num_regions,int *nregions,int *roi,Regions_By_File *rbf);

    
    //START210901
    Regions_By_File *find_regions_by_file_roistr(int num_region_files,int num_regions,Regions **reg,Files_Struct* roistr,Regions_By_File *rbf);


    void free_regions_by_file(Regions_By_File *rbf);
    int crs(double *temp_double,double *region_stack,Regions_By_File *rbf,char *glmfile);

    //START211103
    int crs_rms(double *temp_double,double *region_stack,Regions_By_File *rbf,char *glmfile);

                                                                                            /*rbf->indices*/
    void crsw(double *temp_double,double *region_stack,Regions_By_File *rbf,double *weights,int *weightsi);

#if 0
    #ifdef __cplusplus
        }//extern
    #endif
#endif

#endif
