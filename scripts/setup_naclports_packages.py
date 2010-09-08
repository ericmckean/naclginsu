#!/usr/bin/python2.4
#
# Copyright 2010 Google Inc. All Rights Reserved.

"""One-line documentation for setup_packages module.

A detailed description of setup_packages.
"""

__author__ = 'sanga@google.com (Sang Ahn)'


import os
import shutil
import subprocess

def setup_packages():
  if os.getenv('NACL_TOOLCHAIN_ROOT') is None:
    raise AssertionError, "NACL_TOOLCHAIN_ROOT must be defined."
  # get the packages from svn
  subprocess.call(["svn",
                   "checkout",
                   "http://naclports.googlecode.com/svn/trunk@106",
                   ".naclports"])
  root = os.getcwd()
  path = os.path.join(root, ".naclports", "src", "packages", "scripts")
  os.chdir(path)
  packages = ["boost_1_43_0", "OpenSceneGraph-2.9.7", "gtest-1.5.0",
              "gmock-1.5.0"]
  # Go into each package directory and run the script of each individual
  # package.
  for p in packages:
    os.chdir(p)
    subprocess.call(["./nacl-%s.sh" % p])
    os.chdir(path)
  # cleanup
  os.chdir(root)
  shutil.rmtree('.naclports')

if __name__ == '__main__':
  setup_packages()
