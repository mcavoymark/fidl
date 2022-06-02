/* Copyright 8/3/12 Washington University.  All Rights Reserved.
   combinations.c  $Revision: 1.1 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fidl.h>
#include <gsl/gsl_combination.h>

void combinations(size_t setsize) 
{
    gsl_combination *c;
    size_t i; 
    for(i=1;i<=setsize;i++) {
        c = gsl_combination_calloc(4,i);
         
    
