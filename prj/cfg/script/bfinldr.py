#*******************************************************************************
#*
#*    bfin-elf ldr creator
#*
#*    Version 1.0
#*
#*    Copyright (c) 2016, Harry E. Zhurov
#*
#*******************************************************************************

import os
import sys
import subprocess
import re
import getopt
import struct

objdump = ''
objcopy = ''

#-------------------------------------------------------------------------------
# 
# 
# 
def pexec(cmd):
    p = subprocess.Popen( cmd.split(), universal_newlines = True,
                         stdin  = subprocess.PIPE,
                         stdout = subprocess.PIPE,
                         stderr = subprocess.PIPE )


    out, err = p.communicate()

    return p.returncode, out, err
#-------------------------------------------------------------------------------
def namegen(fullpath, ext):
    name     = os.path.splitext(fullpath)[0]
    return name + os.path.extsep + ext
#-------------------------------------------------------------------------------
def check_tools(toolchain_prefix = ''):

    #------------------------------------------------------------
    #
    #    Check bfin-elf utilities
    #
    
    rcode, out, err = pexec('which ' + os.path.join(toolchain_prefix, 'bfin-elf-objdump'))
    
    if rcode:
        print 'E: cannot find "bfin-elf-objdump" utility'
        objdump = None
    else:
        objdump = out.strip()
        
    rcode, out, err = pexec('which ' + os.path.join(toolchain_prefix, 'bfin-elf-objcopy'))

    if rcode:
        print 'E: cannot find "bfin-elf-objcopy" utility'
        objcopy = None
    else:
        objcopy = out.strip()
             
    if not objdump or not objcopy:
        print '\nHINT: make sure toolchain is installed and/or toolprefix correct\n'
           
    return objdump, objcopy
        
#-------------------------------------------------------------------------------
def get_sections_info(elf):
    rcode, out, err = pexec(objdump + ' -h ' + elf)
    pattern = '\d (\.\w+)\s+(\w+)\s+\w+\s+(\w+).+\n\s+(.+)\n'
    
    sections = {}
    sections['code'] = []
    sections['data'] = []
    
    for i in re.finditer(pattern, out):
        if i.group(4).find('LOAD') != -1:  # group(4) - section flags
            if i.group(4).find('CODE') != -1:
                sections['code'].append( i.groups() )
            if i.group(4).find('DATA') != -1:
                sections['data'].append( i.groups() )
                
    return sections
#-------------------------------------------------------------------------------
def header(*args, **kwargs):
    PFLAGS = 5
    h      = 0
    
    for i in args:
        h |= 1 << i
        
    key = kwargs.keys()[0]
    h |= kwargs[key] << PFLAGS
        
    return h
#-------------------------------------------------------------------------------
def create_ldr_rec(elf, sections, proc, pf_num, init = False):

    wdir = os.path.dirname(elf)
    tmp  = namegen(elf, 'dump')
    
    #-----------------------------------------------------------
    #
    #   Block Header Flag offsets
    #
    ZEROFILL  = 0
    RESVECT   = 1
    INIT      = 3
    IGNORE    = 4
    FINAL     = 15
    
    #-----------------------------------------------------------
    #
    #   Process data sections 
    #   
    outstream = ''
    for section in sections['data']:
        name = section[0]
        size = int(section[1], 16)
        addr = int(section[2], 16)
        rcode, out, err = pexec(objcopy + ' -O binary --only-section=' + name + ' ' + elf + ' ' + tmp )
        if rcode:
            print out + err
            print 'E: nonzero return code from objcopy'
            sys.exit(1)
            
        h = header(RESVECT, PF=pf_num)
        dumpfile = open(tmp, 'rb')
        dumpdata = dumpfile.read()
        dumpfile.close()
        os.remove(tmp)
        outstream += struct.pack('<LLH', addr, size, h) + dumpdata
    
    #-----------------------------------------------------------
    #
    #   Process code sections 
    #   
    for idx, section in enumerate( sections['code'], start=1 ):
        name = section[0]
        size = int(section[1], 16)
        addr = int(section[2], 16)
        rcode, out, err = pexec(objcopy + ' -O binary --only-section=' + name + ' ' + elf + ' ' + tmp )
        if rcode:
            print out + err
            print 'E: nonzero return code from objcopy'
            sys.exit(1)

        if idx == len(sections['code']):
            if init:
                h = header(RESVECT, INIT, PF=pf_num)
            else:
                h = header(RESVECT, FINAL, PF=pf_num)
        else:
            h = header(RESVECT, PF=pf_num)
            
        dumpfile = open(tmp, 'rb')
        dumpdata = dumpfile.read()
        dumpfile.close()
        os.remove(tmp)
        outstream += struct.pack('<LLH', addr, size, h) + dumpdata
    
    #-----------------------------------------------------------
    #
    #   Create dummy section (workaround undocumented 
    #   feature of Blackfin processor loader firmware)
    #
    h = header( RESVECT, IGNORE, PF=pf_num )

    rec = struct.pack( '<LLHL', 0xFF800060, 4, h, len(outstream) ) + outstream
        
    return rec
    
