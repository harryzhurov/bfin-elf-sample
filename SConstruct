#*******************************************************************************
#
#     SCons main construct file
#
#     Processor: Blackfin (Analog Devices)
#
#     Toolkit:   VisualDSP++
#
#     Copyright (c) 2006-2015, Harry E. Zhurov
#
#-------------------------------------------------------------------------------

import os
import sys
import glob
import subprocess
import re

sys.path.append(os.getcwd() + '/cfg/script')

import utils
import get_info
import vdsp_lst
import scmIC

#===============================================================================
#
#     User definable area
#
#-------------------------------------------------------------------------------
#
#     Project settings
#
ProjectName  = 'slon'

SourceDirs   = ['src']
                
#--------------------------------------
#
#     Options
#
Processor    = 'bf533'
DeviceRev    = '0.6'

#Optimization = ' '
Optimization =' -O3 '
DebugLevel   =' -g3 '          # empty to disable debug info

ExtraAFlags  = ' '
ExtraCFlags  = ' '
ExtraLFlags  = ' '


#===============================================================================
#
#     !!! Service Area !!!
#
#
#-------------------------------------------------------------------------------
#
#    Project structure
#
ConfigDir   = 'cfg'
ScriptDir   = 'cfg/script'
ObjDir      = 'obj'
ListDir     = 'lst'
ExeDir      = 'bin'

ServiceDirs = [ ObjDir, ListDir, ExeDir ]
for i in ServiceDirs:
    curdir = os.path.join( os.getcwd(), i )
    if not os.path.exists(curdir):
        print 'Directory ' + curdir + ' does not exists. Creating the directory...',
        os.mkdir(curdir)
        print 'done'

#-------------------------------------------------------------------------------
#
#    Platform specific stuff
#
AsmExt  = 'S'
CExt    = 'c'
CppExt  = 'cpp'
ObjExt  = 'o'
LstExt  = 'lst'
MapExt  = 'map'
BinExt  = 'elf'
HexExt  = 'ldr'
LdExt   = 'ld'

#-------------------------------------------------------------------------------
#
#      Toolkit
#
TOOLKIT_PATH = os.environ['CAD'] + '/adi/bfin-elf'

ASM      = TOOLKIT_PATH + '/bin/bfin-elf-as'
CC       = TOOLKIT_PATH + '/bin/bfin-elf-gcc'
Linker   = TOOLKIT_PATH + '/bin/bfin-elf-ld'
#XML2HTML = TOOLKIT_PATH + '/bin/bfin-elf-'
#ELFDUMP  = TOOLKIT_PATH + '/bin/bfin-elf-'
LOADER   = TOOLKIT_PATH + '/bin/bfin-elf-ldr'

ToolkitIncPath = [TOOLKIT_PATH] # + '/bfin-elf/include'] #,  TOOLKIT_PATH + '/Blackfin/lib/src/libio']

ToolkitLibPath = [TOOLKIT_PATH] # + '/Blackfin\lib532_rev_' + DeviceRev]

#-------------------------------------------------------------------------------
#
#      Tools options
#
#----------------------------------------------------
#
#      Include, library, config files and paths
#
IncludePath = ''.join(' -I' + i for i in SourceDirs + ToolkitIncPath)
#IncludePathAsm = ''.join(' -flags-asm -I' + i for i in SourceDirs + ToolkitIncPath)
LibraryPathOptions = ''  #''.join(' -Wl,-L' + i for i in ToolkitLibPath)


LinkerMainScript   = os.path.join(ConfigDir, Processor + 'cpp.' + LdExt)
LinkerMapFile      = os.path.join(ListDir, ProjectName + os.extsep + MapExt)

#-------------------------------------------------------------------------------
#
#    The options
#
COMMON_FLAGS = ' -mcpu=' + Processor + '-' + DeviceRev

