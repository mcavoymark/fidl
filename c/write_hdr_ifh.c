/*$Id: write_hdr_ifh.c,v 12.80 1999/03/24 21:37:58 jmo Exp $*/
/*$Log: write_hdr_ifh.c,v $
 * Revision 12.80  1999/03/24  21:37:58  jmo
 * Code for fidl rev 2.04
 *
 * Revision 12.70  1999/03/02  17:00:39  fidl
 * RCS moved to fidl account
 *
 * Revision 12.65  1999/02/16  20:29:36  jmo
 * Fix error in mult_comp
 *
 * Revision 12.60  1999/02/12  23:27:54  jmo
 * Bug fix.  Rev 2.01
 *
 * Revision 12.55  1999/01/29  21:04:42  jmo
 * Rev 2.0
 *
 * Revision 12.50  1999/01/29  00:08:18  jmo
 * fidl Rev 2.0
 *
 * Revision 2.0  1998/08/10  22:59:21  jmo
 * see_version.txt
 *
 * Revision 1.2  1997/07/16  18:54:16  jmo
 * *** empty log message ***
 *
 * Revision 1.1  1996/12/27  21:57:28  jmo
 * Initial revision
 **/

/*_________________________________________________________________
  Program:	ecat2analyze

  Description:

  Authors:	Avi Snyder

  History:	06-May-96.	
_________________________________________________________________*/

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <petutil/ANALYZE.h>			 /* dsr */

#define MAXLINE 256

static char     rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/write_hdr_ifh.c,v 12.80 1999/03/24 21:37:58 jmo Exp $";

