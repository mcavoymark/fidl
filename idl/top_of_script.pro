;Copyright 6/30/04 Washington University.  All Rights Reserved.
;top_of_script.pro  $Revision: 1.3 $
pro top_of_script,lu

;printf,lu,'#!/bin/csh'+string(10B)+'unlimit'
;printf,lu,string(10B)+'set BIN = '+!BIN+string(10B)+'set BINLINUX = '+!BINLINUX
;printf,lu,'if(`uname` == Linux) set BIN = $BINLINUX'
;printf,lu,''

printf,lu,'#!/bin/csh'+string(10B)+'unlimit'
printf,lu,string(10B)+'set BIN = '+!BIN+string(10B)+'set BINLINUX = '+!BINLINUX+string(10B)+'set BINLINUX64 = '+!BINLINUX64
printf,lu,'if(`uname` == Linux) then'
printf,lu,'    set dog = `uname -a`'
printf,lu,'    set cat = `expr $#dog - 1`'
printf,lu,'    if($dog[$cat] == x86_64) then'
printf,lu,'        set BIN = $BINLINUX64'
printf,lu,'    else'
printf,lu,'        set BIN = $BINLINUX'
printf,lu,'    endif'
printf,lu,'endif'
printf,lu,''

end
