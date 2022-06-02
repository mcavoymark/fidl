/* Copyright 21/8/31 Washington University.  All Rights Reserved.
   lut2.h  $Revision: 1.1 $ */
#ifndef __LUT2_H__
    #define __LUT2_H__ 

    #include <unordered_map>
    #include <string>

    class lut{

        //START210831
        //char *FS[65536]; //*L,*R
        
        unsigned short Lval,Rval;

        public:

            int regvalmaxplusone;
         
            lut(); 
            virtual ~lut(){};

            void lut0(char* filename);

            unsigned short lutLval();
            unsigned short lutRval();

            std::unordered_map<int,std::string> LUT; 
            std::string root;

            int lut2(char* lutf);    

            //char** lut1(char* filename,char* lutf); 
            int lut1(char* filename,char* lutf); 
        };
#endif
