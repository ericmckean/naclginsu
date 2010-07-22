#!/bin/bash
#
# Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

CURR_DIR=`pwd -P`
export GINSU_ROOT=$CURR_DIR
export SCONS_DIR=$CURR_DIR/third_party/scons/scons-local
exec /bin/bash $CURR_DIR/third_party/swtoolkit/hammer.sh $*