main (argc, argv)
	int		argc;
	char           *argv[];
{
	FILE			*hdrfp;			/* output ANALYZE hdr */
	FILE			*ifhfp;			/* output Interfile Format header */
	FILE			*ANAfp;			/* output ANALYZE image  */
	char			fileroot [MAXLINE];
	char			imgfile [MAXLINE];
	char			ANAfile [MAXLINE];
	char			hdrfile [MAXLINE];
	char			ifhfile [MAXLINE];
	char			*string;
	unsigned char		*imgB;
	float			*imgt;
	float			*imgr;
	float			mmppixr [3];
	float			centerr [3];
	float			mmppixA [3];
	float			maxval, minval;
	float			pixel_size;
	float			plane_separation;
	int			bitpix = 16;
	int			delz = 0;
	int			dimension;
	int			img_max;
	int			img_min;
	int			i, j, k;
	int			nxr, nyr, nzr;
	int			nxA = 128;
	int			nyA = 256;
	int			nzA = 256;
	int			ntA = 1;
	int			npixA;
	int			resample = 0;
	int			scanner;
	int			status = 0;
	void			*imgV;
	short			*imgA;
	short			width;			/* input image */
	short			height;			/* input image */
	short			num_slices;		/* input image */
	struct dsr		hdr;			/* ANALYZE header */

	/* Get command line arguments. */
	if (argc < 9) {
	    printf ("Usage: write_hdr_ifh file_name xdim ydim zdim tdim img_min img_max bits_per_pixel x_pix_siz y_pix_siz z_pix_siz\n");
	    exit (-1);
	    }

	strcpy (imgfile, argv [1]);
	strcpy (fileroot, imgfile);
	if ((string = strrchr (fileroot, '.'))) *string = '\0';
	strcpy (hdrfile, fileroot); strcat (hdrfile, ".hdr");
	strcpy (ifhfile, fileroot); strcat (ifhfile, ".ifh");

	nxA = atoi(argv[2]);
	nyA = atoi(argv[3]);
	nzA = atoi(argv[4]);
	ntA = atoi(argv[5]);
	npixA = nxA * nyA * nzA * ntA;
	img_min = atof(argv[6]);
	img_max = atof(argv[7]);
	bitpix = atoi(argv[8]);
	mmppixA[0] = atof(argv[9]);
	mmppixA[1] = atof(argv[10]);
	mmppixA[2] = atof(argv[11]);

	/* Create ANALYZE hdr file. */
	strncpy (hdr.hk.db_name, imgfile, 17);
	hdr.hk.sizeof_hdr = sizeof (struct dsr); /* required by developers */
	hdr.hk.extents = 16384;			 /* recommended by developers  */
	hdr.hk.regular = 'r';			 /* required by developers */
	hdr.dime.dim[0] = 4;			 /* typically 4 dimensions  */
	hdr.dime.dim[1] = nxA;			 /* x dimension */
	hdr.dime.dim[2] = nyA;			 /* y dimension */
	hdr.dime.dim[3] = nzA;			 /* z dimesnion */
	hdr.dime.dim[4] = ntA;			 /* number of volumes */
	hdr.dime.unused8 = 0;			 /* despite the label, some of this is used */
	hdr.dime.unused9 = 0;
	hdr.dime.unused10 = 0;
	hdr.dime.unused11 = 0;
	hdr.dime.unused12 = 0;
	hdr.dime.unused13 = 0;
	hdr.dime.unused14 = 0;
	hdr.dime.datatype = 16;
	hdr.dime.bitpix = bitpix;
	hdr.dime.pixdim[1] = mmppixA [0];
	hdr.dime.pixdim[2] = mmppixA [1];
	hdr.dime.pixdim[3] = mmppixA [2];
	hdr.dime.funused8 = 0;
	hdr.dime.funused9 = 0;
	hdr.dime.funused10 = 0;
	hdr.dime.funused11 = 0;
	hdr.dime.funused12 = 0;
	hdr.dime.funused13 = 0;
	hdr.dime.glmax = img_max;
	hdr.dime.glmin = img_min;
	strncpy (hdr.hist.descrip, imgfile, 79);
	strcpy (hdr.hist.originator, "fidl");
	strncpy (hdr.hist.patient_id, "1.0", 4);
	hdr.hist.orient = 'U';

	if ((hdrfp = fopen (hdrfile, "w")) == 0) {
/*		printf ("Unable to create ANALYZE header %s\n", hdrfile);*/
		fprintf(stdout,"-1");
	}
	if ((fwrite (&hdr, sizeof (struct dsr), 1, hdrfp)) != 1) {
/*		printf ("Error writing %s\n", hdrfile);*/
		fprintf(stdout,"-1");
	}
	close (hdrfp);

	/* Create Interfile Format header file */
	if ((ifhfp = fopen (ifhfile, "w")) == 0) {
	/*	printf ("Unable to create Interfile Format header %s\n", ifhfile);*/
		fprintf(stdout,"-1");
	}
	fprintf (ifhfp, "INTERFILE :=\n");
	fprintf (ifhfp, "version of keys    := 3.3\n");
	fprintf (ifhfp, "number format  := float\n");
	fprintf (ifhfp, "number of bytes per pixel  := 4\n");
	fprintf (ifhfp, "orientation             := 2\n");
	fprintf (ifhfp, "number of dimensions   := 4\n");
	fprintf (ifhfp, "matrix size [1]    := %d\n", nxA);
	fprintf (ifhfp, "matrix size [2]    := %d\n", nyA);
	fprintf (ifhfp, "matrix size [3]    := %d\n", nzA);
	fprintf (ifhfp, "matrix size [4]    := 1\n");
	fprintf (ifhfp, "scaling factor (mm/pixel) [1]  := %f\n", mmppixA [0]);
	fprintf (ifhfp, "scaling factor (mm/pixel) [2]  := %f\n", mmppixA [1]);
	fprintf (ifhfp, "scaling factor (mm/pixel) [3]  := %f\n", mmppixA [2]);
	fprintf (ifhfp, "matrix initial element [1] := right\n");
	fprintf (ifhfp, "matrix initial element [2] := posterior\n");
	fprintf (ifhfp, "matrix initial element [3] := inferior\n");
	close (ifhfp);

fprintf(stdout,"0");

}
