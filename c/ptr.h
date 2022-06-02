/* Copyright 9/26/19 Washington University.  All Rights Reserved.
   ptr.h  $Revision: 1.1 $ */
#ifndef __PTR_H__
    #define __PTR_H__ 
    #include "header.h"
    #include "map_disk.h"
    class ptr : public header { 
        int SunOS_Linux,swapbytes;
        Memory_Map *mm;
        void* niftiPtr;

        public:
            ptr();
            int ptr0(char *file);
            int ptrstack(float *temp_float,int idx);
            int ptrfree();
        };
#endif
