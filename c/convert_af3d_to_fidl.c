/* Copyright 12/31/99 Washington University.  All Rights Reserved.
   convert_af3d_to_fidl.c  $Revision: 1.30 $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fidl.h>

void make_base(char *base,float x,float y,float z);
void make_regname(char *regname,int index,char *base,int nvox);

main(int argc,char **argv)
{
char *filename,string[MAXNAME],string2[MAXNAME],*directory=NULL,*str_ptr,*out=NULL,regname[MAXNAME],*log=NULL,base[MAXNAME],
    *ifhfile=NULL,*regnamelabel=NULL,diameterstr[10];
int i,j,k,l,kk,kkk,incr,idx,atlas=222,x,y,z,length_directory,lcseparate=0,SunOS_Linux,nreg,*len,*nvox,*index,lccom=0,lcold=1;
float threshold=0,diameter=0,*temp_float,regval,*peakval;
double *coor,*peakcoor;
FILE *fp=NULL;
Af3d *af3d;
Interfile_header *ifh=NULL;
Atlas_Param *ap;

if(argc < 9) {
    fprintf(stderr,"Usage: convert_af3d_to_fidl -af3d_file cue_2v_zstat_ecat.3sp -threshold 3.00 -diameter 10 -atlas 222\n");
    fprintf(stderr,"        -af3d_file:      Name of af3d file.\n");
    fprintf(stderr,"        -threshold:      Atlas coordinates whose value are less than this threshold are ignored.\n"); 
    fprintf(stderr,"                         The default is 0.\n");
    fprintf(stderr,"        -diameter_in_mm: Create a spherical region with its center the atlas coordinate in the af3d file.\n");
    fprintf(stderr,"                         The default is 0 mm.\n");
    fprintf(stderr,"        -atlas:          Either 111, 222 or 333. Default is 222. If you use -ifh, this field is ignored.\n");
    fprintf(stderr,"        -ifh:            Desired space, center, and mmppix are pulled from this file in lieu of -atlas.\n");
    fprintf(stderr,"        -directory:      Specify directory for output.\n");
    fprintf(stderr,"        -separate        Separate file for each point.\n");
    fprintf(stderr,"        -out:            Name of output file. No effect with -separate.\n");
    fprintf(stderr,"        -log:            Name of log file.\n");
    fprintf(stderr,"        -center_of_mass  Name regions by their center of mass instead of the supplied coordinate.\n");
    fprintf(stderr,"        -new             Integerize at the very end.\n");
    fprintf(stderr,"        -regnamelabel:   Label appended to region names.\n");
    exit(-1);
    }
for(i=1;i<argc;i++) { 
    if(!strcmp(argv[i],"-af3d_file") && argc > i+1)
        filename = argv[++i];
    if(!strcmp(argv[i],"-threshold") && argc > i+1)
	threshold = atof(argv[++i]);
    if(!strcmp(argv[i],"-diameter_in_mm") && argc > i+1)
	diameter = atof(argv[++i]);
    if(!strcmp(argv[i],"-atlas") && argc > i+1)
	atlas = atoi(argv[++i]);
    if(!strcmp(argv[i],"-ifh") && argc > i+1)
        ifhfile = argv[++i];
    if(!strcmp(argv[i],"-directory") && argc > i+1) {
        length_directory = strlen(argv[i+1]);
        if(!(directory=malloc(sizeof*directory*(length_directory+1)))) {
            printf("Error: Unable to malloc directory\n");
            exit(-1);
            }
        strcpy(directory,argv[++i]);
        if(directory[length_directory-2] == '/') directory[length_directory-2] = 0;
        }
    if(!strcmp(argv[i],"-separate"))
        lcseparate = 1;
    if(!strcmp(argv[i],"-out") && argc > i+1)
        out = argv[++i];
    if(!strcmp(argv[i],"-log") && argc > i+1)
        log = argv[++i];
    if(!strcmp(argv[i],"-center_of_mass"))
        lccom = 1;
    if(!strcmp(argv[i],"-new"))
        lcold = 0;
    if(!strcmp(argv[i],"-regnamelabel") && argc > i+1)
        regnamelabel = argv[++i];
    }
if(diameter<0.001) {
    printf("Error: diameter_in_mm=%f  Must be greater than 0.\n",diameter);
    exit(-1);
    }
sprintf(diameterstr,"_%gmm",diameter);
if(lcseparate && out) out = 0;
if((SunOS_Linux=checkOS())==-1) exit(-1);
fprintf(stderr,"%s\n",lcold?"Old way.":"New way.");
if(ifhfile) if(!(ifh=read_ifh(ifhfile,(Interfile_header*)NULL))) exit(-1);
if(!(ap=get_atlas_param(atlas,ifh))) exit(-1);
if(!(af3d=read_af3d(filename,ap,threshold,"af3d"))) exit(-1);
if(!(temp_float=malloc(sizeof*temp_float*ap->vol))) {
    printf("Error: Unable to malloc temp_float\n");
    exit(-1);
    }
for(i=0;i<ap->vol;i++) temp_float[i] = 0.;
if(!(nvox=malloc(sizeof*nvox*af3d->nindex))) {
    printf("Error: Unable to malloc nvox\n");
    exit(-1);
    }
if(!(len=malloc(sizeof*len*af3d->nindex))) {
    printf("Error: Unable to malloc len\n");
    exit(-1);
    }
if(!(index=malloc(sizeof*index*af3d->nindex))) {
    printf("Error: Unable to malloc index\n");
    exit(-1);
    }
if(!ifh) {
    if(!(ifh=init_ifh(4,ap->xdim,ap->ydim,ap->zdim,1,ap->voxel_size[0],ap->voxel_size[1],ap->voxel_size[2],1))) exit(-1);
    for(i=0;i<3;i++) {
        ifh->center[i] = (float)ap->center[i];
        ifh->mmppix[i] = (float)ap->mmppix[i];
        }
    }
else {
    free(ifh->file_name);
    }
if(!(ifh->file_name=malloc(sizeof*ifh->file_name*(strlen(filename)+1)))) {
    printf("Error: Unable to malloc ifh->file_name\n");
    exit(-1);
    }
strcpy(ifh->file_name,filename);
if(!out) {

    #if 0
    if(!directory) {
        if(!(directory=getenv("PWD"))) {
            printf("Error: Unable to get environment variable PWD\n");
            exit(-1);
            }
        } 
    strcpy(string2,filename);
    if(!(str_ptr=get_tail_sans_ext(string2))) exit(-1);
    sprintf(string2,"%s/%s_D%gmm",directory,str_ptr,diameter);
    if(!lcseparate) sprintf(string,"%s_t%g_reg.4dfp.img",string2,threshold);
    #endif
    /*START111229*/
    strcpy(string2,filename);
    if(!(str_ptr=get_tail_sans_ext(string2))) exit(-1);
    if(directory) 
        sprintf(string2,"%s/%s_D%gmm",directory,str_ptr,diameter);
    else
        sprintf(string2,"%s_D%gmm",str_ptr,diameter);
    if(!lcseparate) sprintf(string,"%s_t%g_reg.4dfp.img",string2,threshold);

    }
