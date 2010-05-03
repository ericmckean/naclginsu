// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_VIEW_SCENE_VIEW_H_
#define GINSU_VIEW_SCENE_VIEW_H_

#include "osg/ref_ptr"

namespace osg {
class Matrixf;
class Node;
}
namespace osgUtil {
class SceneView;
}

namespace ginsu {
namespace view {

class SceneView {
 public:
  SceneView();
  ~SceneView();

  void Init(osg::Node* scene_root);
  void SetViewport(int x, int y, int width, int height);
  void SetViewMatrix(const osg::Matrixf& matrix);
  void SetProjectionMatrix(const osg::Matrixf& matrix);
  void Draw();

 private:
  osg::ref_ptr<osgUtil::SceneView> impl_;
};

}  // namespace view
}  // namespace ginsu
#endif  // GINSU_VIEW_SCENE_VIEW_H_
