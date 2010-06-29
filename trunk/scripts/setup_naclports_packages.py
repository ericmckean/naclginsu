#!/usr/bin/python2.4
#
# Copyright 2010 Google Inc. All Rights Reserved.

"""One-line documentation for setup_packages module.

A detailed description of setup_packages.
"""

__author__ = 'sanga@google.com (Sang Ahn)'


import os
import subprocess

def setup_packages():
  if os.getenv('NACL_SDK_BASE') is None:
    raise AssertionError, "NACL_SDK_BASE must be defined."
  # get the packages from svn
  subprocess.call(["svn",
                   "checkout",
                   "http://naclports.googlecode.com/svn/trunk@40",
                   "naclports"])
  # cd in the packages directory.
  path = os.path.join("naclports", "src", "packages", "scripts")
  os.chdir(path)
  # cd into boost
  os.chdir('boost_1_43_0')
  # kick off script
  subprocess.call(['./nacl-boost_1_43_0.sh'])
  # cd into osg
  os.chdir('../OpenSceneGraph-2.9.7')
  # kick off script
  subprocess.call(['./nacl-OpenSceneGraph-2.9.7.sh'])

if __name__ == '__main__':
  setup_packages()
