/* Copyright 1/22/19 Washington University.  All Rights Reserved.
   bids.h  $Revision: 1.1 $ */
#ifndef __BIDS_H__
    #define __BIDS_H__ 

    class bids{
        char *file;
        int ntimes;
        double TR,*times;

        public:

            int bids0(char* file);
            int bidsSliceTimingtxt(char *file);

        };
#endif
