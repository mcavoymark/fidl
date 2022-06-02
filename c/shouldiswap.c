/* Copyright 4/23/07 Washington University.  All Rights Reserved.
   shouldiswap.c  $Revision: 1.6 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __sun__
    #include <stdint.h>
#endif

//#include "fidl.h"
//START150714
#include "shouldiswap.h"

int _shouldiswap(int argc,char **argv)
{
    #ifdef __sun__
        return shouldiswap((int)argv[0],(int)argv[1]);
    #else
        return shouldiswap((intptr_t)argv[0],(intptr_t)argv[1]);
    #endif
}
int shouldiswap(int SunOS_Liunx,int bigendian)
{
    if(!SunOS_Liunx && !bigendian) return 1;
    if(SunOS_Liunx && bigendian) return 1;
    return 0;
}
