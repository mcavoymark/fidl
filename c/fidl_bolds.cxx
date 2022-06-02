/* Copyright 6/19/17 Washington University.  All Rights Reserved.
   fidl_bolds.cxx  $Revision: 1.23 $ */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <gsl/gsl_cdf.h>
#include "fidl.h"
#include "gauss_smoth2.h"
#include "read_frames_file.h"
#include "mask.h"
#include "dim_param2.h"
#include "get_atlas_param.h"
#include "subs_util.h"
#include "t4_atlas.h"
#include "shouldiswap.h"
#include "read_tc_string_new.h"
#include "filetype.h"
#include "subs_cifti.h"
#include "subs_nifti.h"
#include "write1.h"
#include "minmax.h"
#include "checkOS.h"
#include "timestr.h"

//#include "find_regions_by_file_cover.h"
//START211017
#include "find_regions_by_file_cover3.h"

#include "map_disk.h"
#include "get_atlas.h"
#include "read_xform.h"
#include "twoA_or_twoB.h"
#include "check_dimensions.h"
#include "ptr.h"
#include "lookuptable.h"

//#include "lut.h"
//START211017
#include "lut2.h"

char const* Fileext[]={"",".4dfp.img",".dscalar.nii",".nii",".dtseries.nii"};

//static char rcsid[] = "$Header: /home/hannah/mcavoy/idl/clib/RCS/fidl_bolds.c,v 1.23 2017/05/09 16:43:33 mcavoy Exp $";

