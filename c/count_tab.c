/* Copyright 11/24/14 Washington University.  All Rights Reserved.
   count_tab.c  $Revision: 1.4 $*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fidl.h"
#include "count_tab.h"
int count_tab(char *sp1,char *sp2,char terminator){
    size_t i,sl,cnt=0,cnt1,unsl;
    int k=0;
    char *uns="1e-37",*unsp;
    sl = strlen(sp1);
    unsl = strlen(uns);
    while(*sp1) {
        cnt1=0;
        while(*sp1!='\t') {
            if(isalnum(*sp1)||*sp1=='.'||*sp1=='_') {
                *sp2++=*sp1++;cnt++;cnt1++;
                }
            else {
                sp1++;cnt++;
                }
            if(cnt>=sl)break;
            }
        if(*sp1=='\t'){sp1++;cnt++;}
        if(!cnt1) {
            for(unsp=uns,i=0;i<unsl;i++) *sp2++=*unsp++;
            }
        k++;
        if(cnt>=sl)break;
        *sp2++ = terminator;
        }
    *sp2 = 0;
    return k;
    }
