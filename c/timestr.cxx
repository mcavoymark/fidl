/* Copyright 12/5/19 Washington University.  All Rights Reserved.
   timestr.c  $Revision: 1.1 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <string>
#include "timestr.h" 

timestr::timestr(){
    char tstr[13];
    struct timeval tv;struct tm *tm;
    gettimeofday(&tv,NULL);
    tm=localtime(&tv.tv_sec);
    strftime(tstr,13,"%y%m%d%H%M%S",tm);
    sprintf(timestr0,"%s%06ld",tstr,tv.tv_usec);
    }
const char* timestr::_timestr(){
    std::string str(timestr0); 
    str.insert(0,"_"); 
    return str.c_str();
    }
