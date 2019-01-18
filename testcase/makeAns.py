#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import subprocess
import sys
import re

def read_txt_file(filename):
    with open(filename,'r') as f:
        txt = f.read().strip()
    # Python should be able to do it automatically, but just in case...
    txt = txt.replace('\r','')
    # Remove version/copyright info of SPIM
    return txt

if __name__ == '__main__':
    names = sys.argv[1:]
    if not names:
        names = sorted(os.listdir('.'))
    for name in names:
        bname,ext = os.path.splitext(name)
        if ext != '.cpp':
            continue
        # Run the test case, redirecting stdout/stderr to output/bname.dout
        cmd = ['make', bname]
        subprocess.call(cmd)
        subprocess.call('./' + bname, stdin = open(bname + '.in', 'r'), stdout = open(bname + '.ans', 'w'))

