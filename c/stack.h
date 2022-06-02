/* Copyright 2/21/18 Washington University.  All Rights Reserved.
   stack.h  $Revision: 1.1 $ */
#ifndef __STACK_H__
    #define __STACK_H__
    #include "header.h"
    class stack : public header {

        float *f;

        int *i0;
        unsigned short *us; 
        public:
            int SunOS_Linux;

            stack(); 

            //START190930
            ~stack(); 

            float* stack0(char *file);
            
            //START191001
            float* stack0();

            unsigned short* stack0us(char *file);

            //START191001
            unsigned short* stack0us();

            //START211019
            int* stack0int(char *file);

            int stack1(char *file0,float *temp_float);

            int getstack(float* stack);
            int getstack(unsigned short* stack);
            int getstack(int* stack);

            int flip(float* f0);
            int flip(unsigned short* f0);
            int flip(int* f0);

        };
#endif
