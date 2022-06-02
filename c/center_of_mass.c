/* Copyright 5/18/05 Washington University.  All Rights Reserved.
   center_of_mass.c  $Revision: 1.26 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef __sun__
    #include <stdint.h>
#endif

/*#include <fidl.h>*/
/*START150324*/
#include "fidl.h"
int center_of_mass_cover(int vol,float *act,float *actmask,int nreg,double *coor,double *peakcoor,float *peakval,int SunOS_Linux,
   Interfile_header *ifh);
int center_of_mass(float *act,Regions_By_File *rbf,double *coor,Atlas_Param *ap,double *peakcoor,float *peakval);

int _center_of_mass(int argc,char **argv)
{
    float *act,*actmask,*peakval;
    int i,*coor,nreg,vol,*peakcoor,SunOS_Linux;
    double *dcoor,*dpeakcoor;
    act = (float*)argv[0];
    actmask = (float*)argv[1];
    coor = (int*)argv[4];
    peakcoor = (int*)argv[5];
    peakval = (float*)argv[6];
    #ifdef __sun__
        vol = (int)argv[2];
        nreg = (int)argv[3];
    #else
        vol = (intptr_t)argv[2];
        nreg = (intptr_t)argv[3];
    #endif
    if(!(dcoor=malloc(sizeof*dcoor*3*nreg))) {
        printf("fidlError: Unable to malloc coor\n");
        return 0;
        }
    if(!(dpeakcoor=malloc(sizeof*dpeakcoor*3*nreg))) {
        printf("fidlError: Unable to malloc dpeakcoor\n");
        return 0;
        }
    if((SunOS_Linux=checkOS())==-1) return 0;
    if(!center_of_mass_cover(vol,act,actmask,nreg,dcoor,dpeakcoor,peakval,SunOS_Linux,(Interfile_header*)NULL)) return 0;
    for(i=0;i<nreg*3;i++) coor[i] = (int)dcoor[i];
    for(i=0;i<nreg*3;i++) peakcoor[i] = (int)dpeakcoor[i];
    free(dcoor);
    free(dpeakcoor);
    return 1;
}
int center_of_mass_cover(int vol,float *act,float *actmask,int nreg,double *coor,double *peakcoor,float *peakval,int SunOS_Linux,
   Interfile_header *ifh)
{
    Regions *reg;
    Regions_By_File *rbf;
    Atlas_Param *ap;
    Interfile_header *ifh1;
    int i,*roi;
    char *atlas=NULL;
    ifh1 = (atlas=get_atlas(vol)) ? (Interfile_header*)NULL : ifh;
    if(!(ap=get_atlas_param(atlas,ifh1))) return 0;
    if(!(roi=malloc(sizeof*roi*nreg))) {
        printf("fidlError: Unable to malloc roi in center_of_mass_cover\n");
        fflush(stdout);
        return 0;
        }
    for(i=0;i<nreg;i++) roi[i] = i;
    if(!(reg=extract_regions((char*)NULL,0,vol,actmask,nreg,SunOS_Linux,(char**)NULL))) return 0;
    if(!(rbf=find_regions_by_file_cover(1,nreg,&reg,roi))) return 0;
    if(!(center_of_mass(act,rbf,coor,ap,peakcoor,peakval))) return 0;
    free_regions_by_file(rbf);
    free_regions(reg);
    free(roi);
    free_atlas_param(ap);
    return 1;
}
int center_of_mass(float *act,Regions_By_File *rbf,double *coor,Atlas_Param *ap,double *peakcoor,float *peakval)
{
    double denominator,num_x,num_y,num_z,*x,*y,*z,*px,*py,*pz,max,td;
    int i,j,k; /*These must be integers.*/
    if(!(x=malloc(sizeof*x*rbf->nvoxels))) {
        printf("Error: Unable to malloc x in center of mass\n");
        return 0;
        }
    if(!(y=malloc(sizeof*y*rbf->nvoxels))) {
        printf("Error: Unable to malloc y in center of mass\n");
        return 0;
        }
    if(!(z=malloc(sizeof*z*rbf->nvoxels))) {
        printf("Error: Unable to malloc z in center of mass\n");
        return 0;
        }
    if(!(px=malloc(sizeof*px*rbf->nreg))) {
        printf("Error: Unable to malloc px in center of mass\n");
        return 0;
        }
    if(!(py=malloc(sizeof*py*rbf->nreg))) {
        printf("Error: Unable to malloc py in center of mass\n");
        return 0;
        }
    if(!(pz=malloc(sizeof*pz*rbf->nreg))) {
        printf("Error: Unable to malloc pz in center of mass\n");
        return 0;
        }
    col_row_slice(rbf->nvoxels,rbf->indices,x,y,z,ap);
    for(k=i=0;i<rbf->nreg;i++) {
        for(max=denominator=num_x=num_y=num_z=0.,j=0;j<rbf->nvoxels_region[i];j++,k++) {
            denominator += (double)act[rbf->indices[k]];
            num_x += (double)(x[k]*act[rbf->indices[k]]);
            num_y += (double)(y[k]*act[rbf->indices[k]]);
            num_z += (double)(z[k]*act[rbf->indices[k]]);
            if((td=fabs(act[rbf->indices[k]])) > max) {
                max = td;
                peakval[i] = act[rbf->indices[k]];
                px[i] = (double)x[k];
                py[i] = (double)y[k];
                pz[i] = (double)z[k];
                }
            }
        x[i] = num_x/denominator;
        y[i] = num_y/denominator;
        z[i] = num_z/denominator;
        }
    get_atlas_coor(rbf->nreg,x,y,z,(double)ap->zdim,ap->center,ap->mmppix,coor);
    get_atlas_coor(rbf->nreg,px,py,pz,(double)ap->zdim,ap->center,ap->mmppix,peakcoor);
    free(x);
    free(y);
    free(z);
    free(px);
    free(py);
    free(pz);
    return 1;
}
