/* Copyright 10/1/19 Washington University.  All Rights Reserved.
   lut.h  $Revision: 1.1 $ */
#ifndef __LUT_H__
    #define __LUT_H__ 

    //#include <map>
    //START210503
    #include <unordered_map>

    #include <string>

    class lut{

        char *FS[65536]; //*L,*R
        unsigned short Lval,Rval;

        public:

            //START200213
            int regvalmaxplusone;
         
            lut(); 

            //START200211
            virtual ~lut(){};

            void lut0(char* filename);

            unsigned short lutLval();
            unsigned short lutRval();

            //std::map<int,std::string> LUT; 
            //START210503
            std::unordered_map<int,std::string> LUT; 

            //void lut2(char* lutf);    
            //START210504
            int lut2(char* lutf);    

            char** lut1(char* filename,char* lutf); 

        };

#if 0
    #ifdef __cplusplus
        extern "C"{
    #endif
            const char* lut_name(char* lutf);
    #ifdef __cplusplus
            }
    #endif
#endif

#endif
