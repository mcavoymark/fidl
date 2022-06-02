#!/bin/csh
set UNAME = `uname`
if($UNAME == Linux) then
    gunzip idl71linux.x86.tar.gz 
    tar xf idl71linux.x86.tar
    rm -rf idl71linux.x86.tar install license
else if($UNAME == SunOS) then
    gunzip idl71solaris2.sparc.tar.gz 
    tar xf idl71solaris2.sparc.tar
    rm -f idl71solaris2.sparc.tar 
else
    echo $UNAME  Unknown operating system!
endif
