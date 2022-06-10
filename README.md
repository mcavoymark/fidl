# fidl
Neuroimaging analysis software

Three directories are included: c, idl and FIDL.  
Most of the computing is done in C/C++ whose code resides in the "c" directory.  
The user interface is written in the IDL scripting language whose code resides in the "idl" directory.  
The FIDL directory includes   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"build": linux built libraries and binaries  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"fidl": CMakeLists.txt for building the libraries and binaries. You will need to edit it.  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"cifti/build": place holder for CiftiLib (https://github.com/Washington-University/CiftiLib) - prebuilt one included    
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"gsl": place holder for the gsl library (https://www.gnu.org/software/gsl) also called by fidl  
In addition fidl calls RSCRIPT with scripts written on the fly by the binaries:    
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fidl_anova6.cxx calls the R lmerTest which calls the lme4 library.  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fidl_pcat.c calls the RCUR library  

It's greatest strengths are its general linear models, average statistics, repeated measures ANOVAs and linear mixed effects.  
Now accepts Freesurfer style \*LUT.txt files to identify regions in niftis in addition to 4dfps with regions taking integer values from 2 upward.