AFLAGS  = COMMON_FLAGS
AFLAGS += IncludePath
AFLAGS += DebugLevel  
AFLAGS += ' -x assembler-with-cpp'
AFLAGS += ' -c '
AFLAGS += ExtraAFlags
#-------------------------------------------------------------------------------
CFLAGS  = COMMON_FLAGS
#CFLAGS += ' -C'                       # Retains preprocessor comments in the output file
#CFLAGS += ' -fmessage-length=0'        # output messages in one line
CFLAGS += ' -c'                       # Compiles and/or assembles only, but does not link
CFLAGS += DebugLevel
CFLAGS += ' -pipe'
CFLAGS += ' -ffunction-sections -fdata-sections'
#CFLAGS += ' -Wsuppress cc1164'        # Suppress warning about "void main()"
#CFLAGS += ' -path-output ' + ObjDir
#CFLAGS += ' -save-temps'
CFLAGS += IncludePath + Optimization + ExtraCFlags
CPPFLAGS  = CFLAGS #+ ' -c++ -no-implicit-inclusion '
CPPFLAGS += ' -fno-exceptions -fno-rtti'

#-------------------------------------------------------------------------------
LFLAGS  = COMMON_FLAGS
LFLAGS += ' -Wl,--gc-sections'
LFLAGS += ' -Wl,--script=' + LinkerMainScript
LFLAGS += ' -Wl,-Map=' + LinkerMapFile + ',--cref'
LFLAGS += ' -Wl,-L '   + ObjDir + LibraryPathOptions
LFLAGS += ' -nostdlib -lgcc'
LFLAGS += ExtraLFlags

#-------------------------------------------------------------------------------
#
#    Action functions
#
#-------------------------------------------------------------------------------
#
#    Build object file from c source file
#
def compile_c(target, source, env):
    #------------------------------------------------
    #
    #   Check if the first source is compiling
    #
    if env['FIRST_ENTRY'] == False:
        env['FIRST_ENTRY'] = True
        print '*'*60

    #------------------------------------------------
    #
    #   Launch compiler
    #
    print 'Compile:  ' + os.path.basename( str(source[0]) )
    cmd = env['CC'] + env['CFLAGS'] + ' -o ' + str(target[0]) + ' ' + str(source[0])
    print cmd
    p = subprocess.Popen(cmd.split(), universal_newlines = True,
                         stdin  = subprocess.PIPE,
                         stdout = subprocess.PIPE,
                         stderr = subprocess.PIPE )

    out, err = p.communicate()
    out += err
    if out:
        print out.replace('`', '\'')
    
    rcode = p.returncode

    if rcode != 0:
        return rcode

#-------------------------------------------------------------------------------
#
#    Build object file from cpp source file
#
def compile_cpp(target, source, env):
    #------------------------------------------------
    #
    #   Check if the first source is compiling
    #
    if env['FIRST_ENTRY'] == False:
        env['FIRST_ENTRY'] = True
        print '*'*60

    #------------------------------------------------
    #
    #   Launch compiler
    #
    #print 'Compile:  ' + os.path.relpath( str(source[0]) )
    print 'Compile:  ' + os.path.basename( str(source[0]) )
    cmd = env['CC'] + env['CPPFLAGS'] + ' -o ' + str(target[0]) + ' ' + str(source[0])
    #print cmd.split()
    p = subprocess.Popen(cmd.split(), universal_newlines = True,
                         stdin  = subprocess.PIPE,
                         stdout = subprocess.PIPE,
                         stderr = subprocess.PIPE )

    out, err = p.communicate()
    out += err
    if out:
        print out.replace('`', '\'')

    rcode = p.returncode

    if rcode != 0:
        return rcode


#-------------------------------------------------------------------------------
#
#    Build object file from assembler source file
#
def assembly_src(target, source, env):
    #------------------------------------------------
    #
    #   Check if the first source is compiling
    #
    if env['FIRST_ENTRY'] == False:
        env['FIRST_ENTRY'] = True
        print '*'*60

    #------------------------------------------------
    #
    #   Launch assembler
    #
    src_name = str(source[0])
    print 'Assembly: ' + src_name
    cmd = env['CC'] + env['AFLAGS'] + ' -o ' + str(target[0]) + ' ' + src_name
    print cmd
    p = subprocess.Popen( cmd.split() )
    p.wait()
    return p.returncode
