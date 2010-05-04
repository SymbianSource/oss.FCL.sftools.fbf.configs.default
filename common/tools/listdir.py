# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#
# Contributors:
# mattd <mattd@symbian.org>
#
# Description:
# listdir.py - Lists a directory contents.
# listdir.py <directory> (<exclude_directory>)

import os
import re
import sys
import string
from os.path import join, isfile

def main():
  directory = sys.argv[1]
  exclude_dirs = []
  if(len(sys.argv)>2):
    x_dirs = string.lower(sys.argv[2])
    exclude_dirs = re.split(',', x_dirs)
  scandir(directory, exclude_dirs)

def scandir(top, exclude_dirs):
    fixpath = re.compile('\\\\')    
    fixroot = re.compile('^%s\\\\' % top)
    for root, dirs, files in os.walk(top, topdown=True):
        for dirname in dirs:
            if(string.lower(fixpath.sub('/',os.path.join(root,dirname))) in exclude_dirs):
              dirs.remove(dirname)
        for name in files:
            filename = os.path.join(root, name)
            fn = string.lower(fixpath.sub('/',fixroot.sub('',filename)))
            print top+"/"+fn

main()
