/* Copyright 1/23/21 Washington University.  All Rights Reserved.
   ptrw.h  $Revision: 1.1 $ */

#ifndef __PTRW_H__
    #define __PTRW_H__ 
    #include "write1.h"
    class ptrw{ 
        int SunOS_Linux,filetype;
        void* ret;
        char* file;
        float min,max;
        public:
            ptrw();
            int ptrw0(char* file0,W1* w1);
            int ptrwstack(float* temp_float,int how_many,int idx);
            char* ptrwfree(Interfile_header *ifh);
        };
#endif
