/* Copyright 3/26/13 Washington University.  All Rights Reserved.
   assign_region_names2.h  $Revision: 1.3 $ */
typedef struct {
    size_t *tstv;
    char *regname;
    } assign_region_names2struct;
assign_region_names2struct *assign_region_names2init(int nreg);
void assign_region_names2free(assign_region_names2struct *arn);

#if 0
int assign_region_names2(int nreg,assign_region_names2struct *arn,Interfile_header *ifh,int *nvoxels_region,double *coor,
    double *peakcoor,float *peakval);
#endif
/*START150408*/
int assign_region_names2(int nreg,assign_region_names2struct *arn,Interfile_header *ifh,int *nvoxels_region,double *coor,
    double *peakcoor,float *peakval,char **regnames);
