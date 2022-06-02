#Copyright 8/29/06 Washington University.  All Rights Reserved.
#make_fidl.csh  $Revision 1.4 $




#if($#argv < 2) then
#    echo Usage: make_fidl.csh FIDL_PATH REV
#    echo Ex.1 make_fidl.csh /usr/local/bin/fidl_code 2.61 #NIL
#    echo Ex.2 make_fidl.csh /usr/local/fidl 2.61 #psych
#    exit
#endif
#set FIDL_PATH = $1
#set REV = $2
#set FIDL = ${FIDL_PATH}/fidl_${REV}/scripts/fidl

if($#argv < 3) then
    echo Usage: make_fidl.csh INSTALL_PATH FIDL_PATH REV
    exit
endif
set FIDL = $1/scripts/fidl
set FIDL_PATH = $2
set REV = $3





echo '#\!/bin/csh' > $FIDL
echo '           ' >> $FIDL
echo '# fidl 2.63 16pt 1024scrx 800scry 24color cmdline' >> $FIDL
echo '#    2.63       Call fidl version 2.63. Not including this option calls the most recent version.' >> $FIDL
echo '#    16pt       Text size' >> $FIDL
echo '#    1024scrx   X display dimension. MUST BE LESS THAN YOUR MONITOR SIZE.' >> $FIDL
echo '#               1024 is the default.' >> $FIDL
echo '#    800scry    Y display dimension. MUST BE LESS THAN YOUR MONITOR SIZE.' >> $FIDL
echo '#               800 is the default.' >> $FIDL
echo '#    24color    Color depth. Default is 8 bit color.' >> $FIDL
echo '#    cmdline    Command line.' >> $FIDL
echo '#    vm         Virtual machine.' >> $FIDL
echo '#               Default is a runtime license. That is no command line and no virtual machine.' >> $FIDL
echo '           ' >> $FIDL
echo 'set FIDL_PATH = '${FIDL_PATH} >> $FIDL
echo 'set REV = '${REV} >> $FIDL
#echo 'echo `/usr/ucb/whoami` start on `/usr/bin/date` >> ${FIDL_PATH}/fidl_${REV}/log' >> $FIDL
echo 'set cmd1 = ${FIDL_PATH}/fidl_${REV}/scripts/fidl_rt' >> $FIDL
echo 'set cmd =' >> $FIDL
echo 'set i = 1' >> $FIDL
echo 'while ($i <= $#argv)' >> $FIDL

#echo '    set y = ` echo $argv[$i] | /usr/xpg4/bin/grep -F .`' >> $FIDL
#START140402
echo '    if(`uname` == Linux) then' >> $FIDL
echo '        set y = ` echo $argv[$i] | grep -F .`' >> $FIDL
echo '    else' >> $FIDL
echo '        set y = ` echo $argv[$i] | /usr/xpg4/bin/grep -F .`' >> $FIDL
echo '    endif' >> $FIDL





echo '    if($#y > 0) then' >> $FIDL
echo '        set cmd1 = $FIDL_PATH/fidl_$argv[$i]/scripts/fidl_rt' >> $FIDL
echo '    else' >> $FIDL
echo '        set cmd = "$cmd $argv[$i]"' >> $FIDL
echo '    endif' >> $FIDL
echo '    @ i++' >> $FIDL
echo 'end' >> $FIDL
echo 'echo $cmd1 $cmd' >> $FIDL
echo '$cmd1 $cmd' >> $FIDL
#echo 'echo `/usr/ucb/whoami` ended on `/usr/bin/date` >> ${FIDL_PATH}/fidl_${REV}/log' >> $FIDL
chmod +x $FIDL
