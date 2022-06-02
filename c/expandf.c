#include	<string.h>
#include	<stdio.h>

#define MAXNAME 5000
#define TRUE 1
#define FALSE 0

/*$Revision: 12.80 $*/
 
void main(argc,argv)
 
int     argc;
char    *argv[];
 
{


char	passed_string[MAXNAME],*input_string;

int	stat;

if(argc < 2) {
        printf("Usage: expandf encoded_paradigm\n");
	printf("encoded paradigm: ASCII definition of stimulation time-course as used by actmap.\n");
        exit(-1);
        }
input_string = argv[1];

strcpy(passed_string,input_string);
stat = expandf(passed_string,MAXNAME);
/*printf("%d %s %d\n",stat,test,strlen(test));*/
fprintf(stdout,"%s",passed_string);

}


/**************************/
int	expandf (strin, len)
/**************************/

	char	*strin;
	int	len;

{

	char	*stringt, *string;	/* buffers */
	char	*lp;			/* left delimeter */
	char	*np;			/* ascii integer */
	char	*rp;			/* right delimiter */
	char	c2 [2];
	int	c, k, l;
	int	len2;
	int	level;
	int	num;
	int	status;
	int	debug = 0;

	status = 0;
	memset (c2, '\0', 2);
	len2 = len * 2;
	stringt = (char *) calloc (len2 + 1, sizeof (char));
	string  = (char *) calloc (len2 + 1, sizeof (char));

	/* Squeeze out white space. */
	rp = strin;
	lp = string;
	while (c = *rp++) if (!isspace (c)) *lp++ = c;
	*lp = '\0';
	if (debug) {l = strlen (string); printf ("%s\t%d\n", string, l);}

	/* Expand parentheses. */
	level = 0;
	while (rp = strrchr (string, ')')) {
		*rp = '\0';
		lp = rp;
		while (lp > string && isdigit (c = *(lp - 1))) *--lp = '\0';
		level++;
		while ((level > 0) && (lp > string)) {
			lp--;
			if (*lp == ')') level++;
			if (*lp == '(') level--;
		}
		if (level) {
			printf ("expandf error: unbalanced parentheses\n");
			status = 1; goto DONE;
		}
		*lp = '\0';
		num = 1;
		np = lp;
		while (np > string && isdigit (c = *(np - 1))) np--;
		if (strlen (np) > 0) {
			num = atoi (np);		/* printf ("num=%d\n", num); */
			*np = '\0';
		}
		strcpy (stringt, string);
		for (k = 0; k < num; k++) strncat (stringt, lp + 1, len2);
		strncat (stringt, rp + 1, len2);
		strncpy (string, stringt, len2);	/* printf ("%s\n", string); */
	}
	if (strrchr (string, '(')) {
		printf ("expandf error: unbalanced parentheses\n");
		status = 1; goto DONE;
	}
	if (debug) {l = strlen (string); printf ("%s\t%d\n", string, l);}

	/* Expand multiples. */
	while (np = strpbrk (string, "0123456789")) {
		rp = np;
		while (isdigit (c = *++rp));
		strncpy (c2, rp, 1);
		*rp = '\0';
		num = atoi (np);			/* printf ("num=%d\n", num); */
		*np = '\0';
		strcpy (stringt, string);
		for (k = 0; k < num; k++) strncat (stringt, c2, len2);
		strncat (stringt, rp + 1, len2);
		strncpy (string, stringt, len2);	/* printf ("%s\n", string); */
	}
	if (debug) {l = strlen (string); printf ("%s\t%d\n", string, l);}

DONE:	l = strlen (string);
	if (l >= len) {
		printf ("expandf error: expanded string too long\n");
		string [len - 1] = '\0';
		status = 1;
	}
	strcpy (strin, string);
	free (string);
	free (stringt);
	return (status);
}
