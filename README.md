# fidl
Neuroimaging analysis software

Three directories are included: c, idl and FIDL.
Most of the computing is done in C/C++ whose code resides in the "c" directory.
The user interface is written in the IDL scripting language whose code resides in the "idl" directory.
The FIDL directory includes 
    "build": linux built libraries and binaries
    "fidl": CMakeLists.txt for building the libraries and binaries. You will need to edit it.
    "cifti/build": linux built CiftiLib (https://github.com/Washington-University/CiftiLib) which fidl calls
    "gsl": place holder for the gsl library (https://www.gnu.org/software/gsl) also called by fidl
In addition fidl calls RSCRIPT with scripts written on the fly by the binaries:
      fidl_anova6.cxx calls the R lmerTest which calls the lme4 library.
      fidl_pcat.c calls the RCUR library
