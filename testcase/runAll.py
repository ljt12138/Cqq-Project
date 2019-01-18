#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import subprocess
import sys
import re
import time

def read_txt_file(filename):
    with open(filename,'r') as f:
        txt = f.read().strip()
    # Python should be able to do it automatically, but just in case...
    txt = txt.replace('\r','')
    # Remove version/copyright info of SPIM
    return txt

if __name__ == '__main__':
    my_cpp = os.path.join('..','result','driver')
    names = sys.argv[1:]
    if not names:
        names = sorted(os.listdir('.'))
    for name in names:
        bname,ext = os.path.splitext(name)
        if ext != '.cpp':
            continue
        # Run the test case, redirecting stdout/stderr to output/bname.dout
        cmd = [my_cpp, name]
        N = 3
        t = 0
        for i in range(N):
            start = time.time()
            subprocess.call(cmd,
                            stdin = open(bname+'.in', 'r'),
                            stdout = open(bname+'.out', 'w'))
            end = time.time()
            t = t + end - start
        t /= N
        reference = read_txt_file(bname+'.out')
        our_result = read_txt_file(bname+'.ans')
        
        if reference == our_result:
            info = 'OK :)'
        else:
            info = 'ERROR!'
        print ('{0:<40}{1} > time = {2:.4}'.format(name,info,t))
        
    if os.name == 'nt':
        print ('Press Enter to continue...')
        try:
            raw_input() # Python 2
        except:
            input() # Python 3

            
