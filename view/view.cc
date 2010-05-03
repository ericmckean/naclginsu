// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "view/view.h"

#include "osg/Matrixf"
#include "view/scene.h"
#include "view/scene_view.h"

namespace ginsu {
namespace view {

View::View() {
}

View::~View() {
}

void View::InitGL() {
  scene_.reset(new Scene);
  scene_->Init();

  scene_view_.reset(new SceneView);
  scene_view_->Init(scene_->root());

  // Set the camera so that everything in the scene is shown.
  const osg::BoundingSphere& bound = scene_->GetBound();
  const osg::Vec3f& target = bound.center();
  // Eye is on the negative y-axis.
  osg::Vec3f eye = target + osg::Vec3f(0.0f, -4.0f * bound.radius(), 0.0f);
  // Up is along the z-axis.
  osg::Vec3f up(0.0f, 0.0f, 1.0f);
  scene_view_->SetViewMatrix(osg::Matrixf::lookAt(eye, target, up));
}

void View::ReleaseGL() {
  scene_view_.reset();
  scene_.reset();
}

void View::SetWindowSize(int width, int height) {
  scene_view_->SetViewport(0, 0, width, height);

  float fovy = 30.0f;
  float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
  float z_near = 1.0f;
  float z_far = 1000.0f;
  scene_view_->SetProjectionMatrix(osg::Matrixf::perspective(
      fovy, aspect_ratio, z_near, z_far));
}

void View::Draw() {
  scene_->Update();
  scene_view_->Draw();
}

}  // namespace view
}  // namespace ginsu
