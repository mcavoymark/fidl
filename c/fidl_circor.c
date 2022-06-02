/* Copyright 2/24/06 Washington University.  All Rights Reserved.
   fidl_circor.c  $Revision: 1.3 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>


static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_circor.c,v 1.3 2006/05/02 16:44:01 mcavoy Exp $";

main(int argc,char **argv)
{
char filename[MAXNAME],*mask_file=NULL;
int i,j,k,jj,nl=0,nr=0,nindroots=0,SunOS_Linux,*nimage,lcuniform=0,lcgeneral=0;
float *tfl,*tfr,*rpos,*rneg,rposmax,rposmin,rnegmax,rnegmin,
      *chisquare,cmax,cmin; 
double add,sub,*cossub,*sinsub,*cosadd,*sinadd,
       costhe,sinthe,cosphi,sinphi,
       *sum_costhe,*sum_sinthe,*sum_cosphi,*sum_sinphi,
       *sum2_costhe,*sum2_sinthe,*sum2_cosphi,*sum2_sinphi,
       *ct_cp,*ct_sp,*st_cp,*st_sp,*ct_st,*cp_sp,
       SSct,SSst,SScp,SSsp,SSct_cp,SSct_sp,SSst_cp,SSst_sp,SSct_st,SScp_sp,rcc,rcs,rsc,rss,r1,r2,R2;
Files_Struct *lfiles,*rfiles,*indroots;
Interfile_header *ifh;
Dim_Param *dpl,*dpr;
Mask_Struct *ms;
Memory_Map *mml,*mmr;

print_version_number(rcsid,stderr);

#if 0
printf("sin(60.*(double)M_PI/180.)=%f\n",sin(60.*(double)M_PI/180.));
printf("sin(420.*(double)M_PI/180.)=%f\n",sin(420.*(double)M_PI/180.));
printf("sin(-15.*(double)M_PI/180.)=%f\n",sin(-15.*(double)M_PI/180.));
exit(-1);
#endif

if(argc < 7) {
    fprintf(stderr,"    -l:        Lefties.\n");
    fprintf(stderr,"    -r:        Righties.\n");
    fprintf(stderr,"    -out:      Average of ratios.\n");
    fprintf(stderr,"    -indroots: Individual ratios.\n");
    fprintf(stderr,"    -mask:     Only voxels within the mask are analyzed.\n");
    fprintf(stderr,"    -uniform:  Asuumes angles uniformly distributed on the unit circle.\n");
    fprintf(stderr,"               Output is positive and negative correlations.\n");
    fprintf(stderr,"    -general:  General solution.\n");
    fprintf(stderr,"               Output is a chisquare statistic with four degrees of freedom.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-l") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nl;
        if(!(lfiles=get_files(nl,&argv[i+1]))) exit(-1);
        i += nl;
        }
    if(!strcmp(argv[i],"-r") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nr;
        if(!(rfiles=get_files(nr,&argv[i+1]))) exit(-1);
        i += nr;
        }
    if(!strcmp(argv[i],"-indroots") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nindroots;
        if(!(indroots=get_files(nindroots,&argv[i+1]))) exit(-1);
        i += nindroots;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-uniform"))
        lcuniform = 1;
    if(!strcmp(argv[i],"-general"))
        lcgeneral = 1;
    }
if(!lcuniform && !lcgeneral) {
    printf("Need to specify either -uniform or -general\n"); 
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(nl!=nr) {
    printf("nl=%d nr=%d They must be equal. Abort!\n",nl,nr);
    exit(-1);
    }
if(nindroots) {
    if(nindroots != nl) {
        printf("nl=%d nindroots=%d They must be equal. Abort!\n",nl,nindroots);
        exit(-1);
        }
    }
if(!(ifh=read_ifh(lfiles->files[0]))) exit(-1); /*just for output*/
if(!(dpl=dim_param(lfiles->nfiles,lfiles->files))) exit(-1);
if(!(dpr=dim_param(rfiles->nfiles,rfiles->files))) exit(-1);
if(!(ms=get_mask_struct(mask_file,dpl->vol,(int*)NULL,SunOS_Linux))) exit(-1);
if(dpl->vol!=dpr->vol || dpl->vol!=ms->lenvol) {
    printf("Error: dpl->vol=%d dpr->vol=%d ms->lenvol=%d  They must be equal.\n",dpl->vol,dpr->vol,ms->lenvol);
    }
