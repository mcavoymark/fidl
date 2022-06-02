/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   fidl_4dfp_to_text.c  $Revision: 1.8 $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>
static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_4dfp_to_text.c,v 1.8 2012/08/31 21:35:27 mcavoy Exp $";
main(int argc,char **argv)
{
char *mask_file=NULL,*output="fidl_4dfp_to_text.txt",*strptr;
int atlas,nfiles=0,SunOS_Linux;
size_t i,j,k,l,m;
float *temp_float,**temp_float2;
double *col,*row,*slice,*coor,*dptr;
FILE *fp;
Dim_Param *dp;
Files_Struct *files;
Mask_Struct *ms;
/*Memory_Map **mm;*/
Atlas_Param *ap;
print_version_number(rcsid,stdout);
if(argc < 7) {
    fprintf(stderr,"    -files:  4dfp or conc.\n");
    fprintf(stderr,"    -mask:   Only voxels within the mask are printed.\n"); 

    /*START120831*/
    /*fprintf(stderr,"    -output: Output filename.\n");*/

    exit(-1);
    }
for(i=1;i<argc;i++) { 

    #if 0
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1;i+j < argc && strchr(argv[i+j],'-') != argv[i+j];j++) ++nfiles;
        if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
        i += nfiles;
        }
    #endif
    /*START120831*/
    if(!strcmp(argv[i],"-files") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nfiles;
        if(nfiles) {
            strptr = strrchr(argv[i+1],'.');
            if(!strcmp(strptr,".conc")) {
                if(nfiles>1) {printf("fidlError: Only set up to handle a single conc. Abort!\n");fflush(stdout);exit(-1);}
                if(!(files=read_conc(argv[++i]))) exit(-1);
                }
            else if(!strcmp(strptr,".img")){
                if(!(files=get_files(nfiles,&argv[i+1]))) exit(-1);
                i += nfiles;
                }
            else {
                printf("Error: -file not conc or img. Abort!\n");fflush(stdout);exit(-1);
                }
            }
        }

    if(!strcmp(argv[i],"-mask") && argc > i+1)
        mask_file = argv[++i];

    /*START120831*/
    #if 0
    if(!strcmp(argv[i],"-output") && argc > i+1)
        output = argv[++i];
    #endif

    }
if(!nfiles) {
    printf("Error: No files specified with -files option. Abort!\n");
    exit(-1);
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);

/*if(!(dp=dim_param(files->nfiles,files->files,SunOS_Linux))) exit(-1);*/
/*START120831*/
if(!(dp=dim_param(files->nfiles,files->files,SunOS_Linux,0))) exit(-1);


/*if(!(ms=get_mask_struct(mask_file,dp->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL))) exit(-1);*/
/*START120831*/
if(!(ms=get_mask_struct(mask_file,dp->vol,(int*)NULL,SunOS_Linux,(LinearModel*)NULL,dp->vol))) exit(-1);

if(ms->lenvol!=dp->vol) {
    printf("Error: mask and files not in the same space. Abort!\n");
    exit(-1);
    }

#if 0
if(!(mm=malloc(sizeof*mm*nfiles))) {
    printf("Error: Unable to malloc mm\n");
    exit(-1);
    }
for(i=0;i<nfiles;i++) if(!(mm[i] = map_disk(files->files[i],dp->vol*dp->tdim[i],0))) exit(-1);
#endif
/*START120831*/
if(!(temp_float=malloc(sizeof*temp_float*dp->lenvol_max))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_float2=malloc(sizeof*temp_float*ms->lenbrain*dp->tdim_max))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(!(temp_float2=d2float(files->nfiles,ms->lenbrain*dp->tdim_max))) {
    printf("Error: Unable to malloc temp_float2\n");
    exit(-1);
    }
for(i=0;i<files->nfiles;i++) {
    if(!readstack(files->files[i],(float*)temp_float,sizeof(float),(size_t)dp->lenvol[i],SunOS_Linux)) exit(-1);
    for(m=l=j=0;j<dp->tdim[i];j++,l+=dp->vol) for(k=0;k<ms->lenbrain;k++,m++) temp_float2[i][m] = temp_float[l+ms->brnidx[k]];
    }

/*START120831*/
atlas=get_atlas(dp->vol);
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(!(coor=malloc(sizeof*coor*ms->lenbrain*3))) {
    printf("Error: Unable to malloc coor\n");
    exit(-1);
    }
