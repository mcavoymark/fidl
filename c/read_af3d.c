/* Copyright 3/15/01 Washington University.  All Rights Reserved.
   read_af3d.c  $Revision: 1.21 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "fidl.h"

//START151002
#include "read_af3d.h"

Af3d *read_af3d(char *filename,Atlas_Param *ap,float threshold,char *type){
char string[MAXNAME],write_back[MAXNAME];
int i,nstrings,nskip=-1,lc_af3d=0,nstrings_max=0;
float coordinates[6];
FILE *fp;
Af3d *af3d;
if(!strcmp(type,"af3d")) {
    lc_af3d = 1;
    }
else if(!strcmp(type,"ellipse")) {
    lc_af3d = 2;
    }
if(!(fp=fopen_sub(filename,"r"))) return NULL;

if(lc_af3d < 2) {
    for(i=0;fgets(string,sizeof(string),fp);i++) {
        if((nstrings=count_strings(string,write_back,' ')) > nstrings_max) nstrings_max=nstrings;
        if(nskip == -1) {
            if(!lc_af3d) nstrings = 3;
            if(nstrings==3 || nstrings==4) {
                grab_string(write_back,string);
                if(isdigit(write_back[0]) || (write_back[0]=='-' && isdigit(write_back[1]))) {
                    nskip = i;
                    }
                }
            }
        else if(!nstrings) {
            break;
            }
        else if(nstrings == 4 && threshold != (float)UNSAMPLED_VOXEL) {
            strings_to_float(write_back,coordinates,4);
            if(coordinates[3] < threshold) break;
            }
        }
    if(nskip>0) i -= nskip;
    }
else {
    for(i=0;fgets(string,sizeof(string),fp);) if(count_strings(string,write_back,' ') >= 6) i++;
    if(!i) {
        printf("Error: %s has no data. Abort!\n",filename); 
        return NULL; 
        }
    nskip = 0;
    }
if(!(af3d=malloc(sizeof*af3d))) {
    printf("Error: Unable to malloc af3d\n");
    return NULL;
    }
af3d->nindex = i;
if(!(af3d->coordinates=d2double(af3d->nindex,nstrings_max))) exit(-1);
if(!(af3d->x=malloc(sizeof*af3d->x*af3d->nindex))) {
    printf("Error: Unable to malloc af3d->x\n");
    return NULL;
    }
if(!(af3d->y=malloc(sizeof*af3d->y*af3d->nindex))) {
    printf("Error: Unable to malloc af3d->y\n");
    return NULL;
    }
if(!(af3d->z=malloc(sizeof*af3d->z*af3d->nindex))) {
    printf("Error: Unable to malloc af3d->z\n");
    return NULL;
    }
if(!(af3d->xd=malloc(sizeof*af3d->xd*af3d->nindex))) {
    printf("Error: Unable to malloc af3d->xd\n");
    return NULL;
    }
if(!(af3d->yd=malloc(sizeof*af3d->yd*af3d->nindex))) {
    printf("Error: Unable to malloc af3d->yd\n");
    return NULL;
    }
if(!(af3d->zd=malloc(sizeof*af3d->zd*af3d->nindex))) {
    printf("Error: Unable to malloc af3d->zd\n");
    return NULL;
    }
if(!(af3d->index=malloc(sizeof*af3d->index*af3d->nindex))) {
    printf("Error: Unable to malloc af3d->z\n");
    return NULL;
    }
if(lc_af3d < 2) {
    if(!(af3d->ncoordinates=malloc(sizeof*af3d->ncoordinates*af3d->nindex))) {
        printf("Error: Unable to malloc af3d->z\n");
        return NULL;
        }
    for(rewind(fp),i=0;i<nskip;i++) fgets(string,sizeof(string),fp);
    for(i=0;i<af3d->nindex;i++) {
        fgets(string,sizeof(string),fp);
        af3d->ncoordinates[i] = count_strings(string,write_back,' ');
        if(lc_af3d && af3d->ncoordinates[i] < 3) {
            free_af3d(af3d);
            printf("Error: Less than three points detected. Abort!\n");
            return NULL;
            } 
        }
    for(rewind(fp),i=0;i<nskip;i++) fgets(string,sizeof(string),fp);
    for(i=0;i<af3d->nindex;i++) {
        fgets(string,sizeof(string),fp);
        strings_to_double(string,af3d->coordinates[i],af3d->ncoordinates[i]);
        }
    if(lc_af3d) atlas_to_index(af3d->nindex,af3d->coordinates,ap,af3d->x,af3d->y,af3d->z,af3d->xd,af3d->yd,af3d->zd,af3d->index);
    fclose(fp);
    }
else {
    af3d->ncoordinates=NULL;
    for(rewind(fp),i=0;fgets(string,sizeof(string),fp);) {
        if(count_strings(string,write_back,' ') >= 6) {
            strings_to_double(write_back,af3d->coordinates[i],6);
            i++;
            }
        }
    atlas_to_index(af3d->nindex,af3d->coordinates,ap,af3d->x,af3d->y,af3d->z,af3d->xd,af3d->yd,af3d->zd,af3d->index);
    }
#if 0
for(i=0;i<af3d->nindex;i++) {
    printf("%f %f %f %f\n",af3d->coordinates[i][0],af3d->coordinates[i][1],af3d->coordinates[i][2],af3d->coordinates[i][3]);
    printf("%d %d %d\n",af3d->x[i],af3d->y[i],af3d->z[i]);
    }
#endif
return af3d;
}
void free_af3d(Af3d *af3d)
{
    free_d2double(af3d->coordinates);
    free(af3d->x);
    free(af3d->y);
    free(af3d->z);
    free(af3d->xd);
    free(af3d->yd);
    free(af3d->zd);
    free(af3d->index);
    if(af3d->ncoordinates) free(af3d->ncoordinates);
    free(af3d);
}
