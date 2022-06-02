/* Copyright 4/23/21 Washington University.  All Rights Reserved.
   spatialextent.cxx  $Revision: 1.1 $*/

#include "spatialextent.h"

spatialextent::~spatialextent(){
    if(offsets) delete[] offsets;
    if(offsets_pln0) delete[] offsets_pln0;
    if(offsets_plnN) delete[] offsets_plnN;
    if(voxlist) delete[] voxlist;
    }
spatialextent::spatialextent(int xdim0,int ydim0,int zdim0,int nbrnidx0,int* brnidx0){
    xdim=xdim0;ydim=ydim0;zdim=zdim0;
    n_nghbrs_middle = (int)N_NGHBRS_FACE;
    n_nghbrs_end = (int)N_NGHBRS_END_FACE;
    plndim = xdim*ydim;
    vol = xdim*ydim*zdim;
    //nbrnidx = nbrnidx0;
    brnidx.insert(brnidx.end(),brnidx0,brnidx0+nbrnidx0); 
    offsets=new int[6];
    offsets_pln0=new int[5];
    offsets_plnN=new int[5];
    voxlist=new int[vol];
    offsets[0]=1;offsets[1]=-1;offsets[2]=-xdim;offsets[3]=xdim;offsets[4]=plndim;offsets[5]=-plndim;
    offsets_pln0[0]=1;offsets_pln0[1]=-1;offsets_pln0[2]=-xdim;offsets_pln0[3]=xdim;offsets_pln0[4]=plndim;
    offsets_plnN[0]=1;offsets_plnN[1]=-1;offsets_plnN[2]=-xdim;offsets_plnN[3]=xdim;offsets_plnN[4]=-plndim;
    }
