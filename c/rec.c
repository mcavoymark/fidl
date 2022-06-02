/*******************************************************************************************/
/* Copyright 1999, 2000, Washington University, Mallinckrodt Institute of Radiology.       */
/* All Rights Reserved.                                                                    */
/* This software may not be reproduced, copied, or distributed without written             */
/* permission of Washington University. For further information contact A. Z. Snyder.      */
/*******************************************************************************************/
/*$Header: /home/hannah/mcavoy/idl/clib/RCS/rec.c,v 1.1 2000/12/28 19:44:56 jmo Exp $*/
/*$Log: rec.c,v $
 * Revision 1.1  2000/12/28  19:44:56  jmo
 * Initial revision
 *
 * Revision 1.4  2000/12/13  06:02:41  avi
 * replace external get_date_log () with get_time_usr ()
 *
 * Revision 1.3  2000/12/13  02:47:57  avi
 * copyright
 *
 * Revision 1.2  1999/01/21  07:51:41  avi
 * prototyping
 *
 * Revision 1.1  1999/01/21  07:20:10  avi
 * Initial revision
 **/
#include <stdio.h>
#include <string.h>
#include <unistd.h>		/* R_OK and W_OK */
#include <time.h>

#define  MAXL	256
static char recfile[MAXL] = "";

static char rcsid[] = "$Id: rec.c,v 1.1 2000/12/28 19:44:56 jmo Exp $";
int printrec (char *string) {
 	FILE		*recfp;

	if (!(recfp = fopen (recfile, "a"))) {
	 	fprintf (stderr, "printrec: %s write error\n", recfile);
		return -1;
	}
	fprintf (recfp, "%s", string);
	fclose (recfp);
	return 0;
}

int catrec (char *file) {
	FILE		*recfp;
	char		filerec[MAXL], command[MAXL];
	int		k, debug = 0;
	int		isimg;

	if (access (recfile, W_OK)) {
		fprintf (stderr, "catrec: recfile not initialized\n");
		return -1;
	}
	strcpy (filerec, file);
	k = strlen (filerec);
	isimg = (!strcmp (filerec + k - 4, ".img")) || (!strcmp (filerec + k - 4, ".trk"));
	if (isimg) strcat (filerec, ".rec");
	if (access (filerec, R_OK)) {	
		if (!(recfp = fopen (recfile, "a"))) {
	 		fprintf (stderr, "printrec: %s write error\n", recfile);
			return -1;
		}
		fprintf (recfp, "%s not found\n", filerec);
		fclose (recfp);
		return 1;
	} else {
		sprintf (command, "cat -s %s >> %s", filerec, recfile);
		if (debug) fprintf (stderr, "%s\n", command);
		return system (command);
	}
}

int startrec (char *outfile, int argc, char *argv[], char *rcsid) {
	extern void	get_time_usr (char *string);
	FILE		*recfp;
	char 		*str, string[MAXL];
	int 		k;

	strcpy (string, outfile);
	while (str = strrchr (string, '.')) {
		if (!strcmp (str, ".rec")) *str = '\0';
		else break;
	}
	strcpy (recfile, string);
	strcat (recfile, ".rec");
	if (!(recfp = fopen (recfile, "w"))) {
		fprintf (stderr, "startrec: %s write error\n", recfile);
		return -1;
	}
	fprintf (recfp, "rec %s", string);
	get_time_usr (string);
	fprintf (recfp, "  %s\n", string);
	for (k = 0; k < argc; k++) fprintf (recfp, "%s ", argv[k]);
	fprintf (recfp, "\n%s\n", rcsid); 
	fclose (recfp);
	return 0;
}

int startrecl (char *outfile, int argc, char *argv[], char *rcsid) {
	extern void	get_time_usr (char *string);
	FILE		*recfp;
	char 		*str, string[MAXL];
	int 		k;

	strcpy (string, outfile);
	while (str = strrchr (string, '.')) {
		if (!strcmp (str, ".rec")) *str = '\0';
		else break;
	}
	strcpy (recfile, string);
	strcat (recfile, ".rec");
	if (!(recfp = fopen (recfile, "w"))) {
		fprintf (stderr, "startrecl: %s write error\n", recfile);
		return -1;
	}
	fprintf (recfp, "rec %s", string);
	get_time_usr (string);
	fprintf (recfp, "  %s\n", string);
	for (k = 0; k < argc; k++) fprintf (recfp, "\t%s\n", argv[k]);
	fprintf (recfp, "%s\n", rcsid); 
	fclose (recfp);
	return 0;
}

int endrec (void) {
	extern void	get_time_usr (char *string);
	FILE		*recfp;
	char 		string[MAXL];
	
	if (!(recfp = fopen (recfile, "a"))) {
		fprintf (stderr, "endrec: recfile write error\n");
		return -1;
	}
	get_time_usr (string);
	fprintf (recfp, "endrec %s\n", string);
	fclose (recfp);
	return 0;
}

void	get_time_usr (char *string) {
	time_t		time_sec;

	time (&time_sec);
	strcpy (string, ctime (&time_sec));
	string [24] = '\0';
	strcat (string, "  ");
	cuserid (string + 26);
}
