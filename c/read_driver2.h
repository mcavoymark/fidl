/* Copyright 3/24/15 Washington University.  All Rights Reserved.
   read_driver2.h  $Revision: 1.10 $ */
#ifndef __READ_DRIVER2_H__
    #define __READ_DRIVER2_H__
    #define RANDOM      0       /*zeroeth column is RANDOM factor for Perlman_levels*/

    #ifdef __cplusplus
        extern "C" {
    #endif

    int offset(int *level,int num_factors,int *num_levels);

    /*START150908*/
    int offset2(int *level,int num_factors,int *num_levels,int *notfactor);
   
    //START160407
    int offset3(int *level,int num_factors,int *num_levels);

    int setsize(int set,int maxsize);

    int nextlevel(int *level,int source,int sourceflag,int num_factors,int *num_levels);

    /*START150909*/
    int nextlevel2(int *level,int source,int sourceflag,int num_factors,int *num_levels,int *factori);

    typedef struct {
        int chunks;
        int num_factors;
        int num_sources;            /* num_sources = 2**num_factors */
        int num_datafiles;
        int num_cells,num_cells_notRANDOM;
        int between,within;         /* holds between subject factors */
        int *Perlman_levels0,*Perlman_levels;        
        int *address;               /* length = num_datafiles */
        int *replications;          /* length = num_cells */
        int *level;                 /* length = num_factors, computational tool */
        int *sumcount;              /* length = num_factors, computational tool */
        int *dfeffect;              /* length = num_sources/2 - 1 */
        double *dfeffectSPH;
        int *dferror;               /* length = num_sources/2 - 1 */
        double *dferrorSPH;
        int *dferror_within;        /* length = num_sources/2 - 1, df from within subjects factors contributing to dferror */
        int betprod,withprod;
        float *data;                /* length = num_cells */
        float *driver_data;         /* length = nlines */
        double *fstat,*chat,*htildaprimeprime,*htilda; /* length = num_sources/2 - 1 */

        double *bracket;            /* length = num_sources */
        char **datafiles;           /* length = num_datafiles */
        
        char **fstatfilesptr;       /* num_sources */
        size_t *fstatfilesl;           /* num_sources */

        size_t max_length;
    
        char **fnptr0,**fnptr,*datafilesstack,*ln,**lnptr0,**lnptr;
        int aa,aa1,nwithin,nbetween,*within_between,*nterms,within_neffects,*within_effects,*betweeni,*betweenf,*withini,num_factors0,
            aa0;
        double *sseffect,*mseffect,*sserror,*mserror;
        int **levelall;
        int nnotfactor,*notfactor,*notfactori,*NA,*numeric0;
        } AnovaDesign;

    //AnovaDesign *read_driver2(char *driver_file,int nreg,int not_num_factors,int lclevelall,char separator);
    //START170207
    AnovaDesign *read_driver2(char *driver_file,int nreg,int not_num_factors,int lclevelall,char separator,int dontcheck);
    //START200424
    //AnovaDesign *read_driver2(char *driver_file,int nreg,int not_num_factors,int lclevelall,char *separator,int dontcheck);

    /*START150908*/
    char* get_level(char *write_back,AnovaDesign *ad);

    //START160408
    char* get_level2(char *write_back,AnovaDesign *ad,int num_factors);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
