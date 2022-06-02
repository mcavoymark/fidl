/* Copyright 5/26/15 Washington University.  All Rights Reserved.
   box_correction.h  $Revision: 1.2 $ */
#ifndef __BOX_CORRECTION_H__
    #define __BOX_CORRECTION_H__

    #ifdef __cplusplus
        extern "C" {
    #endif

double calculate_epsilon_triangular(double **transformed_ATAm1,int num_tc);
double calculate_epsilon_whole_matrix(double **transformed_ATAm1,int num_tc);
double calculate_epsilon_whole_matrix_box(double **transformed_ATAm1,int num_tc);
//void min_and_max_var(double **transformed_ATAm1,int num_tc,double *min,double *max);
//int calculate_and_print_correlation_coefficient(double **cov,int dim,Interfile_header *ifh,char *filename);
//double **cov_tc_avg_reduced_ATA(char **glm_files,int num_glm_files,TC *tcs,int swapbytes);
//int subcov_tc_reduced_ATA(char **glm_files,int num_glm_files,TC *tcs,double *subcov,int swapbytes);
//double **cov_mag_avg_reduced_ATA(char **glm_files,int num_glm_files,int **contrast,int num_contrasts,int swapbytes);
int print_cov_matrix(char *filename,double **ATAm1,int dim,Interfile_header *ifh,int bigendian,int swapbytes);
//double box_tc_avg_reduced_ATA_wYYT(char **glm_files,int num_glm_files,TC *tcs,int *are_variances_heterogeneous,char **YYT_files);
//double **cov_avg_reduced_ATA(char **glm_files,int num_glm_files,TC *tcs,int swapbytes,int tc_contrast);
//int subcov_reduced_ATA_noV(char **glm_files,int num_glm_files,TC *tcs,double *subcov,int swapbytes,int tc_contrast);
//int subcov_reduced_ATA_V(char **glm_files,int num_glm_files,TC *tcs,double *subcov,int swapbytes,int tc_contrast,Files_Struct *V_files);
//int subcov_reduced_ATA(char **glm_files,int num_glm_files,TC *tcs,double *subcov,int swapbytes,int tc_contrast,Files_Struct *Vconc);
//int subcov_ATAm1(char **glm_files,int num_glm_files,TC *tcs,double *subcov,int swapbytes,int tc_contrast,Files_Struct *Vconc);
void get_transform_gsl(TC *tcs,int tc_contrast,int j,double **ATA,int Mcol,double *c,double **transform);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
