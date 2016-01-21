#*******************************************************************************
#*
#*    Build support utilities
#*
#*    Version 1.0
#*
#*    Copyright (c) 2016, Harry E. Zhurov
#*
#*******************************************************************************

import os
import subprocess

#-------------------------------------------------------------------------------
# 
# 
# 
def namegen(fullpath, ext):
    basename = os.path.basename(fullpath)
    name     = os.path.splitext(basename)[0]
    return name + os.path.extsep + ext
#-------------------------------------------------------------------------------
def pexec(cmd):
    p = subprocess.Popen( cmd.split(), universal_newlines = True,
                         stdin  = subprocess.PIPE,
                         stdout = subprocess.PIPE,
                         stderr = subprocess.PIPE )


    out, err = p.communicate()

    return p.returncode, out, err
    
#-------------------------------------------------------------------------------

