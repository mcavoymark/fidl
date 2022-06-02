#!/bin/tcsh

# Set these three fields.
#------------------------
set FIDL_PATH = 
set LIB_PATH = 
set VM_PATH =

#Definitions.
#------------
# FIDL_PATH    where fidl is to be installed
# LIB_PATH     location of fidl's library
# VM_PATH      location of the idl virtual machine

#Example 1
#---------
#set FIDL_PATH = /mypath
#set LIB_PATH = /mypath/lib
#set VM_PATH = /mypath/vm/idl71linux
#
#Then to call fidl after installation:
#    /mypath/fidl_2.65/scripts/fidl 24color 18pt

#Example 2
#---------
#set FIDL_PATH = /data/nil-bluearc/raichle/mcavoy/fidl/170221
#set LIB_PATH = /home/usr/fidl/lib 
#set VM_PATH = /home/usr/fidl/vm/idl71linux
#
#Then to call fidl after installation:
#    /data/nil-bluearc/raichle/mcavoy/fidl/170221/fidl_2.65/scripts/fidl 24color 18pt

if($#argv != 1) then
    echo Usage: tcsh install_fidl.csh compressed_tar_file
    exit
endif

set TAR_FILE = $1
set SCRAP = $TAR_FILE:r
set SCRAP = $SCRAP:r
echo $SCRAP:s/rev/ / > scrap.dat
set REV = `gawk '{print $2;}' scrap.dat`
rm scrap.dat
echo REV = $REV
set INSTALL_PATH = ${FIDL_PATH}/fidl_${REV}
set D = `pwd`

gunzip $TAR_FILE

cd $FIDL_PATH
echo "Extracting files."
tar xf $D/$TAR_FILE:r

ln -s ${INSTALL_PATH}/bin_linux64/libgsl.so.0.17.0 ${INSTALL_PATH}/bin_linux64/libgsl.so
ln -s ${INSTALL_PATH}/bin_linux64/libgsl.so.0.17.0 ${INSTALL_PATH}/bin_linux64/libgsl.so.0
ln -s ${INSTALL_PATH}/bin_linux64/libgslcblas.so.0.0.0 ${INSTALL_PATH}/bin_linux64/libgslcblas.so
ln -s ${INSTALL_PATH}/bin_linux64/libgslcblas.so.0.0.0 ${INSTALL_PATH}/bin_linux64/libgslcblas.so.0

#csh ${INSTALL_PATH}/idl_6.3/make_fidl.csh $INSTALL_PATH $FIDL_PATH ${REV}
#csh ${INSTALL_PATH}/idl_6.3/make_fidl_rt.csh $LIB_PATH $INSTALL_PATH ${INSTALL_PATH}/idl_6.3 $IDL_PATH $VM_PATH $LICENSE_FILE
#START170221
csh ${INSTALL_PATH}/idl_6.3/make_fidl.csh $INSTALL_PATH $FIDL_PATH ${REV}
csh ${INSTALL_PATH}/idl_6.3/make_fidl_rt_export.csh $LIB_PATH $INSTALL_PATH ${INSTALL_PATH}/idl_6.3 $VM_PATH
#csh /home/hannah/mcavoy/idl/make_fidl.csh $INSTALL_PATH $FIDL_PATH ${REV}
#csh /home/hannah/mcavoy/idl/make_fidl_rt_export.csh $LIB_PATH $INSTALL_PATH ${INSTALL_PATH}/idl_6.3 $VM_PATH

rm $D/$TAR_FILE:r
