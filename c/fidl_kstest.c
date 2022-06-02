/* Copyright 11/29/07 Washington University.  All Rights Reserved.
   fidl_kstest.c  $Revision: 1.2 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_kstest.c,v 1.2 2008/01/02 18:55:34 mcavoy Exp $";

typedef struct {
    double d,prob;
    } KS;
KS *ks2(VEC *a,VEC *b);
double probks(double alam);

main(int argc,char **argv)
{
int i,j,k,nfiles=0,ncombo,count;    
double corrected;
Files_Struct *files;
Data *data;
VEC **x;
PERM *order;
KS **ks;

if(argc < 3) {
    fprintf(stderr,"    -files: *red_ascii_dump_EKG_diag.txt\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    }
if(!nfiles) {
    printf("Error: Need to specify -files\n");
    exit(-1);
    }

if(!(x=malloc(sizeof*x*files->nfiles))) {
    printf("Error: Unable to malloc x\n");
    exit(-1);
    }
ncombo=files->nfiles*(files->nfiles-1)/2.;
if(!(ks=malloc(sizeof*ks*ncombo))) {
    printf("Error: Unable to malloc ks\n");
    exit(-1);
    }

for(i=0;i<files->nfiles;i++) {
    fflush(stdout);
    if(!(data=read_data(files->files[i],0,9,3))) exit(-1); 
    fflush(stdout);
    x[i] = v_get(data->nsubjects);
    order = px_get(data->nsubjects);
    for(j=0;j<data->nsubjects;j++) x[i]->ve[j]=data->x[j][2];
    x[i] = v_sort(x[i],order);
    free_data(data);
    if(px_free(order)) {
        printf("Error: Unable to px_free(order)\n");
        exit(-1);
        }
    } 
for(k=i=0;i<files->nfiles;i++) for(j=i+1;j<files->nfiles;j++,k++) ks[k]=ks2(x[i],x[j]);
#if 0
for(k=i=0;i<files->nfiles;i++) {
    for(j=i+1;j<files->nfiles;j++,k++) {
    /*for(j=0;j<files->nfiles;j++,k++) {*/
        ks[k]=ks2(x[i],x[j]);
        printf("%s\n%s\n\tks[%d]->d=%f\tks->[%d]->prob=%.12f\n",files->files[i],files->files[j],k,ks[k]->d,k,ks[k]->prob); 
        exit(-1);
        }
    }
#endif     

for(count=k=i=0;i<files->nfiles;i++) {
    for(j=i+1;j<files->nfiles;j++,k++) {
        if((corrected=(double)ncombo*ks[k]->prob)<.05) count++;
        printf("%s\n%s\n\tks[%d]->d=%f\tks->[%d]->prob=%.12f corrected=%.12f %s\n",files->files[i],files->files[j],k,ks[k]->d,k,
            ks[k]->prob,corrected,corrected<.05?"*":" "); 
        }
    }        
printf("ncombo=%d count=%d\n",ncombo,count);
}







KS *ks2(VEC *a,VEC *b) /* http://root.cern.ch/root/html/TMath.html#TMath:KolmolgorovTest */
{                  /* (C) Copr. 1986-92 Numerical Recipes Software |oV'41.5.){2p49. */
    int i,ia,ib;
    double sa,sb,x,diff,dmax,en1,en2,en;
    KS *ks;
    if(!(ks=malloc(sizeof*ks))) {
        printf("Error: Unable to malloc ks\n");
        return NULL;
        }
    sa=1./(double)a->dim; sb=1./(double)b->dim;
    for(dmax=diff=0.,ia=ib=i=0;i<a->dim+b->dim;i++) {
        /*printf("a->ve[%d]=%f b->ve[%d]=%f\n",ia,a->ve[ia],ib,b->ve[ib]);*/
        if(a->ve[ia]<b->ve[ib]) {
            diff-=sa;
            if(++ia==a->dim) break;
            }
        else if(a->ve[ia]>b->ve[ib]) {
            diff+=sb;
            if(++ib==b->dim) break;
            }
        else {
            x=a->ve[ia];
            while(a->ve[ia]==x && ia<a->dim) {
                diff-=sa;
                ia++;
                }
            while(b->ve[ib]==x && ib<b->dim) {
                diff+=sb;
                ib++;
                }
            if(ia==a->dim) break;
            if(ib==b->dim) break;
            }
        if((x=fabs(diff))>dmax) dmax=x;
        }
    en1=(double)a->dim; en2=(double)b->dim;
    en=sqrt(en1*en2/(en1+en2));
    /*printf("en1=%f en2=%f en=%f dmax=%f (en+0.12+0.11/en)*dmax=%f\n",en1,en2,en,dmax,(en+0.12+0.11/en)*dmax);*/
    ks->prob=probks((en+0.12+0.11/en)*dmax);
    ks->d=dmax;
    return ks;
}

#if 1
double probks(double alam) /* (C) Copr. 1986-92 Numerical Recipes Software |oV'41.5.){2p49. */
{
    int i;
    double a2,sum,term,termbf,x,fac=2.0,eps1=0.001,eps2=1.e-8;
    a2 = -2.0*alam*alam;
    for(termbf=sum=0.,i=1;i<=100;i++) {
        sum+=term=fac*exp(a2*i*i);
        x=fabs(term);
        if(x<=eps1*termbf || x<eps2*sum) return sum;
        fac=-fac;
        termbf=x;
        }
    return 1.0;
}
#endif

#if 0
#define EPS1 0.001
#define EPS2 1.0e-8
double probks(double alam)
{
        int j;
        double a2,fac=2.0,sum=0.0,term,termbf=0.0;

        a2 = -2.0*alam*alam;
        printf("a2=%f\n",a2);
        for (j=1;j<=100;j++) {
                term=fac*exp(a2*j*j);
                sum += term;
                printf("j=%d sum=%f term=%f EPS1*termbf=%f EPS2*sum=%f\n",j,sum,term,EPS1*termbf,EPS2*sum);
                fflush(stdout);
                if (fabs(term) <= EPS1*termbf || fabs(term) <= EPS2*sum) return sum;
                fac = -fac;
                termbf=fabs(term);
        }
        return 1.0;
}
#endif
