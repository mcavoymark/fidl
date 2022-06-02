/* Copyright 7/13/17 Washington University.  All Rights Reserved.
   header.h  $Revision: 1.1 $ */
#ifndef __HEADER_H__
    #define __HEADER_H__ 

    //START211112
    #include <string>

    #include <inttypes.h>
    #include "ifh.h"

    class header{

        //START210422
        //int orientation;

        //START191126
        //float voxel_size[3];

        public:
            char *file;
            int dim[4],vol,filetype,bigendian,c_orient[3],number_format,orientation,lenvol;
            int16_t datatype; 
            int64_t dim64[4];
            float mmppixf[3],centerf[3],voxel_size[3];
          
            Interfile_header *ifh;

            //START211019
            std::string datatypestr; 
        

            header();   
            
            //START190927
            ~header();

            int header0(char* file0); //returns vol

            int get_cf_flip(){return c_orient[0]*100+c_orient[1]*10+c_orient[2];}; 

            void assign(int& filetype); 
            void assign(int& filetype,int *c_orient); 

            //START190925
            void assign(int& filetype,int *c_orient,int16_t& datatype); 

            void assign(int& xdim,int& ydim,int& zdim);
            void assign(int& xdim,int& ydim,int& zdim,float& voxel_size_1,float& voxel_size_2,float& voxel_size_3,
                float *center,float *mmppix);
            void assign(int& xdim,int& ydim,int& zdim,int& tdim,float& voxel_size_1,float& voxel_size_2,float& voxel_size_3,
                float *center,float *mmppix,int& bigendian,int& orientation,int *c_orient);

            //START210422
            //void assign(int& xdim,int& ydim,int& zdim,int& tdim,float& voxel_size_1,float& voxel_size_2,float& voxel_size_3,
            //    float *center,float *mmppix,int& bigendian,int& orientation,int *c_orient,int& number_format);

            void assign(float *center,float *mmppix);
            void assign(float& voxel_size_1,float& voxel_size_2,float& voxel_size_3,float *center,float *mmppix);

            //START190920
            void assign(int64_t *dim64,float *center,float *mmppix); 

            //START191007
            void assign(int64_t *dim64,float *center,float *mmppix,int *c_orient); 
 
            Interfile_header* getifh(){return ifh;}
        };
#endif
