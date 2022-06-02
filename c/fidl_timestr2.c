/* Copyright 4/6/11 Washington University.  All Rights Reserved.
   fidl_timestr2.c  $Revision: 1.3 $ */

/*START1503225*/
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fidl.h>
#endif
/*START1503225*/
#include "fidl.h"

main(int argc,char **argv)
{
char timestr[23];
printf("%s\n",make_timestr2(timestr));
}