#-------------------------------------------------------------------------------
#
#    Build executable file from object files
#
def build_bin(target, source, env):
    if env['FIRST_ENTRY'] == False:
        print '*'*60
        print 'Sources are up to date'

    print '*'*60
    print 'Link: ' + str(target[0])

    src_list = ''
    for i in xrange(len(source)):
        src_list += ' ' + str(source[i])

    target_name_full  = str(target[0])
    target_name_short = os.path.split(target_name_full)[1]
    target_name       = os.path.splitext(target_name_short)[0]
    dumpfile_name = target_name_full + '.dmp'

    #-------------------------------------------------------------
    #
    #    Link executable
    #
    cmd  =  env['CC'] + env['LFLAGS'] + ' -o ' + target_name_full + src_list

    print cmd
    p = subprocess.Popen(cmd.split(), universal_newlines = True,
                         stdin  = subprocess.PIPE,
                         stdout = subprocess.PIPE,
                         stderr = subprocess.PIPE )

    out, err = p.communicate()
    out += err
    if out:
        print out.replace('`', '\'')

    rcode = p.returncode

    print '*'*60
    if rcode != 0:
        return rcode

    #-------------------------------------------------------------
    #
    #    Create map file
    #
#   cmd = env['XML2HTML'] + ' ' + ListDir + '/' + mapfile_name
#   p = subprocess.Popen(cmd)
#   rcode = p.wait()
#   if rcode != 0:
#       return

    #-------------------------------------------------------------
    #
    #    Create dump of output sections
    #
#   cmd = env['ELFDUMP'] + ' ' + target_name_full
#   f = open(dumpfile_name, 'w')
#   p = subprocess.Popen(cmd, stdout = f)
#   f.close()
#   rcode = p.wait()
#   if rcode != 0:
#       return
#
#   get_info.get_info(dumpfile_name)

#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#
#   Builder objects
#
c2obj      = Builder(action         = compile_c,
                     suffix         = ObjExt,
                     src_suffix     = CExt,
                     source_scanner = CScanner)  
#-------------------------------------------------------------------------------
cpp2obj    = Builder(action         = compile_cpp,
                     suffix         = ObjExt,
                     src_suffix     = CppExt,
                     source_scanner = CScanner)  
#-------------------------------------------------------------------------------
asm2obj    = Builder(action         = assembly_src,
                     suffix         = ObjExt,
                     src_suffix     = AsmExt,
                     source_scanner = CScanner)
#-------------------------------------------------------------------------------
binBuilder = Builder(action     = build_bin,
                     suffix     = BinExt,
                     src_suffix = ObjExt)

#-------------------------------------------------------------------------------
#
#    State variables
#
FirstEntry = False

#-------------------------------------------------------------------------------
#
#    Environment
#

env = Environment(TOOLS = {})

env['BUILDERS'] = {
                     'asmObj' : asm2obj,
                     'cObj'   : c2obj,
                     'cppObj' : cpp2obj,
                     BinExt   : binBuilder
                  }

env['ASM'          ] = ASM
env['CC'           ] = CC
env['LINKER'       ] = Linker
#env['XML2HTML'     ] = XML2HTML
#env['ELFDUMP'      ] = ELFDUMP
env['LOADER'       ] = LOADER
env['AFLAGS'       ] = AFLAGS
env['CFLAGS'       ] = CFLAGS
env['CPPFLAGS'     ] = CPPFLAGS
env['LFLAGS'       ] = LFLAGS
env['SRC_PATH'     ] = SourceDirs[0:2] 
env['CPPPATH'      ] = SourceDirs + ToolkitIncPath
env['FIRST_ENTRY'  ] = FirstEntry