#-------------------------------------------------------------------------------
#def elf2ldr(elf, init_elf, target, pf_num, tool_prefix=''):
def elf2ldr(**kwargs):
    
    keys = kwargs.keys()
    if keys.count('elf'): elf = kwargs['elf']
    else:                 elf = ''
    
    if keys.count('init_elf'): init_elf = kwargs['init_elf']
    else:                      init_elf = None
        
    if keys.count('target'): target = kwargs['target']
    else:                    target = ''
    
    if keys.count('pf_num'): pf_num = kwargs['pf_num']
    else:                    pf_num = 1
    
    if keys.count('tool_prefix'): tool_prefix = kwargs['tool_prefix']
    else:                         tool_prefix = ''
    
    if target != 'bf531' and target != 'bf532' and target != 'bf533':
        print 'E: invalid target specified. Only bf531, bf532 and bf533 are supported for now'
        sys.exit(1)
        
    if init_elf and not os.path.exists(init_elf):
        print 'E: input <init>.elf file does not exists'
        sys.exit(1)

    if pf_num < 1 or pf_num > 15:
        print 'E: PF number must be in range 1..15'
        sys.exit(1)
            
    if not os.path.exists(elf):
        print 'E: input elf file does not exists'
        sys.exit(1)
        
    if not target:
        print 'E: target processor does not specified'
        sys.exit(1)

    global objdump
    global objcopy

    objdump, objcopy = check_tools(tool_prefix)

    if not objdump or not objcopy:
        sys.exit(1)
                        
    #-----------------------------------------------------------
    #
    #   Perform actions
    #   
    init_rec = ''
    if init_elf:
        print '    process init elf sections...',
        sections = get_sections_info(init_elf)
        init_rec = create_ldr_rec(init_elf, sections, target, pf_num, True)
        print 'done'

    print '    process main elf sections...',
    main_rec = ''  
    sections = get_sections_info(elf)
    main_rec = create_ldr_rec(elf, sections, target, pf_num)
    print 'done'

    print '    create ldr image file with PF' + str(pf_num) + ' as HWAIT'
    ldr_name = namegen(elf, 'ldr')
    open(ldr_name, 'wb').write(init_rec + main_rec)
    
#-------------------------------------------------------------------------------
def main():

    #-----------------------------------------------------------
    #
    #   Process argument list
    #   
    optlist, f = getopt.gnu_getopt(sys.argv[1:], 'i:p:t:b:')
    
    if f == []:
        print 'bfin-elf elf->ldr utility, (c) 2016, Harry E. Zhurov'
        print '\n Usage: bfin-ldr.py [options] <elf-file>'
        print '     where options:'
        print '        -t : target (bf531, bf532 or bf533)'
        print '        -i : init executable elf'
        print '        -p : PF number for HWAIT, defaut is PF1'
        print '        -b : prefix for bfin toolchain bin files (example: /opt/bfin-elf/bin.'
        print '             Not need if toolchain executable dir is in $PATH'
        print ''
        print 'The result is the LDR file in raw binary format with same name as input elf-file'
        print 'but with ".ldr" extension'
        sys.exit(0)
    
    if len(f) > 1:  # elf is list object for now
        print 'E: only one input elf-file should be specified'
        sys.exit(1)
        
    elf       = f[0]      # get filename
    init_elf  = ''
    target    = ''
    pf_num    = 1
    tool_prefix = ''
    for i in optlist:
        if i[0] == '-t':
            target = i[1]
        elif i[0] == '-i':
            init_elf = i[1]
        elif i[0] == '-p':
            pf_num = i[1]
            if not pf_num.isdigit():
                print 'E: PF number must be digital value'
                sys.exit(1)
            pf_num = int(pf_num)
        elif i[0] == '-b':
            tool_prefix = i[1]
        
    elf2ldr(elf=elf, init_elf=init_elf, target=target, pf_num=pf_num, tool_prefix=tool_prefix)
                    
#-------------------------------------------------------------------------------
if __name__ == '__main__':
    main()
#-------------------------------------------------------------------------------


