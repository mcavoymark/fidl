/* Copyright 3/10/05 Washington University.  All Rights Reserved.
   fidl_flip.c  $Revision: 1.10 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

typedef struct {
    char *regname;
    int nlines,*len;
    double *mean,*sd;
    } MeanSd;
MeanSd *read_meansdfile(char *meansdfile);

main(int argc,char **argv)
{
char *meanfile=NULL,*sdfile=NULL,*file=NULL,*unflipname=NULL,*flipname=NULL,*maskfile=NULL,*outfile,string[MAXNAME],
    *meansdfile=NULL, *blank="                                                                                   "; 
int i,j,m,vol,vol_mean,vol_sd,atlascoor[3],fidlcoor[3],SunOS_Linux,flipindex,atlas,num_region_files=0,nroi=0,*roi,swapbytes;
float *temp_float,*y,*mean,*sd;
double *temp_double,*meand,*sdd,*yd,*ydflip;
Interfile_header *ifh,*ifh_mean,*ifh_sd;
Atlas_Param *ap;
Mask_Struct *ms;
Files_Struct *region_files;
Regions **reg;
Regions_By_File *rbf;
MeanSd *meansd;
FILE *fp;

if(argc < 9) {
    fprintf(stderr,"    -mean:                Sample mean.\n");
    fprintf(stderr,"    -sd:                  Sample standard deviation\n"); 
    fprintf(stderr,"    -file:                Magnitude image.\n");
    fprintf(stderr,"    -unflip:              Name of unflipped z score.\n");
    fprintf(stderr,"    -flip:                Name of flipped z score image.\n");
    fprintf(stderr,"    -mask:                Limit analysis to voxels within the mask.\n");
    fprintf(stderr,"    -region_file:         *.4dfp.img file(s) that specifies regions of interest.\n");
    fprintf(stderr,"    -regions_of_interest: First region is one.\n");
    fprintf(stderr,"    -out:                 Name of output file for regional analysis.\n");
    fprintf(stderr,"    -mean_and_sd:         Text file containing population mean and sd for a regional analysis.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-mean") && argc > i+1)
        meanfile = argv[++i];
    if(!strcmp(argv[i],"-sd") && argc > i+1)
        sdfile = argv[++i];
    if(!strcmp(argv[i],"-file") && argc > i+1)
        file = argv[++i];
    if(!strcmp(argv[i],"-unflip") && argc > i+1)
        unflipname = argv[++i];
    if(!strcmp(argv[i],"-flip") && argc > i+1)
        flipname = argv[++i];
    if(!strcmp(argv[i],"-mask") && argc > i+1)
        maskfile = argv[++i];
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=get_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroi;
        if(!(roi=malloc(sizeof*roi*nroi))) {
            printf("Error: Unable to malloc roi\n");
            exit(-1);
            }
        for(j=0;j<nroi;j++) roi[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-out") && argc > i+1)
        outfile = argv[++i];
    if(!strcmp(argv[i],"-mean_and_sd") && argc > i+1)
        meansdfile = argv[++i];
    }
if((SunOS_Linux=checkOS())==-1) exit(-1);
if(!(ifh=read_ifh(file))) exit(-1);
swapbytes=shouldiswap(SunOS_Linux,ifh->bigendian);
vol = ifh->dim1*ifh->dim2*ifh->dim3;
if(meanfile) {
    if(!(ifh_mean = read_ifh(meanfile))) exit(-1);
    vol_mean = ifh_mean->dim1*ifh_mean->dim2*ifh_mean->dim3;
    if(!(ifh_sd = read_ifh(sdfile))) exit(-1);
    vol_sd =  ifh_sd->dim1*ifh_sd->dim2*ifh_sd->dim3;
    if(vol != vol_mean || vol != vol_sd) {
        printf("Error: %s has %d voxels\nError: %s has %d voxels\nError: %s has %d voxels\nError: They must all be the same. Abort!"
            ,file,vol,meanfile,vol_mean,sdfile,vol_sd);
        }
    }
else if(meansdfile) {
    if(!(meansd=read_meansdfile(meansdfile))) exit(-1);
    }
else {
    printf("Error: Need to provid -mean and -sd or -mean_and_sd\n");
    exit(-1);
    }

if(!(temp_float=malloc(sizeof*temp_float*vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
if(num_region_files) {
    if(!check_dimensions(num_region_files,region_files->files,vol)) exit(-1);
    if(!(reg=malloc(sizeof*reg*num_region_files))) {
        printf("Error: Unable to malloc reg\n");
        exit(-1);
        }
    for(m=0;m<num_region_files;m++) {
        if(!readstack(region_files->files[m],(float*)temp_float,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
        if(!(reg[m] = extract_regions(region_files->files[m],0,vol,temp_float,0,SunOS_Linux,(char**)NULL))) exit(-1);
        }
    if(!nroi) for(m=0;m<num_region_files;m++) nroi += reg[m]->nregions;
    if(!(rbf=find_regions_by_file_cover(num_region_files,nroi,reg,roi))) exit(-1);
    }
if(!(ms=get_mask_struct(maskfile,maskfile?vol:rbf->nvoxels,maskfile?(int*)NULL:rbf->indices,SunOS_Linux,(LinearModel*)NULL))) exit(-1);

if(!(y=malloc(sizeof*y*vol))) {
    printf("Error: Unable to malloc y\n");
    exit(-1);
    }
if(!readstack(file,(float*)y,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
if(meanfile) {
    if(!(mean=malloc(sizeof*mean*vol))) {
        printf("Error: Unable to malloc mean\n");
        exit(-1);
        }
    if(!(sd=malloc(sizeof*sd*vol))) {
        printf("Error: Unable to malloc sd\n");
        exit(-1);
        }
    if(!readstack(meanfile,(float*)mean,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    if(!readstack(sdfile,(float*)sd,sizeof(float),(size_t)vol,SunOS_Linux)) exit(-1);
    }

atlas = get_atlas(vol);
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL))) exit(-1);
if(!num_region_files) {
    for(i=0;i<vol;i++) temp_float[i] = 0.;
    if(unflipname) {
        for(i=0;i<ms->lenbrain;i++)
            temp_float[ms->brnidx[i]] = (float)(((double)y[ms->brnidx[i]]-(double)mean[ms->brnidx[i]])/(double)sd[ms->brnidx[i]]);
        min_and_max(temp_float,vol,&ifh->global_min,&ifh->global_max);
        if(!writestack(unflipname,temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
        if(!write_ifh(unflipname,ifh,(int)FALSE)) exit(-1);
        printf("Unflipped written to %s\n",unflipname);
        }
    if(flipname) {
        for(i=0;i<ms->lenbrain;i++) {
            voxel_index_to_fidl_and_atlas(ms->brnidx[i],ap->xdim,ap->ydim,ap->zdim,ap->center,ap->mmppix,atlascoor,fidlcoor);
            flipindex = (atlascoor[2]-1)*ap->xdim*ap->ydim + atlascoor[1]*ap->xdim - atlascoor[0];
                                                                                   /*HERE IS THE FLIP - THE MINUS SIGN.*/
            /*temp_float[ms->brnidx[i]] = (float)(((double)y[ms->brnidx[i]]-(double)mean[flipindex])/(double)sd[flipindex]);*/
            temp_float[flipindex] = (float)(((double)y[flipindex]-(double)mean[ms->brnidx[i]])/(double)sd[ms->brnidx[i]]);
                /*Doing it this way prevents an edge artifact.*/
            }
        min_and_max(temp_float,vol,&ifh->global_min,&ifh->global_max);
        if(!writestack(flipname,temp_float,sizeof(float),(size_t)vol,swapbytes)) exit(-1);
        if(!write_ifh(flipname,ifh,(int)FALSE)) exit(-1);
        printf("Flipped written to %s\n",flipname);
        }
    }