#-------------------------------------------------------------------------------
#
#    Service functions
#
#-------------------------------------------------------------------------------
#
#   Make dictionary in form { src : obj } from input source list
#
def make_target_dict(src_list):
    targets = {}

    for i in src_list:
        name_ext = os.path.split(i)[1]
        name     = os.path.splitext(name_ext)[0] + '.' + ObjExt
        targets[i] = ObjDir + '/' + name
        Depends(targets[i],  'SConstruct')      

    return targets

#-------------------------------------------------------------------------------
#
#   Make objects from sources
#
def make_objects(asm, cpp, c = {}):
    obj_list = []
    for i in c.items():
        obj_list.append( env.cObj(i[1], i[0]) )

    for i in cpp.items():
        obj_list.append( env.cppObj(i[1], i[0]) )

    for i in asm.items():
        obj_list.append( env.asmObj(i[1], i[0]) )

    return obj_list

#-------------------------------------------------------------------------------
#
#   Prepare source files list for object building
#
#-------------------------------------------------------------------------------
#
#    C source file list
#
c_files = []
for i in SourceDirs:
    c_files += glob.glob(i + '/*.' + CExt)

c = make_target_dict(c_files)

#-------------------------------------------------------------------------------
#
#    Cpp source file list
#
cpp_files = []
for i in SourceDirs:
    cpp_files += glob.glob(i + '/*.' + CppExt)

cpp = make_target_dict(cpp_files)
       
#-------------------------------------------------------------------------------
#
#    Asm source file list
#
asm_files = []
for i in SourceDirs:
    asm_files += glob.glob(i + '/*.' + AsmExt)

asm = make_target_dict(asm_files)

#-------------------------------------------------------------------------------
#
#    Objects builing
#
obj_list        = make_objects(asm, cpp, c)

#-------------------------------------------------------------------------------
#
#    Executable targets building
#
main_trg   = env.elf(source = obj_list, target = ExeDir +'/' + ProjectName)


#-------------------------------------------------------------------------------
#
#    Clean and Rebuid
#
def remove_files(dir_, mask):
    file_list = glob.glob(dir_ + '/*.' + mask)
    for i in file_list:
        os.remove(i)

#-------------------------------------------------------------------------------
def clean(target, source, env):
    print 'Cleaning targets...',
    if os.path.exists('.sconsign.dblite'): os.remove('.sconsign.dblite')

    remove_files(ObjDir,    ObjExt)
    remove_files(ObjDir,    's')
    remove_files(ObjDir,    'lst')
    remove_files(ObjDir,    'sbn')
    remove_files(ObjDir,    'ii')
    remove_files(ObjDir,    'ti')
    remove_files(ExeDir,    BinExt)
    remove_files(ExeDir,    HexExt)
    remove_files(ExeDir,    'dmp')
    remove_files(ExeDir,    'xml')
    remove_files(ListDir,   'html')
    remove_files(ListDir,   'xml')
    remove_files(ListDir,   'lst')
    remove_files(ConfigDir, 'is')
    remove_files(ScriptDir, 'pyc')
    print 'Done'


#-------------------------------------------------------------------------------
#
#
#
def check_scmRTOS_cfg(target, source, env):
    if env['FIRST_ENTRY'] == False:
        print '*'*60
        print 'Sources are up to date'

    print 'Run integrity checker...',
    rcode = scmIC.checker(env['SRC_PATH'])
    if rcode != 0:
        print '\n scmRTOS configuration is invalid.'

    print '*'*60

#-------------------------------------------------------------------------------
#
#       Targets
#
#-------------------------------------------------------
#
#   Intermediate targets
#
main      = env.Alias(ProjectName, ExeDir + '/' + ProjectName + '.' + BinExt)
clean_all = env.Alias('cln',      action = clean)
ic        = env.Alias('ic',       action = check_scmRTOS_cfg)

all = [main, ic]

#-------------------------------------------------------------------------------
#
#    Final targets
#
env.Alias('all', all)
env.Alias('rebuild', [clean_all, all])

env.AlwaysBuild('all', 'cln', 'ic')

Default(all)

#-------------------------------------------------------------------------------
              
#===============================================================================




