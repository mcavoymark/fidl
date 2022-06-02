/* Copyright 4/29/21 Washington University.  All Rights Reserved.
   spatialextent2.cxx  $Revision: 1.1 $*/

#include <iostream>

//#include "spatialextent.h"
//START210429
#include "spatialextent2.h"

spatialextent::~spatialextent(){

    std::cout<<"here200"<<std::endl;

    if(offsets) delete[] offsets;

    std::cout<<"here201"<<std::endl;

    if(offsets_pln0) delete[] offsets_pln0;

    std::cout<<"here202"<<std::endl;

    if(offsets_plnN) delete[] offsets_plnN;

    std::cout<<"here203"<<std::endl;

    if(voxlist) delete[] voxlist;

    std::cout<<"here204"<<std::endl;
    }

#if 0
spatialextent::spatialextent(int xdim0,int ydim0,int zdim0,int nbrnidx0,int* brnidx0){
    xdim=xdim0;ydim=ydim0;zdim=zdim0;
    n_nghbrs_middle = (int)N_NGHBRS_FACE;
    n_nghbrs_end = (int)N_NGHBRS_END_FACE;
    plndim = xdim*ydim;
    vol = xdim*ydim*zdim;
    brnidx.insert(brnidx.end(),brnidx0,brnidx0+nbrnidx0); 
    offsets=new int[6];
    offsets_pln0=new int[5];
    offsets_plnN=new int[5];
    voxlist=new int[vol];
    offsets[0]=1;offsets[1]=-1;offsets[2]=-xdim;offsets[3]=xdim;offsets[4]=plndim;offsets[5]=-plndim;
    offsets_pln0[0]=1;offsets_pln0[1]=-1;offsets_pln0[2]=-xdim;offsets_pln0[3]=xdim;offsets_pln0[4]=plndim;
    offsets_plnN[0]=1;offsets_plnN[1]=-1;offsets_plnN[2]=-xdim;offsets_plnN[3]=xdim;offsets_plnN[4]=-plndim;
    }
#endif
//START210429
spatialextent::spatialextent(const mask& ms){
    xdim=ms.dim[0];ydim=ms.dim[1];zdim=ms.dim[2];
    n_nghbrs_middle = (int)N_NGHBRS_FACE;
    n_nghbrs_end = (int)N_NGHBRS_END_FACE;
    plndim = xdim*ydim;
    vol = xdim*ydim*zdim;
    brnidx.insert(brnidx.end(),ms.brnidx,ms.brnidx+ms.lenbrain); 
    offsets=new int[6];
    offsets_pln0=new int[5];
    offsets_plnN=new int[5];
    voxlist=new int[vol];
    offsets[0]=1;offsets[1]=-1;offsets[2]=-xdim;offsets[3]=xdim;offsets[4]=plndim;offsets[5]=-plndim;
    offsets_pln0[0]=1;offsets_pln0[1]=-1;offsets_pln0[2]=-xdim;offsets_pln0[3]=xdim;offsets_pln0[4]=plndim;
    offsets_plnN[0]=1;offsets_plnN[1]=-1;offsets_plnN[2]=-xdim;offsets_plnN[3]=xdim;offsets_plnN[4]=-plndim;
    }