else {
    if(!(temp_double=malloc(sizeof*temp_double*vol))) {
        printf("Error: Unable to malloc temp_double\n");
        exit(-1);
        }
    if(meanfile) {
        if(!(meand=malloc(sizeof*meand*nroi))) {
            printf("Error: Unable to malloc meand\n");
            exit(-1);
            }
        if(!(sdd=malloc(sizeof*sdd*nroi))) {
            printf("Error: Unable to malloc sdd\n");
            exit(-1);
            }
        for(i=0;i<vol;i++) temp_double[i] = mean[i]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL : (double)mean[i];
        crs(temp_double,meand,rbf,(char*)NULL);
        for(i=0;i<vol;i++) temp_double[i] = sd[i]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL : (double)sd[i];
        crs(temp_double,sdd,rbf,(char*)NULL);
        }
    else {
        meand = meansd->mean;
        sdd = meansd->sd;
        }
        
    if(!(yd=malloc(sizeof*yd*nroi))) {
        printf("Error: Unable to malloc yd\n");
        exit(-1);
        }
    if(!(ydflip=malloc(sizeof*ydflip*nroi))) {
        printf("Error: Unable to malloc ydflip\n");
        exit(-1);
        }
    for(i=0;i<vol;i++) temp_double[i] = y[i]==(float)UNSAMPLED_VOXEL ? (double)UNSAMPLED_VOXEL : (double)y[i];
    crs(temp_double,yd,rbf,(char*)NULL);
    for(i=0;i<rbf->nvoxels;i++) {
        voxel_index_to_fidl_and_atlas(rbf->indices[i],ap->xdim,ap->ydim,ap->zdim,ap->center,ap->mmppix,atlascoor,fidlcoor);
        rbf->indices[i] = (atlascoor[2]-1)*ap->xdim*ap->ydim + atlascoor[1]*ap->xdim - atlascoor[0];
        }                                                                            /*HERE IS THE FLIP - THE MINUS SIGN.*/
    crs(temp_double,ydflip,rbf,(char*)NULL);

    for(j=i=0;i<nroi;i++,j++) temp_float[j] = (float)((yd[i]-meand[i])/sdd[i]);
    for(i=0;i<nroi;i++,j++) temp_float[j] = (float)((ydflip[i]-meand[i])/sdd[i]);

    if(!(fp=fopen_sub(outfile,"w"))) exit(-1);
    strcpy(string,blank);
    string[rbf->max_length+21]=0;
    fprintf(fp,"%sunflipped         flipped\n",string);
    strcpy(string,blank);
    string[rbf->max_length+3]=0;
    fprintf(fp,"%spop mean pop sd   sub mean z        sub mean z\n",string);
    /*fprintf(fp,"%spop mean\tpop sd\tsub mean\tz\tsub mean\tz\n",string);*/
    for(i=0;i<nroi;i++) {
        fprintf(fp,"%s%*.4f %8.4f %8.4f %8.4f %8.4f %8.4f\n",rbf->region_names_ptr[i],rbf->max_length-rbf->length[i]+10,meand[i],
            sdd[i],yd[i],temp_float[i],ydflip[i],temp_float[i+nroi]);
        /*fprintf(fp,"%s%*.4f\t%8.4f\t%8.4f\t%8.4f\t%8.4f\t%8.4f\n",rbf->region_names_ptr[i],rbf->max_length-rbf->length[i]+10,
              meand[i],sdd[i],yd[i],temp_float[i],ydflip[i],temp_float[i+nroi]);*/
        }
    fclose(fp);
    printf("Output written to %s\n",outfile);
    }
