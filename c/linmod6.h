/* Copyright 3/20/19 Washington University.  All Rights Reserved.
   linmod6.h  $Revision: 1.1 $ */
#ifndef __LINMOD6_H__
    #define __LINMOD6_H__
    #ifdef __cplusplus
        extern "C" {
    #endif
    //#include "files_struct.h"

int linmod6(
           Files_Struct *file_names,
           char *in_glm_file,   /* File containing glm info (*.glm).    */
           char *out_glm_file_in,       /* File glm info written to (*.glm).    */
           Files_Struct *xform_file,    /* File defining atlas transformation (2-2-2). NULL=none. */
           char  *mask_file,    /* Mask defining regions of atlas containing brain. */
           int   mode,          /* bit 0: smooth.
                            bit 1: tranforms to 2-2-2 space,
                            bit 2: Compute smoothness estimate.
                            the following values hold:
                            0: Unsmoothed data in subject space.
                            1: smooth only,
                            2: transform only,
                            3: smooth and transform.
                            4: Estimate smoothness (assume preprocessing)
                            7: Smooth, transform and estimate smoothness. */
           float fwhm,           /* FWHM of smoothing filter. */
           unsigned short *seed, /* If seed, then generate white noise data with the seed. */
           int *t4_identify,int num_regions,Files_Struct *region_files,int *roi,char *atlas,int SunOS_Linux,int bigendian,
           int lcdetrend,char *scratchdir,int lccompressb,int lcvoxels,int lcR2,Files_Struct *roistr,char *lutf);

           /* Syntax: -F label1 computes an F statistic for label1 where label1 is a label in the
                      event file.  -F label1&label2 computes and F statistic over the effects corresponding 
                      label1 and label2. Label1 and label2 might be levels of a main effect, so this would
                      compute an F for the main effect. */

int linmod7();
    #ifdef __cplusplus
        }//extern
    #endif
#endif
