// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "view/view.h"

#include "osg/Matrix"
#include "osg/Vec3"
#include "view/scene.h"
#include "view/scene_view.h"

namespace ginsu {
namespace view {

View::View(model::Model* model) : model_(model) {
}

View::~View() {
}

void View::InitGL() {
  assert(model_ != NULL);

  scene_.reset(new Scene(model_));
  scene_->Init();

  scene_view_.reset(new SceneView);
  scene_view_->Init(scene_->root());

  // Setup an ISO view so that everything in the scene is shown.
  const osg::BoundingSphere& bound = scene_->GetBound();
  const osg::Vec3f& target = bound.center();
  // Eye is along (1.0, -1.0, 0.5) axis.
  float eye_dist = 2.5f * bound.radius();
  osg::Vec3 eye = target + osg::Vec3(eye_dist, -eye_dist, 0.5f * eye_dist);
  // Up is along the z-axis.
  osg::Vec3 up(0.0f, 0.0f, 1.0f);
  scene_view_->SetLookAt(eye, target, up);
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
  scene_view_->SetProjectionMatrix(osg::Matrix::perspective(
      fovy, aspect_ratio, z_near, z_far));
}

void View::Draw() {
  scene_->Update();
  scene_view_->Draw();
}

double View::GetCameraOrientation() {
  return 42.0;
}

bool View::SetCameraOrientation(double orientation) {
  return true;
}

c_salt::ScriptingBridge* View::CreateScriptingBridgeWithInstance(
      const c_salt::Instance* instance) {
  if (!scripting_bridge_.get()) {
    // Note that CreateScriptingBridgeWithInstance() performs a synchronous
    // rendezvous with the browser. If it returns non-NULL, |scripting_bridge_|
    // is all hooked up to the browser and is ready to use.
    scripting_bridge_.reset(
        c_salt::ScriptingBridge::CreateScriptingBridgeWithInstance(instance));
    scripting_bridge_->AddMethodNamed("getCameraOrientation",
                                      this,
                                      &View::GetCameraOrientation);
    scripting_bridge_->AddMethodNamed("setCameraOrientation",
                                      this,
                                      &View::SetCameraOrientation);
  }
  return scripting_bridge_.get();
}

}  // namespace view
}  // namespace ginsu