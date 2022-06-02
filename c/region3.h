/* Copyright 8/31/21 Washington University.  All Rights Reserved.
   region3.h  $Revision: 1.1 $*/

#include <stdint.h>
#include <stddef.h>
#include "files_struct.h" 

#ifndef __REGION_H__
    #define __REGION_H__

    typedef struct {

        //int     nregions,vol,*nvoxels_region,*harolds_num,**voxel_indices,c_orient[3],nvoxels; //nvoxels added 200224
        //START211017
        int     nregions,vol,*nvoxels_region,*harolds_num,**voxel_indices,c_orient[3],nvoxels,*regval;

        size_t *length;
        char    **region_names;
        } Regions;

    #ifdef __cplusplus
        extern "C" {
    #endif

    int shouldiflip(Files_Struct* files);

    //Regions *get_reg(char *region_file,int vol,void *image,int nreg,char **ifh_region_names,int lcflip,int lcnamesonly,int nregval,int* regval,char* lutf);
    //START210831
    //Regions *get_reg(char *region_file,int vol,void *image,int nreg,char **ifh_region_names,int lcflip,int lcnamesonly,int nregval,int* regval,char* lutf,int lcregind);
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
