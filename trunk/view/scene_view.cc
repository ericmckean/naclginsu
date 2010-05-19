// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "view/scene_view.h"

#include "osgUtil/SceneView"

namespace ginsu {
namespace view {

SceneView::SceneView() {
}

SceneView::~SceneView() {
}

void SceneView::Init(osg::Node* scene_root) {
  impl_ = new osgUtil::SceneView;
  impl_->setDefaults();
  impl_->setClearColor(osg::Vec4(0.8, 0.8, 0.0, 1.0));
  impl_->setSceneData(scene_root);
  impl_->init();

  timer_.setStartTick();
}

void SceneView::SetViewport(int x, int y, int width, int height) {
  impl_->setViewport(x, y, width, height);
}

void SceneView::SetViewMatrix(const osg::Matrix& matrix) {
  impl_->setViewMatrix(matrix);
}

void SceneView::SetProjectionMatrix(const osg::Matrix& matrix) {
  impl_->setProjectionMatrix(matrix);
}

void SceneView::SetLookAt(const osg::Vec3& eye,
                          const osg::Vec3& target,
                          const osg::Vec3& up) {
  eye_ = eye;
  target_ = target;
  up_ = up;
}

void SceneView::Draw() {
  // TODO(alokp): Remove camera animation after demo.
  double elapsed_sec = timer_.time_s();
  // Rotate the along z-axis.
  osg::Matrix rotate;
  const static osg::Vec3 axis(0.0f, 0.0f, 1.0f);
  float angle = 0.5f * static_cast<float>(elapsed_sec);
  rotate.makeRotate(angle, axis);
  eye_ = osg::Matrix::transform3x3(eye_, rotate);
  impl_->setViewMatrix(osg::Matrix::lookAt(eye_, target_, up_));

  impl_->update();
  impl_->cull();
  impl_->draw();

  timer_.setStartTick();
}

}  // namespace view
}  // namespace ginsu