if(!(col=malloc(sizeof*col*ms->lenbrain))) {
    printf("Error: Unable to malloc col\n");
    exit(-1);
    }
if(!(row=malloc(sizeof*row*ms->lenbrain))) {
    printf("Error: Unable to malloc row\n");
    exit(-1);
    }
if(!(slice=malloc(sizeof*slice*ms->lenbrain))) {
    printf("Error: Unable to malloc slice\n");
    exit(-1);
    }
col_row_slice(ms->lenbrain,ms->brnidx,col,row,slice,ap);
get_atlas_coor(ms->lenbrain,col,row,slice,(double)ap->zdim,ap->center,ap->mmppix,coor);




if(!(fp=fopen_sub(output,"w"))) exit(-1);


#if 0
if(mask_file) { 
    if(!(strptr=get_tail_sans_ext(mask_file))) exit(-1);
    fprintf(fp,"%s\n",strptr);
    }
#endif
/*START120831*/
if(mask_file) fprintf(fp,"mask %s\n\n",mask_file);

#if 0
for(i=0;i<nfiles;i++) {
    if(!(strptr=get_tail_sans_ext(files->files[i]))) exit(-1);
    fprintf(fp,"%s",strptr);
    if(dp->tdim[i] > 1) {
        fprintf(fp,"_1");
        for(j=1;j<dp->tdim[i];j++) fprintf(fp,", %s_%d",strptr,j+1);
        }
    if(i<nfiles-1) fprintf(fp,", ");
    }
#endif
/*START120831*/
/*for(i=0;i<files->nfiles;i++) fprintf(fp,"%d\t%s\t%d\tframes\n",i,files->files[i],dp->tdim[i]); fprintf(fp,"\n");*/
for(i=0;i<files->nfiles;i++) fprintf(fp,"%-3d%s %d frames\n",i,files->files[i],dp->tdim[i]); fprintf(fp,"\n");




#if 0
fprintf(fp,"\n");
for(k=0;k<ms->lenbrain;k++) {
    for(i=0;i<nfiles;i++) { 
        for(j=0;j<dp->tdim[i];j++) { 

            fprintf(fp,"%.7g",mm[i]->ptr[j*dp->vol+ms->brnidx[k]]);

            if(j<dp->tdim[i]-1) fprintf(fp,", ");
            }
        if(i<nfiles-1) fprintf(fp,", ");
        }
    fprintf(fp,"\n");
    }
#endif
/*START120831*/
#if 0
/*fprintf(fp,"\nx   y   z   ");*/
fprintf(fp,"\nx\ty\tz\t");
for(i=0;i<files->nfiles;i++) for(j=0;j<dp->tdim[i];j++) fprintf(fp,"%d_%d\t",i,j); fprintf(fp,"\n");
for(dptr=coor,k=0;k<ms->lenbrain;k++,dptr+=3) {
    /*fprintf(fp,"%3d %3d %3d ",(int)rint(*dptr),(int)rint(*(dptr+1)),(int)rint(*(dptr+2)));*/
    fprintf(fp,"%3d\t%3d\t%3d\t",(int)rint(*dptr),(int)rint(*(dptr+1)),(int)rint(*(dptr+2)));
    for(i=0;i<files->nfiles;i++) {
        for(j=0;j<dp->tdim[i];j++) {
            fprintf(fp,"%g\t",mm[i]->ptr[j*dp->vol+ms->brnidx[k]]);
            }
        }
    fprintf(fp,"\n");
    }
#endif
fprintf(fp,"\nx\ty\tz\t");
for(i=0;i<files->nfiles;i++) for(j=0;j<dp->tdim[i];j++) fprintf(fp,"%d_%d\t",i,j); fprintf(fp,"\n");
for(dptr=coor,k=0;k<ms->lenbrain;k++,dptr+=3) {
    fprintf(fp,"%3d\t%3d\t%3d\t",(int)rint(*dptr),(int)rint(*(dptr+1)),(int)rint(*(dptr+2)));
    for(i=0;i<files->nfiles;i++) {
        for(j=0;j<dp->tdim[i];j++) {
            fprintf(fp,"%g\t",temp_float2[i][j*ms->lenbrain+k]);
            }
        }
    fprintf(fp,"\n");
    }


fclose(fp);
printf("Output written to %s\n",output);
exit(0);
}
