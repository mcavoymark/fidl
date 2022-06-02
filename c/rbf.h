/* Copyright 10/9/19 Washington University.  All Rights Reserved.
   rbf.h  $Revision: 1.1 $ */
#ifndef __RBF_H__
    #define __RBF_H__
    #include <vector> 
    #include "stack.h"

    class reg: public stack {

        int nregions,**voxel_indices;
        char** region_names;
        std::vector<size_t> length;
        std::vector<int> nvoxels_region;

        public:
            reg();
            ~reg();
            int reg0(char* regf,int lcflip,int lcnamesonly);
            int reg0_guts(void* image,int* regval);

        };

    class rbf: public reg {

        public:
            int rbf0(int nregf,char** regf,int lcflip,int lcnamesonly);

        };
#endif
