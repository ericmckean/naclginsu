// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_VIEW_SCENE_VIEW_H_
#define GINSU_VIEW_SCENE_VIEW_H_

#include "osg/Matrix"
#include "osg/ref_ptr"
#include "osg/Timer"
#include "osg/Vec3"

namespace osg {
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
  void SetViewMatrix(const osg::Matrix& matrix);
  void SetProjectionMatrix(const osg::Matrix& matrix);
  void Draw();

  // TODO(alokp): Remove this.
  void SetLookAt(const osg::Vec3& eye,
                 const osg::Vec3& target,
                 const osg::Vec3& up);

 private:
  osg::Timer timer_;
  osg::Vec3 eye_, target_, up_;
  osg::ref_ptr<osgUtil::SceneView> impl_;
};

}  // namespace view
}  // namespace ginsu
#endif  // GINSU_VIEW_SCENE_VIEW_H_
