/* Copyright 10/2/15 Washington University.  All Rights Reserved.
   read_af3d.h  $Revision: 1.2 $ */
typedef struct {
    double **coordinates,*xd,*yd,*zd;
    int *index,*x,*y,*z,*ncoordinates,nindex;
    } Af3d;
Af3d *read_af3d(char *filename,Atlas_Param *ap,float threshold,char *type);
void free_af3d(Af3d *af3d);