int main(int argc,char **argv){
char string[MAXNAME],filename[MAXNAME],*mask_file=NULL,*frames_file=NULL,string2[MAXNAME],write_back[MAXNAME],*scratchdir0=NULL,
    *strptr=NULL,fwhmstr[10],*string_ptr,normstr[4],*avgacrossfiles=NULL,*label=NULL,atlas[7]="",
    filename0[MAXNAME],*cleanup=NULL,*sumacrossfiles=NULL,string3[MAXNAME],*lutf=NULL,*concf=NULL; //scratchdir0=NULL,timestr[23]
int i,j,k,l,n=0,p=0,q,r,ii,jj,num_regions=0,num_region_files=0,num_tc_files=0,*roi=NULL,lc_names_only=0,  //swapbytes=0,
    *A_or_B_or_U=NULL,chunks=1,dummy,sum_tdim=0,num_frames=0,argc_tc_weights=0,num_tc_weights=0,
    argc_frames=0,chunkvol=0,nxform_file=0,nt4_identify=0,*t4_identify=NULL,*nframe,lccompress=0,*nsubframereg=NULL,nscratchdir=0,lcflip=0,
    lcroiIND=0,dontaddmissingFSreg=0,nregnames=0,nroinames=0,roinamesi=-1,lcrms=0,*superbird=NULL;
    //*msbrnidx=NULL,mslenbrain,msvol
int64_t dims[3];
size_t i1,j1;
float *t4=NULL,*temp_float=NULL,fwhm=0.,*temp_float2=NULL;
double *temp_double=NULL,*stat=NULL,*mean=NULL,*subframereg=NULL,*sum_framereg,*sum2_framereg,TR=0.,starttime=UNSAMPLED_VOXEL,*td1=NULL,*dptr,td=0.;
    //*meanvox=NULL;
FILE *fp,*fp1,*fp2;
Interfile_header *ifh=NULL,*ifh0=NULL;
Regions **reg=NULL;
Regions_By_File *rbf=NULL,*fbf=NULL;
Dim_Param2 *dp;
Atlas_Param *ap;
FS *fs;
TCnew *tc_weights=NULL;
Files_Struct *tc_files=NULL,*region_files=NULL,*xform_file=NULL,*scratchdir=NULL,*regnames=NULL,*roinames=NULL;
gauss_smoth2_struct *gs=NULL;
W1 *w1=NULL;
XmlS *xmls;
 
//mask ms;
//START210131
mask ms0,ms_reg0,*ms=NULL,*ms_reg=NULL;

if(argc < 5) {
    std::cout<<"  -tc_files:            .nii's, .nii.gz's or conc(s) or list(s)."<<std::endl;
    std::cout<<"  -region_file:         *.4dfp.img file(s) that specifies regions of interest."<<std::endl;
    std::cout<<"                        Time courses are averaged over the region."<<std::endl;
    std::cout<<"  -regions_of_interest: Compute timecourses for selected regions in the region file(s)."<<std::endl;
    std::cout<<"                        First region is one."<<std::endl;
    
    //START211103
    std::cout<<"  -rms                  Regional averages are computed in quadrature as RMS values."<<std::endl;
    std::cout<<"                        This is the proper way to do it for standard deviations."<<std::endl;

    //START210512
#if 0 
    std::cout<<"  -roinames:            List of roi names. Use with -lut to specify which regions to use."<<std::endl;
    std::cout<<"                        With -conc a separate file is created for each -tc_files and region"<<std::endl;
#endif
    std::cout<<"  -roinames:            List(s) of freesurfer regions. Use with -lut to specify which regions to use."<<std::endl;
    std::cout<<"                        Use *.list or *.conc so fidl knows to read the region names."<<std::endl;
    std::cout<<"                        With -conc a separate file is created for each -tc_files and region"<<std::endl;

#if 0
    std::cout<<"  -cluster: One file per cluster. List of freesurfer regions. Name of file becomes name of cluster."<<std::endl;
    std::cout<<"            Use *.list or *.conc so fidl knows to read the region names."<<std::endl;
#endif

    //START210511
    std::cout<<"  -regnames:            Use with clusters so that the region is named in the text file for plotting."<<std::endl;

    std::cout<<"  -frames:              File containing frames of interest. First frame is 1."<<std::endl;
    std::cout<<"  -frames_of_interest:  Frames of interest. First frame is 1."<<std::endl;
    std::cout<<"  -names_only           Generate filenames for creation of the anova driving file."<<std::endl;
    std::cout<<"                        No computation is performed. No files are created."<<std::endl;
    std::cout<<"  -xform_file:          t4 file defining the transform to atlas space."<<std::endl;
    std::cout<<"  -t4:                  For use with more than a single t4. Identifies which t4 goes with each file."<<std::endl;
    std::cout<<"                        First t4 is 1. Need a number for each file."<<std::endl;
    std::cout<<"  -atlas:               Either 111, 222, 333, 222MNI . Use with -xform_file option."<<std::endl;
    std::cout<<"  -compress:            Only voxels in the mask are analyzed."<<std::endl;
    std::cout<<"  -mask:                Only voxels in the mask are analyzed. Output dimensions are kept."<<std::endl;
    std::cout<<"  -chunks               Breaks the compressed anova scratch files into chunks."<<std::endl;
    std::cout<<"                        This is done for large designs that could exceed the 2 gig limit."<<std::endl;
    std::cout<<"  -scratchdir:          Files are output to this location. Include the backslash at the end."<<std::endl;
    std::cout<<"                        Multiple directories ok. Same files written to all."<<std::endl;
    std::cout<<"  -gauss_smoth:         Amount of smoothing to be done in units of voxels at fwhm with a 3D gaussian filter."<<std::endl;
    std::cout<<"  -normalize ALL        Normalize to the mean of all frames in the file."<<std::endl;
    std::cout<<"  -tc_weights           Weighting to apply to summed timepoints (eg contrast)."<<std::endl;
    //std::cout<<"  -swapbytes            Swap bytes of output stack."<<std::endl;
    std::cout<<"  -avgacrossfiles:      Name of output file."<<std::endl;
    std::cout<<"  -sumacrossfiles:      Output root."<<std::endl;

    //std::cout<<"  -label:               Condition label for output text file."<<std::endl;
    //START210505
    std::cout<<"  -label:               Condition label for output text file and for .nii.gz"<<std::endl;

    std::cout<<"  -TR:                  TR for output text file."<<std::endl;
    std::cout<<"  -starttime:           Time for first line of output text file. Time is incremented by TR."<<std::endl;
    std::cout<<"  -cleanup:             Remove directory."<<std::endl;
    std::cout<<"  -lut:                 Lookup table. Ex. $FREESURFER_HOME/FreeSurferColorLUT.txt"<<std::endl;

    //START210420
    std::cout<<"  -dontaddmissingFSreg  If freesurfer regions 72:5th-Ventricle and 80:non-WM-hypointensities are missing, then they won't be added."<<std::endl;
    std::cout<<"                        These regions are added for plotting, so that the tables have the same number of columns."<<std::endl;

    //START210505
    std::cout<<"  -conc:                Write conc file with the given name."<<std::endl;
    
    exit(-1);
    }
fwhmstr[0] = 0;
normstr[0] = 0;
for(i=1;i<argc;i++){
    if(!strcmp(argv[i],"-tc_files") && argc > i+1){
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++)++num_tc_files;
        if(!(tc_files=read_files(num_tc_files,&argv[i+1])))exit(-1);
        i+=num_tc_files;
        }
    if(!strcmp(argv[i],"-region_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_region_files;
        if(!(region_files=read_files(num_region_files,&argv[i+1]))) exit(-1);
        i += num_region_files;
        }
    if(!strcmp(argv[i],"-regions_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_regions;
        if(num_regions){
            if(!strcmp(argv[i+1],"INDIVIDUAL")){
                lcroiIND=1;++i;
                }
            else{

                #if 0
                if(!(roi=(int*)malloc(sizeof*roi*num_regions))) {
                    printf("fidlError: Unable to malloc roi\n");
                    exit(-1);
                    }
                #endif
                //START210512
                roi=new int[num_regions];

                for(j=0;j<num_regions;j++) roi[j] = atoi(argv[++i]) - 1;
                }
            }
        }

    //START211103
    if(!strcmp(argv[i],"-rms"))
        lcrms = 1;
    
    //START210512
    #if 0 
    if(!strcmp(argv[i],"-roinames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroinames;
        if(!(roinames=get_files(nroinames,&argv[i+1]))) exit(-1);
        i += nroinames;
        }
    #endif
    if(!strcmp(argv[i],"-roinames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nroinames;
        if(!(roinames=read_files(nroinames,&argv[roinamesi=i+1]))) exit(-1);
        i += nroinames;
        }

    //START210511
    if(!strcmp(argv[i],"-regnames") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nregnames;
        if(!(regnames=get_files(nregnames,&argv[i+1]))) exit(-1);
        i += nregnames;
        }

    if(!strcmp(argv[i],"-frames") && argc > i+1)
        frames_file = argv[++i];
    if(!strcmp(argv[i],"-frames_of_interest") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++num_frames;
        argc_frames = i+1; 
        i += num_frames;
        }
    if(!strcmp(argv[i],"-names_only"))
        lc_names_only = 1;
    if(!strcmp(argv[i],"-xform_file") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nxform_file;
        if(!(xform_file=get_files(nxform_file,&argv[i+1]))) exit(-1);
        i += nxform_file;
        }
    if(!strcmp(argv[i],"-t4") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nt4_identify;
        if(!(t4_identify=(int*)malloc(sizeof*t4_identify*nt4_identify))){
            std::cout<<"fidlError: Unable to malloc t4_identify"<<std::endl;
            exit(-1);
            }
        for(j=0;j<nt4_identify;j++) t4_identify[j] = atoi(argv[++i]) - 1;
        }
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
        strcpy(atlas,argv[++i]);
    if(!strcmp(argv[i],"-compress") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
        mask_file = argv[++i];
        lccompress=1;
        }
    if(!strcmp(argv[i],"-mask") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        mask_file = argv[++i];
    if(!strcmp(argv[i],"-chunks"))
        chunks = 20;
    if(!strcmp(argv[i],"-scratchdir") && argc > i+1) {
        for(j=1; i+j < argc && strchr(argv[i+j],'-') != argv[i+j]; j++) ++nscratchdir;
        if(!(scratchdir=get_files(nscratchdir,&argv[i+1]))) exit(-1);
        i += nscratchdir;
        }
    if(!strcmp(argv[i],"-gauss_smoth") && argc > i+1) {
        fwhm = atof(argv[++i]);
        sprintf(fwhmstr,"_fwhm%.1f",fwhm);
        }
    if(!strcmp(argv[i],"-normalize") && argc > i+1) {
        if(argc > i+1 && !strcmp(argv[i+1],"ALL")) {
            strcpy(normstr,"_NA");
            ++i;
            }
        }
    if(!strcmp(argv[i],"-tc_weights") && argc > i+1) {
        argc_tc_weights = i+1;
        for(j=1;i+j<argc;j++) {
            string_ptr = argv[i+j] + 1;
            if(*string_ptr == '.') string_ptr++;
            if(strchr(argv[i+j],'-') == argv[i+j] && !isdigit(*string_ptr)) break;
            ++num_tc_weights;
            }
        i += num_tc_weights;
        }
    //if(!strcmp(argv[i],"-swapbytes"))
    //    swapbytes = 1;
    if(!strcmp(argv[i],"-avgacrossfiles") && argc > i+1)
        avgacrossfiles = argv[++i];

    //START170616
    if(!strcmp(argv[i],"-sumacrossfiles") && argc > i+1)
        sumacrossfiles = argv[++i];

    if(!strcmp(argv[i],"-label") && argc > i+1)
        label = argv[++i];
    if(!strcmp(argv[i],"-TR") && argc > i+1)
        TR = atof(argv[++i]);
    if(!strcmp(argv[i],"-starttime") && argc > i+1)
        starttime = atof(argv[++i]);
    if(!strcmp(argv[i],"-cleanup") && argc > i+1 && strchr(argv[i+1],'-') != argv[i+1])
        cleanup=argv[++i];

    //START210129
    if(!strcmp(argv[i],"-lut")) {
        if(argc > i+1 && strchr(argv[i+1],'-') != argv[i+1]) {
            lutf = argv[++i];
            }
        else {
            std::cout<<"fidlError: No lookup table specified after -lut option. Abort!"<<std::endl;
            exit(-1);
            }
        }

    //START210420
    if(!strcmp(argv[i],"-dontaddmissingFSreg"))
        dontaddmissingFSreg=1;

    //START210505
    if(!strcmp(argv[i],"-conc") && argc > i+1)
        concf = argv[++i];

    }
//if(!lc_names_only)print_version_number(rcsid,stdout);

if(!num_tc_files){std::cout<<"fidlError: No timecourse files. Abort!"<<std::endl;exit(-1);}

#if 0
//START210505
std::ofstream concofs;
std::string pwd;
if(concf){
    concofs.open(concf,std::ofstream::out);
    pwd.assign(getenv("PWD"));
    } 
#endif
//START210512
std::ofstream concofs;
std::string pwd;
std::vector<std::string> concfs; 
//std::vector<std::ofstream*> concfsofs;
std::vector<std::shared_ptr<std::ofstream> > concfsofs;
if(concf){
    pwd.assign(getenv("PWD"));
    if(!nroinames){
        concofs.open(concf,std::ofstream::out);
        }
    else{
        chunks=roinames->nfiles;
        }
    } 
    

if(nt4_identify) {
    if(nt4_identify!=(int)tc_files->nfiles) {
        printf("fidlError: nt4_identify=%d num_tc_files=%zd Must be equal. Abort!\n",nt4_identify,tc_files->nfiles);
        exit(-1);
        }
    fprintf(stderr,"t4_identify="); for(i=0;i<nt4_identify;i++) fprintf(stderr,"%d ",t4_identify[i]); fprintf(stderr,"\n");
    }
else {
    if(nxform_file) if(xform_file->nfiles>1) {printf("fidlError: Need to specify -t4\n");fflush(stdout);exit(-1);} 
    if(!(t4_identify=(int*)malloc(sizeof*t4_identify*tc_files->nfiles))){
        std::cout<<"fidlError: Unable to malloc t4_identify"<<std::endl;
        exit(-1);
        }
    for(i1=0;i1<tc_files->nfiles;i1++) t4_identify[i1]=0;
    }

//START220113
timestr t0;
std::string scratchdir1;

if(!scratchdir&&lccompress){

    //if(!(scratchdir0=make_scratchdir(1,(char*)"SCRATCH")))exit(-1);
    //START220113
    scratchdir1="SCRATCH";scratchdir1+=t0._timestr();scratchdir1+="/";
    std::string cmd="mkdir "+scratchdir1;
    if(system(cmd.c_str())==-1){std::cout<<"fidlError: "<<cmd<<std::endl;exit(-1);}
    scratchdir0=(char*)scratchdir1.c_str();
 

    if(!(scratchdir=(Files_Struct*)malloc(sizeof*scratchdir))){
        std::cout<<"fidlError: Unable to malloc scratchdir"<<std::endl;
        exit(-1);
        }
    scratchdir->nfiles=1;
    scratchdir->files=&scratchdir0;
    }
if(!nscratchdir)nscratchdir=1;
if(num_frames) {
    if(!(frames_file=(char*)malloc(sizeof*frames_file*MAXNAME))) {
        std::cout<<"fidlError: Unable to malloc frames_file"<<std::endl;
        exit(-1);
        }

    //sprintf(frames_file,"%sfidlframes%s.dat",scratchdir?scratchdir->files[0]:"",make_timestr2(timestr));
    //START210428
    timestr t0;
    sprintf(frames_file,"%sfidlframes%s.dat",scratchdir?scratchdir->files[0]:"",t0._timestr());

    if(!(fp = fopen_sub(frames_file,"w"))) exit(-1);
    for(j=0;j<num_frames;j++) fprintf(fp,"%s\n",argv[argc_frames+j]);
    fclose(fp);
    }
if(!(fs=read_frames_file(frames_file,0,0,0))) exit(-1);

/*printf("num_tc_weights=%d fs->ntc=%d\n",num_tc_weights,fs->ntc);
printf("fs->num_frames_to_sum="); for(i=0;i<fs->ntc;i++) printf("%d ",fs->num_frames_to_sum[i]); printf("\n");*/
/*printf("fs->nframes=%d\n",fs->nframes);
printf("fs->frames="); for(i=0;i<fs->nframes;i++) printf("%d ",fs->frames[i]); printf("\n");*/

if(num_tc_weights) {
    if(!(tc_weights=read_tc_string_TCnew(num_tc_weights,fs->num_frames_to_sum,argc_tc_weights,argv,'+'))) exit(-1);
    }
if(!(dp=dim_param2(tc_files->nfiles,tc_files->files)))exit(-1);
if(!xform_file&&!(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES))get_atlas(dp->volall,atlas); //unknown atlas ok
if(!(ap=get_atlas_param(atlas,(Interfile_header*)NULL,tc_files->files[0])))exit(-1);
if(!xform_file)ap->vol = dp->volall;
if(!atlas[0]){
    ap->xdim=dp->xdim[0];
    ap->ydim=dp->ydim[0];
    ap->zdim=dp->zdim[0];
    ap->voxel_size[0]=dp->dx[0];
    ap->voxel_size[1]=dp->dy[0];
    ap->voxel_size[2]=dp->dz[0];
    }

if(!lc_names_only) {

    //START210131
    //if(!(ms.get_mask(mask_file,dp->volall,(int*)NULL,(LinearModel*)NULL,dp->volall))) exit(-1);
    //chunkvol=(int)ceil((double)ms.lenbrain/(double)chunks);

    if(xform_file) {
        if(!(t4=(float*)malloc(sizeof*t4*(size_t)T4SIZE*xform_file->nfiles))) {
            printf("fidlError: Unable to malloc t4\n");
            exit(-1);
            }
        if(!(A_or_B_or_U=(int*)malloc(sizeof*A_or_B_or_U*xform_file->nfiles))) {
            printf("fidlError: Unable to malloc A_or_B_or_U\n");
            exit(-1);
            }
        for(i1=0;i1<xform_file->nfiles;i1++) {
            if(!read_xform(xform_file->files[i1],&t4[i1*(int)T4SIZE])) exit(-1);
            if((A_or_B_or_U[i1]=twoA_or_twoB(xform_file->files[i1])) == 2) exit(-1);
            }
        }
    if(region_files){
        if(!check_dimensions(region_files->nfiles,region_files->files,ap->vol))exit(-1);
        if(!(reg=(Regions**)malloc(sizeof*reg*region_files->nfiles))){
            std::cout<<"fidlError: Unable to malloc reg"<<std::endl;
            exit(-1);
            }
        for(i1=0;i1<region_files->nfiles;++i1)if(!(reg[i1]=get_reg(region_files->files[i1],0,(float*)NULL,0,(char**)NULL,0,lc_names_only,0,(int*)NULL,lutf)))exit(-1);
        size_t l1;
        if(!lcroiIND){
            int num_regions1;
            for(num_regions1=l1=0;l1<region_files->nfiles;l1++) num_regions1 += (int)reg[l1]->nregions;
            if(num_regions>num_regions1) {
                num_regions = num_regions1;
                }
            else if(!num_regions) {
                num_regions = num_regions1;

                #if 0
                if(!(roi=(int*)malloc(sizeof*roi*num_regions))) {
                    printf("fidlError: Unable to malloc roi\n");
                    exit(-1);
                    }
                #endif
                //START210512
                roi=new int[num_regions]; 

                for(j=0;j<num_regions;j++) roi[j] = j;
                }
            }
        else{

            #if 0
            for(num_regions=reg[0]->nregions,l1=1;l1<region_files->nfiles;l1++){
                if(reg[l1]->nregions!=num_regions){
                    std::cout<<"fidlError: reg["<<l1<<"]->nregions="<<reg[l1]->nregions<<" num_regions="<<num_regions<<" Must be the same. Abort!"<<std::endl;
                    }
                }
            #endif
            //START210512
            if(!nroinames){
                for(num_regions=reg[0]->nregions,l1=1;l1<region_files->nfiles;l1++){
                    if(reg[l1]->nregions!=num_regions){
                        std::cout<<"fidlError: reg["<<l1<<"]->nregions="<<reg[l1]->nregions<<" num_regions="<<num_regions<<" Must be the same. Abort!"<<std::endl;
                        }
                    }
                }  
            else{
                num_regions=roinames->nfiles;
                }

            }


        if(!nroinames){

            //if(!(rbf=find_regions_by_file_cover(!lcroiIND?region_files->nfiles:0,!lcroiIND?num_regions:0,reg,roi))) exit(-1);
            //START211017
            if(!(rbf=find_regions_by_file_cover(!lcroiIND?region_files->nfiles:0,!lcroiIND?num_regions:0,reg,roi,NULL))) exit(-1);

            }
        else{

            lut l0;
            l0.lut2(lutf);
            roi=new int[roinames->nfiles]; 
            for(k=i=0;i<nroinames;++i){

                //START210526
                //std::string str(argv[roinamesi+i]);
                //str=str.substr(str.find_last_of("/")+1,str.find_last_of(".")-str.find_last_of("/")-1);

                for(j=0;j<roinames->nfileseach[i];++j,++k){
                    auto it=l0.LUT.begin();
                    for(;it!=l0.LUT.end();++it){
                        if(!strcmp(it->second.c_str(),roinames->files[k])){
                            roi[k]=it->first;
                            break;
                            }
                        }
                    if(it==l0.LUT.end()){
                        std::cout<<"****************** "<<roinames->files[k]<<" not found ******************"<<std::endl;
                        exit(-1);
                        }
                    }
                }
            if(concf){
                std::string str(concf);
                concfs.resize(roinames->nfiles);
                for(size_t i=0;i<roinames->nfiles;++i){
                    concfs[i]=str+"_"+roinames->files[i]+".conc";

                    //std::ofstream* of=new std::ofstream(concfs[i]);
                    //concfsofs.push_back(of);  
                    
                    std::shared_ptr<std::ofstream> out(new std::ofstream);
                    out->open(concfs[i].c_str());
                    concfsofs.push_back(out); 
                    }
                //for(size_t i=0;i<roinames->nfiles;++i)std::cout<<"concfs["<<i<<"]="<<concfs[i]<<std::endl;;
                }
            }


        //START210511
        if(regnames){
            if(num_regions!=(int)regnames->nfiles){
                std::cout<<"num_regions="<<num_regions<<" regnames->nfiles="<<regnames->nfiles<<" Must be equal. Abort!"<<std::endl;
                exit(-1);
                } 
            //rbf->region_names_ptr=regnames->files;
            }

        //START180302
        //lcflip=1;
        //START180319 what you really want to check is c_orient or the string derived from it
        if(dp->filetypeall!=get_filetype(region_files->files[0]))lcflip=1;
        std::cout<<"lcflip="<<lcflip<<std::endl;

        }

//#if 0
    if(xform_file||fwhm>0.)if(!(ms0.get_mask((char*)NULL,dp->volall,(int*)NULL,(LinearModel*)NULL,dp->volall)))exit(-1);
    ms=&ms0;
//#if 0

    //if(!lcroiIND)if(!(ms_reg0.get_mask(mask_file,!rbf?dp->volall:rbf->nindices_uniqsort,!rbf?(int*)NULL:rbf->indices_uniqsort,(LinearModel*)NULL,ap->vol)))exit(-1);
    //START210419
    if(!lcroiIND||!num_region_files)if(!(ms_reg0.get_mask(mask_file,!rbf?dp->volall:rbf->nindices_uniqsort,!rbf?(int*)NULL:rbf->indices_uniqsort,(LinearModel*)NULL,ap->vol)))exit(-1);
//#if 0

    ms_reg=&ms_reg0;
    if(!xform_file&&fwhm<=0.)ms=ms_reg;
    if(!num_region_files) {
        num_regions=ms_reg->lenbrain;
        if(ms_reg->vol!=dp->volall){std::cout<<"fidlError: fidl_bolds ms_reg->vol="<<ms_reg->vol<<" dp->volall="<<dp->volall<<" Must be equal."<<std::endl;exit(-1);}
        }
//#if 0

    //if((lcroiIND&&!roinames)||lutf){
    //START220314
    if(avgacrossfiles||sumacrossfiles){

        superbird = new int[num_regions](); //empty parentheses zero intializes

        //START220314 Why does i1 start at 1 and not 0?
        if(region_files)for(i1=1;i1<region_files->nfiles;i1++)for(i=0;i<reg[i1]->nregions;++i)if(reg[i1]->nvoxels_region[i])superbird[i]++;
        //if(region_files){
        //    for(i1=1;i1<region_files->nfiles;i1++)for(i=0;i<reg[i1]->nregions;++i)std::cout<<"reg["<<i1<<"]->nvoxels_region["<<i<<"]="<<reg[i1]->nvoxels_region[i]<<std::endl;;
        //    }

    //START220314
    //    }
    //if(avgacrossfiles||sumacrossfiles){

        //std::cout<<"here0 num_regions="<<num_regions<<" num_region_files="<<num_region_files<<" ms_reg->lenbrain="<<ms_reg->lenbrain<<std::endl;

        #if 0
        //if(!(subframereg=(double*)malloc(sizeof*subframereg*dp->tdim_max*num_regions*(num_region_files?tc_files->nfiles:1)))){
        //START210419
        if(!(subframereg=(double*)malloc(sizeof*subframereg*dp->tdim_max*num_regions*((num_region_files||lutf)?tc_files->nfiles:1)))){

            printf("fidlError: Unable to malloc subframereg\n");
            exit(-1);
            }
        if(!num_region_files){
            if(!(nsubframereg=(int*)malloc(sizeof*nsubframereg*dp->tdim_max*num_regions))){
                printf("fidlError: Unable to malloc nsubframereg\n");
                exit(-1);
                }
            for(i=0;i<dp->tdim_max*num_regions;i++){subframereg[i]=0.;nsubframereg[i]=0;}
            }
        #endif
        //START210420
        subframereg=new double[dp->tdim_max*num_regions*((num_region_files||lutf)?tc_files->nfiles:1)](); //empty parentheses zero intializes
        if(!num_region_files&&!lutf)nsubframereg=new int[dp->tdim_max*num_regions](); //empty parentheses zero intializes

        }

    if(((avgacrossfiles||sumacrossfiles)&&!num_region_files)||(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES)){
        if(!(temp_float2=(float*)malloc(sizeof*temp_float2*ap->vol*dp->tdim_max)))
            {printf("fidlError: Unable to malloc temp_float2\n");exit(-1);}
        }
    if(!(temp_float=(float*)malloc(sizeof*temp_float*ap->vol))) {
        printf("fidlError: Unable to malloc temp_float\n");
        exit(-1);
        }
    if(!(temp_double=(double*)malloc(sizeof*temp_double*ap->vol))) {
        printf("fidlError: Unable to malloc temp_double\n");
        exit(-1);
        }
    if(!(stat=(double*)malloc(sizeof*stat*ap->vol))) {
        printf("fidlError: Unable to malloc stat\n");
        exit(-1);
        }
    if(fwhm>0.||num_region_files){
        if(!(td1=(double*)malloc(sizeof*td1*(fwhm>0.?ap->vol:num_regions)))){
            printf("fidlError: Unable to malloc td1\n");
            exit(-1);
            }
        }
    if(normstr[0]) {
        if(!(mean=(double*)malloc(sizeof*mean*dp->volall))) {
            printf("fidlError: Unable to malloc mean\n");
            exit(-1);
            }
        }
    if(fwhm>0.){if(!(gs=gauss_smoth2_init(ap->xdim,ap->ydim,ap->zdim,fwhm,fwhm)))exit(-1);}
    }

//if(!(fbf=find_regions_by_file(tc_files->nfiles,fs->nframes,dp->tdim,fs->frames))) exit(-1);
//START211017
if(!(fbf=find_regions_by_file(tc_files->nfiles,fs->nframes,dp->tdim,fs->frames,NULL))) exit(-1);

//std::cout<<"here0 lookuptable_name(lutf,0)="<<lookuptable_name(lutf,0)<<std::endl;

if(!lc_names_only){

    //chunkvol=(int)ceil((double)ms_reg->lenbrain/(double)chunks);
    //START210512
    chunkvol=!roinames?(int)ceil((double)ms_reg->lenbrain/(double)chunks):1;

    n = chunks > 1 ? chunkvol : num_regions;
    p = chunks > 1 ? chunkvol : 1;

    //START170720
    //if(dp->filetypeall==(int)NIFTI&&lccompress){
    //START210129
    //if(dp->filetypeall==(int)NIFTI&&(lccompress||num_region_files)){
    //START210302
    if(dp->filetypeall==(int)NIFTI){

        //if(!(ifh=init_ifh(4,n,1,1,1,ms->voxel_size_1,ms->voxel_size_2,ms->voxel_size_3,0,ms->center,ms->mmppix)))exit(-1);
        //START190509
        //float msvoxel_size_1,msvoxel_size_2,msvoxel_size_3,*mscenter=NULL,*msmmppix=NULL;
        //ms.assign(msvoxel_size_1,msvoxel_size_2,msvoxel_size_3,mscenter,msmmppix);
        //if(!(ifh=init_ifh(4,n,1,1,1,msvoxel_size_1,msvoxel_size_2,msvoxel_size_3,0,mscenter,msmmppix)))exit(-1);
        //ifh->fwhm=fwhm;
        //START210129
        //if(!(ifh0=init_ifh(4,n,1,1,1,ms.voxel_size[0],ms.voxel_size[1],ms.voxel_size[2],0,ms.centerf,ms.mmppixf)))exit(-1);
        //START210131
        if(!(ifh0=init_ifh(4,n,1,1,1,ms_reg->voxel_size[0],ms_reg->voxel_size[1],ms_reg->voxel_size[2],0,ms_reg->centerf,ms_reg->mmppixf)))exit(-1);
        ifh0->fwhm=fwhm;
        }

    }

for(jj=r=l=0;l<fs->nframes;) {
    for(dummy=sum_tdim=i1=0;i1<tc_files->nfiles;i1++) {
        if(fbf->num_regions_by_file[i1]) {
            dummy += fbf->num_regions_by_file[i1];
            sum_tdim += dp->tdim[i1];
            ptr p0; 
            if(!lc_names_only) {
                if(!p0.ptr0(tc_files->files[i1]))exit(-1); 
                if((ifh=p0.getifh())){
                    ifh->dim4 = 1;
                    ifh->fwhm=fwhm;
                    if(num_region_files||(mask_file&&lccompress)||chunks>1){
                        ifh->dim1 = n;
                        ifh->dim2 = 1;
                        ifh->dim3 = 1;
                        }
                    }                
                else if(ifh0) ifh=ifh0;


                #if 0
                if(lcroiIND&&region_files){
                    rbf->nreg=reg[i1]->nregions;
                    rbf->nvoxels_region=reg[i1]->nvoxels_region;
                    rbf->indices=&reg[i1]->voxel_indices[0][0];
                    ms->lenbrain=reg[i1]->nvoxels;
                    ms->brnidx=&reg[i1]->voxel_indices[0][0];
                    }
                #endif
                //START210512
                if(lcroiIND&&region_files){
                    if(!nroinames){
                        rbf->nreg=reg[i1]->nregions;
                        rbf->nvoxels_region=reg[i1]->nvoxels_region;
                        rbf->indices=&reg[i1]->voxel_indices[0][0];
                        ms->lenbrain=reg[i1]->nvoxels;
                        ms->brnidx=&reg[i1]->voxel_indices[0][0];
                        }
                    else{
                        if(rbf)free_regions_by_file(rbf);

                        //if(!(rbf=find_regions_by_file_cover(1,roinames->nfiles,&reg[i1],roi)))exit(-1);
                        //START211017
                        if(!(rbf=find_regions_by_file_cover(1,roinames->nfiles,&reg[i1],roi,NULL)))exit(-1);

                        ms->lenbrain=rbf->nvoxels;
                        ms->brnidx=rbf->indices;
                        }
                    }
                if(normstr[0]){
                    for(i=0;i<dp->vol[i1];++i)mean[i]=0.;
                    for(i=0;i<dp->tdim[i1];++i){
                        if(!p0.ptrstack(temp_float,dp->tdim[i1]))exit(-1);
                        for(j=0;j<dp->vol[i1];++j)mean[j]+=(double)temp_float[j];
                        }
                    for(i=0;i<dp->vol[i1];++i)mean[i]=mean[i]<.000000000001?1:mean[i]/(double)dp->tdim[i1];
                    }


                //START190730
                //if(masknorm){ 
                //    for(i=0;i<dp->tdim[i1];i++)meanvox[i]=0.;
                //    if(!dp->swapbytes[i1]) {
                //        for(k=i=0;i<dp->tdim[i1];i++,k+=dp->vol[i1]){
                //            for(j=0;j<mslenbrain;j++)meanvox[i]+=(double)mm->ptr[k+msbrnidx[j]];
                //            }
                //        }
                //    else {
                //        for(k=i=0;i<dp->tdim[i1];i++,k+=dp->vol[i1]) {
                //            for(j=0;j<mslenbrain;j++)temp_float[j]=mm->ptr[k+msbrnidx[j]];
                //            swap_bytes((unsigned char *)temp_float,sizeof(float),(size_t)mslenbrain);
                //            for(j=0;j<mslenbrain;j++)meanvox[i]+=(double)temp_float[j];
                //            }
                //        }
                //    for(i=0;i<dp->tdim[i1];i++)meanvox[i]/=(double)mslenbrain;
                //    }

                }
            if(!avgacrossfiles&&!sumacrossfiles){
                strcpy(string,tc_files->files[i1]);
                if(!(strptr=get_tail_sans_ext(string)))exit(-1);
                }
            for(q=0,i=0;i<fbf->num_regions_by_file[i1]&&fbf->file_index[l]==(int)i1;i++,l++) {
                if(!avgacrossfiles&&!sumacrossfiles){
                    if(!q){sprintf(write_back,"%s_%d",strptr,fbf->roi_index[l]+1);}
                    else{sprintf(string2,"+%d",fbf->roi_by_file[i1][i]+1);strcat(write_back,string2);}
                    }
                if(!lc_names_only) {

                    //printf("fbf->num_regions_by_file[%zd]=%d i1=%zd q=%d\n",i1,fbf->num_regions_by_file[i1],i1,q);
                    //printf("fbf->roi_index[%d]=%d fbf->roi_by_file[%zd][%d]=%d\n",l,fbf->roi_index[l],i1,i,fbf->roi_by_file[i1][i]);

                    if(!q)for(j=0;j<dp->vol[i1];j++)temp_double[j]=0;
                    if(!p0.ptrstack(temp_float,fbf->roi_index[l]))exit(-1); 

                    #if 0
                    for(j=0;j<dp->vol[i1];j++) {
                        if(temp_float[j]==(float)UNSAMPLED_VOXEL) {
                            temp_double[j] = (double)UNSAMPLED_VOXEL;
                            }
                        else if(temp_double[j]==(double)UNSAMPLED_VOXEL) {
                            /*do nothing*/
                            }
                        else if(num_tc_weights) {
                            temp_double[j] += tc_weights->tc[tc_weights->eachi[r]+q]*(double)temp_float[j];
                            }
                        else {
                            temp_double[j] += (double)temp_float[j];
                            }
                        }
                    #endif
                    //START210131
                    for(j=0;j<ms->lenbrain;j++) {
                        if(temp_float[ms->brnidx[j]]==(float)UNSAMPLED_VOXEL) {
                            temp_double[ms->brnidx[j]] = (double)UNSAMPLED_VOXEL;
                            }
                        else if(temp_double[ms->brnidx[j]]==(double)UNSAMPLED_VOXEL) {
                            /*do nothing*/
                            }
                        else if(num_tc_weights) {
                            temp_double[ms->brnidx[j]] += tc_weights->tc[tc_weights->eachi[r]+q]*(double)temp_float[ms->brnidx[j]];
                            }
                        else {
                            temp_double[ms->brnidx[j]] += (double)temp_float[ms->brnidx[j]];
                            }
                        }
                    if(q == (fs->num_frames_to_sum[r]-1)) {
                        if(fs->num_frames_to_sum[r]>1){

                            //for(j=0;j<dp->vol[i1];j++)if(temp_double[j]!=(double)UNSAMPLED_VOXEL)temp_double[j]/=(double)fs->num_frames_to_sum[r];
                            //START210131
                            for(j=0;j<ms->lenbrain;j++)if(temp_double[ms->brnidx[j]]!=(double)UNSAMPLED_VOXEL)temp_double[ms->brnidx[j]]/=(double)fs->num_frames_to_sum[r];

                            }
                        if(normstr[0]){

                            //for(j=0;j<dp->vol[i1];j++)if(temp_double[j]!=(double)UNSAMPLED_VOXEL)temp_double[j]/=mean[j];
                            //START210131
                            for(j=0;j<ms->lenbrain;j++)if(temp_double[ms->brnidx[j]]!=(double)UNSAMPLED_VOXEL)temp_double[ms->brnidx[j]]/=mean[ms->brnidx[j]];

                            }
                        dptr=temp_double;
                        if(xform_file){
                            if(!t4_atlas(temp_double,stat,&t4[t4_identify[i1]*(int)T4SIZE],dp->xdim[i1],dp->ydim[i1],dp->zdim[i1],
                                dp->dx[i1],dp->dz[i1],A_or_B_or_U[t4_identify[i1]],dp->orientation[i1],ap,(double*)NULL))exit(-1);
                            dptr=stat;
                            }
                        if(fwhm>0.){
                            gauss_smoth2(dptr,td1,gs);
                            dptr=td1;
                            }

                        //if(num_region_files){
                        //START210416
                        if(region_files){
                            //std::cout<<"rbf->nvoxels_region=";for(j=0;j<rbf->nreg;++j)std::cout<<" "<<rbf->nvoxels_region[j];std::cout<<std::endl;

                            //crs(dptr,td1,rbf,(char*)NULL);
                            //START211103
                            if(!lcrms){
                                crs(dptr,td1,rbf,(char*)NULL);
                                }
                            else{
                                std::cerr<<"Computing rms values"<<std::endl;
                                crs_rms(dptr,td1,rbf,(char*)NULL);
                                }

                            if(!avgacrossfiles&&!sumacrossfiles){ 
                                for(j=0;j<num_regions;j++)temp_float[j]=(float)td1[j];

                                //std::cout<<"num_regions="<<num_regions<<std::endl;
                                //std::cout<<"temp_float=";for(j=0;j<num_regions;++j)std::cout<<" "<<temp_float[j];std::cout<<std::endl;


                                }
                            else{
                                for(j=0;j<num_regions;j++)subframereg[jj++]=td1[j];
                                }
                            }
                        else{
                            if(!avgacrossfiles&&!sumacrossfiles){
                                for(j=0;j<ms_reg->lenbrain;j++)temp_float[j]=dptr[ms_reg->brnidx[j]]==(double)UNSAMPLED_VOXEL?
                                    (float)UNSAMPLED_VOXEL:(float)dptr[ms_reg->brnidx[j]];
                                }
                            else{

                                #if 0
                                for(k=ms_reg->lenbrain*fbf->roi_index[l],j=0;j<ms_reg->lenbrain;j++,k++){
                                    if(fabs(dptr[ms_reg->brnidx[j]])>(double)UNSAMPLED_VOXEL){
                                        subframereg[k]+=dptr[ms_reg->brnidx[j]];
                                        nsubframereg[k]++;
                                        }
                                    }
                                #endif
                                //START210420
                                if(!lutf){ 
                                    for(k=ms_reg->lenbrain*fbf->roi_index[l],j=0;j<ms_reg->lenbrain;j++,k++){
                                        if(fabs(dptr[ms_reg->brnidx[j]])>(double)UNSAMPLED_VOXEL){
                                            subframereg[k]+=dptr[ms_reg->brnidx[j]];
                                            nsubframereg[k]++;
                                            }
                                        }
                                    }
                                else{
                                    //for(k=ms_reg->lenbrain*fbf->roi_index[l],j=0;j<ms_reg->lenbrain;j++,k++){
                                    //START210420
                                    for(k=ms_reg->lenbrain*fbf->roi_index[l],j=0;j<ms_reg->lenbrain;++j,++k,++jj){

                                        //std::cout<<"dptr["<<ms_reg->brnidx[j]<<"]="<<dptr[ms_reg->brnidx[j]]<<std::endl;

                                        if(fabs(dptr[ms_reg->brnidx[j]])>(double)UNSAMPLED_VOXEL){
                                            //subframereg[jj++]=dptr[ms_reg->brnidx[j]];
                                            //std::cout<<"    k="<<k<<std::endl;
                                            //subframereg[k]=dptr[ms_reg->brnidx[j]];
                                            subframereg[jj]=dptr[ms_reg->brnidx[j]];
                                            if(superbird)superbird[j]++; 
                                            }
                                        }
                                    }



                                }
                            }
                        }
                    }
                if(q++ == (fs->num_frames_to_sum[r]-1)) {
                    q = 0;
                    r++;
                    if(!avgacrossfiles&&!sumacrossfiles){ 
                        for(j=0;j<chunks;j++) {
                            strcpy(filename,write_back);
                            if(chunks > 1) {

                                //sprintf(string2,"_CHUNK%d",j+1);
                                //strcat(filename,string2);
                                //START210512
                                if(!roinames){ 
                                    sprintf(string2,"_CHUNK%d",j+1);
                                    strcat(filename,string2);
                                    }                                    
                                else{
                                    sprintf(string2,"_%s",roinames->files[j]);
                                    strcat(filename,string2);
                                    }                                    

                                }
                            if(num_tc_weights) {
                                sprintf(string2,"w%d",r);
                                strcat(filename,string2);
                                }
                            strcat(filename,normstr);
                            strcat(filename,fwhmstr);
                            if(scratchdir) sprintf(filename,"%s_%d",filename,l);
                            if(label) sprintf(filename,"%s_%s",filename,label);
                            strcat(filename,".4dfp.img");
                            if(!lc_names_only){
                                min_and_max_floatstack(&temp_float[j*p],n,&ifh->global_min,&ifh->global_max);
                                }
                            for(k=0;k<nscratchdir;k++){
                                sprintf(filename0,"%s%s",!scratchdir?"":scratchdir->files[k],filename);
                                if(!lc_names_only){
                                    //for(int i=0;i<n;++i)std::cout<<"temp_float["<<j*p+i<<"]="<<temp_float[j*p+i]<<std::endl;
                                    if(!writestack(filename0,&temp_float[j*p],sizeof(float),(size_t)n,0))exit(-1);
                                    if(!write_ifh(filename0,ifh,0)) exit(-1);
                                    }
                                printf("Time courses written to %s\n",filename0);

                                //if(concf)concofs<<pwd<<"/"<<filename0<<std::endl;
                                //START210512
                                if(concf){
                                    if(!nroinames){
                                        concofs<<pwd<<"/"<<filename0<<std::endl;
                                        }
                                    else{
                                        *concfsofs[j]<<pwd<<"/"<<filename0<<std::endl;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            if(!lc_names_only) {

                #if 0
                if(dp->filetypeall==(int)IMG){free_ifh(ifh,0);if(!unmap_disk(mm))exit(-1);}
                else if(dp->filetypeall==(int)NIFTI){nS.~niftiStack();}
                #endif
                //START190927
                //if(!p0.ptrfree())exit(-1); 
                if(!p0.ptrfree()){
                    exit(-1); 
                    }
                }
            }
        }
    }

if(avgacrossfiles||sumacrossfiles){
    if(num_region_files||lutf){

        //std::cout<<"here2 lookuptable_name(lutf,0)="<<lookuptable_name(lutf,0)<<std::endl;

        //if(lutf&&!dontaddmissingFSreg){
        //START210602
        if(superbird&&lutf&&!dontaddmissingFSreg){

            //if(!superbird[72])superbird[72]=-1; //5th-Ventricle
            //if(!superbird[80])superbird[80]=-1; //non-WM-hypointensities
            //START211217
            if(num_regions>72)if(!superbird[72])superbird[72]=-1; //5th-Ventricle
            if(num_regions>80)if(!superbird[80])superbird[80]=-1; //non-WM-hypointensities



            //std::cout<<"superbird=";for(i=0;i<num_regions;++i)if(superbird[i])std::cout<<i<<":"<<superbird[i]<<" ";std::cout<<std::endl;
            }

        //for(i1=0;i1<tc_files->nfiles;i1++){
        //    for(i=0;i<fbf->num_regions_by_file[i1];i++){
        //        std::cout<<"here51 fbf->roi_by_file["<<i1<<"]["<<i<<"]="<<fbf->roi_by_file[i1][i]<<std::endl;
        //        }
        //    }

        if(!(sum_framereg=(double*)malloc(sizeof*sum_framereg*dp->tdim_max*num_regions))) {
            printf("fidlError: Unable to malloc sum_framereg\n");
            exit(-1);
            }
        for(i=0;i<dp->tdim_max*num_regions;i++) sum_framereg[i]=0.;
        if(!(sum2_framereg=(double*)malloc(sizeof*sum2_framereg*dp->tdim_max*num_regions))) {
            printf("fidlError: Unable to malloc sum2_framereg\n");
            exit(-1);
            }
        for(i=0;i<dp->tdim_max*num_regions;i++) sum2_framereg[i]=0.;
        if(!(nframe=(int*)malloc(sizeof*nframe*dp->tdim_max))) {
            printf("fidlError: Unable to malloc nframe\n");
            exit(-1);
            }
        for(i=0;i<dp->tdim_max;i++) nframe[i]=0;
        int* nframesub; 
        if(!(nframesub=(int*)malloc(sizeof*nframesub*dp->tdim_max*num_regions))){
            std::cout<<"fidlError: Unable to malloc nframesub"<<std::endl;
            exit(-1);
            }
        for(i=0;i<dp->tdim_max*num_regions;i++)nframesub[i]=0;

        //std::cout<<"here3 lookuptable_name(lutf,0)="<<lookuptable_name(lutf,0)<<std::endl;
        //std::cout<<"here3 num_regions="<<num_regions<<std::endl;
        //std::cout<<"here3 dp->tdim_max*num_regions="<<dp->tdim_max*num_regions<<std::endl;
        //for(i1=0;i1<tc_files->nfiles;i1++){
        //    for(i=0;i<fbf->num_regions_by_file[i1];i++){
        //        std::cout<<"here52 fbf->roi_by_file["<<i1<<"]["<<i<<"]="<<fbf->roi_by_file[i1][i]<<std::endl;
        //        }
        //    }



        #if 0
        for(jj=i1=0;i1<tc_files->nfiles;i1++) {
            for(i=0;i<fbf->num_regions_by_file[i1];i++) {
                nframe[fbf->roi_by_file[i1][i]]++;
                for(k=fbf->roi_by_file[i1][i]*num_regions,j=0;j<num_regions;j++,jj++,k++) {
                    sum_framereg[k]+=subframereg[jj];
                    sum2_framereg[k]+=subframereg[jj]*subframereg[jj];
                    }
                }
            }
        for(ii=i=0;i<dp->tdim_max;i++,ii+=num_regions) {
            if(nframe[i]) {
                for(jj=ii,j=0;j<num_regions;j++,jj++) {
                    sum2_framereg[jj] = sqrt((sum2_framereg[jj]-sum_framereg[jj]*sum_framereg[jj]/(double)nframe[i])/
                        (double)(nframe[i]-1)/(double)nframe[i]);
                    sum_framereg[jj] /= (double)nframe[i];
                    }
                }
            }
        #endif
        //START210202
        for(jj=i1=0;i1<tc_files->nfiles;i1++){
            for(i=0;i<fbf->num_regions_by_file[i1];i++){
                nframe[fbf->roi_by_file[i1][i]]++;
                for(k=fbf->roi_by_file[i1][i]*num_regions,j=0;j<num_regions;j++,jj++,k++) {
                    if(fabs(subframereg[jj])>(double)UNSAMPLED_VOXEL){
                        nframesub[k]++;
                        sum_framereg[k]+=subframereg[jj];
                        sum2_framereg[k]+=subframereg[jj]*subframereg[jj];
                        }
                    }
                }
            }
        for(ii=i=0;i<dp->tdim_max;i++,ii+=num_regions) {
            if(nframe[i]){
                for(jj=ii,j=0;j<num_regions;j++,jj++) {
                    sum2_framereg[jj] = sqrt((sum2_framereg[jj]-sum_framereg[jj]*sum_framereg[jj]/(double)nframesub[jj])/
                        (double)(nframesub[jj]-1)/(double)nframesub[jj]);
                    sum_framereg[jj] /= (double)nframesub[jj];
                    }
                }
            }

        if(!get_tail_sans_ext(avgacrossfiles))exit(-1);
        sprintf(filename,"%s_meansem.txt",avgacrossfiles);
        sprintf(string2,"%s_mean95CI.txt",avgacrossfiles);
        sprintf(string3,"%s_mean.txt",avgacrossfiles);
        if(!(fp=fopen_sub(filename,"w")))exit(-1);
        if(!(fp1=fopen_sub(string2,"w")))exit(-1);
        if(!(fp2=fopen_sub(string3,"w")))exit(-1);


        //START210511
        if(regnames){ 
            for(j=0;j<num_regions;j++){
                fprintf(fp,"\t%s",regnames->files[j]);
                fprintf(fp1,"\t%s",regnames->files[j]);
                fprintf(fp2,"\t%s",regnames->files[j]);
               }
           }
        //if(!lcroiIND){
        //START210511
        else if(!lcroiIND){

            #if 0
            for(j=0;j<num_regions;j++){
                fprintf(fp,"\t%s %d",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                fprintf(fp1,"\t%s %d",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                fprintf(fp2,"\t%s %d",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
               }
            #endif
            //START211116
            if(rbf){ 
                for(j=0;j<num_regions;j++){
                    fprintf(fp,"\t%s %d",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                    fprintf(fp1,"\t%s %d",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                    fprintf(fp2,"\t%s %d",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                   }
                }
            else if(lutf){
                for(j=0;j<num_regions;j++){
                    if(superbird)if(!superbird[j])continue; 
                    fprintf(fp,"\t%s",lookuptable_name(lutf,j));
                    fprintf(fp1,"\t%s",lookuptable_name(lutf,j));
                    fprintf(fp2,"\t%s",lookuptable_name(lutf,j));
                   }
                }


            }
        else{
            for(j=0;j<num_regions;j++){
                if(!superbird[j])continue;

                #if 0
                fprintf(fp,"\t%s",lookuptable_name(lutf,j));
                fprintf(fp1,"\t%s",lookuptable_name(lutf,j));
                fprintf(fp2,"\t%s",lookuptable_name(lutf,j));
                #endif
                #if 0
                //START211017
                fprintf(fp,"\t%s",lookuptable_name(lutf,reg[0]->regval[j]));
                fprintf(fp1,"\t%s",lookuptable_name(lutf,reg[0]->regval[j]));
                fprintf(fp2,"\t%s",lookuptable_name(lutf,reg[0]->regval[j]));
                #endif
                //START211020
                if(!reg[0]->regval){
                    fprintf(fp,"\t%s",lookuptable_name(lutf,j));
                    fprintf(fp1,"\t%s",lookuptable_name(lutf,j));
                    fprintf(fp2,"\t%s",lookuptable_name(lutf,j));
                    }
                else{
                    fprintf(fp,"\t%s",lookuptable_name(lutf,reg[0]->regval[j]));
                    fprintf(fp1,"\t%s",lookuptable_name(lutf,reg[0]->regval[j]));
                    fprintf(fp2,"\t%s",lookuptable_name(lutf,reg[0]->regval[j]));
                    }

                }
            }
        fprintf(fp,"\n");fprintf(fp1,"\n");fprintf(fp2,"\n");
        for(ii=i=0;i<dp->tdim_max;i++,ii+=num_regions) {
            if(TR>0.&&starttime!=(double)UNSAMPLED_VOXEL) {
                fprintf(fp,"%f",starttime+i*TR);
                if(label) fprintf(fp,"_%s",label);
                }
            else if(label) fprintf(fp,"%s",label);

            if(TR>0.&&starttime!=(double)UNSAMPLED_VOXEL) {
                fprintf(fp1,"%f",starttime+i*TR);
                if(label) fprintf(fp1,"_%s",label);
                }
            else if(label) fprintf(fp1,"%s",label);

            if(TR>0.&&starttime!=(double)UNSAMPLED_VOXEL) {
                fprintf(fp2,"%f",starttime+i*TR);
                if(label) fprintf(fp2,"_%s",label);
                }
            else if(label) fprintf(fp2,"%s",label);
            if(nframe[i]>1){
                td=gsl_cdf_tdist_Qinv(.025,(double)(nframe[i]-1)); 
                printf("t(.05/2,df=%d)=%f\n",nframe[i]-1,td);     
                }

            #if 0 
            for(jj=ii,j=0;j<num_regions;j++,jj++){
                if(superbird)if(!superbird[j])continue;
                fprintf(fp,"\t%f %f",sum_framereg[jj],sum2_framereg[jj]);
                fprintf(fp1,"\t%f %f",sum_framereg[jj],nframe[i]>1?sum2_framereg[jj]*td:sum2_framereg[jj]);
                fprintf(fp2,"\t%f",sum_framereg[jj]);
                }
            #endif
            #if 0
            //START210202
            for(jj=ii,j=0;j<num_regions;j++,jj++){
                if(superbird)if(!superbird[j])continue;
                td=gsl_cdf_tdist_Qinv(.025,(double)(nframesub[jj]-1)); 
                fprintf(fp,"\t%f %f",sum_framereg[jj],sum2_framereg[jj]);
                fprintf(fp1,"\t%f %f",sum_framereg[jj],nframesub[jj]>1?sum2_framereg[jj]*td:sum2_framereg[jj]);
                fprintf(fp2,"\t%f",sum_framereg[jj]);
                }
            #endif
            //START210305
            for(jj=ii,j=0;j<num_regions;j++,jj++){
                if(superbird)if(!superbird[j])continue;
	        //std::cout<<"here1 nframesub["<<jj<<"]="<<nframesub[jj]<<" superbird["<<j<<"]="<<superbird[j]<<std::endl;
                td=nframesub[jj]>1?gsl_cdf_tdist_Qinv(.025,(double)(nframesub[jj]-1)):0.;
                fprintf(fp,"\t%f %f",sum_framereg[jj],sum2_framereg[jj]);
                fprintf(fp1,"\t%f %f",sum_framereg[jj],nframesub[jj]>1?sum2_framereg[jj]*td:sum2_framereg[jj]);
                fprintf(fp2,"\t%f",sum_framereg[jj]);
                }

            fprintf(fp,"\n");
            fprintf(fp1,"\n");
            fprintf(fp2,"\n");
            }
        fflush(fp);fclose(fp);printf("Output written to %s\n",filename);
        fflush(fp1);fclose(fp1);printf("Output written to %s\n",string2);
        fflush(fp2);fclose(fp2);printf("Output written to %s\n",string3);
        if(scratchdir){
            sprintf(string,"rm -rf %s",scratchdir->files[0]);
            if(system(string) == -1) printf("fidlError: unable to %s\n",string);
            }
        sprintf(filename,"%s_ind.txt",avgacrossfiles);
        if(!(fp=fopen_sub(filename,"w")))exit(-1);
        for(i=0;i<dp->tdim_max;i++){
            if(dp->tdim_max>1)fprintf(fp,"FRAME %d\n",i+1);
            if(regnames){
                for(j=0;j<num_regions;j++)fprintf(fp,"\t%s",regnames->files[j]);
                }
            else if(!lcroiIND){

                //for(j=0;j<num_regions;j++)fprintf(fp,"\t%s %d",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                //START211116
                if(rbf){
                    for(j=0;j<num_regions;j++)fprintf(fp,"\t%s %d",rbf->region_names_ptr[j],rbf->nvoxels_region[j]);
                    }
                else if(lutf){

                    //for(j=0;j<num_regions;j++)fprintf(fp,"\t%s",lookuptable_name(lutf,j));
                    //START211207
                    for(j=0;j<num_regions;j++){
                        if(superbird)if(!superbird[j])continue;
                        fprintf(fp,"\t%s",lookuptable_name(lutf,j));
                        }

                    }

                }
            else{
                
                //for(j=0;j<num_regions;j++)if(superbird[j])fprintf(fp,"\t%s",lookuptable_name(lutf,j));
                //START211017
                //for(j=0;j<num_regions;j++)if(superbird[j])fprintf(fp,"\t%s",lookuptable_name(lutf,reg[0]->regval[j]));
                //START211020
                if(!reg[0]->regval){ 
                    for(j=0;j<num_regions;j++)if(superbird[j])fprintf(fp,"\t%s",lookuptable_name(lutf,j));
                    }
                else{
                    for(j=0;j<num_regions;j++)if(superbird[j])fprintf(fp,"\t%s",lookuptable_name(lutf,reg[0]->regval[j]));
                    }
                }

            fprintf(fp,"\n");
            for(i1=0;i1<tc_files->nfiles;i1++){
                if(i<fbf->num_regions_by_file[i1]){

                    #if 0
                    strcpy(string,tc_files->files[i1]);
                    if(!(strptr=get_tail_sans_ext(string)))exit(-1);
                    fprintf(fp,"%s",strptr);
                    #endif
                    //START210201
                    if(!label){ 
                        strcpy(string,tc_files->files[i1]);
                        if(!(strptr=get_tail_sans_ext(string)))exit(-1);
                        fprintf(fp,"%s",strptr);
                        }
                    else{
                        fprintf(fp,"%s",label);
                        }


                    for(jj=i*num_regions,j1=0;j1<i1;j1++)jj+=fbf->num_regions_by_file[j1]*num_regions;

                    //for(j=0;j<num_regions;j++)fprintf(fp,"\t%f",subframereg[jj++]);
                    //START210201
                    for(j=0;j<num_regions;j++,jj++){
                        if(superbird)if(!superbird[j])continue;
                        fprintf(fp,"\t%f",subframereg[jj]);
                        }

                    fprintf(fp,"\n");
                    }
                }
            }
        fflush(fp);fclose(fp);printf("Individual subject values written to %s\n",filename);fflush(stdout);

        #if 0
        //START210201
        if(lcroiIND){
            sprintf(filename,"%s_regnames.txt",avgacrossfiles);
            if(!(fp=fopen_sub(filename,"w")))exit(-1);
            for(j=0;j<num_regions;j++)if(superbird[j])fprintf(fp,"%s\n",lookuptable_name(lutf,j));
            fflush(fp);fclose(fp);
            printf("Region names written to %s\n",filename);fflush(stdout);
            }
        #endif
        //START211207
        if(lcroiIND||lutf){
            sprintf(filename,"%s_regnames.txt",avgacrossfiles);
            if(!(fp=fopen_sub(filename,"w")))exit(-1);
            for(j=0;j<num_regions;j++){
                if(superbird)if(!superbird[j])continue;
                fprintf(fp,"%s\n",lookuptable_name(lutf,j));
                }
            fflush(fp);fclose(fp);
            printf("Region names written to %s\n",filename);fflush(stdout);
            }
 

        if(num_frames){
            sprintf(string,"rm -f %s",frames_file);
            if(system(string)==-1)printf("fidError: Unable to %s\n",string);
            }
        }
    else{
        if(!(w1=write1_init()))exit(-1);
        w1->filetype=dp->filetypeall;
        w1->swapbytes=0;
        w1->temp_float=temp_float2;
        if(dp->filetypeall==(int)IMG){
            w1->temp_double=subframereg;;

            //w1->lenbrain=ms.lenbrain*dp->tdim_max;
            //START210131
            w1->lenbrain=ms_reg->lenbrain*dp->tdim_max;

            w1->how_many=ap->vol*dp->tdim_max;
            if(!(ifh=read_ifh(tc_files->files[0],(Interfile_header*)NULL)))exit(-1);
            ifh->dim1=ap->xdim;ifh->dim2=ap->ydim;ifh->dim3=ap->zdim;ifh->dim4=dp->tdim_max;
            ifh->center[0]=(float)ap->center[0];ifh->center[1]=(float)ap->center[1];ifh->center[2]=(float)ap->center[2];
            ifh->mmppix[0]=(float)ap->mmppix[0];ifh->mmppix[1]=(float)ap->mmppix[1];ifh->mmppix[2]=(float)ap->mmppix[2];
            w1->ifh=ifh;
            }
        else if(dp->filetypeall==(int)CIFTI||dp->filetypeall==(int)CIFTI_DTSERIES){
            if(!(xmls=cifti_getxml(tc_files->files[0])))exit(-1);
            w1->xmlsize=xmls->size;;
            w1->cifti_xmldata=xmls->data;
            dims[0]=(int64_t)dp->xdim[0];dims[1]=(int64_t)dp->ydim[0];dims[2]=(int64_t)dp->zdim[0];
            w1->dims=dims;
            w1->tdim=(int64_t)dp->tdim_max;
            }
        else if(dp->filetypeall==(int)NIFTI){
            w1->file_hdr=tc_files->files[0];
            }
        if(sumacrossfiles){
            sprintf(filename0,"%s%s",sumacrossfiles,Fileext[dp->filetype[0]]);
            for(l=k=i=0;i<dp->tdim_max;i++,l+=ap->vol)for(j=0;j<ms_reg->lenbrain;j++,k++)temp_float2[l+ms_reg->brnidx[j]]=subframereg[k];
            for(l=k=i=0;i<dp->tdim_max;i++,l+=ap->vol)for(j=0;j<ms_reg->lenbrain;j++,k++){
                if(subframereg[k]>0.)printf("subframereg[%d]=%f nsubframereg[%d]=%d\n",k,subframereg[k],k,nsubframereg[k]);
                }
            if(!write1(filename0,w1))exit(-1);
            printf("Output written to %s\n",filename0);
            }
        if(avgacrossfiles){

            //for(l=k=i=0;i<dp->tdim_max;i++,l+=ap->vol)for(j=0;j<ms.lenbrain;j++,k++)temp_float2[l+ms.brnidx[j]]=subframereg[k]/nsubframereg[k];
            //START210131
            for(l=k=i=0;i<dp->tdim_max;i++,l+=ap->vol)for(j=0;j<ms_reg->lenbrain;j++,k++)temp_float2[l+ms_reg->brnidx[j]]=subframereg[k]/nsubframereg[k];

            if(!write1(avgacrossfiles,w1))exit(-1);
            printf("Output written to %s\n",avgacrossfiles);
            }


        if(num_frames){
            sprintf(string,"rm -f %s",frames_file);
            if(system(string)==-1)printf("fidError: Unable to %s\n",string);
            }
        }
    }
if(cleanup){
    sprintf(string,"rm -rf %s",cleanup);
    if(system(string)==-1)printf("fidlError: fidl_bolds  Unable to %s\n",string);
    }

//if(concf)std::cout<<"Conc written to "<<concf<<std::endl;
//START210513
if(concf){
    if(!nroinames){
        std::cout<<"Conc written to "<<concf<<std::endl;
        }
    else{
        for(size_t i=0;i<roinames->nfiles;++i){
            std::cout<<"Conc written to "<<concfs[i]<<std::endl;
            }
        }
    }

exit(0);
}