else {
    strcpy(string,out);
    }
if(log) {
    if(!(fp=fopen_sub(log,"w"))) exit(-1);
    fprintf(fp,"%s\n",filename);
    }
if(lccom) {
    if(!(coor=malloc(sizeof*coor*af3d->nindex*3))) {
        printf("Error: Unable to malloc coor in fidl_convert_af3d_to_fidl\n");
        exit(-1);
        }
    if(!(peakcoor=malloc(sizeof*peakcoor*af3d->nindex*3))) {
        printf("Error: Unable to malloc peakcoor in fidl_convert_af3d_to_fidl\n");
        exit(-1);
        }
    if(!(peakval=malloc(sizeof*peakval*af3d->nindex))) {
        printf("Error: Unable to malloc peakval in fidl_convert_af3d_to_fidl\n");
        exit(-1);
        }
    }
for(nreg=i=0;i<af3d->nindex;i++) {
    /*printf("i=%d   %f %f %f\n",i,af3d->coordinates[i][0],af3d->coordinates[i][1],af3d->coordinates[i][2]); fflush(stdout);*/
    regval = lcseparate ? 2. : (float)(nreg+2);
    make_base(base,af3d->coordinates[i][0],af3d->coordinates[i][1],af3d->coordinates[i][2]);
    if(log) fprintf(fp,"%f %f %f\n",af3d->coordinates[i][0],af3d->coordinates[i][1],af3d->coordinates[i][2]);
    if(lcold) {
        nvox[i]=make_sphere(af3d->x[i],af3d->y[i],af3d->z[i],temp_float,regval,ap->xdim,ap->ydim,ap->zdim,diameter,
            ap->voxel_size[0],fp);
        }
    else {
        nvox[i]=make_sphere_new(af3d->xd[i],af3d->yd[i],af3d->zd[i],temp_float,regval,ap->xdim,ap->ydim,ap->zdim,diameter,
            ap->voxel_size[0],fp);
        }
    if(!nvox[i]) {
        printf("Information: %s has zero voxels.\n",base);
        if(log) fprintf(fp,"        %s has zero voxels.\n",base);
        }
    else {
        if(lccom) {
            idx=nreg*3;
            if(!center_of_mass_cover(ap->vol,temp_float,temp_float,1,&coor[idx],&peakcoor[idx],&peakval[nreg])) exit(-1);

            #if 0
            for(kkk=j=0;j<nreg;j++) {
                for(kk=idx,incr=k=0;k<3;k++,kk++,kkk++) if(coor[kk]==coor[kkk]) incr++;
                if(incr==3) break;
                }
            #endif
            #if 0
            for(kkk=j=0;j<nreg;j++) {
                for(kk=idx,incr=k=0;k<3;k++,kk++,kkk++) {
            #endif
            #if 0
            for(kkk=idx-1,j=0;j<nreg;j++) {
                for(kk=idx+2,incr=k=0;k<3;k++,kk--,kkk--) {
                    /*printf("    coor[%d]=%f coor[%d]=%f",kk,coor[kk],kkk,coor[kkk]);*/
                    if(coor[kk]==coor[kkk]) {
                        incr++;
                        /*printf(" yes");*/
                        }
                    /*printf(" incr=%d\n",incr);*/
                    }
                if(incr==3) break;
                }
            #endif
            for(kkk=idx-1,j=0;j<nreg;j++,kkk-=3) {
                for(kk=idx+2,l=kkk,incr=k=0;k<3;k++,kk--,l--) {
                    /*printf("    coor[%d]=%f coor[%d]=%f",kk,coor[kk],l,coor[l]);*/
                    if(coor[kk]==coor[l]) {
                        incr++;
                        /*printf(" yes");*/
                        }
                    else {
                        /*printf("\n");*/
                        break;
                        }
                    /*printf(" incr=%d\n",incr);*/
                    }
                if(incr==3) break;
                }


            if(incr==3) {
                /*if(log) fprintf(fp,"    Same region as %f %f %f\n",af3d->coordinates[j][0],af3d->coordinates[j][1],
                    af3d->coordinates[j][2]);*/
                if(log) fprintf(fp,"    Same region as %f %f %f\n",coor[kkk-2],coor[kkk-1],coor[kkk]);
                /*printf("    Same region as coor[%d]=%f coor[%d]=%f coor[%d]=%f\n",kkk-2,coor[kkk-2],kkk-1,coor[kkk-1],kkk,coor[kkk]);*/
                for(k=0;k<ap->vol;k++) temp_float[k] = 0.;
                continue;
                }
            sprintf(base,"%.1f_%.1f_%.1f",coor[idx],coor[idx+1],coor[idx+2]);
            }

        /*START110713*/
        strcat(base,diameterstr);
        if(regnamelabel) strcat(base,regnamelabel);

        make_regname(regname,lcseparate?0:nreg,base,nvox[i]);
        len[nreg] = strlen(regname)+1;
        }
    if(lcseparate && nvox[i]) {
        ifh->nregions = 1;
        if(!(ifh->region_names=d2charvar(ifh->nregions,&len[nreg]))) exit(-1);
        strcpy(ifh->region_names[0],regname);
        sprintf(string,"%s_%s_reg.4dfp.img",string2,base);
        ifh->global_min = 0.; ifh->global_max = 2.;
        if(!writestack(string,temp_float,sizeof(float),(size_t)ap->vol,SunOS_Linux)) exit(-1);
        if(!write_ifh(string,ifh,0)) exit(-1);
        printf("Region file written to %s\n",string);
        if(log) fprintf(fp,"    %s\n",string);
        free_ifhregnames(ifh);
        for(j=0;j<ap->vol;j++) temp_float[j] = 0.;
        }
    if(nvox[i]) index[nreg++]=i;
    }

if(!lcseparate) {
    ifh->nregions = nreg;
    if(!(ifh->region_names=d2charvar(ifh->nregions,len))) exit(-1);
    for(i=0;i<nreg;i++) {
        make_base(base,af3d->coordinates[index[i]][0],af3d->coordinates[index[i]][1],af3d->coordinates[index[i]][2]);
        strcat(base,diameterstr);
        if(regnamelabel) strcat(base,regnamelabel);
        make_regname(regname,i,base,nvox[index[i]]);
        strcpy(ifh->region_names[i],regname);
        }
    ifh->global_min = 0.; ifh->global_max = af3d->nindex+1;
    if(!writestack(string,temp_float,sizeof(float),(size_t)ap->vol,SunOS_Linux)) exit(-1);
    if(!write_ifh(string,ifh,0)) exit(-1);
    printf("Region file written to %s\n",string);
    if(log) fprintf(fp,"    Region file written to %s\n",string);
    }


if(log) {
    fclose(fp);
    printf("Information: Log file written to %s\n",log);
    }
}


#if 0
void make_base(char *base,float x,float y,float z) 
{
    sprintf(base,"%d_%d_%d",(int)rint(x),(int)rint(y),(int)rint(z));
}
#endif
/*START110713*/
void make_base(char *base,float x,float y,float z) 
{
    /*sprintf(base,"%.1f_%.1f_%.1f",x,y,z);*/
    /*sprintf(base,"%.1g_%.1g_%.1g",x,y,z);*/
    sprintf(base,"%g_%g_%g",x,y,z);
}


void make_regname(char *regname,int index,char *base,int nvox)
{
    sprintf(regname,"%d %s %d",index,base,nvox);
}
