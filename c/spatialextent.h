/* Copyright 4/23/21 Washington University.  All Rights Reserved.
   spatialextent.h  $Revision: 1.1 $*/

#ifndef __SPATIALEXTENT_H__
    #define __SPATIALEXTENT_H__

    #include <vector>

    #define N_NGHBRS_FACE 6
    #define N_NGHBRS_END_FACE 5
    class spatialextent{
        public:
            int xdim,ydim,zdim,n_nghbrs_middle,n_nghbrs_end,plndim,vol,*offsets,*offsets_pln0,*offsets_plnN,*voxlist; //nbrnidx;
            std::vector<int> brnidx;

            ~spatialextent();
            spatialextent(int xdim0,int ydim0,int zdim0,int nbrnidx0,int* brnidx0); 
        };
#endif
