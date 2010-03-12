// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//#include "model/model.h"
#include "osgViewer/Viewer"

int main(int argc, char* argv[]) {
  //ginsu::model::Model model;

  osg::ArgumentParser arguments(&argc, argv);
  osgViewer::Viewer viewer(arguments);
  viewer.realize();
  return viewer.run();
}