for(i=0;i<lfiles->nfiles;i++) {
    if(dpl->tdim[i]!=dpr->tdim[i]) {
         printf("Error: %s has %d frames\nError: %s has %d frames\nError: Must be equal. Abort!\n",lfiles->files[i],dpl->tdim[i],
             rfiles->files[i],dpr->tdim[i]);
        exit(-1);
        }
    }
if(!(tfl=malloc(sizeof*tfl*ms->lenbrain))) {
    printf("Error: Unable to malloc tfl\n");
    exit(-1);
    }
if(!(tfr=malloc(sizeof*tfr*ms->lenbrain))) {
    printf("Error: Unable to malloc tfr\n");
    exit(-1);
    }
if(!(cossub=malloc(sizeof*cossub*ms->lenbrain))) {
    printf("Error: Unable to malloc cossub\n");
    exit(-1);
    }
if(!(sinsub=malloc(sizeof*sinsub*ms->lenbrain))) {
    printf("Error: Unable to malloc sinsub\n");
    exit(-1);
    }
if(!(cosadd=malloc(sizeof*cosadd*ms->lenbrain))) {
    printf("Error: Unable to malloc cosadd\n");
    exit(-1);
    }
if(!(sinadd=malloc(sizeof*sinadd*ms->lenbrain))) {
    printf("Error: Unable to malloc sinadd\n");
    exit(-1);
    }
if(!(nimage=malloc(sizeof*nimage*ms->lenbrain))) {
    printf("Error: Unable to malloc nimage\n");
    exit(-1);
    }
if(!(rpos=malloc(sizeof*rpos*dpl->vol))) {
    printf("Error: Unable to malloc rpos\n");
    exit(-1);
    }
if(!(rneg=malloc(sizeof*rneg*dpl->vol))) {
    printf("Error: Unable to malloc rneg\n");
    exit(-1);
    }

/*START0*/
if(!(sum_costhe=malloc(sizeof*sum_costhe*ms->lenbrain))) {
    printf("Error: Unable to malloc sum_costhe\n");
    exit(-1);
    }
if(!(sum_sinthe=malloc(sizeof*sum_sinthe*ms->lenbrain))) {
    printf("Error: Unable to malloc sum_sinthe\n");
    exit(-1);
    }
if(!(sum_cosphi=malloc(sizeof*sum_cosphi*ms->lenbrain))) {
    printf("Error: Unable to malloc sum_cosphi\n");
    exit(-1);
    }
if(!(sum_sinphi=malloc(sizeof*sum_sinphi*ms->lenbrain))) {
    printf("Error: Unable to malloc sum_sinphi\n");
    exit(-1);
    }

if(!(sum2_costhe=malloc(sizeof*sum2_costhe*ms->lenbrain))) {
    printf("Error: Unable to malloc sum2_costhe\n");
    exit(-1);
    }
if(!(sum2_sinthe=malloc(sizeof*sum2_sinthe*ms->lenbrain))) {
    printf("Error: Unable to malloc sum2_sinthe\n");
    exit(-1);
    }
if(!(sum2_cosphi=malloc(sizeof*sum2_cosphi*ms->lenbrain))) {
    printf("Error: Unable to malloc sum2_cosphi\n");
    exit(-1);
    }
if(!(sum2_sinphi=malloc(sizeof*sum2_sinphi*ms->lenbrain))) {
    printf("Error: Unable to malloc sum2_sinphi\n");
    exit(-1);
    }

if(!(ct_cp=malloc(sizeof*ct_cp*ms->lenbrain))) {
    printf("Error: Unable to malloc ct_cp\n");
    exit(-1);
    }
