/* Copyright 11/1/17 Washington University.  All Rights Reserved.
   mask.h  $Revision: 1.1 $ */
#ifndef __MASK_H__
    #define __MASK_H__
    #include "subs_glm.h"
    #include "stack.h"

    class mask: public stack {
        int nuns,*maskidx,*unsi,lcbrnidx,lcmaskidx;

        public:

            int lenbrain,*brnidx;

            mask();
            ~mask();

            int read_mask(char *mask_file,LinearModel *glm_in);
            int get_mask(char *mask_file,int vol,int *indices,LinearModel *glm,int mslenvol);

            int* get_brnidx(int& lenbrain);
            int* get_brnidx(int& lenbrain,int& vol);
            int* get_brnidx(int& lenbrain,int& xdim,int& ydim,int& zdim);
            int* get_brnidx(int& lenbrain,int& vol,int& xdim,int& ydim,int& zdim);

            int* get_maskidx();
            int* get_unsi(int& nuns);

            int _read_mask(int argc,char **argv);
        };
#endif
