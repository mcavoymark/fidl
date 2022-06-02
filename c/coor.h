/* Copyright 9/30/19 Washington University.  All Rights Reserved.
   coor.h  $Revision: 1.1 $ */
#ifndef __COOR_H__
    #define __COOR_H__ 
    #include "mask.h"

    class coor: public mask {

        float *coorf;

        public:

            coor();
            ~coor();
            float* coor0(char *file);

        };
#endif
