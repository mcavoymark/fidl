/* Copyright 3/19/15 Washington University.  All Rights Reserved.
   subs_cifti.h  $Revision: 1.5 $ */
#ifndef __SUBS_CIFTI_H__
    #define __SUBS_CIFTI_H__
    #ifdef __cplusplus
        extern "C" {
    #endif
    int cifti_getstack(char *file,float *stack);
    typedef struct {
        unsigned int size;
        char *data;
        } XmlS;
    XmlS *cifti_getxml(char *file);
    int cifti_writedscalar(char *file,char *cifti_xmldata,unsigned int cifti_xmlsize,float *stack);

    /*void* cifti_setWritingFile(char *file,char *cifti_xmldata,unsigned int cifti_xmlsize);*/
    /*START150819*/
    int cifti_writedtseries(char *file,char *cifti_xmldata,unsigned int cifti_xmlsize,float *stack,int64_t tdim,int64_t vol);
    void* cifti_setdscalar(char *file,char *cifti_xmldata,unsigned int cifti_xmlsize);
    void* cifti_setdtseries(char *file,char *cifti_xmldata,unsigned int cifti_xmlsize,int64_t tdim);

    #if 0
    int cifti_setColumn(void* ciftiPtr,int64_t c,float *stack);
    int cifti_free(char *file,void* ciftiPtr);
    #endif

    #ifdef __cplusplus
        }//extern
    #endif
#endif