if(!(ct_sp=malloc(sizeof*ct_sp*ms->lenbrain))) {
    printf("Error: Unable to malloc ct_sp\n");
    exit(-1);
    }
if(!(st_cp=malloc(sizeof*st_cp*ms->lenbrain))) {
    printf("Error: Unable to malloc st_cp\n");
    exit(-1);
    }
if(!(st_sp=malloc(sizeof*st_sp*ms->lenbrain))) {
    printf("Error: Unable to malloc st_sp\n");
    exit(-1);
    }
if(!(ct_st=malloc(sizeof*ct_st*ms->lenbrain))) {
    printf("Error: Unable to malloc ct_st\n");
    exit(-1);
    }
if(!(cp_sp=malloc(sizeof*cp_sp*ms->lenbrain))) {
    printf("Error: Unable to malloc cp_sp\n");
    exit(-1);
    }

if(!(chisquare=malloc(sizeof*chisquare*dpl->vol))) {
    printf("Error: Unable to malloc chisquare\n");
    exit(-1);
    }


for(i=0;i<lfiles->nfiles;i++) {

    #if 0
    for(j=0;j<ms->lenbrain;j++) {
        cossub[j]=sinsub[j]=cosadd[j]=sinadd[j]=0.;
        nimage[j]=0;
        }
    #endif

    /*START0*/
    for(j=0;j<ms->lenbrain;j++) {
        cossub[j]=sinsub[j]=cosadd[j]=sinadd[j]=0.;
        sum_costhe[j]=sum_sinthe[j]=sum_cosphi[j]=sum_sinphi[j]=0.;
        sum2_costhe[j]=sum2_sinthe[j]=sum2_cosphi[j]=sum2_sinphi[j]=0.;
        ct_cp[j]=ct_sp[j]=st_cp[j]=st_sp[j]=ct_st[j]=cp_sp[j]=0.;
        nimage[j]=0;
        }
    if(!(mml=map_disk(lfiles->files[i],dpl->tdim[i]*dpl->vol,0))) exit(-1);
    if(!(mmr=map_disk(rfiles->files[i],dpr->tdim[i]*dpr->vol,0))) exit(-1);



    #if 1
    for(jj=j=0;j<dpl->tdim[i];j++,jj+=dpl->vol) {
        for(k=0;k<ms->lenbrain;k++) {
            tfl[k] = mml->ptr[jj+ms->brnidx[k]];
            tfr[k] = mmr->ptr[jj+ms->brnidx[k]];
            }
        if(SunOS_Linux) {
            swap_bytes((unsigned char *)tfl,sizeof(float),(size_t)ms->lenbrain);
            swap_bytes((unsigned char *)tfr,sizeof(float),(size_t)ms->lenbrain);
            }
        for(k=0;k<ms->lenbrain;k++) {
            if(tfl[k]!=(float)UNSAMPLED_VOXEL && tfr[k]!=(float)UNSAMPLED_VOXEL) {
                sub = (double)tfl[k]-(double)tfr[k];
                add = (double)tfl[k]+(double)tfr[k];
                cossub[k] += cos(sub);
                sinsub[k] += sin(sub);
                cosadd[k] += cos(add);
                sinadd[k] += sin(add);
                nimage[k]++;
    #endif

    /*START CHECK Batch p.179*/
    #if 0
    for(j=0;j<8;j++) {
        if(j==0) {
            tfl[0]=60.; tfr[0]=120.;
            }
        else if(j==1) {
            tfl[0]=90.; tfr[0]=135.;
            }
        else if(j==2) {
            tfl[0]=135.; tfr[0]=210.;
            }
        else if(j==3) {
            tfl[0]=150.; tfr[0]=150.;
            }
        else if(j==4) {
            tfl[0]=210.; tfr[0]=195.;
            }
        else if(j==5) {
            tfl[0]=240.; tfr[0]=240.;
            }
        else if(j==6) {
            tfl[0]=255.; tfr[0]=300.;
            }
        else if(j==7) {
            tfl[0]=300.; tfr[0]=315.;
            }
        else {
            printf("Should not be here\n");
            }
        tfl[0]*=(float)M_PI/180.; tfr[0]*=(float)M_PI/180.;
        for(k=0;k<1;k++) {
                nimage[k]++;
    #endif
    /*END CHECK Batch p.179*/



                costhe = cos((double)tfl[k]);
                sinthe = sin((double)tfl[k]);
                cosphi = cos((double)tfr[k]);
                sinphi = sin((double)tfr[k]);

                sum_costhe[k] += costhe;
                sum_sinthe[k] += sinthe;
                sum_cosphi[k] += cosphi;
                sum_sinphi[k] += sinphi;

                sum2_costhe[k] += costhe*costhe;
                sum2_sinthe[k] += sinthe*sinthe;
                sum2_cosphi[k] += cosphi*cosphi;
                sum2_sinphi[k] += sinphi*sinphi;

                ct_cp[k] += costhe*cosphi; 
                ct_sp[k] += costhe*sinphi;
                st_cp[k] += sinthe*cosphi;
                st_sp[k] += sinthe*sinphi;
                ct_st[k] += costhe*sinthe;
                cp_sp[k] += cosphi*sinphi; 

                }

            }
        }


    for(j=0;j<dpl->vol;j++) rpos[j]=rneg[j]=chisquare[j]=0.;
    for(rposmax=rnegmax=cmax=0.,rposmin=rnegmin=1.,cmin=1.e20,j=0;j<ms->lenbrain;j++) {
        if(nimage[j]) {
            rpos[ms->brnidx[j]] = (float)(sqrt(cossub[j]*cossub[j]+sinsub[j]*sinsub[j])/(double)nimage[j]);
            rneg[ms->brnidx[j]] = (float)(sqrt(cosadd[j]*cosadd[j]+sinadd[j]*sinadd[j])/(double)nimage[j]);
            if(rpos[ms->brnidx[j]]>rposmax) rposmax=rpos[ms->brnidx[j]];
            if(rpos[ms->brnidx[j]]<rposmin) rposmin=rpos[ms->brnidx[j]]; 
            if(rneg[ms->brnidx[j]]>rnegmax) rnegmax=rneg[ms->brnidx[j]];
            if(rneg[ms->brnidx[j]]<rnegmin) rnegmin=rneg[ms->brnidx[j]];

            SSct = sum2_costhe[j] - sum_costhe[j]*sum_costhe[j]/(double)nimage[j];
            SSst = sum2_sinthe[j] - sum_sinthe[j]*sum_sinthe[j]/(double)nimage[j];
            SScp = sum2_cosphi[j] - sum_cosphi[j]*sum_cosphi[j]/(double)nimage[j];
            SSsp = sum2_sinphi[j] - sum_sinphi[j]*sum_sinphi[j]/(double)nimage[j];

            SSct_cp = ct_cp[j] - sum_costhe[j]*sum_cosphi[j]/(double)nimage[j];
            SSct_sp = ct_sp[j] - sum_costhe[j]*sum_sinphi[j]/(double)nimage[j];
            SSst_cp = st_cp[j] - sum_sinthe[j]*sum_cosphi[j]/(double)nimage[j];
            SSst_sp = st_sp[j] - sum_sinthe[j]*sum_sinphi[j]/(double)nimage[j];
            SSct_st = ct_st[j] - sum_costhe[j]*sum_sinthe[j]/(double)nimage[j];
            SScp_sp = cp_sp[j] - sum_cosphi[j]*sum_sinphi[j]/(double)nimage[j]; 

            rcc = SSct_cp/sqrt(SSct*SScp);
            rcs = SSct_sp/sqrt(SSct*SSsp);
            rsc = SSst_cp/sqrt(SSst*SScp);
            rss = SSst_sp/sqrt(SSst*SSsp);
            r1 = SSct_st/sqrt(SSct*SSst);
            r2 = SScp_sp/sqrt(SScp*SSsp);
            
            R2 = (rcc*rcc+rcs*rcs+rsc*rsc+rss*rss+2.*(rcc*rss+rcs*rsc)*r1*r2-2*(rcc*rcs+rsc*rss)*r2-2.*(rcc*rsc+rcs*rss)*r1) /
                 ((1.-r1*r1)*(1.-r2*r2));

            /*printf("rcc=%f rcs=%f rsc=%f rss=%f r1=%f r2=%f R2=%f nimage[%d]=%d chisquare=%f\n",
                rcc,rcs,rsc,rss,r1,r2,R2,j,nimage[j],(double)nimage[j]*R2);*/
            /*exit(-1);*/

            chisquare[ms->brnidx[j]] = (float)((double)nimage[j]*R2);
            if(chisquare[ms->brnidx[j]]>cmax) cmax=chisquare[ms->brnidx[j]];
            if(chisquare[ms->brnidx[j]]<cmin) cmin=chisquare[ms->brnidx[j]];
            }
        }



    #if 0
    sprintf(filename,"%s_rpos.4dfp.img",indroots->files[i]);
    if(!writestack(filename,rpos,sizeof(float),(size_t)dpl->vol,SunOS_Linux)) exit(-1);
    if(ifh->file_name) free(ifh->file_name);
    j = strlen(lfiles->files[i]) + strlen(rfiles->files[i]) + 2;
    if(!(ifh->file_name=malloc(sizeof*ifh->file_name*j))) {
        printf("Error: Unable to malloc ifh->file_name\n");
        exit(-1);
        }
    sprintf(ifh->file_name,"%s %s\n",lfiles->files[i],rfiles->files[i]);
    ifh->global_max = rposmax;
    ifh->global_min = rposmin;
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    printf("Output written to %s\n",filename);

    sprintf(filename,"%s_rneg.4dfp.img",indroots->files[i]);
    if(!writestack(filename,rneg,sizeof(float),(size_t)dpl->vol,SunOS_Linux)) exit(-1);
    ifh->global_max = rnegmax;
    ifh->global_min = rnegmin;
    if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
    printf("Output written to %s\n",filename);
    #endif

    if(ifh->file_name) free(ifh->file_name);
    j = strlen(lfiles->files[i]) + strlen(rfiles->files[i]) + 2;
    if(!(ifh->file_name=malloc(sizeof*ifh->file_name*j))) {
        printf("Error: Unable to malloc ifh->file_name\n");
        exit(-1);
        }
    sprintf(ifh->file_name,"%s %s\n",lfiles->files[i],rfiles->files[i]);
    ifh->dim4 = 1;
    
    if(lcuniform) {
        sprintf(filename,"%s_rpos.4dfp.img",indroots->files[i]);
        if(!writestack(filename,rpos,sizeof(float),(size_t)dpl->vol,SunOS_Linux)) exit(-1);
        ifh->global_max = rposmax;
        ifh->global_min = rposmin;
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
        printf("Output written to %s\n",filename);

        sprintf(filename,"%s_rneg.4dfp.img",indroots->files[i]);
        if(!writestack(filename,rneg,sizeof(float),(size_t)dpl->vol,SunOS_Linux)) exit(-1);
        ifh->global_max = rnegmax;
        ifh->global_min = rnegmin;
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
        printf("Output written to %s\n",filename);
        }
    if(lcgeneral) {
        sprintf(filename,"%s_chisquare_circor.4dfp.img",indroots->files[i]);
        if(!writestack(filename,chisquare,sizeof(float),(size_t)dpl->vol,SunOS_Linux)) exit(-1);
        ifh->global_max = cmax;
        ifh->global_min = cmin;
        ifh->dof_condition = 4.;
        if(!write_ifh(filename,ifh,(int)FALSE)) exit(-1);
        printf("Output written to %s\n",filename);
        }

    if(!unmap_disk(mml)) exit(-1);
    if(!unmap_disk(mmr)) exit(-1);
    }
}
