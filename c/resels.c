/*******
resels.c
*******/

/*$Revision: 1.3 $*/

#include <stdlib.h>
#include <stdio.h>
#include <fidl.h>


/**************************/
void resels(
    char region_type,
    int space, /*We need space, because anova 222 images are compressed.*/
    double fwhmx,
    double fwhmy,
    double fwhmz,
    double smoothness,
    double *R0,
    double *R1,
    double *R2,
    double *R3)
/**************************/
{
    double P,Ex,Ey,Ez,Fxy,Fxz,Fyz,C,r0,r11,r12,r13,r21,r22,r23,r3;

    switch(region_type) {
        case 'a':
            switch(space) {
                case 222:
                    P = 298130;
                    Ex = 292255;
                    Ey = 292963;
                    Ez = 292291;
                    Fxy = 287162;
                    Fxz = 286512;
                    Fyz = 287201;
                    C = 281494;
                    break;
                case 333:
                    P = 119195;
                    Ex = 116109;
                    Ey = 116524;
                    Ez = 116138;
                    Fxy = 113456;
                    Fxz = 113089;
                    Fyz = 113490;
                    C = 110455;
                    break;
                default:
                    fprintf(stdout,"Error: No P,Ex,Ey,Ez,Fxy,Fxz,Fyz,C calculated for region. Abort!\n");
                    exit(-1);
                }
            r0 = P-(Ex+Ey+Ez)+(Fyz+Fxz+Fxy)-C;
            r11 = Ex-Fxy-Fxz+C;
            r12 = Ey-Fxy-Fyz+C;
	    r13 = Ez-Fxz-Fyz+C;
	    r21 = Fxy-C;
	    r22 = Fxz-C;
	    r23 = Fyz-C;
	    r3 = C;
	    break;
	case 'b':
            switch(space) {
                case 222:
		    r0 = 1;
		    r11 = (double)I_222 - 1;
		    r12 = (double)J_222 - 1;
		    r13 = (double)K_222 - 1;
		    r21 = ((double)I_222-1)*((double)J_222-1);
		    r22 = ((double)I_222-1)*((double)K_222-1);
		    r23 = ((double)J_222-1)*((double)K_222-1);
		    r3 = ((double)I_222-1)*((double)J_222-1)*((double)K_222-1);
		    break;
		case 333:
                    r0 = 1; 
                    r11 = (double)XDIM_333 - 1; 
                    r12 = (double)YDIM_333 - 1; 
                    r13 = (double)ZDIM_333 - 1; 
                    r21 = ((double)XDIM_333-1)*((double)YDIM_333-1);
                    r22 = ((double)XDIM_333-1)*((double)ZDIM_333-1);
                    r23 = ((double)YDIM_333-1)*((double)ZDIM_333-1);
                    r3 = ((double)XDIM_333-1)*((double)YDIM_333-1)*((double)ZDIM_333-1);
                    break;
		default:
                    fprintf(stdout,"Error: No P,Ex,Ey,Ez,Fxy,Fxz,Fyz,C calculated for region. Abort!\n");
                    exit(-1);
                }
	    break;
	default:
            fprintf(stdout,"Error: Unknown region type. Abort!\n");
            exit(-1);
        }

    if(fwhmx > 0.0 && fwhmy > 0.0 && fwhmz > 0.0) {
	*R0 = r0;
        *R1 = r11/fwhmx + r12/fwhmy + r13/fwhmz;
        *R2 = r21/fwhmx/fwhmy + r22/fwhmx/fwhmz + r23/fwhmy/fwhmz;
        *R3 = r3/fwhmx/fwhmy/fwhmz;
        }
    else {
	*R0 = r0;
        *R1 = (r11+r12+r13)/smoothness;
        *R2 = (r21+r22+r23)/(smoothness*smoothness);
        *R3 = r3/(smoothness*smoothness*smoothness);
        }
}