exit(0);
}

MeanSd *read_meansdfile(char *meansdfile)
{
    char *strptr,line[MAXNAME],write_back[MAXNAME];
    int i,len,nlines,nstrings;
    MeanSd *meansd;
    FILE *fp;
    if(!(fp = fopen_sub(meansdfile,"r"))) return NULL;
    if(!(meansd=malloc(sizeof*meansd))) {
        printf("Error: Unable to malloc meansd\n");
        return NULL;
        }
    for(len=0,meansd->nlines=-1;fgets(line,sizeof(line),fp);) {
        if((nstrings=count_strings(line,write_back,' ')) == 3) {
            if(++meansd->nlines>0) {
                grab_string(write_back,line);
                len += strlen(line);
                } 
            }
        }
    if(!(meansd->regname=malloc(sizeof*meansd->regname*(++len)))) {
        printf("Error: Unable to malloc meansd->regname\n");
        return NULL;
        }
    if(!(meansd->len=malloc(sizeof*meansd->len*meansd->nlines))) {
        printf("Error: Unable to malloc meansd->len\n");
        return NULL;
        }
    if(!(meansd->mean=malloc(sizeof*meansd->mean*meansd->nlines))) {
        printf("Error: Unable to malloc meansd->mean\n");
        return NULL;
        }
    if(!(meansd->sd=malloc(sizeof*meansd->sd*meansd->nlines))) {
        printf("Error: Unable to malloc meansd->sd\n");
        return NULL;
        }
    for(rewind(fp),i=0,nlines=-1;fgets(line,sizeof(line),fp);) {
        if((nstrings=count_strings(line,write_back,' ')) == 3) {
            if(++nlines>0) { 
                strptr=grab_string(write_back,line);
                meansd->len[i] = strlen(line)+1;
                strcat(meansd->regname,line);
                strptr=grab_string(strptr,line);
                meansd->mean[i] = atof(line);
                strptr=grab_string(strptr,line);
                meansd->sd[i++] = atof(line);
                }
            }
        }
    return meansd;
}
