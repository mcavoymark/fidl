/* Copyright 3/25/15 Washington University.  All Rights Reserved.
   read_ttest_driver.h  $Revision: 1.2 $ */

#ifndef __READ_TTEST_H__
    #define __READ_TTEST_H__

    typedef struct {
        char **type,**name,**files1,**files2,**cov,*names,**namesptr;
        int ntests,total_nfiles,*nfiles1,*nfiles2,*ncov,*ncov_per_line,nnames,*lnames;
        } TtestDesign;
    
    #ifdef __cplusplus
        extern "C" {
    #endif

    TtestDesign *read_ttest_driver(char *driver_file);

    #ifdef __cplusplus
        }//extern
    #endif
#endif
