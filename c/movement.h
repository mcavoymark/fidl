/* Copyright 3/31/21 Washington University.  All Rights Reserved.
   movement.h  $Revision: 1.1 $ */
#ifndef __MOVEMENT_H__
    #define __MOVEMENT_H__ 

    #include <vector>
    #include "files_struct.h"

    class movement{

        public:
            double *MARMSmval,*wts;
            //std::vector<double> MARMSmval,wts; 


            movement();
            ~movement();
            int movement0(Files_Struct* MARMSm);



        };

#endif

