/* Copyright 9/4/14 Washington University.  All Rights Reserved.
   assign_region_names2.c  $Revision: 1.9 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif
#include "fidl.h"
#include "assign_region_names2.h"
assign_region_names2struct *assign_region_names2init(int nreg){
    assign_region_names2struct *arn;
    if(!(arn=malloc(sizeof*arn))) {
        printf("fidlError: Unable to malloc arn in assign_region_names2init\n");
        return NULL;
        }
    if(!(arn->tstv=malloc(sizeof*arn->tstv*nreg))) {
        printf("fidlError: Unable to malloc arn->tstv in assign_region_names2init\n");
        return NULL;
        }
    if(!(arn->regname=malloc(sizeof*arn->regname*MAXNAME))) {
        printf("fidlError: Unable to malloc arn->regname in assign_region_names2init\n");
        return NULL;
        }
    return arn;
    }
void assign_region_names2free(assign_region_names2struct *arn){
    free(arn->regname);
    free(arn->tstv);
    free(arn);
    }
int assign_region_names2(int nreg,assign_region_names2struct *arn,Interfile_header *ifh,int *nvoxels_region,double *coor,
    double *peakcoor,float *peakval,char **regnames){
    size_t i,j,k;
    i=j=k=0;
    if(!coor) {
        if(peakcoor) {
            if(ifh) { 
                for(k=i=0;i<(size_t)nreg;i++,k+=3) {
                    sprintf(arn->regname,"%lu %+d_%+d_%+d %d",(unsigned long)i,(int)rint(peakcoor[k]),(int)rint(peakcoor[k+1]),
                        (int)rint(peakcoor[k+2]),nvoxels_region[i]);
                    arn->tstv[i]=strlen(arn->regname)+1;
                    }
                }
            else {
                sprintf(arn->regname,"%+d_%+d_%+d %d",(int)rint(peakcoor[k]),(int)rint(peakcoor[k+1]),(int)rint(peakcoor[k+2]),
                    nvoxels_region[i]);
                return 1;
                }
            }
        else {
            if(ifh) { 
                if(!regnames) {
                    for(i=0;i<(size_t)nreg;i++) {
                        sprintf(arn->regname,"%lu voxel_value%lu %d",(unsigned long)i,(unsigned long)i+2,nvoxels_region[i]);
                        arn->tstv[i]=strlen(arn->regname)+1;
                        }
                    }
                else {
                    for(i=0;i<(size_t)nreg;i++) {
                        sprintf(arn->regname,"%lu %s %d",(unsigned long)i,regnames[i],nvoxels_region[i]);
                        arn->tstv[i]=strlen(arn->regname)+1;
                        }
                    }

                }
            else {
                sprintf(arn->regname,"voxel_value%lu %d",(unsigned long)i+2,nvoxels_region[i]);
                return 1;
                }
            }
        }
    else {
        if(!peakcoor) {
            if(ifh) {
                for(j=i=0;i<(size_t)nreg;i++,j+=3) {
                    sprintf(arn->regname,"%lu %+d_%+d_%+d %d",(unsigned long)i,(int)rint(coor[j]),(int)rint(coor[j+1]),
                        (int)rint(coor[j+2]),nvoxels_region[i]);
                    arn->tstv[i]=strlen(arn->regname)+1;
                    }
                }
            else {
                sprintf(arn->regname,"%+d_%+d_%+d %d",(int)rint(coor[j]),(int)rint(coor[j+1]),(int)rint(coor[j+2]),nvoxels_region[i]);
                return 1;
                }
            }
        else {
            if(ifh) {
                for(k=j=i=0;i<(size_t)nreg;i++,j+=3,k+=3) {
                    sprintf(arn->regname,"%lu com%+d_%+d_%+d_p%+d_%+d_%+d=%.2f %d",(unsigned long)i,(int)rint(coor[j]),
                        (int)rint(coor[j+1]),(int)rint(coor[j+2]),(int)rint(peakcoor[k]),(int)rint(peakcoor[k+1]),
                        (int)rint(peakcoor[k+2]),peakval[i],nvoxels_region[i]);
                    arn->tstv[i]=strlen(arn->regname)+1;
                    }
                }
            else {
                sprintf(arn->regname,"com%+d_%+d_%+d_p%+d_%+d_%+d=%.2f %d",(int)rint(coor[j]),(int)rint(coor[j+1]),
                    (int)rint(coor[j+2]),(int)rint(peakcoor[k]),(int)rint(peakcoor[k+1]),(int)rint(peakcoor[k+2]),peakval[i],
                    nvoxels_region[i]);
                return 1;
                }
            }
        }
    if(ifh) {
        if(!(ifh->region_names=d2charvar(nreg,arn->tstv))) return 0;
        ifh->nregions = nreg;
        if(!coor) {
            if(peakcoor) {
                for(k=i=0;i<(size_t)nreg;i++,k+=3) sprintf(ifh->region_names[i],"%lu %+d_%+d_%+d %d",(unsigned long)i,
                    (int)rint(peakcoor[k]),(int)rint(peakcoor[k+1]),(int)rint(peakcoor[k+2]),nvoxels_region[i]);
                }
            else {
                if(!regnames) {
                    for(i=0;i<(size_t)nreg;i++) sprintf(ifh->region_names[i],"%lu voxel_value%lu %d",(unsigned long)i,(unsigned long)i+2,
                        nvoxels_region[i]);
                    }
                else {
                    for(i=0;i<(size_t)nreg;i++) sprintf(ifh->region_names[i],"%lu %s %d",(unsigned long)i,regnames[i],nvoxels_region[i]);
                    }

                }
            }
        else {
            if(!peakcoor) {
                for(j=i=0;i<(size_t)nreg;i++,j+=3) sprintf(ifh->region_names[i],"%lu %+d_%+d_%+d %d",(unsigned long)i,(int)rint(coor[j]),
                    (int)rint(coor[j+1]),(int)rint(coor[j+2]),nvoxels_region[i]);
                }
            else {
                for(k=j=i=0;i<(size_t)nreg;i++,j+=3,k+=3) sprintf(ifh->region_names[i],"%lu com%+d_%+d_%+d_p%+d_%+d_%+d=%.2f %d",
                    (unsigned long)i,(int)rint(coor[j]),(int)rint(coor[j+1]),(int)rint(coor[j+2]),(int)rint(peakcoor[k]),
                    (int)rint(peakcoor[k+1]),(int)rint(peakcoor[k+2]),peakval[i],nvoxels_region[i]);
                }
            }
        }
    return 1; 
    }
char *_assign_region_names2(int argc,char **argv){
    float *coor=(float*)argv[0],*peakcoor=(float*)argv[1],*peakval=(float*)argv[2];
    #ifdef __sun__
        int regsize=(int)argv[3];
        int lccom=(int)argv[4];
        int lcpeak=(int)argv[5];
    #else
        int regsize=(intptr_t)argv[3];
        int lccom=(intptr_t)argv[4];
        int lcpeak=(intptr_t)argv[5];
    #endif
    size_t i;
    char *regname;
    double dcoor[3],dpeakcoor[3];
    assign_region_names2struct *arn;
    if(lccom) for(i=0;i<3;i++) dcoor[i]=(double)coor[i];
    if(lcpeak) for(i=0;i<3;i++) dpeakcoor[i]=(double)peakcoor[i];
    if(!(arn=assign_region_names2init(1))) return "NULL"; 
    if(!assign_region_names2(1,arn,(Interfile_header*)NULL,&regsize,!lccom?(double*)NULL:dcoor,!lcpeak?(double*)NULL:dpeakcoor,
        !lcpeak?(float*)NULL:peakval,(char**)NULL)) return "NULL";
    if(!(regname=malloc(sizeof*regname*(strlen(arn->regname)+1)))) {
        printf("fidlError: Unable to malloc arn in assign_region_names2init\n");
        return "NULL";
        }
    strcpy(regname,arn->regname);
    assign_region_names2free(arn); 
    return regname;
    }
