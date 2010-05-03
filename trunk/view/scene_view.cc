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
  impl_->setSceneData(scene_root);
  impl_->init();
}

void SceneView::SetViewport(int x, int y, int width, int height) {
  impl_->setViewport(x, y, width, height);
}

void SceneView::SetViewMatrix(const osg::Matrixf& matrix) {
  impl_->setViewMatrix(matrix);
}

void SceneView::SetProjectionMatrix(const osg::Matrixf& matrix) {
  impl_->setProjectionMatrix(matrix);
}

void SceneView::Draw() {
  impl_->update();
  impl_->cull();
  impl_->draw();
}

}  // namespace view
}  // namespace ginsu
