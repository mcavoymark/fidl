/* Copyright 3/16/18 Washington University.  All Rights Reserved.
   region.h  $Revision: 1.1 $*/

//#include <cstdint>
#include <stdint.h>
#include <stddef.h>
#include "files_struct.h" 

#ifndef __REGION_H__
    #define __REGION_H__

    typedef struct {
        int     nregions,vol,*nvoxels_region,*harolds_num,**voxel_indices,c_orient[3],nvoxels; //nvoxels added 200224
        size_t *length;
        char    **region_names;
        } Regions;

    #ifdef __cplusplus
        extern "C" {
    #endif

    //START190328
    int shouldiflip(Files_Struct* files);

    #if 0
    Regions *get_reg(char *region_file,int fidl_aviseg_gt0,int vol,float *image,int nreg,int SunOS_Linux,char **ifh_region_names);
    Regions *get_reg_guts(int nregions,int lenvol,float *image,int fidl_aviseg_gt0,int *reg_val);
    #endif
    //START190306
    //Regions *get_reg(char *region_file,int vol,float *image,int nreg,int SunOS_Linux,char **ifh_region_names);
    //190327
    //Regions *get_reg(char *region_file,int vol,float *image,int nreg,int SunOS_Linux,char **ifh_region_names,int lcflip);
    //190909
    //Regions *get_reg(char *region_file,int vol,float *image,int nreg,char **ifh_region_names,int lcflip,int lcnamesonly);
    //START190926
    //Regions *get_reg(char *region_file,int vol,void *image,int nreg,char **ifh_region_names,int lcflip,int lcnamesonly);
    //START200107
    //Regions *get_reg(char *region_file,int vol,void *image,int nreg,char **ifh_region_names,int lcflip,int lcnamesonly,int nregval,int* regval);
    //START200211
    Regions *get_reg(char *region_file,int vol,void *image,int nreg,char **ifh_region_names,int lcflip,int lcnamesonly,int nregval,int* regval,char* lutf);

    //Regions *get_reg_guts(int nregions,int lenvol,float *image,int fidl_gt0,int *reg_val);
    //START190909
    //Regions *get_reg_guts(int nregions,int lenvol,float *image,int fidl_gt0,int *reg_val,int lcnamesonly);
    //START190924
    //Regions *get_reg_guts(int nregions,int vol,void *image,int *reg_val,int lcnamesonly);
    //START191008
    //Regions *get_reg_guts(int nregions,int vol,void *image,int *reg_val,int lcnamesonly,int16_t datatype);
    //START200213
    //Regions *get_reg_guts(int nregions,int vol,void *image,int *reg_val,int lcnamesonly,char* lutf);
    //START200622
    Regions *get_reg_guts(int nregions,int vol,void *image,int *reg_val,int lcnamesonly,char* lutf,int filetype);

    void free_reg(Regions *reg);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
